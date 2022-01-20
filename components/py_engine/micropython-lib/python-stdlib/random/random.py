from urandom import *

_getrandbits32 = getrandbits


def getrandbits(bits: int) -> int:
    n = bits // 32
    d = 0
    for i in range(n):
        d |= _getrandbits32(32) << (i * 32)

    r = bits % 32
    if r >= 1:
        d |= _getrandbits32(r) << (n * 32)

    return d


def randrange(start, stop=None):
    if stop is None:
        stop = start
        start = 0
    upper = stop - start
    bits = 0
    pwr2 = 1
    while upper > pwr2:
        pwr2 <<= 1
        bits += 1
    while True:
        r = getrandbits(bits)
        if r < upper:
            break
    return r + start


def randint(start, stop):
    return randrange(start, stop + 1)


def shuffle(seq):
    l = len(seq)
    for i in range(l):
        j = randrange(l)
        seq[i], seq[j] = seq[j], seq[i]
