from modbus.uModbusSerial import uModbusSerial
from modbus.uModbusTCP import uModbusTCP
from boardparser import BoardConfigParser
import modbus.uModbusConst as Const

"""
"modbus_test": {
    "type": "MODBUS",
    "mode": 0,
    "port": 1,
    "baudRate": 9600,
    "priority": "none",
    "stopBits": 1,
    "dataWidth": 8,
    "tx": "none",
    "rx": "none",
    "rts": "none",
    "cts": "none",
    "ctrl_pin": "none",
    "ip_addr": "none"
    "ip_port": 502,
    "timeout": 200
}
"""

class uModbus:

    PARITY_NONE = None
    PARITY_EVEN = 0
    PARITY_ODD = 1
    
    def __init__(self):       
        self.modbus = None
        self.mode = 'serial'
        self.port = 1
        self.baudRate = 9600
        self.priority = None
        self.stopBits = 1
        self.dataWidth = 8
        self.serial_pins = None
        self.ctrl_pin = None
        self.ip_addr = None
        self.ip_port = 502
        self.timeout = 200

    def open(self, node):
        
        if type(node) is str:          
            parser = BoardConfigParser()
            item = parser.findItem(node, 'MODBUS')
            
            if item['mode'] == 'serial':
                if 'port' in item:
                    self.port = item['port']
                    
                if 'baudRate' in item:
                    self.baudRate = item['baudRate']
                    
                if 'priority' in item:
                    priority = item['priority']
                    if priority == 'none':
                        self.priority = self.PARITY_NONE
                    elif priority == 'even':
                        self.priority = self.PARITY_EVEN
                    elif priority == 'odd':
                        self.priority = self.PARITY_ODD
                    else:
                         raise ValueError('unSupported priority: {}, valid choice: ["none", "even", "odd"]'.format(item['priority']))
                    
                if 'stopBits' in item:
                    self.stopBits = item['stopBits']
                    
                if 'dataWidth' in item:
                    self.dataWidth = item['dataWidth']
                
                # tx, rx, rts, cts 
                if 'tx' in item and item['tx'] != 'none':
                    tx = item['tx']
                else:
                    tx = None

                if 'rx' in item and item['rx'] != 'none':
                    rx = item['rx']
                else:
                    rx = None

                if 'rts' in item and item['rts'] != 'none':
                    rts = item['rts']
                else:
                    rts = None
                    
                if 'cts' in item and item['cts'] != 'none':
                    cts = item['cts']
                else:
                    cts = None
                    
                if tx is not None and rx is not None:
                    self.serial_pins = [tx, rx]
                
                    if rts is not None and cts is not None:
                        self.serial_pins = [tx, rx, rts, cts]
                
                # ctrl_pin 
                if 'ctrl_pin' in item and item['ctrl_pin'] != 'none':
                    self.ctrl_pin = item['ctrl_pin']
                else:
                    self.ctrl_pin = None

                self.modbus = uModbusSerial(self.port,
                                    baudrate = self.baudRate,
                                    data_bits = self.dataWidth,
                                    stop_bits = self.stopBits,
                                    parity = self.priority,
                                    pins = self.serial_pins,
                                    ctrl_pin = self.ctrl_pin)

            elif item['mode'] == 'tcp':
                if('ip_addr' in item):
                    self.ip_addr = item['ip_addr']

                if('ip_port' in item):
                    self.ip_port = item['ip_port']

                if('timeout' in item):
                    self.timeout = item['timeout']

                self.modbus = uModbusTCP(self.ip_addr, self.ip_port, self.timeout)
            else:
                raise ValueError('unSupported mode: {}, valid choice: ["tcp", "serial"]'.format(item['mode'])) 
        else:
            raise ValueError('Node type should be str')

    def close(self):
        if self.modbus is not None:
            self.modbus.close()
            self.modbus = None
        else:
            raise OSError('modbus not opened....')

        return 0
    
    def readCoils(self, slave_addr, starting_addr, reg_quantity, data):
        ret = self.modbus.read_coils(slave_addr, starting_addr, reg_quantity)

        for i in range(reg_quantity):
            data[i] = ret[i]

        return reg_quantity

    def readDiscreteInputs(self, slave_addr, starting_addr, reg_quantity, data):
        ret = self.modbus.read_discrete_inputs(slave_addr, starting_addr, reg_quantity)

        for i in range(reg_quantity):
            data[i] = ret[i]

        return reg_quantity

    def readHoldingRegisters(self, slave_addr, starting_addr, reg_quantity, data):
        ret = self.modbus.read_holding_registers(slave_addr, starting_addr, reg_quantity, signed=True)

        for i in range(reg_quantity):
            data[i] = ret[i]

        return reg_quantity

    def readInputRegisters(self, slave_addr, starting_addr, reg_quantity, data):
        ret = self.modbus.read_input_registers(slave_addr, starting_addr, reg_quantity, signed=True)

        for i in range(reg_quantity):
            data[i] = ret[i]

        return reg_quantity

    def writeSingleCoil(self, slave_addr, coil_addr, coil_value):
        return self.modbus.write_single_coil(slave_addr, coil_addr, coil_value)

    def writeSingleRegister(self, slave_addr, reg_addr, reg_value):
        return self.modbus.write_single_register(slave_addr, reg_addr, reg_value, signed=True)

    def writeMultipleCoils(self, slave_addr, starting_addr, reg_quantity, data):
        return self.modbus.write_multiple_coils(slave_addr, starting_addr, data)

    def writeMultipleRegisters(self, slave_addr, starting_addr, reg_quantity, data):
        return self.modbus.write_multiple_registers(slave_addr, starting_addr, data, signed=True)

    def writeRaw(self, data):
        slave_addr = data[0]
        fun_code   = data[1]
        start_addr = data[2] * 256 + data[3]
        value_qty  = data[4] * 256 + data[5]

        if fun_code == Const.WRITE_SINGLE_COIL:
            return self.writeSingleCoil(slave_addr, start_addr, value_qty)

        elif fun_code == Const.WRITE_SINGLE_REGISTER:
            reg_value = data[4] * 256 + data[5]
            return self.writeSingleRegister(slave_addr, start_addr, value_qty)

        elif fun_code == Const.WRITE_MULTIPLE_COILS:
            count = data[6]
            out = bytearray(value_qty)
            index = 0
            for i in range(count):
                for k in range(8):
                    out[index] = (data[7+i] >> k) & 0x01
                    if index == value_qty - 1:
                        break
                    else:
                        index += 1

            return self.writeMultipleCoils(slave_addr, start_addr, value_qty, out)

        elif fun_code == Const.WRITE_MULTIPLE_REGISTERS:
            count = int(data[6] / 2)
            out = bytearray(count)
            for i in range(count):
                out[i] = data[7+i*2] * 256 + data[7+i*2+1]

            return self.writeMultipleRegisters(slave_addr, start_addr, value_qty, out)

        elif fun_code == Const.READ_COILS:
            ret = self.modbus.read_coils(slave_addr, start_addr, value_qty)
            out = bytearray(value_qty)
            for i in range(value_qty):
                out[i] = int(ret[i])
            return out

        elif fun_code == Const.READ_DISCRETE_INPUTS:
            ret = self.modbus.read_discrete_inputs(slave_addr, start_addr, value_qty)
            out = bytearray(value_qty)
            for i in range(value_qty):
                out[i] = int(ret[i])
            return out

        elif fun_code == Const.READ_HOLDING_REGISTERS:
            ret = self.modbus.read_holding_registers(slave_addr, start_addr, value_qty)
            out = bytearray(value_qty)
            for i in range(value_qty):
                out[i] = int(ret[i])
            return out

        elif fun_code == Const.READ_INPUT_REGISTER:
            ret = self.modbus.read_input_registers(slave_addr, start_addr, value_qty, signed=True)
            out = bytearray(value_qty)
            for i in range(value_qty):
                out[i] = int(ret[i])
            return out

        else:
            raise Exception('Unsupported function code: {}'.format(fun_code))