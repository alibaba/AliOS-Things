# Adapter for machine driver
    
from machine import Timer as mach_Timer

class TIMER:
    
    ONE_SHOT = 1
    PERIODIC = 2

    def __init__(self, id):
        self.tim = mach_Timer(-1)

    def open(self, period = 1000, mode = mach_Timer.PERIODIC, callback = None):
        if mode != mach_Timer.ONE_SHOT and mode != mach_Timer.PERIODIC:
            raise ValueError('Mode:{} value wrong'.format(mode))
        
        self.period = period
        self.mode = mode
        self.cb = callback
        
    def close(self):
        self.tim.deinit()
        return 0
         
    def start(self):
        self.tim.init(mode=self.mode, period=self.period, callback=self.cb)
        return 0
        
    def stop(self):
        self.tim.deinit()
        return 0
        
    def period(self, *args):
        if len(args) == 0:
            return self.period
        else:
            self.period = args[0]
            
            self.tim.deinit()
            self.tim.init(mode=self.mode, period=self.period, callback=self.cb)
        
    def reload(self):
        self.start()
        return 0
        