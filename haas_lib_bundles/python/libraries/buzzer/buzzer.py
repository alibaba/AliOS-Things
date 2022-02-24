"""
HaaSPython PWM driver for buzzer
"""

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

    def close(self):
        if self.pwmObj is None:
            raise ValueError("invalid PWM object")

        self.pwmObj.close()
