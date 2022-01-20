import time
from multiprocessing import Pool


def f(x):
    return x * x


pool = Pool(4)
future = pool.apply_async(f, (10,))
assert future.get() == 100


def f2(x):
    time.sleep(0.5)
    return x + 1


future = pool.apply_async(f2, (10,))
iter = 0
while not future.ready():
    # print("not ready")
    time.sleep(0.1)
    iter += 1

assert future.get() == 11
assert iter >= 5 and iter <= 8

t = time.time()
futs = [
    pool.apply_async(f2, (10,)),
    pool.apply_async(f2, (11,)),
    pool.apply_async(f2, (12,)),
]

iter = 0
while True:
    # not all(futs):
    c = 0
    for f in futs:
        if not f.ready():
            c += 1
    if not c:
        break
    # print("not ready2")
    time.sleep(0.1)
    iter += 1

assert iter >= 5 and iter <= 8

print("Run 3 parallel sleep(1)'s in: ", time.time() - t)
