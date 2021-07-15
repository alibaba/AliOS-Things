"""
Testing qmc5883 python driver

The below i2c configuration is needed in your board.json.
"qmc5883": {
  "type": "I2C",
  "port": 1,
  "addrWidth": 7,
  "freq": 400000,
  "mode": "master",
  "devAddr": 13
}
"""

from qmc5883 import QMC5883

print("Testing qmc5883 ...")
qmc5883Dev = QMC5883()
qmc5883Dev.open("qmc5883")
qmc5883Dev.init()
print("qmc5883 init finished")

heading = qmc5883Dev.qmc5883l_readHeading()
print("heading = %f\n" %heading)
heading = qmc5883Dev.qmc5883l_readHeading()
print("heading = %f\n" %heading)
heading = qmc5883Dev.qmc5883l_readHeading()
print("heading = %f\n" %heading)
heading = qmc5883Dev.qmc5883l_readHeading()
print("heading = %f\n" %heading)

qmc5883Dev.close()

print("Test qmc5883 success!")
