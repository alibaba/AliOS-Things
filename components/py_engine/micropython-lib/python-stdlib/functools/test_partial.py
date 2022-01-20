from functools import partial


def foo(x, y, prompt="result:"):
    print(prompt, x + y)


f = partial(foo, 10, prompt="arg+10:")
f(20)
