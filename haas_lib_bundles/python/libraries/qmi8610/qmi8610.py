"""
Copyright (C) 2015-2021 Alibaba Group Holding Limited

    MicroPython's driver for QMI8610

    Author: HaaS
    Date:   2021/09/14
"""

from micropython import const
from driver import I2C
from utime import sleep_ms
import math

M_PI = (3.14159265358979323846)
ONE_G = (9.80665)

FISIMU_STATUS1_CMD_DONE     = const(0x01)
FISIMU_STATUS1_WAKEUP_EVENT = const(0x04)
FISIMU_CTRL7_DISABLE_ALL    = const(0x0)
FISIMU_CTRL7_ACC_ENABLE     = const(0x1)
FISIMU_CTRL7_GYR_ENABLE     = const(0x2)
FISIMU_CTRL7_MAG_ENABLE     = const(0x4)
FISIMU_CTRL7_AE_ENABLE      = const(0x8)
FISIMU_CTRL7_ENABLE_MASK    = const(0xF)

FISIMU_CONFIG_ACC_ENABLE      = FISIMU_CTRL7_ACC_ENABLE
FISIMU_CONFIG_GYR_ENABLE      = FISIMU_CTRL7_GYR_ENABLE
FISIMU_CONFIG_MAG_ENABLE      = FISIMU_CTRL7_MAG_ENABLE
FISIMU_CONFIG_AE_ENABLE       = FISIMU_CTRL7_AE_ENABLE
FISIMU_CONFIG_ACCGYR_ENABLE   = (FISIMU_CONFIG_ACC_ENABLE | FISIMU_CONFIG_GYR_ENABLE)
FISIMU_CONFIG_ACCGYRMAG_ENABLE = (FISIMU_CONFIG_ACC_ENABLE | FISIMU_CONFIG_GYR_ENABLE | FISIMU_CONFIG_MAG_ENABLE)
FISIMU_CONFIG_AEMAG_ENABLE = (FISIMU_CONFIG_AE_ENABLE | FISIMU_CONFIG_MAG_ENABLE)

