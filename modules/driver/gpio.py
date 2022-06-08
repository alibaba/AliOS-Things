# Adapter for machine driver

import sys

from boardparser import BoardConfigParser
from machine import Pin as mach_Pin

BOARD_JSON_PATH = '/data/pyamp/board.json'

"""
GPIO APIs
"led": {
    "type": "GPIO",
    "port": "E2",
    "dir": "output", ---- same as "mode"
    "pull": "pullup"
}
"""
class GPIO:
    
    _dirStrToInt = {
        'input': mach_Pin.IN,            # 0
        'output': mach_Pin.OUT,          # 1
        'alt': mach_Pin.ALT,             # 2
        'analog': mach_Pin.ANALOG,       # 3
        'opendrain': mach_Pin.OUT_OD,    # 17
        'altopendrain': mach_Pin.ALT_OPEN_DRAIN # 18
    }
    
    def __init__(self):
        self.pin = None

    def open(self, node):
        if self.pin is not None:
            return -1

        if type(node) is str:          
            parser = BoardConfigParser()
            try:
                item = parser.findItem(node, 'GPIO')
            except Exception as e:
                print(e)
                return BoardConfigParser.NODE_NOT_EXIST
            
            if type(item['port']) is not str:
                raise ValueError('port fild should be str')
            else:
                self.port = item['port']
            
            # Check dir option
            if 'dir' in item:
                self.dir = self._dirStrToInt[item['dir']]
            else:
                raise Exception('dir un-assigned')
            
            # Check pull option
            if 'pull' in item:
                if item['pull'] == 'pullup':
                    self.pull = mach_Pin.PULL_UP
                elif item['pull'] == 'pulldown':
                    self.pull = mach_Pin.PULL_DOWN
                elif item['pull'] == 'none':
                    self.pull = mach_Pin.PULL_NONE
                else:
                    raise ValueError('unSupported pull type, avaiable type is {pullup, pulldown, none}')
            else:
               self.pull = mach_Pin.PULL_NONE
            
            # Check intMode exist or not
            if('intMode' in item):
                if item['intMode'] == 'rising':
                    self.irq = mach_Pin.IRQ_RISING
                elif item['intMode'] == 'falling':
                    self.irq = mach_Pin.IRQ_FALLING
                elif item['intMode'] == 'both':
                    self.irq = mach_Pin.IRQ_FALLING or mach_Pin.IRQ_RISING
                else:
                    raise Exception('unSupported IRQ mode,avaiable type is {rising, falling, both}')
            else:
                self.irq = mach_Pin.IRQ_RISING
            
            self.pin = mach_Pin(self.port,
                                mode=self.dir,
                                pull=self.pull,
                                value=None,
                                af=-1,
                                alt=-1)
            return 0
        else:
            raise ValueError('Node type should be str')

    def close(self):
        if self.pin is None:
            #raise Exception('Close disabled when GPIO closed')
            return -1
        else:
            self.pin = None
            return 0

    def read(self):
        pin = self.pin
        if pin is not None:
            return pin.value()
        else:
            #raise Exception('Read disabled when GPIO closed')
            return -1
    
    def write(self, x):
        pin = self.pin
        if pin is not None:
            pin.value(x)
            return 0
        else:
            #raise Exception('Write disabled when GPIO closed')
            return -1

    def on(self, cb):
        pin = self.pin
        pin.irq(handler=cb,
                trigger=self.irq)
