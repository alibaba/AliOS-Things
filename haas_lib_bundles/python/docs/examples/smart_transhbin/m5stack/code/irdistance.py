#!/usr/bin/env python3
# -*- encoding: utf-8 -*-
'''
@File       :    irdistance.py
@Description:    红外传感器
@Author     :    风裁
@version    :    1.0
'''

from driver import GPIO

class IRDISTANCE(object):
    def __init__(self, gpioObj):
        self.gpioObj = None
        if not isinstance(gpioObj, GPIO):
            raise ValueError("parameter is not a GPIO object")

        self.gpioObj = gpioObj

    def objectDetection(self):
        if self.gpioObj is None:
            raise ValueError("invalid GPIO object")

        value = self.gpioObj.read()
        return value