FisRegister_WhoAmI = const(0)  # FIS device identifier register.
FisRegister_Revision = const(1) # FIS hardware revision register.
FisRegister_Ctrl1 = const(2)   # General and power management modes.
FisRegister_Ctrl2 = const(3)   # Accelerometer control. *
FisRegister_Ctrl3 = const(4)   # Gyroscope control.
FisRegister_Ctrl4 = const(5)   # Magnetometer control.
FisRegister_Ctrl5 = const(6)   # Data processing settings.
FisRegister_Ctrl6 = const(7)   # AttitudeEngine control.
FisRegister_Ctrl7 = const(8)   # Sensor enabled status.
FisRegister_Ctrl8 = const(9)   # Reserved - do not write.
FisRegister_Ctrl9 = const(10)  # Host command register.
FisRegister_Cal1_L = const(11) # Calibration register 1 least significant byte.
FisRegister_Cal1_H = const(12) # Calibration register 1 most significant byte.
FisRegister_Cal2_L = const(13) # Calibration register 2 least significant byte.
FisRegister_Cal2_H = const(14) # Calibration register 2 most significant byte.
FisRegister_Cal3_L = const(15) # Calibration register 3 least significant byte.
FisRegister_Cal3_H = const(16) # Calibration register 3 most significant byte.
FisRegister_Cal4_L = const(17) # Calibration register 4 least significant byte.
FisRegister_Cal4_H = const(18) # Calibration register 4 most significant byte.
FisRegister_FifoCtrl = const(19) # FIFO control register.
FisRegister_FifoData = const(20) # FIFO data register.
FisRegister_FifoStatus = const(21) # FIFO status register.
FisRegister_Status0 = const(22) # Output data overrun and availability.
FisRegister_Status1 = const(23) # Miscellaneous status register.
FisRegister_CountOut = const(24) # Sample counter.
FisRegister_Ax_L = const(25) # Accelerometer X axis least significant byte.
FisRegister_Ax_H = const(26) # Accelerometer X axis most significant byte.
FisRegister_Ay_L = const(27) # Accelerometer Y axis least significant byte.
FisRegister_Ay_H = const(28) # Accelerometer Y axis most significant byte.
FisRegister_Az_L = const(29) # Accelerometer Z axis least significant byte.
FisRegister_Az_H = const(30) # Accelerometer Z axis most significant byte.
FisRegister_Gx_L = const(31) # Gyroscope X axis least significant byte.
FisRegister_Gx_H = const(32) # Gyroscope X axis most significant byte.
FisRegister_Gy_L = const(33) # Gyroscope Y axis least significant byte.
FisRegister_Gy_H = const(34) # Gyroscope Y axis most significant byte.
FisRegister_Gz_L = const(35) # Gyroscope Z axis least significant byte.
FisRegister_Gz_H = const(36) # Gyroscope Z axis most significant byte.
FisRegister_Mx_L = const(37) # Magnetometer X axis least significant byte.
FisRegister_Mx_H = const(38) # Magnetometer X axis most significant byte.
FisRegister_My_L = const(39) # Magnetometer Y axis least significant byte.
FisRegister_My_H = const(40) # Magnetometer Y axis most significant byte.
FisRegister_Mz_L = const(41) # Magnetometer Z axis least significant byte.
FisRegister_Mz_H = const(42) # Magnetometer Z axis most significant byte.
FisRegister_Q1_L = const(43) # Quaternion increment W least significant byte.
FisRegister_Q1_H = const(44) # Quaternion increment W most significant byte.
FisRegister_Q2_L = const(45) # Quaternion increment X least significant byte.
FisRegister_Q2_H = const(46) # Quaternion increment X most significant byte.
FisRegister_Q3_L = const(47) # Quaternion increment Y least significant byte.
FisRegister_Q3_H = const(48) # Quaternion increment Y most significant byte.
FisRegister_Q4_L = const(49) # Quaternion increment Z least significant byte.
FisRegister_Q4_H = const(50) # Quaternion increment Z most significant byte.
FisRegister_Dvx_L = const(51) # Velocity increment X least significant byte.
FisRegister_Dvx_H = const(52) # Velocity increment X most significant byte.
FisRegister_Dvy_L = const(53) # Velocity increment Y least significant byte.
FisRegister_Dvy_H = const(54) # Velocity increment Y most significant byte.
FisRegister_Dvz_L = const(55) # Velocity increment Z least significant byte.
FisRegister_Dvz_H = const(56) # Velocity increment Z most significant byte.
FisRegister_Temperature = const(57) # Temperature output.
FisRegister_AeClipping = const(58) # AttitudeEngine clipping flags.
FisRegister_AeOverflow = const(59) # AttitudeEngine overflow flags.

Ctrl9_Nop = const(0) # No operation.
Ctrl9_ResetFifo = const(0x2) # Reset FIFO.
Ctrl9_SetMagXCalibration = const(0x6)  # Set magnetometer X calibration values.
Ctrl9_SetMagYCalibration = const(0x7) # Set magnetometer Y calibration values.
Ctrl9_SetMagZCalibration = const(0x8) # Set magnetometer Z calibration values.
Ctrl9_SetAccelOffset = const(0x12) # Set accelerometer offset correction value.
Ctrl9_SetGyroOffset = const(0x13) # Set gyroscope offset correction value.
Ctrl9_SetAccelSensitivity = const(0x14) # Set accelerometer sensitivity.
Ctrl9_SetGyroSensitivity = const(0x15) # Set gyroscope sensitivity.
Ctrl9_UpdateMagBias = const(0xB) # Update magnemoter bias compensation.
Ctrl9_TriggerMotionOnDemand = const(0x0c) # Trigger motion on demand sample.
Ctrl9_UpdateAttitudeEngineGyroBias = const(0xE) # Update gyroscope bias compensation.
Ctrl9_ReadTrimmedFrequencyValue = const(0x18) # Read frequency correction value.
Ctrl9_ReadFifo = const(0x0D) # Prepare for FIFO read sequence.
Ctrl9_ConfigureWakeOnMotion = const(0x19) # Set wake on motion parameters.

Lpf_Disable = const(0) # Disable low pass filter.
Lpf_Enable  = const(1) # Enable low pass filter.

