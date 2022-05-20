"""
Copyright (C) 2015-2021 Alibaba Group Holding Limited

    HaaS Python driver for tracker

    Author: HaaS
    Date:   2022/05/13
"""

from driver import GPIO

class TRACKER(object):

    def __init__(self, gpioObj):
        self.gpioObj = None
        if not isinstance(gpioObj, GPIO):
            raise ValueError("parameter is not an GPIO object")

        self.gpioObj = gpioObj

    def getTrackerStatus(self):
        if self.gpioObj is None:
            raise ValueError("invalid GPIO object")

        self.value = self.gpioObj.read()
        return self.value

    def close(self):
        if self.gpioObj is None:
            raise ValueError("invalid I2C object")

        self.gpioObj.close()
