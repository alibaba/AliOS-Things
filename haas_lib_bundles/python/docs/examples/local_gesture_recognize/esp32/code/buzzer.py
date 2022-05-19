from driver import GPIO
from time import sleep


class Buzzer:
    def __init__(self, io_name) -> None:
        self.gpio = GPIO()
        ret = self.gpio.open(io_name)
        if ret < 0:
            raise Exception("Error opening gpio ", io_name, " ret=", ret)
        ret = self.gpio.write(0)
        if ret < 0:
            raise Exception("Error writing init value 1 to gpio ", io_name, " ret=", ret)

    def beep(self, duration_ms):
        self.gpio.write(1)
        sleep(duration_ms / 1000)
        self.gpio.write(0)
