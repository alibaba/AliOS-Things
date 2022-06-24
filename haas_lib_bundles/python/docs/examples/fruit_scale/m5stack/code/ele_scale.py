from machine import Pin
from driver import GPIO
import utime

class Hx711(object):
    def __init__(self, clkObj, dataObj):
        self.clkObj = None
        self.dataObj = None
        if not isinstance(clkObj, GPIO):
            raise ValueError("parameter is not an GPIO object")
        if not isinstance(dataObj, GPIO):
            raise ValueError("parameter is not an GPIO object")

        self.clk = Pin(clkObj.port(), Pin.OUT)
        self.data = Pin(dataObj.port(), Pin.IN, Pin.PULL_DOWN)

    def getValue(self):
        count = 0
        self.clk.value(0)

        while(self.data.value()):
            utime.sleep_ms(1)

        for i in range(24):
            self.clk.value(1)
            count = count<<1
            self.clk.value(0)
            if(self.data.value()):
                count += 1

        self.clk.value(1)
        count ^= 0x800000
        self.clk.value(0)

        return count


class EleScale(Hx711):
    # capValue = 429.5是理论值，可通过调整该参数进行校准，
    # 如果测量值偏大则适当增大capValue，如果测量值偏小则减小该值。
    def __init__(self, clkObj, dataObj, capValue = 429.5):
        Hx711.__init__(self, clkObj, dataObj)
        self.noLoadOffset = self.__hx711Read(10)
        self.capValue = capValue

    def __hx711Read(self, times = 3):
        # times必须 >= 3
        cnt = 3 if (times <= 3) else times

        if (cnt % 2):
            cut = cnt // 2
        else:
            cut = (cnt // 2) -1

        idx = 0
        data = [0] * cnt
        while (idx < cnt):
            data[idx] = self.getValue()
            idx += 1

        data.sort()
        # 取中位数作为结果返回
        return round(sum(data[cut:-cut]) / (len(data[cut:-cut])))

    def getWeight(self):
        data = self.__hx711Read(9) - self.noLoadOffset
        if (data <= 0):
            weight = 0.0
        else:
            weight = data / self.capValue

        return weight

