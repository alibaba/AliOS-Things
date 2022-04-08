"""
Copyright (C) 2015-2021 Alibaba Group Holding Limited
`HaaS Python UV sensor`
====================================================

A driver for uv sensor

* Author(s): HaaS Group

Implementation Notes
--------------------

**Hardware:**

* HaaS Python uv sensor:
  https://haas.iot.aliyun.com/solution/detail/hardware

**Software and Dependencies:**

* HaaS Python API documents:
  https://haas.iot.aliyun.com/haasapi/index.html#/

* HaaS Python Driver Libraries:
  https://github.com/alibaba/AliOS-Things/tree/master/haas_lib_bundles/python/libraries

"""

# coding=utf-8
from driver import ADC

'''
紫外线传感器测量结果和紫外线级别对应关系
紫外线指数-x    测量电压-v（mV）
0   50
1   227
2   318
3   408
4   503
5   606
6   696
7   798
8   881
9   976
10  1079
11  1170

拟合曲线：
{v} = a{x}+b
用1:227和11:1170进行拟合，a = 94.3, b = 132.7
则x = ({v}-b)/a
'''

from driver import ADC


class UV(object):

    def __init__(self, adcObj):
        self.adcObj = None
        if not isinstance(adcObj, ADC):
            raise ValueError("parameter is not an ADC object")

        self.adcObj = adcObj

    # 量测紫外线强度，返回值：UV强度值，范围[0-11]
    def measureUVLevel(self):
        if self.adcObj is None:
            raise ValueError("invalid ADC object")
        value = 0
        total = 0
        i = 0
        for i in range(30):
            value = self.adcObj.readVoltage()
            total += value
            # print(value)
        V = total / 30
        UVLevel=(V - 132.7)/94.3

        print(UVLevel)
        if UVLevel <= 0:
            UVLevel = 0
        if UVLevel >= 11:
            UVLevel = 11

        return round(UVLevel, 0)

if __name__ == "__main__":
    import utime

    adcObj = ADC()
    adcObj.open("uvSensor")
    print('Measuring UV level , press Ctrl-C to quit...')
    UVDev = UV(adcObj)
    while True:
        uvLevel = UVDev.measureUVLevel()
        print('UVLevel:', uvLevel)
        utime.sleep(1)

    adcObj.close()
    del UVDev
