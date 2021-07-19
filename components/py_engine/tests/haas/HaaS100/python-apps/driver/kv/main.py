
from driver import KV

print("-------------------start kv test--------------------")
kv = KV()
result = kv.setStorageSync("aiot", "amp")
print(result)
result = kv.getStorageSync("aiot")
print(result)
result = kv.removeStorageSync("aiot")
print(result)
print("-------------------end kv test--------------------")
