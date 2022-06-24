"""
Copyright (C) 2015-2021 Alibaba Group Holding Limited

    MicroPython's driver for CHT8305

    Author: HaaS
    Date:   2021/09/14
"""

from micropython import const
from utime import sleep_ms
from driver import I2C

CHT8305_REG_TEMP = 0x00
CHT8305_REG_HUMI = 0x01

# The register address in CHT8305 controller.

class CHT8305Error(Exception):
    def __init__(self, value=0, msg="cht8305 common error"):
        self.value = value
        self.msg = msg

    def __str__(self):
        return "Error code:%d, Error message: %s" % (self.value, str(self.msg))

    __repr__ = __str__


class CHT8305(object):
    """
    This class implements cht8305 chip's functions.
    """
    def __init__(self, i2cDev):
        self._i2cDev = None
        if not isinstance(i2cDev, I2C):
            raise ValueError("parameter is not an I2C object")
        # make CHT8305's internal object points to i2cDev
        self._i2cDev = i2cDev

    def getTemperature(self):
        """Get temperature."""
        # make sure CHT8305's internal object is valid before I2C operation
        if self._i2cDev is None:
            raise ValueError("invalid I2C object")

        # send temperature register to CHT8305
        reg = bytearray([CHT8305_REG_TEMP])
        self._i2cDev.write(reg)
        # wait for 30ms
        sleep_ms(30)
        readData = bytearray(2)
        # read temperature from CHT8305
        self._i2cDev.read(readData)

        # convert the temperature data to actual value
        value = (readData[0] << 8) | readData[1]
        if (value & 0xFFFC):
            temperature = (165.0 * float(value)) / 65536.0 - 40.0
        else:
            raise CHT8305Error("failed to get temperature.")
        return temperature

    def getHumidity(self):
        """Get humidity."""
        # make sure CHT8305's internal object is valid before I2C operation
        if self._i2cDev is None:
            raise ValueError("invalid I2C object")

         # send humidity register to CHT8305
        reg = bytearray([CHT8305_REG_HUMI])
        self._i2cDev.write(reg)
        sleep_ms(30)
        # read humidity from CHT8305
        readData = bytearray(2)
        self._i2cDev.read(readData)

        # convert the humidity data to actual value
        value = (readData[0] << 8) | readData[1]
        if (value & 0xFFFE):
            humidity = ((125.0 * float(value)) / 65535.0) - 6.0
        else:
            raise CHT8305Error("failed to get humidity.")

        return humidity

    def getTempHumidity(self):
        """Get temperature and humidity."""
        # make sure CHT8305's internal object is valid before I2C operation
        if self._i2cDev is None:
            raise ValueError("invalid I2C object")

        temphumidity = [0, 1]
         # send temperature register to CHT8305
        reg = bytearray([CHT8305_REG_TEMP])
        self._i2cDev.write(reg)
        sleep_ms(30)

        # 4 bytes means read temperature and humidity back in one read operation
        readData = bytearray(4)
        self._i2cDev.read(readData)
        #print("rawdata %d-%d-%d-%d" %(readData[0],readData[1],readData[2],readData[3]))

        # convert the temperature and humidity data to actual value
        value = (readData[0] << 8) | readData[1]
        if (value & 0xFFFC):
            temphumidity[0] = (165.0 * float(value)) / 65536.0 - 40.0
        else:
            raise CHT8305Error("failed to get temperature.")

        value = (readData[2] << 8) | readData[3]
        if (value & 0xFFFE):
            temphumidity[1] = ((125.0 * float(value)) / 65535.0) - 6.0
        else:
            raise CHT8305Error("failed to get humidity.")

        return temphumidity


if __name__ == "__main__":
    '''
    The below i2c configuration is needed in your board.json.
    "cht8305": {
    "type": "I2C",
    "port": 1,
    "addrWidth": 7,
    "freq": 400000,
    "mode": "master",
    "devAddr": 64
    }
    '''
    print("Testing cht8305 ...")

    i2cDev = I2C()
    i2cDev.open("cht8305")

    cht8305Dev = CHT8305(i2cDev)

    temperature = cht8305Dev.getTemperature()
    print("The temperature is: %f" % temperature)

    humidity = cht8305Dev.getHumidity()
    print("The humidity is: %f" % humidity)

    i2cDev.close()

    print("Test cht8305 done!")
