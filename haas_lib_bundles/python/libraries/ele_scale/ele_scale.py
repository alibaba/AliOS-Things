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

        self.clkObj = clkObj
        self.dataObj = dataObj

    def getValue(self):
        if self.clkObj is None:
            raise ValueError("invalid GPIO object")
        if self.dataObj is None:
            raise ValueError("invalid GPIO object")
        count = 0
        self.dataObj.write(1)
        self.clkObj.write(0)

        while(self.dataObj.read()):
            utime.sleep_ms(1)

        for i in range(24):
            self.clkObj.write(1)
            count = count<<1
            self.clkObj.write(0)
            if(self.dataObj.read()):
                count += 1

        self.clkObj.write(1)
        count ^= 0x800000
        self.clkObj.write(0)

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

        idx = 0
        data = [0] * cnt
        while (idx < cnt):
            data[idx] = self.getValue()
            idx += 1

        data.sort()
        # 去掉最大最小值后的平均值作为结果返回
        return round(sum(data[1:-1]) / (len(data) - 2))

    def getWeight(self):
        data = self.__hx711Read() - self.noLoadOffset
        if (data <= 0):
            weight = 0.0
        else:
            weight = data / self.capValue

        return weight

