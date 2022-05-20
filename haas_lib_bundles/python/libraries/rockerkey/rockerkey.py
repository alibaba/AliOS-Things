"""
Copyright (C) 2015-2021 Alibaba Group Holding Limited

    HaaS Python driver for rockerkey

    Author: HaaS
    Date:   2022/05/06
"""

import ads1x15                       # ADS1X15 ADC传感器驱动库
from driver import I2C,GPIO

class ROCKERKEY(object):

    def __init__(self, i2cObj,gpioObj,MaxVoltage=5500,XAxis=0,YAxis=1,ZAxis=2):
        self.i2cObj = None
        self.zaxisObj = None
        if not isinstance(i2cObj, I2C):
            raise ValueError("parameter is not an I2C object")

        if not isinstance(gpioObj, GPIO):
            raise ValueError("parameter is not an GPIO object")

        self.zaxisObj = gpioObj
        self.i2cObj = i2cObj
        self.adcDev = ads1x15.ADS1115(self.i2cObj)  # 初始化ADS1115传感器对象
        self.adcDev.setMaxVoltage(MaxVoltage)
        self.xaxis = XAxis
        self.yaxis = YAxis
        self.zaxis = ZAxis
        self.xvol = 0
        self.yvol = 0
        self.zvol = 0

    def getXYZAxis(self):
        if self.i2cObj is None:
            raise ValueError("invalid I2C object")
        if self.zaxisObj is None:
            raise ValueError("invalid GPIO object")

        self.xvol = self.adcDev.readVoltage(self.xaxis)      # 量测x轴通道的输入电压值
        self.yvol = self.adcDev.readVoltage(self.yaxis)      # 量测y轴通道的输入电压值
        self.zvol = self.zaxisObj.read()                     # 获取z轴通道的电平值
        return self.xvol,self.yvol,self.zvol

    def close(self):
        if self.i2cObj is None:
            raise ValueError("invalid I2C object")

        self.i2cObj.close()
