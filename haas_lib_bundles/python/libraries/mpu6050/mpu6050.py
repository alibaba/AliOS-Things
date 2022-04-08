"""
Copyright (C) 2015-2021 Alibaba Group Holding Limited

    HaaS Python's driver for MPU6050

    Author: HaaS Python Team
    Date:   2022/02/02
"""

from micropython import const
from driver import I2C
from utime import sleep_ms
import math

MPU_SELF_TESTX_REG = const(0X0D)   #自检寄存器X
MPU_SELF_TESTY_REG = const(0X0E)   #自检寄存器Y
MPU_SELF_TESTZ_REG = const(0X0F)   #自检寄存器Z
MPU_SELF_TESTA_REG = const(0X10)   #自检寄存器A
MPU_SAMPLE_RATE_REG = const(0X19)  #采样频率分频器
MPU_CFG_REG         = const(0X1A)  #配置寄存器
MPU_GYRO_CFG_REG    = const(0X1B)  #陀螺仪配置寄存器
MPU_ACCEL_CFG_REG   = const(0X1C)  #加速度计配置寄存器
MPU_MOTION_DET_REG  = const(0X1F)  #运动检测阀值设置寄存器
MPU_FIFO_EN_REG     = const(0X23)  #FIFO使能寄存器
MPU_I2CMST_CTRL_REG = const(0X24)  #IIC主机控制寄存器
MPU_I2CSLV0_ADDR_REG = const(0X25) #IIC从机0器件地址寄存器
MPU_I2CSLV0_REG      = const(0X26) #IIC从机0数据地址寄存器
MPU_I2CSLV0_CTRL_REG = const(0X27) #IIC从机0控制寄存器
MPU_I2CSLV1_ADDR_REG = const(0X28) #IIC从机1器件地址寄存器
MPU_I2CSLV1_REG      = const(0X29) #IIC从机1数据地址寄存器
MPU_I2CSLV1_CTRL_REG = const(0X2A) #IIC从机1控制寄存器
MPU_I2CSLV2_ADDR_REG = const(0X2B) #IIC从机2器件地址寄存器
MPU_I2CSLV2_REG      = const(0X2C) #IIC从机2数据地址寄存器
MPU_I2CSLV2_CTRL_REG = const(0X2D) #IIC从机2控制寄存器
MPU_I2CSLV3_ADDR_REG = const(0X2E) #IIC从机3器件地址寄存器
MPU_I2CSLV3_REG      = const(0X2F) #IC从机3数据地址寄存器
MPU_I2CSLV3_CTRL_REG = const(0X30) #IIC从机3控制寄存器
MPU_I2CSLV4_ADDR_REG = const(0X31) #IIC从机4器件地址寄存器
MPU_I2CSLV4_REG      = const(0X32) #IIC从机4数据地址寄存器
MPU_I2CSLV4_DO_REG   = const(0X33) #IIC从机4写数据寄存器
MPU_I2CSLV4_CTRL_REG = const(0X34) #IIC从机4控制寄存器
MPU_I2CSLV4_DI_REG   = const(0X35) #IIC从机4读数据寄存器

MPU_I2CMST_STA_REG = const(0X36) #IIC主机状态寄存器
MPU_INTBP_CFG_REG  = const(0X37) #中断/旁路设置寄存器
MPU_INT_EN_REG     = const(0X38) #中断使能寄存器
MPU_INT_STA_REG    = const(0X3A) #中断状态寄存器

MPU_ACCEL_XOUTH_REG = const(0X3B) #加速度值,X轴高8位寄存器
MPU_ACCEL_XOUTL_REG = const(0X3C) #速度值,X轴低8位寄存器
MPU_ACCEL_YOUTH_REG = const(0X3D) #加速度值,Y轴高8位寄存器
MPU_ACCEL_YOUTL_REG = const(0X3E) #加速度值,Y轴低8位寄存器
MPU_ACCEL_ZOUTH_REG = const(0X3F) #加速度值,Z轴高8位寄存器
MPU_ACCEL_ZOUTL_REG = const(0X40) #加速度值,Z轴低8位寄存器

MPU_TEMP_OUTH_REG = const(0X41) #温度值高八位寄存器
MPU_TEMP_OUTL_REG = const(0X42) #温度值低8位寄存器

MPU_GYRO_XOUTH_REG = const(0X43) #陀螺仪值,X轴高8位寄存器
MPU_GYRO_XOUTL_REG = const(0X44) #陀螺仪值,X轴低8位寄存器
MPU_GYRO_YOUTH_REG = const(0X45) #陀螺仪值,Y轴高8位寄存器
MPU_GYRO_YOUTL_REG = const(0X46) #陀螺仪值,Y轴低8位寄存器
MPU_GYRO_ZOUTH_REG = const(0X47) #陀螺仪值,Z轴高8位寄存器
MPU_GYRO_ZOUTL_REG = const(0X48) #陀螺仪值,Z轴低8位寄存器

