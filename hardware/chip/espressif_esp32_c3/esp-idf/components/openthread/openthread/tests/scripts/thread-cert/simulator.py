#!/usr/bin/env python3
#
#  Copyright (c) 2018, The OpenThread Authors.
#  All rights reserved.
#
#  Redistribution and use in source and binary forms, with or without
#  modification, are permitted provided that the following conditions are met:
#  1. Redistributions of source code must retain the above copyright
#     notice, this list of conditions and the following disclaimer.
#  2. Redistributions in binary form must reproduce the above copyright
#     notice, this list of conditions and the following disclaimer in the
#     documentation and/or other materials provided with the distribution.
#  3. Neither the name of the copyright holder nor the
#     names of its contributors may be used to endorse or promote products
#     derived from this software without specific prior written permission.
#
#  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
#  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
#  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
#  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
#  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
#  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
#  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
#  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
#  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
#  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
#  POSSIBILITY OF SUCH DAMAGE.
#

import binascii
import bisect
import os
import socket
import struct
import traceback
import time

import io
import config
import mesh_cop
import message
import pcap
import wpan


def dbg_print(*args):
    if False:
        print(args)


class BaseSimulator(object):

    def __init__(self):
        self._nodes = {}
        self.commissioning_messages = {}
        self._payload_parse_factory = mesh_cop.MeshCopCommandFactory(mesh_cop.create_default_mesh_cop_tlv_factories())
        self._mesh_cop_msg_set = mesh_cop.create_mesh_cop_message_type_set()

    def __del__(self):
        self._nodes = None

    def add_node(self, node):
        self._nodes[node.nodeid] = node
        self.commissioning_messages[node.nodeid] = []

    def set_lowpan_context(self, cid, prefix):
        raise NotImplementedError

    def get_messages_sent_by(self, nodeid):
        raise NotImplementedError

    def go(self, duration, nodeid=None):
        raise NotImplementedError

    def stop(self):
        raise NotImplementedError

    def read_cert_messages_in_commissioning_log(self, nodeids):
        for nodeid in nodeids:
            node = self._nodes[nodeid]
            if not node:
                continue
            for (
                    direction,
                    type,
                    payload,
            ) in node.read_cert_messages_in_commissioning_log():
                msg = self._payload_parse_factory.parse(type.decode("utf-8"), io.BytesIO(payload))
                self.commissioning_messages[nodeid].append(msg)


class RealTime(BaseSimulator):

    def __init__(self, use_message_factory=True):
        super(RealTime, self).__init__()
        self._sniffer = config.create_default_thread_sniffer(use_message_factory=use_message_factory)
        self._sniffer.start()

    def set_lowpan_context(self, cid, prefix):
        self._sniffer.set_lowpan_context(cid, prefix)

    def get_messages_sent_by(self, nodeid):
        messages = self._sniffer.get_messages_sent_by(nodeid).messages
        ret = message.MessagesSet(messages, self.commissioning_messages[nodeid])
        self.commissioning_messages[nodeid] = []
        return ret

    def now(self):
        return time.time()

    def go(self, duration, nodeid=None):
        time.sleep(duration)

    def stop(self):
        if self.is_running:
            # self._sniffer.stop()  # FIXME: seems it blocks forever
            self._sniffer = None

    @property
    def is_running(self):
        return self._sniffer is not None


