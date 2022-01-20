class LED:
    def __init__(self, id):
        self.f = open("/sys/class/leds/%s/brightness" % id, "r+b")

    def on(self):
        self.f.write(b"255")

    def off(self):
        self.f.write(b"0")

    def get(self):
        self.f.seek(0)
        return int(self.f.read())

    def toggle(self):
        v = self.get()
        if v:
            self.off()
        else:
            self.on()