MPU_I2CSLV0_DO_REG = const(0X63) #IIC从机0数据寄存器
MPU_I2CSLV1_DO_REG = const(0X64) #IIC从机1数据寄存器
MPU_I2CSLV2_DO_REG = const(0X65) #IIC从机2数据寄存器
MPU_I2CSLV3_DO_REG = const(0X66) #IIC从机3数据寄存器

MPU_I2CMST_DELAY_REG = const(0X67) #IIC主机延时管理寄存器
MPU_SIGPATH_RST_REG  = const(0X68) #信号通道复位寄存器
MPU_MDETECT_CTRL_REG = const(0X69) #运动检测控制寄存器
MPU_USER_CTRL_REG    = const(0X6A) #用户控制寄存器
MPU_PWR_MGMT1_REG    = const(0X6B) #电源管理寄存器1
MPU_PWR_MGMT2_REG    = const(0X6C) #电源管理寄存器2
MPU_FIFO_CNTH_REG    = const(0X72) #FIFO计数寄存器高八位
MPU_FIFO_CNTL_REG    = const(0X73) #FIFO计数寄存器低八位
MPU_FIFO_RW_REG      = const(0X74) #FIFO读写寄存器
MPU_DEVICE_ID_REG    = const(0X75) #器件ID寄存器

MPU_DEV_ID   = const(0x68)
mpu6050_dict = {'temp': 0.0, 'gyroX': 0, 'gyroY': 0, 'gyroZ': 0, 'accX': 0, 'accY': 0, 'accZ': 0}
class MPU6050Error(Exception):
    def __init__(self, value=0, msg="mpu6050 common error"):
        self.value = value
        self.msg = msg

    def __str__(self):
        return "Error code:%d, Error message: %s" % (self.value, str(self.msg))

    __repr__ = __str__

