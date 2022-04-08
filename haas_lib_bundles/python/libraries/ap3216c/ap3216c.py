"""
Copyright (C) 2015-2020 Alibaba Group Holding Limited

The driver for AP3216C chip, The AP3216C is an integrated ALS & PS module
that includes a digital ambient light sensor [ALS], a proximity sensor [PS],
and an IR LED in a single package.
"""
from micropython import const
from driver import I2C
from utime import sleep_ms

# System Register
AP3216C_SYS_CONFIGURATION_REG    = const(0x00)
AP3216C_SYS_INT_STATUS_REG       = const(0x01)
AP3216C_SYS_INT_CLEAR_MANNER_REG = const(0x02)
AP3216C_IR_DATA_L_REG            = const(0x0A)
AP3216C_IR_DATA_H_REG            = const(0x0B)
AP3216C_ALS_DATA_L_REG           = const(0x0C)
AP3216C_ALS_DATA_H_REG           = const(0x0D)
AP3216C_PS_DATA_L_REG            = const(0x0E)
AP3216C_PS_DATA_H_REG            = const(0x0F)

# ALS Register
AP3216C_ALS_CONFIGURATION_REG    = const(0x10)
AP3216C_ALS_CALIBRATION_REG      = const(0x19)
AP3216C_ALS_THRESHOLD_LOW_L_REG  = const(0x1A)
AP3216C_ALS_THRESHOLD_LOW_H_REG  = const(0x1B)
AP3216C_ALS_THRESHOLD_HIGH_L_REG = const(0x1C)
AP3216C_ALS_THRESHOLD_HIGH_H_REG = const(0x1D)

# PS Register
AP3216C_PS_CONFIGURATION_REG    = const(0x20)
AP3216C_PS_LED_DRIVER_REG       = const(0x21)
AP3216C_PS_INT_FORM_REG         = const(0x22)
AP3216C_PS_MEAN_TIME_REG        = const(0x23)
AP3216C_PS_LED_WAITING_TIME_REG = const(0x24)
AP3216C_PS_CALIBRATION_L_REG    = const(0x28)
AP3216C_PS_CALIBRATION_H_REG    = const(0x29)
AP3216C_PS_THRESHOLD_LOW_L_REG  = const(0x2A)
AP3216C_PS_THRESHOLD_LOW_H_REG  = const(0x2B)
AP3216C_PS_THRESHOLD_HIGH_L_REG = const(0x2C)
AP3216C_PS_THRESHOLD_HIGH_H_REG = const(0x2D)

#mode value
AP3216C_MODE_POWER_DOWN    = const(0x0)
AP3216C_MODE_ALS           = const(0x1)
AP3216C_MODE_PS            = const(0x2)
AP3216C_MODE_ALS_AND_PS    = const(0x3)
AP3216C_MODE_SW_RESET      = const(0x4)
AP3216C_MODE_ALS_ONCE      = const(0x5)
AP3216C_MODE_PS_ONCE       = const(0x6)
AP3216C_MODE_ALS_AND_PS_ONCE  = const(0x7)

#ap3216c_int_clear_manner
AP3216C_INT_CLEAR_MANNER_BY_READING    = const(0x0)
AP3216C_ALS_CLEAR_MANNER_BY_SOFTWARE   = const(0x1)

#als_range
AP3216C_ALS_RANGE_20661    = const(0x0)
AP3216C_ALS_RANGE_5162     = const(0x1)
AP3216C_ALS_RANGE_1291     = const(0x2)
AP3216C_ALS_RANGE_323    = const(0x3)

#als_range
AP3216C_PS_GAIN1    = const(0x0)
AP3216C_PS_GAIN2     = const(0x1)
AP3216C_PS_GAIN4     = const(0x2)
AP3216C_PS_GAIN8    = const(0x3)

