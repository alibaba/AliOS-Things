"""
HaaSPython I2C driver for mlx90614 IR Temperature
"""

import ustruct
from driver import I2C

class SensorBase:

    def read16(self, register):
        data=bytearray(2)
        self._i2cDev.memRead(data,register,8)
        return ustruct.unpack('<H', data)[0]

    def read_temp(self, register):
        temp = self.read16(register)
        # apply measurement resolution (0.02 degrees per LSB)
        temp *= .02
        temp -= 273.15
        return temp

    def read_ambient_temp(self):
        return self.read_temp(self._REGISTER_TA)

    def read_object_temp(self):
        return self.read_temp(self._REGISTER_TOBJ1)

    def read_object2_temp(self):
        if self.dual_zone:
            return self.read_temp(self._REGISTER_TOBJ2)
        else:
            raise RuntimeError("Device only has one thermopile")

    @property
    def ambient_temp(self):
        return self.read_ambient_temp()

    @property
    def object_temp(self):
        return self.read_object_temp()

    @property
    def object2_temp(self):
        return self.read_object2_temp()

class MLX90614(SensorBase):

    _REGISTER_TA = 0x06
    _REGISTER_TOBJ1 = 0x07
    _REGISTER_TOBJ2 = 0x08

    def __init__(self, i2cDev, address=0x5a):
        self._i2cDev = None
        if not isinstance(i2cDev, I2C):
            raise ValueError("parameter is not an I2C object")
        self._i2cDev = i2cDev
        _config1=bytearray(2)
        self._i2cDev.memRead(_config1,0x25,8)
        _dz = ustruct.unpack('<H', _config1)[0] & (1<<6)
        self.dual_zone = True if _dz else False
