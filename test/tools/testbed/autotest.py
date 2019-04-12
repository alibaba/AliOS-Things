#!/usr/bin/python

import os, sys, time, socket, ssl, traceback, Queue
import subprocess, thread, threading, random, re, errno
from operator import itemgetter
from os import path
import TBframe as pkt
import datetime

MAX_MSG_LENGTH = 8192
ENCRYPT = True
DEBUG = True

class ConnectionLost(Exception):
    pass

class Autotest:
    def __init__(self):
        self.keep_running = True
        self.device_list= {}
        self.service_socket = 0
        self.connected = False
        self.output_queue = Queue.Queue(256)
        self.response_queue = Queue.Queue(256)
        self.subscribed = {}
        self.subscribed_reverse = {}
        self.filter = {}
        self.filter_lock = threading.RLock()
        self.filter_event = threading.Event()
        self.filter_event.set()
        self.esc_seq = re.compile(r'\x1b[^m]*m')
        self.poll_str = '\x1b[t'
        bytes = os.urandom(4)
        for byte in bytes:
            self.poll_str += '{0:02x}'.format(ord(byte))
        self.poll_str += 'm'

    def packet_send_thread(self):
        heartbeat_timeout = time.time() + 10
        while self.keep_running:
            try:
                [type, content] = self.output_queue.get(block=True, timeout=0.1)
            except Queue.Empty:
                type = None
                pass
            if self.service_socket == None:
                continue
            if type == None:
                if time.time() < heartbeat_timeout:
                    continue
                heartbeat_timeout += 10
                data = pkt.construct(pkt.HEARTBEAT,'')
            else:
                data = pkt.construct(type, content)
            try:
                self.service_socket.send(data)
            except:
                self.connected = False
                continue

    def send_packet(self, type, content, timeout=0.1):
        if self.service_socket == None:
            return False
        try:
            self.output_queue.put([type, content], True, timeout)
            return True
        except Queue.Full:
            print 'error: output buffer full, drop packet [{0} {1}]'.format(type, content)
        return False

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

            if self.filter['cmdstr'] != self.poll_str and self.poll_str not in logstr:
                self.filter_lock.release()
                return

            logstr = logstr.replace(self.poll_str, '')

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

    def connect_to_server(self, server_ip, server_port):
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        if ENCRYPT:
            certfile = path.join(path.dirname(path.abspath(__file__)), 'certificate.pem')
            sock = ssl.wrap_socket(sock, cert_reqs=ssl.CERT_REQUIRED, ca_certs=certfile)
        try:
            sock.connect((server_ip, server_port))
            self.service_socket = sock
            self.connected = True
            return "success"
        except:
            return "fail"

    def server_interaction(self):
        msg = ''
        while self.keep_running:
            try:
                if self.connected == False:
                    raise ConnectionLost

                new_msg = self.service_socket.recv(MAX_MSG_LENGTH)
                if new_msg == '':
                    raise ConnectionLost
                    break

                msg += new_msg
                while msg != '':
                    type, length, value, msg = pkt.parse(msg)
                    if type == pkt.TYPE_NONE:
                        break

                    #print type, length
                    if type == pkt.ALL_DEV:
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
                        continue
                    if type == pkt.DEVICE_LOG:
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
                                log = logtimestr + ":" + devname + ":" + log
                                self.logfile.write(log)
                        continue
                    if type == pkt.RESPONSE:
                        type = value.split(',')[0]
                        value = value[len(type) + 1:]
                        try:
                            self.response_queue.put([type, value], False)
                        except:
                            pass
                        continue
            except ConnectionLost:
                self.connected = False
                self.service_socket.close()
                print 'connection to server lost, try reconnecting...'
                while True:
                    result = self.connect_to_server(self.server_ip, self.server_port)
                    if result == 'success':
                        break
                    time.sleep(2)
                print 'connection to server resumed'
                random.seed()
                time.sleep(1.2 + random.random())
                for devname in self.subscribed:
                    self.send_packet(pkt.LOG_SUB, self.subscribed[devname])
                    self.send_packet(pkt.DEVICE_CMD, self.subscribed[devname] + ':help')
            except socket.error as e:
                if e.errno == errno.ECONNRESET:
                    print 'Warning: connection closed by server'
                    self.connected = False
                    continue
                if DEBUG:traceback.print_exc()
                break
            except:
                if DEBUG: traceback.print_exc()
                break
        self.keep_running = False;

    def wait_cmd_response(self, type, timeout):
        while self.response_queue.empty() == False:
            self.response_queue.get()
        response = None
        start = time.time()
        while True:
            try:
                item = self.response_queue.get(False)
            except:
                item = None
            if time.time() - start >= timeout:
                break
            if item == None:
                time.sleep(0.01)
                continue
            if item[0] == type:
                response = item[1]
                break
        return response

    def get_devstr_by_partialstr(self, partialstr):
        devices = list(self.device_list)
        devices.sort()
        for devstr in devices:
            if partialstr in devstr:
                return devstr
        return ""

    def send_file_to_client(self, devname, filepath):
        #argument check
        if devname not in self.subscribed:
            print "{0} is not subscribed".format(devname)
            return False
        try:
            expandfilepath = path.expanduser(filepath)
        except:
            print "{0} does not exist".format(filepath)
            return False
        if path.exists(expandfilepath) == False:
            print "{0} does not exist".format(filepath)
            return False
        filepath = expandfilepath

        filehash = pkt.hash_of_file(filepath)
        devstr = self.subscribed[devname]

        #send file begin
        content = devstr  + ':' + filehash + ':' + path.basename(filepath)
        retry = 4
        while retry > 0:
            self.send_packet(pkt.FILE_BEGIN, content)
            response = self.wait_cmd_response(pkt.FILE_BEGIN, 0.6)
            if response == None: #timeout
                retry -= 1;
                continue
            if response == 'busy':
                print "file transfer busy: wait..."
                time.sleep(5)
                continue
            elif response == 'ok' or response == 'exist':
                break
            else:
                print "file transfer error: unexpected response '{0}'".format(response)
                return False
        if retry == 0:
            print 'file transfer error: retry timeout'
            return False
        if response == 'exist':
            return True

        #send file data
        seq = 0
        file = open(filepath,'r')
        header = devstr  + ':' + filehash + ':' + str(seq) + ':'
        content = file.read(1024)
        while(content):
            retry = 4
            while retry > 0:
                self.send_packet(pkt.FILE_DATA, header + content)
                response = self.wait_cmd_response(pkt.FILE_DATA, 0.6)
                if response == None: #timeout
                    retry -= 1;
                    continue
                elif response != 'ok':
                    print('send data fragement {0} failed, error: {1}'.format(header, response))
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
        content = devstr  + ':' + filehash + ':' + path.basename(filepath)
        retry = 4
        while retry > 0:
            self.send_packet(pkt.FILE_END, content)
            response = self.wait_cmd_response(pkt.FILE_END, 0.6)
            if response == None: #timeout
                retry -= 1;
                continue
            elif response != 'ok':
                print('send file failed, error: {0}'.format(response))
                return False
            break
        if retry == 0:
            return False
        return True

    def device_allocate(self, model, number, timeout, purpose='general', reserve='yes'):
        """ request server to allocte free/ilde devices to do autotest

        arguments:
           model  : device model, example: 'mk3060', 'esp32'
           number : number of devices to allocate
           timeout: time (seconds) to wait before timeout
           purpose: specify test purpose, example: 'alink'

        return: allocated device list; empty list if failed
        """
        if self.connected == False:
            return []
        content = ','.join([model, str(number), purpose, reserve])
        allocated = []
        timeout += time.time()
        while time.time() < timeout:
            self.send_packet(pkt.DEVICE_ALLOC, content)
            response = self.wait_cmd_response(pkt.DEVICE_ALLOC, 0.8)
            if response == None: #timeout
                time.sleep(8)
                continue
            values = response.split(',')
            if len(values) != 2:
                print 'error: illegal allocate reponse - {0}'.format(response)
                break
            result = values[0]
            allocated = values[1].split('|')
            if result != 'success': #failed
                allocated = []
                time.sleep(8)
                continue
            if len(allocated) != number:
                print "error: allocated number does not equal requested"
                allocated = []
            break
        return allocated

    def device_subscribe(self, devices):
        """ unsubscribe to devices, thus start receiving log from these devices

        arguments:
           devices : the list of devices to subscribe

        return: Ture - succeed; False - failed
        """
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
            self.send_packet(pkt.LOG_SUB, self.subscribed[devname])
        return True

    def device_unsubscribe(self, devices):
        """ unsubscribe to devices, thus stop receiving log from these devices

        arguments:
           devices : the list of devices to unsubscribe

        return: None
        """
        for devname in list(devices):
            if devname not in self.subscribed:
                continue
            self.send_packet(pkt.LOG_UNSUB, self.subscribed[devname])
            self.subscribed_reverse.pop(self.subscribed[devname])
            self.subscribed.pop(devname)

    def device_erase(self, devname):
        """ erase device

        arguments:
           devname : the device to be erased

        return: Ture - succeed; False - failed
        """
        if self.connected == False:
            return False
        if devname not in self.subscribed:
            print "error: device {0} not subscribed".format(devname)
            return False
        content = self.subscribed[devname]
        self.send_packet(pkt.DEVICE_ERASE, content);
        response = self.wait_cmd_response(pkt.DEVICE_ERASE, 120)
        if response == "success":
            ret = True
        else:
            ret = False
        return ret

    def device_program(self, devname, address, filepath):
        """ program device with a firmware file

        arguments:
           devname : the device to be programmed
           address : start address write the firware(in hex format), example: '0x13200'
           filepath: firmware file path, only bin file is supported right now

        return: Ture - succeed; False - failed
        """
        if self.connected == False:
            return False
        if devname not in self.subscribed:
            print "error: device {0} not subscribed".format(devname)
            return False
        if address.startswith('0x') == False:
            print "error: wrong address input {0}, address should start with 0x".format(address)
            return False
        try:
            expandname = path.expanduser(filepath)
        except:
            print "{0} does not exist".format(filepath)
            return False
        if path.exists(expandname) == False:
            print "{0} does not exist".format(filepath)
            return False

        retry = 3
        while retry:
            if self.send_file_to_client(devname, expandname) == False:
                retry -= 1
                time.sleep(5)
            else:
                break
        if retry == 0:
            return False

        filehash = pkt.hash_of_file(expandname)
        content = self.subscribed[devname] + ',' + address + ',' + filehash
        self.send_packet(pkt.DEVICE_PROGRAM, content);
        elapsed_time = time.time()
        response = self.wait_cmd_response(pkt.DEVICE_PROGRAM, 270)
        if response == "success":
            ret = True
        else:
            ret = False
        elapsed_time = time.time() - elapsed_time
        print "Programming device {0} finished in {1:0.1f}S, result:{2}".format(devname, elapsed_time, ret==True and 'succeed' or 'failed')
        return ret

    def device_control(self, devname, operation):
        """ control device

        arguments:
           operation : the control operation, which can be 'start', 'stop' or 'reset'
                       note: some device may not support 'start'/'stop' operation

        return: Ture - succeed; False - failed
        """
        operations = {"start":pkt.DEVICE_START, "stop":pkt.DEVICE_STOP, "reset":pkt.DEVICE_RESET}

        if self.connected == False:
            return False
        if devname not in self.subscribed:
            return False
        if operation not in list(operations):
            return False

        content = self.subscribed[devname]
        self.send_packet(operations[operation], content)
        response = self.wait_cmd_response(operations[operation], 10)
        if response == "success":
            return True
        else:
            return False

    def device_run_cmd(self, devname, args, response_lines = 0, timeout=0.8, filters=[""]):
        """ run command at device

        arguments:
           devname       : the device to run command
           args          : command arguments, example: 'netmgr connect wifi_ssid wifi_passwd'
           response_lines: number of response lines to wait for, default to 0 (do not wait response)
           timeout       : time (seconds) before wait reponses timeout, default to 0.8S
           filter        : filter applied to response, only response line containing filter item will be returned
                           by default, filter=[""] will match any response line

        return: list containing filtered responses
        """

        if self.connected == False:
            return False
        if devname not in self.subscribed:
            return False
        if len(args) == 0:
            return False
        content = self.subscribed[devname]
        content += ':' + self.poll_str + args.replace(' ', '|')
        with self.filter_lock:
            self.filter['devname'] = devname
            self.filter['cmdstr'] = args
            self.filter['lines_exp'] = response_lines
            self.filter['lines_num'] = 0
            self.filter['filters'] = filters
            self.filter['response'] = []

        retry = 3
        while retry > 0:
            self.send_packet(pkt.DEVICE_CMD, content)
            self.filter_event.clear()
            self.filter_event.wait(timeout)
            if self.filter['lines_num'] > 0:
                break;
            retry -= 1
        response = self.filter['response']
        with self.filter_lock:
            self.filter = {}
        return response

    def device_read_log(self, devname, lines, timeout=1, filters=[""]):
        """ read device log ouput

        arguments:
           devname       : the device to read output from
           response_lines: number of response lines to read
           timeout       : time (seconds) to wait before timeout, default to 1S
           filter        : filter applied to response, only log line containing filter item will be returned
                           by default, filter=[""] will match any response line

        return: list containing log lines
        """

        if self.connected == False:
            return False
        if devname not in self.subscribed:
            return False
        if lines <= 0:
            return False
        with self.filter_lock:
            self.filter['devname'] = devname
            self.filter['cmdstr'] = self.poll_str
            self.filter['lines_exp'] = lines
            self.filter['lines_num'] = 1
            self.filter['filters'] = filters
            self.filter['response'] = []

        self.filter_event.clear()
        self.filter_event.wait(timeout)
        response = self.filter['response']
        with self.filter_lock:
            self.filter = {}
        return response

    def start(self, server_ip, server_port, logname=None):
        """ start Autotest moudle, establish connnection to server

        arguments:
           server_ip  : server ip/hostname to connect
           server_port: server TCP port number to connect
           logname    : log file name(log saved at ./test/logname); logname=None means do not save log

        return: Ture - succeed; False - failed
        """

        self.server_ip = server_ip
        self.server_port = server_port
        result = self.connect_to_server(server_ip, server_port)
        if result != 'success':
            print "connect to server {0}:{1} failed".format(server_ip, server_port)
            return False

        self.connected = True
        self.logfile = None
        if logname != None:
            if path.exists('testlog') == False:
                subprocess.call(['mkdir','testlog'])

            try:
                self.logfile = open("testlog/"+logname, 'w');
            except:
                print "open logfile {0} failed".format(logfile)
                return False

        thread.start_new_thread(self.packet_send_thread, ())
        thread.start_new_thread(self.server_interaction, ())
        time.sleep(0.5)
        return True

    def stop(self):
        """ stop Autotest moudle, disconnect server and clean up

        return: Ture - succeed; False - failed
        """

        self.keep_running = False
        time.sleep(0.2)
        if self.connected:
            self.service_socket.close()
        if self.logfile != None:
            self.logfile.close()
            self.logfile = None