AP3216C_SYSTEM_MODE          = const(0x0)
AP3216C_INT_PARAM            = const(0x1)
AP3216C_ALS_RANGE            = const(0x2)
AP3216C_ALS_PERSIST          = const(0x3)
AP3216C_ALS_CALIBRATION      = const(0x4)
AP3216C_ALS_LOW_THRESHOLD_L  = const(0x5)
AP3216C_ALS_LOW_THRESHOLD_H  = const(0x6)
AP3216C_ALS_HIGH_THRESHOLD_L = const(0x7)
AP3216C_ALS_HIGH_THRESHOLD_H = const(0x8)
AP3216C_PS_INTEGRATED_TIME   = const(0x9)
AP3216C_PS_GAIN              = const(0xa)
AP3216C_PS_PERSIST           = const(0xb)
AP3216C_PS_LED_CONTROL       = const(0xc)
AP3216C_PS_LED_DRIVER_RATIO  = const(0xd)
AP3216C_PS_INT_MODE          = const(0xe)
AP3216C_PS_MEAN_TIME         = const(0xf)
AP3216C_PS_WAITING_TIME      = const(0x10)
AP3216C_PS_CALIBRATION_L     = const(0x11)
AP3216C_PS_CALIBRATION_H     = const(0x12)
AP3216C_PS_LOW_THRESHOLD_L   = const(0x13)
AP3216C_PS_LOW_THRESHOLD_H   = const(0x14)
AP3216C_PS_HIGH_THRESHOLD_L  = const(0x15)
AP3216C_PS_HIGH_THRESHOLD_H  = const(0x16)

class AP3216C(object):
    """
    This class implements ap3216c chip's defs.
    """
    def __init__(self, i2cDev):
        self._i2cDev = None
        if not isinstance(i2cDev, I2C):
            raise ValueError("parameter is not an I2C object")
        # make AP3216C's internal object points to i2cDev
        self._i2cDev = i2cDev
        self.init()

# 写寄存器的值
    def write_reg(self, addr, data):
        msgbuf = bytearray([data])
        self._i2cDev.memWrite(msgbuf, addr, 8)
        # print("--> write addr " + str(addr) + ", value = " + str(msgbuf))

