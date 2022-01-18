import operator


class A:
    pass


a = A()
a.name = "foo"
f = operator.attrgetter("name")
assert f(a) == "foo"
