import ffi
import sys


_h = None

names = ("libc.so", "libc.so.0", "libc.so.6", "libc.dylib")


def get():
    global _h
    if _h:
        return _h
    err = None
    for n in names:
        try:
            _h = ffi.open(n)
            return _h
        except OSError as e:
            err = e
    raise err


def set_names(n):
    global names
    names = n


# Find out bitness of the platform, even if long ints are not supported
# TODO: All bitness differences should be removed from micropython-lib, and
# this snippet too.
bitness = 1
v = sys.maxsize
while v:
    bitness += 1
    v >>= 1
