"""
Copyright (C) 2015-2021 Alibaba Group Holding Limited

    MicroPython's driver for QMP6988

    Author: HaaS
    Date:   2021/09/14
"""

from driver import I2C
from utime import sleep_ms
from micropython import const
import math

QMP6988_CALC_INT = 1

QMP6988_CHIP_ID         = const(0x5C)

QMP6988_CHIP_ID_REG             = const(0xD1)
QMP6988_RESET_REG               = const(0xE0) # Device reset register
QMP6988_DEVICE_STAT_REG         = const(0xF3)  # Device state register
QMP6988_CTRLMEAS_REG            = const(0xF4)  # Measurement Condition Control Register
# data
QMP6988_PRESSURE_MSB_REG        = const(0xF7)  # Pressure MSB Register
QMP6988_TEMPERATURE_MSB_REG     = const(0xFA) # Temperature MSB Reg

# compensation calculation
QMP6988_CALIBRATION_DATA_START  = const(0xA0) # QMP6988 compensation coefficients
QMP6988_CALIBRATION_DATA_LENGTH = const(25)

SHIFT_RIGHT_4_POSITION          = const(4)
SHIFT_LEFT_2_POSITION           = const(2)
SHIFT_LEFT_4_POSITION           = const(4)
SHIFT_LEFT_5_POSITION           = const(5)
SHIFT_LEFT_8_POSITION           = const(8)
SHIFT_LEFT_12_POSITION          = const(12)
SHIFT_LEFT_16_POSITION          = const(16)

# power mode
QMP6988_SLEEP_MODE              = const(0x00)
QMP6988_FORCED_MODE             = const(0x01)
QMP6988_NORMAL_MODE             = const(0x03)

QMP6988_CTRLMEAS_REG_MODE__POS  = const(0)
QMP6988_CTRLMEAS_REG_MODE__MSK  = const(0x03)
QMP6988_CTRLMEAS_REG_MODE__LEN  = const(2)

# oversampling
QMP6988_OVERSAMPLING_SKIPPED    = const(0x00)
QMP6988_OVERSAMPLING_1X         = const(0x01)
QMP6988_OVERSAMPLING_2X         = const(0x02)
QMP6988_OVERSAMPLING_4X         = const(0x03)
QMP6988_OVERSAMPLING_8X         = const(0x04)
QMP6988_OVERSAMPLING_16X        = const(0x05)
QMP6988_OVERSAMPLING_32X        = const(0x06)
QMP6988_OVERSAMPLING_64X        = const(0x07)

QMP6988_CTRLMEAS_REG_OSRST__POS = const(5)
QMP6988_CTRLMEAS_REG_OSRST__MSK = const(0xE0)
QMP6988_CTRLMEAS_REG_OSRST__LEN = const(3)

QMP6988_CTRLMEAS_REG_OSRSP__POS = const(2)
QMP6988_CTRLMEAS_REG_OSRSP__MSK = const(0x1C)
QMP6988_CTRLMEAS_REG_OSRSP__LEN = const(3)
# filter
QMP6988_FILTERCOEFF_OFF         = const(0x00)
QMP6988_FILTERCOEFF_2           = const(0x01)
QMP6988_FILTERCOEFF_4           = const(0x02)
QMP6988_FILTERCOEFF_8           = const(0x03)
QMP6988_FILTERCOEFF_16          = const(0x04)
QMP6988_FILTERCOEFF_32          = const(0x05)

QMP6988_CONFIG_REG              = const(0xF1)  #IIR filter co-efficient setting Register
QMP6988_CONFIG_REG_FILTER__POS  = const(0)
QMP6988_CONFIG_REG_FILTER__MSK  = const(0x07)
QMP6988_CONFIG_REG_FILTER__LEN  = const(3)

SUBTRACTOR = const(8388608)

qmp6988_dict = {'Ctemp': 0.0, 'Ftemp': 0.0,'pressure': 0.0, 'altitude': 0.0}
class qmp6988Error(Exception):
    def __init__(self, value=0, msg="qmp6988 common error"):
        self.value = value
        self.msg = msg

    def __str__(self):
        return "Error code:%d, Error message: %s" % (self.value, str(self.msg))

    __repr__ = __str__

