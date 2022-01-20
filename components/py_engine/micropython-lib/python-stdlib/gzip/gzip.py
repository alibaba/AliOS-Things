# import zlib
import uzlib as zlib

FTEXT = 1
FHCRC = 2
FEXTRA = 4
FNAME = 8
FCOMMENT = 16


def decompress(data):
    assert data[0] == 0x1F and data[1] == 0x8B
    assert data[2] == 8
    flg = data[3]
    assert flg & 0xE0 == 0
    i = 10
    if flg & FEXTRA:
        i += data[11] << 8 + data[10] + 2
    if flg & FNAME:
        while data[i]:
            i += 1
        i += 1
    if flg & FCOMMENT:
        while data[i]:
            i += 1
        i += 1
    if flg & FHCRC:
        i += 2
    return zlib.decompress(memoryview(data)[i:], -15)
