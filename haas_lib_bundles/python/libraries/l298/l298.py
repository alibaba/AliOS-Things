class L298DC():

    def __init__(self, Int1x, Int2x, en=None, freq=50, name="DCMotorX"):
        self.name = name
        self.int1x = Int1x
        self.int2x = Int2x
        self.en = en
        self.freq = freq

    def run(self, rate=50):
        if(self.en != None):
            self.en.setOption({self.freq, rate if rate > 0 else -rate})
            self.int1x.setOption({self.freq, 0 if rate > 0 else 100})
            self.int2x.setOption({self.freq, 100 if rate > 0 else 0})
        else:
            self.int1x.setOption({self.freq, 0 if rate > 0 else 100})
            self.int2x.setOption({self.freq, rate if rate > 0 else 0})

    def stop(self):
        if(self.en != None):
            self.en.setOption({self.freq, 100})
        self.int1x.setOption({self.freq, 0})
        self.int2x.setOption({self.freq, 0})

    def enable(self):
        if(self.en == None):
            raise SystemError("driver EN pin not specific。")
        else:
            self.en.setOption({self.freq, 100})

    def disable(self):
        if(self.en == None):
            raise SystemError("driver EN pin not specific。")
        else:
            self.en.setOption({self.freq, 0})
