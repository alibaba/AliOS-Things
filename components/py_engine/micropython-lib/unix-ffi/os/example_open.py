import os

fd = os.open("example_open.py", os.O_RDONLY)
print(fd)
print(os.read(fd, 10))
