import modbus
import math
import struct

def bytes2float(bytes):
    ba = bytearray()
    ba.append(bytes[1])
    ba.append(bytes[0])
    ba.append(bytes[3])
    ba.append(bytes[2])
    return struct.unpack("!f",ba)[0]

class DDS5188(object):

    def __init__(self, node, slave_addr):
        if modbus.init(node) != 0:
            raise ValueError("Error: Modbus init error.")
        self.slave_addr = slave_addr

    def __del__(self):
        modbus.deinit()

    def getKWh(self):
        rx_buf = bytearray(4)
        code = modbus.readInputRegisters(self.slave_addr, 0x0100, 2, rx_buf, 200)
        if code[0] == 0:
            data = bytes2float(rx_buf)
        else:
            data = 0.0
        return code[0], data

    def getVoltage(self):
        rx_buf = bytearray(4)
        code = modbus.readInputRegisters(self.slave_addr, 0x00, 2, rx_buf, 200)
        if code[0] == 0:
            data = bytes2float(rx_buf)
        else:
            data = 0.0
        return code[0], data

    def getCurrent(self):
        rx_buf = bytearray(4)
        code = modbus.readInputRegisters(self.slave_addr, 0x08, 2, rx_buf, 200)
        if code[0] == 0:
            data = bytes2float(rx_buf)
        else:
            data = 0.0
        return code[0], data

    def getActivePower(self):
        rx_buf = bytearray(4)
        code = modbus.readInputRegisters(self.slave_addr, 0x12, 2, rx_buf, 200)
        if code[0] == 0:
            data = bytes2float(rx_buf)
        else:
            data = 0.0
        return code[0], data
