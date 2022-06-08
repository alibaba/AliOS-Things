# Adapter for machine driver

import sys

from boardparser import BoardConfigParser
from machine import SPI as mach_SPI
from machine import Pin as mach_Pin

"""
SPI APIs:

"spi_bmp280": {
      "type": "SPI",
      "port": 3,
      "mode": "master",
      "freq": 2000000
    }
"""
class SPI:
    def __init__(self):
        self.port = 2
        self.mode = "master"
        self.freq = 2000000
        self.spi = None
        self.cs_pin = None
        self.sck = 'B13'
        self.mosi = 'C2'
        self.miso = 'B15'
        self.cs = 'B12'

    def open(self, node):
        if type(node) is str:          
            parser = BoardConfigParser()
            try:
                item = parser.findItem(node, 'SPI')
            except Exception as e:
                print(e)
                return BoardConfigParser.NODE_NOT_EXIST

            self.port = item['port']   
            self.freq = item['freq']
            self.mode = item['mode']
            
            if 'sck' in item:
                self.sck = item['sck']
                
            if 'mosi' in item:
                self.mosi = item['mosi']
            
            if 'miso' in item:
                self.miso = item['miso']
                
            if 'cs' in item:
                self.cs = item['cs']
            
            self.spi = mach_SPI(self.port, baudrate=self.freq)
            self.cs_pin = mach_Pin(self.cs, mach_Pin.OUT)

            return 0
        else:
            raise ValueError('Node type should be str')

    def close(self):
        spi = self.spi
        if spi is not None:
            spi.deinit()
            self.spi = None
        return 0
         
    def read(self, buf):
        spi = self.spi
        if spi is not None:
            spi.readinto(buf)
            return len(buf)
        else:
            return -1
    
    def write(self, buf):
        spi = self.spi
        if spi is not None:
            spi.write(buf)
            return len(buf)
        else:
            return -1
    
    def readAfterWrite(self, read_buf, write_buf):
        spi = self.spi
        cs_pin = self.cs_pin
        if spi is not None and cs_pin is not None:
            # set cs_pin low
            cs_pin.low()
            
            # issue write then read cmd
            spi.write(write_buf)
            spi.readinto(read_buf)
        
            # set cs_pin high
            cs_pin.high()
            
            return len(read_buf)
        else:
            return -1
