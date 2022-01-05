from __future__ import print_function

import re
import sys

import platform
if platform.python_version_tuple()[0] == '2':
    bytes_cons = lambda val, enc=None: bytearray(val)
    from htmlentitydefs import codepoint2name
elif platform.python_version_tuple()[0] == '3':
    bytes_cons = bytes
    from html.entities import codepoint2name
# end compatibility code

# this must match the equivalent function in qstr.c
def compute_hash(qstr, bytes_hash):
    hash = 5381
    for b in qstr:
        hash = (hash * 33) ^ b
    # Make sure that valid hash is never zero, zero means "hash not computed"
    return (hash & ((1 << (8 * bytes_hash)) - 1)) or 1

def make_bytes(cfg_bytes_len, cfg_bytes_hash, qstr):
    qbytes = bytes_cons(qstr, "utf8")
    qlen = len(qbytes)
    qhash = compute_hash(qbytes, cfg_bytes_hash)
    if all(32 <= ord(c) <= 126 and c != "\\" and c != '"' for c in qstr):
        # qstr is all printable ASCII so render it as-is (for easier debugging)
        qdata = qstr
    else:
        # qstr contains non-printable codes so render entire thing as hex pairs
        qdata = "".join(("\\x%02x" % b) for b in qbytes)
    if qlen >= (1 << (8 * cfg_bytes_len)):
        print("qstr is too long:", qstr)
        assert False
    qlen_str = ("\\x%02x" * cfg_bytes_len) % tuple(
        ((qlen >> (8 * i)) & 0xFF) for i in range(cfg_bytes_len)
    )
    qhash_str = ("\\x%02x" * cfg_bytes_hash) % tuple(
        ((qhash >> (8 * i)) & 0xFF) for i in range(cfg_bytes_hash)
    )
    return 'QDEF(MP_QSTR_%s, (const byte *)"%s%s" "%s")' % (qdata, qhash_str, qlen_str, qdata)

if __name__ == "__main__":

    BYTES_IN_LEN = 1 #MICROPY_QSTR_BYTES_IN_LEN
    BYTES_IN_HASH = 2 #MICROPY_QSTR_BYTES_IN_HASH

    print ("Gen qstr for micropython with:")
    print ("BYTES_IN_LEN = %d" % BYTES_IN_LEN)
    print ("BYTES_IN_HASH = %d" % BYTES_IN_HASH)

    for arg in sys.argv:
        if arg == 'gen_qstr.py':
            continue
        qstr_computer = make_bytes(BYTES_IN_LEN, BYTES_IN_HASH, arg)
        print (qstr_computer)

