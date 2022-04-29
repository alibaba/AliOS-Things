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
        result = kv.get("HAASEDUK1V")
        if (result != None):
            if (result == '1'):
                print("HAASEDUK1 hw version is 1.1")
                return 1
            elif (result == '0'):
                print("HAASEDUK1 hw version is 1.0")
                return 0
            else:
                pass
        # kv中不存在HAASEDUK1V键值对，则通过外围传感器进行判断
        self.i2cDev = I2C()
        self.i2cDev.open("qmi8610")
        buf = bytearray(1)
        buf[0] = 0
        self.i2cDev.memRead(buf, 0, 8) # register address:0 - FIS device identifier register address
        self.i2cDev.close()
        if (buf[0] == 0xfc):
            kv.set("HAASEDUK1V", '1')
            print("HAASEDUK1 hw version is 1.1")
            return 1
        else:
            kv.set("HAASEDUK1V", '0')
            print("HAASEDUK1 hw version is 1.0")
            return 0

