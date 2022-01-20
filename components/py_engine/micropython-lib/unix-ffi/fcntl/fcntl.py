import ffi
import os
import ffilib


libc = ffilib.libc()

fcntl_l = libc.func("i", "fcntl", "iil")
fcntl_s = libc.func("i", "fcntl", "iip")
ioctl_l = libc.func("i", "ioctl", "iil")
ioctl_s = libc.func("i", "ioctl", "iip")


def fcntl(fd, op, arg=0):
    if type(arg) is int:
        r = fcntl_l(fd, op, arg)
        os.check_error(r)
        return r
    else:
        r = fcntl_s(fd, op, arg)
        os.check_error(r)
        # TODO: Not compliant. CPython says that arg should be immutable,
        # and possibly mutated buffer is returned.
        return r


def ioctl(fd, op, arg=0, mut=False):
    if type(arg) is int:
        r = ioctl_l(fd, op, arg)
        os.check_error(r)
        return r
    else:
        # TODO
        assert mut
        r = ioctl_s(fd, op, arg)
        os.check_error(r)
        return r
