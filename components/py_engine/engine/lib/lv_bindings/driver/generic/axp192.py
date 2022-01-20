# Python driver for the AXP192 Power Management IC.
#
# https://gist.github.com/ropg/7216ba90a9d7697114d4ba8aea7bee3c
# 
# Written in 2021 by Rop Gonggrijp.
#
# Some functionality inspired by C driver written by Mika Tuupola
# (https://github.com/tuupola/axp192) and a fork of that maintained by
# Brian Starkey (https://github.com/usedbytes/axp192)
#
# License: MIT


from machine import I2C, Pin

SDA                        = 21
SCL                        = 22
I2C_ADDRESS	               = 0x34

# Power control registers 
POWER_STATUS               = 0x00
CHARGE_STATUS              = 0x01
OTG_VBUS_STATUS            = 0x04
DATA_BUFFER0               = 0x06
DATA_BUFFER1               = 0x07
DATA_BUFFER2               = 0x08
DATA_BUFFER3               = 0x09
DATA_BUFFER4               = 0x0a
DATA_BUFFER5               = 0x0b
# Output control: 2 EXTEN, 0 DCDC2 
EXTEN_DCDC2_CONTROL        = 0x10
# Power output control: 6 EXTEN, 4 DCDC2, 3 LDO3, 2 LDO2, 1 DCDC3, 0 DCDC1 
DCDC13_LDO23_CONTROL       = 0x12
DCDC2_VOLTAGE              = 0x23
DCDC2_SLOPE                = 0x25
DCDC1_VOLTAGE              = 0x26
DCDC3_VOLTAGE              = 0x27
# Output voltage control: 7-4 LDO2, 3-0 LDO3 
LDO23_VOLTAGE              = 0x28
VBUS_IPSOUT_CHANNEL        = 0x30
SHUTDOWN_VOLTAGE           = 0x31
SHUTDOWN_BATTERY_CHGLED_CONTROL   = 0x32
CHARGE_CONTROL_1           = 0x33
CHARGE_CONTROL_2           = 0x34
BATTERY_CHARGE_CONTROL     = 0x35
PEK                        = 0x36
DCDC_FREQUENCY             = 0x37
BATTERY_CHARGE_LOW_TEMP    = 0x38
BATTERY_CHARGE_HIGH_TEMP   = 0x39
APS_LOW_POWER1             = 0x3A
APS_LOW_POWER2             = 0x3B
BATTERY_DISCHARGE_LOW_TEMP    = 0x3c
BATTERY_DISCHARGE_HIGH_TEMP   = 0x3d
DCDC_MODE                  = 0x80
ADC_ENABLE_1               = 0x82
ADC_ENABLE_2               = 0x83
ADC_RATE_TS_PIN            = 0x84
GPIO30_INPUT_RANGE         = 0x85
GPIO0_ADC_IRQ_RISING       = 0x86
GPIO0_ADC_IRQ_FALLING      = 0x87
TIMER_CONTROL              = 0x8a
VBUS_MONITOR               = 0x8b
TEMP_SHUTDOWN_CONTROL      = 0x8f

# GPIO control registers 
GPIO0_CONTROL              = 0x90
GPIO0_LDOIO0_VOLTAGE       = 0x91
GPIO1_CONTROL              = 0x92
GPIO2_CONTROL              = 0x93
GPIO20_SIGNAL_STATUS       = 0x94
GPIO40_FUNCTION_CONTROL    = 0x95
GPIO40_SIGNAL_STATUS       = 0x96
GPIO20_PULLDOWN_CONTROL    = 0x97
PWM1_FREQUENCY             = 0x98
PWM1_DUTY_CYCLE_1          = 0x99
PWM1_DUTY_CYCLE_2          = 0x9a
PWM2_FREQUENCY             = 0x9b
PWM2_DUTY_CYCLE_1          = 0x9c
PWM2_DUTY_CYCLE_2          = 0x9d
N_RSTO_GPIO5_CONTROL       = 0x9e

# Interrupt control registers 
ENABLE_CONTROL_1           = 0x40
ENABLE_CONTROL_2           = 0x41
ENABLE_CONTROL_3           = 0x42
ENABLE_CONTROL_4           = 0x43
ENABLE_CONTROL_5           = 0x4a
IRQ_STATUS_1               = 0x44
IRQ_STATUS_2               = 0x45
IRQ_STATUS_3               = 0x46
IRQ_STATUS_4               = 0x47
IRQ_STATUS_5               = 0x4d

