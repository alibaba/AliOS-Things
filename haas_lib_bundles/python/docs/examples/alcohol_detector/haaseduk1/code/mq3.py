"""
Copyright (C) 2015-2021 Alibaba Group Holding Limited

    MicroPython's driver for MQ3

    Author: HaaS
    Date:   2022/03/15
"""

from driver import ADC
from utime import sleep_ms
from micropython import const
import math


class MQ3(object):
    """
    This class implements mq3 chip's defs.
    """

    def __init__(self, adcObj):
        self._adcObj = None
        if not isinstance(adcObj, ADC):
            raise ValueError("parameter is not an adcObj object")
        self._adcObj = adcObj

    def getVoltage(self):
        if self._adcObj is None:
            raise ValueError("invalid ADC object")
        value = self._adcObj.readVoltage()
        return value
