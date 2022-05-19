from driver import PWM

class BUZZER(object):

    def __init__(self, pwmObj,data=None):
        self.pwmObj = None
        if not isinstance(pwmObj, PWM):
            raise ValueError("parameter is not an PWM object")

        self.pwmObj = pwmObj
        if data is not None:
            self.setOptionDuty(data)

    def setOptionDuty(self,data):
        if self.pwmObj is None:
            raise ValueError("invalid PWM object")

        self.pwmObj.setOption(data)

    def start(self,data):
        if self.pwmObj is None:
            raise ValueError("invalid PWM object")
        self.setOptionDuty(data)

    def close(self,data):
        if self.pwmObj is None:
            raise ValueError("invalid PWM object")
        self.setOptionDuty(data)

