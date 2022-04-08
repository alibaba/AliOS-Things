"""
Copyright (C) 2015-2021 Alibaba Group Holding Limited

    MicroPython's driver for QMC6310 magnetometer

    Author: HaaS
    Date:   2021/09/09

"""
from driver import I2C
from micropython import const
import utime
import math

# vendor chip id
QMC6308_IIC_ADDR = const(0x2C)
QMC6310U_IIC_ADDR = const(0x1c)
QMC6310N_IIC_ADDR = const(0x3c)
QMC6310_CHIP_ID_REG = const(0x00)

# data output register
QMC6310_DATA_OUT_X_LSB_REG = const(0x01)
QMC6310_DATA_OUT_X_MSB_REG = const(0x02)
QMC6310_DATA_OUT_Y_LSB_REG = const(0x03)
QMC6310_DATA_OUT_Y_MSB_REG = const(0x04)
QMC6310_DATA_OUT_Z_LSB_REG = const(0x05)
QMC6310_DATA_OUT_Z_MSB_REG = const(0x06)

# Status registers
QMC6310_STATUS_REG = const(0x09)

# configuration registers
QMC6310_CTL_REG_ONE = const(0x0A)  # Contrl register one * /
QMC6310_CTL_REG_TWO = const(0x0B)  # Contrl register two * /
QMC6310_CTL_REG_THREE = const(0x0D)  # Contrl register three * /

# Magnetic Sensor Operating Mode MODE[1:0]
QMC6310_SUSPEND_MODE = const(0x00)
QMC6310_NORMAL_MODE = const(0x01)
QMC6310_SINGLE_MODE = const(0x02)
QMC6310_H_PFM_MODE = const(0x03)

# data output rate OSR2[2:0]
OUTPUT_DATA_RATE_800HZ = const(0x00)
OUTPUT_DATA_RATE_400HZ = const(0x01)
OUTPUT_DATA_RATE_200HZ = const(0x02)
OUTPUT_DATA_RATE_100HZ = const(0x03)

# oversample Ratio  OSR[1]
OVERSAMPLE_RATE_256 = const(0x01)
OVERSAMPLE_RATE_128 = const(0x00)

SET_RESET_ON = const(0x00)
SET_ONLY_ON = const(0x01)
SET_RESET_OFF = const(0x02)

QMC6310_DEFAULT_DELAY = const(200)

QMC6310_AXIS_X = const(0x00)
QMC6310_AXIS_Y = const(0x01)
QMC6310_AXIS_Z = const(0x02)
QMC6310_AXIS_TOTAL = const(0x03)

INT16_MIN = const(-32768)
INT16_MAX = const(32767)


