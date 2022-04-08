"""
HaaSPython PWM driver for servo
"""

from driver import PWM

class SERVO(object):

    def __init__(self, pwmObj):
        self.pwmObj = None
        if not isinstance(pwmObj, PWM):
            raise ValueError("parameter is not an PWM object")

        self.pwmObj = pwmObj

    def setOptionSero(self,data):
        if self.pwmObj is None:
            raise ValueError("invalid PWM object")

        data_r = {'freq':50, 'duty': int(((data+90)*2/180+0.5)/20*100)}
        self.pwmObj.setOption(data_r)

    def close(self):
        if self.pwmObj is None:
            raise ValueError("invalid PWM object")

        self.pwmObj.close()
