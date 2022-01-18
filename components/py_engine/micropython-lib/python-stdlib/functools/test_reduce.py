from functools import reduce

res = reduce(lambda x, y: x + y, [1, 2, 3, 4, 5])
assert res == 1 + 2 + 3 + 4 + 5

res = reduce(lambda x, y: x + y, [1, 2, 3, 4, 5], 10)
assert res == 10 + 1 + 2 + 3 + 4 + 5
