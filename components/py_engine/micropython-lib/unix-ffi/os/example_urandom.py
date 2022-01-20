import os

r = os.urandom(10)
assert len(r) == 10
