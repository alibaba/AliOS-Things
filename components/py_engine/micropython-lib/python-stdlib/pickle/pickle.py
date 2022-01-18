HIGHEST_PROTOCOL = 0


def dump(obj, f, proto=0):
    f.write(repr(obj))


def dumps(obj, proto=0):
    return repr(obj).encode()


def load(f):
    s = f.read()
    return loads(s)


def loads(s):
    d = {}
    s = s.decode()
    if "(" in s:
        qualname = s.split("(", 1)[0]
        if "." in qualname:
            pkg = qualname.rsplit(".", 1)[0]
            mod = __import__(pkg)
            d[pkg] = mod
    return eval(s, d)
