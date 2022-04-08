"""
Copyright (C) 2015-2021 Alibaba Group Holding Limited
`HaaS Python adxl34x`
====================================================

A driver for the ADXL34x 3-axis accelerometer family

* Author(s): HaaS Group

Based on drivers by Bryan Siepert, K. Townsend and Tony DiCola


Implementation Notes
--------------------

**Hardware:**

* HaaS Python adxl34x
  https://haas.iot.aliyun.com/solution/detail/hardware

**Software and Dependencies:**

* HaaS Python API documents:
  https://haas.iot.aliyun.com/haasapi/index.html#/

* HaaS Python Driver Libraries:
  https://github.com/alibaba/AliOS-Things/tree/master/haas_lib_bundles/python/libraries


"""

# SPDX-FileCopyrightText: 2018 Bryan Siepert for Adafruit Industries
#
# SPDX-License-Identifier: MIT

from driver import I2C
from struct import unpack
from micropython import const

_ADXL345_DEFAULT_ADDRESS: int = const(0x53)  # Assumes ALT address pin low

# Conversion factors
_STANDARD_GRAVITY: float = 9.80665  # earth standard gravity
_ADXL345_MG2G_MULTIPLIER: float = 0.004  # 4mg per lsb

_REG_DEVID: int = const(0x00)  # Device ID
_REG_THRESH_TAP: int = const(0x1D)  # Tap threshold
_REG_OFSX: int = const(0x1E)  # X-axis offset
_REG_OFSY: int = const(0x1F)  # Y-axis offset
_REG_OFSZ: int = const(0x20)  # Z-axis offset
_REG_DUR: int = const(0x21)  # Tap duration
_REG_LATENT: int = const(0x22)  # Tap latency
_REG_WINDOW: int = const(0x23)  # Tap window
_REG_THRESH_ACT: int = const(0x24)  # Activity threshold
_REG_THRESH_INACT: int = const(0x25)  # Inactivity threshold
_REG_TIME_INACT: int = const(0x26)  # Inactivity time
_REG_ACT_INACT_CTL: int = const(0x27)  # Axis enable control for [in]activity detection
_REG_THRESH_FF: int = const(0x28)  # Free-fall threshold
_REG_TIME_FF: int = const(0x29)  # Free-fall time
_REG_TAP_AXES: int = const(0x2A)  # Axis control for single/double tap
_REG_ACT_TAP_STATUS: int = const(0x2B)  # Source for single/double tap
_REG_BW_RATE: int = const(0x2C)  # Data rate and power mode control
_REG_POWER_CTL: int = const(0x2D)  # Power-saving features control
_REG_INT_ENABLE: int = const(0x2E)  # Interrupt enable control
_REG_INT_MAP: int = const(0x2F)  # Interrupt mapping control
_REG_INT_SOURCE: int = const(0x30)  # Source of interrupts
_REG_DATA_FORMAT: int = const(0x31)  # Data format control
_REG_DATAX0: int = const(0x32)  # X-axis data 0
_REG_DATAX1: int = const(0x33)  # X-axis data 1
_REG_DATAY0: int = const(0x34)  # Y-axis data 0
_REG_DATAY1: int = const(0x35)  # Y-axis data 1
_REG_DATAZ0: int = const(0x36)  # Z-axis data 0
_REG_DATAZ1: int = const(0x37)  # Z-axis data 1
_REG_FIFO_CTL: int = const(0x38)  # FIFO control
_REG_FIFO_STATUS: int = const(0x39)  # FIFO status
_INT_SINGLE_TAP: int = const(0b01000000)  # SINGLE_TAP bit
_INT_DOUBLE_TAP: int = const(0b00100000)  # DOUBLE_TAP bit
_INT_ACT: int = const(0b00010000)  # ACT bit
_INT_INACT: int = const(0b00001000)  # INACT bit
_INT_FREE_FALL: int = const(0b00000100)  # FREE_FALL  bit


