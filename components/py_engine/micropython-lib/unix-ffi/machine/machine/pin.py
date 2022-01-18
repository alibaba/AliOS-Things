import umachine


class Pin(umachine.PinBase):

    IN = "in"
    OUT = "out"

    def __init__(self, no, dir=IN):
        pref = "/sys/class/gpio/gpio{}/".format(no)
        dirf = pref + "direction"
        try:
            f = open(dirf, "w")
        except OSError:
            with open("/sys/class/gpio/export", "w") as f:
                f.write(str(no))
            f = open(dirf, "w")
        f.write(dir)
        f.close()
        self.f = open(pref + "value", "r+b")

    def value(self, v=None):
        if v is None:
            self.f.seek(0)
            return 1 if self.f.read(1) == b"1" else 0
        self.f.write(b"1" if v else b"0")

    def deinit(self):
        self.f.close()