# ADC data registers 
ACIN_VOLTAGE               = 0x56
ACIN_CURRENT               = 0x58
VBUS_VOLTAGE               = 0x5a
VBUS_CURRENT               = 0x5c
TEMP                       = 0x5e
TS_INPUT                   = 0x62
GPIO0_VOLTAGE              = 0x64
GPIO1_VOLTAGE              = 0x66
GPIO2_VOLTAGE              = 0x68
GPIO3_VOLTAGE              = 0x6a
BATTERY_POWER              = 0x70
BATTERY_VOLTAGE            = 0x78
CHARGE_CURRENT             = 0x7a
DISCHARGE_CURRENT          = 0x7c
APS_VOLTAGE                = 0x7e
CHARGE_COULOMB             = 0xb0
DISCHARGE_COULOMB          = 0xb4
COULOMB_COUNTER_CONTROL    = 0xb8

BIT_DCDC1_ENABLE           = 0b00000001
BIT_DCDC2_ENABLE           = 0b00010000
BIT_DCDC3_ENABLE           = 0b00000010
BIT_LDO2_ENABLE            = 0b00000100
BIT_LDO3_ENABLE            = 0b00001000
BIT_EXTEN_ENABLE           = 0b01000000

# These are not real registers, see read and write functions
LDO2_VOLTAGE               = 0x0101
LDO3_VOLTAGE               = 0x0102

