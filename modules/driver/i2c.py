# Adapter for machine driver

import sys

from boardparser import BoardConfigParser
from machine import I2C as mach_I2C
from machine import Pin as mach_Pin


"""
I2C APIs:

"i2c_bmp280": {
  "type": "I2C",
  "port": 0,
  "addrWidth": 7,
  "freq": 400000,
  "mode": "master",
  "devAddr": 118
}
"""
class I2C:
    def __init__(self):
       
        self.i2c = None
        self.port = 0           
        self.addrWidth = 7
        self.freq = 400000
        self.mode = "master"
        self.devAddr = -1
        self.scl = 10
        self.sda = 12

    def open(self, node):
        if type(node) is str:          
            parser = BoardConfigParser()
            try:
                item = parser.findItem(node, 'I2C')
            except Exception as e:
                print(e)
                return BoardConfigParser.NODE_NOT_EXIST

            self.port = item['port']
            self.addrWidth = item['addrWidth']
            self.freq = item['freq']
            self.mode = item['mode']
            self.devAddr = item['devAddr']
            
            if('scl' in item):
                self.scl = item['scl']
                
            if('sda' in item):
                self.sda = item['sda']   
            
            self.i2c = mach_I2C(self.port, freq=self.freq)
            return 0
        else:
            raise ValueError('Node type should be str')

    def close(self):
        self.i2c = None
        return 0
    
    def read(self, buf):
        i2c = self.i2c
        if i2c is not None:
            i2c.readfrom_into(self.devAddr, buf)
            return len(buf)
        else:
            return -1
        
    
    def write(self, buf):
        i2c = self.i2c
        if i2c is not None:
            val = i2c.writeto(self.devAddr, buf)
            return len(buf)
        else:
            return -1
    
    def memRead(self, buf, memaddr, addrsize):
        i2c = self.i2c
        if i2c is not None:
            val = i2c.readfrom_mem_into(self.devAddr, memaddr, buf)
            return len(buf)
        else:
            return -1
    
    def memWrite(self,buf, memaddr, addrsize):
        i2c = self.i2c
        if i2c is not None:
            i2c.writeto_mem(self.devAddr, memaddr, buf)
            return len(buf)
        else:
            return -1
