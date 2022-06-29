# Adapter for machine driver

from boardparser import BoardConfigParser
from machine import WDT as mach_WDT


"""

"wdt_test": {
    "type": "WDT",
    "id": 0,
    "timeout": 3000
}
"""
class WDT:
    def __init__(self):
        self.wdt = None
        self.id = 0
        self.timeout = 3000

    def open(self, node):
        if type(node) is str:          
            parser = BoardConfigParser()
            try:
                item = parser.findItem(node, 'WDT')
            except Exception as e:
                print(e)
                return BoardConfigParser.NODE_NOT_EXIST

            self.id = item['id']
            self.timeout = item['timeout']
            
            self.wdt = mach_WDT(self.id, self.timeout)
        else:
            raise ValueError('Node type should be str')

    def feed(self):
        self.wdt.feed()
        return 0

    def close(self):
        raise ValueError('WDT cannot be disabled again except by a reset')
