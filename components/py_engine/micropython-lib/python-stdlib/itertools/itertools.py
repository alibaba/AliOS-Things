def count(start=0, step=1):
    while True:
        yield start
        start += step


def cycle(p):
    try:
        len(p)
    except TypeError:
        # len() is not defined for this type. Assume it is
        # a finite iterable so we must cache the elements.
        cache = []
        for i in p:
            yield i
            cache.append(i)
        p = cache
    while p:
        yield from p


def repeat(el, n=None):
    if n is None:
        while True:
            yield el
    else:
        for i in range(n):
            yield el


def chain(*p):
    for i in p:
        yield from i


def islice(p, start, stop=(), step=1):
    if stop == ():
        stop = start
        start = 0
    # TODO: optimizing or breaking semantics?
    if start >= stop:
        return
    it = iter(p)
    for i in range(start):
        next(it)

    while True:
        yield next(it)
        for i in range(step - 1):
            next(it)
        start += step
        if start >= stop:
            return


def tee(iterable, n=2):
    return [iter(iterable)] * n


def starmap(function, iterable):
    for args in iterable:
        yield function(*args)


def accumulate(iterable, func=lambda x, y: x + y):
    it = iter(iterable)
    try:
        acc = next(it)
    except StopIteration:
        return
    yield acc
    for element in it:
        acc = func(acc, element)
        yield acc
