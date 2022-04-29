from driver import GPIO
import utime

class HX710(object):
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
