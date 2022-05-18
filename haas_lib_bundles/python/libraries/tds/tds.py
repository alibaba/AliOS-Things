from driver import ADC

class TDS(object):

    def __init__(self, adcObj, T = 25):
        self._adcObj = None
        if not isinstance(adcObj, ADC):
            raise ValueError("parameter is not an ADC object")

        self._adcObj = adcObj
        self.temperature = T

    def setTemperature(self, T):
        self.temperature = T

    def getTDS(self):
        if self._adcObj is None:
            raise ValueError("invalid ADC object")
        min = 400
        max = 0
        value = 0
        total = 0

        for i in range(32):
            value = self._adcObj.readVoltage()
            total += value
            # print(value)
            if (min >= value):
                min = value
            if (max <= value):
                max = value
        averageVoltage = (total - min - max) / 30
        averageVoltage /= 1000.0 # unit:V
        # temperature compensation formula: fFinalResult(25^C) = fFinalResult(current)/(1.0+0.02*(fTP-25.0));
        cC = 1.0 + 0.02 * (self.temperature - 25.0); # cC - compensationCoefficient
        cV = averageVoltage / cC # cV - compensationVolatge
        # convert voltage to TDS
        tds = 66.71 * cV * cV * cV - 127.93 * cV * cV + 428.7 * cV

        return int(tds)
