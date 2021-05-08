# -*- coding: UTF-8 -*-

"""
The driver for Si7006 chip, it is a temperature and humidity sensor.
"""

from driver import I2C
from utime import sleep_ms


# The register address in Si7006 controller.
Si7006_MEAS_REL_HUMIDITY_MASTER_MODE    = 0xE5
Si7006_MEAS_REL_HUMIDITY_NO_MASTER_MODE = 0xF5
Si7006_MEAS_TEMP_MASTER_MODE            = 0xE3
Si7006_MEAS_TEMP_NO_MASTER_MODE         = 0xF3
Si7006_READ_OLD_TEMP                    = 0xE0
Si7006_RESET                            = 0xFE
Si7006_READ_ID_LOW_0                    = 0xFA
Si7006_READ_ID_LOW_1                    = 0x0F
Si7006_READ_ID_HIGH_0                   = 0xFC
Si7006_READ_ID_HIGH_1                   = 0xC9
Si7006_READ_Firmware_Revision_0         = 0x84
Si7006_READ_Firmware_Revision_1         = 0xB8

class SI7006Error(Exception):
    def __init__(self, value=0, msg="si7006 common error"):
        self.value = value
        self.msg = msg

    def __str__(self):
        return "Error code:%d, Error message: %s" % (self.value, str(self.msg))

    __repr__ = __str__


class SI7006(object):
    """
    This class implements SI7006 chip's functions.
    """
    def __init__(self):
        self.i2cDev = None

    def open(self, devid):
        self.i2cDev = I2C()
        self.i2cDev.open(devid)

    def getVer(self):
        """
        Get the firmware version of the chip.
        """

        reg = bytearray([Si7006_READ_Firmware_Revision_0, Si7006_READ_Firmware_Revision_1])
        self.i2cDev.write(reg)
        sleep_ms(30)
        version = bytearray(1)
        self.i2cDev.read(version)
        return version[0]

    def getID(self):
        """Get the chip ID."""

        reg = bytearray([Si7006_READ_ID_LOW_0, Si7006_READ_ID_LOW_1])
        self.i2cDev.write(reg)
        sleep_ms(30)
        id_buf_low = bytearray(4)
        self.i2cDev.read(id_buf_low)

        reg = bytearray([Si7006_READ_ID_HIGH_0, Si7006_READ_ID_HIGH_1])
        id_buf_high = bytearray(4)
        self.i2cDev.read(id_buf_high)

        return id_buf_low + id_buf_high

    def getTemperature(self):
        """Get temperature."""

        reg = bytearray([Si7006_MEAS_TEMP_NO_MASTER_MODE])
        self.i2cDev.write(reg)
        sleep_ms(30)
        readData = bytearray(2)
        self.i2cDev.read(readData)
        value = (readData[0] << 8 | readData[1])

        if (value & 0xFFFC):
            temperature = (175.72 * value) / 65536.0 - 46.85
            return temperature
        else:
            raise SI7006Error("failed to get temperature.")


    def getHumidity(self):
        """Get humidity."""

        reg = bytearray([Si7006_MEAS_REL_HUMIDITY_NO_MASTER_MODE])
        self.i2cDev.write(reg)
        sleep_ms(30)
        readData = bytearray(2)
        self.i2cDev.read(readData)
        value = (readData[0] << 8) | readData[1]

        if (value & 0xFFFE):
            humidity = (125.0 * value) / 65535.0 - 6.0
            return humidity
        else:
            raise SI7006Error("failed to get humidity.")

    def getTempHumidity(self):
        """Get temperature and humidity."""

        temphumidity = [0, 0]
        temphumidity[0] = self.getTemperature()
        temphumidity[1] = self.getHumidity()
        return temphumidity

    def close(self):
        self.i2cDev.close()
