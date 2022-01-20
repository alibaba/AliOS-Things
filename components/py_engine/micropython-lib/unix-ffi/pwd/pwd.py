import ffilib
import uctypes
import ustruct

from ucollections import namedtuple


libc = ffilib.libc()

getpwnam_ = libc.func("P", "getpwnam", "s")


struct_passwd = namedtuple(
    "struct_passwd", ["pw_name", "pw_passwd", "pw_uid", "pw_gid", "pw_gecos", "pw_dir", "pw_shell"]
)


def getpwnam(user):
    passwd = getpwnam_(user)
    if not passwd:
        raise KeyError("getpwnam(): name not found: {}".format(user))
    passwd_fmt = "SSIISSS"
    passwd = uctypes.bytes_at(passwd, ustruct.calcsize(passwd_fmt))
    passwd = ustruct.unpack(passwd_fmt, passwd)
    return struct_passwd(*passwd)
