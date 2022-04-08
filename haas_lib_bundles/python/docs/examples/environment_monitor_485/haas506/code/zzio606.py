class ZZIO606(object):
    
    def __init__(self, mbObj, devAddr):
        self.mbObj = mbObj
        self.devAddr = devAddr

    def openChannel(self, chid):
        if self.mbObj is None:
            raise ValueError("invalid modbus object.")
        ret = self.mbObj.writeCoil(self.devAddr, chid, 0xff00, 200)
        return ret[0]

    def closeChannel(self, chid):
        if self.mbObj is None:
            raise ValueError("invalid modbus object.")
        ret = self.mbObj.writeCoil(self.devAddr, chid, 0, 200)
        return ret[0]
    
    def getChannelStatus(self):
        if self.mbObj is None:
            raise ValueError("invalid modbus object.")
        status = bytearray(1)
        ret = self.mbObj.readCoils(self.devAddr, 0, 6, status, 200)
        if ret[0] < 0:
            raise ValueError("modbus readCoils failed, errno:", ret[0])
        return status
