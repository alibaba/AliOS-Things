from driver import ADC

class HCHO(object):

    def __init__(self, adcObj):
        self.adcObj = None
        if not isinstance(adcObj, ADC):
            raise ValueError("parameter is not an ADC object")
        
        self.adcObj = adcObj

    def getPPM(self):
        if self.adcObj is None:
            raise ValueError("invalid ADC object")
        min = 400
        max = 0
        value = 0
        total = 0
        i = 0
        for i in range(32):
            value = self.adcObj.readVoltage()
            total += value
            # print(value)
            if (min >= value):
                min = value
            if (max <= value):
                max = value
        analogVoltage = (total - min - max) / 30
        analogVoltage += 23
        analogVoltage /= 1000.0
        ppm = 3.125 * analogVoltage - 1.25	#linear relationship(0.4V for 0 ppm and 2V for 5ppm)
        if ppm < 0 :
           ppm = 0 
        return ppm   

