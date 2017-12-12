import os, sys, time, platform, json, traceback, random
import socket, thread, threading, subprocess, signal, Queue
import TBframe

DEBUG = True
LOCALLOG = False

try:
    import serial
except:
    print "error: you haven't install pyserial yet"
    print 'intall@ubuntu: sudo apt-get install python-pip'
    print '               sudo pip install pyserial'
    sys.exit(1)

if platform.system() == 'Windows':
    import serial.tools.list_ports
else:
    import glob

MAX_MSG_LENTH = 2000

def signal_handler(sig, frame):
    print "received SIGINT"
    raise KeyboardInterrupt

class ConnectionLost(Exception):
    pass

class Client:
    def __init__(self):
        self.service_socket = 0
        self.devices = {}
        self.keep_running = True
        self.connected = False
        self.models = ['mk3060', 'esp32']
        self.poll_str = '\x1b[t'
        bytes = os.urandom(4)
        for byte in bytes:
            self.poll_str += '{0:02x}'.format(ord(byte))
        self.poll_str += 'm'
        self.poll_interval = 60
        self.uuid = ''

    def send_device_list(self):
        device_list = []
        for port in list(self.devices):
            if self.devices[port]['valid']:
                device_list.append(port)
        content = ':'.join(device_list)
        data = TBframe.construct(TBframe.CLIENT_DEV, content);
        try:
            self.service_socket.send(data)
        except:
            self.connected = False

    def run_poll_command(self, port, command, lines_expect, timeout):
        filter = {}
        response = []
        while self.devices[port]['fqueue'].empty() == False:
            self.devices[port]['fqueue'].get()
        self.devices[port]['serial'].write(self.poll_str + command + '\r')
        start = time.time()
        while True:
            try:
                log = self.devices[port]['fqueue'].get(False)
            except:
                log = None
            if time.time() - start >= timeout:
                break
            if log == None:
                time.sleep(0.01)
                continue
            log = log.replace('\r', '')
            log = log.replace('\n', '')
            log = log.replace(self.poll_str, '')
            if log == '':
                continue
            response.append(log)
            if len(response) > lines_expect:
                break

        if len(response) > 0:
            response.pop(0)
        if not response:
            print "device {0} run poll commad '{1}' faild".format(port, command)
        return response

    def device_cmd_process(self, port, exit_condition):
        poll_fail_num = 0
        poll_queue = Queue.Queue(12)
        if self.devices[port]['attributes'] != {}:
            content = port + ':' + json.dumps(self.devices[port]['attributes'], sort_keys=True)
            data = TBframe.construct(TBframe.DEVICE_STATUS, content)
            self.service_socket.send(data)
        poll_timeout = time.time() + 5 + random.uniform(0, self.poll_interval/3)
        while os.path.exists(port) and exit_condition.is_set() == False:
            try:
                if time.time() >= poll_timeout:
                    poll_timeout += self.poll_interval
                    poll_queue.put(['devname', 1, 0.2])
                    poll_queue.put(['mac', 1, 0.2])
                    poll_queue.put(['version', 2, 0.2])
                    poll_queue.put(['uuid', 1, 0.2])
                    poll_queue.put(['umesh status', 11, 0.2])
                    poll_queue.put(['umesh extnetid', 1, 0.2])
                    poll_queue.put(['umesh nbrs', 33, 0.3])

                block=True
                timeout=0
                try:
                    args = None
                    if self.devices[port]['queue'].empty() == True and poll_queue.empty() == True:
                        args = self.devices[port]['queue'].get(block=True, timeout=0.1)
                    elif self.devices[port]['queue'].empty() == False:
                        args = self.devices[port]['queue'].get()
                except Queue.Empty:
                    args = None
                    continue
                except:
                    if DEBUG: traceback.print_exc()
                    args = None
                    continue

                if args != None:
                    type = args[0]
                    term = args[1]
                    if type == TBframe.DEVICE_ERASE:
                        self.device_erase(port, term)
                    elif type == TBframe.DEVICE_PROGRAM:
                        address = args[2]
                        filename = args[3]
                        self.device_program(port, address, filename, term)
                    elif type == TBframe.DEVICE_RESET or type == TBframe.DEVICE_START or type == TBframe.DEVICE_STOP:
                        self.device_control(port, type, term)
                    elif type == TBframe.DEVICE_CMD:
                        cmd = args[2]
                        self.device_run_cmd(port, cmd, term)
                    else:
                        print "error: unknown operation tyep {0}".format(repr(type))
                    args = None
                    time.sleep(0.05)
                    continue

                if poll_queue.empty() == True:
                    continue

                [cmd, lines, timeout] = poll_queue.get()
                response = self.run_poll_command(port, cmd, lines, timeout)

                if cmd == 'devname': #poll device model
                    if len(response) == lines and response[0].startswith('device name:'):
                        poll_fail_num = 0
                        self.devices[port]['attributes']['model'] = response[0].split()[-1]
                    else:
                        poll_fail_num += 1
                elif cmd == 'mac': #poll device mac
                    if len(response) == 1 and response[0].startswith('MAC address:'):
                        poll_fail_num = 0
                        macaddr = response[0].split()[-1]
                        macaddr = macaddr.replace('-', '') + '0000'
                        self.devices[port]['attributes']['macaddr'] = macaddr
                    else:
                        poll_fail_num += 1
                elif cmd == 'version': #poll device version
                    if len(response) == lines:
                        poll_fail_num = 0
                        for line in response:
                            if 'kernel version :' in line:
                                self.devices[port]['attributes']['kernel_version'] = line.replace('kernel version :AOS-', '')
                            if 'app version :' in line:
                                line = line.replace('app version :', '')
                                line = line.replace('app-', '')
                                line = line.replace('APP-', '')
                                self.devices[port]['attributes']['app_version'] = line
                    else:
                        poll_fail_num += 1
                elif cmd == 'umesh status': #poll mesh status
                    if len(response) == lines:
                        poll_fail_num = 0
                        for line in response:
                            if 'state\t' in line:
                                self.devices[port]['attributes']['state'] = line.replace('state\t', '')
                            elif '\tnetid\t' in line:
                                self.devices[port]['attributes']['netid'] = line.replace('\tnetid\t', '')
                            elif '\tsid\t' in line:
                                self.devices[port]['attributes']['sid'] = line.replace('\tsid\t', '')
                            elif '\tnetsize\t' in line:
                                self.devices[port]['attributes']['netsize'] = line.replace('\tnetsize\t', '')
                            elif '\trouter\t' in line:
                                self.devices[port]['attributes']['router'] = line.replace('\trouter\t', '')
                            elif '\tchannel\t' in line:
                                self.devices[port]['attributes']['channel'] = line.replace('\tchannel\t', '')
                    else:
                        poll_fail_num += 1
                elif cmd == 'umesh nbrs': #poll mesh nbrs
                    if len(response) > 0 and 'num=' in response[-1]:
                        poll_fail_num = 0
                        nbrs = {}
                        for line in response:
                            if '\t' not in line or ',' not in line:
                                continue
                            line = line.replace('\t', '')
                            nbr_info = line.split(',')
                            if len(nbr_info) < 10:
                                continue
                            nbrs[nbr_info[0]] = {'relation':nbr_info[1], \
                                                 'netid':nbr_info[2], \
                                                 'sid':nbr_info[3], \
                                                 'link_cost':nbr_info[4], \
                                                 'child_num':nbr_info[5], \
                                                 'channel':nbr_info[6], \
                                                 'reverse_rssi':nbr_info[7], \
                                                 'forward_rssi':nbr_info[8], \
                                                 'last_heard':nbr_info[9]}
                            if len(nbr_info) > 10:
                                nbrs[nbr_info[0]]['awake'] = nbr_info[10]
                        self.devices[port]['attributes']['nbrs'] = nbrs
                    else:
                        poll_fail_num += 1
                elif cmd == 'umesh extnetid': #poll mesh extnetid
                    if len(response) == 1 and response[0].count(':') == 5:
                        poll_fail_num += 1
                        self.devices[port]['attributes']['extnetid'] = response[0]
                    else:
                        poll_fail_num += 1
                elif cmd == 'uuid': #poll uuid
                    if len(response) == 1 and 'uuid:' in response[0]:
                        poll_fail_num = 0
                        self.devices[port]['attributes']['uuid'] = response[0].replace('uuid: ', '')
                    else:
                        poll_fail_num += 1
                else:
                    print "error: unrecognized poll cmd '{0}'".format(cmd)
                    continue

                if poll_fail_num >= 7:
                    if self.devices[port]['attributes']['status'] == 'active':
                        print "device {0} become inactive".format(port)
                    self.devices[port]['attributes']['status'] = 'inactive'
                else:
                    if self.devices[port]['attributes']['status'] == 'inactive':
                        print "device {0} become active".format(port)
                    self.devices[port]['attributes']['status'] = 'active'

                if poll_queue.empty() == False:
                    continue
                content = port + ':' + json.dumps(self.devices[port]['attributes'], sort_keys=True)
                data = TBframe.construct(TBframe.DEVICE_STATUS, content)
                try:
                    self.service_socket.send(data)
                except:
                    self.connected = False
                    continue
            except:
                if os.path.exists(port) == False:
                    exit_condition.set()
                    break
                if exit_condition.is_set() == True:
                    break
                if DEBUG: traceback.print_exc()
                self.devices[port]['serial'].close()
                self.devices[port]['serial'].open()
        print 'devie command process thread for {0} exited'.format(port)

    def device_log_poll(self, port, exit_condition):
        log_time = time.time()
        log = ''
        if LOCALLOG:
            logfile= 'client/' + port.split('/')[-1] + '.log'
            flog = open(logfile, 'a+')
        while os.path.exists(port) and exit_condition.is_set() == False:
            if self.connected == False or self.devices[port]['slock'].locked():
                time.sleep(0.01)
                continue

            newline = False
            while self.devices[port]['slock'].acquire(False) == True:
                try:
                    c = self.devices[port]['serial'].read(1)
                except:
                    c = ''
                finally:
                    self.devices[port]['slock'].release()

                if c == '':
                    break

                if log == '':
                    log_time = time.time()
                log += c
                if c == '\n':
                    newline = True
                    break

            if newline == True and log != '':
                if self.poll_str in log and self.devices[port]['fqueue'].full() == False:
                    self.devices[port]['fqueue'].put(log, False)
                if LOCALLOG:
                    flog.write('{0:.3f}:'.format(log_time) + log)
                log = port + ':{0:.3f}:'.format(log_time) + log
                data = TBframe.construct(TBframe.DEVICE_LOG,log)
                log = ''
                try:
                    self.service_socket.send(data)
                except:
                    self.connected == False
                    continue
        if LOCALLOG:
            flog.close()
        print 'device {0} removed'.format(port)
        self.devices[port]['valid'] = False
        exit_condition.set()
        self.devices[port]['serial'].close()
        self.send_device_list()
        print 'device log poll thread for {0} exited'.format(port)

    def device_monitor(self):
        while self.keep_running:
            os = platform.system()
            if os == 'Windows':
                coms = serial.tools.list_ports.comports()
                devices_new = []
                for com in coms:
                    devices_new.append(com.device)
            elif os == 'Linux':
                devices_new = glob.glob('/dev/mxchip-*')
                devices_new += glob.glob('/dev/espif-*')
                if devices_new == []:
                    devices_new += glob.glob('/dev/ttyUSB*')
            elif os == 'Darwin':
                devices_new = glob.glob('/dev/tty.usbserial*')
            elif 'CYGWIN' in os:
                devices_new = glob.glob('/dev/ttyS*')
            else:
                print 'unsupported os "{0}"'.format(os)
                break

            devices_new.sort()
            for port in devices_new:
                if port in self.devices and self.devices[port]['valid']:
                    continue
                if port in self.devices:
                    try:
                        self.devices[port]['serial'].close()
                        self.devices[port]['serial'].open()
                    except:
                        print 'device_monitor, error: unable to open {0}'.format(port)
                        continue
                    if self.devices[port]['slock'].locked():
                        self.devices[port]['slock'].release()
                    while self.devices[port]['queue'].empty() == False:
                        self.devices[port]['queue'].get()
                    while self.devices[port]['fqueue'].empty() == False:
                        self.devices[port]['fqueue'].get()
                    self.devices[port]['valid'] = True
                else:
                    if 'mxchip' in port:
                        baudrate = 921600
                    elif 'espif' in port:
                        baudrate = 115200
                    else:
                        baudrate = 115200

                    try:
                        ser = serial.Serial(port, baudrate, timeout = 0.02)
                    except:
                        print 'device_monitor, error: unable to open {0}'.format(port)
                        continue
                    self.devices[port] = {'valid':True, \
                                          'serial':ser, \
                                          'slock':threading.Lock(), \
                                          'queue':Queue.Queue(12), \
                                          'attributes':{}, \
                                          'fqueue':Queue.Queue(64)}
                try:
                    if 'mxchip' in port:
                        self.devices[port]['attributes']['model'] = 'MK3060'
                        ser.setRTS(False)
                    if 'espif' in port:
                        self.devices[port]['attributes']['model'] = 'ESP32'
                        ser.setRTS(True)
                        ser.setDTR(False)
                        time.sleep(0.1)
                        ser.setDTR(True)
                except:
                    self.devices[port]['valid'] = False
                    continue
                print 'device {0} added'.format(port)
                self.devices[port]['attributes']['status'] = 'inactive'
                exit_condition = threading.Event()
                thread.start_new_thread(self.device_log_poll, (port, exit_condition,))
                thread.start_new_thread(self.device_cmd_process, (port, exit_condition,))
                self.send_device_list()
            time.sleep(0.1)
        print 'device monitor thread exited'
        self.keep_running = False

    def esp32_erase(self, port):
        retry = 3
        baudrate = 230400
        error = 'fail'
        while retry > 0:
            script = ['python', 'esptool.py']
            script += ['--chip']
            script += ['esp32']
            script += ['--port']
            script += [port]
            script += ['--baud']
            script += [str(baudrate)]
            script += ['erase_flash']
            ret = subprocess.call(script)
            if ret == 0:
                error = 'success'
                break
            retry -= 1
            baudrate = baudrate / 2
        return error

    def device_erase(self, port, term):
        model = 'unknown'
        if 'model' in self.devices[port]['attributes']:
            model = self.devices[port]['attributes']['model']
        model = model.lower()

        if model != 'esp32':
            print "error: erasing dose not support model '{0}'".format(model)
            content = ','.join(term) + ',' + 'unknow model'
            self.send_packet(TBframe.DEVICE_ERASE, content)
            return

        self.devices[port]['slock'].acquire()
        try:
            self.devices[port]['serial'].close()
            ret = self.esp32_erase(port)
        except:
            if DEBUG: traceback.print_exc()
            ret = 'fail'
        finally:
            if os.path.exists(port) and \
               self.devices[port]['serial'].isOpen() == False:
                self.devices[port]['serial'].open()
            self.devices[port]['slock'].release()
        print 'erasing', port, '...', ret
        content = ','.join(term) + ',' + ret
        self.send_packet(TBframe.DEVICE_ERASE, content)

    def esp32_program(self, port, address, file):
        retry = 3
        baudrate = 230400
        error = 'fail'
        while retry > 0:
            script = ['python']
            script += [os.environ['ESPTOOL_PATH'] + '/esptool.py']
            script += ['--chip']
            script += ['esp32']
            script += ['--port']
            script += [port]
            script += ['--baud']
            script += [str(baudrate)]
            script += ['--before']
            script += ['default_reset']
            script += ['--after']
            script += ['hard_reset']
            script += ['write_flash']
            script += ['-z']
            script += ['--flash_mode']
            script += ['dio']
            script += ['--flash_freq']
            script += ['40m']
            script += ['--flash_size']
            script += ['4MB']
            script += [address]
            script += [file]
            ret = subprocess.call(script)
            if ret == 0:
                error =  'success'
                break
            retry -= 1
            baudrate = baudrate / 2
        return error

    def mxchip_program(self, port, address, file):
        retry = 3
        error = 'fail'
        while retry > 0:
            script = ['python', 'aos_firmware_update.py']
            script += [port]
            script += [address]
            script += [file]
            script += ['--hardreset']
            ret = subprocess.call(script)
            if ret == 0:
                error =  'success'
                break
            retry -= 1
            time.sleep(4)
        return error

    def device_program(self, port, address, file, term):
        if os.path.exists(port) == False or port not in self.devices:
            print "error: progamming nonexist port {0}".format(port)
            content = ','.join(term) + ',' + 'device nonexist'
            self.send_packet(TBframe.DEVICE_PROGRAM, content)
            return

        model = 'unknown'
        if 'model' in self.devices[port]['attributes']:
            model = self.devices[port]['attributes']['model']
        model = model.lower()
        if model not in self.models:
            print "error: programing dose not support model '{0}'".format(model)
            content = ','.join(term) + ',' + 'unknown model'
            self.send_packet(TBframe.DEVICE_PROGRAM, content)
            return

        self.devices[port]['slock'].acquire()
        try:
            self.devices[port]['serial'].close()
            if model == 'esp32':
                ret = self.esp32_program(port, address, file)
            elif model == 'mk3060':
                ret = self.mxchip_program(port, address, file)
            self.devices[port]['serial'].open()
        except:
            if DEBUG: traceback.print_exc()
            ret = 'fail'
        finally:
            if os.path.exists(port) and \
               self.devices[port]['serial'].isOpen() == False:
                self.devices[port]['serial'].open()
            self.devices[port]['slock'].release()
        print 'programming', file, 'to', port, '@', address, '...', ret
        content = ','.join(term) + ',' + ret
        self.send_packet(TBframe.DEVICE_PROGRAM, content)

    def esp32_control(self, port, operation):
        try:
            if operation == TBframe.DEVICE_RESET:
                self.devices[port]['serial'].setDTR(False)
                time.sleep(0.1)
                self.devices[port]['serial'].setDTR(True)
                return 'success'
            elif operation == TBframe.DEVICE_STOP:
                self.devices[port]['serial'].setDTR(False)
                return 'success'
            elif operation == TBframe.DEVICE_START:
                self.devices[port]['serial'].setDTR(True)
                return 'success'
        except:
            pass
        return 'fail'

    def mxchip_control(self, port, operation):
        try:
            if operation == TBframe.DEVICE_RESET:
                self.devices[port]['serial'].setRTS(True)
                time.sleep(0.1)
                self.devices[port]['serial'].setRTS(False)
                return 'success'
            elif operation == TBframe.DEVICE_STOP:
                self.devices[port]['serial'].setRTS(True)
                return 'success'
            elif operation == TBframe.DEVICE_START:
                self.devices[port]['serial'].setRTS(False)
                return 'success'
        except:
            pass
        return 'fail'

    def device_control(self, port, type, term):
        operations= {TBframe.DEVICE_RESET:'reset', TBframe.DEVICE_STOP:'stop', TBframe.DEVICE_START:'start'}
        if os.path.exists(port) == False or port not in self.devices:
            print "error: controlling nonexist port {0}".format(port)
            content = ','.join(term) + ',' + 'device nonexist'
            self.send_packet(type, content)
            return

        model = 'unknown'
        if 'model' in self.devices[port]['attributes']:
            model = self.devices[port]['attributes']['model']
        model = model.lower()
        if model not in self.models:
            print "error: controlling unsupported model '{0}'".format(model)
            content = ','.join(term) + ',' + 'unknown model'
            self.send_packet(type, content)
            return

        try:
            if model == 'esp32':
                ret = self.esp32_control(port, type)
            elif model == 'mk3060':
                ret = self.mxchip_control(port, type)
        except:
            if DEBUG: traceback.print_exc()
            ret = 'fail'
        print operations[type], port, ret
        content = ','.join(term) + ',' + ret
        self.send_packet(type, content)

    def device_run_cmd(self, port, cmd, term):
        if os.path.exists(port) == False or port not in self.devices:
            print "error: run command at nonexist port {0}".format(port)
            content = ','.join(term) + ',' + 'device nonexist'
            self.send_packet(TBframe.DEVICE_CMD, content)
            return

        try:
            self.devices[port]['serial'].write(cmd+'\r')
            result='success'
            print "run command '{0}' at {1} succeed".format(cmd, port)
        except:
            if DEBUG: traceback.print_exc()
            result='fail'
            print "run command '{0}' at {1} failed".format(cmd, port)
        content = ','.join(term) + ',' + result
        self.send_packet(TBframe.DEVICE_CMD, content)

    def heartbeat_func(self):
        heartbeat_timeout = time.time() + 10
        while self.keep_running:
            time.sleep(0.05)
            if time.time() < heartbeat_timeout:
                continue
            try:
                self.service_socket.send(TBframe.construct(TBframe.HEARTBEAT, ''))
            except:
                continue
            heartbeat_timeout += 10

    def send_packet(self, type, content):
        data = TBframe.construct(type, content)
        try:
            self.service_socket.send(data)
        except:
            raise ConnectionLost

    def client_func(self, server_ip, server_port):
        self.service_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        try:
            self.service_socket.connect((server_ip, server_port))
            self.connected = True
            print 'connect to server {0}:{1} succeeded'.format(server_ip, server_port)
        except:
            print 'connect to server {0}:{1} failed'.format(server_ip, server_port)
            return

        if os.path.exists('client') == False:
            os.mkdir('client')
        if os.path.exists('client/.uuid') == True:
            try:
                file = open('client/.uuid','rb')
                self.uuid = json.load(file)
                file.close()
            except:
                print "read uuid from file failed"
        if not self.uuid:
            bytes = os.urandom(6)
            self.uuid = ''
            for byte in bytes:
                self.uuid += '{0:02x}'.format(ord(byte))
            try:
                file = open('client/.uuid','w')
                file.write(json.dumps(self.uuid))
                file.close()
            except:
                print "save uuid to file failed"
        signal.signal(signal.SIGINT, signal_handler)

        self.send_packet(TBframe.CLIENT_UUID, self.uuid)
        self.send_packet(TBframe.CLIENT_TAG, self.poll_str)
        thread.start_new_thread(self.device_monitor,())
        thread.start_new_thread(self.heartbeat_func,())

        file_received = {}
        file_receiving = {}
        msg = ''
        while True:
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

                    for hash in list(file_receiving):
                        if time.time() > file_receiving[hash]['timeout']:
                            file_receiving[hash]['handle'].close()
                            try:
                                os.remove(file_receiving[hash]['name'])
                            except:
                                pass
                            file_receiving.pop(hash)

                    if type == TBframe.FILE_BEGIN:
                        split_value = value.split(':')
                        terminal = split_value[0]
                        hash = split_value[1]
                        filename = split_value[2]
                        if hash in file_received:
                            if os.path.exists(file_received[hash]) == True:
                                content = terminal + ',' + 'exist'
                                self.send_packet(type, content)
                                continue
                            else:
                                file_received.pop(hash)

                        if hash in file_receiving:
                            content = terminal + ',' + 'busy'
                            self.send_packet(type, content)
                            continue

                        filename = 'client/' + filename
                        filename += '-' + terminal.split(',')[0]
                        filename += '@' + time.strftime('%Y-%m-%d-%H-%M')
                        filehandle = open(filename, 'wb')
                        timeout = time.time() + 5
                        file_receiving[hash] = {'name':filename, 'seq':0, 'handle':filehandle, 'timeout': timeout}
                        content = terminal + ',' + 'ok'
                        self.send_packet(type, content)
                        if DEBUG:
                            print 'start receiving {0} as {1}'.format(split_value[2], filename)
                    elif type == TBframe.FILE_DATA:
                        split_value = value.split(':')
                        terminal = split_value[0]
                        hash = split_value[1]
                        seq  = split_value[2]
                        data = value[(len(terminal) + len(hash) + len(seq) + 3):]
                        seq = int(seq)
                        if hash not in file_receiving:
                            content = terminal + ',' + 'noexist'
                            self.send_packet(type, content)
                            continue
                        if file_receiving[hash]['seq'] != seq and file_receiving[hash]['seq'] != seq + 1:
                            content = terminal + ',' + 'seqerror'
                            self.send_packet(type, content)
                            continue
                        if file_receiving[hash]['seq'] == seq:
                            file_receiving[hash]['handle'].write(data)
                            file_receiving[hash]['seq'] += 1
                            file_receiving[hash]['timeout'] = time.time() + 5
                        content = terminal + ',' + 'ok'
                        self.send_packet(type, content)
                    elif type == TBframe.FILE_END:
                        split_value = value.split(':')
                        terminal = split_value[0]
                        hash = split_value[1]
                        if hash not in file_receiving:
                            content = terminal + ',' + 'noexist'
                            self.send_packet(type, content)
                            continue
                        file_receiving[hash]['handle'].close()
                        localhash = TBframe.hash_of_file(file_receiving[hash]['name'])
                        if localhash != hash:
                            response = 'hasherror'
                        else:
                            response = 'ok'
                            file_received[hash] = file_receiving[hash]['name']
                        if DEBUG:
                            print 'finished receiving {0}, result:{1}'.format(file_receiving[hash]['name'], response)
                        file_receiving.pop(hash)
                        content = terminal + ',' + response
                        self.send_packet(type, content)
                    elif type == TBframe.DEVICE_ERASE:
                        args = value.split(',')
                        if len(args) != 3:
                            continue
                        term = args[0:2]
                        port = args[2]
                        if os.path.exists(port) and port in self.devices:
                            if self.devices[port]['queue'].full() == False:
                                self.devices[port]['queue'].put([type, term])
                                continue
                            else:
                                result = 'busy'
                                print 'erase', port,  'failed, device busy'
                        else:
                            result = 'nonexist'
                            print 'erase', port,  'failed, device nonexist'
                        content = ','.join(term) + ',' + result
                        self.send_packet(type, content)
                    elif type == TBframe.DEVICE_PROGRAM:
                        args = value.split(',')
                        if len(args) != 5:
                            continue
                        term = args[0:2]
                        port = args[2]
                        address = args[3]
                        hash = args[4]
                        if hash not in file_received:
                            content = ','.join(term) + ',' + 'error'
                            self.send_packet(type, content)
                            continue
                        filename = file_received[hash]
                        if os.path.exists(port) and port in self.devices:
                            if self.devices[port]['queue'].full() == False:
                                self.devices[port]['queue'].put([type, term, address, filename])
                                continue
                            else:
                                result = 'busy'
                                print 'program {0} to {1} @ {2} failed, device busy'.format(filename, port, address)
                        else:
                            result = 'error'
                            print 'program {0} to {1} @ {2} failed, device nonexist'.format(filename, port, address)
                        content = ','.join(term) + ',' + result
                        self.send_packet(type, content)
                    elif type == TBframe.DEVICE_RESET or type == TBframe.DEVICE_START or type == TBframe.DEVICE_STOP:
                        operations = {TBframe.DEVICE_RESET:'reset', TBframe.DEVICE_START:'start', TBframe.DEVICE_STOP:'stop'}
                        args = value.split(',')
                        if len(args) != 3:
                            continue
                        term = args[0:2]
                        port = args[2]
                        if os.path.exists(port) and port in self.devices:
                            if self.devices[port]['queue'].full() == False:
                                self.devices[port]['queue'].put([type, term])
                                continue
                            else:
                                result = 'busy'
                                print operations[type], port, 'failed, device busy'
                        else:
                            result = 'nonexist'
                            print operations[type], port, 'failed, device nonexist'
                        content = ','.join(term) + ',' + result
                        self.send_packet(type, content)
                    elif type == TBframe.DEVICE_CMD:
                        args = value.split(':')[0]
                        arglen = len(args) + 1
                        args = args.split(',')
                        term = args[0:2]
                        port = args[2]
                        cmd = value[arglen:].split('|')
                        cmd = ' '.join(cmd)
                        if os.path.exists(port) and port in self.devices and \
                            self.devices[port]['valid'] == True:
                            if self.devices[port]['queue'].full() == False:
                                self.devices[port]['queue'].put([type, term, cmd])
                                continue
                            else:
                                result = 'busy'
                                print "run command '{0}' at {1} failed, device busy".format(cmd, port)
                        else:
                            result = 'nonexist'
                            print "run command '{0}' at {1} failed, device nonexist".format(cmd, port)
                        content = ','.join(term) + ',' + result
                        self.send_packet(type, content)
                    elif type == TBframe.CLIENT_UUID:
                        print 'server request UUID'
                        self.send_packet(TBframe.CLIENT_UUID, self.uuid)
                        self.send_packet(TBframe.CLIENT_TAG, self.poll_str)
                        self.send_device_list()
            except ConnectionLost:
                self.connected = False
                print 'connection to server lost, try reconnecting...'
                self.service_socket.close()
                self.service_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
                while True:
                    try:
                        self.service_socket.connect((server_ip, server_port))
                        time.sleep(0.1)
                        self.connected = True
                        self.send_packet(TBframe.CLIENT_UUID, self.uuid)
                        self.send_packet(TBframe.CLIENT_TAG, self.poll_str)
                        self.send_device_list()
                        break
                    except:
                        try:
                            time.sleep(1)
                        except:
                            self.service_socket.close()
                            return
                print 'connection to server resumed'
            except KeyboardInterrupt:
                print "client exiting ..."
                self.keep_running = False
                time.sleep(0.3)
                break
            except:
                if DEBUG: traceback.print_exc()
        self.service_socket.close()
