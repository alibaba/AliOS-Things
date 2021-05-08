"""
Testing si7006 python driver

The below i2c configuration is needed in your board.json.
"si7006": {
  "type": "I2C",
  "port": 1,
  "addrWidth": 7,
  "freq": 400000,
  "mode": "master",
  "devAddr": 64
}
"""

from si7006 import SI7006

print("Testing si7006 ...")
si7006Dev = SI7006()
si7006Dev.open("si7006")
version = si7006Dev.getVer()
print("si7006 version is: %d" % version)

chipID = si7006Dev.getID()
print("si7006 chip id is:", chipID)


temperature = si7006Dev.getTemperature()
print("The temperature is: %f" % temperature)

humidity = si7006Dev.getHumidity()
print("The humidity is: %f" % humidity)

si7006Dev.close()

print("Test si7006 success!")
