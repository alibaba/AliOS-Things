"""
Copyright (C) 2015-2021 Alibaba Group Holding Limited
`HaaS Python tcs34725`
====================================================

A driver for tcs34725 Color Sensor

* Author(s): HaaS Group

Implementation Notes
--------------------

**Hardware:**

* HaaS Python tcs34725:
  https://haas.iot.aliyun.com/solution/detail/hardware

**Software and Dependencies:**

* HaaS Python API documents:
  https://haas.iot.aliyun.com/haasapi/index.html#/

* HaaS Python Driver Libraries:
  https://github.com/alibaba/AliOS-Things/tree/master/haas_lib_bundles/python/libraries

"""

# The MIT License (MIT)
#
# Copyright (c) 2016 Adafruit Industries
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

import time
from driver import I2C

TCS34725_ADDRESS          = 0x29
TCS34725_ID               = 0x12 # 0x44 = TCS34721/TCS34725, 0x4D = TCS34723/TCS34727

TCS34725_COMMAND_BIT      = 0x80

TCS34725_ENABLE           = 0x00
TCS34725_ENABLE_AIEN      = 0x10 # RGBC Interrupt Enable
TCS34725_ENABLE_WEN       = 0x08 # Wait enable - Writing 1 activates the wait timer
TCS34725_ENABLE_AEN       = 0x02 # RGBC Enable - Writing 1 actives the ADC, 0 disables it
TCS34725_ENABLE_PON       = 0x01 # Power on - Writing 1 activates the internal oscillator, 0 disables it
TCS34725_ATIME            = 0x01 # Integration time
TCS34725_WTIME            = 0x03 # Wait time (if TCS34725_ENABLE_WEN is asserted)
TCS34725_WTIME_2_4MS      = 0xFF # WLONG0 = 2.4ms   WLONG1 = 0.029s
TCS34725_WTIME_204MS      = 0xAB # WLONG0 = 204ms   WLONG1 = 2.45s
TCS34725_WTIME_614MS      = 0x00 # WLONG0 = 614ms   WLONG1 = 7.4s
TCS34725_AILTL            = 0x04 # Clear channel lower interrupt threshold
TCS34725_AILTH            = 0x05
TCS34725_AIHTL            = 0x06 # Clear channel upper interrupt threshold
TCS34725_AIHTH            = 0x07
TCS34725_PERS             = 0x0C # Persistence register - basic SW filtering mechanism for interrupts
TCS34725_PERS_NONE        = 0b0000 # Every RGBC cycle generates an interrupt
TCS34725_PERS_1_CYCLE     = 0b0001 # 1 clean channel value outside threshold range generates an interrupt
TCS34725_PERS_2_CYCLE     = 0b0010 # 2 clean channel values outside threshold range generates an interrupt
TCS34725_PERS_3_CYCLE     = 0b0011 # 3 clean channel values outside threshold range generates an interrupt
TCS34725_PERS_5_CYCLE     = 0b0100 # 5 clean channel values outside threshold range generates an interrupt
TCS34725_PERS_10_CYCLE    = 0b0101 # 10 clean channel values outside threshold range generates an interrupt
TCS34725_PERS_15_CYCLE    = 0b0110 # 15 clean channel values outside threshold range generates an interrupt
TCS34725_PERS_20_CYCLE    = 0b0111 # 20 clean channel values outside threshold range generates an interrupt
TCS34725_PERS_25_CYCLE    = 0b1000 # 25 clean channel values outside threshold range generates an interrupt
TCS34725_PERS_30_CYCLE    = 0b1001 # 30 clean channel values outside threshold range generates an interrupt
TCS34725_PERS_35_CYCLE    = 0b1010 # 35 clean channel values outside threshold range generates an interrupt
TCS34725_PERS_40_CYCLE    = 0b1011 # 40 clean channel values outside threshold range generates an interrupt
TCS34725_PERS_45_CYCLE    = 0b1100 # 45 clean channel values outside threshold range generates an interrupt
TCS34725_PERS_50_CYCLE    = 0b1101 # 50 clean channel values outside threshold range generates an interrupt
TCS34725_PERS_55_CYCLE    = 0b1110 # 55 clean channel values outside threshold range generates an interrupt
TCS34725_PERS_60_CYCLE    = 0b1111 # 60 clean channel values outside threshold range generates an interrupt
TCS34725_CONFIG           = 0x0D
TCS34725_CONFIG_WLONG     = 0x02 # Choose between short and long (12x) wait times via TCS34725_WTIME
TCS34725_CONTROL          = 0x0F # Set the gain level for the sensor
TCS34725_ID               = 0x12 # 0x44 = TCS34721/TCS34725, 0x4D = TCS34723/TCS34727
TCS34725_STATUS           = 0x13
TCS34725_STATUS_AINT      = 0x10 # RGBC Clean channel interrupt
TCS34725_STATUS_AVALID    = 0x01 # Indicates that the RGBC channels have completed an integration cycle