class VirtualTime(BaseSimulator):

    OT_SIM_EVENT_ALARM_FIRED = 0
    OT_SIM_EVENT_RADIO_RECEIVED = 1
    OT_SIM_EVENT_UART_WRITE = 2
    OT_SIM_EVENT_RADIO_SPINEL_WRITE = 3
    OT_SIM_EVENT_POSTCMD = 4

    EVENT_TIME = 0
    EVENT_SEQUENCE = 1
    EVENT_ADDR = 2
    EVENT_TYPE = 3
    EVENT_DATA_LENGTH = 4
    EVENT_DATA = 5

    BASE_PORT = 9000
    MAX_NODES = 33
    MAX_MESSAGE = 1024
    END_OF_TIME = float('inf')
    PORT_OFFSET = int(os.getenv('PORT_OFFSET', '0'))

    BLOCK_TIMEOUT = 10

    NCP_SIM = os.getenv('NODE_TYPE', 'sim') == 'ncp-sim'

    _message_factory = None

    def __init__(self, use_message_factory=True):
        super(VirtualTime, self).__init__()
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.sock.setsockopt(socket.SOL_SOCKET, socket.SO_RCVBUF, 2 * 1024 * 1024)
        self.sock.setsockopt(socket.SOL_SOCKET, socket.SO_SNDBUF, 2 * 1024 * 1024)

        ip = '127.0.0.1'
        self.port = self.BASE_PORT + (self.PORT_OFFSET * (self.MAX_NODES + 1))
        self.sock.bind((ip, self.port))

        self.devices = {}
        self.event_queue = []
        # there could be events scheduled at exactly the same time
        self.event_sequence = 0
        self.current_time = 0
        self.current_event = None
        self.awake_devices = set()
        self._nodes_by_ack_seq = {}
        self._node_ack_seq = {}

        self._pcap = pcap.PcapCodec(os.getenv('TEST_NAME', 'current'))
        # the addr for spinel-cli sending OT_SIM_EVENT_POSTCMD
        self._spinel_cli_addr = (ip, self.BASE_PORT + self.port)
        self.current_nodeid = None
        self._pause_time = 0

        if use_message_factory:
            self._message_factory = config.create_default_thread_message_factory()
        else:
            self._message_factory = None

    def __del__(self):
        if self.sock:
            self.stop()

    def stop(self):
        if self.sock:
            self.sock.close()
            self.sock = None

    @property
    def is_running(self):
        return self.sock is not None

    def _add_message(self, nodeid, message_obj):
        addr = ('127.0.0.1', self.port + nodeid)

        # Ignore any exceptions
        try:
            if self._message_factory is not None:
                messages = self._message_factory.create(io.BytesIO(message_obj))
                self.devices[addr]['msgs'] += messages

        except message.DropPacketException:
            print('Drop current packet because it cannot be handled in test scripts')
        except Exception as e:
            # Just print the exception to the console
            print("EXCEPTION: %s" % e)
            traceback.print_exc()

    def set_lowpan_context(self, cid, prefix):
        if self._message_factory is not None:
            self._message_factory.set_lowpan_context(cid, prefix)

    def get_messages_sent_by(self, nodeid):
        """ Get sniffed messages.

        Note! This method flushes the message queue so calling this
        method again will return only the newly logged messages.

        Args:
            nodeid (int): node id

        Returns:
            MessagesSet: a set with received messages.
        """
        addr = ('127.0.0.1', self.port + nodeid)

        messages = self.devices[addr]['msgs']
        self.devices[addr]['msgs'] = []

        ret = message.MessagesSet(messages, self.commissioning_messages[nodeid])
        self.commissioning_messages[nodeid] = []
        return ret

    def _is_radio(self, addr):
        return addr[1] < self.BASE_PORT * 2

    def _to_core_addr(self, addr):
        assert self._is_radio(addr)
        return (addr[0], addr[1] + self.BASE_PORT)

    def _to_radio_addr(self, addr):
        assert not self._is_radio(addr)
        return (addr[0], addr[1] - self.BASE_PORT)

    def _core_addr_from(self, nodeid):
        if self._nodes[nodeid].is_posix:
            return ('127.0.0.1', self.BASE_PORT + self.port + nodeid)
        else:
            return ('127.0.0.1', self.port + nodeid)

    def _next_event_time(self):
        if len(self.event_queue) == 0:
            return self.END_OF_TIME
        else:
            return self.event_queue[0][0]

    def receive_events(self):
        """ Receive events until all devices are asleep. """
        while True:
            if (self.current_event or len(self.awake_devices) or
                (self._next_event_time() > self._pause_time and self.current_nodeid)):
                self.sock.settimeout(self.BLOCK_TIMEOUT)
                try:
                    msg, addr = self.sock.recvfrom(self.MAX_MESSAGE)
                except socket.error:
                    # print debug information on failure
                    print('Current nodeid:')
                    print(self.current_nodeid)
                    print('Current awake:')
                    print(self.awake_devices)
                    print('Current time:')
                    print(self.current_time)
                    print('Current event:')
                    print(self.current_event)
                    print('Events:')
                    for event in self.event_queue:
                        print(event)
                    raise
            else:
                self.sock.settimeout(0)
                try:
                    msg, addr = self.sock.recvfrom(self.MAX_MESSAGE)
                except socket.error:
                    break

            if addr != self._spinel_cli_addr and addr not in self.devices:
                self.devices[addr] = {}
                self.devices[addr]['alarm'] = None
                self.devices[addr]['msgs'] = []
                self.devices[addr]['time'] = self.current_time
                self.awake_devices.discard(addr)
                # print "New device:", addr, self.devices

            delay, type, datalen = struct.unpack('=QBH', msg[:11])
            data = msg[11:]

            event_time = self.current_time + delay

            if data:
                dbg_print(
                    "New event: ",
                    event_time,
                    addr,
                    type,
                    datalen,
                    binascii.hexlify(data),
                )
            else:
                dbg_print("New event: ", event_time, addr, type, datalen)

            if type == self.OT_SIM_EVENT_ALARM_FIRED:
                # remove any existing alarm event for device
                if self.devices[addr]['alarm']:
                    self.event_queue.remove(self.devices[addr]['alarm'])
                    # print "-- Remove\t", self.devices[addr]['alarm']

                # add alarm event to event queue
                event = (event_time, self.event_sequence, addr, type, datalen)
                self.event_sequence += 1
                # print "-- Enqueue\t", event, delay, self.current_time
                bisect.insort(self.event_queue, event)
                self.devices[addr]['alarm'] = event

                self.awake_devices.discard(addr)

                if (self.current_event and self.current_event[self.EVENT_ADDR] == addr):
                    # print "Done\t", self.current_event
                    self.current_event = None

            elif type == self.OT_SIM_EVENT_RADIO_RECEIVED:
                assert self._is_radio(addr)
                # add radio receive events event queue
                frame_info = wpan.dissect(data)

                recv_devices = None
                if frame_info.frame_type == wpan.FrameType.ACK:
                    recv_devices = self._nodes_by_ack_seq.get(frame_info.seq_no)

                recv_devices = recv_devices or self.devices.keys()

                for device in recv_devices:
                    if device != addr and self._is_radio(device):
                        event = (
                            event_time,
                            self.event_sequence,
                            device,
                            type,
                            datalen,
                            data,
                        )
                        self.event_sequence += 1
                        # print "-- Enqueue\t", event
                        bisect.insort(self.event_queue, event)

                self._pcap.append(data, (event_time // 1000000, event_time % 1000000))
                self._add_message(addr[1] - self.port, data)

                # add radio transmit done events to event queue
                event = (
                    event_time,
                    self.event_sequence,
                    addr,
                    type,
                    datalen,
                    data,
                )
                self.event_sequence += 1
                bisect.insort(self.event_queue, event)

                if frame_info.frame_type != wpan.FrameType.ACK and not frame_info.is_broadcast:
                    self._on_ack_seq_change(addr, frame_info.seq_no)

                self.awake_devices.add(addr)

            elif type == self.OT_SIM_EVENT_RADIO_SPINEL_WRITE:
                assert not self._is_radio(addr)
                radio_addr = self._to_radio_addr(addr)
                if radio_addr not in self.devices:
                    self.awake_devices.add(radio_addr)

                event = (
                    event_time,
                    self.event_sequence,
                    radio_addr,
                    self.OT_SIM_EVENT_UART_WRITE,
                    datalen,
                    data,
                )
                self.event_sequence += 1
                bisect.insort(self.event_queue, event)

                self.awake_devices.add(addr)

            elif type == self.OT_SIM_EVENT_UART_WRITE:
                assert self._is_radio(addr)
                core_addr = self._to_core_addr(addr)
                if core_addr not in self.devices:
                    self.awake_devices.add(core_addr)

                event = (
                    event_time,
                    self.event_sequence,
                    core_addr,
                    self.OT_SIM_EVENT_RADIO_SPINEL_WRITE,
                    datalen,
                    data,
                )
                self.event_sequence += 1
                bisect.insort(self.event_queue, event)

                self.awake_devices.add(addr)

            elif type == self.OT_SIM_EVENT_POSTCMD:
                assert self.current_time == self._pause_time
                nodeid = struct.unpack('=B', data)[0]
                if self.current_nodeid == nodeid:
                    self.current_nodeid = None

    def _on_ack_seq_change(self, device: tuple, seq_no: int):
        old_seq = self._node_ack_seq.pop(device, None)
        if old_seq is not None:
            self._nodes_by_ack_seq[old_seq].remove(device)

        self._node_ack_seq[device] = seq_no
        self._nodes_by_ack_seq.setdefault(seq_no, set()).add(device)

    def _send_message(self, message, addr):
        while True:
            try:
                sent = self.sock.sendto(message, addr)
            except socket.error:
                traceback.print_exc()
                time.sleep(0)
            else:
                break
        assert sent == len(message)

    def process_next_event(self):
        assert self.current_event is None
        assert self._next_event_time() < self.END_OF_TIME

        # process next event
        event = self.event_queue.pop(0)

        if len(event) == 5:
            event_time, sequence, addr, type, datalen = event
            dbg_print("Pop event: ", event_time, addr, type, datalen)
        else:
            event_time, sequence, addr, type, datalen, data = event
            dbg_print(
                "Pop event: ",
                event_time,
                addr,
                type,
                datalen,
                binascii.hexlify(data),
            )

        self.current_event = event

        assert event_time >= self.current_time
        self.current_time = event_time

        elapsed = event_time - self.devices[addr]['time']
        self.devices[addr]['time'] = event_time

        message = struct.pack('=QBH', elapsed, type, datalen)

        if type == self.OT_SIM_EVENT_ALARM_FIRED:
            self.devices[addr]['alarm'] = None
            self._send_message(message, addr)
        elif type == self.OT_SIM_EVENT_RADIO_RECEIVED:
            message += data
            self._send_message(message, addr)
        elif type == self.OT_SIM_EVENT_RADIO_SPINEL_WRITE:
            message += data
            self._send_message(message, addr)
        elif type == self.OT_SIM_EVENT_UART_WRITE:
            message += data
            self._send_message(message, addr)

    def sync_devices(self):
        self.current_time = self._pause_time
        for addr in self.devices:
            elapsed = self.current_time - self.devices[addr]['time']
            if elapsed == 0:
                continue
            dbg_print('syncing', addr, elapsed)
            self.devices[addr]['time'] = self.current_time
            message = struct.pack('=QBH', elapsed, self.OT_SIM_EVENT_ALARM_FIRED, 0)
            self._send_message(message, addr)
            self.awake_devices.add(addr)
            self.receive_events()
        self.awake_devices.clear()

    def now(self):
        return self.current_time / 1000000

    def go(self, duration, nodeid=None):
        assert self.current_time == self._pause_time
        duration = int(duration * 1000000)
        dbg_print('running for %d us' % duration)
        self._pause_time += duration
        if nodeid:
            if self.NCP_SIM:
                self.current_nodeid = nodeid
            self.awake_devices.add(self._core_addr_from(nodeid))
        self.receive_events()
        while self._next_event_time() <= self._pause_time:
            self.process_next_event()
            self.receive_events()
        if duration > 0:
            self.sync_devices()
        dbg_print('current time %d us' % self.current_time)


if __name__ == '__main__':
    simulator = VirtualTime()
    while True:
        simulator.go(0)
