from driver import ADC,GPIO
from time import sleep_us

class GP2Y10(object):
    def __init__(self, adcObj,gpioObj):
        self.adcObj = None
        self.gpioObj = None
        if not isinstance(adcObj, ADC):
            raise ValueError("parameter is not an ADC object")

        if not isinstance(gpioObj, GPIO):
            raise ValueError("parameter is not an GPIO object")

        self.adcObj = adcObj
        self.gpioObj = gpioObj
        self.gpioObj.write(1)

    def getVoltage(self):
        if self.adcObj is None:
            raise ValueError("invalid ADC object")

        self.gpioObj.write(0)
        sleep_us(280)
        value = self.adcObj.readVoltage()
        sleep_us(40)
        self.gpioObj.write(1)
        sleep_us(9680)
        return value