# 读寄存器的值
    def read_regs(self, addr, len):
        buf = bytearray(len)
        self._i2cDev.memRead(buf, addr, 8)
        # print("--> read " + str(len) + " bytes from addr " + str(addr) + ", " + str(len) + " bytes value = " + str(buf))
        return buf

    # 软件复位传感器
    def reset_sensor(self):
        self.write_reg(AP3216C_SYS_CONFIGURATION_REG, AP3216C_MODE_SW_RESET); # reset

    def read_low_and_high(self, reg, len):
        data = self.read_regs(reg, len)[0] | (self.read_regs(reg + 1, len)[0] << len * 8) # 读低字节 - 读高字节 - 合并数据

        if (data > (1 << 15)):
            data = data - (1<<16)

        return data

    def ap3216c_get_IntStatus(self):
        # 读中断状态寄存器
        IntStatus = self.read_regs(AP3216C_SYS_INT_STATUS_REG, 1)[0]
        # IntStatus 第 0 位表示 ALS 中断，第 1 位表示 PS 中断。
        return IntStatus # 返回状态

    def ap3216c_int_init(self):
        #print("ap3216c_int_init")
        pass

    #配置 中断输入引脚
    def ap3216c_int_Config(self):
        #print("ap3216c_int_Config")
        pass

    #初始化入口
    def init(self):
        # reset ap3216c
        self.reset_sensor()
        sleep_ms(100)
        self.ap3216c_set_param(AP3216C_SYSTEM_MODE, AP3216C_MODE_ALS_AND_PS)
        sleep_ms(150) # delay at least 112.5ms

        self.ap3216c_int_Config()
        self.ap3216c_int_init()


    # This function reads light by ap3216c sensor measurement
    # @param no
    # @return the ambient light converted to float data.
    #
    def ap3216c_read_ambient_light(self):
        read_data = self.read_low_and_high(AP3216C_ALS_DATA_L_REG, 1)
        range = self.ap3216c_get_param(AP3216C_ALS_RANGE)
        #print("ap3216c_read_ambient_light read_data is " , read_data, range)
        if (range == AP3216C_ALS_RANGE_20661):
            brightness = 0.35 * read_data # sensor ambient light converse to reality
        elif (range == AP3216C_ALS_RANGE_5162):
            brightness = 0.0788 * read_data # sensor ambient light converse to reality
        elif (range == AP3216C_ALS_RANGE_1291):
            brightness = 0.0197 * read_data # sensor ambient light converse to reality
        elif (range == AP3216C_ALS_RANGE_323):
            brightness = 0.0049 * read_data # sensor ambient light converse to reality

        return brightness

    #This function reads proximity by ap3216c sensor measurement
    #@param no
    #@return the proximity data.
    def ap3216c_read_ps_data(self):
        read_data = self.read_low_and_high(AP3216C_PS_DATA_L_REG, 1) # read two data
        #print("ap3216c_read_ps_data read_data is " , read_data);
        if (1 == ((read_data >> 6) & 0x01 or (read_data >> 14) & 0x01)) :
            return 55555 # 红外过高（IR），PS无效 返回一个 55555 的无效数据

        proximity = (read_data & 0x000f) + (((read_data >> 8) & 0x3f) << 4)
        # sensor proximity converse to reality
        if (proximity > (1 << 15)) :
            proximity = proximity - (1<<16)

        proximity |= read_data & 0x8000 # 取最高位，0 表示物体远离，1 表示物体靠近
        return proximity # proximity 后十位是数据位，最高位为状态位


    #This function reads ir by ap3216c sensor measurement
    #@param no
    #@return the ir data.
    def ap3216c_read_ir_data(self):
        read_data = self.read_low_and_high(AP3216C_IR_DATA_L_REG, 1) # read two data
        #print("ap3216c_read_ir_data read_data is" , read_data);
        proximity = (read_data & 0x0003) + ((read_data >> 8) & 0xFF)
        # sensor proximity converse to reality
        if (proximity > (1 << 15)) :
            proximity = proximity - (1<<16)

        return proximity

    #This function sets parameter of ap3216c sensor
    #@param cmd the parameter cmd of device
    #@param value for setting value in cmd register
    #@return the setting parameter status,RT_EOK reprensents setting successfully.

    def ap3216c_set_param(self, cmd, value):
        if cmd == AP3216C_SYSTEM_MODE:
            # default 000,power down
            self.write_reg(AP3216C_SYS_CONFIGURATION_REG, value)
        elif cmd == AP3216C_INT_PARAM:
            self.write_reg(AP3216C_SYS_INT_CLEAR_MANNER_REG, value)
        elif cmd == AP3216C_ALS_RANGE:
            args = self.read_regs(AP3216C_ALS_CONFIGURATION_REG, 1)[0]
            args &= 0xcf
            args |= value << 4
            self.write_reg(AP3216C_ALS_CONFIGURATION_REG, args)
        elif cmd == AP3216C_ALS_PERSIST:
            args = self.read_regs(AP3216C_ALS_CONFIGURATION_REG, 1)[0]
            args &= 0xf0
            args |= value
            self.write_reg(AP3216C_ALS_CONFIGURATION_REG, args)
        elif cmd == AP3216C_ALS_LOW_THRESHOLD_L:
            self.write_reg(AP3216C_ALS_THRESHOLD_LOW_L_REG, value)
        elif cmd == AP3216C_ALS_LOW_THRESHOLD_H:
           self.write_reg(AP3216C_ALS_THRESHOLD_LOW_H_REG, value)
        elif cmd == AP3216C_ALS_HIGH_THRESHOLD_L:
            self.write_reg(AP3216C_ALS_THRESHOLD_HIGH_L_REG, value)
        elif cmd == AP3216C_ALS_HIGH_THRESHOLD_H:
            self.write_reg(AP3216C_ALS_THRESHOLD_HIGH_H_REG, value)
        elif cmd == AP3216C_PS_GAIN:
            args = self.read_regs(AP3216C_PS_CONFIGURATION_REG, 1)[0]
            args &= 0xf3
            args |= value
            self.write_reg(AP3216C_PS_CONFIGURATION_REG, args)
        elif cmd == AP3216C_PS_PERSIST:
            args = self.read_regs(AP3216C_PS_CONFIGURATION_REG, 1)[0]
            args &= 0xfc
            args |= value
            self.write_reg(AP3216C_PS_CONFIGURATION_REG, args)
        elif cmd == AP3216C_PS_LOW_THRESHOLD_L:
            self.write_reg(AP3216C_PS_THRESHOLD_LOW_L_REG, value)
        elif cmd == AP3216C_PS_LOW_THRESHOLD_H:
            self.write_reg(AP3216C_PS_THRESHOLD_LOW_H_REG, value)
        elif cmd == AP3216C_PS_HIGH_THRESHOLD_L:
            self.write_reg(AP3216C_PS_THRESHOLD_HIGH_L_REG, value)
        elif cmd == AP3216C_PS_HIGH_THRESHOLD_H:
            self.write_reg(AP3216C_PS_THRESHOLD_HIGH_H_REG, value)

    #This function gets parameter of ap3216c sensor
    #@param cmd the parameter cmd of device
    #@param value to get value in cmd register
    #@return the getting parameter status,RT_EOK reprensents getting successfully.
    def ap3216c_get_param(self, cmd):
        if cmd == AP3216C_SYSTEM_MODE:
            value = self.read_regs(AP3216C_SYS_CONFIGURATION_REG, 1)[0]
        elif cmd == AP3216C_INT_PARAM:
            value = self.read_regs(AP3216C_SYS_INT_CLEAR_MANNER_REG, 1)[0]
        elif cmd == AP3216C_ALS_RANGE:
            value = self.read_regs(AP3216C_ALS_CONFIGURATION_REG, 1)[0]
            temp = (value & 0xff) >> 4
            value = temp
        elif cmd == AP3216C_ALS_PERSIST:
            temp = self.read_regs(AP3216C_ALS_CONFIGURATION_REG, 1)[0]
            temp = value & 0x0f
            value = temp
        elif cmd == AP3216C_ALS_LOW_THRESHOLD_L:
            value = self.read_regs(AP3216C_ALS_THRESHOLD_LOW_L_REG, 1)[0]
        elif cmd == AP3216C_ALS_LOW_THRESHOLD_H:
            value = self.read_regs(AP3216C_ALS_THRESHOLD_LOW_H_REG, 1)[0]
        elif cmd == AP3216C_ALS_HIGH_THRESHOLD_L:
            value = self.read_regs(AP3216C_ALS_THRESHOLD_HIGH_L_REG, 1)[0]
        elif cmd == AP3216C_ALS_HIGH_THRESHOLD_H:
            value = self.read_regs(AP3216C_ALS_THRESHOLD_HIGH_H_REG, 1)[0]
        elif cmd == AP3216C_PS_GAIN:
            temp = self.read_regs(AP3216C_PS_CONFIGURATION_REG, 1)[0]
            value = (temp & 0xc) >> 2
        elif cmd == AP3216C_PS_PERSIST:
            temp = self.read_regs(AP3216C_PS_CONFIGURATION_REG, 1)[0]
            value = temp & 0x3
        elif cmd == AP3216C_PS_LOW_THRESHOLD_L:
            value = self.read_regs(AP3216C_PS_THRESHOLD_LOW_L_REG, 1)[0]
        elif cmd == AP3216C_PS_LOW_THRESHOLD_H:
            value = self.read_regs(AP3216C_PS_THRESHOLD_LOW_H_REG, 1)[0]
        elif cmd == AP3216C_PS_HIGH_THRESHOLD_L:
            value = self.read_regs(AP3216C_PS_THRESHOLD_HIGH_L_REG, 1)[0]
        elif cmd == AP3216C_PS_HIGH_THRESHOLD_H:
            value = self.read_regs(AP3216C_PS_THRESHOLD_HIGH_H_REG, 1)[0]

        return value

    def getData(self):
        ap3216c_dict = {'brightness': 0, 'ir': 0, 'ps': 0}

        brightness = self.ap3216c_read_ambient_light()
        ir_data = self.ap3216c_read_ir_data()
        ps_data = self.ap3216c_read_ps_data()
        ap3216c_dict['brightness'] = brightness
        ap3216c_dict['ir'] = ir_data
        ap3216c_dict['ps'] = ps_data
        return ap3216c_dict

    # 获取光照强度值
    def getIlluminance(self):
        if not self._i2cDev:
            raise ValueError("i2cObj is not initialized")

        return self.ap3216c_read_ambient_light()

    # 获取接近状态：接近返回True，否则返回False
    def isProximate(self):
        if not self._i2cDev:
            raise ValueError("i2cObj is not initialized")

        ps = self.ap3216c_read_ps_data()
        if ((ps >> 15) & 1):
            return True
        else:
            return False

if __name__ == "__main__":
    '''
    The below i2c configuration is needed in your board.json.
    "ap3216c": {
      "type": "I2C",
      "port": 1,
      "addrWidth": 7,
      "freq": 100000,
      "mode": "master",
      "devAddr": 30
    }
    '''
    print("Testing ap3216c ...")

    i2cDev = I2C()
    i2cDev.open("ap3216c")

    ap3216cDev = AP3216C(i2cDev)

    illuminance = ap3216cDev.getIlluminance()
    print("The illuminance is:", illuminance)

    proxi = ap3216cDev.isProximate()
    print("The proximity state is", proxi)

    data = ap3216cDev.getData()

    print("The total datais: ", data)

    i2cDev.close()

    del ap3216cDev
    print("Test ap3216c done!")

