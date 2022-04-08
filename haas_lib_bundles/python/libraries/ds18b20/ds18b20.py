from driver import GPIO
from onewire import OneWire
from machine import Pin
import time

class DS18B20():
    def __init__(self, gpioObj, resolution=12):
        self.pin = None
        if not isinstance(gpioObj, GPIO):
            raise ValueError("parameter is not a GPIO object")

        self.pin = gpioObj.port()
        self.no_addr=0
        self.addr=self.getaddr()
        self.unit='c'
        self.res=resolution

    def getaddr(self):
        ow=OneWire(Pin(self.pin))
        a=ow.scan()
        for i in a:
            self.no_addr+=1
        return a

    def getTemperature(self):
        if self.no_addr==0:
            print ("no sensors detected")
        if self.no_addr>=1:
            temp = self._request(self.addr[0])
            return temp

    def _request(self, addr):
        self._res(addr)
        ow=OneWire(Pin(self.pin))
        ow.reset()
        ow.select_rom(addr)
        ow.writebyte(0x44) #command to take reading
        if self.res==12: #the resolution determines the amount of time needed
            time.sleep_ms(1000)
        if self.res==11:
            time.sleep_ms(400)
        if self.res==10:
            time.sleep_ms(200)
        if self.res==9:
            time.sleep_ms(100)
        ow.reset() #reset required for data
        ow.select_rom(addr)
        ow.writebyte(0xBE)
        LSB=ow.readbyte()
        MSB=ow.readbyte()
        ow.readbyte()
        ow.readbyte()
        ow.readbyte()
        ow.readbyte()
        ow.readbyte()
        ow.readbyte()
        ow.readbyte()
        ow.reset()
        d_LSB=float(0)
        d_MSB=float(0)
        count=0
        b=bin(LSB)
        b2=bin(MSB)
        b3=""
        l=10-len(b2)
        for i in range(l):
            if len(b2)<10:
                b3+="0"
        b2=b3+b2
        b4=""
        l=10-len(b)
        for i in range(l):
            if len(b)<10:
                b4+="0"
        b5=b4+b
        for i in b5:
            if count == 2:
                if i=='1':
                    d_LSB+=2**3
            if count == 3:
                if i=='1':
                    d_LSB+=2**2
            if count == 4:
                if i=='1':
                    d_LSB+=2**1
            if count == 5:
                if i=='1':
                    d_LSB+=2**0
            if count == 6:
                if i=='1':
                    d_LSB+=2**-1
            if count == 7:
                if i=='1':
                    d_LSB+=2**-2
            if count == 8:
                if i=='1':
                    d_LSB+=2**-3
            if count == 9:
                if i=='1':
                    d_LSB+=2**-4
            count+=1
        count=0
        sign=1
        for i in b2:
            if count == 6:
                if i=='1':
                    sign=-1
            if count == 7:
                if i=='1':
                    d_MSB+=2**6
            if count == 8:
                if i=='1':
                    d_MSB+=2**5
            if count == 9:
                if i=='1':
                    d_MSB+=2**4
            count+=1
        temp=(d_LSB+d_MSB)*sign
        # if self.unit=='c'or self.unit=='C':
            # print("TEMP is: "+str(temp)+" degrees C")
        if self.unit=='f'or self.unit=='F':
            temp=(temp*9/5)+32
            # print("TEMP F is: "+str(temp))
        return temp

    def _res(self,addr):
        ow=OneWire(Pin(self.pin))
        ow.reset()
        ow.select_rom(addr)
        ow.writebyte(0x4E)
        if self.res==12:
            ow.writebyte(0x7F)
            ow.writebyte(0x7F)
            ow.writebyte(0x7F)
        if self.res==11:
            ow.writebyte(0x5F)
            ow.writebyte(0x5F)
            ow.writebyte(0x5F)
        if self.res==10:
            ow.writebyte(0x3F)
            ow.writebyte(0x3F)
            ow.writebyte(0x3F)
        if self.res==9:
            ow.writebyte(0x1F)
            ow.writebyte(0x1F)
            ow.writebyte(0x1F)
        ow.reset()
