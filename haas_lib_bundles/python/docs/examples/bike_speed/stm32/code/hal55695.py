import utime
from driver import GPIO


class HAL55695():
    def  __init__(self, gpioObj):
        self.gpioObj = None
        if not isinstance(gpioObj, GPIO):
            raise ValueError("parameter gpioObj must be an GPIO object")
        
        self.gpioObj = gpioObj
    
    def getHallValue(self):
        if self.gpioObj is None:
            raise ValueError("invalid GPIO object")
        value = self.gpioObj.read()
        return value
        
    def speed_calulate(self):
        speed = 0
        while self.gpioObj.read() == 1:
            pass
        if self.gpioObj.read() == 0:
            ts=utime.ticks_ms()                   #开始时间
            utime.sleep(0.25)
            while self.gpioObj.read() == 1:   #等待车轮转一圈
                pass
            te=utime.ticks_ms()                   #结束时间
            tc=te-ts                        #车轮转一圈的时间
            print("the time is %d" %tc)
            if(tc == 0):
                raise ValueError("车轮未转")
            speed = 66.04*3.14/tc *10         #速度计算，车轮周长/时间 （单位为:m/s）这里假设车轮周长是20
        return speed
