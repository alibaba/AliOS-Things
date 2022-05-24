"""
Copyright (C) 2015-2021 Alibaba Group Holding Limited

    MicroPython's driver for ULN2003

    Author: HaaS
    Date:   2022/03/15
"""

from driver import GPIO
from utime import sleep_ms
from micropython import const
import math


class ULN2003(object):
    """
    This class implements uln2003 chip's defs.
    """

    def __init__(self, a, a_, b, b_):
        self._a = None
        self._a_ = None
        self._b = None
        self._b_ = None
        if not isinstance(a, GPIO):
            raise ValueError("parameter is not an GPIO object")
        if not isinstance(a_, GPIO):
            raise ValueError("parameter is not an GPIO object")
        if not isinstance(b, GPIO):
            raise ValueError("parameter is not an GPIO object")
        if not isinstance(b_, GPIO):
            raise ValueError("parameter is not an GPIO object")
        # make ULN2003's internal object points to gpio
        self._a = a
        self._a_ = a_
        self._b = b
        self._b_ = b_

    def motorCw(self, speed=4):
        self._a.write(1)
        self._a_.write(0)
        self._b.write(0)
        self._b_.write(0)
        sleep_ms(speed)

        self._a.write(0)
        self._a_.write(1)
        self._b.write(0)
        self._b_.write(0)
        sleep_ms(speed)

        self._a.write(0)
        self._a_.write(0)
        self._b.write(1)
        self._b_.write(0)
        sleep_ms(speed)

        self._a.write(0)
        self._a_.write(0)
        self._b.write(0)
        self._b_.write(1)
        sleep_ms(speed)

    def motorCcw(self, speed=4):
        self._a.write(0)
        self._a_.write(0)
        self._b.write(0)
        self._b_.write(1)
        sleep_ms(speed)

        self._a.write(0)
        self._a_.write(0)
        self._b.write(1)
        self._b_.write(0)
        sleep_ms(speed)

        self._a.write(0)
        self._a_.write(1)
        self._b.write(0)
        self._b_.write(0)
        sleep_ms(speed)

        self._a.write(1)
        self._a_.write(0)
        self._b.write(0)
        self._b_.write(0)
        sleep_ms(speed)

    def motorStop(self):
        self._a.write(0)
        self._a_.write(0)
        self._b.write(0)
        self._b_.write(0)
