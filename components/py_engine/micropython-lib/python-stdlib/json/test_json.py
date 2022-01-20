import json

inp = ["foo", {"bar": ("baz", None, 1, 2)}]
print(inp)

s = json.dumps(inp)
print(s)

outp = json.loads(s)
print(outp)

# Doesn't work because JSON doesn't have tuples
# assert inp == outp