TCS34725_CDATAL           = 0x14 # Clear channel data
TCS34725_CDATAH           = 0x15
TCS34725_RDATAL           = 0x16 # Red channel data
TCS34725_RDATAH           = 0x17
TCS34725_GDATAL           = 0x18 # Green channel data
TCS34725_GDATAH           = 0x19
TCS34725_BDATAL           = 0x1A # Blue channel data
TCS34725_BDATAH           = 0x1B

TCS34725_INTEGRATIONTIME_2_4MS  = 0xFF   #  2.4ms - 1 cycle    - Max Count: 1024
TCS34725_INTEGRATIONTIME_24MS   = 0xF6   # 24ms  - 10 cycles  - Max Count: 10240
TCS34725_INTEGRATIONTIME_50MS   = 0xEB   #  50ms  - 20 cycles  - Max Count: 20480
TCS34725_INTEGRATIONTIME_101MS  = 0xD5   #  101ms - 42 cycles  - Max Count: 43008
TCS34725_INTEGRATIONTIME_154MS  = 0xC0   #  154ms - 64 cycles  - Max Count: 65535
TCS34725_INTEGRATIONTIME_240MS  = 0x9C   #  240ms - 100 cycles  - Max Count: 65535
TCS34725_INTEGRATIONTIME_700MS  = 0x00   #  700ms - 256 cycles - Max Count: 65535

TCS34725_GAIN_1X                  = 0x00   #  No gain
TCS34725_GAIN_4X                  = 0x01   #  2x gain
TCS34725_GAIN_16X                 = 0x02   #  16x gain
TCS34725_GAIN_60X                 = 0x03   #  60x gain

# Lookup table for integration time delays.
INTEGRATION_TIME_DELAY = {
    0xFF: 0.0024,  # 2.4ms - 1 cycle    - Max Count: 1024
    0xF6: 0.024,   # 24ms  - 10 cycles  - Max Count: 10240
    0xEB: 0.050,   # 50ms  - 20 cycles  - Max Count: 20480
    0xD5: 0.101,   # 101ms - 42 cycles  - Max Count: 43008
    0xC0: 0.154,   # 154ms - 64 cycles  - Max Count: 65535
    0x00: 0.700    # 700ms - 256 cycles - Max Count: 65535
}


