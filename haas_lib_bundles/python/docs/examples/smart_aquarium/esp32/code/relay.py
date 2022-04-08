from driver import GPIO

class Relay():

    def __init__(self, gpioObj, trigger):
        self.gpioObj = None
        if not isinstance(gpioObj, GPIO):
            raise ValueError("parameter gpioObj is not a GPIO object")
        if (trigger is not 0) and (trigger is not 1):
            raise ValueError("parameter trigger should be 0 or 1")
        self.trigger = trigger
        self.gpioObj = gpioObj

    def open(self):
        if self.gpioObj is None:
            raise ValueError("invalid GPIO object")

        value = self.gpioObj.write(self.trigger)
        return value

    def close(self):
        if self.gpioObj is None:
            raise ValueError("invalid GPIO object")

        value = self.gpioObj.write(1 - self.trigger)
        return value