class AXP192():

    def __init__(self, i2c_dev, sda, scl, freq=400000, i2c_addr=I2C_ADDRESS):
        self.i2c = I2C(i2c_dev, sda=Pin(sda), scl=Pin(scl), freq=freq)
        self.i2c_addr = i2c_addr

    def read_byte(self, reg_addr):
        tmp = self.i2c.readfrom_mem(self.i2c_addr, reg_addr, 1)[0]
        # print("read_byte: 0x{:x} from 0x{:x}".format(tmp, reg_addr))
        return tmp

    def write_byte(self, reg_addr, data):
        # print("write_byte: 0x{:x} to 0x{:x}".format(data, reg_addr))
        self.i2c.writeto_mem(self.i2c_addr, reg_addr, bytes([data]))

    def twiddle(self, reg_addr, affects, value):
        self.write_byte(reg_addr, (self.read_byte(reg_addr) & (affects ^ 0xff)) | (value & affects))

    def write(self, reg_addr, data):
        # print("write: {:x} with {}".format(reg_addr, data))
        if reg_addr == DCDC1_VOLTAGE:
            if data == 0:
                self.twiddle(DCDC13_LDO23_CONTROL, BIT_DCDC1_ENABLE, 0)
                return
            if data < 0.7 or data > 3.5:
                raise ValueError("Voltage out of range")
            self.twiddle(reg_addr, 0b01111111, int((data - 0.7) / 0.025))
            self.twiddle(DCDC13_LDO23_CONTROL, BIT_DCDC1_ENABLE, BIT_DCDC1_ENABLE)
            return
        elif reg_addr == DCDC2_VOLTAGE:
            if data == 0:
                self.twiddle(DCDC13_LDO23_CONTROL, BIT_DCDC2_ENABLE, 0)
                return
            if data < 0.7 or data > 2.275:
                raise ValueError("Voltage out of range")
            self.twiddle(reg_addr, 0b00111111, int((data - 0.7) / 0.025))
            self.twiddle(DCDC13_LDO23_CONTROL, BIT_DCDC2_ENABLE, BIT_DCDC2_ENABLE)
            return
        if reg_addr == DCDC3_VOLTAGE:
            if data == 0:
                self.twiddle(DCDC13_LDO23_CONTROL, BIT_DCDC3_ENABLE, 0)
                return
            if data < 0.7 or data > 3.5:
                raise ValueError("Voltage out of range")
            self.twiddle(reg_addr, 0b01111111, int((data - 0.7) / 0.025))
            self.twiddle(DCDC13_LDO23_CONTROL, BIT_DCDC3_ENABLE, BIT_DCDC3_ENABLE)
            return
        elif reg_addr == LDO2_VOLTAGE:
            if data == 0:
                self.twiddle(DCDC13_LDO23_CONTROL, BIT_LDO2_ENABLE, 0)
                return
            if data < 1.8 or data > 3.3:
                raise ValueError("Voltage out of range")
            val = int((data - 1.8) / 0.1)
            self.twiddle(LDO23_VOLTAGE, 0xf0, val << 4)
            self.twiddle(DCDC13_LDO23_CONTROL, BIT_LDO2_ENABLE, BIT_LDO2_ENABLE)
            return
        elif reg_addr == LDO3_VOLTAGE:
            if data == 0:
                self.twiddle(DCDC13_LDO23_CONTROL, BIT_LDO3_ENABLE, 0)
                return
            if data < 1.8 or data > 3.3:
                raise ValueError("Voltage out of range")
            val = int((data - 1.8) / 0.1)
            self.twiddle(LDO23_VOLTAGE, 0x0f, val)
            self.twiddle(DCDC13_LDO23_CONTROL, BIT_LDO3_ENABLE, BIT_LDO3_ENABLE)
            return

        if type(data) != "bytes":
            self.write_byte(reg_addr, data)
        else:
            self.i2c.writeto_mem(self.i2c_addr, reg_addr, data)

    def read(self, reg_addr, length=1):

        if length != 1:
            return self.i2c.readfrom_mem(self.i2c_addr, reg_addr, length)

        sensitivity = 1.0
        offset = 0.0

        if reg_addr == ACIN_VOLTAGE or reg_addr == VBUS_VOLTAGE:
            # 1.7mV per LSB 
            sensitivity = 1.7 / 1000
        elif reg_addr == ACIN_CURRENT:
            # 0.625mA per LSB 
            sensitivity = 0.625 / 1000
        elif reg_addr == VBUS_CURRENT:
            # 0.375mA per LSB 
            sensitivity = 0.375 / 1000
        elif reg_addr == TEMP:
            # 0.1C per LSB, 0x00 = -144.7C 
            sensitivity = 0.1
            offset = -144.7
        elif reg_addr == TS_INPUT:
            # 0.8mV per LSB 
            sensitivity = 0.8 / 1000
        elif reg_addr == BATTERY_POWER:
            # 1.1mV * 0.5mA per LSB 
            return int.from_bytes(self.read(BATTERY_POWER, 3), "big") * (1.1 * 0.5 / 1000)
        elif reg_addr == BATTERY_VOLTAGE:
            # 1.1mV per LSB 
            sensitivity = 1.1 / 1000
        elif reg_addr == CHARGE_CURRENT or reg_addr == DISCHARGE_CURRENT:
            # 0.5mV per LSB 
            sensitivity = 0.5 / 1000
        elif reg_addr == APS_VOLTAGE:
            # 1.4mV per LSB 
            sensitivity = 1.4 / 1000
        elif reg_addr == CHARGE_COULOMB or reg_addr == DISCHARGE_COULOMB:
            return int.from_bytes(self.read(reg_addr, 4), "big")
        elif reg_addr == DCDC1_VOLTAGE:
            if self.read_byte(DCDC13_LDO23_CONTROL) & BIT_DCDC1_ENABLE == 0:
                return 0
            return (self.read_byte(reg_addr) & 0b01111111) * 0.25 + 0.7
        elif reg_addr == DCDC2_VOLTAGE:
            if self.read_byte(DCDC13_LDO23_CONTROL) & BIT_DCDC2_ENABLE == 0:
                return 0
            return (self.read_byte(reg_addr) & 0b00111111) * 0.25 + 0.7
        elif reg_addr == DCDC3_VOLTAGE:
            if self.read_byte(DCDC13_LDO23_CONTROL) & BIT_DCDC3_ENABLE == 0:
                return 0
            return (self.read_byte(reg_addr) & 0b01111111) * 0.25 + 0.7
        elif reg_addr == LDO2_VOLTAGE:
            if self.read_byte(DCDC13_LDO23_CONTROL) & BIT_LDO2_ENABLE == 0:
                return 0
            return ((self.read_byte(LDO23_VOLTAGE) & 0xf0) >> 4) * 0.1 + 1.8
        elif reg_addr == LDO3_VOLTAGE:
            if self.read_byte(DCDC13_LDO23_CONTROL) & BIT_LDO3_ENABLE == 0:
                return 0
            return ((self.read_byte(LDO23_VOLTAGE) & 0x0f)) * 0.1 + 1.8
        # any values not listed will just read a single byte
        else:
            return self.read_byte(reg_addr)
        # handle cases above that did not end in return
        tmp = self.i2c.readfrom_mem(self.i2c_addr, reg_addr, 2)
        return (((tmp[0] << 4) + tmp[1]) * sensitivity) + offset

    def coulomb_counter(self):
        # CmAh = 65536 * 0.5mA *（coin - cout) / 3600 / ADC sample rate
        return 32768 * (self.read(CHARGE_COULOMB) - self.read(DISCHARGE_COULOMB)) / 3600 / 25

    def coulomb_counter_enable(self):
        self.write(COULOMB_COUNTER_CONTROL, 0b10000000)

    def coulomb_counter_disable(self):
        self.write(COULOMB_COUNTER_CONTROL, 0b00000000)

    def coulomb_counter_suspend(self):
        self.write(COULOMB_COUNTER_CONTROL, 0b11000000)

    def coulomb_counter_clear(self):
        self.write(COULOMB_COUNTER_CONTROL, 0b10100000)
