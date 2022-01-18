"""
Pure Python, iperf3-compatible network performance test tool.

MIT license; Copyright (c) 2018-2019 Damien P. George

Supported modes: server & client, TCP & UDP, normal & reverse

Usage:
    import iperf3
    iperf3.server()
    iperf3.client('192.168.1.5')
    iperf3.client('192.168.1.5', udp=True, reverse=True)
"""

import sys, os, struct
import time, select, socket
import json

DEBUG = False

# iperf3 cookie size, last byte is null byte
COOKIE_SIZE = 37

# iperf3 commands
TEST_START = 1
TEST_RUNNING = 2
TEST_END = 4
PARAM_EXCHANGE = 9
CREATE_STREAMS = 10
EXCHANGE_RESULTS = 13
DISPLAY_RESULTS = 14
IPERF_DONE = 16

if DEBUG:
    cmd_string = {
        TEST_START: "TEST_START",
        TEST_RUNNING: "TEST_RUNNING",
        TEST_END: "TEST_END",
        PARAM_EXCHANGE: "PARAM_EXCHANGE",
        CREATE_STREAMS: "CREATE_STREAMS",
        EXCHANGE_RESULTS: "EXCHANGE_RESULTS",
        DISPLAY_RESULTS: "DISPLAY_RESULTS",
        IPERF_DONE: "IPERF_DONE",
    }


def fmt_size(val, div):
    for mult in ("", "K", "M", "G"):
        if val < 10:
            return "% 5.2f %s" % (val, mult)
        elif val < 100:
            return "% 5.1f %s" % (val, mult)
        elif mult == "G" or val < 1000:
            return "% 5.0f %s" % (val, mult)
        else:
            val /= div


