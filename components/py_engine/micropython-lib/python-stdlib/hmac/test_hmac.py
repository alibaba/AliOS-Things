import hmac
from hashlib.sha256 import sha256
from hashlib.sha512 import sha512

msg = b"zlutoucky kun upel dabelske ody"

dig = hmac.new(b"1234567890", msg=msg, digestmod=sha256).hexdigest()

print("c735e751e36b08fb01e25794bdb15e7289b82aecdb652c8f4f72f307b39dad39")
print(dig)

if dig != "c735e751e36b08fb01e25794bdb15e7289b82aecdb652c8f4f72f307b39dad39":
    raise Exception("Error")

dig = hmac.new(b"1234567890", msg=msg, digestmod=sha512).hexdigest()

print(
    "59942f31b6f5473fb4eb630fabf5358a49bc11d24ebc83b114b4af30d6ef47ea14b673f478586f520a0b9c53b27c8f8dd618c165ef586195bd4e98293d34df1a"
)
print(dig)

if (
    dig
    != "59942f31b6f5473fb4eb630fabf5358a49bc11d24ebc83b114b4af30d6ef47ea14b673f478586f520a0b9c53b27c8f8dd618c165ef586195bd4e98293d34df1a"
):
    raise Exception("Error")

key = b"\x06\x1au\x90|Xz;o\x1b<\xafGL\xbfn\x8a\xc94YPfC^\xb9\xdd)\x7f\xaf\x85\xa1\xed\x82\xbexp\xaf\x13\x1a\x9d"

dig = hmac.new(key[:20], msg=msg, digestmod=sha256).hexdigest()

print("59e332b881df09fdecf569c8b142b27fc989638720aeda2813f82442b6e3d91b")
print(dig)

if dig != "59e332b881df09fdecf569c8b142b27fc989638720aeda2813f82442b6e3d91b":
    raise Exception("Error")

dig = hmac.new(key[:32], msg=msg, digestmod=sha256).hexdigest()

print("b72fed815cd71acfa3a2f5cf2343679565fa18e7cd92226ab443aabd1fd7b7b0")
print(dig)

if dig != "b72fed815cd71acfa3a2f5cf2343679565fa18e7cd92226ab443aabd1fd7b7b0":
    raise Exception("Error")

dig = hmac.new(key, msg=msg, digestmod=sha256).hexdigest()

print("4e51beae6c2b0f90bb3e99d8e93a32d168b6c1e9b7d2130e2d668a3b3e10358d")
print(dig)

if dig != "4e51beae6c2b0f90bb3e99d8e93a32d168b6c1e9b7d2130e2d668a3b3e10358d":
    raise Exception("Error")
