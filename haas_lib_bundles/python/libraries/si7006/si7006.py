"""
Copyright (C) 2015-2021 Alibaba Group Holding Limited

    MicroPython's drive for SI7006

    Author: HaaS
    Date:   2021/09/09
"""

from driver import I2C
from utime import sleep_ms


# The commands provided by SI7006
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
    # i2cDev should be an I2C object and it should be opened before __init__ is called
    def __init__(self, i2cDev):
        self._i2cDev = None
        if not isinstance(i2cDev, I2C):
            raise ValueError("parameter is not an I2C object")
        # make SI7006's internal object points to i2cDev
        self._i2cDev = i2cDev

    def getVer(self):
        """
        Get the firmware version of the chip.
        """
        # make sure SI7006's internal object is valid before I2C operation
        if self._i2cDev is None:
            raise ValueError("invalid I2C object")
        # send read firmware version command to SI7006
        reg = bytearray([Si7006_READ_Firmware_Revision_0, Si7006_READ_Firmware_Revision_1])
        self._i2cDev.write(reg)
        sleep_ms(30)
        version = bytearray(1)
        # read the version info back
        self._i2cDev.read(version)
        return version[0]

    def getID(self):
        """Get the chip ID."""
        # make sure SI7006's internal object is valid before I2C operation
        if self._i2cDev is None:
            raise ValueError("invalid I2C object")

        # send read chip id‘s lower part command to SI7006
        reg = bytearray([Si7006_READ_ID_LOW_0, Si7006_READ_ID_LOW_1])
        self._i2cDev.write(reg)
        sleep_ms(30)
        id_buf_low = bytearray(4)
        # read the id info back
        self._i2cDev.read(id_buf_low)

        # send read chip id‘s higher part command to SI7006
        reg = bytearray([Si7006_READ_ID_HIGH_0, Si7006_READ_ID_HIGH_1])
        id_buf_high = bytearray(4)
        self._i2cDev.read(id_buf_high)

        return id_buf_low + id_buf_high

    def getTemperature(self):
        """Get temperature."""
        # make sure SI7006's internal object is valid before I2C operation
        if self._i2cDev is None:
            raise ValueError("invalid I2C object")

        # send measure temperature command to SI7006
        reg = bytearray([Si7006_MEAS_TEMP_NO_MASTER_MODE])
        self._i2cDev.write(reg)
        # wait for 30ms to wait for the measure finish according to SI7006's datasheet
        sleep_ms(30)
        readData = bytearray(2)
        # read the temperature measure result
        self._i2cDev.read(readData)
        value = (readData[0] << 8 | readData[1])

        # convert to actual temperature
        if (value & 0xFFFC):
            temperature = (175.72 * value) / 65536.0 - 46.85
            return round(temperature, 1)
        else:
            raise SI7006Error("failed to get temperature.")

    def getHumidity(self):
        """Get humidity."""
        # make sure SI7006's internal object is valid before I2C operation
        if self._i2cDev is None:
            raise ValueError("invalid I2C object")

        # send measure humidity command to SI7006
        reg = bytearray([Si7006_MEAS_REL_HUMIDITY_NO_MASTER_MODE])
        self._i2cDev.write(reg)
        # wait for 30ms to wait for the measure finish according to SI7006's datasheet
        sleep_ms(30)
        readData = bytearray(2)
        self._i2cDev.read(readData)
        value = (readData[0] << 8) | readData[1]

        # convert to actual humidity
        if (value & 0xFFFE):
            humidity = (125.0 * value) / 65535.0 - 6.0
            return round(humidity)
        else:
            raise SI7006Error("failed to get humidity.")

    def getTempHumidity(self):
        """Get temperature and humidity."""
        # make sure SI7006's internal object is valid before I2C operation
        if self._i2cDev is None:
            raise ValueError("invalid I2C object")

        # read tempperature and humidity in sequence
        temphumidity = [0, 0]
        temphumidity[0] = self.getTemperature()
        temphumidity[1] = self.getHumidity()
        return temphumidity


if __name__ == "__main__":
    '''
    The below i2c configuration is needed in your board.json.
    "si7006": {
    "type": "I2C",
    "port": 1,
    "addrWidth": 7,
    "freq": 400000,
    "mode": "master",
    "devAddr": 64
    }
    '''
    print("Testing si7006 ...")

    i2cDev = I2C()
    i2cDev.open("si7006")

    si7006Dev = SI7006(i2cDev)

    version = si7006Dev.getVer()
    print("si7006 version is: %d" % version)

    chipID = si7006Dev.getID()
    print("si7006 chip id is:", chipID)


    temperature = si7006Dev.getTemperature()
    print("The temperature is: %f" % temperature)

    humidity = si7006Dev.getHumidity()
    print("The humidity is: %d" % humidity)

    i2cDev.close()

    print("Test si7006 done!")