# Utility methods:
def calculate_color_temperature(r, g, b):
    """Converts the raw R/G/B values to color temperature in degrees Kelvin."""
    # 1. Map RGB values to their XYZ counterparts.
    # Based on 6500K fluorescent, 3000K fluorescent
    # and 60W incandescent values for a wide range.
    # Note: Y = Illuminance or lux
    X = (-0.14282 * r) + (1.54924 * g) + (-0.95641 * b)
    Y = (-0.32466 * r) + (1.57837 * g) + (-0.73191 * b)
    Z = (-0.68202 * r) + (0.77073 * g) + ( 0.56332 * b)
    # Check for divide by 0 (total darkness) and return None.
    if (X + Y + Z) == 0:
        return None
    # 2. Calculate the chromaticity co-ordinates
    xc = (X) / (X + Y + Z)
    yc = (Y) / (X + Y + Z)
    # Check for divide by 0 again and return None.
    if (0.1858 - yc) == 0:
        return None
    # 3. Use McCamy's formula to determine the CCT
    n = (xc - 0.3320) / (0.1858 - yc)
    # Calculate the final CCT
    cct = (449.0 * (n ** 3.0)) + (3525.0 *(n ** 2.0)) + (6823.3 * n) + 5520.33
    return int(cct)

def calculate_lux(r, g, b):
    """Converts the raw R/G/B values to luminosity in lux."""
    illuminance = (-0.32466 * r) + (1.57837 * g) + (-0.73191 * b)
    return int(illuminance)


