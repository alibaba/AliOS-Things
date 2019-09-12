import os, errno, sys

def mkdir_p(path):
    try:
        os.makedirs(path)
    except OSError as exc: # Python >2.5 (except OSError, exc: for Python <2.5)
        if exc.errno == errno.EEXIST and os.path.isdir(path):
            pass
        else: raise

if len(sys.argv) != 2:
    print("usage:python "+sys.argv[0]+" dirname")
else:
    mkdir_p(sys.argv[1])