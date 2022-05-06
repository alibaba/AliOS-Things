"""
Copyright (C) 2015-2021 Alibaba Group Holding Limited
"""
from driver import I2C
import kv

class HAASEDUK1(object):
    def __init__(self):
        self.i2cDev = None

    # 获取版本号
    # 返回值为1，代表k1c
    # 返回值为0，代表k1
    def getHWID(self):
        hwId = -1
        result = kv.geti("HAASEDU_NAME")
        if (result != None):
            if (result == 1):
                print("HAASEDUK1 hw version is 1.1")
                return 1
            elif (result == 0):
                print("HAASEDUK1 hw version is 1.0")
                return 0
            else:
                pass
        # kv中不存在HAASEDU_NAME键值对，则通过外围传感器进行判断
        # 读取QMI8610传感器device identifier register的值
        self.i2cDev = I2C()
        self.i2cDev.open("qmi8610")
        buf = bytearray(1)
        buf[0] = 0
        self.i2cDev.memRead(buf, 0, 8) # register address:0 - FIS device identifier register address
        self.i2cDev.close()
        if buf[0] == 0xfc:
            hwId = 1
        else:
            # 读取QMI8610传感器chip id register的值
            self.i2cDev.open("qmp6988")
            buf[0] = 0xD1 # chip id register
            self.i2cDev.write(buf)
            self.i2cDev.read(buf)
            self.i2cDev.close()
            if buf[0] == 0x5C:
                hwId = 1
            else:
                # 读取QMC6310传感器chip id register的值
                self.i2cDev.open("qmc6310")
                buf[0] = 0x00 # chip id register
                self.i2cDev.write(buf)
                self.i2cDev.read(buf)
                self.i2cDev.close()
                if buf[0] == 0x80:
                    hwId = 1
        if hwId == 1:
            kv.seti("HAASEDU_NAME", 1)
            print("HAASEDUK1 hw version is 1.1")
            return 1
        else:
            kv.seti("HAASEDU_NAME", 0)
            print("HAASEDUK1 hw version is 1.0")
            return 0

