"""
Copyright (C) 2015-2021 Alibaba Group Holding Limited

    HaaS Python driver for motorspeed

    Author: HaaS
    Date:   2022/05/15
"""

from driver import GPIO

class MOTORSPEED(object):
    def __init__(self, gpioObj, func=None):
        self.gpioObj = None
        if not isinstance(gpioObj, GPIO):
            raise ValueError("parameter is not a GPIO object")

        self.gpioObj = gpioObj
        if func != None:
            self.gpioObj.on(func)

    def objectDetection(self):
        if self.gpioObj is None:
            raise ValueError("invalid GPIO object")

        value = self.gpioObj.read()
        return value
