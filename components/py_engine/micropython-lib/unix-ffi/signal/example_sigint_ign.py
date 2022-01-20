import time
from signal import *


print("org signal() val:", signal(SIGINT, SIG_IGN))
print("read back signal() val:", signal(SIGINT, SIG_IGN))

print("You can't break me - use SIGTERM")
while 1:
    time.sleep(1)
