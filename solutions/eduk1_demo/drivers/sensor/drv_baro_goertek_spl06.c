/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include "drv_baro_goertek_spl06.h"
#include "aos/hal/i2c.h"
#include "ulog/ulog.h"
#include <stdio.h>
#include <stdlib.h>

#define SPL06_I2C_PORT 1

uint8_t EEPROM_CHIP_ADDRESS = 0x77;

static void
i2c_eeprom_write_uint8_t(uint8_t deviceaddress, uint8_t eeaddress, uint8_t data)
{
    uint8_t write_buffer[2] = {eeaddress, data};
    sensor_i2c_master_send(SPL06_I2C_PORT, EEPROM_CHIP_ADDRESS, write_buffer, 2, 1000);
}

static uint8_t i2c_eeprom_read_uint8_t(uint8_t deviceaddress, uint8_t eeaddress)
{
    uint8_t data;

    sensor_i2c_master_send(SPL06_I2C_PORT, EEPROM_CHIP_ADDRESS, &eeaddress, 1, 1000);
    aos_msleep(2);
    sensor_i2c_master_recv(SPL06_I2C_PORT, EEPROM_CHIP_ADDRESS, &data, 1, 1000);

    return data;
}

static double get_altitude(double pressure, double seaLevelhPa)
{
    if (seaLevelhPa == 0) {
        return -1;
    }

    double altitude;

    pressure /= 100;
    altitude = 44330 * (1.0 - pow(pressure / seaLevelhPa, 0.1903));

    return altitude;
}

static double get_temperature_scale_factor()
{
    double k;

    uint8_t tmp_Byte;
    tmp_Byte = i2c_eeprom_read_uint8_t(EEPROM_CHIP_ADDRESS, 0X07); // MSB

    tmp_Byte = tmp_Byte & 0B00000111;
    // printf("tmp_Byte: %d\n", tmp_Byte);

    switch (tmp_Byte) {
        case 0B000:
            k = 524288.0;
            break;

        case 0B001:
            k = 1572864.0;
            break;

        case 0B010:
            k = 3670016.0;
            break;

        case 0B011:
            k = 7864320.0;
            break;

        case 0B100:
            k = 253952.0;
            break;

        case 0B101:
            k = 516096.0;
            break;

        case 0B110:
            k = 1040384.0;
            break;

        case 0B111:
            k = 2088960.0;
            break;
    }
    return k;
}

static double get_pressure_scale_factor()
{
    double k;

    uint8_t tmp_Byte;
    tmp_Byte = i2c_eeprom_read_uint8_t(EEPROM_CHIP_ADDRESS, 0X06); // MSB
    // tmp_Byte = tmp_Byte >> 4; //Focus on bits 6-4 - measurement rate
    tmp_Byte = tmp_Byte & 0B00000111; // Focus on 2-0 oversampling rate
    // tmp_Byte = 0B011;

    // oversampling rate
    switch (tmp_Byte) {
        case 0B000:
            k = 524288.0;
            break;

        case 0B001:
            k = 1572864.0;
            break;

        case 0B010:
            k = 3670016.0;
            break;

        case 0B011:
            k = 7864320.0;
            break;

        case 0B100:
            k = 253952.0;
            break;

        case 0B101:
            k = 516096.0;
            break;

        case 0B110:
            k = 1040384.0;
            break;

        case 0B111:
            k = 2088960.0;
            break;
    }
    return k;
}

static int32_t get_traw()
{
    int32_t tmp;
    uint8_t tmp_MSB, tmp_LSB, tmp_XLSB;
    tmp_MSB  = i2c_eeprom_read_uint8_t(EEPROM_CHIP_ADDRESS, 0X03); // MSB
    tmp_LSB  = i2c_eeprom_read_uint8_t(EEPROM_CHIP_ADDRESS, 0X04); // LSB
    tmp_XLSB = i2c_eeprom_read_uint8_t(EEPROM_CHIP_ADDRESS, 0X05); // XLSB

    tmp = (tmp_MSB << 8) | tmp_LSB;
    tmp = (tmp << 8) | tmp_XLSB;

    if (tmp & (1 << 23))
        tmp = tmp | 0XFF000000; // Set left bits to one for 2's complement
                                // conversion of negitive number
    return tmp;
}

static int32_t get_praw()
{
    int32_t tmp;
    uint8_t tmp_MSB, tmp_LSB, tmp_XLSB;
    tmp_MSB  = i2c_eeprom_read_uint8_t(EEPROM_CHIP_ADDRESS, 0X00); // MSB
    tmp_LSB  = i2c_eeprom_read_uint8_t(EEPROM_CHIP_ADDRESS, 0X01); // LSB
    tmp_XLSB = i2c_eeprom_read_uint8_t(EEPROM_CHIP_ADDRESS, 0X02); // XLSB

    tmp = (tmp_MSB << 8) | tmp_LSB;
    tmp = (tmp << 8) | tmp_XLSB;

    if (tmp & (1 << 23))
        tmp = tmp | 0XFF000000; // Set left bits to one for 2's complement
                                // conversion of negitive number
    return tmp;
}

