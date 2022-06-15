class DRV8833():

    def __init__(self, Int1x, Int2x,freq):
        self.int1x = Int1x
        self.int2x = Int2x
        if freq == None:
            self.mode = 0
        else:
            self.mode = 1
            self.freq = freq

    def run(self, status, rate):
        if self.mode == 0 :
            if status == 1:
                self.int1x.write(0)
                self.int2x.write(1)
            elif status == 2:
                self.int1x.write(1)
                self.int2x.write(0)
        elif self.mode == 1:
            return

    def stop(self):
        if self.mode == 0 :
            self.int1x.write(1)
            self.int2x.write(1)
        elif self.mode == 1:
            return