class QMC6310():
    def __init__(self, i2cDev):
        self.addr = QMC6310U_IIC_ADDR
        self._i2cDev = None
        if not isinstance(i2cDev, I2C):
            raise ValueError("parameter is not an I2C object")
        # make QMC6310's internal object points to i2cDev
        self._i2cDev = i2cDev
        self.chipId = self.getChipId()
        self.resetCal()
        self.setLayout(0)
        self.enable()

        ctrl_value = self.getReg(QMC6310_CTL_REG_ONE)
        #print('ctrl_value one = %d' % ctrl_value)

        ctrl_value = self.getReg(QMC6310_CTL_REG_TWO)
        #print('ctrl_value two = %d' % ctrl_value)

        ctrl_value = self.getReg(QMC6310_CTL_REG_THREE)
        #print('ctrl_value three = %d' % ctrl_value)

    def int16(self, dat):
        if dat > 32767:
            return dat - 65536
        else:
            return dat

    # set reg
    def setReg(self, reg, dat):
        buf = bytearray(2)
        buf[0] = reg
        buf[1] = dat
        self._i2cDev.write(buf)

    # get reg
    def getReg(self, reg):
        buf = bytearray(1)
        buf[0] = reg
        self._i2cDev.write(buf)
        self._i2cDev.read(buf)
        return buf[0]

    # get mem reg
    def getMemReg(self, reg, len):
        buf = bytearray(1)
        buf[0] = reg
        self._i2cDev.write(buf)
        data = bytearray(len)
        self._i2cDev.read(data)
        return data

    def getChipId(self):
        id = self.getReg(QMC6310_CHIP_ID_REG)
        return id

    def setLayout(self, layout):
        if (layout == 0):
            self.sign_x = 1
            self.sign_y = 1
            self.sign_z = 1
            self.map_x = QMC6310_AXIS_X
            self.map_y = QMC6310_AXIS_Y
            self.map_z = QMC6310_AXIS_Z
        elif (layout == 1):
            self.sign_x = -1
            self.sign_y = 1
            self.sign_z = 1
            self.map_x = QMC6310_AXIS_Y
            self.map_y = QMC6310_AXIS_X
            self.map_z = QMC6310_AXIS_Z
        elif (layout == 2):
            self.sign_x = -1
            self.sign_y = -1
            self.sign_z = 1
            self.map_x = QMC6310_AXIS_X
            self.map_y = QMC6310_AXIS_Y
            self.map_z = QMC6310_AXIS_Z
        elif (layout == 3):
            self.sign_x = 1
            self.sign_y = -1
            self.sign_z = 1
            self.map_x = QMC6310_AXIS_Y
            self.map_y = QMC6310_AXIS_X
            self.map_z = QMC6310_AXIS_Z
        elif (layout == 4):
            self.sign_x = -1
            self.sign_y = 1
            self.sign_z = -1
            self.map_x = QMC6310_AXIS_X
            self.map_y = QMC6310_AXIS_Y
            self.map_z = QMC6310_AXIS_Z
        elif (layout == 5):
            self.sign_x = 1
            self.sign_y = 1
            self.sign_z = -1
            self.map_x = QMC6310_AXIS_Y
            self.map_y = QMC6310_AXIS_X
            self.map_z = QMC6310_AXIS_Z
        elif (layout == 6):
            self.sign_x = 1
            self.sign_y = -1
            self.sign_z = -1
            self.map_x = QMC6310_AXIS_X
            self.map_y = QMC6310_AXIS_Y
            self.map_z = QMC6310_AXIS_Z
        elif (layout == 7):
            self.sign_x = -1
            self.sign_y = -1
            self.sign_z = -1
            self.map_x = QMC6310_AXIS_Y
            self.map_y = QMC6310_AXIS_X
            self.map_z = QMC6310_AXIS_Z
        else:
            self.sign_x = 1
            self.sign_y = 1
            self.sign_z = 1
            self.map_x = QMC6310_AXIS_X
            self.map_y = QMC6310_AXIS_Y
            self.map_z = QMC6310_AXIS_Z

    def enable(self):
        self.setReg(0x0d, 0x40)
        self.setReg(0x29, 0x06)
        self.setReg(0x0a, 0x0F)
        self.setReg(0x0b, 0x00)

    def resetCal(self):
        self.x_max = INT16_MIN
        self.y_max = INT16_MIN
        self.z_max = INT16_MIN
        self.x_min = INT16_MAX
        self.y_min = INT16_MAX
        self.z_min = INT16_MAX

    def getXYZ(self):
        # Check status register for data availability
        rdy = self.getReg(QMC6310_STATUS_REG)
        count = 0

        while not (rdy & 0x01) and (count < 5):
            rdy = self.getReg(QMC6310_STATUS_REG)
            count = count + 1
            utime.sleep(0.5)

        data = self.getMemReg(QMC6310_DATA_OUT_X_LSB_REG, 6)

        t0 = self.int16((data[1] << 8) | data[0])
        t1 = self.int16((data[3] << 8) | data[2])
        t2 = self.int16((data[5] << 8) | data[4])

        hw_d = [t0, t1, t2]

        raw_c0 = self.sign_x * hw_d[self.map_x] / 10.0
        raw_c1 = self.sign_y * hw_d[self.map_y] / 10.0
        raw_c2 = self.sign_z * hw_d[self.map_z] / 10.0

        return [raw_c0, raw_c1, raw_c2]

    def getHeading(self):

        # Check status register for data availability
        rdy = self.getReg(QMC6310_STATUS_REG)
        count = 0

        while not (rdy & 0x01) and (count < 5):
            rdy = self.getReg(QMC6310_STATUS_REG)
            count = count + 1
            utime.sleep(0.5)

        data = self.getMemReg(QMC6310_DATA_OUT_X_LSB_REG, 6)

        x_org = self.int16((data[1] << 8) | data[0])
        y_org = self.int16((data[3] << 8) | data[2])
        z_org = self.int16((data[5] << 8) | data[4])

        self.x_min = min(x_org, self.x_min)
        self.x_max = max(x_org, self.x_max)
        self.y_min = min(y_org, self.y_min)
        self.y_max = max(y_org, self.y_max)
        self.z_min = min(z_org, self.z_min)
        self.z_max = max(z_org, self.z_max)

        # Bail out if not enough data is available.
        if self.x_min == self.x_max or self.y_min == self.y_max or self.z_min == self.z_max:
            return 0.0

        # Recenter the measurement by subtracting the average * /
        x_offset = (self.x_max + self.x_min) / 2.0
        y_offset = (self.y_max + self.y_min) / 2.0
        z_offset = (self.z_max + self.z_min) / 2.0

        x_fit = (x_org - x_offset) * 1000.0 / (self.x_max - self.x_min)
        y_fit = (y_org - y_offset) * 1000.0 / (self.y_max - self.y_min)
        z_fit = (z_org - z_offset) * 1000.0 / (self.z_max - self.z_min)

        heading = 180.0 * math.atan2(x_fit, y_fit) / math.pi
        if heading <= 0:
            heading = heading + 360.0

        return heading

    def setMode(self, mode):
        value = self.getReg(QMC6310_CTL_REG_ONE)
        value = (value & (~0x03)) | mode
        self.setReg(QMC6310_CTL_REG_ONE, value)

    def setRate(self, rate):
        value = self.getReg(QMC6310_CTL_REG_ONE)
        value = (value & (~0xE8)) | (rate << 5)
        self.setReg(QMC6310_CTL_REG_ONE, value)

if __name__ == "__main__":
    '''
    The below i2c configuration is needed in your board.json.
    "qmc6310": {
        "type": "I2C",
        "port": 1,
        "addrWidth": 7,
        "freq": 100000,
        "mode": "master",
        "devAddr": 28
        },
    '''
    print("Testing qmc6310 ...")

    i2cDev = I2C()
    i2cDev.open("qmc6310")

    qmc6310Dev = QMC6310(i2cDev)

    h = qmc6310Dev.getHeading()
    print("The heading data is:", h)

    i2cDev.close()

    del qmc6310Dev
    print("Test qmc6310 done!")
