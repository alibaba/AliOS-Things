"""
Copyright (C) 2015-2021 Alibaba Group Holding Limited

    MicroPython's drive for Raindrops

    Author: HaaS
    Date:   2022/03/29
"""
from driver import GPIO
from driver import ADC

class Raindrops(object):
    def __init__(self, DO, AO=None):
        self.DO = None
        self.AO = None
        if not isinstance(DO, GPIO):
            raise ValueError('parameter DO is not an GPIO object')
        if AO is not None and not isinstance(AO, ADC):
            raise ValueError('parameter AO should be ADC object or None')

        self.DO = DO
        self.AO = AO

    # 读取数字信号
    def raindropDetect(self):
        return self.DO.read()

    # 读取模拟信号，电压
    def getVoltage(self):
        if not self.AO:
            raise RuntimeError('Can not get voltage, AO is not inited')
        return self.AO.readVoltage()

def main():
    DO = GPIO()
    DO.open('LM393_DO')
    AO = ADC()
    AO.open('LM393_AO')
    drv = Raindrops(DO, AO)
    return drv

if __name__ == '__main__':
    drv = main()
