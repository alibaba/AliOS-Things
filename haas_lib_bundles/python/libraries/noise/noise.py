'''
Copyright (C) 2015-2022 Alibaba Group Holding Limited

    MicroPython's driver for Noise

    Author: HaaS
    Date:   2022/03/23
'''
import utime   # 延时函数在utime库中
from driver import ADC

class Noise(object):
    def __init__(self, adcObj, avgSz=100):
        self.adcObj = None
        if not isinstance(adcObj, ADC):
            raise ValueError('parameter is not an ADC object')
        self.adcObj = adcObj

        self.avgSz = avgSz
        self.avgVec = [0] * self.avgSz
        self.avgVecIdx = -1

        self.init()

    # 初始化均值数组，后续通过计算平滑均值曲线值与当前值进行比较
    def init(self):
        # init avg vector
        for idx in range(self.avgSz):
            voltage = self.adcObj.readVoltage()
            self.avgVec[idx] = voltage

    # 获取当前电压值 mV
    def getVoltage(self):
        return self.adcObj.readVoltage()

    # 单次检查当前声音分贝是否超过阈值，changed 为 True 表示有变化，voltage 为当前电压值(mV)
    def checkNoise(self, voltage, threshold=400):
        self.avgVecIdx = (self.avgVecIdx + 1) % self.avgSz
        self.avgVec[self.avgVecIdx] = voltage
        avg = sum(self.avgVec) / self.avgSz
        changed = abs(voltage - avg) > threshold
        return changed

def main():
    adcObj = ADC()
    ret = adcObj.open('noise_adc')
    if ret != 0:
        raise Exception('open device failed %s' % ret)

    drv = Noise(adcObj)

    print('watch, doing...')
    while True:
        voltage = drv.getVoltage()
        changed = drv.checkNoise(voltage, 400)
        if changed:
            print('got change %s' % voltage)
        utime.sleep_ms(30)
    return drv

if __name__ == '__main__':
    main()
