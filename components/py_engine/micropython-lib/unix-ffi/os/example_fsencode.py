import os

print(os.fsencode("abc"))
print(os.fsencode(b"abc"))

print(os.fsdecode("abc"))
print(os.fsdecode(b"abc"))
