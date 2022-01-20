import pickle
import sys
import io


def roundtrip(val):
    t = pickle.dumps(val)
    assert isinstance(t, bytes)
    t = pickle.loads(t)
    assert t == val


roundtrip(1)
roundtrip(1.0)
roundtrip("str")
roundtrip(b"bytes")
roundtrip((1,))
roundtrip([1, 2])
roundtrip({1: 2, 3: 4})

try:
    pickle.loads(b"1; import micropython")
    assert 0, "SyntaxError expected"
except SyntaxError:
    pass