static int16_t get_c0()
{
    int16_t tmp;
    uint8_t tmp_MSB, tmp_LSB;

    tmp_MSB = i2c_eeprom_read_uint8_t(EEPROM_CHIP_ADDRESS, 0X10);
    tmp_LSB = i2c_eeprom_read_uint8_t(EEPROM_CHIP_ADDRESS, 0X11);

    tmp_LSB = tmp_LSB >> 4;

    tmp = (tmp_MSB << 4) | tmp_LSB;

    if (tmp & (1 << 11))
        // Check for 2's complement negative number
        tmp = tmp | 0XF000; // Set left bits to one for 2's complement
                            // conversion of negitive number

    return tmp;
}

static int16_t get_c1()
{
    int16_t tmp;
    uint8_t tmp_MSB, tmp_LSB;

    tmp_MSB = i2c_eeprom_read_uint8_t(EEPROM_CHIP_ADDRESS, 0X11);
    tmp_LSB = i2c_eeprom_read_uint8_t(EEPROM_CHIP_ADDRESS, 0X12);

    tmp_MSB = tmp_MSB & 0XF;
    tmp     = (tmp_MSB << 8) | tmp_LSB;

    if (tmp & (1 << 11))
        // Check for 2's complement negative number
        tmp = tmp | 0XF000; // Set left bits to one for 2's complement
                            // conversion of negitive number

    return tmp;
}

static int32_t get_c00()
{
    int32_t tmp;
    uint8_t tmp_MSB, tmp_LSB, tmp_XLSB;

    tmp_MSB  = i2c_eeprom_read_uint8_t(EEPROM_CHIP_ADDRESS, 0X13);
    tmp_LSB  = i2c_eeprom_read_uint8_t(EEPROM_CHIP_ADDRESS, 0X14);
    tmp_XLSB = i2c_eeprom_read_uint8_t(EEPROM_CHIP_ADDRESS, 0X15);

    tmp_XLSB = tmp_XLSB >> 4;

    tmp = (tmp_MSB << 8) | tmp_LSB;
    tmp = (tmp << 4) | tmp_XLSB;

    tmp = (uint32_t)tmp_MSB << 12 | (uint32_t)tmp_LSB << 4 |
          (uint32_t)tmp_XLSB >> 4;

    if (tmp & (1 << 19))
        tmp = tmp | 0XFFF00000; // Set left bits to one for 2's complement
                                // conversion of negitive number

    return tmp;
}

static int32_t get_c10()
{
    int32_t tmp;
    uint8_t tmp_MSB, tmp_LSB, tmp_XLSB;

    tmp_MSB  = i2c_eeprom_read_uint8_t(EEPROM_CHIP_ADDRESS, 0X15); // 4 bits
    tmp_LSB  = i2c_eeprom_read_uint8_t(EEPROM_CHIP_ADDRESS, 0X16); // 8 bits
    tmp_XLSB = i2c_eeprom_read_uint8_t(EEPROM_CHIP_ADDRESS, 0X17); // 8 bits

    tmp_MSB = tmp_MSB & 0b00001111;

    tmp = (tmp_MSB << 4) | tmp_LSB;
    tmp = (tmp << 8) | tmp_XLSB;

    tmp = (uint32_t)tmp_MSB << 16 | (uint32_t)tmp_LSB << 8 | (uint32_t)tmp_XLSB;

    if (tmp & (1 << 19))
        tmp = tmp | 0XFFF00000; // Set left bits to one for 2's complement
                                // conversion of negitive number

    return tmp;
}

static int16_t get_c01()
{
    int16_t tmp;
    uint8_t tmp_MSB, tmp_LSB;

    tmp_MSB = i2c_eeprom_read_uint8_t(EEPROM_CHIP_ADDRESS, 0X18);
    tmp_LSB = i2c_eeprom_read_uint8_t(EEPROM_CHIP_ADDRESS, 0X19);

    tmp = (tmp_MSB << 8) | tmp_LSB;
    return tmp;
}

static int16_t get_c11()
{
    int16_t tmp;
    uint8_t tmp_MSB, tmp_LSB;

    tmp_MSB = i2c_eeprom_read_uint8_t(EEPROM_CHIP_ADDRESS, 0X1A);
    tmp_LSB = i2c_eeprom_read_uint8_t(EEPROM_CHIP_ADDRESS, 0X1B);

    tmp = (tmp_MSB << 8) | tmp_LSB;
    return tmp;
}

static int16_t get_c20()
{
    int16_t tmp;
    uint8_t tmp_MSB, tmp_LSB;

    tmp_MSB = i2c_eeprom_read_uint8_t(EEPROM_CHIP_ADDRESS, 0X1C);
    tmp_LSB = i2c_eeprom_read_uint8_t(EEPROM_CHIP_ADDRESS, 0X1D);

    tmp = (tmp_MSB << 8) | tmp_LSB;
    return tmp;
}

static int16_t get_c21()
{
    int16_t tmp;
    uint8_t tmp_MSB, tmp_LSB;

    tmp_MSB = i2c_eeprom_read_uint8_t(EEPROM_CHIP_ADDRESS, 0X1E);
    tmp_LSB = i2c_eeprom_read_uint8_t(EEPROM_CHIP_ADDRESS, 0X1F);

    tmp = (tmp_MSB << 8) | tmp_LSB;
    return tmp;
}

