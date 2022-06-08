# Adapter for machine driver

import sys

if sys.platform == "pyboard":
    from pyb import Pin, Timer
else:
    from machine import PWM as mach_PWM
    
from machine import Pin as mach_Pin
from boardparser import BoardConfigParser

"""
"pwm_test": {
  "type": "PWM",
  "port": "A0",
}
    
Available port is:
Pin     Skin      Timer
PB14    Y15     TIM12_CH1
PB15    Y16     TIM12_CH2

PB6     X1      TIM4_CH1
PB7     X2      TIM4_CH2
PB8     Y11     TIM4_CH3
PB9     Y12     TIM4_CH4

PA6     X15     TIM3_CH1
PA7     X16     TIM3_CH2

PC6     Y9      TIM3_CH1
PC7     Y10     TIM3_CH2
PB0     Y3      TIM3_CH3
PB1     Y4      TIM3_CH4

PA5     X14     TIM2_CH1
PB3     X17     TIM2_CH2
PB10    Y1      TIM2_CH3
PB11    Y2      TIM2_CH4

PA0     X9      TIM2_CH1
PA1     X10     TIM2_CH2
PA2     X11     TIM2_CH3
PA3     X12     TIM2_CH4
    
"""

class PWM:

    _pinToTimerChan = {
        'B14': (2, 1),
        'B15': (2, 2),
        
        'B6': (4, 1),
        'B7': (4, 2),
        'B8': (4, 3),
        'B9': (4, 4),
        
        'A6': (3, 1),
        'A7': (3, 2),
        
        'C6': (3, 1),
        'C7': (3, 2),
        'B0': (3, 3),
        'B1': (3, 4),
        
        'A5': (2, 1),
        'B3': (2, 2),
        'B10': (2, 3),
        'B11': (2, 4),
        
        'A0': (2, 1),
        'A1': (2, 2),
        'A2': (2, 3),
        'A3': (2, 4)
    }
        
    def __init__(self):
        self.tim = None
        self.tchannel = None

    def open(self, node):
        if type(node) is str:          
            parser = BoardConfigParser()
            try:
                item = parser.findItem(node, 'PWM')
            except Exception as e:
                print(e)
                return BoardConfigParser.NODE_NOT_EXIST

            port = item['port']
            if port in self._pinToTimerChan:
                timerchan = self._pinToTimerChan[port]
                print('get timer and channel', timerchan)
            else:
                raise Exception('Can not set port {} as PWM'.format(port))
                
            if sys.platform == "pyboard":
                self.tim = Timer(timerchan[0], freq=1000)
                self.tchannel = self.tim.channel(timerchan[1],
                                                Timer.PWM,
                                                pin=Pin(port),
                                                pulse_width_percent=50)
            return 0
        else:
            raise ValueError('Node type should be str')

    def close(self):
        self.tim.deinit()
        self.tim = None
        return 0
         
    def setOption(self, opt_dict):
        if('freq' in opt_dict):
            freq = opt_dict['freq']
        
        if('duty' in opt_dict):
            duty = opt_dict['duty']

        self.tim.freq(freq)
        self.tchannel.pulse_width_percent(duty)

        return 0
    
    def getOption(self):      
        duty = self.tchannel.pulse_width_percent()
        freq = self.tim.freq()
        opt_dict = {'duty': duty, 'freq': int(freq)}
        
        return opt_dict
        