class QMP6988(object):
    """
    This class implements qmp6988 chip's defs.
    """
    def __init__(self, i2cDev):
        self._i2cDev = None
        if not isinstance(i2cDev, I2C):
            raise ValueError("parameter is not an I2C object")
        # make QMP6988's internal object points to i2cDev
        self._i2cDev = i2cDev

        self.init()
        self.ik_a0 = 0
        self.ik_b00 = 0
        self.ik_a1 = 0
        self.ik_a2 = 0
        self.ik_bt1 = 0
        self.ik_bt2 = 0
        self.ik_bp1 = 0
        self.ik_b11 = 0
        self.ik_bp2 = 0
        self.ik_b12 = 0
        self.ik_b21 = 0
        self.ik_bp3 = 0

        self.fk_a0 = 0.0
        self.fk_b00 = 0.0
        self.fk_a1 = 0.0
        self.fk_a2 = 0.0
        self.fk_bt1 = 0.0
        self.fk_bt2 = 0.0
        self.fk_bp1 = 0.0
        self.fk_b11 = 0.0
        self.fk_bp2 = 0.0
        self.fk_b12 = 0.0
        self.fk_b21 = 0.0
        self.fk_bp3 = 0.0

        self.power_mode = 0
        self.temperature = 0
        self.init()

    def int16(self, dat):
        #return int(dat)
        if dat > 32767:
            return dat - 65536
        else:
            return dat

    def int32(self, dat):
        #return int(dat)
        if dat > (1 << 31):
            return dat - (1 << 32)
        else:
            return dat

    def int64(self, dat):
        #return int(dat)
        if dat > (1 << 63):
            return dat - (1 << 64)
        else:
            return dat

    #写寄存器
    def writeReg(self, addr, value):
        Reg = bytearray([addr, value])
        self._i2cDev.write(Reg)
        #print("--> write addr " + hex(addr) + ", value = " + hex(value))
        return 0

   #读寄存器
    def readReg(self, addr, len):
        Reg = bytearray([addr])
        self._i2cDev.write(Reg)
        sleep_ms(2)
        tmp = bytearray(len)
        self._i2cDev.read(tmp)
        #print("<-- read addr " + hex(addr) + ", value = " + hex(tmp[0]))
        return tmp

    def deviceCheck(self):
        ret = self.readReg(QMP6988_CHIP_ID_REG, 1)[0]
        #print("qmp6988 read chip id = " + hex(ret))
        if (ret == QMP6988_CHIP_ID):
            return 0
        else:
            return 1

    def getCalibrationData(self):
        if (QMP6988_CALC_INT):
            pass
        else:
            Conv_A_S = [[-6.30E-03, 4.30E-04],
                        [-1.90E-11, 1.20E-10],
                        [1.00E-01, 9.10E-02],
                        [1.20E-08, 1.20E-06],
                        [3.30E-02, 1.90E-02],
                        [2.10E-07, 1.40E-07],
                        [-6.30E-10, 3.50E-10],
                        [2.90E-13, 7.60E-13],
                        [2.10E-15, 1.20E-14],
                        [1.30E-16, 7.90E-17]]

        a_data_u8r = bytearray(QMP6988_CALIBRATION_DATA_LENGTH)
        for len in range(QMP6988_CALIBRATION_DATA_LENGTH):
            a_data_u8r[len] = self.readReg(QMP6988_CALIBRATION_DATA_START + len, 1)[0]

        COE_a0 = self.int32(((a_data_u8r[18] << SHIFT_LEFT_12_POSITION) | (a_data_u8r[19] << SHIFT_LEFT_4_POSITION) | (a_data_u8r[24] & 0x0f)) << 12)
        COE_a0 = self.int32(COE_a0 >> SHIFT_LEFT_12_POSITION)
        COE_a1 = self.int16((a_data_u8r[20] << SHIFT_LEFT_8_POSITION) | a_data_u8r[21])
        COE_a2 = self.int16((a_data_u8r[22] << SHIFT_LEFT_8_POSITION) | a_data_u8r[23])
        COE_b00 = self.int32((a_data_u8r[0] << SHIFT_LEFT_12_POSITION) | (a_data_u8r[1] << SHIFT_LEFT_4_POSITION) | ((a_data_u8r[24] & 0xf0) >> SHIFT_RIGHT_4_POSITION)) << 12
        COE_b00 = self.int32(COE_b00 >> SHIFT_LEFT_12_POSITION)
        COE_bt1 = self.int16((a_data_u8r[2] << SHIFT_LEFT_8_POSITION) | a_data_u8r[3])
        COE_bt2 = self.int16((a_data_u8r[4] << SHIFT_LEFT_8_POSITION) | a_data_u8r[5])
        COE_bp1 = self.int16((a_data_u8r[6] << SHIFT_LEFT_8_POSITION) | a_data_u8r[7])
        COE_b11 = self.int16((a_data_u8r[8] << SHIFT_LEFT_8_POSITION) | a_data_u8r[9])
        COE_bp2 = self.int16((a_data_u8r[10] << SHIFT_LEFT_8_POSITION) | a_data_u8r[11])
        COE_b12 = self.int16((a_data_u8r[12] << SHIFT_LEFT_8_POSITION) | a_data_u8r[13])
        COE_b21 = self.int16((a_data_u8r[14] << SHIFT_LEFT_8_POSITION) | a_data_u8r[15])
        COE_bp3 = self.int16((a_data_u8r[16] << SHIFT_LEFT_8_POSITION) | a_data_u8r[17])
        """"
        print("<-----------calibration data-------------->")
        print("COE_a0[%d] COE_a1[%d] COE_a2[%d] COE_b00[%d]" %(COE_a0, COE_a1, COE_a2, COE_b00))
        print("COE_bt1[%d] COE_bt2[%d] COE_bp1[%d] COE_b11[%d]" %(COE_bt1, COE_bt2, COE_bp1, COE_b11))
        print("COE_bp2[%d] COE_b12[%d] COE_b21[%d] COE_bp3[%d]" %(COE_bp2, COE_b12, COE_b21, COE_bp3))
        print("<-----------calibration data-------------->")
        """
        if (QMP6988_CALC_INT):
            self.ik_a0 = COE_a0 # 20Q4
            self.ik_b00 = COE_b00 # 20Q4
            self.ik_a1 = self.int32(3608 * (COE_a1) - 1731677965) # 31Q23
            self.ik_a2 = self.int32(16889 * (COE_a2) - 87619360) # 30Q47
            self.ik_bt1 = self.int64(2982 * (COE_bt1) + 107370906) # 28Q15
            self.ik_bt2 = self.int64(329854 * (COE_bt2) + 108083093) # 34Q38
            self.ik_bp1 = self.int64(19923 * (COE_bp1) + 1133836764) # 31Q20
            self.ik_b11 = self.int64(2406 * (COE_b11) + 118215883) # 28Q34
            self.ik_bp2 = self.int64(3079 * (COE_bp2) - 181579595) # 29Q43
            self.ik_b12 = self.int64(6846 * (COE_b12) + 85590281) # 29Q53
            self.ik_b21 = self.int64(13836 * (COE_b21) + 79333336) # 29Q60
            self.ik_bp3 = self.int64(2915 * (COE_bp3) + 157155561) # 28Q65
            """
            print("<----------- int calibration data -------------->")
            print("a0[%d] a1[%d] a2[%d] b00[%d]" %(self.ik_a0, self.ik_a1, self.ik_a2, self.ik_b00))
            print("bt1[%d] bt2[%d] bp1[%d] b11[%d]" %(self.ik_bt1, self.ik_bt2, self.ik_bp1, self.ik_b11))
            print("bp2[%d] b12[%d] b21[%d] bp3[%d]" %(self.ik_bp2, self.ik_b12, self.ik_b21, self.ik_bp3))
            print("<----------- int calibration data -------------->")
            """
        else:
            self.fk_a0 = COE_a0 / 16.0
            self.fk_b00 = COE_b00 / 16.0

            self.fk_a1 = Conv_A_S[0][0] + Conv_A_S[0][1] * COE_a1 / 32767.0
            self.fk_a2 = Conv_A_S[1][0] + Conv_A_S[1][1] * COE_a2 / 32767.0
            self.fk_bt1 = Conv_A_S[2][0] + Conv_A_S[2][1] * COE_bt1 / 32767.0
            self.fk_bt2 = Conv_A_S[3][0] + Conv_A_S[3][1] * COE_bt2 / 32767.0
            self.fk_bp1 = Conv_A_S[4][0] + Conv_A_S[4][1] * COE_bp1 / 32767.0
            self.fk_b11 = Conv_A_S[5][0] + Conv_A_S[5][1] * COE_b11 / 32767.0
            self.fk_bp2 = Conv_A_S[6][0] + Conv_A_S[6][1] * COE_bp2 / 32767.0
            self.fk_b12 = Conv_A_S[7][0] + Conv_A_S[7][1] * COE_b12 / 32767.0
            self.fk_b21 = Conv_A_S[8][0] + Conv_A_S[8][1] * COE_b21 / 32767.0
            self.fk_bp3 = Conv_A_S[9][0] + Conv_A_S[9][1] * COE_bp3 / 32767.0

    def convTx02e(self, dt):
        if (QMP6988_CALC_INT):
            wk1 = self.int64((self.ik_a1) * (dt)) # 31Q23+24-1=54 (54Q23)
            wk2 = self.int64(((self.ik_a2) * (dt)) >> 14) # 30Q47+24-1=53 (39Q33)
            wk2 = self.int64((wk2 * (dt)) >> 10) # 39Q33+24-1=62 (52Q23)
            wk2 = self.int64(((wk1 + wk2) // 32767) >> 19) # 54,52->55Q23 (20Q04)
            ret = self.int16((self.ik_a0 + wk2) >> 4) # 21Q4 -> 17Q0
            return ret
        else:
            pass

    def getPressure02e(self, dp, tx):
        if (QMP6988_CALC_INT):
            wk1 = ((self.ik_bt1) * (tx)) # 28Q15+16-1=43 (43Q15)
            wk2 = self.int64((self.ik_bp1 * (dp)) >> 5) # 31Q20+24-1=54 (49Q15)
            wk1 += wk2 # 43,49->50Q15
            wk2 = self.int64(((self.ik_bt2) * (tx)) >> 1) # 34Q38+16-1=49 (48Q37)
            wk2 = self.int64((wk2 * self.int64(tx)) >> 8) # 48Q37+16-1=63 (55Q29)
            wk3 = wk2 # 55Q29
            wk2 = self.int64(((self.ik_b11) * (tx)) >> 4) # 28Q34+16-1=43 (39Q30)
            wk2 = self.int64((wk2 * (dp)) >> 1) # 39Q30+24-1=62 (61Q29)
            wk3 += wk2 # 55,61->62Q29
            wk2 = self.int64(((self.ik_bp2) * (dp)) >> 13) # 29Q43+24-1=52 (39Q30)
            wk2 = self.int64((wk2 * (dp)) >> 1) # 39Q30+24-1=62 (61Q29)
            wk3 += wk2 # 62,61->63Q29
            wk1 += self.int64(wk3 >> 14) # Q29 >> 14 -> Q15
            wk2 = ((self.ik_b12) * (tx)) # 29Q53+16-1=45 (45Q53)
            wk2 = (wk2 * (tx)) >> 22 # 45Q53+16-1=61 (39Q31)
            wk2 = (wk2 * (dp)) > 1 # 39Q31+24-1=62 (61Q30)
            wk3 = wk2 # 61Q30
            wk2 = (self.int64(self.ik_b21) * (tx)) >> 6 # 29Q60+16-1=45 (39Q54)
            wk2 = self.int64((wk2 * (dp)) >> 23) # 39Q54+24-1=62 (39Q31)
            wk2 = self.int64((wk2 * self.int64(dp)) >> 1) # 39Q31+24-1=62 (61Q20)
            wk3 += wk2 # 61,61->62Q30
            wk2 = self.int64(((self.ik_bp3) * (dp)) >> 12) # 28Q65+24-1=51 (39Q53)
            wk2 = self.int64((wk2 * (dp)) >> 23) # 39Q53+24-1=62 (39Q30)
            wk2 = (wk2 * (dp)) # 39Q30+24-1=62 (62Q30)
            wk3 += wk2 # 62,62->63Q30
            wk1 += self.int64(wk3 >> 15) # Q30 >> 15 = Q15
            wk1 = self.int64(wk1 // 32767)
            wk1 = self.int64(wk1 >> 11) # Q15 >> 7 = Q4
            wk1 += self.ik_b00 # Q4 + 20Q4
            # wk1 >>= 4 # 28Q4 -> 24Q0
            ret = self.int32(wk1)
            # print("wk1[%d] wk2[%d] ret[%d]" %(wk1, wk2, ret))
            return ret
        else:
            pass

    def softwareReset(self):
        pass

    def setPowermode(self, power_mode):
        # print("qmp_set_powermode ")
        self.power_mode = power_mode
        data = self.readReg(QMP6988_CTRLMEAS_REG,1)[0]
        data = data & 0xfc
        if (power_mode == QMP6988_SLEEP_MODE):
            data |= 0x00
        elif (power_mode == QMP6988_FORCED_MODE):
            data |= 0x01
        elif (power_mode == QMP6988_NORMAL_MODE):
            data |= 0x03

        self.writeReg(QMP6988_CTRLMEAS_REG, data)

        # print("qmp_set_powermode 0xf4=0x", data)
        sleep_ms(20)

    def setFilter(self, filter):
        data = (filter & 0x03)
        self.writeReg(QMP6988_CONFIG_REG, data)
        sleep_ms(20)

    def setOversamplingP(self, oversampling_p):
        data = self.readReg(QMP6988_CTRLMEAS_REG, 1)[0]
        data &= 0xe3
        data |= self.int16(oversampling_p << 2)
        self.writeReg(QMP6988_CTRLMEAS_REG, data)
        sleep_ms(20)

    def setOversamplingT(self, oversampling_t):
        data = self.readReg(QMP6988_CTRLMEAS_REG, 1)[0]
        data &= 0x1f
        data |= self.int16(oversampling_t << 5)
        self.writeReg(QMP6988_CTRLMEAS_REG, data)
        sleep_ms(20)

    def calcAltitude(self, pressure, temp):
        altitude = (pow((101325 / pressure), 1 / 5.257) - 1) * (temp + 273.15) / 0.0065
        return altitude

    def getData(self):
        global qmp6988_dict
        a_data_u8r = bytearray(6)
        # press
        a_data_u8r = self.readReg(QMP6988_PRESSURE_MSB_REG, 6)
        P_read = (a_data_u8r[0] << SHIFT_LEFT_16_POSITION) | (a_data_u8r[1] << SHIFT_LEFT_8_POSITION) | (a_data_u8r[2])
        P_raw = self.int32(P_read - SUBTRACTOR)
        T_read = (a_data_u8r[3] << SHIFT_LEFT_16_POSITION) | (a_data_u8r[4] << SHIFT_LEFT_8_POSITION) | (a_data_u8r[5])
        T_raw = self.int32(T_read - SUBTRACTOR)

        if (QMP6988_CALC_INT):
            T_int = self.convTx02e(T_raw)
            P_int = self.getPressure02e(P_raw, T_int)
            #self.temperature = float(T_int) / 256.0
            Ctemp = float(T_int) / 256.0
            Ftemp = (Ctemp * 9 / 5) + 32
            Ftemp = round(Ftemp,2)
            #self.pressure = float(P_int) / 16.0
            qmp6988_dict['Ctemp'] = Ctemp
            qmp6988_dict['Ftemp'] = Ftemp
            qmp6988_dict['pressure'] = float(P_int) / 16.0
            #print("int temp = %f Pressure = %f " %(self.temperature, self.pressure))
        else:
            Tr = self.fk_a0 + (self.fk_a1 * T_raw) + (self.fk_a2 * T_raw) *T_raw
            # Unit centigrade
            #self.temperature = float(Tr) / 256.0
            qmp6988_dict['Ctemp'] = float(Tr) / 256.0
            # compensation pressure, Unit Pa
            qmp6988_dict['pressure'] = self.fk_b00 + (self.fk_bt1 * Tr) + (self.fk_bp1 * P_raw) + (self.fk_b11 * Tr) *P_raw + (self.fk_bt2 * Tr) *Tr + (self.fk_bp2 * P_raw) *P_raw + (self.fk_b12 * P_raw) *(Tr *Tr) + (self.fk_b21 * P_raw) *(P_raw *Tr) + (self.fk_bp3 * P_raw) *(P_raw *P_raw)
            #print("float temp = %f  Pressure = %f" %(self.temperature, self.pressure))

        qmp6988_dict['altitude'] = self.calcAltitude(qmp6988_dict['pressure'], qmp6988_dict['Ctemp'])
        #print("altitude = ",self.altitude)
        return qmp6988_dict

    def getTemperature(self):
        return self.getData()['Ctemp']

    def getTemperatureF(self):
        return self.getData()['Ftemp']

    def getPressure(self):
        return self.getData()['pressure']

    def getAltitude(self):
        return self.getData()['altitude']

    def init(self):
        ret = self.deviceCheck()
        if (ret != 0):
            return 0
        else:
            pass
        self.softwareReset()
        self.getCalibrationData()
        self.setPowermode(QMP6988_NORMAL_MODE)
        self.setFilter(QMP6988_FILTERCOEFF_OFF)
        self.setOversamplingP(QMP6988_OVERSAMPLING_2X)
        self.setOversamplingT(QMP6988_OVERSAMPLING_1X)

if __name__ == "__main__":
    '''
    The below i2c configuration is needed in your board.json.
    "qmp6988": {
        "type": "I2C",
        "port": 1,
        "addrWidth": 7,
        "freq": 100000,
        "mode": "master",
        "devAddr": 86
    }
    '''
    print("Testing qmp6988 ...")

    i2cDev = I2C()
    i2cDev.open("qmp6988")

    baroDev = QMC6988(i2cDev)

    pressure = baroDev.getPressure()
    print("pressure:%f" % (pressure))
    i2cDev.close()

    del baroDev
    i2cDev.close()
    print("Test qmp6988 done!")