Hpf_Disable = const(0) # Disable high pass filter.
Hpf_Enable  = const(1) # Enable high pass filter.

AccRange_2g = const(0 << 3)  # +/- 2g range
AccRange_4g = const(1 << 3)  # +/- 4g range
AccRange_8g = const(2 << 3)  # +/- 8g range
AccRange_16g = const(3 << 3) # +/- 16g range

AccOdr_1024Hz = const(0)         # High resolution 1024Hz output rate.
AccOdr_256Hz = const(1)          # High resolution 256Hz output rate.
AccOdr_128Hz = const(2)          # High resolution 128Hz output rate.
AccOdr_32Hz = const(3)           # High resolution 32Hz output rate.
AccOdr_LowPower_128Hz = const(4) # Low power 128Hz output rate.
AccOdr_LowPower_64Hz = const(5)  # Low power 64Hz output rate.
AccOdr_LowPower_25Hz = const(6)  # Low power 25Hz output rate.
AccOdr_LowPower_3Hz = const(7)   # Low power 3Hz output rate.

GyrRange_32dps = const(0 << 3)   # +-32 degrees per second.
GyrRange_64dps = const(1 << 3)   # +-64 degrees per second.
GyrRange_128dps = const(2 << 3)  # +-128 degrees per second.
GyrRange_256dps = const(3 << 3)  # +-256 degrees per second.
GyrRange_512dps = const(4 << 3)  # +-512 degrees per second.
GyrRange_1024dps = const(5 << 3) # +-1024 degrees per second.
GyrRange_2048dps = const(6 << 3) # +-2048 degrees per second.
GyrRange_2560dps = const(7 << 3) # +-2560 degrees per second.

"""
Gyroscope output rate configuration.
"""
GyrOdr_1024Hz           = const(0)   # High resolution 1024Hz output rate.
GyrOdr_256Hz            = const(1)   # High resolution 256Hz output rate.
GyrOdr_128Hz            = const(2)   # High resolution 128Hz output rate.
GyrOdr_32Hz             = const(3)   # High resolution 32Hz output rate.
GyrOdr_OIS_8192Hz       = const(6)   # OIS Mode 8192Hz output rate.
GyrOdr_OIS_LL_8192Hz    = const(7)   # OIS LL Mode 8192Hz output rate.

AeOdr_1Hz = const(0)  # 1Hz output rate.
AeOdr_2Hz = const(1)  # 2Hz output rate.
AeOdr_4Hz = const(2)  # 4Hz output rate.
AeOdr_8Hz = const(3)  # 8Hz output rate.
AeOdr_16Hz = const(4) # 16Hz output rate.
AeOdr_32Hz = const(5) # 32Hz output rate.
AeOdr_64Hz = const(6) # 64Hz output rate.
"""
    * Motion on demand mode.
    *
    * In motion on demand mode the application can trigger AttitudeEngine
    * output samples as necessary. This allows the AttitudeEngine to be
    * synchronized with external data sources.
    *
    * When in Motion on Demand mode the application should request new data
    * by calling the FisImu_requestAttitudeEngineData() function. The
    * AttitudeEngine will respond with a data ready event (INT2) when the
    * data is available to be read.
"""
AeOdr_motionOnDemand = const(128) # 128Hz output rate.

MagOdr_32Hz = const(2) # 32Hz output rate.

MagDev_AK8975 = const(0 << 4) # AKM AK8975.
MagDev_AK8963 = const(1 << 4) # AKM AK8963.

AccUnit_g   = const(0) # Accelerometer output in terms of g (9.81m/s^2).
AccUnit_ms2 = const(1) # Accelerometer output in terms of m/s^2.

GyrUnit_dps  = const(0) # Gyroscope output in degrees/s.
GyrUnit_rads = const(1) # Gyroscope output in rad/s.

AXIS_X = const(0)
AXIS_Y = const(1)
AXIS_Z = const(2)
AXIS_TOTAL = const(4)

# FIS INT1 line.
Fis_Int1 = const(0 << 6)
# FIS INT2 line.
Fis_Int2 = const(1 << 6)

