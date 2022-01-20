import utime
from machine import Timer


t1 = Timer(0, 10)
t2 = Timer(1, 3)
t1.callback(lambda t: print(t, "tick1"))
t2.callback(lambda t: print(t, "tick2"))

utime.sleep(3)
print("done")
