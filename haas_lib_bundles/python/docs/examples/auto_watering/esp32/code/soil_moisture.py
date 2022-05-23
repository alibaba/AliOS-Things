from driver import GPIO
from driver import ADC

class SoilMoisture(object):
    def __init__(self, DO, AO=None):
        self.DO = None
        self.AO = None
        if not isinstance(DO, GPIO):
            raise ValueError('parameter DO is not an GPIO object')
        if AO is not None and not isinstance(AO, ADC):
            raise ValueError('parameter AO should be ADC object or None')

        self.DO = DO
        self.AO = AO

    # 读取数字信号
    def moistureDetect(self):
        return self.DO.read()

    # 读取模拟信号，电压
    def getVoltage(self):
        if not self.AO:
            raise RuntimeError('Can not get voltage, AO is not inited')
        return self.AO.readVoltage()
