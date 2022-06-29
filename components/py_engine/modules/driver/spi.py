# Adapter for machine driver

from boardparser import BoardConfigParser
from machine import SPI as mach_SPI
from machine import Pin as mach_Pin
import systemAdaptor

"""
SPI APIs:

"spi_bmp280": {
      "type": "SPI",
      "port": 2,
      "mode": "master",
      "freq": 2000000
    }
"""
class SPI:
    def __init__(self):
        self.mode = "master"

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
            
            pinMap = systemAdaptor.getPinMap()
            spiName = "SPI" + str(self.port)

            if 'sck' in item and 'mosi' in item and 'miso' in item:
                self.sck = item['sck']
                self.mosi = item['mosi']
                self.miso = item['miso']
                self.spi = mach_SPI(self.port, baudrate=self.freq, sck=mach_Pin(self.sck), mosi=mach_Pin(self.mosi), miso = mach_Pin(self.miso))
            elif pinMap is not None and spiName in pinMap:
                self.port = pinMap[spiName]["PORT"]
                self.sck = pinMap[spiName]["SCLK"]
                self.mosi = pinMap[spiName]["MOSI"]
                self.miso = pinMap[spiName]["MOSO"]
                self.cs = pinMap[spiName]["CS"]
                if systemAdaptor.getChipName() == "stm32" :
                    self.spi = mach_SPI(self.port, baudrate=self.freq)
                else :
                    self.spi = mach_SPI(self.port, baudrate=self.freq, sck=mach_Pin(self.sck), mosi=mach_Pin(self.mosi), miso = mach_Pin(self.miso))

            else:
                self.spi = mach_SPI(self.port, baudrate=self.freq)

            del pinMap
            
            if 'cs' in item:
                self.cs = item['cs']
            
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
        cs_pin = self.cs_pin
        if spi is not None and cs_pin is not None:
            cs_pin.value(0)
            spi.readinto(buf)
            cs_pin.value(1)
            return len(buf)
        else:
            return -1
    
    def write(self, buf):
        spi = self.spi
        cs_pin = self.cs_pin
        if spi is not None and cs_pin is not None:
            cs_pin.value(0)
            spi.write(buf)
            cs_pin.value(1)
            return len(buf)
        else:
            return -1
    
    def readAfterWrite(self, read_buf, write_buf):
        spi = self.spi
        cs_pin = self.cs_pin
        if spi is not None and cs_pin is not None:
            # set cs_pin low
            cs_pin.value(0)
            
            # issue write then read cmd
            spi.write(write_buf)
            spi.readinto(read_buf)
        
            # set cs_pin high
            cs_pin.value(1)
            
            return len(read_buf)
        else:
            return -1
