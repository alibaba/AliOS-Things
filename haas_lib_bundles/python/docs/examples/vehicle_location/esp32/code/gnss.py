from driver import UART
from micropyGNSS import MicropyGNSS

class Gnss(object):

    def __init__(self, uartObj):
        self.uartObj = None
        if not isinstance(uartObj, UART):
            raise ValueError("parameter is not a GPIO object")
        
        # 初始化定位模组串口
        self.uartObj = uartObj
        self.gnss = MicropyGNSS(location_formatting='dd')

    def getLocation(self):
        if self.uartObj is None:
            raise ValueError("invalid UART object")
        # 创建定位信息解析器   
        sentence = bytearray(100)
        recvsize = self.uartObj.read(sentence)
        if(recvsize):
            print(sentence)
            sentence = sentence.decode()
            # 解析地理位置信息
            for c in sentence:
                self.gnss.update(c)
            print(self.gnss.longitude, self.gnss.latitude, self.gnss.altitude)
        
        return self.gnss