InterruptInitialState_high = const(1 << 7) # Interrupt high.
InterruptInitialState_low  = const(0 << 7) # Interrupt low.

WomThreshold_high = const(128) # High threshold - large motion needed to wake.
WomThreshold_low  = const(32)  # Low threshold - small motion needed to wake.

acc_lsb_div = 0
gyro_lsb_div = 0

qmi8610_dict = {'temp': 0.0, 'gyroX': 0.0, 'gyroY': 0.0, 'gyroZ': 0.0, 'accX': 0.0, 'accY': 0.0, 'accZ': 0.0}


class QMI8610(object):
    """
    This class implements QMI8610 chip's defs.
    """

    def __init__(self, i2cDev):
        self._i2cDev = None
        if not isinstance(i2cDev, I2C):
            raise ValueError("parameter is not an I2C object")
        # make QMI8610's internal object points to i2cDev
        self._i2cDev = i2cDev
        # 初始化QMI8610传感器
        r = self.init()
        if r == 0:
            raise ValueError("QMI8610 init error")

    def int16(self, dat):
        if dat > (1 << 15):
            return dat - (1 << 16)
        else:
            return dat

    def int32(self, dat):
        if dat > (1 << 31):
            return dat - (1 << 32)
        else:
            return dat

    #写寄存器
    def writeReg(self, addr, value):
            Reg = bytearray([addr, value])
            self._i2cDev.write(Reg)
            #print("--> write addr " + str(addr) + ", value = " + str(value))

    #写多个寄存器
    def writeRegs(self, addr, value, len):
            Regs = bytearray(value)
            if (len != Regs.count):
                return "Error code:%d, Error message: %s" % (self, str(self.msg))
            self.writeReg(addr,Regs)
            #print("--> write addr " + str(addr) + ", value = " + str(Regs))

   #读寄存器
    def readReg(self, addr, len):
        reg = bytearray([addr])
        data = bytearray(len)

        self._i2cDev.memRead(data, addr, 8)
        #print("--> read " + str(len) + " bytes from addr " + str(addr) + ", " + str(len) + " bytes value = " + str(data))
        return data

    # 设置layout
    def setLayout(self, layout):
        sign = [1,2,3]
        map = [1,2,3]
        if (layout == 0):
            sign[AXIS_X] = 1
            sign[AXIS_Y] = 1
            sign[AXIS_Z] = 1
            map[AXIS_X] = AXIS_X
            map[AXIS_Y] = AXIS_Y
            map[AXIS_Z] = AXIS_Z
        elif (layout == 1):
            sign[AXIS_X] = -1
            sign[AXIS_Y] = 1
            sign[AXIS_Z] = 1
            map[AXIS_X] = AXIS_Y
            map[AXIS_Y] = AXIS_X
            map[AXIS_Z] = AXIS_Z
        elif (layout == 2):
            sign[AXIS_X] = -1
            sign[AXIS_Y] = -1
            sign[AXIS_Z] = 1
            map[AXIS_X] = AXIS_X
            map[AXIS_Y] = AXIS_Y
            map[AXIS_Z] = AXIS_Z
        elif (layout == 3):
            sign[AXIS_X] = 1
            sign[AXIS_Y] = -1
            sign[AXIS_Z] = 1
            map[AXIS_X] = AXIS_Y
            map[AXIS_Y] = AXIS_X
            map[AXIS_Z] = AXIS_Z
        elif (layout == 4):
            sign[AXIS_X] = -1
            sign[AXIS_Y] = 1
            sign[AXIS_Z] = -1
            map[AXIS_X] = AXIS_X
            map[AXIS_Y] = AXIS_Y
            map[AXIS_Z] = AXIS_Z
        elif (layout == 5):
            sign[AXIS_X] = 1
            sign[AXIS_Y] = 1
            sign[AXIS_Z] = -1
            map[AXIS_X] = AXIS_Y
            map[AXIS_Y] = AXIS_X
            map[AXIS_Z] = AXIS_Z
        elif (layout == 6):
            sign[AXIS_X] = 1
            sign[AXIS_Y] = -1
            sign[AXIS_Z] = -1
            map[AXIS_X] = AXIS_X
            map[AXIS_Y] = AXIS_Y
            map[AXIS_Z] = AXIS_Z
        elif (layout == 7):
            sign[AXIS_X] = 1
            sign[AXIS_Y] = 1
            sign[AXIS_Z] = 1
            map[AXIS_X] = AXIS_X
            map[AXIS_Y] = AXIS_Y
            map[AXIS_Z] = AXIS_Z

    def configACC(self, range, odr, lpfEnable, hpfEnable):
        ctl_dada = 0
        range_set = 0
        global acc_lsb_div

        if (range == AccRange_2g):
            range_set = 0 << 3
            acc_lsb_div = (1 << 14)
        elif (range == AccRange_4g):
            range_set = 1 << 3
            acc_lsb_div = (1 << 13)
        elif (range == AccRange_8g):
            range_set = 2 << 3
            acc_lsb_div = (1 << 12)
        else:
            range_set = 2 << 3
            acc_lsb_div = (1 << 12)

        ctl_dada = range_set | odr
        self.writeReg(FisRegister_Ctrl2, ctl_dada)
    # set LPF & HPF
        ctl_dada = self.readReg(FisRegister_Ctrl5, 1)[0]
        ctl_dada &= 0xfc
        if (lpfEnable == Lpf_Enable):
            ctl_dada |= 0x02
        if (hpfEnable == Hpf_Enable):
            ctl_dada |= 0x01

        self.writeReg(FisRegister_Ctrl5, ctl_dada)

    def configGyro(self, range, odr, lpfEnable, hpfEnable):
        #  Set the CTRL3 register to configure dynamic range and ODR
        global gyro_lsb_div
        ctl_dada = range | odr
        self.writeReg(FisRegister_Ctrl3, ctl_dada)

        # Store the scale factor for use when processing raw data
        if (range == GyrRange_32dps):
            gyro_lsb_div = 1024
        elif (range == GyrRange_64dps):
            gyro_lsb_div = 512
        elif (range == GyrRange_128dps):
            gyro_lsb_div = 256
        elif (range == GyrRange_256dps):
            gyro_lsb_div = 128
        elif (range == GyrRange_512dps):
            gyro_lsb_div = 64
        elif (range == GyrRange_1024dps):
            gyro_lsb_div = 32
        elif (range == GyrRange_2048dps):
            gyro_lsb_div = 16
        elif (range == GyrRange_2560dps):
            #gyro_lsb_div = 8
            pass
        else:
             gyro_lsb_div = 32

        #  Conversion from degrees/s to rad/s if necessary
        #  set LPF & HPF
        ctl_dada = self.readReg(FisRegister_Ctrl5, 1)[0]
        ctl_dada &= 0xf3

        if (lpfEnable == Lpf_Enable):
            ctl_dada |= 0x08
        if (hpfEnable == Hpf_Enable):
            ctl_dada |= 0x04
        self.writeReg(FisRegister_Ctrl5, ctl_dada)

    def configAe(self, odr):
        # Configure Accelerometer and Gyroscope settings
        self.configACC(AccRange_8g, AccOdr_1024Hz, Lpf_Enable, Hpf_Disable)
        self.configGyro(GyrRange_2048dps, GyrOdr_1024Hz, Lpf_Enable, Hpf_Disable)
        self.writeReg(FisRegister_Ctrl5, odr)

    def readStatus0(self):
        status = self.readReg(FisRegister_Status0, 1)[0]
        return status

    def readStatus1(self):
        status = self.readReg(FisRegister_Status1, 1)[0]
        return status

    # 读取xyz的值
    def readXyz(self):
        buf_reg = bytearray(12)
        raw_acc_xyz = [1, 2, 3]
        raw_gyro_xyz = [1, 2, 3]
        xyz_t = [1, 2, 3, 4, 5, 6]

        buf_reg = self.readReg(FisRegister_Ax_L | 0x80, 12)

        raw_acc_xyz[0] = self.int16((buf_reg[1] << 8) | (buf_reg[0]))
        raw_acc_xyz[1] = self.int16((buf_reg[3] << 8) | (buf_reg[2]))
        raw_acc_xyz[2] = self.int16((buf_reg[5] << 8) | (buf_reg[4]))

        raw_gyro_xyz[0] = self.int16((buf_reg[7] << 8) | (buf_reg[6]))
        raw_gyro_xyz[1] = self.int16((buf_reg[9] << 8) | (buf_reg[8]))
        raw_gyro_xyz[2] = self.int16((buf_reg[11] << 8) | (buf_reg[10]))

    # m/s2
        xyz_t[0] = (raw_acc_xyz[0] * ONE_G) / acc_lsb_div
        xyz_t[1] = (raw_acc_xyz[1] * ONE_G) / acc_lsb_div
        xyz_t[2] = (raw_acc_xyz[2] * ONE_G) / acc_lsb_div

        xyz_t[0] = -xyz_t[0]
        xyz_t[1] = -xyz_t[1]
        xyz_t[2] = -xyz_t[2]
    #  rad/s
        xyz_t[3] = (raw_gyro_xyz[0] * M_PI / 180) / gyro_lsb_div
        xyz_t[4] = (raw_gyro_xyz[1] * M_PI / 180) / gyro_lsb_div
        xyz_t[5] = (raw_gyro_xyz[2] * M_PI / 180) / gyro_lsb_div

        xyz_t[3] = xyz_t[3]
        xyz_t[4] = xyz_t[4]
        xyz_t[5] = -xyz_t[5]

        return xyz_t

    def applyScaleFactor(self, scaleFactor, nElements, rawData, calibratedData):
        for i in range(nElements):
            calibratedData[i] = (scaleFactor * rawData[2 * i]) | (rawData[2 * i + 1] << 8)

    def processAccelerometerData(self, rawData, calibratedData):
        self.applyScaleFactor(ONE_G/acc_lsb_div, 3, rawData, calibratedData)

    def processGyroscopeData(self, rawData, calibratedData):
        self.applyScaleFactor(M_PI / (gyro_lsb_div * 180), 3, rawData, calibratedData)

    def readRawsample(self, rawData, calibratedData):
        self.applyScaleFactor(M_PI / (gyro_lsb_div * 180), 3, rawData, calibratedData)

    def writeCalibrationVectorBuffer(self, calVector, conversionFactor, fractionalBits):
        o = 0
        calCmd = bytearray(6)

        for i in range(3):
            o = round(calVector[i] * conversionFactor * (1 << fractionalBits))
            calCmd[(2 * i)] = o & 0xFF
            calCmd[(2 * i) + 1] = o >> 8
        self.writeRegs(FisRegister_Cal1_L, calCmd, 6)

    def doCtrl9Command(self, cmd):
        gyroConfig = 0
        oisModeBits = const(0x06)
        oisEnabled = 0
        status = 0
        count = 0

        gyroConfig = self.readReg(FisRegister_Ctrl3, 1)[0]
        oisEnabled = ((gyroConfig & oisModeBits) == oisModeBits)

        if (oisEnabled):
            self.writeReg(FisRegister_Ctrl3, (gyroConfig & ~oisModeBits))

        self.writeReg(FisRegister_Ctrl9, cmd)

        # Check that command has been executed
        while (((status & FISIMU_STATUS1_CMD_DONE) == 0) and (count < 10000)):
            status = self.readReg(FisRegister_Status1, 1)[0]
            count += 1

        if (oisEnabled):
        # Re-enable OIS mode configuration if necessary
            self.writeReg(FisRegister_Ctrl3, gyroConfig)

    def applyAccelerometerOffset(self, offset, unit):
        if (unit == AccUnit_ms2):
            conversionFactor = 1 / ONE_G
        else:
            conversionFactor = 1
        self.writeCalibrationVectorBuffer(offset, conversionFactor, 11)
        self.doCtrl9Command(Ctrl9_SetAccelOffset)

    def applyGyroscopeOffset(self, offset, unit):
        if (unit == GyrUnit_rads):
            conversionFactor = 180 / M_PI
        else:
            conversionFactor = 1
        self.writeCalibrationVectorBuffer(offset, conversionFactor, 6)
        self.doCtrl9Command(Ctrl9_SetGyroOffset)

    def applyOffsetCalibration(self, accUnit, accOffset, gyrUnit, gyrOffset):
        self.applyAccelerometerOffset(accOffset, accUnit)
        self.applyGyroscopeOffset(gyrOffset, gyrUnit)

