#!/usr/bin/python

import os, sys, time, socket, ssl, traceback
import subprocess, thread, threading, random, re
from operator import itemgetter
import TBframe

MAX_MSG_LENTH = 8192
ENCRYPT = False
DEBUG = True

class ConnectionLost(Exception):
    pass

class Autotest:
    def __init__(self):
        self.keep_running = True
        self.device_list= {}
        self.service_socket = 0
        self.connected = False
        self.cmd_excute_state = 'idle'
        self.cmd_excute_return = ''
        self.cmd_excute_event = threading.Event()
        self.subscribed = {}
        self.subscribed_reverse = {}
        self.filter = {}
        self.filter_lock = threading.RLock()
        self.filter_event = threading.Event()
        self.filter_event.set()
        self.esc_seq = re.compile(r'\x1b[^m]*m')

    def send_data(self, data):
        try:
            self.service_socket.send(data)
        except:
            self.connected = False

    def heartbeat_func(self):
        heartbeat_timeout = time.time() + 10
        while self.keep_running:
            time.sleep(0.05)
            if time.time() < heartbeat_timeout:
                continue
            self.send_data(TBframe.construct(TBframe.HEARTBEAT, ''))
            heartbeat_timeout += 10

    def get_devname_by_devstr(self, devstr):
        if devstr in list(self.subscribed_reverse):
            return self.subscribed_reverse[devstr]
        return ""

    def response_filter(self, devname, logstr):
        if self.filter_event.is_set() == True:
            return

        if self.filter_lock.acquire(False) == False:
            return

        try:
            if len(self.filter) == 0:
                self.filter_lock.release()
                return

            if self.filter['devname'] != devname:
                self.filter_lock.release()
                return

            if self.filter['lines_exp'] == 0:
                if self.filter['cmdstr'] in logstr:
                    self.filter['lines_num'] += 1
                    self.filter_event.set()
            else:
                if self.filter['lines_num'] == 0:
                    if self.filter['cmdstr'] in logstr:
                        self.filter['lines_num'] += 1
                elif self.filter['lines_num'] <= self.filter['lines_exp']:
                    log = self.esc_seq.sub('', logstr)
                    log = log.replace("\r", "")
                    log = log.replace("\n", "")
                    if log != "":
                        for filterstr in self.filter['filters']:
                            if filterstr not in log:
                                continue
                            else:
                                self.filter['response'].append(log)
                                self.filter['lines_num'] += 1
                                break
                    if self.filter['lines_num'] > self.filter['lines_exp']:
                        self.filter_event.set()
            self.filter_lock.release()
        except:
            if DEBUG: traceback.print_exc()
            self.filter_lock.release()

    def server_interaction(self):
        msg = ''
        while self.keep_running:
            try:
                new_msg = self.service_socket.recv(MAX_MSG_LENTH)
                if new_msg == '':
                    raise ConnectionLost
                    break

                msg += new_msg
                while msg != '':
                    type, length, value, msg = TBframe.parse(msg)
                    if type == TBframe.TYPE_NONE:
                        break

                    #print type, length
                    if type == TBframe.ALL_DEV:
                        new_list = {}
                        clients = value.split(':')
                        for c in clients:
                            if c == '':
                                continue
                            devs = c.split(',')
                            uuid = devs[0]
                            devs = devs[1:]
                            for d in devs:
                                if d == '':
                                    continue
                                [dev, using] = d.split('|')
                                new_list[uuid + ',' + dev] = using

                        for dev in list(new_list):
                            self.device_list[dev] = new_list[dev]
                    if type == TBframe.DEVICE_LOG:
                        dev = value.split(':')[0]
                        logtime = value.split(':')[1]
                        log = value[len(dev) + 1 + len(logtime) + 1:]
                        try:
                            logtime = float(logtime)
                            logtimestr = time.strftime("%Y-%m-%d %H-%M-%S", time.localtime(logtime));
                            logtimestr += ("{0:.3f}".format(logtime-int(logtime)))[1:]
                        except:
                            continue
                        if dev not in list(self.device_list):
                            continue
                        devname = self.get_devname_by_devstr(dev)
                        if devname != "":
                            self.response_filter(devname, log)
                            if self.logfile != None:
                                log =  devname + ":" + logtimestr + ":" + log
                                self.logfile.write(log)
                    if type == TBframe.CMD_DONE:
                        self.cmd_excute_return = value
                        self.cmd_excute_state = 'done'
                        self.cmd_excute_event.set()
                    if type == TBframe.CMD_ERROR:
                        self.cmd_excute_return = value
                        self.cmd_excute_state = 'error'
                        self.cmd_excute_event.set()
                    if type == TBframe.DEVICE_ALLOC:
                        values = value.split(',')
                        if len(values) != 2:
                            continue
                        result = values[0]
                        allocated = values[1].split('|')
                        if result != 'success':
                            self.cmd_excute_return = []
                            continue
                        self.cmd_excute_return = allocated
                        self.cmd_excute_state = 'done'
                        self.cmd_excute_event.set()
            except ConnectionLost:
                self.connected = False
                self.service_socket.close()
                print 'connection to server lost, try reconnecting...'
                while True:
                    result = self.connect_to_server(self.server_ip, self.server_port)
                    if result == 'success':
                        break
                    time.sleep(1)
                print 'connection to server resumed'
                random.seed()
                time.sleep(1.2 + random.random())
                for devname in self.subscribed:
                    data = TBframe.construct(TBframe.LOG_SUB, self.subscribed[devname])
                    self.send_data(data)
                    data = TBframe.construct(TBframe.DEVICE_CMD, self.subscribed[devname] + ':help')
                    self.send_data(data)
            except:
                if DEBUG:
                    traceback.print_exc()
                    raise
                break
        self.keep_running = False;

    def wait_cmd_excute_done(self, timeout):
        self.cmd_excute_state = 'wait_response'
        self.cmd_excute_return = None
        self.cmd_excute_event.clear()
        if self.cmd_excute_event.wait(timeout) == False:
            self.cmd_excute_state = "timeout"

    def get_devstr_by_partialstr(self, partialstr):
        devices = list(self.device_list)
        devices.sort()
        for devstr in devices:
            if partialstr in devstr:
                return devstr
        return ""

    def send_file_to_client(self, devname, filename):
        #argument check
        if devname not in self.subscribed:
            print "{0} is not subscribed".format(devname)
            return False
        try:
            expandfilename = os.path.expanduser(filename)
        except:
            print "{0} does not exist".format(filename)
            return False
        if os.path.exists(expandfilename) == False:
            print "{0} does not exist".format(filename)
            return False
        filename = expandfilename

        filehash = TBframe.hash_of_file(filename)
        devstr = self.subscribed[devname]

        #send file begin
        content = devstr  + ':' + filehash + ':' + filename.split('/')[-1]
        data = TBframe.construct(TBframe.FILE_BEGIN, content)
        retry = 4
        while retry > 0:
            self.send_data(data)
            self.wait_cmd_excute_done(0.2)
            if self.cmd_excute_state == 'timeout':
                retry -= 1;
                continue
            if self.cmd_excute_return == 'busy':
                print "file transfer busy: wait..."
                time.sleep(5)
                continue
            elif self.cmd_excute_return == 'ok' or self.cmd_excute_return == 'exist':
                break
            else:
                print "file transfer error: unexpected response '{0}'".format(self.cmd_excute_return)
                return False
        if retry == 0:
            print 'file transfer error: retry timeout'
            return False
        if self.cmd_excute_return == 'exist':
            return True

        #send file data
        seq = 0
        file = open(filename,'r')
        header = devstr  + ':' + filehash + ':' + str(seq) + ':'
        content = file.read(1024)
        while(content):
            data = TBframe.construct(TBframe.FILE_DATA, header + content)
            retry = 4
            while retry > 0:
                self.send_data(data)
                self.wait_cmd_excute_done(0.2)
                if self.cmd_excute_return == None:
                    retry -= 1;
                    continue
                elif self.cmd_excute_return != 'ok':
                    file.close()
                    return False
                break

            if retry == 0:
                file.close()
                return False

            seq += 1
            header = devstr  + ':' + filehash + ':' + str(seq) + ':'
            content = file.read(1024)
        file.close()

        #send file end
        content = devstr  + ':' + filehash + ':' + filename.split('/')[-1]
        data = TBframe.construct(TBframe.FILE_END, content)
        retry = 4
        while retry > 0:
            self.send_data(data)
            self.wait_cmd_excute_done(0.2)
            if self.cmd_excute_return == None:
                retry -= 1;
                continue
            elif self.cmd_excute_return != 'ok':
                return False
            break
        if retry == 0:
            return False
        return True

    def device_allocate(self, type, number, timeout, purpose='general'):
        if self.connected == False:
            return []
        content = ','.join([type, str(number), purpose])
        data = TBframe.construct(TBframe.DEVICE_ALLOC, content)
        timeout += time.time()
        while time.time() < timeout:
            self.send_data(data)
            self.wait_cmd_excute_done(0.8)
            if self.cmd_excute_return == None or self.cmd_excute_return == []:
                time.sleep(8)
                continue
            if len(self.cmd_excute_return) != number:
                print "error: allocated number does not equal requested"
            break
        if time.time() > timeout:
            self.cmd_excute_return = []
        return self.cmd_excute_return

    def device_subscribe(self, devices):
        if self.connected == False:
            return False
        for devname in list(devices):
            devstr = self.get_devstr_by_partialstr(devices[devname])
            if devstr == "":
                self.subscribed = {}
                self.subscribed_reverse = {}
                return False
            else:
                self.subscribed[devname] = devstr;
                self.subscribed_reverse[devstr] = devname
        for devname in devices:
            data = TBframe.construct(TBframe.LOG_SUB, self.subscribed[devname])
            self.send_data(data)
        return True

    def device_unsubscribe(self, devices):
        for devname in list(devices):
            if devname not in self.subscribed:
                continue
            data = TBframe.construct(TBframe.LOG_UNSUB, self.subscribed[devname])
            self.send_data(data)
            self.subscribed_reverse.pop(self.subscribed[devname])
            self.subscribed.pop(devname)

    def device_erase(self, devname):
        if self.connected == False:
            return False
        if devname not in self.subscribed:
            print "error: device {0} not subscribed".format(devname)
            return False
        content = self.subscribed[devname]
        data = TBframe.construct(TBframe.DEVICE_ERASE, content);
        self.send_data(data)
        self.wait_cmd_excute_done(10)
        if self.cmd_excute_state == "done":
            ret = True
        else:
            ret = False
        self.cmd_excute_state = 'idle'
        return ret

    def device_program(self, devname, address, filename):
        if self.connected == False:
            return False
        if devname not in self.subscribed:
            print "error: device {0} not subscribed".format(devname)
            return False
        if address.startswith('0x') == False:
            print "error: wrong address input {0}, address should start with 0x".format(address)
            return False
        try:
            expandname = os.path.expanduser(filename)
        except:
            print "{0} does not exist".format(filename)
            return False
        if os.path.exists(expandname) == False:
            print "{0} does not exist".format(filename)
            return False
        if self.send_file_to_client(devname, expandname) == False:
            return False

        filehash = TBframe.hash_of_file(expandname)
        content = self.subscribed[devname] + ',' + address + ',' + filehash
        data = TBframe.construct(TBframe.DEVICE_PROGRAM, content);
        self.send_data(data)
        self.wait_cmd_excute_done(270)
        if self.cmd_excute_state == "done":
            ret = True
        else:
            ret = False
        self.cmd_excute_state = 'idle'
        return ret

    def device_control(self, devname, operation):
        operations = {"start":TBframe.DEVICE_START, "stop":TBframe.DEVICE_STOP, "reset":TBframe.DEVICE_RESET}

        if self.connected == False:
            return False
        if devname not in self.subscribed:
            return False
        if operation not in list(operations):
            return False

        content = self.subscribed[devname]
        data = TBframe.construct(operations[operation], content)
        self.send_data(data)
        return True

    def device_run_cmd(self, devname, args, expect_lines = 0, timeout=0.8, filters=[""]):
        if self.connected == False:
            return False
        if devname not in self.subscribed:
            return False
        if len(args) == 0:
            return False
        content = self.subscribed[devname]
        content += ':' + '|'.join(args)
        data = TBframe.construct(TBframe.DEVICE_CMD, content)
        with self.filter_lock:
            self.filter['devname'] = devname
            self.filter['cmdstr'] = ' '.join(args)
            self.filter['lines_exp'] = expect_lines
            self.filter['lines_num'] = 0
            self.filter['filters'] = filters
            self.filter['response'] = []

        retry = 3
        while retry > 0:
            self.send_data(data)
            self.filter_event.clear()
            self.filter_event.wait(timeout)
            if self.filter['lines_num'] > 0:
                break;
            retry -= 1
        response = self.filter['response']
        with self.filter_lock:
            self.filter = {}
        return response

    def get_device_list(self):
        return list(self.device_list)

    def connect_to_server(self, server_ip, server_port):
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        if ENCRYPT:
            sock = ssl.wrap_socket(sock, cert_reqs=ssl.CERT_REQUIRED, ca_certs='server_cert.pem')
        try:
            sock.connect((server_ip, server_port))
            self.service_socket = sock
            self.connected = True
            return "success"
        except:
            return "fail"

    def start(self, server_ip, server_port, logname=None):
        self.server_ip = server_ip
        self.server_port = server_port
        result = self.connect_to_server(server_ip, server_port)
        if result != 'success':
            print "connect to server {0}:{1} failed".format(server_ip, server_port)
            return False

        self.connected = True
        self.logfile = None
        if logname != None:
            if os.path.exists('testlog') == False:
                subprocess.call(['mkdir','testlog'])

            try:
                self.logfile = open("testlog/"+logname, 'w');
            except:
                print "open logfile {0} failed".format(logfile)
                return False

        thread.start_new_thread(self.server_interaction, ())
        thread.start_new_thread(self.heartbeat_func, ())
        time.sleep(0.5)
        return True

    def stop(self):
        self.keep_running = False
        time.sleep(0.2)
        if self.connected:
            self.service_socket.close()
