import sys

try:
    import ffi
except ImportError:
    ffi = None

_cache = {}


def open(name, maxver=10, extra=()):
    if not ffi:
        return None
    try:
        return _cache[name]
    except KeyError:
        pass

    def libs():
        if sys.platform == "linux":
            yield "%s.so" % name
            for i in range(maxver, -1, -1):
                yield "%s.so.%u" % (name, i)
        else:
            for ext in ("dylib", "dll"):
                yield "%s.%s" % (name, ext)
        for n in extra:
            yield n

    err = None
    for n in libs():
        try:
            l = ffi.open(n)
            _cache[name] = l
            return l
        except OSError as e:
            err = e
    raise err


def libc():
    return open("libc", 6)


# Find out bitness of the platform, even if long ints are not supported
# TODO: All bitness differences should be removed from micropython-lib, and
# this snippet too.
bitness = 1
v = sys.maxsize
while v:
    bitness += 1
    v >>= 1
