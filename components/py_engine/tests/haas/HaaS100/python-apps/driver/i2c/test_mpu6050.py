"""
Testing mpu6050 python driver

The below i2c configuration is needed in your board.json.

"mpu6050": {
    "type": "I2C",
    "port": 1,
    "addrWidth": 7,
    "freq": 400000,
    "mode": "master",
    "devAddr": 105
}
"""

print("testing mpu6050...")
from mpu6050 import MPU6050

mpu6050Dev = MPU6050()
mpu6050Dev.open("mpu6050")
mpu6050Dev.init()
print("mpu6050 init finished")

temp = mpu6050Dev.get_Temperature()
print("mpu6050 temp is: " , temp)

gy = []
gy = mpu6050Dev.get_Gyroscope()
print("mpu6050 gyro is: " , gy[0], gy[1], gy[2])
ac = []
ac = mpu6050Dev.get_Accelerometer()
print("mpu6050 acc is: " , ac[0], ac[1], ac[2])

mpu6050Dev.close()
print("test mpu6050 success!")
