from random import *


UPPER = 100

s = set()

# This number of course depends on a particular PRNG and its default seed
# as used by MicroPython.
for c in range(496):
    r = randrange(UPPER)
    s.add(r)

assert len(s) == UPPER
