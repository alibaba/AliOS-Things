# Adapter for machine driver

from boardparser import BoardConfigParser
from machine import UART as mach_UART
from machine import Pin as mach_Pin

import systemAdaptor

"""

"uart_test": {
    "type": "UART",
    "port": 2,
    "dataWidth": 8,
    "baudRate": 115200,
    "stopBits": 1,
    "flowControl": "disable",
    "parity": "none",
    "tx": "G4",
    "rx": "G5"
}
"""
class UART:
    def __init__(self):
        self.uart = None
        self.dataWidth = 8
        self.baudRate = 115200
        self.stopBits = 1
        self.flowControl = 0
        self.parity = None
        self.tx = None
        self.rx = None

    def open(self, node):
        if type(node) is str:          
            parser = BoardConfigParser()
            try:
                item = parser.findItem(node, 'UART')
            except Exception as e:
                print(e)
                return BoardConfigParser.NODE_NOT_EXIST

            self.port = item['port']   
            self.dataWidth = item['dataWidth']
            self.baudRate = item['baudRate']
            self.stopBits = item['stopBits']
            
            # parser flowControl
            if item['flowControl'] == 'disable':
                self.flowControl = 0
            elif item['flowControl'] == 'rts':
                self.flowControl = mach_UART.RTS
            elif item['flowControl'] == 'cts':
                self.flowControl = mach_UART.CTS
            elif item['flowControl'] == 'rtscts':
                self.flowControl = mach_UART.RTS or mach_UART.CTS
            else:
                raise ValueError('flow control:{} mode not supported'.format(item['flowControl']))
              
            # parser parity
            if item['parity'] == 'none':
                self.parity = None
            elif item['parity'] == 'odd':
                self.parity = 1
            elif item['parity'] == 'even':
                self.parity = 0
            else:
                raise ValueError('parity:{} mode not supported'.format(item['parity']))
            
            self.uart = mach_UART(self.port)
            
            pinMap = systemAdaptor.getPinMap()
            uartName = "UART" + str(self.port)
            
            if 'tx' in item and 'rx' in item:
                self.uart.init(self.baudRate,
                    bits = self.dataWidth,
                    parity = self.parity,
                    stop = self.stopBits,
                    flow = self.flowControl,
                    tx = mach_Pin(item['tx']),
                    rx = mach_Pin(item['rx']))

            elif uartName in pinMap:
                self.uart.init(self.baudRate,
                    bits = self.dataWidth,
                    parity = self.parity,
                    stop = self.stopBits,
                    flow = self.flowControl,
                    tx = mach_Pin(pinMap[uartName]["TX"]),
                    rx = mach_Pin(pinMap[uartName]["RX"]))
            else:
                self.uart.init(self.baudRate,
                    bits = self.dataWidth,
                    parity = self.parity,
                    stop = self.stopBits,
                    flow = self.flowControl)
            
            del pinMap
            return 0
        else:
            raise ValueError('Node type should be str')

    def close(self):
        self.uart.deinit()
        return 0

    def read(self, buf):
        return self.uart.readinto(buf)
    
    def write(self, buf):
        return self.uart.write(buf)
    
    def any(self):
        return self.uart.any()
    
    def on(self, cb):
        self.uart.irq(trigger=mach_UART.IRQ_RXIDLE, handler=cb, hard=True)
