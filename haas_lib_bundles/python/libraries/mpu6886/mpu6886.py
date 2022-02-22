"""
HaaSPython I2C driver for MPU6886 6-axis motion tracking device
"""

# pylint: disable=import-error
import ustruct
import utime
# from machine import I2C, Pin
from driver import I2C
from micropython import const
# pylint: enable=import-error

_CONFIG = const(0x1a)
_GYRO_CONFIG = const(0x1b)
_ACCEL_CONFIG = const(0x1c)
_ACCEL_CONFIG2 = const(0x1d)
_ACCEL_XOUT_H = const(0x3b)
_ACCEL_XOUT_L = const(0x3c)
_ACCEL_YOUT_H = const(0x3d)
_ACCEL_YOUT_L = const(0x3e)
_ACCEL_ZOUT_H = const(0x3f)
_ACCEL_ZOUT_L = const(0x40)
_TEMP_OUT_H = const(0x41)
_TEMP_OUT_L = const(0x42)
_GYRO_XOUT_H = const(0x43)
_GYRO_XOUT_L = const(0x44)
_GYRO_YOUT_H = const(0x45)
_GYRO_YOUT_L = const(0x46)
_GYRO_ZOUT_H = const(0x47)
_GYRO_ZOUT_L = const(0x48)
_PWR_MGMT_1 = const(0x6b)
_WHO_AM_I = const(0x75)

ACCEL_FS_SEL_2G = const(0b00000000)
ACCEL_FS_SEL_4G = const(0b00001000)
ACCEL_FS_SEL_8G = const(0b00010000)
ACCEL_FS_SEL_16G = const(0b00011000)

_ACCEL_SO_2G = 16384 # 1 / 16384 ie. 0.061 mg / digit
_ACCEL_SO_4G = 8192 # 1 / 8192 ie. 0.122 mg / digit
_ACCEL_SO_8G = 4096 # 1 / 4096 ie. 0.244 mg / digit
_ACCEL_SO_16G = 2048 # 1 / 2048 ie. 0.488 mg / digit

GYRO_FS_SEL_250DPS = const(0b00000000)
GYRO_FS_SEL_500DPS = const(0b00001000)
GYRO_FS_SEL_1000DPS = const(0b00010000)
GYRO_FS_SEL_2000DPS = const(0b00011000)

_GYRO_SO_250DPS = 131
_GYRO_SO_500DPS = 62.5
_GYRO_SO_1000DPS = 32.8
_GYRO_SO_2000DPS = 16.4

_TEMP_SO = 326.8
_TEMP_OFFSET = 25

SF_G = 1
SF_M_S2 = 9.80665 # 1 g = 9.80665 m/s2 ie. standard gravity
SF_DEG_S = 1
SF_RAD_S = 0.017453292519943 # 1 deg/s is 0.017453292519943 rad/s

