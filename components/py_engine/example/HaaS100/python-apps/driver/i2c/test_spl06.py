"""
Testing spl06 python driver

The below i2c configuration is needed in your board.json.
"spl06": {
  "type": "I2C",
  "port": 1,
  "addrWidth": 7,
  "freq": 400000,
  "mode": "master",
  "devAddr": 119
}
"""

from spl06 import SPL06

print("Testing spl06 ...")
spl06Dev = SPL06()
spl06Dev.open("spl06")
spl06Dev.init()
print("spl06 init finished")

id = spl06Dev.getID()
print("getID: " + str(id))

spl06Dev.spl06_getdata()

spl06Dev.close()

print("Test spl06 success!")
