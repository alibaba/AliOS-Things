from time import sleep_us,ticks_us
from driver import GPIO

class HCSR04():
    def __init__(self,trigObj,echoObj):
        self.trig = None
        self.echo = None
        if not isinstance(trigObj, GPIO):
            raise ValueError("parameter is not a GPIO object")

        if not isinstance(echoObj, GPIO):
            raise ValueError("parameter is not a GPIO object")
        self.trig = trigObj
        self.echo = echoObj

    def measureDistance(self):
        distance=0
        self.trig.write(1)
        sleep_us(20)
        self.trig.write(0)
        while self.echo.read() == 0:
            pass
        if self.echo.read() == 1:
            ts=ticks_us()                   #开始时间
            while self.echo.read() == 1:   #等待脉冲高电平结束
                pass
            te=ticks_us()                   #结束时间
            tc=te-ts                        #回响时间（单位us，1us=1*10^(-6)s）
            distance=(tc*170)/10000         #距离计算 （单位为:cm）
        return distance
