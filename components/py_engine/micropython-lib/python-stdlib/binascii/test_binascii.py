from binascii import *
import utime

data = b"zlutoucky kun upel dabelske ody"
h = hexlify(data)

if h != b"7a6c75746f75636b79206b756e207570656c20646162656c736b65206f6479":
    raise Exception("Error")

data2 = unhexlify(h)

if data2 != data:
    raise Exception("Error")

a2b_base64(b"as==") == b"j"

start = utime.time()
for x in range(100000):
    d = unhexlify(h)

print("100000 iterations in: " + str(utime.time() - start))

print("OK")
