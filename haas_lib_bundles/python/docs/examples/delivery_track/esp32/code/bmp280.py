"""
Copyright (C) 2015-2021 Alibaba Group Holding Limited

    MicroPython's driver for BMP280

    Author: HaaS
    Date:   2022/03/15
"""

from driver import I2C
from utime import sleep_ms
from micropython import const
import math

BSP280_CHIP_ID = const(0x58)

BMP280_REGISTER_DIG_T1 = const(0x88)
BMP280_REGISTER_DIG_T2 = const(0x8A)
BMP280_REGISTER_DIG_T3 = const(0x8C)
BMP280_REGISTER_DIG_P1 = const(0x8E)
BMP280_REGISTER_DIG_P2 = const(0x90)
BMP280_REGISTER_DIG_P3 = const(0x92)
BMP280_REGISTER_DIG_P4 = const(0x94)
BMP280_REGISTER_DIG_P5 = const(0x96)
BMP280_REGISTER_DIG_P6 = const(0x98)
BMP280_REGISTER_DIG_P7 = const(0x9A)
BMP280_REGISTER_DIG_P8 = const(0x9C)
BMP280_REGISTER_DIG_P9 = const(0x9E)

BMP280_REGISTER_CHIPID = const(0xD0)
BMP280_REGISTER_VERSION = const(0xD1)
BMP280_REGISTER_SOFTRESET = const(0xE0)

BMP280_REGISTER_CAL26 = const(0xE1)
BMP280_REGISTER_CONTROL = const(0xF4)
BMP280_REGISTER_CONFIG = const(0xF5)
BMP280_REGISTER_PRESSUREDATA = const(0xF7)
BMP280_REGISTER_TEMPDATA = const(0xFA)


class bmp280Error(Exception):

    def __init__(self, value=0, msg="bmp280 common error"):
        self.value = value
        self.msg = msg

    def __str__(self):
        return "Error code:%d, Error message: %s" % (self.value, str(self.msg))

    __repr__ = __str__


class BMP280(object):
    """
    This class implements bmp280 chip's defs.
    """

    def __init__(self, i2cDev):
        self._i2cDev = None
        if not isinstance(i2cDev, I2C):
            raise ValueError("parameter is not an I2C object")
        # make BMP280's internal object points to i2cDev
        self._i2cDev = i2cDev
        self.dig_T1 = 0
        self.dig_T2 = 0
        self.dig_T3 = 0

        self.dig_P1 = 0
        self.dig_P2 = 0
        self.dig_P3 = 0
        self.dig_P4 = 0
        self.dig_P5 = 0
        self.dig_P6 = 0
        self.dig_P7 = 0
        self.dig_P8 = 0
        self.dig_P9 = 0

        self.init()
        self.readCoefficients()
        self.writeReg(BMP280_REGISTER_CONTROL, 0x3f)
        self.t_fine = 0

    def readCoefficients(self):
        self.dig_T1 = self.readReg16(BMP280_REGISTER_DIG_T1)
        self.dig_T2 = self.readReg16_INT16(BMP280_REGISTER_DIG_T2)
        self.dig_T3 = self.readReg16_INT16(BMP280_REGISTER_DIG_T3)
        self.dig_P1 = self.readReg16(BMP280_REGISTER_DIG_P1)
        self.dig_P2 = self.readReg16_INT16(BMP280_REGISTER_DIG_P2)
        self.dig_P3 = self.readReg16_INT16(BMP280_REGISTER_DIG_P3)
        self.dig_P4 = self.readReg16_INT16(BMP280_REGISTER_DIG_P4)
        self.dig_P5 = self.readReg16_INT16(BMP280_REGISTER_DIG_P5)
        self.dig_P6 = self.readReg16_INT16(BMP280_REGISTER_DIG_P6)
        self.dig_P7 = self.readReg16_INT16(BMP280_REGISTER_DIG_P7)
        self.dig_P8 = self.readReg16_INT16(BMP280_REGISTER_DIG_P8)
        self.dig_P9 = self.readReg16_INT16(BMP280_REGISTER_DIG_P9)

    #写寄存器
    def writeReg(self, addr, value):
        Reg = bytearray([addr, value])
        self._i2cDev.write(Reg)
        #print("--> write addr " + hex(addr) + ", value = " + hex(value))
        return 0

#读寄存器

    def readReg(self, addr, len):
        Reg = bytearray([addr])
        self._i2cDev.write(Reg)
        sleep_ms(2)
        tmp = bytearray(len)
        self._i2cDev.read(tmp)
        #print("<-- read addr " + hex(addr) + ", value = " + hex(tmp[0]))
        return tmp

    def readReg16(self, addr):
        tmp = self.readReg(addr, 2)
        data = (tmp[1] << 8) + tmp[0]
        return data

    def readReg16_BE(self, addr):
        tmp = self.readReg(addr, 2)
        data = (tmp[0] << 8) + tmp[1]
        return data

    def readReg8(self, addr):
        tmp = self.readReg(addr, 1)
        data = tmp[0]
        return data

    def int16(self, dat):
        if dat > 32767:
            return dat - 65536
        else:
            return dat

    def int32(self, dat):
        if dat > (1 << 31):
            return dat - (1 << 32)
        else:
            return dat

    def readReg16_INT16(self, addr):
        tmp = self.readReg(addr, 2)
        data = (tmp[1] << 8) + tmp[0]
        data = self.int16(data)
        return data

    def deviceCheck(self):
        ret = self.readReg(BMP280_REGISTER_CHIPID, 1)[0]
        if (ret == BSP280_CHIP_ID):
            return 0
        else:
            return 1

    def getTemperature(self):
        adc_T = self.readReg16_BE(BMP280_REGISTER_TEMPDATA)
        adc_T <<= 8
        adc_T |= self.readReg8(BMP280_REGISTER_TEMPDATA + 2)
        adc_T >>= 4

        var1 = ((adc_T >> 3) - (self.dig_T1 << 1)) * (self.dig_T2) >> 11
        var2 = (((((adc_T >> 4) - self.dig_T1) *
                  ((adc_T >> 4) - self.dig_T1)) >> 12) * (self.dig_T3)) >> 14
        self.t_fine = var1 + var2
        t = ((self.t_fine * 5) + 128) >> 8
        return t / 100

    def getPressure(self):
        # before get pressure, needs to get temperature.
        self.getTemperature()
        adc_P = self.readReg16_BE(BMP280_REGISTER_PRESSUREDATA)
        adc_P <<= 8
        adc_P |= self.readReg8(BMP280_REGISTER_PRESSUREDATA + 2)
        adc_P >>= 4
        var1 = self.t_fine - 128000
        var2 = var1 * var1 * self.dig_P6
        var2 = (var2) + (((var1 * self.dig_P5)) << 17)
        var2 = var2 + (self.dig_P4 << 35)
        var1 = ((var1 * var1 * self.dig_P3) >> 8) + (
            (var1 * self.dig_P2) << 12)
        var1 = (((1 << 47) + var1) * (self.dig_P1)) >> 33
        p = 1048576 - adc_P
        p = (((p << 31) - var2) * 3125) / var1
        var1 = ((self.dig_P9) * (p / (1 << 13)) * (p / (1 << 13))) / (1 << 25)
        var2 = (self.dig_P8 * p) / (1 << 19)

        p = ((p + var1 + var2) / (1 << 8)) + (self.dig_P7 << 4)
        return p / 256

    def init(self):
        ret = self.deviceCheck()
        if (ret != 0):
            print("bmp280 init fail")
            return 0
        else:
            pass
