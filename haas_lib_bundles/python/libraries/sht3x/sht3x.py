"""
Copyright (C) 2015-2021 Alibaba Group Holding Limited

    MicroPython's driver for CHT8305

    Author: HaaS
    Date:   2021/09/14
"""

from micropython import const
import utime
from driver import I2C

'''
# sht3x commands definations
# read serial number:                            CMD_READ_SERIALNBR  0x3780
# read status register:                          CMD_READ_STATUS     0xF32D
# clear status register:                         CMD_CLEAR_STATUS    0x3041
# enabled heater:                                CMD_HEATER_ENABLE   0x306D
# disable heater:                                CMD_HEATER_DISABLE  0x3066
# soft reset:                                    CMD_SOFT_RESET      0x30A2
# accelerated response time:                     CMD_ART             0x2B32
# break, stop periodic data acquisition mode:    CMD_BREAK           0x3093
# measurement: polling, high repeatability:      CMD_MEAS_POLLING_H  0x2400
# measurement: polling, medium repeatability:    CMD_MEAS_POLLING_M  0x240B
# measurement: polling, low repeatability:       CMD_MEAS_POLLING_L  0x2416
'''

class SHT3X(object):

    # i2cDev should be an I2C object and it should be opened before __init__ is called
    def __init__(self, i2cDev):
        self._i2cDev = None
        if not isinstance(i2cDev, I2C):
            raise ValueError("parameter is not an I2C object")

        # make AHB21B's internal object points to _i2cDev
        self._i2cDev = i2cDev
        self.start()

    def start(self):
        # make sure AHB21B's internal object is valid before I2C operation
        if self._i2cDev is None:
            raise ValueError("invalid I2C object")

        # send clear status register command - 0x3041 - CMD_CLEAR_STATUS
        cmd = bytearray(2)
        cmd[0] = 0x30
        cmd[1] = 0x41
        self._i2cDev.write(cmd)

        # wait for 20ms
        utime.sleep_ms(20)

        return 0

    def getTempHumidity(self):
        if self._i2cDev is None:
            raise ValueError("invalid I2C object")

        tempHumidity = [-1, 2]

        # start measurement: polling, medium repeatability - 0x240B - CMD_MEAS_POLLING_M
        # if you want to adjust measure repeatability, you can send the following commands:
        # high repeatability: 0x2400 - CMD_MEAS_POLLING_H
        # low repeatability: 0x2416  - CMD_MEAS_POLLING_L
        cmd = bytearray(2)
        cmd[0] = 0x24
        cmd[1] = 0x0b
        self._i2cDev.write(cmd)

        # must wait for a little before the measurement finished
        utime.sleep_ms(20)

        dataBuffer = bytearray(6)
        # read the measurement result
        self._i2cDev.read(dataBuffer)

        # print(dataBuffer)
        # calculate real temperature and humidity according to SHT3X-DIS' data sheet
        temp = (dataBuffer[0]<<8) | dataBuffer[1]
        humi = (dataBuffer[3]<<8) | dataBuffer[4]

        tempHumidity[1] = humi * 0.0015259022
        tempHumidity[0] = -45.0 + (temp) * 175.0 / (0xFFFF - 1)

        return tempHumidity

    def getTemperature(self):
        data = self.getTempHumidity()
        return data[0]

    def getHumidity(self):
        data = self.getTempHumidity()
        return data[1]

    def stop(self):

        if self._i2cDev is None:
            raise ValueError("invalid I2C object")

        # stop periodic data acquisition mode
        cmd = bytearray(3)
        cmd[0] = 0x30
        cmd[1] = 0x93
        self._i2cDev.write(cmd)
        # wait for a little while
        utime.sleep_ms(20)

        self._i2cDev = None
        return 0

    def __del__(self):
        print('sht3x __del__')

if __name__ == "__main__":
    '''
    The below i2c configuration is needed in your board.json.
    "sht3x": {
        "type": "I2C",
        "port": 1,
        "addrWidth": 7,
        "freq": 400000,
        "mode": "master",
        "devAddr": 68
    },
    '''
    print("Testing sht3x ...")

    i2cDev = I2C()
    i2cDev.open("sht3x")

    sht3xDev = SHT3X(i2cDev)

    '''
    # future usage:
    i2cDev = I2C("sht3x")
    sht3xDev = sht3x.SHT3X(i2cDev)
    '''

    temperature = sht3xDev.getTemperature()
    print("The temperature is: %f" % temperature)

    humidity = sht3xDev.getHumidity()
    print("The humidity is: %f" % humidity)

    print("Test sht3x done!")

