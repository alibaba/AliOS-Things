import os


dir = "."
if "/" in __file__:
    dir = __file__.rsplit("/", 1)[0]

assert os.access(dir + "/test_filestat.py", os.F_OK) == True
assert os.access(dir + "/test_filestat.py-not", os.F_OK) == False