static int16_t get_c30()
{
    int16_t tmp;
    uint8_t tmp_MSB, tmp_LSB;

    tmp_MSB = i2c_eeprom_read_uint8_t(EEPROM_CHIP_ADDRESS, 0X20);
    tmp_LSB = i2c_eeprom_read_uint8_t(EEPROM_CHIP_ADDRESS, 0X21);

    tmp = (tmp_MSB << 8) | tmp_LSB;
    return tmp;
    //  printf("tmp: %d\n", tmp);
}

void spl06_init(void)
{
    uint8_t tmp, data;
    int32_t ret = sensor_i2c_open(SPL06_I2C_PORT, EEPROM_CHIP_ADDRESS, I2C_BUS_BIT_RATES_100K, 0);
    if (ret) {
        LOGE("SENSOR", "sensor i2c open failed, ret:%d\n", ret);
        return;
    }
    aos_msleep(500);

    // printf("\nDevice Reset\n");
    // i2c_eeprom_write_uint8_t(EEPROM_CHIP_ADDRESS, 0X0C, 0b1001);
    // aos_msleep(1000);

    tmp = i2c_eeprom_read_uint8_t(EEPROM_CHIP_ADDRESS, 0x0D);
    //  printf("ID: %d\n", tmp);
    i2c_eeprom_write_uint8_t(EEPROM_CHIP_ADDRESS, 0X06,
                             0x03); // Pressure 8x oversampling
    i2c_eeprom_write_uint8_t(EEPROM_CHIP_ADDRESS, 0X07,
                             0X83); // Temperature 8x oversampling
    i2c_eeprom_write_uint8_t(
        EEPROM_CHIP_ADDRESS, 0X08,
        0B0111); // continuous temp and pressure measurement
    // i2c_eeprom_write_uint8_t(EEPROM_CHIP_ADDRESS, 0X08, 0B0001); // standby
    // pressure measurement
    i2c_eeprom_write_uint8_t(EEPROM_CHIP_ADDRESS, 0X09,
                             0X00); // FIFO Pressure measurement
}

void spl06_getdata(spl06_data_t *sp)
{
    uint8_t tmp;
    int32_t c00, c10;
    int16_t c0, c1, c01, c11, c20, c21, c30;

    // Serial.println("\nDevice Reset\n");
    // i2c_eeprom_write_uint8_t(EEPROM_CHIP_ADDRESS, 0x0C, 0b1001);
    // delay(1000);

    tmp    = i2c_eeprom_read_uint8_t(EEPROM_CHIP_ADDRESS, 0x0D);
    sp->id = tmp;
    tmp    = i2c_eeprom_read_uint8_t(EEPROM_CHIP_ADDRESS, 0x06);
    tmp    = i2c_eeprom_read_uint8_t(EEPROM_CHIP_ADDRESS, 0x07);
    tmp    = i2c_eeprom_read_uint8_t(EEPROM_CHIP_ADDRESS, 0x08);
    tmp    = i2c_eeprom_read_uint8_t(EEPROM_CHIP_ADDRESS, 0x09);
    tmp    = i2c_eeprom_read_uint8_t(EEPROM_CHIP_ADDRESS, 0x0A);
    tmp    = i2c_eeprom_read_uint8_t(EEPROM_CHIP_ADDRESS, 0x0B);

    c0              = get_c0();
    c1              = get_c1();
    c00             = get_c00();
    c10             = get_c10();
    c01             = get_c01();
    c11             = get_c11();
    c20             = get_c20();
    c21             = get_c21();
    c30             = get_c30();
    int32_t traw    = get_traw();
    double  traw_sc = (double)traw / get_temperature_scale_factor();
    // printf("traw_sc: %0.2f\n", traw_sc);

    sp->Ctemp = (double)c0 * 0.5f + (double)c1 * traw_sc;
    sp->Ftemp = (sp->Ctemp * 9 / 5) + 32;

    int32_t praw = get_praw();

    double praw_sc = (double)(praw) / get_pressure_scale_factor();

    double pcomp =
        (double)(c00) +
        praw_sc * ((double)(c10) +
                   praw_sc * ((double)(c20) + praw_sc * (double)(c30))) +
        traw_sc * (double)(c01) +
        traw_sc * praw_sc * ((double)(c11) + praw_sc * (double)(c21));

    sp->pressure = pcomp / 100; // convert to mb

    // double local_pressure = 1010.5; // Look up local sea level pressure on
    // google
    double local_pressure =
        1011.1; // Look up local sea level pressure on google // Local pressure
                // from airport website 8/22
    // printf("Local Airport Sea Level Pressure: %0.2f mb\n", local_pressure);
    sp->altitude = get_altitude(pcomp, local_pressure);
}

void spl06_deinit(void)
{
    int32_t ret = sensor_i2c_close(SPL06_I2C_PORT);
    if (ret) {
        LOGE("SENSOR", "sensor i2c close failed, ret:%d\n", ret);
        return;
    }
}