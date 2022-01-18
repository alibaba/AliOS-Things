import time
from pyb import LED

l = LED("left:amber")
print(l.get())
while 1:
    #    l.on()
    l.toggle()
    print(l.get())
    time.sleep(1)
#    l.off()
#    print(l.get())
#    time.sleep(1)
