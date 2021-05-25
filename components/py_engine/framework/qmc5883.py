# -*- coding: UTF-8 -*-

"""
The driver for qmc5883 chip
"""

from driver import I2C
from utime import sleep_ms
import math

QMC5883L_ADDR  = 0x0D;
x_max = 0;
x_min = 0;
z_min = 0;
y_max = 0;
y_min = 0;
z_max = 0;
addr = 0;
mode = 0;
rate = 0;
g_range = 0;
oversampling = 0;

INT16_MIN = (-32767-1)
INT16_MAX = 32767

# Register numbers 
QMC5883L_X_LSB    = 0
QMC5883L_X_MSB    = 1
QMC5883L_Y_LSB    = 2
QMC5883L_Y_MSB    = 3
QMC5883L_Z_LSB    = 4
QMC5883L_Z_MSB    = 5
QMC5883L_STATUS   = 6
QMC5883L_TEMP_LSB = 7
QMC5883L_TEMP_MSB = 8
QMC5883L_CONFIG   = 9
QMC5883L_CONFIG2  = 10
QMC5883L_RESET    = 11
QMC5883L_RESERVED = 12
QMC5883L_CHIP_ID  = 13

QMC5883L_STATUS_DRDY = 1
QMC5883L_STATUS_OVL  = 2
QMC5883L_STATUS_DOR  = 4

# Oversampling values for the CONFIG register 
QMC5883L_CONFIG_OS512 = 0b00000000
QMC5883L_CONFIG_OS256 = 0b01000000
QMC5883L_CONFIG_OS128 = 0b10000000
QMC5883L_CONFIG_OS64  = 0b11000000

# Range values for the CONFIG register 
QMC5883L_CONFIG_2GAUSS = 0b00000000
QMC5883L_CONFIG_8GAUSS = 0b00010000

# Rate values for the CONFIG register 
QMC5883L_CONFIG_10HZ  = 0b00000000
QMC5883L_CONFIG_50HZ  = 0b00000100
QMC5883L_CONFIG_100HZ = 0b00001000
QMC5883L_CONFIG_200HZ = 0b00001100

# Mode values for the CONFIG register 
QMC5883L_CONFIG_STANDBY = 0b00000000
QMC5883L_CONFIG_CONT    = 0b00000001

# Apparently M_PI isn't available in all environments. 
M_PI = 3.14159265358979323846264338327950288

class qmc5883Error(Exception):
    def __init__(self, value=0, msg="qmc5883 common error"):
        self.value = value
        self.msg = msg

    def __str__(self):
        return "Error code:%d, Error message: %s" % (self.value, str(self.msg))

    __repr__ = __str__