class MPU6886:
    def __init__(self, i2cDev):
        self._i2cDev = None
        if not isinstance(i2cDev, I2C):
            raise ValueError("parameter is not an I2C object")

        # make AHB21B's internal object points to _i2cDev
        self._i2cDev = i2cDev
        self.start()

    def start(self,address=0x68):
        self.address = address

        if 0x19 != self.whoami:
            raise RuntimeError("MPU6886 not found in I2C bus.")

        self._register_char(_PWR_MGMT_1, 0b10000000) # reset
        utime.sleep_ms(100)
        self._register_char(_PWR_MGMT_1, 0b00000001) # autoselect clock

        accel_fs=ACCEL_FS_SEL_2G
        gyro_fs=GYRO_FS_SEL_250DPS
        accel_sf=SF_M_S2
        gyro_sf=SF_RAD_S
        gyro_offset=(0, 0, 0)

        self._accel_so = self._accel_fs(accel_fs)
        self._gyro_so = self._gyro_fs(gyro_fs)
        self._accel_sf = accel_sf
        self._gyro_sf = gyro_sf
        self._gyro_offset = gyro_offset

    @property
    def acceleration(self):
        """
        Acceleration measured by the sensor. By default will return a
        3-tuple of X, Y, Z axis acceleration values in m/s^2 as floats. Will
        return values in g if constructor was provided `accel_sf=SF_M_S2`
        parameter.
        """
        so = self._accel_so
        sf = self._accel_sf

        xyz = self._register_three_shorts(_ACCEL_XOUT_H)
        return tuple([value / so * sf for value in xyz])

    @property
    def gyro(self):
        """
        X, Y, Z radians per second as floats.
        """
        so = self._gyro_so
        sf = self._gyro_sf
        ox, oy, oz = self._gyro_offset

        xyz = self._register_three_shorts(_GYRO_XOUT_H)
        xyz = [value / so * sf for value in xyz]

        xyz[0] -= ox
        xyz[1] -= oy
        xyz[2] -= oz

        return tuple(xyz)

    @property
    def temperature(self):
        """
        Die temperature in celcius as a float.
        """
        temp = self._register_short(_TEMP_OUT_H)
        # return ((temp - _TEMP_OFFSET) / _TEMP_SO) + _TEMP_OFFSET
        return (temp / _TEMP_SO) +  _TEMP_OFFSET

    @property
    def whoami(self):
        """ Value of the whoami register. """
        return self._register_char(_WHO_AM_I)

    def calibrate(self, count=256, delay=0):
        ox, oy, oz = (0.0, 0.0, 0.0)
        self._gyro_offset = (0.0, 0.0, 0.0)
        n = float(count)

        while count:
            utime.sleep_ms(delay)
            gx, gy, gz = self.gyro
            ox += gx
            oy += gy
            oz += gz
            count -= 1

        self._gyro_offset = (ox / n, oy / n, oz / n)
        return self._gyro_offset

    def _register_short(self, register, value=None, buf=bytearray(2)):
        if value is None:
            # cmd = bytearray(2)
            # cmd[0] = self.address
            # cmd[1] = register
            # self._i2cDev.write(cmd)
            # self._i2cDev.read(buf)
            self._i2cDev.memRead(buf,register,8)
            print(buf[0])
            # self.i2c.readfrom_mem_into(self.address, register, buf)
            return ustruct.unpack(">h", buf)[0]

        ustruct.pack_into(">h", buf, 0, value)
        # cmd = bytearray(2)
        # cmd[0] = self.address
        # cmd[1] = register
        # self._i2cDev.write(cmd)
        # self._i2cDev.write(buf)
        self._i2cDev.memWrite(buf,register,8)
        # return self.i2c.writeto_mem(self.address, register, buf)

    def _register_three_shorts(self, register, buf=bytearray(6)):
        # cmd = bytearray(2)
        # cmd[0] = self.address
        # cmd[1] = register
        # self._i2cDev.write(cmd)
        # self._i2cDev.read(buf)
        self._i2cDev.memRead(buf,register,8)
        # self.i2c.readfrom_mem_into(self.address, register, buf)
        return ustruct.unpack(">hhh", buf)

    def _register_char(self, register, value=None, buf=bytearray(1)):
        if value is None:
            # cmd = bytearray(2)
            # cmd[0] = self.address
            # cmd[1] = register
            # self._i2cDev.write(cmd)
            # self._i2cDev.read(buf)
            self._i2cDev.memRead(buf,register,8)
            print(buf[0])
            # self.i2c.readfrom_mem_into(self.address, register, buf)
            return buf[0]

        ustruct.pack_into("<b", buf, 0, value)
        # cmd = bytearray(2)
        # cmd[0] = self.address
        # cmd[1] = register
        # self._i2cDev.write(cmd)
        # self._i2cDev.write(buf)
        return self._i2cDev.memWrite(buf,register,8)
        # return self.i2c.writeto_mem(self.address, register, buf)

    def _accel_fs(self, value):
        self._register_char(_ACCEL_CONFIG, value)

        # Return the sensitivity divider
        if ACCEL_FS_SEL_2G == value:
            return _ACCEL_SO_2G
        elif ACCEL_FS_SEL_4G == value:
            return _ACCEL_SO_4G
        elif ACCEL_FS_SEL_8G == value:
            return _ACCEL_SO_8G
        elif ACCEL_FS_SEL_16G == value:
            return _ACCEL_SO_16G

    def _gyro_fs(self, value):
        self._register_char(_GYRO_CONFIG, value)

        # Return the sensitivity divider
        if GYRO_FS_SEL_250DPS == value:
            return _GYRO_SO_250DPS
        elif GYRO_FS_SEL_500DPS == value:
            return _GYRO_SO_500DPS
        elif GYRO_FS_SEL_1000DPS == value:
            return _GYRO_SO_1000DPS
        elif GYRO_FS_SEL_2000DPS == value:
            return _GYRO_SO_2000DPS

    def __enter__(self):
        return self

    def __exit__(self, exception_type, exception_value, traceback):
        pass
