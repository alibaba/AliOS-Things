from driver import ADC

class WATER4LEVEL(object):
    def __init__(self, adcObj):
        self.adcObj = None
        if not isinstance(adcObj, ADC):
            raise ValueError("parameter is not an ADC object")

        self.adcObj = adcObj

    def measureLevel(self):
        if self.adcObj is None:
            raise ValueError("invalid ADC object")

        value = self.adcObj.readVoltage()
        if value >= 0 and value < 800:
            return 0
        elif value >= 800 and value < 1100:
            return 1
        elif value >= 1100 and value < 1300:
            return 2
        elif value >= 1300 and value < 1400:
            return 3
        elif value >= 1400:
            return 4
