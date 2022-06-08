from boardparser import BoardConfigParser
from modbus.uModBusSerial import uModBusSerial
from modbus.uModBusTCP import uModBusTCP

BOARD_JSON_PATH = '/data/pyamp/board.json'

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
                         raise ValueError('unSupported priority: {}, valid choice: ["none", "tcp", "serial"]'.format(item['priority'])) 
                    
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
                
                print('pins = ', self.serial_pins)

                self.modbus = uModBusSerial(self.port,
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

                self.modbus = uModBusTCP(self.ip_addr, self.ip_port, self.timeout)
            else:
                raise ValueError('unSupported mode: {}, valid choice: ["tcp", "serial"]'.format(item['mode'])) 
        else:
            raise ValueError('Node type should be str')

    def close(self):
        self.modbus.close()
        return 0
    
    def readCoils(self, slave_addr, starting_addr, reg_quantity, data):
        ret = self.modbus.read_coils(slave_addr, starting_addr, reg_quantity)
        return bytearray(ret)

    def readDiscreteInputs(self, slave_addr, starting_address, reg_quantity, data):
        ret = self.modbus.read_discrete_inputs(slave_addr, starting_addr, reg_quantity)
        return bytearray(ret)

    def readHoldingRegisters(self, slave_addr, starting_address, reg_quantity, data):
        ret = self.modbus.read_holding_registers(slave_addr, starting_addr, reg_quantity, signed=True)
        return bytearray(ret)

    def readInputRegisters(self, slave_addr, starting_address, reg_quantity, data):
        ret = self.modbus.read_input_registers(slave_addr, starting_address, reg_quantity, signed=True)
        return bytearray(ret)

    def writeSingleCoil(self, slave_addr, coil_addr, coil_value):
        return self.modbus.write_single_coil(slave_addr, coil_addr, coil_value)

    def writeSingleRegister(self, slave_addr, register_addr, register_value):
        return self.modbus.write_single_register(slave_addr, register_addr, register_value, signed=True)

    def writeMultipleCoils(self, slave_addr, starting_address, reg_quantity, data):
        return self.modbus.write_multiple_coils(slave_addr, starting_address, data)

    def writeMultipleRegisters(self, slave_addr, starting_address, reg_quantity, data):
        return self.modbus.write_multiple_registers(slave_addr, starting_address, data, signed=True)