import time
from signal import *

quit = 0


def handler(signum):
    global quit
    #    quit = 1
    print("Signal handler called with signal", signum)
    raise OSError("Couldn't open device!")


print("org signal() val:", signal(SIGINT, handler))
print("read back signal() val:", signal(SIGINT, handler))

try:
    while not quit:
        time.sleep(10)
except:
    print("Caught exc")

print("Quitting")
