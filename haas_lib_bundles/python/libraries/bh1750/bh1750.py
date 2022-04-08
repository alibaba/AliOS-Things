"""
Copyright (C) 2015-2022 Alibaba Group Holding Limited

    MicroPython's drive for BH1750

    Author: HaaS
    Date:   2022/03/22
"""
import utime
from driver import I2C

class BH1750(object):
    MT_HIGH_MAX = 180 # ms
    MT_LOW_MAX = 24 # ms
    MT_HIGH_TYP = 120 # ms
    MT_LOW_TYP = 16 # ms

    # Continuously high resolution mode 1/2, low resolution mode
    CMD_CONT_H_MODE = 0b0001_0000  # 1lx resolution
    CMD_CONT_H_MODE2 = 0b0001_0001 # 0.5lx resolution
    CMD_CONT_L_MODE = 0b0001_0011  # 4lx resolution

    # One time high resolution mode 1/2, low resolution mode
    CMD_ONE_H_MODE = 0b0010_0000   # 1lx
    CMD_ONE_H_MODE2 = 0b0010_0001  # 0.5lx
    CMD_ONE_L_MODE = 0b0010_0011   # 4lx

    def __init__(self, i2cDev):
        self._i2cDev = None
        if not isinstance(i2cDev, I2C):
            raise ValueError("parameter is not an I2C object")
        self._i2cDev = i2cDev

    # 获取高分辨率照度，模式1，分辨率为 1lx
    def oneHighRes(self):
        if not self._i2cWriteBytes(self.CMD_ONE_H_MODE):
            return 0
        utime.sleep_ms(self.MT_HIGH_MAX)
        buf = self._i2cReadBytes(2)
        if buf is None:
            return 0
        # else parse result
        return self._parseLx(buf)

    # 获取高分辨率照度，模式2，分辨率为 0.5lx
    def oneHighRes2(self):
        if not self._i2cWriteBytes(self.CMD_ONE_H_MODE2):
            return 0
        utime.sleep_ms(self.MT_HIGH_MAX)
        buf = self._i2cReadBytes(2)
        if buf is None:
            return 0
        # else parse result
        return self._parseLx(buf)

    # 获取低分辨率照度，分辨率为 4lx
    def oneLowRes(self):
        if not self._i2cWriteBytes(self.CMD_ONE_L_MODE):
            return 0
        utime.sleep_ms(self.MT_LOW_MAX)
        buf = self._i2cReadBytes(2)
        if buf is None:
            return 0
        # else parse result
        return self._parseLx(buf)

    def getLuminance(self):
        return self.oneLowRes()

    def _contRead(self, cb, cmd, mtMax, mtTyp):
        if not self._i2cWriteBytes(cmd):
            return False
        utime.sleep_ms(mtMax)
        # else parse result
        while True:
            buf = self._i2cReadBytes(2)
            if buf is None:
                return False
            val = self._parseLx(buf)
            if cb(val):
                break
            utime.sleep_ms(mtTyp)
        return True

    # 连续获取光照度，高分辨率，通过 cb(val) 传递值
    def contHighRes(self, cb):
        return self._contRead(cb, self.CMD_CONT_H_MODE, self.MT_HIGH_MAX, self.MT_HIGH_TYP)

    # 连续获取光照度，高分辨率模式2，通过 cb(val) 传递值
    def contHighRes2(self, cb):
        return self._contRead(cb, self.CMD_CONT_H_MODE2, self.MT_HIGH_MAX, self.MT_HIGH_TYP)

    # 连续获取光照度，低分辨率，通过 cb(val) 传递值
    def contLowRes(self, cb):
        return self._contRead(cb, self.CMD_CONT_L_MODE, self.MT_LOW_MAX, self.MT_LOW_TYP)

    def _parseLx(self, buf):
        #print(buf)
        return (buf[0] << 8 | buf[1]) / 1.2

    def _i2cWriteBytes(self, *args):
        buf = bytearray(args)
        ret = self._i2cDev.write(buf)
        if ret != len(buf):
            print('write failed, ret %s, expect %s', (ret, len(buf)))
            return False
        return True

    def _i2cReadBytes(self, byteNum):
        buf = bytearray(byteNum)
        ret = self._i2cDev.read(buf)
        if ret != byteNum:
            print('read failed, ret %s, expect %s' % (ret, byteNum))
            return None
        return buf
