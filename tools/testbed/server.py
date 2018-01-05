import os, sys, time, socket, ssl, signal, re
import thread, threading, json, traceback, shutil
import TBframe

MAX_MSG_LENTH = 8192
ENCRYPT_CLIENT = False
ENCRYPT_TERMINAL = False
DEBUG = True

def signal_handler(sig, frame):
    print "received SIGINT"
    raise KeyboardInterrupt

class Server:
    def __init__(self):
        self.keyfile = 'server_key.pem'
        self.certfile = 'server_cert.pem'
        self.client_socket = 0
        self.terminal_socket = 0
        self.client_list = []
        self.terminal_list = []
        self.conn_timeout = {}
        self.keep_running = True
        self.log_preserve_period = (7 * 24 * 3600) * 3 #save log for 3 weeks
        self.allocated = {'lock':threading.Lock(), 'devices':[], 'timeout':0}
        self.special_purpose_set = {'mk3060-alink':[], 'esp32-alink':[]}
        #mk3060
        self.special_purpose_set['mk3060-alink'] += ['DN02QRKQ', 'DN02RDVL', 'DN02RDVT', 'DN02RDVV']
        self.special_purpose_set['mk3060-alink'] += ['DN02X2ZO', 'DN02X2ZS', 'DN02X2ZX', 'DN02X2ZZ']
        self.special_purpose_set['mk3060-alink'] += ['DN02X303', 'DN02X304', 'DN02X30B', 'DN02X30H']
        self.special_purpose_set['mk3060-mesh'] = self.special_purpose_set['mk3060-alink']
        #esp32
        self.special_purpose_set['esp32-alink'] += ['espif-3.1.1', 'espif-3.1.2', 'espif-3.1.3', 'espif-3.1.4']
        self.special_purpose_set['esp32-alink'] += ['espif-3.2.1', 'espif-3.2.2', 'espif-3.2.3', 'espif-3.2.4']
        self.special_purpose_set['esp32-alink'] += ['espif-3.3.1', 'espif-3.3.2', 'espif-3.3.3', 'espif-3.3.4']
        self.special_purpose_set['esp32-mesh'] = self.special_purpose_set['esp32-alink']

    def construct_dev_list(self):
        l = []
        for client in self.client_list:
            if client['valid'] == False:
                continue
            devices = client['uuid']
            for port in client['devices']:
                if client['devices'][port]['valid'] == False:
                    continue
                devices += ',' + port + '|' + str(client['devices'][port]['using'])
            l.append(devices)
        data = ':'.join(l)
        return data

    def send_device_list_to_terminal(self, terminal):
        devs = self.construct_dev_list()
        data = TBframe.construct(TBframe.ALL_DEV, devs)
        terminal['socket'].send(data)

    def send_device_list_to_all(self):
        devs = self.construct_dev_list()
        data = TBframe.construct(TBframe.ALL_DEV, devs)
        for t in self.terminal_list:
            try:
                t['socket'].send(data)
            except:
                continue

    def client_serve_thread(self, conn, addr):
        file = {}
        msg = ''
        client = None
        self.conn_timeout[conn] = {'type':'client', 'addr': addr, 'timeout': time.time() + 30}
        while self.keep_running:
            try:
                new_msg = conn.recv(MAX_MSG_LENTH)
                if new_msg == '':
                    break

                msg += new_msg
                while msg != '':
                    type, length, value, msg = TBframe.parse(msg)
                    if type == TBframe.TYPE_NONE:
                        break

                    self.conn_timeout[conn]['timeout'] = time.time() + 30
                    if client == None:
                        if type != TBframe.CLIENT_UUID:
                            data = TBframe.construct(TBframe.CLIENT_UUID, 'give me your uuid first')
                            conn.send(data)
                            time.sleep(0.1)
                            continue
                        for c in self.client_list:
                            if c['uuid'] != value:
                                continue
                            client = c
                            break
                        if client == None:
                            client = {'uuid':value,
                                      'valid':True,
                                      'socket':conn,
                                      'addr':addr,
                                      'devices':{}}
                            self.client_list.append(client)
                            print "new client {0} connected @ {1}".format(value, addr)
                        else:
                            client['socket'] = conn
                            client['addr'] = addr
                            client['valid'] = True
                            print "client {0} re-connected @ {1}".format(value, addr)
                        continue

                    if type == TBframe.CLIENT_DEV:
                        new_devices = value.split(':')
                        for port in new_devices:
                            if port == "":
                                continue
                            if port in client['devices'] and client['devices'][port]['valid'] == True:
                                continue
                            if port not in client['devices']:
                                print "new device {0} added to client {1}".format(port, client['uuid'])
                                client['devices'][port] = {
                                        'lock':threading.Lock(),
                                        'valid':True,
                                        'using':0,
                                        'status':'{}',
                                        'log_subscribe':[],
                                        'status_subscribe':[]
                                        }
                            else:
                                print "device {0} re-added to client {1}".format(port, client['uuid'])
                                client['devices'][port]['status'] = '{}'
                                client['devices'][port]['valid'] = True

                        for port in list(client['devices']):
                            if port in new_devices:
                                continue
                            if client['devices'][port]['valid'] == False:
                                continue
                            client['devices'][port]['status'] = '{}'
                            client['devices'][port]['valid'] = False
                            print "device {0} removed from client {1}".format(port, client['uuid'])

                        for port in list(file):
                            if client['devices'][port]['valid'] == True:
                                continue
                            file[port]['handle'].close()
                            file.pop(port)
                        self.send_device_list_to_all()
                    elif type == TBframe.DEVICE_LOG:
                        port = value.split(':')[0]
                        if port not in client['devices']:
                            continue
                        #forwad log to subscribed devices
                        if client['devices'][port]['log_subscribe'] != [] and \
                           ('tag' not in client or client['tag'] not in value):
                            log = client['uuid'] + ',' + value
                            data = TBframe.construct(type, log)
                            for s in client['devices'][port]['log_subscribe']:
                                try:
                                    s.send(data)
                                except:
                                    continue

                        #save log to files
                        try:
                            logtime = value.split(':')[1]
                            logstr = value[len(port) + 1 + len(logtime):]
                            logtime = float(logtime)
                            logtimestr = time.strftime("%Y-%m-%d@%H:%M:%S", time.localtime(logtime))
                            logtimestr += ("{0:.3f}".format(logtime-int(logtime)))[1:]
                            logstr = logtimestr + logstr
                            logdatestr = time.strftime("%Y-%m-%d", time.localtime(logtime))
                        except:
                            if DEBUG: traceback.print_exc()
                            continue
                        if (port not in file) or (file[port]['date'] != logdatestr):
                            if port in file:
                                file[port]['handle'].close()
                                file.pop(port)
                            log_dir = 'server/' + logdatestr
                            if os.path.isdir(log_dir) == False:
                                try:
                                    os.mkdir(log_dir)
                                except:
                                    print "error: can not create directory {0}".format(log_dir)
                            filename = log_dir + '/' + client['uuid'] + '-' + port.split('/')[-1]  + '.log'
                            try:
                                handle = open(filename, 'a+')
                                file[port] = {'handle':handle, 'date': logdatestr}
                            except:
                                print "error: can not open/create file ", filename
                        if port in file and file[port]['date'] == logdatestr:
                            file[port]['handle'].write(logstr)
                    elif type == TBframe.DEVICE_STATUS:
                        #print value
                        port = value.split(':')[0]
                        if port not in client['devices']:
                            continue
                        client['devices'][port]['status'] = value[len(port)+1:]
                        if client['devices'][port]['status_subscribe'] != []:
                            log = client['uuid'] + ',' + port
                            log += value[len(port):]
                            data = TBframe.construct(type, log)
                            for s in client['devices'][port]['status_subscribe']:
                                try:
                                    s.send(data)
                                except:
                                    continue
                    elif type == TBframe.DEVICE_ERASE or type == TBframe.DEVICE_PROGRAM or \
                         type == TBframe.DEVICE_START or type == TBframe.DEVICE_STOP or \
                         type == TBframe.DEVICE_RESET or type == TBframe.DEVICE_CMD or \
                         type == TBframe.FILE_BEGIN or type == TBframe.FILE_DATA or \
                         type == TBframe.FILE_END:
                        values = value.split(',')
                        addr = (values[0], int(values[1]))
                        terminal = None
                        for t in self.terminal_list:
                            if t['addr'] == addr:
                                terminal = t
                        if terminal != None:
                            if values[2] != 'success' and values[2] != 'ok':
                                data = TBframe.construct(TBframe.CMD_ERROR, ','.join(values[2:]))
                            else:
                                data = TBframe.construct(TBframe.CMD_DONE, ','.join(values[2:]))
                            terminal['socket'].send(data)
                    elif type == TBframe.CLIENT_TAG:
                        client['tag'] = value
                        print 'client {0} tag: {1}'.format(client['uuid'],repr(value))
            except:
                if DEBUG: traceback.print_exc()
                break
        conn.close()
        self.conn_timeout.pop(conn)
        if client:
            for port in client['devices']:
                if client['devices'][port]['valid'] == False:
                    continue
                client['devices'][port]['status'] = '{}'
                client['devices'][port]['valid'] = False
                print "device {0} removed from client {1}".format(port, client['uuid'])
            client['valid'] = False
            print "client {0} @ {1} disconnected".format(client['uuid'], addr)
            self.send_device_list_to_all()
        else:
            print "client @ {0} disconnected".format(addr)

    def send_file_to_someone(self, dst, filename):
        if os.path.exists(filename) == False:
            print "{0} does not exist\n".format(filename)
            return False
        print "sending {0} to {1} ...".format(filename, dst['addr']),
        file = open(filename,'r')
        content = filename.split('/')[-1]
        data = TBframe.construct(TBframe.FILE_BEGIN, content)
        try:
            dst['socket'].send(data)
        except:
            print "failed"
            return False
        content = file.read(1024)
        while(content):
            data = TBframe.construct(TBframe.FILE_DATA, content)
            try:
                dst['socket'].send(data)
            except:
                print "failed"
                return False
            content = file.read(1024)
        file.close()
        content = filename.split('/')[-1]
        data = TBframe.construct(TBframe.FILE_END, content)
        try:
            dst['socket'].send(data)
        except:
            print "failed"
            return False
        print "succeed"
        return True

    def get_client_by_uuid(self, uuid):
        ret = None
        for client in self.client_list:
            if client['uuid'] != uuid:
                continue
            if client['valid'] == False:
                continue
            ret = client
            break
        return ret

    def allocate_devices(self, value):
        values = value.split(',')
        if len(values) < 2:
            return ['error','argument']

        model = values[0]
        model = model.lower()

        number = values[1]
        try:
            number = int(number)
        except:
            return ['error','argument']
        if number <= 0:
            return ['error','argument']

        purpose = 'general'
        if len(values) > 2:
            purpose = values[2]
        func_set = None
        if purpose != 'general':
            func_set = model + '-' + purpose
            if func_set not in self.special_purpose_set:
                print "error: allocate {0} for {1} purpose not supported".format(model, purpose)
                return ['error','argument']
            func_set = self.special_purpose_set[func_set]
        if DEBUG and func_set: print purpose, func_set

        allocated = []
        with self.allocated['lock']:
            for client in self.client_list:
                allocated = []
                ports = list(client['devices'])
                ports.sort()
                for port in ports:
                    if client['devices'][port]['valid'] == False: #no exist
                        continue

                    if client['devices'][port]['using'] != 0: #busy
                        continue

                    if port in self.allocated['devices']: #in allocated buffer
                        continue

                    try:
                        status = json.loads(client['devices'][port]['status'])
                    except:
                        print 'parse {0} status failed'.format(port)
                        status = None
                    if status:
                        if 'model' not in status or model != status['model'].lower():
                            continue
                    else:
                        paths = {'mk3060':'mxchip', 'esp32':'espif'}
                        if model not in paths:
                            continue
                        pathstr = paths[model]
                        if pathstr not in port:
                            continue

                    if func_set:
                        match = False
                        for devicestr in func_set:
                            if devicestr not in port:
                                continue
                            match = True
                            break
                        if match == False:
                            continue

                    allocated.append(port)
                    if len(allocated) >= number:
                        break
                if len(allocated) >= number:
                    break
            if len(allocated) >= number:
                self.allocated['devices'] += allocated
                self.allocated['timeout'] = time.time() + 10
                if DEBUG: print "allocated", allocated
                return ['success', '|'.join(allocated)]
            else:
                if DEBUG: print "allocate failed"
                return ['fail', 'busy']

    def increase_device_refer(self, client, port, using_list):
        if [client['uuid'], port] not in using_list:
            if port in list(client['devices']):
                with client['devices'][port]['lock']:
                    client['devices'][port]['using'] += 1
                using_list.append([client['uuid'], port])
                self.send_device_list_to_all()

    def terminal_serve_thread(self, conn, addr):
        terminal = {'socket':conn, 'addr':addr}
        self.terminal_list.append(terminal)
        using_list = []
        msg = ''
        self.conn_timeout[conn] = {'type': 'terminal', 'addr': addr, 'timeout': time.time() + 30}
        self.send_device_list_to_terminal(terminal)
        while self.keep_running:
            try:
                new_msg = terminal['socket'].recv(MAX_MSG_LENTH);
                if new_msg == '':
                    break

                msg += new_msg
                while msg != '':
                    type, length, value, msg = TBframe.parse(msg)
                    if type == TBframe.TYPE_NONE:
                        break

                    self.conn_timeout[conn]['timeout'] = time.time() + 30
                    if type == TBframe.FILE_BEGIN or type == TBframe.FILE_DATA or type == TBframe.FILE_END:
                        dev_str = value.split(':')[0]
                        uuid = dev_str.split(',')[0]
                        target_data = value[len(dev_str):]
                        client = self.get_client_by_uuid(uuid)
                        if client == None:
                            data = TBframe.construct(TBframe.CMD_ERROR, 'nonexist')
                            terminal['socket'].send(data)
                            continue
                        content  = terminal['addr'][0] + ',' + str(terminal['addr'][1])
                        content += target_data
                        data = TBframe.construct(type, content)
                        client['socket'].send(data)
                    elif type == TBframe.DEVICE_ERASE or type == TBframe.DEVICE_PROGRAM or \
                         type == TBframe.DEVICE_START or type == TBframe.DEVICE_STOP or \
                         type == TBframe.DEVICE_RESET or type == TBframe.DEVICE_CMD:
                        dev_str_split = value.split(':')[0].split(',')[0:2]
                        if len(dev_str_split) != 2:
                            data = TBframe.construct(TBframe.CMD_ERROR,'argerror')
                            terminal['socket'].send(data)
                            continue
                        [uuid, port] = dev_str_split
                        client = self.get_client_by_uuid(uuid)
                        if client == None:
                            data = TBframe.construct(TBframe.CMD_ERROR,'nonexist')
                            terminal['socket'].send(data)
                            continue
                        content = terminal['addr'][0]
                        content += ',' + str(terminal['addr'][1])
                        content += value[len(uuid):]
                        data = TBframe.construct(type, content)
                        client['socket'].send(data)
                        self.increase_device_refer(client, port, using_list)
                    elif type == TBframe.DEVICE_ALLOC:
                        result = self.allocate_devices(value)
                        content = ','.join(result)
                        data = TBframe.construct(TBframe.DEVICE_ALLOC, content)
                        terminal['socket'].send(data)
                    elif type == TBframe.LOG_SUB or type == TBframe.LOG_UNSUB or \
                         type == TBframe.STATUS_SUB or type == TBframe.STATUS_UNSUB:
                        dev_str_split = value.split(',')
                        if len(dev_str_split) != 2:
                            continue
                        [uuid, port] = dev_str_split
                        client = self.get_client_by_uuid(uuid)
                        if client == None:
                            continue
                        if port not in list(client['devices']):
                            continue
                        if type == TBframe.LOG_SUB:
                            if terminal['socket'] in client['devices'][port]['log_subscribe']:
                                continue
                            client['devices'][port]['log_subscribe'].append(terminal['socket'])
                            print "terminal {0}:{1}".format(terminal['addr'][0], terminal['addr'][1]),
                            print "subscribed log of device {0}:{1}".format(uuid, port)
                        elif type == TBframe.LOG_UNSUB:
                            if terminal['socket'] not in client['devices'][port]['log_subscribe']:
                                continue
                            client['devices'][port]['log_subscribe'].remove(terminal['socket'])
                            print "terminal {0}:{1}".format(terminal['addr'][0], terminal['addr'][1]),
                            print "unsubscribed log of device {0}:{1}".format(uuid, port)
                        elif type == TBframe.STATUS_SUB:
                            if terminal['socket'] in client['devices'][port]['status_subscribe']:
                                continue
                            client['devices'][port]['status_subscribe'].append(terminal['socket'])
                            print "terminal {0}:{1}".format(terminal['addr'][0], terminal['addr'][1]),
                            print "subscribed status of device {0}:{1}".format(uuid, port)
                            content = client['uuid'] + ',' + port
                            content += ':' + client['devices'][port]['status']
                            data = TBframe.construct(TBframe.DEVICE_STATUS, content)
                            terminal['socket'].send(data)
                        elif type == TBframe.STATUS_UNSUB:
                            if terminal['socket'] not in client['devices'][port]['status_subscribe']:
                                continue
                            client['devices'][port]['status_subscribe'].remove(terminal['socket'])
                            print "terminal {0}:{1}".format(terminal['addr'][0], terminal['addr'][1]),
                            print "unsubscribed status of device {0}:{1}".format(uuid, port)
                    elif type == TBframe.LOG_DOWNLOAD:
                        dev_str_split = value.split(',')
                        if len(dev_str_split) != 2:
                            continue
                        [uuid, port] = dev_str_split
                        datestr = time.strftime('%Y-%m-%d')
                        filename = 'server/' + datestr + '/' + uuid + '-' + port.split('/')[-1] + '.log'
                        client = self.get_client_by_uuid(uuid)
                        if client == None or port not in list(client['devices']) or os.path.exists(filename) == False:
                            data = TBframe.construct(TBframe.CMD_ERROR,'fail')
                            terminal['socket'].send(data)
                            print "terminal {0}:{1}".format(terminal['addr'][0], terminal['addr'][1]),
                            print "downloading log of device {0}:{1} ... failed".format(uuid, port)
                            continue
                        self.send_file_to_someone(terminal, filename)
                        heartbeat_timeout = time.time() + 30
                        data = TBframe.construct(TBframe.CMD_DONE, 'success')
                        terminal['socket'].send(data)
                        print "terminal {0}:{1}".format(terminal['addr'][0], terminal['addr'][1]),
                        print "downloading log of device {0}:{1} ... succeed".format(uuid, port)
            except:
                if DEBUG: traceback.print_exc()
                break
        for client in self.client_list:
            for port in list(client['devices']):
                if terminal['socket'] in client['devices'][port]['log_subscribe']:
                    client['devices'][port]['log_subscribe'].remove(terminal['socket'])
                if terminal['socket'] in client['devices'][port]['status_subscribe']:
                    client['devices'][port]['status_subscribe'].remove(terminal['socket'])
        for device in using_list:
            uuid = device[0]
            port = device[1]
            client = None
            for c in self.client_list:
                if c['uuid'] != uuid:
                    continue
                client = c
                break
            if client != None and port in list(client['devices']):
                with client['devices'][port]['lock']:
                    if client['devices'][port]['using'] > 0:
                        client['devices'][port]['using'] -= 1
        terminal['socket'].close()
        self.conn_timeout.pop(conn)
        print "terminal ", terminal['addr'], "disconnected"
        self.terminal_list.remove(terminal)
        self.send_device_list_to_all()

    def client_listen_thread(self):
        self.client_socket.listen(5)
        if ENCRYPT_CLIENT:
            self.client_socket = ssl.wrap_socket(self.client_socket, self.keyfile, self.certfile, True)
        while self.keep_running:
            try:
                (conn, addr) = self.client_socket.accept()
                thread.start_new_thread(self.client_serve_thread, (conn, addr,))
            except:
                traceback.print_exc()

    def terminal_listen_thread(self):
        self.terminal_socket.listen(5)
        if ENCRYPT_TERMINAL:
            self.terminal_socket = ssl.wrap_socket(self.terminal_socket, self.keyfile, self.certfile, True)
        while self.keep_running:
            try:
                (conn, addr) = self.terminal_socket.accept()
                thread.start_new_thread(self.terminal_serve_thread, (conn, addr,))
                print "terminal ", addr," connected"
            except:
                traceback.print_exc()

    def statistics_thread(self):
        minute = time.strftime("%Y-%m-%d@%H:%M")
        datestr = time.strftime("%Y-%m-%d")
        statistics={ \
                'terminal_num_max':0, \
                'client_num_max':0, \
                'device_num_max':0, \
                'device_use_max':0, \
                'terminal_num_avg':0, \
                'client_num_avg':0, \
                'device_num_avg':0, \
                'device_use_avg':0 \
                }
        statistics_cnt = 0
        logname='statistics.log'
        try:
            f = open(logname, 'a+')
        except:
            print "error: unable to create/open {0}".format(logname)
            return
        while self.keep_running:
            time.sleep(3)

            #remove outdated log files
            if time.strftime("%Y-%m-%d") != datestr:
                tbefore = time.mktime(time.strptime(time.strftime('%Y-%m-%d'), '%Y-%m-%d'))
                tbefore -= self.log_preserve_period
                flist = os.listdir('server')
                for fname in flist:
                    if os.path.isdir('server/'+ fname) == False:
                        continue
                    if re.match('[0-9]{4}-[0-9]{2}-[0-9]{2}', fname) == None:
                        continue
                    ftime = time.strptime(fname, '%Y-%m-%d')
                    ftime = time.mktime(ftime)
                    if ftime >= tbefore:
                        continue
                    shutil.rmtree('server/' + fname)
                datestr = time.strftime("%Y-%m-%d")

            #disconnect timeout connections
            now = time.time()
            for conn in list(self.conn_timeout):
                if now <= self.conn_timeout[conn]['timeout']:
                    continue
                conn.close()
                print self.conn_timeout[conn]['type'], self.conn_timeout[conn]['addr'], 'timeout'
                self.conn_timeout.pop(conn)

            #generate and save statistics data
            client_cnt = 0
            device_cnt = 0
            device_use = 0
            terminal_cnt = len(self.terminal_list)
            for client in self.client_list:
                if client['valid'] == False:
                    continue
                client_cnt += 1
                for port in client['devices']:
                    if client['devices'][port]['valid'] == False:
                        continue
                    device_cnt += 1
                    if client['devices'][port]['using'] > 0:
                        device_use += 1
            if terminal_cnt > statistics['terminal_num_max']:
                statistics['terminal_num_max'] = terminal_cnt
            if client_cnt > statistics['client_num_max']:
                statistics['client_num_max'] = client_cnt
            if device_cnt > statistics['device_num_max']:
                statistics['device_num_max'] = device_cnt
            if device_use > statistics['device_use_max']:
                statistics['device_use_max'] = device_use
            statistics['terminal_num_avg'] += terminal_cnt
            statistics['client_num_avg'] += client_cnt
            statistics['device_num_avg'] += device_cnt
            statistics['device_use_avg'] += device_use
            statistics_cnt += 1.0

            now = time.strftime("%Y-%m-%d@%H:%M")
            if now == minute:
                continue
            statistics['terminal_num_avg'] = round(statistics['terminal_num_avg']/statistics_cnt, 2)
            statistics['client_num_avg'] = round(statistics['client_num_avg']/statistics_cnt, 2)
            statistics['device_num_avg'] = round(statistics['device_num_avg']/statistics_cnt, 2)
            statistics['device_use_avg'] = round(statistics['device_use_avg']/statistics_cnt, 2)
            data = json.dumps({minute:statistics}, sort_keys=True) + '\n'
            f.write(data)
            f.flush()
            minute = now
            statistics={ \
                    'terminal_num_max':0, \
                    'client_num_max':0, \
                    'device_num_max':0, \
                    'device_use_max':0, \
                    'terminal_num_avg':0, \
                    'client_num_avg':0, \
                    'device_num_avg':0, \
                    'device_use_avg':0 \
                    }
            statistics_cnt = 0
            if os.path.isfile(logname) == True:
                continue
            try:
                f.close()
                f = open(logname, 'a+')
            except:
                print "error: unable to create/open {0}".format(logname)
                return

    def init(self, server_port):
        try:
            #initilize CLIENT socket
            self.client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            self.client_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
            self.client_socket.bind(('', server_port))
            #initilize TERMINAL socket
            self.terminal_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            self.terminal_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
            self.terminal_socket.bind(('', server_port + 1))
        except:
            print "address still in use, try later"
            return "fail"
        if os.path.exists('server') == False:
            os.mkdir('server')
        return "success"

    def run(self):
        signal.signal(signal.SIGINT, signal_handler)
        try:
            thread.start_new_thread(self.client_listen_thread, ())
            thread.start_new_thread(self.terminal_listen_thread, ())
            thread.start_new_thread(self.statistics_thread, ())
            while True:
                time.sleep(0.1)
                if self.allocated['devices'] != [] and time.time() > self.allocated['timeout']:
                    with self.allocated['lock']:
                        self.allocated['devices'] = []
        except:
            print "server exiting ..."
            self.keep_running = False

    def deinit(self):
        for c in self.client_list:
            if c['valid'] == False:
                continue
            c['socket'].close()
        for t in self.terminal_list:
            t['socket'].close()
        try:
            self.client_socket.close()
            self.terminal_socket.close()
        except:
            pass

    def server_func(self, server_port):
        if self.init(server_port) == "success":
            self.run()
        self.deinit()
