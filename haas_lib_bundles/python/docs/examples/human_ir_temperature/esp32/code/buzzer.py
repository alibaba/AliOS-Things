from driver import PWM

class BUZZER(object):

    def __init__(self, pwmObj):
        self.pwmObj = None
        if not isinstance(pwmObj, PWM):
            raise ValueError("parameter is not an PWM object")

        self.pwmObj = pwmObj

    def setOptionDuty(self,data):
        if self.pwmObj is None:
            raise ValueError("invalid PWM object")

        self.pwmObj.setOption(data)

    def start(self,obj):
        if self.pwmObj is None:
            raise ValueError("invalid PWM object")

        self.pwmObj.open(obj)

    def close(self):
        if self.pwmObj is None:
            raise ValueError("invalid PWM object")

        self.pwmObj.close()
