"""
Copyright (C) 2015-2021 Alibaba Group Holding Limited
`HaaS Python ads1x15`
====================================================

A driver for ads1x15

* Author(s): HaaS Group

Implementation Notes
--------------------

**Hardware:**

* HaaS Python ads1x15
  https://haas.iot.aliyun.com/solution/detail/hardware

**Software and Dependencies:**

* HaaS Python API documents:
  https://haas.iot.aliyun.com/haasapi/index.html#/

* HaaS Python Driver Libraries:
  https://github.com/alibaba/AliOS-Things/tree/master/haas_lib_bundles/python/libraries

"""

# coding=utf-8
import utime as time
from driver import I2C

'''
# Choose a gain of 1 for reading voltages from 0 to 4.09V.
# Or pick a different gain to change the range of voltages that are read:
#  - 2/3 = +/-6.144V
#  -   1 = +/-4.096V
#  -   2 = +/-2.048V
#  -   4 = +/-1.024V
#  -   8 = +/-0.512V
#  -  16 = +/-0.256V
'''

# Register and other configuration values:
ADS1x15_DEFAULT_ADDRESS        = 0x48
ADS1x15_POINTER_CONVERSION     = 0x00
ADS1x15_POINTER_CONFIG         = 0x01
ADS1x15_POINTER_LOW_THRESHOLD  = 0x02
ADS1x15_POINTER_HIGH_THRESHOLD = 0x03
ADS1x15_CONFIG_OS_SINGLE       = 0x8000
ADS1x15_CONFIG_MUX_OFFSET      = 12
# Maping of gain values to config register values.
ADS1x15_CONFIG_GAIN = {
    2/3: 0x0000,
    1:   0x0200,
    2:   0x0400,
    4:   0x0600,
    8:   0x0800,
    16:  0x0A00
}

ADS1x15_GAIN_RESOLUTION = {
    2/3: 6.144/32767,
    1:   4.096/32767,
    2:   2.048/32767,
    4:   1.024/32767,
    8:   0.512/32767,
    16:  0.256/32767
}
ADS1x15_CONFIG_MODE_CONTINUOUS  = 0x0000
ADS1x15_CONFIG_MODE_SINGLE      = 0x0100
# Mapping of data/sample rate to config register values for ADS1015 (faster).
ADS1015_CONFIG_DR = {
    128:   0x0000,
    250:   0x0020,
    490:   0x0040,
    920:   0x0060,
    1600:  0x0080,
    2400:  0x00A0,
    3300:  0x00C0
}
# Mapping of data/sample rate to config register values for ADS1115 (slower).
ADS1115_CONFIG_DR = {
    8:    0x0000,
    16:   0x0020,
    32:   0x0040,
    64:   0x0060,
    128:  0x0080,
    250:  0x00A0,
    475:  0x00C0,
    860:  0x00E0
}
ADS1x15_CONFIG_COMP_WINDOW      = 0x0010
ADS1x15_CONFIG_COMP_ACTIVE_HIGH = 0x0008
ADS1x15_CONFIG_COMP_LATCHING    = 0x0004
ADS1x15_CONFIG_COMP_QUE = {
    1: 0x0000,
    2: 0x0001,
    4: 0x0002
}
ADS1x15_CONFIG_COMP_QUE_DISABLE = 0x0003

class ADS1x15(object):
    def __init__(self, *args,**Kwargs):
        self._i2cDev = None

        if not isinstance(args[0], I2C):
            raise ValueError("parameter is not an I2C object")

        #实例化
        self.i2cDev=args[0]

        self._gain = 1 # voltage range:+/-4.096V

    def _data_rate_default(self):
        #子类需要实现 父类的方法，否则报错
        raise NotImplementedError("subclass must implemet _data_rate_default!")

    def _data_rate_config(self,data_rate):
        #子类需要实现 父类的方法，否则报错
        raise NotImplementedError("subclass must implemet _data_rate_default!")

    def _conversion_value(self, low, high):
        #子类需要实现 父类的方法，否则报错
        raise NotImplementedError('Subclass must implement _conversion_value function!')

    #mux, gain, data_rate, and mode 要在规定的范围内
    def _read(self, mux, gain, data_rate, mode):
        # Go out of power-down mode for conversion.
        config = ADS1x15_CONFIG_OS_SINGLE

        # Specify mux value.
        config |= (mux & 0x07) << ADS1x15_CONFIG_MUX_OFFSET

        #设置增益
        if gain not in ADS1x15_CONFIG_GAIN:
            raise ValueError('Gain must be one of: 2/3, 1, 2, 4, 8, 16')
        config |= ADS1x15_CONFIG_GAIN[gain]

        #设置模式(continuous or single shot)
        config |= mode

        #测量速率，默认128bps
        if data_rate is None:
            data_rate = self._data_rate_default()

        #设置速率
        config |= self._data_rate_config(data_rate)
        #disable 比较器模式
        config |= ADS1x15_CONFIG_COMP_QUE_DISABLE

        #I2C的写函数
        writeData=bytearray(3)
        writeData[0]=ADS1x15_POINTER_CONFIG
        writeData[1]=(config >> 8) & 0xFF
        writeData[2]=config & 0xFF
        self.i2cDev.write(writeData)

        #等待ADC采样（根据采样率加上一个很小的偏置，如0.1ms)
        time.sleep(1.0/data_rate+0.0001)

        #读取ADC采样结果
        readData=bytearray([0x0,0x00])
        self.i2cDev.memRead(readData, ADS1x15_POINTER_CONVERSION, 8)

        #return 读取到的数据，包含高八位和低八位
        return self._conversion_value(readData[1], readData[0])

    def read_adc(self, channel, gain=1, data_rate=None):
        #读单个ADC通道，通道值取值范围为[0,3]
        assert 0 <= channel <= 3, 'Channel must be a value within 0-3!'
        # Perform a single shot read and set the mux value to the channel plus
        # the highest bit (bit 3) set.
        return self._read(channel + 0x04, gain, data_rate, ADS1x15_CONFIG_MODE_SINGLE)

    def readVoltage(self, channel):
        #读取ADC量测结果
        data = self.read_adc(channel, self._gain)
        #电压值：ADC量测结果*分辨率，单位：mV
        return round(data * ADS1x15_GAIN_RESOLUTION[self._gain] * 1000, 0)

    def setMaxVoltage(self, maxVoltage):
        #设置要量测的最大电压，单位：mV
        if (maxVoltage > 6144):
            raise ValueError('voltage should between (-6144, 6144), unit:mV')
        if (maxVoltage >= 4096):
            self._gain = 1
        elif (maxVoltage >= 2048):
            self._gain = 2
        elif (maxVoltage >= 1024):
            self._gain = 4
        elif (maxVoltage >= 512):
            self._gain = 8
        elif (maxVoltage >= 256):
            self._gain = 8
        else:
            self._gain = 16

        return 0

#继承父类
class ADS1115(ADS1x15):
    """ADS1115 16-bit analog to digital converter instance."""
    def __init__(self, *args, **kwargs):
        super(ADS1115, self).__init__(*args, **kwargs)

    def _data_rate_default(self):
        #默认速率为128bps
        return 128

    def _data_rate_config(self, data_rate):
        if data_rate not in ADS1115_CONFIG_DR:
            raise ValueError('Data rate must be one of: 8, 16, 32, 64, 128, 250, 475, 860')
        return ADS1115_CONFIG_DR[data_rate]

    def _conversion_value(self, low, high):
        #转换16位数据
        value = ((high & 0xFF) << 8) | (low & 0xFF)
        if value & 0x8000 != 0:
            value = -(0xffff - value)
        return value
