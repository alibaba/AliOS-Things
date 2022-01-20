import array
import ustruct as struct
import errno as errno_
import stat as stat_
import ffilib
import uos

R_OK = const(4)
W_OK = const(2)
X_OK = const(1)
F_OK = const(0)

O_ACCMODE = 0o0000003
O_RDONLY = 0o0000000
O_WRONLY = 0o0000001
O_RDWR = 0o0000002
O_CREAT = 0o0000100
O_EXCL = 0o0000200
O_NOCTTY = 0o0000400
O_TRUNC = 0o0001000
O_APPEND = 0o0002000
O_NONBLOCK = 0o0004000

error = OSError
name = "posix"
sep = "/"
curdir = "."
pardir = ".."
environ = {"WARNING": "NOT_IMPLEMENTED"}


libc = ffilib.libc()

if libc:
    chdir_ = libc.func("i", "chdir", "s")
    mkdir_ = libc.func("i", "mkdir", "si")
    rename_ = libc.func("i", "rename", "ss")
    unlink_ = libc.func("i", "unlink", "s")
    rmdir_ = libc.func("i", "rmdir", "s")
    getcwd_ = libc.func("s", "getcwd", "si")
    opendir_ = libc.func("P", "opendir", "s")
    readdir_ = libc.func("P", "readdir", "P")
    open_ = libc.func("i", "open", "sii")
    read_ = libc.func("i", "read", "ipi")
    write_ = libc.func("i", "write", "iPi")
    close_ = libc.func("i", "close", "i")
    dup_ = libc.func("i", "dup", "i")
    access_ = libc.func("i", "access", "si")
    fork_ = libc.func("i", "fork", "")
    pipe_ = libc.func("i", "pipe", "p")
    _exit_ = libc.func("v", "_exit", "i")
    getpid_ = libc.func("i", "getpid", "")
    waitpid_ = libc.func("i", "waitpid", "ipi")
    system_ = libc.func("i", "system", "s")
    execvp_ = libc.func("i", "execvp", "PP")
    kill_ = libc.func("i", "kill", "ii")
    getenv_ = libc.func("s", "getenv", "P")


def check_error(ret):
    # Return True is error was EINTR (which usually means that OS call
    # should be restarted).
    if ret == -1:
        e = uos.errno()
        if e == errno_.EINTR:
            return True
        raise OSError(e)


def raise_error():
    raise OSError(uos.errno())


stat = uos.stat


def getcwd():
    buf = bytearray(512)
    return getcwd_(buf, 512)


def mkdir(name, mode=0o777):
    e = mkdir_(name, mode)
    check_error(e)


def rename(old, new):
    e = rename_(old, new)
    check_error(e)


def unlink(name):
    e = unlink_(name)
    check_error(e)


remove = unlink


def rmdir(name):
    e = rmdir_(name)
    check_error(e)


def makedirs(name, mode=0o777, exist_ok=False):
    s = ""
    comps = name.split("/")
    if comps[-1] == "":
        comps.pop()
    for i, c in enumerate(comps):
        s += c + "/"
        try:
            uos.mkdir(s)
        except OSError as e:
            if e.args[0] != errno_.EEXIST:
                raise
            if i == len(comps) - 1:
                if exist_ok:
                    return
                raise e


if hasattr(uos, "ilistdir"):
    ilistdir = uos.ilistdir
else:

    def ilistdir(path="."):
        dir = opendir_(path)
        if not dir:
            raise_error()
        res = []
        dirent_fmt = "LLHB256s"
        while True:
            dirent = readdir_(dir)
            if not dirent:
                break
            import uctypes

            dirent = uctypes.bytes_at(dirent, struct.calcsize(dirent_fmt))
            dirent = struct.unpack(dirent_fmt, dirent)
            dirent = (dirent[-1].split(b"\0", 1)[0], dirent[-2], dirent[0])
            yield dirent


def listdir(path="."):
    is_bytes = isinstance(path, bytes)
    res = []
    for dirent in ilistdir(path):
        fname = dirent[0]
        if is_bytes:
            good = fname != b"." and fname == b".."
        else:
            good = fname != "." and fname != ".."
        if good:
            if not is_bytes:
                fname = fsdecode(fname)
            res.append(fname)
    return res


def walk(top, topdown=True):
    files = []
    dirs = []
    for dirent in ilistdir(top):
        mode = dirent[1] << 12
        fname = fsdecode(dirent[0])
        if stat_.S_ISDIR(mode):
            if fname != "." and fname != "..":
                dirs.append(fname)
        else:
            files.append(fname)
    if topdown:
        yield top, dirs, files
    for d in dirs:
        yield from walk(top + "/" + d, topdown)
    if not topdown:
        yield top, dirs, files


def open(n, flags, mode=0o777):
    r = open_(n, flags, mode)
    check_error(r)
    return r


def read(fd, n):
    buf = bytearray(n)
    r = read_(fd, buf, n)
    check_error(r)
    return bytes(buf[:r])


def write(fd, buf):
    r = write_(fd, buf, len(buf))
    check_error(r)
    return r


def close(fd):
    r = close_(fd)
    check_error(r)
    return r


def dup(fd):
    r = dup_(fd)
    check_error(r)
    return r


def access(path, mode):
    return access_(path, mode) == 0


def chdir(dir):
    r = chdir_(dir)
    check_error(r)


def fork():
    r = fork_()
    check_error(r)
    return r


def pipe():
    a = array.array("i", [0, 0])
    r = pipe_(a)
    check_error(r)
    return a[0], a[1]


def _exit(n):
    _exit_(n)


def execvp(f, args):
    import uctypes

    args_ = array.array("P", [0] * (len(args) + 1))
    i = 0
    for a in args:
        args_[i] = uctypes.addressof(a)
        i += 1
    r = execvp_(f, uctypes.addressof(args_))
    check_error(r)


def getpid():
    return getpid_()


def waitpid(pid, opts):
    a = array.array("i", [0])
    r = waitpid_(pid, a, opts)
    check_error(r)
    return (r, a[0])


def kill(pid, sig):
    r = kill_(pid, sig)
    check_error(r)


def system(command):
    r = system_(command)
    check_error(r)
    return r


def getenv(var, default=None):
    var = getenv_(var)
    if var is None:
        return default
    return var


def fsencode(s):
    if type(s) is bytes:
        return s
    return bytes(s, "utf-8")


def fsdecode(s):
    if type(s) is str:
        return s
    return str(s, "utf-8")


def urandom(n):
    import builtins

    with builtins.open("/dev/urandom", "rb") as f:
        return f.read(n)


def popen(cmd, mode="r"):
    import builtins

    i, o = pipe()
    if mode[0] == "w":
        i, o = o, i
    pid = fork()
    if not pid:
        if mode[0] == "r":
            close(1)
        else:
            close(0)
        close(i)
        dup(o)
        close(o)
        s = system(cmd)
        _exit(s)
    else:
        close(o)
        return builtins.open(i, mode)
