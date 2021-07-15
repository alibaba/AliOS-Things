
from driver import I2C

print("-------------------i2c test--------------------")
i2c = I2C()
i2c.open("pca9544")
regval = bytearray(1)
regval[0] = 0x5
print(regval)
ret = i2c.write(regval)
print(ret)
i2c.close()
print("-------------------i2c test--------------------")
