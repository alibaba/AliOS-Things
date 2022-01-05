"""
Copyright (C) 2015-2021 Alibaba Group Holding Limited

    MicroPython's driver for CHT8305

    Author: HaaS
    Date:   2021/09/14
"""

import utime
from micropython import const
from driver import I2C

#AHT21B_I2CADDR_DEFAULT = const(0x38)  # Default I2C address
AHT21B_CMD_CALIBRATE = const(0xE1)  # Calibration command
AHT21B_CMD_TRIGGER = const(0xAC)  # Trigger reading command
AHT21B_CMD_SOFTRESET = const(0xBA)  # Soft reset command
AHT21B_STATUS_READY = const(0x18)  # Status bit for ready for operate
AHT21B_STATUS_BUSY = const(0x80)  # Status bit for busy
AHT21B_STATUS_CALIBRATED = const(0x08)  # Status bit for calibrated

class AHT21B(object):
    # i2cDev should be an I2C object and it should be opened before __init__ is called
    def __init__(self, i2cDev):
        self._i2cDev = None
        if not isinstance(i2cDev, I2C):
            raise ValueError("parameter is not an I2C object")
        # make AHB21B's internal object points to i2cDev
        self._i2cDev = i2cDev
        self.start()

    # read status register
    def __status(self):
        dataBuffer = bytearray(1)
        self._i2cDev.read(dataBuffer)
        return dataBuffer[0]

    def start(self):
        # make sure AHB21B's internal object is valid before I2C operation
        if self._i2cDev is None:
            raise ValueError("invalid I2C object")

        print("open AHT21B")
        status = self.__status()

        # check whether AHT21B is ready for operation or not, if it is not ready do AHT21B reinitialization
        if (status & AHT21B_STATUS_READY) != AHT21B_STATUS_READY:
            # print(dataBuffer)
            self.reset()

    # reset register
    def resetReg(self, regAddr):

        print("reset ", regAddr)
        dataBuffer = bytearray(3)
        dataBuffer[0] = regAddr
        dataBuffer[1] = 0x00
        dataBuffer[2] = 0x00
        self._i2cDev.write(dataBuffer)
        utime.sleep_ms(5)

        self._i2cDev.read(dataBuffer)
        utime.sleep_ms(10)
        print(dataBuffer)

        dataBuffer[0] = regAddr|0xB0
        self._i2cDev.write(dataBuffer)

    # reset AHT21B chip according to its datasheet
    def reset(self):
        print("reset AHT21B start")

        self.resetReg(0x1b)
        self.resetReg(0x1c)
        self.resetReg(0x1e)

    # measure temperature and humidity
    def getTempHumidity(self):
        # make sure AHB21B's internal object is valid before I2C operation
        if self._i2cDev is None:
            raise ValueError("invalid I2C object")

        tempHumidity = [-1, 2]
        dataBuffer = bytearray(7)
        for i in range(1, len(dataBuffer)):
            dataBuffer[i] = 0x0

        # send "Trigger reading command" to AHT21B to start the measurement
        regValue = bytearray(3)
        regValue[0] = AHT21B_CMD_TRIGGER
        regValue[1] = 0x33
        regValue[2] = 0x00
        # print('start to measure')
        # send command to do the measurement
        self._i2cDev.write(regValue)
        # according to AHT21B's datasheet, after send measurement command, should wait for 80ms
        utime.sleep_ms(80)
        # check if status[7] is 0 or not, 0 means measure operation finished
        while self.__status() & AHT21B_STATUS_BUSY:
            utime.sleep_ms(5)

        # read sensor's measure result
        self._i2cDev.read(dataBuffer)

        # calculate actual temperature and humidity according to AHT21B's datasheet
        humi = ((dataBuffer[1] << 16) | (dataBuffer[2] << 8) | dataBuffer[3]) >> 4
        temp = ((dataBuffer[3] << 16) | (dataBuffer[4] << 8) | dataBuffer[5]) & 0xfffff

        tempHumidity[0] = round(temp * 200 /1024 / 1024 - 50, 2)
        tempHumidity[1] = round(humi * 100 /1024 / 1024)

        # print(tempHumidity)
        return tempHumidity

    # start measure and return temperature
    def getTemperature(self):
        data = self.getTempHumidity()
        return data[0]

    # start measure and return humidity
    def getHumidity(self):
        data = self.getTempHumidity()
        return data[1]

if __name__ == "__main__":
    '''
    The below i2c configuration is needed in your board.json.
    "aht21b": {
        "type": "I2C",
        "port": 1,
        "addrWidth": 7,
        "freq": 400000,
        "mode": "master",
        "devAddr": 56
    },
    '''
    print("Testing aht21b ...")

    i2cObj = I2C()
    i2cObj.open("aht21b")

    aht21bDev = AHT21B(i2cObj)

    temperature = aht21bDev.getTemperature()
    print("The temperature is: %f" % temperature)

    humidity = aht21bDev.getHumidity()
    print("The humidity is: %d" % humidity)

    i2cObj.close()

    print("Test aht21b done!")
