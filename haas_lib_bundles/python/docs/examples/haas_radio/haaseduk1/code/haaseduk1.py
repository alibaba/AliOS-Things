
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
        result = kv.get("HAASEDU_NAME")
        if (result != None):
            if (result == '1'):
                # print("HAASEDU hw version is 1.1")
                return 1
            elif (result == '0'):
                # print("HAASEDU hw version is 1.0")
                return 0
            else:
                pass
        else:
            pass
        self.i2cDev = I2C()
        self.i2cDev.open("qmi8610")
        buf = bytearray(1)
        buf[0] = 0               # FIS device identifier register address
        self.i2cDev.write(buf)
        self.i2cDev.read(buf)
        self.i2cDev.close()
        if (buf[0] == 0xfc):
            kv.set("HAASEDU_NAME", "1")
            return 1
        else:
            kv.set("HAASEDU_NAME", "0")
            return 0
