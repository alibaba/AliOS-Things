from driver import GPIO

class MagnetronSensor():

    def __init__(self, gpioObj):
        self.gpioObj = None
        if not isinstance(gpioObj, GPIO):
            raise ValueError("parameter is not a GPIO object")

        self.gpioObj = gpioObj

    def isMagnetic(self):
        if self.gpioObj is None:
            raise ValueError("invalid GPIO object")

        value = self.gpioObj.read()
        return value
