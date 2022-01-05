from driver import GPIO

class DCMOTOR(object):

    def __init__(self, gpioObj):
        self.gpioObj = None
        if not isinstance(gpioObj, GPIO):
            raise ValueError("parameter is not a GPIO object")
        self.gpioObj = gpioObj

    def ctrl(self,value):
        if self.gpioObj is None:
            raise ValueError("invalid GPIO object")
        self.gpioObj.write(value)
