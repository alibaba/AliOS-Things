# -*- coding: UTF-8 -*-

"""
The driver for spl06 chip, it is a temperature and humidity sensor.
"""

from driver import I2C
from utime import sleep_ms
import math

EEPROM_CHIP_ADDRESS = 0x77;

class spl06Error(Exception):
    def __init__(self, value=0, msg="spl06 common error"):
        self.value = value
        self.msg = msg

    def __str__(self):
        return "Error code:%d, Error message: %s" % (self.value, str(self.msg))

    __repr__ = __str__

class SPL06(object):
    """
    This class implements spl06 chip's defs.
    """
    def __init__(self):
        self.i2cDev = None

    def open(self, devid):
        self.i2cDev = I2C()
        self.i2cDev.open(devid)

    def i2c_eeprom_read_var(self, chipAddress, addr):
        return self.devRegReadWrite1Byte(0, addr, 0);

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

    def init(self):
        tmp = 0;
        rRegID = bytearray([0x0D, 0x0]);
        wRegPressure8xOversampling = bytearray([0x06, 0x03]);
        wRegTemperature8xOversampling = bytearray([0x07, 0x83]);
        wRegContinuousTempAndPressureMeasurement = bytearray([0x08, 0B0111]);
        wRegFIFOPressureMeasurement = bytearray([0x09, 0x00]);

        tmp = rRegID;
        self.devRegReadWrite1Byte(0, tmp[0], tmp[1]);

        tmp = wRegPressure8xOversampling;
        self.devRegReadWrite1Byte(1, tmp[0], tmp[1]);
        tmp = wRegTemperature8xOversampling;
        self.devRegReadWrite1Byte(1, tmp[0], tmp[1]);
        tmp = wRegContinuousTempAndPressureMeasurement;
        self.devRegReadWrite1Byte(1, tmp[0], tmp[1]);
        tmp = wRegFIFOPressureMeasurement;
        self.devRegReadWrite1Byte(1, tmp[0], tmp[1]);

    # Get the firmware version of the chip.
    def getID(self) :
        reg = bytearray([0x0D]);
        version = bytearray(1);

        self.i2cDev.write(reg);
        self.i2cDev.read(version);
        print("spl06 ID is " + str(version[0]));

        return version[0];


    def get_altitude(self, pressure, seaLevelhPa):
        if (seaLevelhPa == 0):
            return -1;

        pressure /= 100;
        altitude = 44330 * (1.0 - math.pow(pressure / seaLevelhPa, 0.1903));

        return altitude;

    def get_temperature_scale_factor(self):
        tmp_Byte = self.i2c_eeprom_read_var(EEPROM_CHIP_ADDRESS, 0X07); # MSB

        tmp_Byte = tmp_Byte & 0B00000111;
        print("tmp_Byte: %d\n" %tmp_Byte);

        if (tmp_Byte == 0B000):
            k = 524288.0;
            
        elif (tmp_Byte == 0B001):
            k = 1572864.0;
            
        elif (tmp_Byte == 0B010):
            k = 3670016.0;
            
        elif (tmp_Byte == 0B011):
            k = 7864320.0;
            
        elif (tmp_Byte == 0B100):
            k = 253952.0;
            
        elif (tmp_Byte == 0B101):
            k = 516096.0;
            
        elif (tmp_Byte == 0B110):
            k = 1040384.0;
            
        elif (tmp_Byte == 0B111):
            k = 2088960.0;

        print("k=%d\n" %k);
        return k;

    def get_pressure_scale_factor(self):
        tmp_Byte = self.i2c_eeprom_read_var(EEPROM_CHIP_ADDRESS, 0X06); # MSB
        # tmp_Byte = tmp_Byte >> 4; #Focus on bits 6-4 - measurement rate
        tmp_Byte = tmp_Byte & 0B00000111; # Focus on 2-0 oversampling rate
        # tmp_Byte = 0B011;

        # oversampling rate
        if (tmp_Byte == 0B000):
            k = 524288.0;
            
        elif (tmp_Byte == 0B001):
            k = 1572864.0;
            
        elif (tmp_Byte == 0B010):
            k = 3670016.0;
            
        elif (tmp_Byte == 0B011):
            k = 7864320.0;
            
        elif (tmp_Byte == 0B100):
            k = 253952.0;
            
        elif (tmp_Byte == 0B101):
            k = 516096.0;
            
        elif (tmp_Byte == 0B110):
            k = 1040384.0;
            
        elif (tmp_Byte == 0B111):
            k = 2088960.0;

        print("k=%d\n" %k);
        return k;

    def get_traw(self):
        tmp_MSB  = self.i2c_eeprom_read_var(EEPROM_CHIP_ADDRESS, 0X03); # MSB
        tmp_LSB  = self.i2c_eeprom_read_var(EEPROM_CHIP_ADDRESS, 0X04); # LSB
        tmp_XLSB = self.i2c_eeprom_read_var(EEPROM_CHIP_ADDRESS, 0X05); # XLSB

        tmp = (tmp_MSB << 8) | tmp_LSB;
        tmp = (tmp << 8) | tmp_XLSB;

        if (tmp & (1 << 23)):
            tmp = tmp | 0XFF000000; # Set left bits to one for 2's complement
                                    # conversion of negitive number
        print("get_traw: tmp_MSB=%d, tmp_LSB=%d, tmp_XLSB=%d\n" %(tmp_MSB, tmp_LSB, tmp_XLSB));
        return tmp;

    def get_praw(self):
        tmp_MSB  = self.i2c_eeprom_read_var(EEPROM_CHIP_ADDRESS, 0X00); # MSB
        tmp_LSB  = self.i2c_eeprom_read_var(EEPROM_CHIP_ADDRESS, 0X01); # LSB
        tmp_XLSB = self.i2c_eeprom_read_var(EEPROM_CHIP_ADDRESS, 0X02); # XLSB

        tmp = (tmp_MSB << 8) | tmp_LSB;
        tmp = (tmp << 8) | tmp_XLSB;

        if (tmp & (1 << 23)):
            tmp = -((2 << 23) - tmp)

        return tmp;

    def get_c0(self):
        tmp_MSB = self.i2c_eeprom_read_var(EEPROM_CHIP_ADDRESS, 0X10);
        tmp_LSB = self.i2c_eeprom_read_var(EEPROM_CHIP_ADDRESS, 0X11);

        tmp_LSB = tmp_LSB >> 4;

        tmp = (tmp_MSB << 4) | tmp_LSB;

        if (tmp & (1 << 11)):
            # Check for 2's complement negative number
            tmp = tmp | 0XF000; # Set left bits to one for 2's complement
                                # conversion of negitive number
        if (tmp > (1 << 15)):
            tmp &= 0xFFFF
            tmp = tmp - (1<<16)

        print("get_c0: tmp_MSB=%d, tmp_LSB=%d, tmp=%d\n" %(tmp_MSB, tmp_LSB, tmp));

        return tmp;

    def get_c1(self):
        tmp_MSB = self.i2c_eeprom_read_var(EEPROM_CHIP_ADDRESS, 0X11);
        tmp_LSB = self.i2c_eeprom_read_var(EEPROM_CHIP_ADDRESS, 0X12);

        tmp_MSB = tmp_MSB & 0XF;
        tmp     = (tmp_MSB << 8) | tmp_LSB;

        if (tmp & (1 << 11)):
            # Check for 2's complement negative number
            tmp = tmp | 0XF000; # Set left bits to one for 2's complement
                                # conversion of negitive number
        
        if (tmp > (1 << 15)):
            tmp = tmp - (1<<16)
        print("get_c1: tmp_MSB=%d, tmp_LSB=%d, tmp=%d\n" %(tmp_MSB, tmp_LSB, tmp));
        return tmp;

    def get_c00(self):
        tmp_MSB  = self.i2c_eeprom_read_var(EEPROM_CHIP_ADDRESS, 0X13);
        tmp_LSB  = self.i2c_eeprom_read_var(EEPROM_CHIP_ADDRESS, 0X14);
        tmp_XLSB = self.i2c_eeprom_read_var(EEPROM_CHIP_ADDRESS, 0X15);

        tmp_XLSB = tmp_XLSB >> 4;

        tmp = (tmp_MSB << 8) | tmp_LSB;
        tmp = (tmp << 4) | tmp_XLSB;

        tmp = tmp_MSB << 12 | tmp_LSB << 4 | tmp_XLSB >> 4;

        if (tmp & (1 << 19)):
            tmp = tmp | 0XFFF00000; # Set left bits to one for 2's complement
                                    # conversion of negitive number
        return tmp;

    def get_c10(self):
        tmp_MSB  = self.i2c_eeprom_read_var(EEPROM_CHIP_ADDRESS, 0X15); # 4 bits
        tmp_LSB  = self.i2c_eeprom_read_var(EEPROM_CHIP_ADDRESS, 0X16); # 8 bits
        tmp_XLSB = self.i2c_eeprom_read_var(EEPROM_CHIP_ADDRESS, 0X17); # 8 bits

        tmp_MSB = tmp_MSB & 0b00001111;

        tmp = (tmp_MSB << 4) | tmp_LSB;
        tmp = (tmp << 8) | tmp_XLSB;

        tmp = tmp_MSB << 16 | tmp_LSB << 8 | tmp_XLSB;

        if (tmp & (1 << 19)):
            tmp = tmp | 0XFFF00000; # Set left bits to one for 2's complement
                                    # conversion of negitive number
        
        if (tmp > (1 << 15)):
            tmp &= 0xFFFF
            tmp = tmp - (1<<16);

        return tmp;

    def get_c01(self):
        tmp_MSB = self.i2c_eeprom_read_var(EEPROM_CHIP_ADDRESS, 0X18);
        tmp_LSB = self.i2c_eeprom_read_var(EEPROM_CHIP_ADDRESS, 0X19);

        tmp = (tmp_MSB << 8) | tmp_LSB;
        if (tmp > (1 << 15)):
            tmp = tmp - (1<<16);
        return tmp;

    def get_c11(self):
        tmp_MSB = self.i2c_eeprom_read_var(EEPROM_CHIP_ADDRESS, 0X1A);
        tmp_LSB = self.i2c_eeprom_read_var(EEPROM_CHIP_ADDRESS, 0X1B);

        tmp = (tmp_MSB << 8) | tmp_LSB;
        if (tmp > (1 << 15)):
            tmp = tmp - (1<<16);
        return tmp;

    def get_c20(self):
        tmp_MSB = self.i2c_eeprom_read_var(EEPROM_CHIP_ADDRESS, 0X1C);
        tmp_LSB = self.i2c_eeprom_read_var(EEPROM_CHIP_ADDRESS, 0X1D);

        tmp = (tmp_MSB << 8) | tmp_LSB;
        if (tmp > (1 << 15)):
            tmp = tmp - (1<<16);
        return tmp;

    def get_c21(self):
        tmp_MSB = self.i2c_eeprom_read_var(EEPROM_CHIP_ADDRESS, 0X1E);
        tmp_LSB = self.i2c_eeprom_read_var(EEPROM_CHIP_ADDRESS, 0X1F);

        tmp = (tmp_MSB << 8) | tmp_LSB;
        if (tmp > (1 << 15)):
            tmp = tmp - (1<<16);
        return tmp;

    def get_c30(self):
        tmp_MSB = self.i2c_eeprom_read_var(EEPROM_CHIP_ADDRESS, 0X20);
        tmp_LSB = self.i2c_eeprom_read_var(EEPROM_CHIP_ADDRESS, 0X21);

        tmp = (tmp_MSB << 8) | tmp_LSB;
        if (tmp > (1 << 15)):
            tmp = tmp - (1<<16);
        return tmp;

    def spl06_getdata(self):
        # Serial.println("\nDevice Reset\n");
        # i2c_eeprom_write_var(EEPROM_CHIP_ADDRESS, 0x0C, 0b1001);
        # delay(1000);

        tmp    = self.i2c_eeprom_read_var(EEPROM_CHIP_ADDRESS, 0x0D);
        tmp    = self.i2c_eeprom_read_var(EEPROM_CHIP_ADDRESS, 0x06);
        tmp    = self.i2c_eeprom_read_var(EEPROM_CHIP_ADDRESS, 0x07);
        tmp    = self.i2c_eeprom_read_var(EEPROM_CHIP_ADDRESS, 0x08);
        tmp    = self.i2c_eeprom_read_var(EEPROM_CHIP_ADDRESS, 0x09);
        tmp    = self.i2c_eeprom_read_var(EEPROM_CHIP_ADDRESS, 0x0A);
        tmp    = self.i2c_eeprom_read_var(EEPROM_CHIP_ADDRESS, 0x0B);

        c0              = self.get_c0();
        c1              = self.get_c1();
        c00             = self.get_c00();
        c10             = self.get_c10();
        c01             = self.get_c01();
        c11             = self.get_c11();
        c20             = self.get_c20();
        c21             = self.get_c21();
        c30             = self.get_c30();
        traw    = self.get_traw();
        traw_sc = traw / self.get_temperature_scale_factor();
        traw_sc = round(traw_sc,2)
        print("traw_sc: %0.2f\n" %traw_sc);

        Ctemp = c0 * 0.5 + c1 * traw_sc;
        Ctemp = round(Ctemp,2)
        print("Ctemp:" + str(Ctemp) + ".  " + "c0:" + str(c0) + " c1" + str(c1) + " traw_sc:" + str(traw_sc));

        Ftemp = (Ctemp * 9 / 5) + 32;
        Ftemp = round(Ftemp,2)
        print("Ftemp: %d" %Ftemp)

        praw = self.get_praw();

        praw_sc = (praw) / self.get_pressure_scale_factor();
    
        print("praw: %d" %praw)
        print("praw_sc: %d" %praw_sc)

        print("c00: %d" %c00)
        print("c10: %d" %c10)
        print("c20: %d" %c20)
        print("c30: %d" %c30)
        print("c01: %d" %c01)
        print("c11: %d" %c11)
        print("c21: %d" %c21)

        pcomp =\
            (c00) + \
            praw_sc * ((c10) + \
                    praw_sc * ((c20) + praw_sc * (c30))) + \
            traw_sc * (c01) + \
            traw_sc * praw_sc * ((c11) + praw_sc * (c21));

        pressure = pcomp / 100; # convert to mb
        print("pressure: %d" %pressure)

        # local_pressure = 1010.5; # Look up local sea level pressure on
        # google
        local_pressure =  \
            1011.1; # Look up local sea level pressure on google # Local pressure
                    # from airport website 8/22
        print("Local Airport Sea Level Pressure: %0.2f mb\n" %local_pressure);
        altitude = self.get_altitude(pcomp, local_pressure);
        print("altitude: %d" %altitude)

    def close(self):
        self.i2cDev.close()
