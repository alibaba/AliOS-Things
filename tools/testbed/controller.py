import sys, time, socket, ssl, signal, select, thread
import TBframe

CONFIG_TIMEOUT = 30
CONFIG_MAXMSG_LENTH = 8192
CONFIG_ENCRYPT = True

def signal_handler(sig, frame):
    print "received SIGINT"
    raise KeyboardInterrupt

class selector():
    EVENT_READ = 0
    EVENT_WRITE = 1
    EVENT_ERROR = 2
    def __init__(self):
        self.read_map = {}
        self.write_map = {}
        self.error_map = {}

    def register(self, fd, type, callback):
        types = {self.EVENT_READ:self.read_map, self.EVENT_WRITE:self.write_map, self.EVENT_ERROR: self.error_map}
        if type not in types:
            return
        map = types[type]
        map[fd] = callback

    def unregister(self, fd, type):
        types = {self.EVENT_READ:self.read_map, self.EVENT_WRITE:self.write_map, self.EVENT_ERROR: self.error_map}
        if type not in types:
            return
        map = types[type]
        if fd in map: map.pop(fd)

    def select(self):
        ret = []
        r, w, e = select.select(list(self.read_map), list(self.write_map), list(self.error_map))
        for fd in r:
            ret.append([fd, self.read_map[fd]])
        for fd in w:
            ret.append([fd, self.write_map[fd]])
        for fd in e:
            ret.append([fd, self.error_map[fd]])
        return ret


class Controller():
    def __init__(self, host, port):
        self.host = host
        self.port = port
        self.clients = {}
        self.servers = {}
        self.terminals = {}
        self.timeouts = {}
        self.keyfile = 'server_key.pem'
        self.certfile = 'server_cert.pem'

    def accept(self, sel, sock, type):
        funcs = {'client':self.client_serve, 'server':self.server_serve, 'terminal':self.terminal_serve}
        clist = {'client':self.clients, 'server':self.servers, 'terminal':self.terminals}
        conn, addr = sock.accept()
        if type not in funcs:
            print "error: unsupported type {0}".format(type)
            conn.close()
            return
        print('{0} {1} connected'.format(type, addr))
        conn.setblocking(False)
        sel.register(conn, selector.EVENT_READ, funcs[type])
        self.timeouts[sel][conn] = time.time() + CONFIG_TIMEOUT
        clist[type][conn] = {}

    def client_serve(self, sel, sock, type):
        data = sock.recv(CONFIG_MAXMSG_LENTH)
        if not data:
            print("client {0} disconnect".format(sock.getpeername()))
            sel.unregister(sock, selector.EVENT_READ)
            sock.close()
            self.timeouts[sel].pop(sock)
            self.clients.pop(sock)
            return
        self.timeouts[sel][sock] = time.time() + CONFIG_TIMEOUT
        print type, data

    def server_serve(self, sel, sock, type):
        data = sock.recv(CONFIG_MAXMSG_LENTH)
        if not data:
            print("server {0} disconnect".format(sock.getpeername()))
            sel.unregister(sock, selector.EVENT_READ)
            sock.close()
            self.timeouts[sel].pop(sock)
            self.servers.pop(sock)
            return
        self.timeouts[sel][sock] = time.time() + CONFIG_TIMEOUT
        print type, data

    def terminal_serve(self, sel, sock, type):
        data = sock.recv(CONFIG_MAXMSG_LENTH)
        if not data:
            print("terminal {0} disconnect".format(sock.getpeername()))
            sel.unregister(sock, selector.EVENT_READ)
            sock.close()
            self.timeouts[sel].pop(sock)
            self.terminals.pop(sock)
            return
        self.timeouts[sel][sock] = time.time() + CONFIG_TIMEOUT
        print type, data

    def service_thread(self, port, type):
        types = {'client':self.clients, 'server':self.servers, 'terminal':self.terminals}
        if type not in types:
            return
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        sock.setblocking(False)
        sock.bind((self.host, port))
        if CONFIG_ENCRYPT:
            sock = ssl.wrap_socket(sock, self.keyfile, self.certfile, True)
        sock.listen(100)
        sel = selector()
        sel.register(sock, selector.EVENT_READ, self.accept)
        self.timeouts[sel] = {}
        while self.keep_running:
            events = sel.select()
            for [sock, callback] in events:
                callback(sel, sock, type)

            #close timeout connections
            now = time.time()
            conlist = types[type]
            for conn in list(self.timeouts[sel]):
                if now < self.timeouts[sel][conn]:
                    continue
                sel.unregister(conn, selector.EVENT_READ)
                conn.close()
                self.timeouts[sel].pop(conn)
                conlist.pop(conn)

    def run(self):
        signal.signal(signal.SIGINT, signal_handler)
        self.keep_running = True
        thread.start_new_thread(self.service_thread, (self.port, 'client',))
        thread.start_new_thread(self.service_thread, (self.port + 2, 'server',))
        thread.start_new_thread(self.service_thread, (self.port + 1, 'terminal',))
        while True:
            try:
                time.sleep(1)
            except:
                self.keep_running = False
                break

if __name__ == '__main__':
    host = ''
    port = '34567'
    if len(sys.argv) > 1:
        host = sys.argv[1]
    if len(sys.argv) > 2:
        port = sys.argv[2]
    cntr = Controller(host, 34567)
    cntr.run()
    sys.exit(0)
