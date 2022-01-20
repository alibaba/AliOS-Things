from multiprocessing import Pool


def f(x):
    return x * x


pool = Pool(4)
assert pool.apply(f, (10,)) == 100
