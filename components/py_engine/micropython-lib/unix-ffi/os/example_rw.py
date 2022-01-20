import os


assert os.write(1, "hello\n") == len("hello\n")
data = os.read(0, 256)
os.write(2, "read: %s\n" % data)
