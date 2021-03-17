
import driver

print("-------------------i2c test--------------------")
i2cObj = driver.i2c();
i2cObj.open("/data/python/config/i2c.json", "bmp280");

def bmp280SoftReset():
    regaddr = 0xe0;
    regval  = bytearray([0xb6]);
    i2cObj.writeReg(regaddr, regval)

def bmp280SetOdr():
    regval  = bytearray(1);
    print(regval)
    regval[0] &= 0x1f
    regval[0] |= 0x20
    regaddr = 0xf5;
    i2cObj.writeReg(regaddr, regval)

def bmp280ReadCalibParams():
    calibTable = bytearray(6)
    i2cObj.readReg(0x88, calibTable)
    print("calibTable")
    print(calibTable)
    calibParamT1 = (calibTable[1] << 8) | calibTable[0];
    calibParamT2 = (calibTable[3] << 8) | calibTable[2];
    calibParamT3 = (calibTable[5] << 8) | calibTable[4];
    print(calibParamT1)
    print(calibParamT2)
    print(calibParamT3)

def bmp280SetWorkMode():
    regval = bytearray(1)
    i2cObj.readReg(0xf4, regval)
    print("bmp280 old work mode")
    print(regval)
    regval[0] &= 0x03;
    regval[0] |= (0x03 << 5 | 0x03 << 2);
    i2cObj.writeReg(0xf4, regval)
    i2cObj.readReg(0xf4, regval)
    print("bmp280 new work mode")
    print(regval)

def bmp280PowerEnable():
    regval = bytearray(1)
    i2cObj.readReg(0xf4, regval)
    print(regval)
    regval[0] |= 0x03;
    i2cObj.writeReg(0xf4, regval)
    print("bmp280 power up")

chipID = bytearray(1)
i2cObj.readReg(0xd0, chipID)
print("chipID")
print(chipID)
bmp280SoftReset()
bmp280SetOdr()
bmp280ReadCalibParams();
bmp280SetWorkMode();
bmp280PowerEnable();
i2cObj.close()

print("-------------------i2c test--------------------")