class Testcase:
    models = {
        'mk3060':'0x13200',
        'esp32':'0x10000',
        'stm32f429':'0x8000000',
        'stm32l432':'0x8000000',
        'stm32l475':'0x8000000',
        'stm32f103':'0x8000000',
        'stm32f091':'0x8000000',
        'starterkit':'0x8000000',
        'developerkit':'0x8000000',
        'esp8266': '0x1000',
        'lpc54018':'0x10000000',
        'lpc54102':'0x0',
        'lpc54114':'0x0',
        'lpc54628':'0x0',
        'nrf52832':'0x0',
        'atsaml21j18' :'0x0',
        'w600':'0x0',
        'mimxrt1052xxxxb':'0x60080000',
        'gd32f350rb':'0x8000000',
        'gd32f307vc':'0x8000000',
        'gd32f450zk':'0x8000000',
        'atsame54p20':'0x0',
        'mk3080':'0x19000',
        'nrf52832_xxaa':'0x0',
        'sv6266':'0x0',
        'csky':'0x10010800',
        'xr871':'0x0',
        'rda5981':'0x4000',
    }

    ap_ssid = 'aos_test_01'
    ap_pass = 'Alios@Embedded'
    server = '11.238.148.46'
    port = 34568
    model = 'unknown'
    firmware = None
    at=Autotest()

    def __init__(self, testname='default', argv=None):
        if argv == None:
            argv = sys.argv
        self.parse_args(argv)

        now=time.strftime('%Y-%m-%d_%H-%M-%S')
        self.logname = '{}@{}@{}.log'.format(testname, self.model, now)

    def parse_args(self, argv):
        #parse input
        i = 1
        while i < len(argv):
            arg = sys.argv[i]
            if arg.startswith('--firmware='):
                args = arg.split('=')
                if len(args) != 2:
                    print 'wrong argument {0} input, example: --firmware=firmware.bin'.format(arg)
                self.firmware = args[1]
            elif arg.startswith('--model='):
                args = arg.split('=')
                if len(args) != 2:
                    print 'wrong argument {0} input, example: --model=mk3060'.format(arg)
                self.model = args[1]
            elif arg.startswith('--server='):
                args = arg.split('=')
                if len(args) != 2:
                    print 'wrong argument {0} input, example: --server=192.168.10.16'.format(arg)
                    return [1, 'argument {0} error'.format(arg)]
                self.server = args[1]
            elif arg.startswith('--port='):
                args = arg.split('=')
                if len(args) != 2 or args[1].isdigit() == False:
                    print 'wrong argument {0} input, example: --port=34568'.format(arg)
                    return [1, 'argument {0} error'.format(arg)]
                self.port = int(args[1])
            elif arg.startswith('--wifissid='):
                args = arg.split('=')
                if len(args) != 2:
                    print 'wrong argument {0} input, example: --wifissid=test_wifi'.format(arg)
                self.ap_ssid = args[1]
            elif arg.startswith('--wifipass='):
                args = arg.split('=')
                if len(args) != 2:
                    print 'wrong argument {0} input, example: --wifipass=test_password'.format(arg)
                self.ap_pass = args[1]
            elif arg=='--help':
                print 'Usage: python {0} [--firmware=xxx.bin] [--wifissid=wifi_ssid] [--wifipass=password]'.format(sys.argv[0])
                return [0, 'help']
            i += 1

    def start(self, number, purpose):
        at = self.at
        model = self.model
        server = self.server
        port = self.port
        firmware = self.firmware

        if model not in self.models:
            print "error: unsupported model {}".format(repr(model))
            return [1, 'unsupported model']

        if at.start(server, port, self.logname) == False:
            return [1, 'connect testbed failed']

        #request device allocation
        timeout = 10
        allocated = at.device_allocate(model, number, timeout, purpose)
        if len(allocated) != number:
            return [1, 'allocate device failed']
        devices={} #construct device list
        for i in range(len(allocated)):
            devices[chr(ord('A') + i)] = allocated[i]
        print 'allocated:'
        for device in sorted(list(devices)):
            print '    {}: {}'.format(device, devices[device])

        #subscribe
        result = at.device_subscribe(devices)
        if result == False:
            print 'subscribe devices failed'
            return [1, 'subscribe devices failed']

        #reboot to capture the device (prevent others use it)
        for device in sorted(list(devices)):
            at.device_control(device, 'reset')

        #program devices
        device_list = sorted(list(devices))
        address = self.models[model]
        for device in device_list:
            result = at.device_program(device, address, firmware)
            self.app_start_time = time.time()
            if result == False:
                print 'program device failed'
                return [1, 'program device failed']
            print 'program succeed'
        return [0, 'succeed']

    def stop(self):
        self.at.stop()

    def get_model(self):
        return self.model

    def get_firmware(self):
        return self.firmware

    def get_log(self):
        flog = open('testlog/' + self.logname, 'r')
        return flog

    def dump_log(self):
        flog = open('testlog/' + self.logname, 'r')
        for line in flog:
            try:
                logtime = line.split(':')[0]
                sec_int, sec_frac = logtime.split('.')
                sec_int = time.mktime(time.strptime(sec_int, "%Y-%m-%d %H-%M-%S"))
                sec_frac = float('0.' + sec_frac)
                logtime = sec_int + sec_frac
                if logtime >= self.app_start_time:
                    print line,
            except:
                print line,
        flog.close()
