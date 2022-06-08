from machine import I2C
from machine import Pin
from micropython import const
import struct
#import unit

#PORTA = (unit.PORTA)

PAHUB0 = (0, None)
PAHUB1 = (1, None)
PAHUB2 = (2, None)
PAHUB3 = (3, None)
PAHUB4 = (4, None)
PAHUB5 = (5, None)

M_BUS = (21, 22)

UINT8LE = const((0 << 6) | (1 << 4) | 1)
UINT16LE = const((0 << 6) | (1 << 4) | 2)
UINT32LE = const((0 << 6) | (1 << 4) | 4)
INT8LE = const((0 << 6) | (0 << 4) | 1)
INT16LE = const((0 << 6) | (0 << 4) | 2)
INT32LE = const((0 << 6) | (0 << 4) | 4)
UINT8BE = const((1 << 6) | (1 << 4) | 1)
UINT16BE = const((1 << 6) | (1 << 4) | 2)
UINT32BE = const((1 << 6) | (1 << 4) | 4)
INT8BE = const((1 << 6) | (0 << 4) | 1)
INT16BE = const((1 << 6) | (0 << 4) | 2)
INT32BE = const((1 << 6) | (0 << 4) | 4)

def get(port, pos=0, freq=400000, device_in=False):
    if port[1] is None:
        return Pahub_I2C(port[0])
    if device_in or port == (21, 22):
        return I2C(1, sda=Pin(port[0]), scl=Pin(port[1]), freq=freq)
    else:
        return I2C(0, sda=Pin(port[0]), scl=Pin(port[1]), freq=freq)

class easyI2C():
    def __init__(self, port, addr, freq=400000):
        self.i2c = get(port, pos=0, freq=freq)
        self.addr = addr

    def write_u8(self, reg, data):
        buf = bytearray(1)
        buf[0] = data
        self.i2c.writeto_mem(self.addr, reg, buf)

    def write_u16(self, reg, data, byteorder='big'):
        buf = bytearray(2)
        encode = '<h' if byteorder == 'little' else '>h'
        struct.pack_into(encode, buf, 0, data)
        self.i2c.writeto_mem(self.addr, reg, buf)

    def write_u32(self, reg, data, byteorder='big'):
        buf = bytearray(4)
        encode = '<i' if byteorder == 'little' else '>i'
        struct.pack_into(encode, buf, 0, data)
        self.i2c.writeto_mem(self.addr, reg, buf)

    def read_u8(self, reg):
        return self.i2c.readfrom_mem(self.addr, reg, 1)[0]

    def read_u16(self, reg, byteorder='big'):
        buf = bytearray(2)
        self.i2c.readfrom_mem_into(self.addr, reg, buf)
        encode = '<h' if byteorder == 'little' else '>h'
        return struct.unpack(encode, buf)[0]

    def read_u32(self, reg, byteorder='big'):
        buf = bytearray(4)
        self.i2c.readfrom_mem_into(self.addr, reg, buf)
        encode = '<i' if byteorder == 'little' else '>i'
        return struct.unpack(encode, buf)[0]

    def read(self, num):
        return self.i2c.readfrom(self.addr, num)

    def read_reg(self, reg, num):
        return self.i2c.readfrom_mem(self.addr, reg, num)

    @staticmethod
    def _get_format_str(format_type):
        format_str = '>' if (format_type & (1 << 6)) else '<'
        format_str += {1: 'b', 2: 'h', 4: 'i'}.get(format_type & 0x0f)
        format_str = format_str.upper() if (format_type & (1 << 4)) else format_str
        return format_str
    
    def write_mem_data(self, reg, data, format_type):
        format_str = self._get_format_str(format_type)
        buf = bytearray(struct.pack(format_str, data))
        self.i2c.writeto_mem(self.addr, reg, buf)

    def write_data(self, data, format_type):
        format_str = self._get_format_str(format_type)
        buf = bytearray(struct.pack(format_str, data))
        self.i2c.writeto(self.addr, buf)

    def write_list(self, data):
        buf = bytearray(data)
        self.i2c.writeto(self.addr, buf)

    def write_mem_list(self, reg, data, num):
        buf = bytearray(data)
        self.i2c.writeto_mem(self.addr, reg, buf)

    def read_data(self, num, format_type):
        format_str = self._get_format_str(format_type)
        format_str = format_str[0] + format_str[1] * num
        buf = bytearray((format_type & 0x0f) * num)
        self.i2c.readfrom_into(self.addr, buf)
        return struct.unpack(format_str, buf)

    def read_mem_data(self, reg, num, format_type):
        format_str = self._get_format_str(format_type)
        format_str = format_str[0] + format_str[1] * num
        buf = bytearray((format_type & 0x0f) * num)
        self.i2c.readfrom_mem_into(self.addr, reg, buf)
        return struct.unpack(format_str, buf)

    def scan(self):
        return self.i2c.scan()

    def available(self):
        return self.i2c.is_ready(self.addr)

class Pahub_I2C:
    def __init__(self, pos, port=(32, 33), freq=100000):  # PORTA (32, 33)
        from units import _pahub
        self.pahub = _pahub.Pahub(port)
        self.i2c = get(port, freq=freq)
        self.pos = pos

    def readfrom(self, addr, num):
        self.pahub.select_only_on(self.pos)
        data = self.i2c.readfrom(addr, num)
        return data

    def readfrom_into(self, addr, buf):
        buf_in = bytearray(len(buf))
        self.pahub.select_only_on(self.pos)
        self.i2c.readfrom_into(addr, buf_in)
        for i in range(len(buf)):
            buf[i] = buf_in[i]

    def readfrom_mem_into(self, addr, reg, buf):
        buf_in = bytearray(len(buf))
        self.pahub.select_only_on(self.pos)
        self.i2c.readfrom_mem_into(addr, reg, buf_in)
        for i in range(len(buf)):
            buf[i] = buf_in[i]

    def readfrom_mem(self, addr, reg, num):
        self.pahub.select_only_on(self.pos)
        data = self.i2c.readfrom_mem(addr, reg, num)
        return data

    def writeto_mem(self, addr, reg, data):
        self.pahub.select_only_on(self.pos)
        self.i2c.writeto_mem(addr, reg, data)

    def writeto(self, addr, data):
        self.pahub.select_only_on(self.pos)
        self.i2c.writeto(addr, data)

    def is_ready(self, addr):
        self.pahub.select_only_on(self.pos)
        data = self.i2c.is_ready(addr)
        return data

    def scan(self):
        self.pahub.select_only_on(self.pos)
        data = self.i2c.scan()
        return data

    def available(self):
        return self.i2c.is_ready(self.addr)

    def deinit(self):
        pass

class Unit(Exception):
    pass

class UnitI2C:
    def __init__(self, port, freq, addr):
        self.i2c = easyI2C(port, addr, freq)

    def _check_device(self):
        if self.i2c.available() or self.i2c.available():
            pass
        else:
            raise Unit("{} unit not found".format(self.__qualname__.upper()))

    def deinit(self):
        pass