class DataRate:  # pylint: disable=too-few-public-methods
    """An enum-like class representing the possible data rates.

    Possible values are:

    - ``DataRate.RATE_3200_HZ``
    - ``DataRate.RATE_1600_HZ``
    - ``DataRate.RATE_800_HZ``
    - ``DataRate.RATE_400_HZ``
    - ``DataRate.RATE_200_HZ``
    - ``DataRate.RATE_100_HZ``
    - ``DataRate.RATE_50_HZ``
    - ``DataRate.RATE_25_HZ``
    - ``DataRate.RATE_12_5_HZ``
    - ``DataRate.RATE_6_25HZ``
    - ``DataRate.RATE_3_13_HZ``
    - ``DataRate.RATE_1_56_HZ``
    - ``DataRate.RATE_0_78_HZ``
    - ``DataRate.RATE_0_39_HZ``
    - ``DataRate.RATE_0_20_HZ``
    - ``DataRate.RATE_0_10_HZ``

    """

    RATE_3200_HZ: int = const(0b1111)  # 1600Hz Bandwidth   140mA IDD
    RATE_1600_HZ: int = const(0b1110)  # 800Hz Bandwidth    90mA IDD
    RATE_800_HZ: int = const(0b1101)  # 400Hz Bandwidth   140mA IDD
    RATE_400_HZ: int = const(0b1100)  # 200Hz Bandwidth   140mA IDD
    RATE_200_HZ: int = const(0b1011)  # 100Hz Bandwidth   140mA IDD
    RATE_100_HZ: int = const(0b1010)  # 50Hz Bandwidth   140mA IDD
    RATE_50_HZ: int = const(0b1001)  # 25Hz Bandwidth    90mA IDD
    RATE_25_HZ: int = const(0b1000)  # 12.5Hz Bandwidth    60mA IDD
    RATE_12_5_HZ: int = const(0b0111)  # 6.25Hz Bandwidth    50mA IDD
    RATE_6_25HZ: int = const(0b0110)  # 3.13Hz Bandwidth    45mA IDD
    RATE_3_13_HZ: int = const(0b0101)  # 1.56Hz Bandwidth    40mA IDD
    RATE_1_56_HZ: int = const(0b0100)  # 0.78Hz Bandwidth    34mA IDD
    RATE_0_78_HZ: int = const(0b0011)  # 0.39Hz Bandwidth    23mA IDD
    RATE_0_39_HZ: int = const(0b0010)  # 0.20Hz Bandwidth    23mA IDD
    RATE_0_20_HZ: int = const(0b0001)  # 0.10Hz Bandwidth    23mA IDD
    RATE_0_10_HZ: int = const(0b0000)  # 0.05Hz Bandwidth    23mA IDD (default value)


class Range:  # pylint: disable=too-few-public-methods
    """An enum-like class representing the possible measurement ranges in +/- G.

    Possible values are:

    - ``Range.RANGE_16_G``
    - ``Range.RANGE_8_G``
    - ``Range.RANGE_4_G``
    - ``Range.RANGE_2_G``

    """

    RANGE_16_G: int = const(0b11)  # +/- 16g
    RANGE_8_G: int = const(0b10)  # +/- 8g
    RANGE_4_G: int = const(0b01)  # +/- 4g
    RANGE_2_G: int = const(0b00)  # +/- 2g (default value)


