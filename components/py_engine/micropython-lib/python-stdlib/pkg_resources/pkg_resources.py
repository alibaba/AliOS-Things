import uio

c = {}


def resource_stream(package, resource):
    if package not in c:
        try:
            if package:
                p = __import__(package + ".R", None, None, True)
            else:
                p = __import__("R")
            c[package] = p.R
        except ImportError:
            if package:
                p = __import__(package)
                d = p.__path__
            else:
                d = "."
            #            if d[0] != "/":
            #                import uos
            #                d = uos.getcwd() + "/" + d
            c[package] = d + "/"

    p = c[package]
    if isinstance(p, dict):
        return uio.BytesIO(p[resource])
    return open(p + resource, "rb")