class TCS34725(object):
    """TCS34725 color sensor."""

    def __init__(self, *args, **Kwargs):
        """Initialize the TCS34725 sensor."""
        # Setup I2C interface for the device.
        self._i2cDev = None

        if not isinstance(args[0], I2C):
            raise ValueError("parameter is not an I2C object")
        self._i2cDev = args[0]

        # default parameters
        gain=TCS34725_GAIN_60X
        integration_time=TCS34725_INTEGRATIONTIME_240MS

        # Make sure we're connected to the sensor.
        chip_id = self._readU8(TCS34725_ID)
        if chip_id != 0x44:
            raise RuntimeError('Failed to read TCS34725 chip ID, check your wiring.')
        # Set default integration time and gain.
        self.set_integration_time(integration_time)
        self.set_gain(gain)
        # Enable the device (by default, the device is in power down mode on bootup).
        self.enable()

    def _readU8(self, reg):
        """Read an unsigned 8-bit register."""
        v = bytearray(1)
        self._i2cDev.memRead(v, TCS34725_COMMAND_BIT | reg, 8)
        return v[0]

    def _readU16LE(self, reg):
        """Read a 16-bit little endian register."""

        v = bytearray(2)
        self._i2cDev.memRead(v, TCS34725_COMMAND_BIT | reg, 8)
        return (v[0] | (v[1] << 8))

    def _write8(self, reg, value):
        """Write a 8-bit value to a register."""
        v = bytearray([value])
        self._i2cDev.memWrite(v, TCS34725_COMMAND_BIT | reg, 8)

    def enable(self):
        """Enable the chip."""
        # Flip on the power and enable bits.
        self._write8(TCS34725_ENABLE, TCS34725_ENABLE_PON)
        time.sleep(0.01)
        self._write8(TCS34725_ENABLE, (TCS34725_ENABLE_PON | TCS34725_ENABLE_AEN))
        time.sleep(0.01)

    def disable(self):
        """Disable the chip (power down)."""
        # Flip off the power on and enable bits.
        reg = self._readU8(TCS34725_ENABLE)
        reg &= ~(TCS34725_ENABLE_PON | TCS34725_ENABLE_AEN)
        self._write8(TCS34725_ENABLE, reg)


    def set_integration_time(self, integration_time):
        """Sets the integration time for the TC34725.  Provide one of these
        constants:
         - TCS34725_INTEGRATIONTIME_2_4MS  = 2.4ms - 1 cycle    - Max Count: 1024
         - TCS34725_INTEGRATIONTIME_24MS   = 24ms  - 10 cycles  - Max Count: 10240
         - TCS34725_INTEGRATIONTIME_50MS   = 50ms  - 20 cycles  - Max Count: 20480
         - TCS34725_INTEGRATIONTIME_101MS  = 101ms - 42 cycles  - Max Count: 43008
         - TCS34725_INTEGRATIONTIME_154MS  = 154ms - 64 cycles  - Max Count: 65535
         - TCS34725_INTEGRATIONTIME_700MS  = 700ms - 256 cycles - Max Count: 65535
        """
        self._integration_time = integration_time
        self._write8(TCS34725_ATIME, integration_time)


    def get_integration_time(self):
        """Return the current integration time value.  This will be one of the
        constants specified in the set_integration_time doc string.
        """
        return self._readU8(TCS34725_ATIME)

    def set_gain(self, gain):
        """Adjusts the gain on the TCS34725 (adjusts the sensitivity to light).
        Use one of the following constants:
         - TCS34725_GAIN_1X   = No gain
         - TCS34725_GAIN_4X   = 2x gain
         - TCS34725_GAIN_16X  = 16x gain
         - TCS34725_GAIN_60X  = 60x gain
        """
        self._write8(TCS34725_CONTROL, gain)

    def get_gain(self):
        """Return the current gain value.  This will be one of the constants
        specified in the set_gain doc string.
        """
        return self._readU8(TCS34725_CONTROL)

    def wait_for_valid(self):
        status = self._readU8(TCS34725_STATUS)
        while not (status & TCS34725_STATUS_AVALID):
            pass
        print(self._readU8(TCS34725_STATUS))

    def get_raw_data(self):
        """Reads the raw red, green, blue and clear channel values. Will return
        a 4-tuple with the red, green, blue, clear color values (unsigned 16-bit
        numbers).
        """
        self.wait_for_valid()

        # Read each color register.
        r = self._readU16LE(TCS34725_RDATAL)
        g = self._readU16LE(TCS34725_GDATAL)
        b = self._readU16LE(TCS34725_BDATAL)
        c = self._readU16LE(TCS34725_CDATAL)
        # Delay for the integration time to allow for next reading immediately.
        #time.sleep(INTEGRATION_TIME_DELAY[self._integration_time])
        return (r, g, b, c)

    def set_interrupt(self, enabled):
        """Enable or disable interrupts by setting enabled to True or False."""
        enable_reg = self._readU8(TCS34725_ENABLE)
        if enabled:
            enable_reg |= TCS34725_ENABLE_AIEN
        else:
            enable_reg &= ~TCS34725_ENABLE_AIEN
        self._write8(TCS34725_ENABLE, enable_reg)
        time.sleep(1)

    def set_interrupt_limits(self, low, high):
        """Set the interrupt limits to provied unsigned 16-bit threshold values.
        """
        self._write8(0x04, low & 0xFF)
        self._write8(0x05, low >> 8)
        self._write8(0x06, high & 0xFF)
        self._write8(0x07, high >> 8)

    def setInterrupt(self, enabled):
        self.set_interrupt(enabled)

    def setInterruptLimits(self, low, high):
        self.set_interrupt_limits(low, high)

    def getRawData(self):
        return self.get_raw_data()

    def getRGB(self):
        i = 1.0
        r, g, b, c = self.get_raw_data()
        '''
        r /= c
        g /= c
        b /= c
        return (int(r * 256), int(g * 256), int(b * 256))
        '''
        if (r >= g) and (r >= b):
            i = r/255 + 1
        elif (g >= r) and (g >= b):
            i = g/255 + 1
        elif (b >= g) and (b >= r):
            i = b/255 + 1

        # print (i, r, g, b)
        if i != 0:
            r /= i
            g /= i
            b /= i

        if (r > 30):
            r -= 30
        if (g > 30):
            g -= 30
        if (b > 30):
            b -= 30

        r = r * 255 / 225
        g = g * 255 / 225
        b = b * 255 / 225

        if (r > 255):
            r = 255
        if (g > 255):
            g = 255
        if (b >= 255):
            b = 255

        return (int(r), int(g), int(b))

    def calculateLux(self, r, g, b):
        """Converts the raw R/G/B values to luminosity in lux."""
        return calculate_lux(r, g, b)

    def calculateCT(self, r, g, b):
        return calculate_color_temperature(r, g, b)