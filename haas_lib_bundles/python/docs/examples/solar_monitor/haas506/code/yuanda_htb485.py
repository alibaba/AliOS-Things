import ustruct

class HTB485(object):
    
    def __init__(self, mbObj, devAddr):
        self.mbObj = mbObj
        self.devAddr = devAddr
    
    def getHumidity(self):
        if self.mbObj is None:
            raise ValueError("invalid modbus object.")
        value = bytearray(4)
        ret = self.mbObj.readHoldingRegisters(self.devAddr, 0, 2, value, 200)
        if ret[0] < 0:
            raise ValueError("readHoldingRegisters failed. errno:", ret[0])
        
        humidity = ustruct.unpack('hh',value)
        return humidity[0]
    
    def getTemperature(self):
        if self.mbObj is None:
            raise ValueError("invalid modbus object.")
        value = bytearray(4)
        ret = self.mbObj.readHoldingRegisters(self.devAddr, 0, 2, value, 200)
        if ret[0] < 0:
            raise ValueError("readHoldingRegisters failed. errno:", ret[0])
        temperature = ustruct.unpack('hh',value)
        return temperature[1]
    
    def getBrightness(self):
        if self.mbObj is None:
            raise ValueError("invalid modbus object.")
        value = bytearray(4)
        ret = self.mbObj.readHoldingRegisters(self.devAddr, 2, 2, value, 200)
        if ret[0] < 0:
            raise ValueError("readHoldingRegisters failed. errno:", ret[0])
        brightness = ustruct.unpack('hh',value)
        return brightness[1]
    
    def getHTB(self):
        if self.mbObj is None:
            raise ValueError("invalid modbus object.")
        value = bytearray(10)
        ret = self.mbObj.readHoldingRegisters(self.devAddr, 0, 5, value, 200)
        if ret[0] < 0:
            raise ValueError("readHoldingRegisters failed. errno:", ret[0])
        htb = ustruct.unpack('hhhhh',value)
        return htb
