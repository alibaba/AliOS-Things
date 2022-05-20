"""
Copyright (C) 2015-2021 Alibaba Group Holding Limited

    HaaS Python driver for ws2812

    Author: HaaS
    Date:   2022/05/10
"""

from driver import GPIO
from machine import Pin
from neopixel import NeoPixel
import utime

br = 1.0

class WS2812(object):
    def __init__(self,gpioObj,led_pixel=24):
        self.port_num = None
        if not isinstance(gpioObj, GPIO):
            raise ValueError("parameter is not an GPIO object")
        self.port_num = gpioObj.port()
        self.led_pixel = led_pixel

        self.pin = Pin(self.port_num, Pin.OUT)
        self.np = NeoPixel(self.pin, self.led_pixel)

    def set(self,r,g,b,index=0):
        if index >= self.led_pixel:
            raise ValueError("index is too large")
        self.np[index] = (r,g,b)
        self.np.write()

    def dot(self,r,g,b,t):
        r=int(br*r)
        g=int(br*g)
        b=int(br*b)
        for i in range(0,self.led_pixel):
            self.clear_array()
            self.np[i] = (r,g,b)
            self.np.write()
            utime.sleep_ms(t)

    def rainbow(self,t):
        r=100
        g=0
        b=0

        for c in range(0,50):
            g=g+2
            for i in range(0,self.led_pixel):
                self.np[i] = (r,g,b)
            self.np.write()
            utime.sleep_us(t)

        for c in range(0,50):
            r=r-2
            for i in range(0,self.led_pixel):
                self.np[i] = (r,g,b)
            self.np.write()
            utime.sleep_us(t)

        for c in range(0,50):
            b=b+2
            for i in range(0,self.led_pixel):
                self.np[i] = (r,g,b)
            self.np.write()
            utime.sleep_us(t)

        for c in range(0,50):
            g=g-2
            for i in range(0,self.led_pixel):
                self.np[i] = (r,g,b)
            self.np.write()
            utime.sleep_us(t)

        for c in range(0,50):
            r=r+2
            for i in range(0,self.led_pixel):
                self.np[i] = (r,g,b)
            self.np.write()
            utime.sleep_us(t)

        for c in range(0,50):
            b=b-2
            for i in range(0,self.led_pixel):
                self.np[i] = (r,g,b)
            self.np.write()
            utime.sleep_us(t)

    def wipe(self,r,g,b,t):
        r=int(br*r)
        g=int(br*g)
        b=int(br*b)
        self.clear_array()
        for i in range(0,self.led_pixel):
            self.np[i] = (r,g,b)
            self.np.write()
            utime.sleep_ms(t)

    def clear_array(self):
        for i in range(0,self.led_pixel):
            self.np[i] = (0,0,0)

    def clear(self,index=None):
        if index == None:
            for i in range(0,self.led_pixel):
                self.np[i] = (0,0,0)

        if index >= self.led_pixel:
            raise ValueError("index is too large")
        else:
            self.np[index] = (0,0,0)
        self.np.write()

    # h-(0,23) m-(0,59)
    def timeShowCircle(self,pr,pg,pb,h,m):
        r=int(br*pr)
        g=int(br*pg)
        b=int(br*pb)
        ha = 0
        ma = 0
        if m == 0:
            ha = (h%12)*2
            ma = 0
        else:
            ha = (h%12)*2 + 1
            ma = int(m/2.5)
        self.clear_array()
        for i in range(0,self.led_pixel-2):
            if i != ha and i != ma:
                self.np[i] = (r,g,b)
            if (i+1) != ha and (i+1) != ma:
                self.np[i+1] = (r,g,b)
            if (i+2) != ha and (i+2) != ma:
                self.np[i+2] = (r,g,b)
            self.np[ha] = (200,0,0)
            self.np[ma] = (0,200,0)
            self.np.write()
            if i != ha and i != ma:
                self.np[i] = (0,0,0)
            utime.sleep_ms(41)

    # h-(0,23) m-(0,59)
    def timeShow(self,pr,pg,pb,h,m,s):
        r=int(br*pr)
        g=int(br*pg)
        b=int(br*pb)
        ha = 0
        ma = 0
        if m == 0 or m < 30:
            ha = (h%12)*2
            if m == 0:
                ma = 0
            else:
                ma = int(m/2.5)
        else:
            ha = (h%12)*2 + 1
            ma = int(m/2.5)+1

        self.clear_array()
        i = int(s/2.5)
        if i != ha and i != ma:
            self.np[i] = (r,g,b)
        if (i+1) != ha and (i+1) != ma and i<self.led_pixel-1:
            self.np[i+1] = (r,g,b)

        if (i+2) != ha and (i+2) != ma and i<self.led_pixel-2:
            self.np[i+2] = (r,g,b)

        if i == 22:
            self.np[0] = (r,g,b)
        if i == 23:
            self.np[0] = (r,g,b)
            self.np[1] = (r,g,b)

        if ha != ma:
            self.np[ha] = (200,0,0)
            if ma==24:
                self.np[0] = (0,0,200)
            else:
                self.np[ma] = (0,0,200)
        else:
            self.np[ma] = (200,0,200)
        self.np.write()
        if i != ha and i != ma:
            self.np[i] = (0,0,0)