class QMC5883(object):
    """
    This class implements qmc5883 chip's defs.
    """
    def __init__(self):
        self.i2cDev = None

    def open(self, devid):
        self.i2cDev = I2C()
        self.i2cDev.open(devid)

    def devRegRead1Byte(self, addr):
        return self.devRegReadWrite1Byte(0, addr, 0);

    def devRegReadWrite1Byte(self, mode, addr, value):
        #0 read mode
        #1 write mode
        if (mode == 0):
            Reg = bytearray([addr])
            self.i2cDev.write(Reg);
            sleep_ms(30)
            tmp = bytearray(1)
            self.i2cDev.read(tmp)
            print("<-- read addr " + str(addr) + ", value = " + str(tmp[0]));
            return tmp[0];
        else:
            Reg = bytearray([addr, value])
            self.i2cDev.write(Reg);
            print("--> write addr " + str(addr) + ", value = " + str(value));
            return 0;

    def devRegWrite1Byte(self, data):
        Reg = bytearray([data])
        self.i2cDev.write(Reg);
        print("--> write value = " + str(Reg[0]));

    def devRegReadNByte(self, addr, len):
        reg = bytearray([addr]);
        data = bytearray(len);
        self.i2cDev.write(reg);
        sleep_ms(20)
        self.i2cDev.read(data);
        print("--> read " + str(len) + " bytes from addr " + str(addr) + ", " + str(len) + " bytes value = " + str(data));
        return data;
    
    def qmc5883l_write_register(self, addr, reg,  data):
        print(">>>> wirte reg: %d, data: %d\n" %(reg, data))
        self.devRegReadWrite1Byte(1, reg, data);

    def qmc5883l_read_register(self, addr, reg):
        return self.devRegRead1Byte(reg);

    def qmc5883l_read_len(self, reg, len):
        return self.devRegReadNByte(reg, len);

    def qmc5883l_reconfig(self):
        self.qmc5883l_write_register(addr, QMC5883L_CONFIG, oversampling | g_range | rate | mode);
        sleep_ms(50)
        self.qmc5883l_write_register(addr, QMC5883L_CONFIG2, 0x1);

    def qmc5883l_reset(self):
        self.qmc5883l_write_register(addr, QMC5883L_RESET, 0x01);
        sleep_ms(500)
        self.qmc5883l_reconfig();
        sleep_ms(50)
        self.qmc5883l_resetCalibration();

    def qmc5883l_setOversampling(self, x):
        global oversampling
        if (x == 512):
            oversampling = QMC5883L_CONFIG_OS512;
        elif (x == 256):
            oversampling = QMC5883L_CONFIG_OS256;
        elif (x == 128):
            oversampling = QMC5883L_CONFIG_OS128;
        elif (x == 64):
            oversampling = QMC5883L_CONFIG_OS64;
        self.qmc5883l_reconfig();

    def qmc5883l_setRange(self, x):
        global g_range
        if (x == 2):
            g_range = QMC5883L_CONFIG_2GAUSS;
        elif (x == 8):
            g_range = QMC5883L_CONFIG_8GAUSS;
        self.qmc5883l_reconfig();

    def qmc5883l_setSamplingRate(self, x):
        global rate
        if (x == 10):
            rate = QMC5883L_CONFIG_10HZ;
        elif (x == 50):
            rate = QMC5883L_CONFIG_50HZ;
        elif (x == 100):
            rate = QMC5883L_CONFIG_100HZ;
        elif (x == 200):
            rate = QMC5883L_CONFIG_200HZ;
        self.qmc5883l_reconfig();

    def _qmc5883l_init(self):
        global addr
        global oversampling
        global g_range
        global rate
        global mode
        # This assumes the wire library has been initialized. 
        addr         = QMC5883L_ADDR;
        oversampling = QMC5883L_CONFIG_OS512;
        g_range        = QMC5883L_CONFIG_8GAUSS;
        rate         = QMC5883L_CONFIG_200HZ;
        mode         = QMC5883L_CONFIG_CONT;
        print("addr %d,oversampling %d,g_range %d,rate %d, mode %d" %(addr, oversampling, g_range, rate, mode))

        self.qmc5883l_reset();

    def qmc5883l_ready(self):
        sleep_ms(200);
        tmp = self.qmc5883l_read_register(addr, QMC5883L_STATUS) & QMC5883L_STATUS_DRDY
        return tmp;

    def qmc5883l_readRaw(self):
        timeout = 10000;
        arr = [1, 2, 3];
        data = bytearray(6);
        ready = self.qmc5883l_ready();
        while (ready == 0 and timeout):
            ready = self.qmc5883l_ready();
            timeout -= 1;
            print("ready = %d" %(ready))

        data = self.qmc5883l_read_len(QMC5883L_X_LSB, 6);

        x = data[0] | (data[1] << 8);
        y = data[2] | (data[3] << 8);
        z = data[4] | (data[5] << 8);

        print("read_raw[%f,%f,%f],\n" %(x ,y, z));

        if (x > (1 << 15)):
            x = x - (1<<16)

        if (y > (1 << 15)):
            y = y - (1<<16)

        if (z > (1 << 15)):
            z = z - (1<<16)

        arr[0] = x;
        arr[1] = y;
        arr[2] = z;
        return arr;

    def qmc5883l_resetCalibration(self):
        global x_max
        global x_min
        global z_min
        global y_max
        global y_min
        global z_max
        x_max = y_max = z_max = INT16_MIN;
        x_min = y_min = z_min = INT16_MAX;

    def qmc5883l_readHeading(self):
        global x_max
        global x_min
        global z_min
        global y_max
        global y_min
        global z_max
        global addr
        global mode
        global rate
        global g_range
        global oversampling

        tmp = self.qmc5883l_read_register(addr, QMC5883L_STATUS) & QMC5883L_STATUS_DRDY;    
        print("read QMC5883L_STATUS: %d\n" %(tmp))

        xyz_org = self.qmc5883l_readRaw();
        x_org = xyz_org[0];
        y_org = xyz_org[1];
        z_org = xyz_org[2];
        print("org[%f,%f,%f]\n" %(x_org ,y_org, z_org));

        # Update the observed boundaries of the measurements 
        if (x_org < x_min):
            x_min = x_org;

        if (x_org > x_max):
            x_max = x_org;

        if (y_org < y_min):
            y_min = y_org;

        if (y_org > y_max):
            y_max = y_org;

        if (z_org < z_min):
            z_min = z_org;

        if (z_org > z_max):
            z_max = z_org;

        # Bail out if not enough data is available. 
        if ((x_min == x_max) or (y_min == y_max) or (z_max == z_min)):
            print("x_min %f == x_max %f or y_min %f == y_max %f or z_max%f == z_min%f\n" %(x_min, x_max, y_min, y_max, z_max, z_min))
            return 0;

        # Recenter the measurement by subtracting the average 
        x_offset = (x_max + x_min) / 2.0;
        y_offset = (y_max + y_min) / 2.0;
        z_offset = (z_max + z_min) / 2.0;

        x_fit = (x_org - x_offset) * 1000.0 / (x_max - x_min);
        y_fit = (y_org - y_offset) * 1000.0 / (y_max - y_min);
        z_fit = (z_org - z_offset) * 1000.0 / (z_max - z_min);

        print("fix[%f,%f,%f],\n" %(x_fit ,y_fit, z_fit));

        heading = 180.0 * math.atan2(x_fit, y_fit) / M_PI;
        if (heading <= 0):
            heading = heading + 360;
        print("heading = %f\n", heading);
        return heading;

    def init(self):
        self._qmc5883l_init();

    def close(self):
        self.i2cDev.close()
