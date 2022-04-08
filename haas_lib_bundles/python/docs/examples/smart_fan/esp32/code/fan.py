from driver import PWM

class Fan(object):

    def __init__(self, pwmObj,data=None):
        self.pwmObj = None
        if not isinstance(pwmObj, PWM):
            raise ValueError("parameter is not an PWM object")

        self.pwmObj = pwmObj
        if data is not None:
            self.data = data
            self.setOptionDuty()
        else:
            self.data = {'freq':2000, 'duty': 0 }

    def setOptionDuty(self):
        if self.pwmObj is None:
            raise ValueError("invalid PWM object")
        self.pwmObj.setOption(self.data)

    def control(self,gear):
        if not isinstance(gear,int):
            raise ValueError("gear is not an int object")
        if not gear in range(4):
            raise ValueError("gear must be in range 0-3")
        if gear == 0:
            self.data['duty'] = 0
        if gear == 1:
            self.data['duty'] = 33
        if gear == 2:
            self.data['duty'] = 66
        if gear == 3:
            self.data['duty'] = 99
        self.setOptionDuty()


