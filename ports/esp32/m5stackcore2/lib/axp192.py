from micropython import const
import ustruct
import i2c_bus
import utime as time
# import deviceCfg

_BTN_IRQ = const(0x46)

def map_value(value, input_min, input_max, aims_min, aims_max):
    value = min(max(input_min, value), input_max)
    value_deal = (value - input_min) * (aims_max - aims_min) / (input_max - input_min) + aims_min
    return value_deal

class Axp192:

    CURRENT_100MA = const(0b0000)
    CURRENT_190MA = const(0b0001)
    CURRENT_280MA = const(0b0010)
    CURRENT_360MA = const(0b0011)
    CURRENT_450MA = const(0b0100)
    CURRENT_550MA = const(0b0101)
    CURRENT_630MA = const(0b0110)
    CURRENT_700MA = const(0b0111)

    def __init__(self):
        self.addr = 0x34
        self.i2c = i2c_bus.get(i2c_bus.M_BUS, device_in=True)

    def powerAll(self):
        regchar = self._regChar
        # axp: vbus limit off
        regchar(0x30, (self._read8bit(0x30) & 0x04) | 0x02)
        # AXP192 GPIO1:OD OUTPUT
        regchar(0x92, self._read8bit(0x92) & 0xf8)
        # AXP192 GPIO2:OD OUTPUT
        regchar(0x93, self._read8bit(0x93) & 0xf8)
        # AXP192 RTC CHG
        regchar(0x35, (self._read8bit(0x35) & 0x1c) | 0xa3)
        # ESP voltage:3.35V
        self.setESPVoltage(3350)
        # LCD backlight voltage:3.3V
        # self.setLCDBacklightVoltage(3300)
        # Periph power voltage preset (LCD_logic, SD card)
        self.setLDOVoltage(2, 3300)
        # Vibrator power voltage preset
        self.setLDOVoltage(3, 2000)
        # Eanble LCD SD power
        self.setLDOEnable(2, True)
        # Set charge current:100ma
        # self.setChargeCurrent(CURRENT_100MA)
        # self.setChargeCurrent(deviceCfg.get_bat_charge_current())

        # AXP192 GPIO4
        regchar(0x95, (self._read8bit(0x95) & 0x72) | 0x84)

        regchar(0x36, 0x4C)
        regchar(0x82, 0xff)

        self.setLCDReset(0)
        time.sleep(0.1)
        self.setLCDReset(1)
        time.sleep(0.1)

        #if deviceCfg.get_comx_status():
        #    self.setBusPowerMode(1)  # disable M-Bus 5V output if use COM.X module.
        #else:
        self.setBusPowerMode(0)  # enable M-Bus 5V output as default.

    def powerOff(self):
        self._regChar(0x32, self._regChar(0x32) | 0x80)

# AXP192 Status getting function

    def getTempInAXP192(self):
        return (self._read12Bit(0x5e)) * 0.1 - 144.7

    def getChargeState(self):
        return True if self._regChar(0x01) & 0x40 else False

    def getBatVoltage(self):
        return (self._read12Bit(0x78)) * 1.1 / 1000

    def getBatCurrent(self):
        currentIn = self._read13Bit(0x7A)
        currentOut = self._read13Bit(0x7C)
        return (currentIn - currentOut) * 0.5

    def getVinVoltage(self):
        return (self._read12Bit(0x56)) * 1.7 / 1000

    def getVinCurrent(self):
        return (self._read12Bit(0x58)) * 0.625

    def getVBusVoltage(self):
        return (self._read12Bit(0x5A)) * 1.7 / 1000

    def getVBusCurrent(self):
        return (self._read12Bit(0x5C)) * 0.375


