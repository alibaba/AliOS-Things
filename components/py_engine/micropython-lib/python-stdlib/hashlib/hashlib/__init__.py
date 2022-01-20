try:
    import uhashlib
except ImportError:
    uhashlib = None


def init():
    for i in ("sha1", "sha224", "sha256", "sha384", "sha512"):
        c = getattr(uhashlib, i, None)
        if not c:
            c = __import__("_" + i, None, None, (), 1)
            c = getattr(c, i)
        globals()[i] = c


init()


def new(algo, data=b""):
    try:
        c = globals()[algo]
        return c(data)
    except KeyError:
        raise ValueError(algo)
