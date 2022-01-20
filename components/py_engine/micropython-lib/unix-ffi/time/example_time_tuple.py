import time

ts = 0
for i in range(0, 31):
    ts += 1 << i
    ltt = time.localtime(ts)
    gtt = time.gmtime(ts)
    print("ts: ", ts, " local: ", tuple(ltt), " utc: ", tuple(gtt))
    lts = time.mktime(ltt)
    assert ts == lts
