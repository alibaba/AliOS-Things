"""
Copyright (C) 2015-2021 Alibaba Group Holding Limited

    MicroPython's driver for SN74HC595

    Author: HaaS
    Date:   2022/03/15
"""

from driver import GPIO
from utime import sleep_ms
from micropython import const
import math


class SN74HC595(object):
    """
    This class implements sn74hc595 chip's defs.
    """

    def __init__(self, rclk, sclk, dio):
        self._rclk = None
        self._sclk = None
        self._dio = None
        if not isinstance(rclk, GPIO):
            raise ValueError("parameter is not an GPIO object")
        if not isinstance(sclk, GPIO):
            raise ValueError("parameter is not an GPIO object")
        if not isinstance(dio, GPIO):
            raise ValueError("parameter is not an GPIO object")
        # make 74HC595's internal object points to gpio
        self._rclk = rclk
        self._sclk = sclk
        self._dio = dio
        self.led_0f = bytearray([
            0xc0, 0xf9, 0xa4, 0xb0, 0x99, 0x92, 0x82, 0xf8, 0x80, 0x90, 0x8c,
            0xbf, 0xc6, 0xa1, 0x86, 0xff, 0xbf
        ])

    def write(self, data):
        for i in range(8):
            if (data & 0x80) != 0:
                self._dio.write(1)
            else:
                self._dio.write(0)
            data <<= 1
            self._sclk.write(0)
            self._sclk.write(1)

    def outSegmentdigit(self, position, digit):
        digit = 0xf & digit
        position = (0x3 & position)
        position = 1 << position
        digit = self.led_0f[digit]
        self.write(digit)
        self.write(position)
        self._rclk.write(0)
        self._rclk.write(1)

    def outSegmentHex(self, data):
        digit = data & 0xf
        self.outSegmentdigit(0, digit)
        sleep_ms(2)

        data >>= 4
        digit = data & 0xf
        self.outSegmentdigit(1, digit)
        sleep_ms(2)

        data >>= 4
        digit = data & 0xf
        self.outSegmentdigit(2, digit)
        sleep_ms(2)

        data >>= 4
        digit = data & 0xf
        self.outSegmentdigit(3, digit)
        sleep_ms(2)