class Stats:
    def __init__(self, param):
        self.pacing_timer_us = param["pacing_timer"] * 1000
        self.udp = param.get("udp", False)
        self.reverse = param.get("reverse", False)
        self.running = False

    def start(self):
        self.running = True
        self.t0 = self.t1 = ticks_us()
        self.nb0 = self.nb1 = 0  # num bytes
        self.np0 = self.np1 = 0  # num packets
        self.nm0 = self.nm1 = 0  # num lost packets
        if self.udp:
            if self.reverse:
                extra = "         Jitter    Lost/Total Datagrams"
            else:
                extra = "         Total Datagrams"
        else:
            extra = ""
        print("Interval           Transfer     Bitrate" + extra)

    def max_dt_ms(self):
        if not self.running:
            return -1
        return max(0, (self.pacing_timer_us - ticks_diff(ticks_us(), self.t1)) // 1000)

    def add_bytes(self, n):
        if not self.running:
            return
        self.nb0 += n
        self.nb1 += n
        self.np0 += 1
        self.np1 += 1

    def add_lost_packets(self, n):
        self.np0 += n
        self.np1 += n
        self.nm0 += n
        self.nm1 += n

    def print_line(self, ta, tb, nb, np, nm, extra=""):
        dt = tb - ta
        print(
            " %5.2f-%-5.2f  sec %sBytes %sbits/sec"
            % (ta, tb, fmt_size(nb, 1024), fmt_size(nb * 8 / dt, 1000)),
            end="",
        )
        if self.udp:
            if self.reverse:
                print(
                    " %6.3f ms  %u/%u (%.1f%%)" % (0, nm, np, 100 * nm / (max(1, np + nm))), end=""
                )
            else:
                print("  %u" % np, end="")
        print(extra)

    def update(self, final=False):
        if not self.running:
            return
        t2 = ticks_us()
        dt = ticks_diff(t2, self.t1)
        if final or dt > self.pacing_timer_us:
            ta = ticks_diff(self.t1, self.t0) * 1e-6
            tb = ticks_diff(t2, self.t0) * 1e-6
            self.print_line(ta, tb, self.nb1, self.np1, self.nm1)
            self.t1 = t2
            self.nb1 = 0
            self.np1 = 0
            self.nm1 = 0

    def stop(self):
        self.update(True)
        self.running = False
        self.t3 = ticks_us()
        dt = ticks_diff(self.t3, self.t0)
        print("- " * 30)
        self.print_line(0, dt * 1e-6, self.nb0, self.np0, self.nm0, "  sender")

    def report_receiver(self, stats):
        st = stats["streams"][0]
        dt = st["end_time"] - st["start_time"]
        self.print_line(
            st["start_time"],
            st["end_time"],
            st["bytes"],
            st["packets"],
            st["errors"],
            "  receiver",
        )
        return


def recvn(s, n):
    data = b""
    while len(data) < n:
        data += s.recv(n - len(data))
    return data


def recvinto(s, buf):
    if hasattr(s, "readinto"):
        return s.readinto(buf)
    else:
        return s.recv_into(buf)


def recvninto(s, buf):
    if hasattr(s, "readinto"):
        n = s.readinto(buf)
        assert n == len(buf)
    else:
        mv = memoryview(buf)
        off = 0
        while off < len(buf):
            off += s.recv_into(mv[off:])


def make_cookie():
    cookie_chars = b"abcdefghijklmnopqrstuvwxyz234567"
    cookie = bytearray(COOKIE_SIZE)
    for i, x in enumerate(os.urandom(COOKIE_SIZE - 1)):
        cookie[i] = cookie_chars[x & 31]
    return cookie


def server_once():
    # Listen for a connection
    ai = socket.getaddrinfo("0.0.0.0", 5201)
    ai = ai[0]
    print("Server listening on", ai[-1])
    s_listen = socket.socket(ai[0], socket.SOCK_STREAM)
    s_listen.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    s_listen.bind(ai[-1])
    s_listen.listen(1)
    s_ctrl, addr = s_listen.accept()

    # Read client's cookie
    cookie = recvn(s_ctrl, COOKIE_SIZE)
    if DEBUG:
        print(cookie)

    # Ask for parameters
    s_ctrl.sendall(bytes([PARAM_EXCHANGE]))

    # Get parameters
    n = struct.unpack(">I", recvn(s_ctrl, 4))[0]
    param = recvn(s_ctrl, n)
    param = json.loads(str(param, "ascii"))
    if DEBUG:
        print(param)
    reverse = param.get("reverse", False)

    # Ask to create streams
    s_ctrl.sendall(bytes([CREATE_STREAMS]))

    if param.get("tcp", False):
        # Accept stream
        s_data, addr = s_listen.accept()
        print("Accepted connection:", addr)
        recvn(s_data, COOKIE_SIZE)
    elif param.get("udp", False):
        # Close TCP connection and open UDP "connection"
        s_listen.close()
        s_data = socket.socket(ai[0], socket.SOCK_DGRAM)
        s_data.bind(ai[-1])
        data, addr = s_data.recvfrom(4)
        s_data.sendto(b"\x12\x34\x56\x78", addr)
    else:
        assert False

    # Start test
    s_ctrl.sendall(bytes([TEST_START]))

    # Run test
    s_ctrl.sendall(bytes([TEST_RUNNING]))

    # Read data, and wait for client to send TEST_END
    poll = select.poll()
    poll.register(s_ctrl, select.POLLIN)
    if reverse:
        poll.register(s_data, select.POLLOUT)
    else:
        poll.register(s_data, select.POLLIN)
    stats = Stats(param)
    stats.start()
    running = True
    data_buf = bytearray(os.urandom(param["len"]))
    while running:
        for pollable in poll.poll(stats.max_dt_ms()):
            if pollable_is_sock(pollable, s_ctrl):
                cmd = recvn(s_ctrl, 1)[0]
                if DEBUG:
                    print(cmd_string.get(cmd, "UNKNOWN_COMMAND"))
                if cmd == TEST_END:
                    running = False
            elif pollable_is_sock(pollable, s_data):
                if reverse:
                    n = s_data.send(data_buf)
                    stats.add_bytes(n)
                else:
                    recvninto(s_data, data_buf)
                    stats.add_bytes(len(data_buf))
        stats.update()

    # Need to continue writing so other side doesn't get blocked waiting for data
    if reverse:
        while True:
            for pollable in poll.poll(0):
                if pollable_is_sock(pollable, s_data):
                    s_data.send(data_buf)
                    break
            else:
                break

    stats.stop()

    # Ask to exchange results
    s_ctrl.sendall(bytes([EXCHANGE_RESULTS]))

    # Get client results
    n = struct.unpack(">I", recvn(s_ctrl, 4))[0]
    results = recvn(s_ctrl, n)
    results = json.loads(str(results, "ascii"))
    if DEBUG:
        print(results)

    # Send our results
    results = {
        "cpu_util_total": 1,
        "cpu_util_user": 0.5,
        "cpu_util_system": 0.5,
        "sender_has_retransmits": 1,
        "congestion_used": "cubic",
        "streams": [
            {
                "id": 1,
                "bytes": stats.nb0,
                "retransmits": 0,
                "jitter": 0,
                "errors": 0,
                "packets": stats.np0,
                "start_time": 0,
                "end_time": ticks_diff(stats.t3, stats.t0) * 1e-6,
            }
        ],
    }
    results = json.dumps(results)
    s_ctrl.sendall(struct.pack(">I", len(results)))
    s_ctrl.sendall(bytes(results, "ascii"))

    # Ask to display results
    s_ctrl.sendall(bytes([DISPLAY_RESULTS]))

    # Wait for client to send IPERF_DONE
    cmd = recvn(s_ctrl, 1)[0]
    assert cmd == IPERF_DONE

    # Close all sockets
    s_data.close()
    s_ctrl.close()
    s_listen.close()


def server():
    while True:
        server_once()


def client(host, udp=False, reverse=False, bandwidth=10 * 1024 * 1024):
    print("CLIENT MODE:", "UDP" if udp else "TCP", "receiving" if reverse else "sending")

    param = {
        "client_version": "3.6",
        "omit": 0,
        "parallel": 1,
        "pacing_timer": 1000,
        "time": 10,
    }

    if udp:
        param["udp"] = True
        param["len"] = 1500 - 42
        param["bandwidth"] = bandwidth  # this should be should be intended bits per second
        udp_interval = 1000000 * 8 * param["len"] // param["bandwidth"]
    else:
        param["tcp"] = True
        param["len"] = 3000

    if reverse:
        param["reverse"] = True

    # Connect to server
    ai = socket.getaddrinfo(host, 5201)[0]
    print("Connecting to", ai[-1])
    s_ctrl = socket.socket(ai[0], socket.SOCK_STREAM)
    s_ctrl.connect(ai[-1])

    # Send our cookie
    cookie = make_cookie()
    if DEBUG:
        print(cookie)
    s_ctrl.sendall(cookie)

    # Object to gather statistics about the run
    stats = Stats(param)

    # Run the main loop, waiting for incoming commands and dat
    ticks_us_end = param["time"] * 1000000
    poll = select.poll()
    poll.register(s_ctrl, select.POLLIN)
    s_data = None
    start = None
    udp_packet_id = 0
    while True:
        for pollable in poll.poll(stats.max_dt_ms()):
            if pollable_is_sock(pollable, s_data):
                # Data socket is writable/readable
                t = ticks_us()
                if ticks_diff(t, start) > ticks_us_end:
                    if reverse:
                        # Continue to drain any incoming data
                        recvinto(s_data, buf)
                    if stats.running:
                        # End of run
                        s_ctrl.sendall(bytes([TEST_END]))
                        stats.stop()
                else:
                    # Send/receiver data
                    if udp:
                        if reverse:
                            recvninto(s_data, buf)
                            udp_in_sec, udp_in_usec, udp_in_id = struct.unpack_from(">III", buf, 0)
                            # print(udp_in_sec, udp_in_usec, udp_in_id)
                            if udp_in_id != udp_packet_id + 1:
                                stats.add_lost_packets(udp_in_id - (udp_packet_id + 1))
                            udp_packet_id = udp_in_id
                            stats.add_bytes(len(buf))
                        else:
                            # print('UDP send', udp_last_send, t, udp_interval)
                            if t - udp_last_send > udp_interval:
                                udp_last_send += udp_interval
                                udp_packet_id += 1
                                struct.pack_into(
                                    ">III", buf, 0, t // 1000000, t % 1000000, udp_packet_id
                                )
                                n = s_data.sendto(buf, ai[-1])
                                stats.add_bytes(n)
                    else:
                        if reverse:
                            recvninto(s_data, buf)
                            n = len(buf)
                        else:
                            # print('TCP send', len(buf))
                            n = s_data.send(buf)
                        stats.add_bytes(n)

            elif pollable_is_sock(pollable, s_ctrl):
                # Receive command
                cmd = recvn(s_ctrl, 1)[0]
                if DEBUG:
                    print(cmd_string.get(cmd, "UNKNOWN_COMMAND"))
                if cmd == TEST_START:
                    if reverse:
                        # Start receiving data now, because data socket is open
                        poll.register(s_data, select.POLLIN)
                        start = ticks_us()
                        stats.start()
                elif cmd == TEST_RUNNING:
                    if not reverse:
                        # Start sending data now
                        poll.register(s_data, select.POLLOUT)
                        start = ticks_us()
                        if udp:
                            udp_last_send = start - udp_interval
                        stats.start()
                elif cmd == PARAM_EXCHANGE:
                    param_j = json.dumps(param)
                    s_ctrl.sendall(struct.pack(">I", len(param_j)))
                    s_ctrl.sendall(bytes(param_j, "ascii"))
                elif cmd == CREATE_STREAMS:
                    if udp:
                        s_data = socket.socket(ai[0], socket.SOCK_DGRAM)
                        s_data.sendto(struct.pack("<I", 123456789), ai[-1])
                        recvn(s_data, 4)  # get dummy response from server (=987654321)
                    else:
                        s_data = socket.socket(ai[0], socket.SOCK_STREAM)
                        s_data.connect(ai[-1])
                        s_data.sendall(cookie)
                    buf = bytearray(os.urandom(param["len"]))
                elif cmd == EXCHANGE_RESULTS:
                    # Close data socket now that server knows we are finished, to prevent it flooding us
                    poll.unregister(s_data)
                    s_data.close()
                    s_data = None

                    results = {
                        "cpu_util_total": 1,
                        "cpu_util_user": 0.5,
                        "cpu_util_system": 0.5,
                        "sender_has_retransmits": 1,
                        "congestion_used": "cubic",
                        "streams": [
                            {
                                "id": 1,
                                "bytes": stats.nb0,
                                "retransmits": 0,
                                "jitter": 0,
                                "errors": stats.nm0,
                                "packets": stats.np0,
                                "start_time": 0,
                                "end_time": ticks_diff(stats.t3, stats.t0) * 1e-6,
                            }
                        ],
                    }
                    results = json.dumps(results)
                    s_ctrl.sendall(struct.pack(">I", len(results)))
                    s_ctrl.sendall(bytes(results, "ascii"))

                    n = struct.unpack(">I", recvn(s_ctrl, 4))[0]
                    results = recvn(s_ctrl, n)
                    results = json.loads(str(results, "ascii"))
                    stats.report_receiver(results)

                elif cmd == DISPLAY_RESULTS:
                    s_ctrl.sendall(bytes([IPERF_DONE]))
                    s_ctrl.close()
                    time.sleep(1)  # delay so server is ready for any subsequent client connections
                    return

        stats.update()


def main():
    opt_mode = None
    opt_udp = False
    opt_reverse = False

    sys.argv.pop(0)
    while sys.argv:
        opt = sys.argv.pop(0)
        if opt == "-R":
            opt_reverse = True
        elif opt == "-u":
            opt_udp = True
        elif opt == "-s":
            opt_mode = opt
        elif opt == "-c":
            opt_mode = opt
            opt_host = sys.argv.pop(0)
        else:
            print("unknown option:", opt)
            raise SystemExit(1)

    if opt_mode == "-s":
        server()
    else:
        client(opt_host, opt_udp, opt_reverse)


if sys.platform == "linux":

    def pollable_is_sock(pollable, sock):
        return sock is not None and pollable[0] == sock.fileno()

    def ticks_us():
        return int(time.time() * 1e6)

    def ticks_diff(a, b):
        return a - b

    if __name__ == "__main__":
        main()
else:

    def pollable_is_sock(pollable, sock):
        return pollable[0] == sock

    from time import ticks_us, ticks_diff
