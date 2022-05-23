import  time
from ustruct import pack
from math import pi
from driver import I2C
import math

class PCA9685:
    def __init__(self, i2cDev):
        self._i2c = None
        if not isinstance(i2cDev, I2C):
            raise ValueError("parameter is not an I2C object")
        self._i2c = i2cDev
        self.freq = 50 #50hz == 20ms
        self.ontime = 410
        self.setFreq(self.freq)

    def setFreq(self,freq = None):
        prescale = int(25000000.0 / 4096.0 / freq + 0.5)
        self._i2c.memWrite(b'\x10',0x00,8)
        self._i2c.memWrite(pack('B',prescale),0xfe,8)
        self._i2c.memWrite(b'\x00',0x00,8)
        time.sleep_us(5)
        self._i2c.memWrite(b'\xa1',0x00,8)

    def pwm(self,index, on=None, off=None):
        self._i2c.memWrite(pack('<HH',on,off),0x06 + (index*4),8)

    def reset(self, index):
        self.pwm(index, 0 , 4095)

    def setServo(self,index,pos):
      if pos > 0:
        value = 205*(pos/(pi/2))
        self.offtime = self.ontime + 306 + int(value)
      elif pos < 0:
        value = 205*((-pos)/(pi/2))
        self.offtime = self.ontime + 306 - int(value)
      else:
        self.offtime = self.ontime + 306

      self.pwm(index,self.ontime,self.offtime)
