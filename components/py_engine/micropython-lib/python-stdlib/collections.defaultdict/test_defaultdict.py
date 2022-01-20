from collections import defaultdict

d = defaultdict.defaultdict(lambda: 42)
assert d[1] == 42
d[2] = 3
assert d[2] == 3
del d[1]
assert d[1] == 42

assert "foo" not in d
