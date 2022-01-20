import os
import pickle
import select


class Process:
    def __init__(self, group=None, target=None, name=None, args=(), kwargs={}):
        self.target = target
        self.args = args
        self.kwargs = kwargs
        self.pid = 0
        self.r = self.w = None

    def start(self):
        self.pid = os.fork()
        if not self.pid:
            if self.r:
                self.r.close()
            self.target(*self.args, **self.kwargs)
            os._exit(0)
        else:
            if self.w:
                self.w.close()
            return

    def join(self):
        os.waitpid(self.pid, 0)

    def register_pipe(self, r, w):
        """Extension to CPython API: any pipe used for parent/child
        communication should be registered with this function."""
        self.r, self.w = r, w


class Connection:
    def __init__(self, fd):
        self.fd = fd
        self.f = open(fd)

    def __repr__(self):
        return "<Connection %s>" % self.f

    def send(self, obj):
        s = pickle.dumps(obj)
        self.f.write(len(s).to_bytes(4, "little"))
        self.f.write(s)

    def recv(self):
        s = self.f.read(4)
        if not s:
            raise EOFError
        l = int.from_bytes(s, "little")
        s = self.f.read(l)
        if not s:
            raise EOFError
        return pickle.loads(s)

    def close(self):
        self.f.close()


def Pipe(duplex=True):
    assert duplex == False
    r, w = os.pipe()
    return Connection(r), Connection(w)


class AsyncResult:
    def __init__(self, p, r):
        self.p = p
        self.r = r
        self.ep = None

    def get(self):
        res = self.r.recv()
        self.p.join()
        return res

    def ready(self):
        if not self.ep:
            self.ep = select.epoll()
            self.ep.register(self.r.f.fileno(), select.EPOLLIN, None)
        res = self.ep.poll(0)
        if res:
            self.ep.close()
        return bool(res)


class Pool:
    def __init__(self, num):
        self.num = num

    def _apply(self, f, args, kwargs):
        # This is pretty inefficient impl, doesn't really use pool worker
        def _exec(w):
            r = f(*args, **kwargs)
            w.send(r)

        r, w = Pipe(False)
        p = Process(target=_exec, args=(w,))
        p.register_pipe(r, w)
        p.start()
        return p, r

    def apply(self, f, args=(), kwargs={}):
        p, r = self._apply(f, args, kwargs)
        res = r.recv()
        p.join()
        return res

    def apply_async(self, f, args=(), kwargs={}, callback=None, errback=None):
        p, r = self._apply(f, args, kwargs)
        return AsyncResult(p, r)