class MPU6050(object):
    """
    This class implements mpu6050 chip's defs.
    """
    def __init__(self, i2cDev):
        self._i2cDev = None
        if not isinstance(i2cDev, I2C):
            raise ValueError("parameter is not an I2C object")
        # make MPU6050's internal object points to i2cDev
        self._i2cDev = i2cDev
        # 初始化MPU6050传感器
        r = self.init()
        if r != 0:
            raise ValueError("MPU6050 init error")


    def i2c_write_byte(self, addr, value):
            Reg = bytearray([addr, value])
            self._i2cDev.write(Reg)
            #print("--> write addr " + str(addr) + ", value = " + str(value))

    def i2c_read_byte(self, addr):
            Reg = bytearray([addr])
            self._i2cDev.write(Reg)
            tmp = bytearray(1)
            self._i2cDev.read(tmp)
            #print("<-- read addr " + str(addr) + ", value = " + str(tmp[0]))
            return tmp[0]

    def i2c_read_len(self, addr, len):
        reg = bytearray([addr])
        data = bytearray(len)
        self._i2cDev.write(reg)
        sleep_ms(20)
        self._i2cDev.read(data)
        # print("--> read " + str(len) + " bytes from addr " + str(addr) + ", " + str(len) + " bytes value = " + str(data))
        return data

    # 设置MPU6050陀螺仪传感器满量程范围
    # fsr:0,±250dps;1,±500dps;2,±1000dps;3,±2000dps
    # 返回值:0,设置成功
    # 其他,设置失败
    def setGyroFsr(self, fsr):
        return self.i2c_write_byte(MPU_GYRO_CFG_REG, fsr << 3) # 设置陀螺仪满量程范围

    # 设置MPU6050加速度传感器满量程范围
    # fsr:0,±2g;1,±4g;2,±8g;3,±16g
    # 返回值:0,设置成功
    # 其他,设置失败
    def setAccelFsr(self, fsr):
        return self.i2c_write_byte(MPU_ACCEL_CFG_REG, fsr << 3) # 设置加速度传感器满量程范围

    # 设置MPU6050的数字低通滤波器
    # lpf:数字低通滤波频率(Hz)
    # 返回值:0,设置成功
    # 其他,设置失败
    def setLPF(self, lpf):
        if (lpf >= 188):
            data = 1
        elif (lpf >= 98):
            data = 2
        elif (lpf >= 42):
            data = 3
        elif (lpf >= 20):
            data = 4
        elif (lpf >= 10):
            data = 5
        else:
            data = 6

        return self.i2c_write_byte(MPU_CFG_REG, data) # 设置数字低通滤波器

    # 设置MPU6050的采样率(假定Fs=1KHz)
    # rate:4~1000(Hz)
    # 返回值:0,设置成功
    # 其他,设置失败
    def setRate(self, rate):
        if (rate > 1000):
            rate = 1000
        if (rate < 4):
            rate = 4
        data = 1000 // rate - 1
        self.i2c_write_byte(MPU_SAMPLE_RATE_REG, data) # 设置数字低通滤波器
        return self.setLPF(rate / 2) # 自动设置LPF为采样率的一半

    # 得到温度值
    # 返回值:温度值
    def getTemperature(self):
        buf = bytearray(2)
        buf = self.i2c_read_len(MPU_TEMP_OUTH_REG, 2)
        raw  = (buf[0] << 8) | buf[1]
        if (raw > (1 << 15)):
            raw = raw - (1<<16)

        temp = 36.53 + (raw) / 340

        return round(temp, 2)

    # 得到陀螺仪值(原始值)
    # gx,gy,gz:陀螺仪x,y,z轴的原始读数(带符号)
    # 返回值:0,成功
    # 其他,错误代码
    def getGyroscope(self):
        buf = bytearray(6)

        buf = self.i2c_read_len(MPU_GYRO_XOUTH_REG, 6)
        gx = (buf[0] << 8) | buf[1]
        gy = (buf[2] << 8) | buf[3]
        gz = (buf[4] << 8) | buf[5]

        if (gx > (1 << 15)):
            gx = gx - (1<<16)

        if (gy > (1 << 15)):
            gy = gy - (1<<16)

        if (gz > (1 << 15)):
            gz = gz - (1<<16)

        return (gx, gy, gz)

    # 得到加速度值(原始值)
    # gx,gy,gz:陀螺仪x,y,z轴的原始读数(带符号)
    # 返回值:0,成功
    # 其他,错误代码
    def getAcceleration(self):
        buf = bytearray(6)

        buf = self.i2c_read_len(MPU_ACCEL_XOUTH_REG, 6)
        ax = (buf[0] << 8) | buf[1]
        ay = (buf[2] << 8) | buf[3]
        az = (buf[4] << 8) | buf[5]

        if (ax > (1 << 15)):
            ax = ax - (1<<16)

        if (ay > (1 << 15)):
            ay = ay - (1<<16)

        if (az > (1 << 15)) :
            az = az - (1<<16)

        return (ax, ay, az)

    def getData(self):
        global mpu6050_dict
        mpu6050_dict['temp'] = self.getTemperature()
        arr = self.getGyroscope()
        mpu6050_dict['gyroX'] = arr[0]
        mpu6050_dict['gyroY'] = arr[1]
        mpu6050_dict['gyroZ'] = arr[2]
        brr = self.getAcceleration()
        mpu6050_dict['accX'] = brr[0]
        mpu6050_dict['accY'] = brr[1]
        mpu6050_dict['accZ'] = brr[2]
        return mpu6050_dict

    # 初始化MPU6050
    # 返回值:0,成功
    # 其他,错误代码
    def init(self):
        device_id = 0

        self.i2c_write_byte(MPU_PWR_MGMT1_REG, 0X80)  # 复位MPU6050
        sleep_ms(200)
        self.i2c_write_byte(MPU_PWR_MGMT1_REG, 0X00)  # 唤醒MPU6050
        self.setGyroFsr(3)                      # 陀螺仪传感器,±2000dps
        self.setAccelFsr(0)                     # 加速度传感器,±2g
        self.setRate(50)                         # 设置采样率50Hz
        self.i2c_write_byte(MPU_INT_EN_REG, 0X00)     # 关闭所有中断
        self.i2c_write_byte(MPU_USER_CTRL_REG, 0X00)  # I2C主模式关闭
        self.i2c_write_byte(MPU_FIFO_EN_REG, 0X00)    # 关闭FIFO
        self.i2c_write_byte(MPU_INTBP_CFG_REG, 0X80)  # INT引脚低电平有效
        device_id = self.i2c_read_byte(MPU_DEVICE_ID_REG)
        if (device_id == MPU_DEV_ID):
            # 器件ID正确
            self.i2c_write_byte(MPU_PWR_MGMT1_REG, 0X01) # 设置CLKSEL,PLL X轴为参考
            self.i2c_write_byte(MPU_PWR_MGMT2_REG, 0X00) # 加速度与陀螺仪都工作
            self.setRate(50)                        # 设置采样率为50Hz
            return 0
        else:
            return 1