class ADXL345:
    """Driver for the ADXL345 3 axis accelerometer

    :param ~busio.I2C i2c: The I2C bus the ADXL345 is connected to.
    :param int address: The I2C device address for the sensor. Default is :const:`0x53`.

    **Quickstart: Importing and using the device**

        Here is an example of using the :class:`ADXL345` class.
        First you will need to import the libraries to use the sensor

        .. code-block:: python

            import board
            import adafruit_adxl34x

        Once this is done you can define your `board.I2C` object and define your sensor object

        .. code-block:: python

            i2c = board.I2C()  # uses board.SCL and board.SDA
            accelerometer = adafruit_adxl34x.ADXL343(i2c)


        Now you have access to the :attr:`acceleration` attribute

        .. code-block:: python

            acceleration = accelerometer.acceleration

    """

    def __init__(self, *args, **kwargs):
        self._i2cDev = None

        if not isinstance(args[0], I2C):
            raise ValueError("parameter is not an I2C object")

        #实例化
        self._i2cDev=args[0]

        # Check that the acclerometer is connected, then enable it.
        if self._read_register(_REG_DEVID, 1)[0] == 0xE5:
            self._write_register_byte(_REG_POWER_CTL, 0x08)
        else:
            raise RuntimeError('Failed to find the expected device ID register value, check your wiring.')

        # set the 'measure' bit in to enable measurement
        self._write_register_byte(_REG_POWER_CTL, 0x08)
        self._write_register_byte(_REG_INT_ENABLE, 0x0)

        # set output data rate to 25Hz
        self.OutputRage = DataRate.RATE_25_HZ
        self.data_rate = self.OutputRage

        # set detection range to +-16G
        self.FSRange = Range.RANGE_16_G
        self.range = self.FSRange


        self._enabled_interrupts = {}
        self._event_status = {}

    @property
    def acceleration(self) -> Tuple[int, int, int]:
        """The x, y, z acceleration values returned in a 3-tuple in :math:`m / s ^ 2`"""
        x, y, z = unpack("<hhh", self._read_register(_REG_DATAX0, 6))

        x = x * _ADXL345_MG2G_MULTIPLIER * _STANDARD_GRAVITY
        y = y * _ADXL345_MG2G_MULTIPLIER * _STANDARD_GRAVITY
        z = z * _ADXL345_MG2G_MULTIPLIER * _STANDARD_GRAVITY

        return x, y, z

    @property
    def raw_x(self) -> int:
        """The raw x value."""
        return unpack("<h", self._read_register(_REG_DATAX0, 2))[0]

    @property
    def raw_y(self) -> int:
        """The raw y value."""
        return unpack("<h", self._read_register(_REG_DATAY0, 2))[0]

    @property
    def raw_z(self) -> int:
        """The raw z value."""
        return unpack("<h", self._read_register(_REG_DATAZ0, 2))[0]

    @property
    def events(self) -> Dict[str, bool]:
        """
        :attr:`events` will return a dictionary with a key for each
        event type that has been enabled.
        The possible keys are:

        +------------+----------------------------------------------------------------------------+
        | Key        | Description                                                                |
        +============+============================================================================+
        | ``tap``    | True if a tap was detected recently. Whether it's looking for a single or  |
        |            | double tap is determined by the tap param of `enable_tap_detection`        |
        +------------+----------------------------------------------------------------------------+
        | ``motion`` | True if the sensor has seen acceleration above the threshold               |
        |            | set with `enable_motion_detection`.                                        |
        +------------+----------------------------------------------------------------------------+
        |``freefall``| True if the sensor was in freefall. Parameters are set when enabled with   |
        |            | `enable_freefall_detection`                                                |
        +------------+----------------------------------------------------------------------------+


        """

        interrupt_source_register = self._read_clear_interrupt_source()

        self._event_status.clear()

        for event_type, value in self._enabled_interrupts.items():
            if event_type == "motion":
                self._event_status[event_type] = (
                    interrupt_source_register & _INT_ACT > 0
                )
            if event_type == "tap":
                if value == 1:
                    self._event_status[event_type] = (
                        interrupt_source_register & _INT_SINGLE_TAP > 0
                    )
                else:
                    self._event_status[event_type] = (
                        interrupt_source_register & _INT_DOUBLE_TAP > 0
                    )
            if event_type == "freefall":
                self._event_status[event_type] = (
                    interrupt_source_register & _INT_FREE_FALL > 0
                )

        return self._event_status

    def enable_motion_detection(self, *, threshold: int = 18):
        """
        The activity detection parameters.

        :param int threshold: The value that acceleration on any axis must exceed to\
        register as active. The scale factor is 62.5 mg/LSB.

        If you wish to set them yourself rather than using the defaults,
        you must use keyword arguments::

            accelerometer.enable_motion_detection(threshold=20)

        """
        active_interrupts = self._read_register_unpacked(_REG_INT_ENABLE)

        self._write_register_byte(_REG_INT_ENABLE, 0x0)  # disable interrupts for setup
        self._write_register_byte(
            _REG_ACT_INACT_CTL, 0b01110000
        )  # enable activity on X,Y,Z
        self._write_register_byte(_REG_THRESH_ACT, threshold)
        self._write_register_byte(_REG_INT_ENABLE, _INT_ACT)  # Inactive interrupt only

        active_interrupts |= _INT_ACT
        self._write_register_byte(_REG_INT_ENABLE, active_interrupts)
        self._enabled_interrupts["motion"] = True

    def disable_motion_detection(self) -> None:
        """
        Disable motion detection
        """
        active_interrupts = self._read_register_unpacked(_REG_INT_ENABLE)
        active_interrupts &= ~_INT_ACT
        self._write_register_byte(_REG_INT_ENABLE, active_interrupts)
        self._enabled_interrupts.pop("motion")

    def enable_freefall_detection(self, *, threshold: int = 10, time: int = 25) -> None:
        """
        Freefall detection parameters:

        :param int threshold: The value that acceleration on all axes must be under to\
        register as dropped. The scale factor is 62.5 mg/LSB.

        :param int time: The amount of time that acceleration on all axes must be less than\
        ``threshold`` to register as dropped. The scale factor is 5 ms/LSB. Values between 100 ms\
        and 350 ms (20 to 70) are recommended.

        If you wish to set them yourself rather than using the defaults,
        you must use keyword arguments:

        .. code-block:: python

            accelerometer.enable_freefall_detection(time=30)

       """

        active_interrupts = self._read_register_unpacked(_REG_INT_ENABLE)

        self._write_register_byte(_REG_INT_ENABLE, 0x0)  # disable interrupts for setup
        self._write_register_byte(_REG_THRESH_FF, threshold)
        self._write_register_byte(_REG_TIME_FF, time)

        # add FREE_FALL to the active interrupts and set them to re-enable
        active_interrupts |= _INT_FREE_FALL
        self._write_register_byte(_REG_INT_ENABLE, active_interrupts)
        self._enabled_interrupts["freefall"] = True

    def disable_freefall_detection(self) -> None:
        """Disable freefall detection"""
        active_interrupts = self._read_register_unpacked(_REG_INT_ENABLE)
        active_interrupts &= ~_INT_FREE_FALL
        self._write_register_byte(_REG_INT_ENABLE, active_interrupts)
        self._enabled_interrupts.pop("freefall")

    def enable_tap_detection(
        self,
        *,
        tap_count: int = 1,
        threshold: int = 20,
        duration: int = 50,
        latency: int = 20,
        window: int = 255
    ):
        """
        The tap detection parameters.

        :param int tap_count: 1 to detect only single taps, and 2 to detect only double taps.

        :param int threshold: A threshold for the tap detection. The scale factor is 62.5 mg/LSB\
        The higher the value the less sensitive the detection.

        :param int duration: This caps the duration of the impulse above ``threshold``.\
        Anything above ``duration`` won't register as a tap. The scale factor is 625 µs/LSB

        :param int latency: (double tap only) The length of time after the initial impulse\
        falls below ``threshold`` to start the window looking for a second impulse.\
        The scale factor is 1.25 ms/LSB.

        :param int window: (double tap only) The length of the window in which to look for a\
        second tap. The scale factor is 1.25 ms/LSB

        If you wish to set them yourself rather than using the defaults,
        you must use keyword arguments:

        .. code-block:: python

            accelerometer.enable_tap_detection(duration=30, threshold=25)

        """
        active_interrupts = self._read_register_unpacked(_REG_INT_ENABLE)

        self._write_register_byte(_REG_INT_ENABLE, 0x0)  # disable interrupts for setup
        self._write_register_byte(
            _REG_TAP_AXES, 0b00000111
        )  # enable X, Y, Z axes for tap
        self._write_register_byte(_REG_THRESH_TAP, threshold)
        self._write_register_byte(_REG_DUR, duration)

        if tap_count == 1:
            active_interrupts |= _INT_SINGLE_TAP
            self._write_register_byte(_REG_INT_ENABLE, active_interrupts)
            self._enabled_interrupts["tap"] = 1
        elif tap_count == 2:
            self._write_register_byte(_REG_LATENT, latency)
            self._write_register_byte(_REG_WINDOW, window)

            active_interrupts |= _INT_DOUBLE_TAP
            self._write_register_byte(_REG_INT_ENABLE, active_interrupts)
            self._enabled_interrupts["tap"] = 2
        else:
            raise ValueError(
                "tap must be 0 to disable, 1 for single tap, or 2 for double tap"
            )

    def disable_tap_detection(self) -> None:
        """Disable tap detection"""
        active_interrupts = self._read_register_unpacked(_REG_INT_ENABLE)
        active_interrupts &= ~_INT_SINGLE_TAP
        active_interrupts &= ~_INT_DOUBLE_TAP
        self._write_register_byte(_REG_INT_ENABLE, active_interrupts)
        self._enabled_interrupts.pop("tap")

    @property
    def data_rate(self) -> int:
        """The data rate of the sensor."""
        rate_register = self._read_register_unpacked(_REG_BW_RATE)
        return rate_register & 0x0F

    @data_rate.setter
    def data_rate(self, val: int) -> None:
        self._write_register_byte(_REG_BW_RATE, val)

    @property
    def range(self) -> int:
        """The measurement range of the sensor."""
        range_register = self._read_register_unpacked(_REG_DATA_FORMAT)
        return range_register & 0x03

    @range.setter
    def range(self, val: int) -> None:
        # read the current value of the data format register
        format_register = self._read_register_unpacked(_REG_DATA_FORMAT)

        # clear the bottom 4 bits and update the data rate
        format_register &= ~0x0F
        format_register |= val

        # Make sure that the FULL-RES bit is enabled for range scaling
        format_register |= 0x08

        # write the updated values
        self._write_register_byte(_REG_DATA_FORMAT, format_register)

    @property
    def offset(self) -> Tuple[int, int, int]:
        """
        The x, y, z offsets as a tuple of raw count values.

        See offset_calibration example for usage.
        """
        x_offset, y_offset, z_offset = unpack("<bbb", self._read_register(_REG_OFSX, 3))
        return x_offset, y_offset, z_offset

    @offset.setter
    def offset(self, val: Tuple[int, int, int]) -> None:
        x_offset, y_offset, z_offset = val
        self._write_register_byte(_REG_OFSX, x_offset)
        self._write_register_byte(_REG_OFSY, y_offset)
        self._write_register_byte(_REG_OFSZ, z_offset)

    def _read_clear_interrupt_source(self) -> int:
        return self._read_register_unpacked(_REG_INT_SOURCE)

    def _read_register_unpacked(self, register: int) -> int:
        return unpack("<b", self._read_register(register, 1))[0]

    def _read_register(self, register: int, length: int) -> int:
        buf = bytearray(length)
        self._i2cDev.memRead(buf, register & 0xFF, 8)
        return buf[0:length]

    def _write_register_byte(self, register: int, value: int) -> None:
        buf = bytearray([register & 0xFF, value & 0xFF])
        self._i2cDev.write(buf)

    # 设置测量频率
    def setDataRate(self, val: int) -> None:
        self.OutputRage = val
        self.data_rate = self.OutputRage

    # 设置量测范围
    def setRange(self, val: int) -> None:
        self.FSRange = val
        self.range = self.FSRange

    # 得到加速度值(原始值)
    # (ax,ay,az):陀螺仪x,y,z轴的重力加速度，单位：m/s²
    # 返回值:0,成功
    # 其他,错误代码
    def getAcceleration(self):
        return self.acceleration

class ADXL343(ADXL345):
    """
    Stub class for the ADXL343 3-axis accelerometer
    """
if __name__ == "__main__":
    import utime
    # coding=utf-8

    i2cObj = I2C()
    i2cObj.open("adxl345_esp32")
    gyroDev = ADXL345(i2cObj)
    print('Reading ADXL345 values, press Ctrl-C to quit...')

    while True:
        gyroDev.setRange(Range.RANGE_2_G)
        print('range:2G - value:', gyroDev.getAcceleration())
        utime.sleep(1)
        gyroDev.setRange(Range.RANGE_4_G)
        print('range:4G - value:', gyroDev.getAcceleration())
        utime.sleep(1)
        gyroDev.setRange(Range.RANGE_8_G)
        print('range:8G - value:', gyroDev.getAcceleration())
        utime.sleep(1)
        gyroDev.setRange(Range.RANGE_16_G)
        print('range:16G - value:', gyroDev.getAcceleration())
        utime.sleep(1)

    i2cObj.close()
    del i2cObj

