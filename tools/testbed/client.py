import os, sys, time, platform, json, traceback, random, re, glob, uuid
import socket, ssl, thread, threading, subprocess, signal, Queue, importlib
import TBframe

MAX_MSG_LENTH = 8192
ENCRYPT = False
DEBUG = True
EN_STATUS_POLL = True
LOCALLOG = False

try:
    import serial
except:
    print "error: you haven't install pyserial yet"
    print 'intall@ubuntu: sudo apt-get install python-pip'
    print '               sudo pip install pyserial'
    sys.exit(1)

def signal_handler(sig, frame):
    print "received SIGINT"
    raise KeyboardInterrupt

def queue_safeput(queue, item):
    try:
        queue.put(item, False)
    except:
        pass

class ConnectionLost(Exception):
    pass

class Client:
    def __init__(self):
        self.service_socket = 0
        self.devices = {}
        self.keep_running = True
        self.connected = False
        self.poll_str = '\x1b[t'
        bytes = os.urandom(4)
        for byte in bytes:
            self.poll_str += '{0:02x}'.format(ord(byte))
        self.poll_str += 'm'
        self.poll_interval = 60
        self.uuid = '{0:012x}'.format(uuid.getnode())
        self.model_interface = {}
        self.mesh_changed = [re.compile('become leader'),
                             re.compile('become detached'),
                             re.compile('allocate sid 0x[0-9a-f]{4}, become [0-9] in net [0-9a-f]{4}')]
        self.neighbor_changed = [re.compile('sid [0-9a-f]{4} mac [0-9a-f]{16} is replaced'),
                                 re.compile('[0-9a-f]{1,4} neighbor [0-9a-f]{16} become inactive')]
        self.device_uuid_changed = ["ACCS: connected",
                             "ACCS: disconnected",
                             'GATEWAY: connect to server succeed']

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
            #self.connected = False
            pass

    def run_poll_command(self, port, command, lines_expect, timeout):
        filter = {}
        response = []
        while self.devices[port]['plog_queue'].empty() == False:
            self.devices[port]['plog_queue'].get()
        self.devices[port]['serial'].write(self.poll_str + command + '\r')
        start = time.time()
        while True:
            try:
                log = self.devices[port]['plog_queue'].get(False)
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
        pcmd_queue = self.devices[port]['pcmd_queue']
        if self.devices[port]['attributes'] != {}:
            content = port + ':' + json.dumps(self.devices[port]['attributes'], sort_keys=True)
            data = TBframe.construct(TBframe.DEVICE_STATUS, content)
            try:
                self.service_socket.send(data)
            except:
                #self.connected = False
                pass
        poll_timeout = time.time() + 3 + random.uniform(0, self.poll_interval/10)
        while os.path.exists(port) and exit_condition.is_set() == False:
            try:
                if EN_STATUS_POLL == True and time.time() >= poll_timeout:
                    poll_timeout += self.poll_interval
                    queue_safeput(pcmd_queue, ['devname', 1, 0.2])
                    queue_safeput(pcmd_queue, ['mac', 1, 0.2])
                    queue_safeput(pcmd_queue, ['version', 2, 0.2])
                    queue_safeput(pcmd_queue, ['uuid', 1, 0.2])
                    queue_safeput(pcmd_queue, ['umesh status', 11, 0.2])
                    queue_safeput(pcmd_queue, ['umesh extnetid', 1, 0.2])
                    queue_safeput(pcmd_queue, ['umesh nbrs', 35, 0.3])

                block=True
                timeout=0
                try:
                    args = None
                    if self.devices[port]['ucmd_queue'].empty() == True and pcmd_queue.empty() == True:
                        args = self.devices[port]['ucmd_queue'].get(block=True, timeout=0.1)
                    elif self.devices[port]['ucmd_queue'].empty() == False:
                        args = self.devices[port]['ucmd_queue'].get()
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
                        if re.search('umesh extnetid [0-9A-Fa-f]{12}', cmd) != None:
                            queue_safeput(pcmd_queue, ['umesh extnetid', 1, 0.2])
                    else:
                        print "error: unknown operation tyep {0}".format(repr(type))
                    args = None
                    time.sleep(0.05)
                    continue

                if pcmd_queue.empty() == True:
                    continue

                [cmd, lines, timeout] = pcmd_queue.get()
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

                if pcmd_queue.empty() == False:
                    continue
                content = port + ':' + json.dumps(self.devices[port]['attributes'], sort_keys=True)
                data = TBframe.construct(TBframe.DEVICE_STATUS, content)
                try:
                    self.service_socket.send(data)
                except:
                    #self.connected = False
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

    def device_log_filter(self, port, log):
        pcmd_queue = self.devices[port]['pcmd_queue']
        if EN_STATUS_POLL == False:
            return
        if pcmd_queue.full() == True:
            return
        for flog in self.mesh_changed:
            if flog.search(log) == None:
                continue
            #print log
            #print "device {0} mesh status changed".format(port)
            queue_safeput(pcmd_queue, ['umesh status', 11, 0.2])
            queue_safeput(pcmd_queue, ['umesh nbrs', 33, 0.3])
            return
        for flog in self.neighbor_changed:
            if flog.search(log) == None:
                continue
            #print log
            #print "device {0} neighbors changed".format(port)
            queue_safeput(pcmd_queue, ['umesh nbrs', 33, 0.3])
            return
        for flog in self.device_uuid_changed:
            if flog not in log:
                continue
            #print log
            #print "device {0} uuid changed".format(port)
            queue_safeput(pcmd_queue, ['uuid', 1, 0.2])
            return

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
                if self.poll_str in log:
                    queue_safeput(self.devices[port]['plog_queue'], log)
                else:
                    self.device_log_filter(port, log)
                if LOCALLOG:
                    flog.write('{0:.3f}:'.format(log_time) + log)
                log = port + ':{0:.3f}:'.format(log_time) + log
                data = TBframe.construct(TBframe.DEVICE_LOG,log)
                log = ''
                try:
                    self.service_socket.send(data)
                except:
                    #self.connected = False
                    continue
        if LOCALLOG:
            flog.close()
        print 'device {0} removed'.format(port)
        self.devices[port]['valid'] = False
        exit_condition.set()
        self.devices[port]['serial'].close()
        self.send_device_list()
        print 'device log poll thread for {0} exited'.format(port)

    def add_new_device(self, mi, port):
        ser = mi.new_device(port)
        if ser == None:
            return False
        self.devices[port] = {'valid':True, \
                              'serial':ser, \
                              'if' : mi, \
                              'slock':threading.Lock(), \
                              'attributes':{}, \
                              'ucmd_queue':Queue.Queue(12), \
                              'pcmd_queue':Queue.Queue(64), \
                              'plog_queue':Queue.Queue(64)}
        self.devices[port]['attributes']['status'] = 'inactive'
        return True

    def add_old_device(self, mi, port):
        ser = mi.new_device(port)
        if ser == None:
            return False
        self.devices[port]['serial'] = ser
        if self.devices[port]['slock'].locked():
            self.devices[port]['slock'].release()
        while self.devices[port]['ucmd_queue'].empty() == False:
            self.devices[port]['ucmd_queue'].get()
        while self.devices[port]['pcmd_queue'].empty() == False:
            self.devices[port]['pcmd_queue'].get()
        while self.devices[port]['plog_queue'].empty() == False:
            self.devices[port]['plog_queue'].get()
        self.devices[port]['attributes']['status'] = 'inactive'
        self.devices[port]['valid'] = True
        return True

    def list_devices(self):
        os = platform.system()
        devices_new = []
        for model in self.model_interface:
            mi = self.model_interface[model]

            devices = mi.list_devices(os)
            for port in devices:
                if port in self.devices and self.devices[port]['valid'] == True:
                    continue
                if port not in self.devices:
                    ret = self.add_new_device(mi, port)
                else:
                    ret = self.add_old_device(mi, port)
                if ret == True:
                    devices_new.append(port)

        devices_new.sort()
        return devices_new

    def device_monitor(self):
        while self.keep_running:
            devices_new = self.list_devices()
            for port in devices_new:
                print 'device {0} added'.format(port)
                exit_condition = threading.Event()
                thread.start_new_thread(self.device_log_poll, (port, exit_condition,))
                thread.start_new_thread(self.device_cmd_process, (port, exit_condition,))
            if devices_new != []:
                self.send_device_list()
            time.sleep(0.1)
        print 'device monitor thread exited'
        self.keep_running = False

    def get_interface_by_model(self, model):
        if model not in self.model_interface:
            if os.path.exists('board/'+model) == False or \
               os.path.exists('board/{0}/{0}.py'.format(model)) == False:
                return None
            if 'board/'+model not in sys.path:
                sys.path.append('board/'+model)
            try:
                self.model_interface[model] =  importlib.import_module(model)
            except:
                if DEBUG: traceback.print_exc()
                return None
        return self.model_interface[model]

    def load_interfaces(self):
        candidates = glob.glob("board/*/*.py")
        for d in candidates:
            r = re.search("board/(.*)/(.*)\.py", d)
            if r == None:
                continue
            model = r.groups()[0]
            if model != r.groups()[1]:
                continue
            self.get_interface_by_model(model)
            print 'model loaded - ',model

    def device_erase(self, port, term):
        interface = self.devices[port]['if']
        if interface == None:
            print "error: erasing dose not support model '{0}'".format(model)
            content = ','.join(term) + ',' + 'unsupported model'
            self.send_packet(TBframe.DEVICE_ERASE, content)
            return

        self.devices[port]['slock'].acquire()
        try:
            self.devices[port]['serial'].close()
            ret = interface.erase(port)
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

    def device_program(self, port, address, file, term):
        if os.path.exists(port) == False or port not in self.devices:
            print "error: progamming nonexist port {0}".format(port)
            content = ','.join(term) + ',' + 'device nonexist'
            self.send_packet(TBframe.DEVICE_PROGRAM, content)
            return

        interface = self.devices[port]['if']
        if interface == None:
            print "error: programming dose not support model '{0}'".format(model)
            content = ','.join(term) + ',' + 'unsupported model'
            self.send_packet(TBframe.DEVICE_PROGRAM, content)
            return

        self.devices[port]['slock'].acquire()
        try:
            self.devices[port]['serial'].close()
            ret = interface.program(port, address, file)
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

    def device_control(self, port, type, term):
        operations= {TBframe.DEVICE_RESET:'reset', TBframe.DEVICE_STOP:'stop', TBframe.DEVICE_START:'start'}
        if os.path.exists(port) == False or port not in self.devices:
            print "error: controlling nonexist port {0}".format(port)
            content = ','.join(term) + ',' + 'device nonexist'
            self.send_packet(type, content)
            return

        interface = self.devices[port]['if']
        if interface == None:
            print "error: controlling dose not support model '{0}'".format(model)
            content = ','.join(term) + ',' + 'unsupported model'
            self.send_packet(type, content)
            return

        try:
            ret = interface.control(port, operations[type])
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
            if DEBUG: traceback.print_exc()
            return "fail"

    def client_func(self, server_ip, server_port):
        self.load_interfaces()

        result = self.connect_to_server(server_ip, server_port)
        if result == 'success':
            print 'connect to server {0}:{1} succeeded'.format(server_ip, server_port)
        else:
            print 'connect to server {0}:{1} failed'.format(server_ip, server_port)
            return

        if os.path.exists('client') == False:
            os.mkdir('client')
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
                        if time.time() < file_receiving[hash]['timeout']:
                            continue
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
                        timeout = time.time() + 4
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
                            file_receiving[hash]['timeout'] = time.time() + 4
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
                            if self.devices[port]['ucmd_queue'].full() == False:
                                self.devices[port]['ucmd_queue'].put([type, term])
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
                            if self.devices[port]['ucmd_queue'].full() == False:
                                self.devices[port]['ucmd_queue'].put([type, term, address, filename])
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
                            if self.devices[port]['ucmd_queue'].full() == False:
                                self.devices[port]['ucmd_queue'].put([type, term])
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
                            if self.devices[port]['ucmd_queue'].full() == False:
                                self.devices[port]['ucmd_queue'].put([type, term, cmd])
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
                self.service_socket.close()
                print 'connection to server lost, try reconnecting...'
                while True:
                    result = self.connect_to_server(server_ip, server_port)
                    if result == 'success':
                        self.send_packet(TBframe.CLIENT_UUID, self.uuid)
                        self.send_packet(TBframe.CLIENT_TAG, self.poll_str)
                        self.send_device_list()
                        break
                    time.sleep(1)
                print 'connection to server resumed'
            except KeyboardInterrupt:
                print "client exiting ..."
                self.keep_running = False
                time.sleep(0.3)
                break
            except:
                if DEBUG: traceback.print_exc()
        self.service_socket.close()
