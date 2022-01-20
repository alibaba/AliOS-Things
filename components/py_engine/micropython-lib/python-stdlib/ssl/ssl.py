from ussl import *
import ussl as _ussl

# Constants
for sym in "CERT_NONE", "CERT_OPTIONAL", "CERT_REQUIRED":
    if sym not in globals():
        globals()[sym] = object()


def wrap_socket(
    sock,
    keyfile=None,
    certfile=None,
    server_side=False,
    cert_reqs=CERT_NONE,
    *,
    ca_certs=None,
    server_hostname=None
):
    # TODO: More arguments accepted by CPython could also be handled here.
    # That would allow us to accept ca_certs as a positional argument, which
    # we should.
    kw = {}
    if keyfile is not None:
        kw["keyfile"] = keyfile
    if certfile is not None:
        kw["certfile"] = certfile
    if server_side is not False:
        kw["server_side"] = server_side
    if cert_reqs is not CERT_NONE:
        kw["cert_reqs"] = cert_reqs
    if ca_certs is not None:
        kw["ca_certs"] = ca_certs
    if server_hostname is not None:
        kw["server_hostname"] = server_hostname
    return _ussl.wrap_socket(sock, **kw)