# for XKF3
    def enableWakeOnMotion(self):
        womCmd = bytearray[3]
        interrupt = Fis_Int1
        initialState = InterruptInitialState_low
        threshold = WomThreshold_low
        blankingTime = 0x00
        blankingTimeMask = 0x3F

        self.enableSensors(FISIMU_CTRL7_DISABLE_ALL)
        self.configACC(AccRange_2g, AccOdr_LowPower_25Hz, Lpf_Disable, Hpf_Disable)

        womCmd[0] = FisRegister_Cal1_L # WoM Threshold: absolute value in mg (with 1mg/LSB resolution)
        womCmd[1] = threshold
        womCmd[2] = interrupt | initialState | (blankingTime & blankingTimeMask)
        self.writeReg(FisRegister_Cal1_L, womCmd[1])
        self.writeReg(FisRegister_Cal1_H, womCmd[2])

        self.doCtrl9Command(Ctrl9_ConfigureWakeOnMotion)
        self.enableSensors(FISIMU_CTRL7_ACC_ENABLE)

    def disableWakeOnMotion(self):
        self.enableSensors(FISIMU_CTRL7_DISABLE_ALL)
        self.writeReg(FisRegister_Cal1_L, 0)
        self.doCtrl9Command(Ctrl9_ConfigureWakeOnMotion)

    def enableSensors(self, enableFlags):
        if (enableFlags & FISIMU_CONFIG_AE_ENABLE):
            enableFlags |= FISIMU_CTRL7_ACC_ENABLE | FISIMU_CTRL7_GYR_ENABLE
        self.writeReg(FisRegister_Ctrl7, enableFlags & FISIMU_CTRL7_ENABLE_MASK)

    def configMAG(self, device, odr):
        pass

    def configApply(self, inputSelection, accRange, accOdr, gyrRange, gyrOdr, aeOdr, magOdr, magDev):
        fisSensors = inputSelection
        if (fisSensors & FISIMU_CONFIG_AE_ENABLE):
            self.configAe(aeOdr)
        else:
            if (inputSelection & FISIMU_CONFIG_ACC_ENABLE):
                self.configACC(accRange, accOdr, Lpf_Enable, Hpf_Disable)
            if (inputSelection & FISIMU_CONFIG_GYR_ENABLE):
                self.configGyro(gyrRange, gyrOdr, Lpf_Enable, Hpf_Disable)

        if (inputSelection & FISIMU_CONFIG_MAG_ENABLE):
            self.configMAG(magDev, magOdr)

        self.enableSensors(fisSensors)

    # 得到温度值
    # 返回值:温度值
    def getTemperature(self):
        temp = self.readReg(FisRegister_Temperature, 1)[0]
        return round(temp, 2)

    # 得到加速度值(原始值)
    # (gx,gy,gz):陀螺仪x,y,z轴的重力加速度，单位：m/s²
    # 返回值:0,成功
    # 其他,错误代码
    def getAcceleration(self):
        global acc_lsb_div
        buf_reg = bytearray(6)
        raw_acc_xyz = [1, 2, 3]
        acc_xyz = [1, 2, 3]

        buf_reg[0] = self.readReg(FisRegister_Ax_L, 1)[0]
        buf_reg[1] = self.readReg(FisRegister_Ax_H, 1)[0]
        buf_reg[2] = self.readReg(FisRegister_Ay_L, 1)[0]
        buf_reg[3] = self.readReg(FisRegister_Ay_H, 1)[0]
        buf_reg[4] = self.readReg(FisRegister_Az_L, 1)[0]
        buf_reg[5] = self.readReg(FisRegister_Az_H, 1)[0]

        raw_acc_xyz[0] = self.int16((buf_reg[1] << 8) | (buf_reg[0]))
        raw_acc_xyz[1] = self.int16((buf_reg[3] << 8) | (buf_reg[2]))
        raw_acc_xyz[2] = self.int16((buf_reg[5] << 8) | (buf_reg[4]))

        print("raw acc is acc0[%d] acc1[%d] acc2[%d]" %(raw_acc_xyz[0], raw_acc_xyz[1], raw_acc_xyz[2]))

        acc_xyz[0] = (raw_acc_xyz[0] * ONE_G) / acc_lsb_div
        acc_xyz[1] = (raw_acc_xyz[1] * ONE_G) / acc_lsb_div
        acc_xyz[2] = (raw_acc_xyz[2] * ONE_G) / acc_lsb_div
        print("fis210x acc is", acc_xyz[0], acc_xyz[1], acc_xyz[2])
        return acc_xyz

    # 得到陀螺仪值(原始值)
    # gx,gy,gz:陀螺仪x,y,z轴的原始读数(带符号)
    # 返回值:0,成功
    # 其他,错误代码
    def getGyro(self):
        global gyro_lsb_div
        buf_reg = bytearray(6)
        raw_gyro_xyz = [1, 2, 3]
        gyro_xyz = [1, 2, 3]
        #buf_reg = self.readReg(FisRegister_Gx_L, 6)

        buf_reg[0] = self.readReg(FisRegister_Gx_L, 1)[0]
        buf_reg[1] = self.readReg(FisRegister_Gx_H, 1)[0]
        buf_reg[2] = self.readReg(FisRegister_Gy_L, 1)[0]
        buf_reg[3] = self.readReg(FisRegister_Gy_H, 1)[0]
        buf_reg[4] = self.readReg(FisRegister_Gz_L, 1)[0]
        buf_reg[5] = self.readReg(FisRegister_Gz_H, 1)[0]

        raw_gyro_xyz[0] = self.int16((buf_reg[1] << 8) | (buf_reg[0]))
        raw_gyro_xyz[1] = self.int16((buf_reg[3] << 8) | (buf_reg[2]))
        raw_gyro_xyz[2] = self.int16((buf_reg[5] << 8) | (buf_reg[4]))

        #print("raw gyro is g0[%d] g1[%d g2[%d]" %(raw_gyro_xyz[0], raw_gyro_xyz[1], raw_gyro_xyz[2]))

        gyro_xyz[0] = (raw_gyro_xyz[0] * 1.0) / gyro_lsb_div
        gyro_xyz[1] = (raw_gyro_xyz[1] * 1.0) / gyro_lsb_div
        gyro_xyz[2] = (raw_gyro_xyz[2] * 1.0) / gyro_lsb_div

        #print("fis210x gyro is", gyro_xyz[0], gyro_xyz[1], gyro_xyz[2])
        return gyro_xyz

    def getData(self):
        global qmi8610_dict
        qmi8610_dict['temp'] = self.getTemperature()
        arr = self.getGyro()
        qmi8610_dict['gyroX'] = arr[0]
        qmi8610_dict['gyroY'] = arr[1]
        qmi8610_dict['gyroZ'] = arr[2]
        brr = self.getAcceleration()
        qmi8610_dict['accX'] = brr[0]
        qmi8610_dict['accY'] = brr[1]
        qmi8610_dict['accZ'] = brr[2]
        return qmi8610_dict

    def init(self):

        chip_id = 0x00

        chip_id = self.readReg(FisRegister_WhoAmI, 1)[0]
        sleep_ms(100)
        if (chip_id == 0xfc):
            inputSelection = FISIMU_CONFIG_ACCGYR_ENABLE
            accRange = AccRange_4g
            accOdr = AccOdr_128Hz
            gyrRange = GyrRange_1024dps # GyrRange_1024dps;
            gyrOdr = GyrOdr_256Hz # GyrOdr_1024Hz
            magOdr = MagOdr_32Hz
            magDev = MagDev_AK8963
            aeOdr = AeOdr_32Hz
            sleep_ms(100)

            self.configApply(inputSelection, accRange, accOdr, gyrRange, gyrOdr, aeOdr, magOdr, magDev)
            sleep_ms(100)
            self.setLayout(2)
        else:
            chip_id = 0

        return chip_id