# AXP192 Status setting function

    def setChargeState(self, state):
        pass

    def setChargeCurrent(self, current):
        buf = self._regChar(0x33)
        buf = (buf & 0xf0) | (current & 0x0f)
        self._regChar(0x33, buf)

    def setBusPowerMode(self, mode):
        """
        0: M-BUS 5V output mode.
        1: M-BUS 5V input mode.
        """
        if mode == 0:
            self._regChar(0x91, (self._read8bit(0x91) & 0x0F) | 0xF0)
            self._regChar(0x90, (self._read8bit(0x90) & 0xF8) | 0x02)
            self._regChar(0x12, (self._read8bit(0x12) | 0x40))
        else:
            self._regChar(0x12, self._read8bit(0x12) & 0xBF)
            self._regChar(0x90, (self._read8bit(0x90) & 0xF8) | 0x01)

    def setLDOVoltage(self, number, voltage):
        # print("number: " + str(number) + "  voltage: " + str(voltage))
        if voltage > 3300:
            vol = 15
        else:
            vol = (int)((voltage / 100) - 18)
        regchar = self._regChar
        if number == 2:
            regchar(0x28, ((self._read8bit(0x28) & 0x0F) | (vol << 4)))
        if number == 3:
            regchar(0x28, ((self._read8bit(0x28) & 0xF0) | vol))

    def setLDOEnable(self, number, state):
        mask = 0x01
        if number < 2 or number > 3:
            return
        mask = mask << number
        if(state):
            self._regChar(0x12, self._read8bit(0x12) | mask)
        else:
            self._regChar(0x12, self._read8bit(0x12) & (~ mask))

    def setDCVoltage(self, number, voltage):
        addr = [0, 0x26, 0x25, 0x27]
        regchar = self._regChar
        if number < 1 and number > 3:
            return
        vol = (int)(0 if voltage < 700 else (voltage - 700) / 25)
        regchar(addr[number], (self._read8bit(addr[number]) & 0x80) | (vol & 0x7F))

    def disableAllIRQ(self):
        for i in [0x40, 0x41, 0x42, 0x43, 0x4a]:
            self._regChar(i, 0x00)

    def clearAllIRQ(self):
        for i in [0x44, 0x45, 0x46, 0x47, 0x4d]:
            self._regChar(i, 0xff)

    def enableBtnIRQ(self):
        self._regChar(0x42, 0x02)

    #  ESP32 Voltage
    def setESPVoltage(self, voltage):
        if voltage >= 3000 and voltage <= 3400:
            self.setDCVoltage(1, voltage)

    #  LCD backlight Voltage
    def setLCDBacklightVoltage(self, voltage):
        voltage = voltage * 1000
        if voltage >= 2400 and voltage <= 3300:
            self.setDCVoltage(3, voltage)

    def setLCDEnable(self, state):
        self.setLDOEnable(2, state)

    #  LCD Brightness
    def setLCDBrightness(self, brightness):
        vol = map_value(brightness, 0, 100, 2400, 3300)
        self.setDCVoltage(3, vol)

    #  LCD Reset
    def setLCDReset(self, state):
        mask = 0x02
        if state:
            self._regChar(0x96, self._read8bit(0x96) | mask)
        else:
            self._regChar(0x96, self._read8bit(0x96) & (~ mask))

    #  Set Power LED
    def setPowerLED(self, state):
        if state:
            self._regChar(0x94, self._read8bit(0x94) & 0xFD)
        else:
            self._regChar(0x94, self._read8bit(0x94) | 0x02)

    def setSpkEnable(self, state):
        gpio_bit = 0x04
        data = self._read8bit(0x94)
        if state:
            data = data | gpio_bit
        else:
            data = data & (~gpio_bit)
        self._regChar(0x94, data)

    # It seem not useful for Vibration motor.
    # LDO3: 1.8v ~ 3.3v
    def setVibrationIntensity(self, value):
        vol = map_value(value, 0, 100, 1800, 3300)
        self.setLDOVoltage(3, vol)

    def setVibrationEnable(self, state):
        self.setLDOEnable(3, state)

# I2C read and write function
    def _regChar(self, reg, value=None, buf=bytearray(1)):
        if value is None:
            self.i2c.readfrom_mem_into(self.addr, reg, buf)
            return buf[0]

        ustruct.pack_into('<b', buf, 0, value)
        return self.i2c.writeto_mem(self.addr, reg, buf)

    def _read8bit(self, reg):
        buf = bytearray(1)
        self.i2c.readfrom_mem_into(self.addr, reg, buf)
        return buf[0]

    def _read12Bit(self, reg):
        buf = bytearray(2)
        self.i2c.readfrom_mem_into(self.addr, reg, buf)
        return (buf[0] << 4) | buf[1]

    def _read13Bit(self, reg):
        buf = bytearray(2)
        self.i2c.readfrom_mem_into(self.addr, reg, buf)
        return (buf[0] << 5) | buf[1]

    def _read16Bit(self, reg):
        buf = bytearray(2)
        self.i2c.readfrom_mem_into(self.addr, reg, buf)
        return (buf[0] << 8) | buf[1]

    def deinit(self):
        pass

