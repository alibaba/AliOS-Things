/*
* Copyright (C) 2015-2017 Alibaba Group Holding Limited
*
*
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <aos/aos.h>
#include <vfs_conf.h>
#include <vfs_err.h>
#include <vfs_register.h>
#include <hal/base.h>
#include "common.h"
#include "hal/sensor.h"


#define BMP280_TEMPERATURE_CALIB_DIG_T1_LSB_REG         (0x88)
#define BMP280_TEMPERATURE_CALIB_DIG_T1_MSB_REG         (0x89)
#define BMP280_TEMPERATURE_CALIB_DIG_T2_LSB_REG         (0x8A)
#define BMP280_TEMPERATURE_CALIB_DIG_T2_MSB_REG         (0x8B)
#define BMP280_TEMPERATURE_CALIB_DIG_T3_LSB_REG         (0x8C)
#define BMP280_TEMPERATURE_CALIB_DIG_T3_MSB_REG         (0x8D)
#define BMP280_PRESSURE_CALIB_DIG_P1_LSB_REG            (0x8E)
#define BMP280_PRESSURE_CALIB_DIG_P1_MSB_REG            (0x8F)
#define BMP280_PRESSURE_CALIB_DIG_P2_LSB_REG            (0x90)
#define BMP280_PRESSURE_CALIB_DIG_P2_MSB_REG            (0x91)
#define BMP280_PRESSURE_CALIB_DIG_P3_LSB_REG            (0x92)
#define BMP280_PRESSURE_CALIB_DIG_P3_MSB_REG            (0x93)
#define BMP280_PRESSURE_CALIB_DIG_P4_LSB_REG            (0x94)
#define BMP280_PRESSURE_CALIB_DIG_P4_MSB_REG            (0x95)
#define BMP280_PRESSURE_CALIB_DIG_P5_LSB_REG            (0x96)
#define BMP280_PRESSURE_CALIB_DIG_P5_MSB_REG            (0x97)
#define BMP280_PRESSURE_CALIB_DIG_P6_LSB_REG            (0x98)
#define BMP280_PRESSURE_CALIB_DIG_P6_MSB_REG            (0x99)
#define BMP280_PRESSURE_CALIB_DIG_P7_LSB_REG            (0x9A)
#define BMP280_PRESSURE_CALIB_DIG_P7_MSB_REG            (0x9B)
#define BMP280_PRESSURE_CALIB_DIG_P8_LSB_REG            (0x9C)
#define BMP280_PRESSURE_CALIB_DIG_P8_MSB_REG            (0x9D)
#define BMP280_PRESSURE_CALIB_DIG_P9_LSB_REG            (0x9E)
#define BMP280_PRESSURE_CALIB_DIG_P9_MSB_REG            (0x9F)


#define BMP280_CHIP_ID_REG                              (0xD0)
#define BMP280_RST_REG                                  (0xE0)
#define BMP280_STAT_REG                                 (0xF3)
#define BMP280_CTRL_MEAS_REG                            (0xF4)
#define BMP280_CONFIG_REG                               (0xF5)
#define BMP280_PRESSURE_MSB_REG                         (0xF7)
#define BMP280_PRESSURE_LSB_REG                         (0xF8)
#define BMP280_PRESSURE_XLSB_REG                        (0xF9)
#define BMP280_TEMPERATURE_MSB_REG                      (0xFA)
#define BMP280_TEMPERATURE_LSB_REG                      (0xFB)
#define BMP280_TEMPERATURE_XLSB_REG                     (0xFC)  


#define BMP280_SHIFT_BY_01_BIT                          (1)
#define BMP280_SHIFT_BY_02_BITS                         (2)
#define BMP280_SHIFT_BY_03_BITS                         (3)
#define BMP280_SHIFT_BY_04_BITS                         (4)
#define BMP280_SHIFT_BY_05_BITS                         (5)
#define BMP280_SHIFT_BY_08_BITS                         (8)
#define BMP280_SHIFT_BY_11_BITS                         (11)
#define BMP280_SHIFT_BY_12_BITS                         (12)
#define BMP280_SHIFT_BY_13_BITS                         (13)
#define BMP280_SHIFT_BY_14_BITS                         (14)
#define BMP280_SHIFT_BY_15_BITS                         (15)
#define BMP280_SHIFT_BY_16_BITS                         (16)
#define BMP280_SHIFT_BY_17_BITS                         (17)
#define BMP280_SHIFT_BY_18_BITS                         (18)
#define BMP280_SHIFT_BY_19_BITS                         (19)
#define BMP280_SHIFT_BY_25_BITS                         (25)
#define BMP280_SHIFT_BY_31_BITS                         (31)
#define BMP280_SHIFT_BY_33_BITS                         (33)
#define BMP280_SHIFT_BY_35_BITS                         (35)
#define BMP280_SHIFT_BY_47_BITS                         (47)


#define BMP280_PRESSURE_TEMPERATURE_CALIB_DATA_LENGTH   (24)
#define BMP280_GEN_READ_WRITE_DATA_LENGTH               (1)
#define BMP280_REGISTER_READ_DELAY                      (1)
#define BMP280_TEMPERATURE_DATA_LENGTH                  (3)
#define BMP280_PRESSURE_DATA_LENGTH                     (3)
#define BMP280_ALL_DATA_FRAME_LENGTH                    (6)
#define BMP280_INIT_VALUE                               (0)
#define BMP280_CHIP_ID_READ_COUNT                       (5)
#define BMP280_CHIP_ID_READ_SUCCESS                     (0)
#define BMP280_CHIP_ID_READ_FAIL                        ((int8_t)-1)
#define BMP280_INVALID_DATA                             (0)




#define BMP280_TEMPERATURE_DATA_SIZE                    (3)
#define BMP280_PRESSURE_DATA_SIZE                       (3)
#define BMP280_DATA_FRAME_SIZE                          (6)
#define BMP280_CALIB_DATA_SIZE                          (24)

#define BMP280_TEMPERATURE_MSB_DATA                     (0)
#define BMP280_TEMPERATURE_LSB_DATA                     (1)
#define BMP280_TEMPERATURE_XLSB_DATA                    (2)



#define BMP280_I2C_ADDRESS1                             (0x76)
#define BMP280_I2C_ADDRESS2                             (0x77)


#define BMP280_SLEEP_MODE                               (0x00)
#define BMP280_FORCED_MODE                              (0x01)
#define BMP280_NORMAL_MODE                              (0x03)
#define BMP280_SOFT_RESET_CODE                          (0xB6)



#define BMP280_STANDBY_TIME_1_MS                        (0x00)
#define BMP280_STANDBY_TIME_63_MS                       (0x01)
#define BMP280_STANDBY_TIME_125_MS                      (0x02)
#define BMP280_STANDBY_TIME_250_MS                      (0x03)
#define BMP280_STANDBY_TIME_500_MS                      (0x04)
#define BMP280_STANDBY_TIME_1000_MS                     (0x05)
#define BMP280_STANDBY_TIME_2000_MS                     (0x06)
#define BMP280_STANDBY_TIME_4000_MS                     (0x07)


#define BMP280_OVERSAMP_SKIPPED                         (0x00)
#define BMP280_OVERSAMP_1X                              (0x01)
#define BMP280_OVERSAMP_2X                              (0x02)
#define BMP280_OVERSAMP_4X                              (0x03)
#define BMP280_OVERSAMP_8X                              (0x04)
#define BMP280_OVERSAMP_16X                             (0x05)

#define BMP280_FILTER_COEFF_OFF                         (0x00)
#define BMP280_FILTER_COEFF_2                           (0x01)
#define BMP280_FILTER_COEFF_4                           (0x02)
#define BMP280_FILTER_COEFF_8                           (0x03)
#define BMP280_FILTER_COEFF_16                          (0x04)


#define BMP280_ULTRA_LOW_POWER_MODE                     (0x00)
#define BMP280_LOW_POWER_MODE                           (0x01)
#define BMP280_STANDARD_RESOLUTION_MODE                 (0x02)
#define BMP280_HIGH_RESOLUTION_MODE                     (0x03)
#define BMP280_ULTRA_HIGH_RESOLUTION_MODE               (0x04)

#define BMP280_ULTRALOWPOWER_OVERSAMP_PRESSURE          BMP280_OVERSAMP_1X
#define BMP280_ULTRALOWPOWER_OVERSAMP_TEMPERATURE       BMP280_OVERSAMP_1X

#define BMP280_LOWPOWER_OVERSAMP_PRESSURE                BMP280_OVERSAMP_2X
#define BMP280_LOWPOWER_OVERSAMP_TEMPERATURE            BMP280_OVERSAMP_1X

#define BMP280_STANDARDRESOLUTION_OVERSAMP_PRESSURE     BMP280_OVERSAMP_4X
#define BMP280_STANDARDRESOLUTION_OVERSAMP_TEMPERATURE  BMP280_OVERSAMP_1X

#define BMP280_HIGHRESOLUTION_OVERSAMP_PRESSURE         BMP280_OVERSAMP_8X
#define BMP280_HIGHRESOLUTION_OVERSAMP_TEMPERATURE      BMP280_OVERSAMP_1X

#define BMP280_ULTRAHIGHRESOLUTION_OVERSAMP_PRESSURE    BMP280_OVERSAMP_16X
#define BMP280_ULTRAHIGHRESOLUTION_OVERSAMP_TEMPERATURE BMP280_OVERSAMP_2X


#define BMP280_STATUS_REG_MEASURING__POS                (3)
#define BMP280_STATUS_REG_MEASURING__MSK                (0x08)
#define BMP280_STATUS_REG_MEASURING__LEN                (1)
#define BMP280_STATUS_REG_MEASURING__REG                (BMP280_STAT_REG)

#define BMP280_STATUS_REG_IM_UPDATE__POS                (0)
#define BMP280_STATUS_REG_IM_UPDATE__MSK                (0x01)
#define BMP280_STATUS_REG_IM_UPDATE__LEN                (1)
#define BMP280_STATUS_REG_IM_UPDATE__REG                (BMP280_STAT_REG)


#define BMP280_CTRL_MEAS_REG_OVERSAMP_TEMPERATURE__POS  (5)
#define BMP280_CTRL_MEAS_REG_OVERSAMP_TEMPERATURE__MSK  (0xE0)
#define BMP280_CTRL_MEAS_REG_OVERSAMP_TEMPERATURE__LEN  (3)
#define BMP280_CTRL_MEAS_REG_OVERSAMP_TEMPERATURE__REG  (BMP280_CTRL_MEAS_REG)
    

#define BMP280_CTRL_MEAS_REG_OVERSAMP_PRESSURE__POS     (2)
#define BMP280_CTRL_MEAS_REG_OVERSAMP_PRESSURE__MSK     (0x1C)
#define BMP280_CTRL_MEAS_REG_OVERSAMP_PRESSURE__LEN     (3)
#define BMP280_CTRL_MEAS_REG_OVERSAMP_PRESSURE__REG     (BMP280_CTRL_MEAS_REG)

#define BMP280_CTRL_MEAS_REG_POWER_MODE__POS            (0)
#define BMP280_CTRL_MEAS_REG_POWER_MODE__MSK            (0x03)
#define BMP280_CTRL_MEAS_REG_POWER_MODE__LEN            (2)
#define BMP280_CTRL_MEAS_REG_POWER_MODE__REG            (BMP280_CTRL_MEAS_REG)


#define BMP280_CONFIG_REG_STANDBY_DURN__POS             (5)
#define BMP280_CONFIG_REG_STANDBY_DURN__MSK             (0xE0)
#define BMP280_CONFIG_REG_STANDBY_DURN__LEN             (3)
#define BMP280_CONFIG_REG_STANDBY_DURN__REG             (BMP280_CONFIG_REG)

#define BMP280_CONFIG_REG_FILTER__POS                   (2)
#define BMP280_CONFIG_REG_FILTER__MSK                   (0x1C)
#define BMP280_CONFIG_REG_FILTER__LEN                   (3)
#define BMP280_CONFIG_REG_FILTER__REG                   (BMP280_CONFIG_REG)


#define BMP280_CONFIG_REG_SPI3_ENABLE__POS              (0)
#define BMP280_CONFIG_REG_SPI3_ENABLE__MSK              (0x01)
#define BMP280_CONFIG_REG_SPI3_ENABLE__LEN              (1)
#define BMP280_CONFIG_REG_SPI3_ENABLE__REG              (BMP280_CONFIG_REG)


#define BMP280_PRESSURE_XLSB_REG_DATA__POS              (4)
#define BMP280_PRESSURE_XLSB_REG_DATA__MSK              (0xF0)
#define BMP280_PRESSURE_XLSB_REG_DATA__LEN              (4)
#define BMP280_PRESSURE_XLSB_REG_DATA__REG              (BMP280_PRESSURE_XLSB_REG)

#define BMP280_TEMPERATURE_XLSB_REG_DATA__POS           (4)
#define BMP280_TEMPERATURE_XLSB_REG_DATA__MSK           (0xF0)
#define BMP280_TEMPERATURE_XLSB_REG_DATA__LEN           (4)
#define BMP280_TEMPERATURE_XLSB_REG_DATA__REG           (BMP280_TEMPERATURE_XLSB_REG)

#define BMP280_TEMPERATURE_MSB_DATA                     (0)
#define BMP280_TEMPERATURE_LSB_DATA                     (1)
#define BMP280_TEMPERATURE_XLSB_DATA                    (2)

#define BMP280_PRESSURE_MSB_DATA                        (0)
#define BMP280_PRESSURE_LSB_DATA                        (1)
#define BMP280_PRESSURE_XLSB_DATA                       (2)

#define BMP280_DATA_FRAME_PRESSURE_MSB_BYTE             (0)
#define BMP280_DATA_FRAME_PRESSURE_LSB_BYTE             (1)
#define BMP280_DATA_FRAME_PRESSURE_XLSB_BYTE            (2)
#define BMP280_DATA_FRAME_TEMPERATURE_MSB_BYTE          (3)
#define BMP280_DATA_FRAME_TEMPERATURE_LSB_BYTE          (4)
#define BMP280_DATA_FRAME_TEMPERATURE_XLSB_BYTE         (5) 

#define BMP280_TEMPERATURE_CALIB_DIG_T1_LSB             (0)
#define BMP280_TEMPERATURE_CALIB_DIG_T1_MSB             (1)
#define BMP280_TEMPERATURE_CALIB_DIG_T2_LSB             (2)
#define BMP280_TEMPERATURE_CALIB_DIG_T2_MSB             (3)
#define BMP280_TEMPERATURE_CALIB_DIG_T3_LSB             (4)
#define BMP280_TEMPERATURE_CALIB_DIG_T3_MSB             (5)
#define BMP280_PRESSURE_CALIB_DIG_P1_LSB                (6)
#define BMP280_PRESSURE_CALIB_DIG_P1_MSB                (7)
#define BMP280_PRESSURE_CALIB_DIG_P2_LSB                (8)
#define BMP280_PRESSURE_CALIB_DIG_P2_MSB                (9)
#define BMP280_PRESSURE_CALIB_DIG_P3_LSB                (10)
#define BMP280_PRESSURE_CALIB_DIG_P3_MSB                (11)
#define BMP280_PRESSURE_CALIB_DIG_P4_LSB                (12)
#define BMP280_PRESSURE_CALIB_DIG_P4_MSB                (13)
#define BMP280_PRESSURE_CALIB_DIG_P5_LSB                (14)
#define BMP280_PRESSURE_CALIB_DIG_P5_MSB                (15)
#define BMP280_PRESSURE_CALIB_DIG_P6_LSB                (16)
#define BMP280_PRESSURE_CALIB_DIG_P6_MSB                (17)
#define BMP280_PRESSURE_CALIB_DIG_P7_LSB                (18)
#define BMP280_PRESSURE_CALIB_DIG_P7_MSB                (19)
#define BMP280_PRESSURE_CALIB_DIG_P8_LSB                (20)
#define BMP280_PRESSURE_CALIB_DIG_P8_MSB                (21)
#define BMP280_PRESSURE_CALIB_DIG_P9_LSB                (22)
#define BMP280_PRESSURE_CALIB_DIG_P9_MSB                (23)

#define BMP280_SOFT_RESRT_VALUE                         (0XB6)

#define BMP280_I2C_SLAVE_ADDR_LOW                       (0X76)
#define BMP280_I2C_SLAVE_ADDR_HIGN                      (0X77)

#define BMP280_DEFAULT_ODR_1HZ                          (1)

#define BMP280_BIT(x)                                   ((uint8_t)(x))
#define BMP280_CHIP_ID_VAL                              BMP280_BIT(0X58)
#define BMP280_I2C_ADDR_TRANS(n)                        ((n)<<1)  
#define BMP280_I2C_ADDR                                 BMP280_I2C_ADDR_TRANS(BMP280_I2C_SLAVE_ADDR_LOW)


#define BMP280_GET_BITSLICE(regvar, bitname)            ((regvar & bitname##__MSK) >> bitname##__POS)
#define BMP280_SET_BITSLICE(regvar, bitname, val)       ((regvar & ~bitname##__MSK) | ((val<<bitname##__POS)&bitname##__MSK))


typedef struct bmp280_calib_param_t {
    uint16_t    dig_T1;
    int16_t     dig_T2;
    int16_t     dig_T3;
    uint16_t    dig_P1;
    int16_t     dig_P2;
    int16_t     dig_P3;
    int16_t     dig_P4;
    int16_t     dig_P5;
    int16_t     dig_P6;
    int16_t     dig_P7;
    int16_t     dig_P8;
    int16_t     dig_P9;
    int         t_fine;
}bmp280_calib_param_t;

typedef struct bmp280_device_cfg_t {
    uint8_t         odr;
    uint8_t         mode_filter;
    uint8_t         mode_baro;
    uint8_t         mode_temp;
    uint8_t         mode_power;
    uint8_t         oversamp_temp;
    uint8_t         oversamp_baro;
}bmp280_device_cfg_t;


static bmp280_device_cfg_t    g_bmp280_dev_cfg;
static bmp280_calib_param_t   g_bmp280_calib_table;


i2c_dev_t bmp280_ctx = {
    .port = 1,
    .config.address_width = 8,
    .config.freq = 400000,
    .config.dev_addr = BMP280_I2C_ADDR,
};


static int  drv_baro_bosch_bmp280_get_calib_param(i2c_dev_t* drv)
{
    int     ret = 0;
    uint8_t a_data_u8[BMP280_CALIB_DATA_SIZE] = {0};

    ret = sensor_i2c_read(drv,BMP280_TEMPERATURE_CALIB_DIG_T1_LSB_REG,
        a_data_u8,BMP280_PRESSURE_TEMPERATURE_CALIB_DATA_LENGTH,I2C_OP_RETRIES);
    
    if(unlikely(ret)){
        return ret;
    }
    
    g_bmp280_calib_table.dig_T1 = (uint16_t)((((uint16_t)((uint8_t)a_data_u8[BMP280_TEMPERATURE_CALIB_DIG_T1_MSB]))
            << BMP280_SHIFT_BY_08_BITS) | a_data_u8[BMP280_TEMPERATURE_CALIB_DIG_T1_LSB]);
    g_bmp280_calib_table.dig_T2 = (int16_t)((((int16_t)((int8_t)a_data_u8[BMP280_TEMPERATURE_CALIB_DIG_T2_MSB]))
            << BMP280_SHIFT_BY_08_BITS) | a_data_u8[BMP280_TEMPERATURE_CALIB_DIG_T2_LSB]);
    g_bmp280_calib_table.dig_T3 = (int16_t)((((int16_t)((int8_t)a_data_u8[BMP280_TEMPERATURE_CALIB_DIG_T3_MSB]))
            << BMP280_SHIFT_BY_08_BITS) | a_data_u8[BMP280_TEMPERATURE_CALIB_DIG_T3_LSB]);
    g_bmp280_calib_table.dig_P1 = (uint16_t)((((uint16_t)((uint8_t)a_data_u8[BMP280_PRESSURE_CALIB_DIG_P1_MSB]))
            << BMP280_SHIFT_BY_08_BITS) | a_data_u8[BMP280_PRESSURE_CALIB_DIG_P1_LSB]);
    g_bmp280_calib_table.dig_P2 = (int16_t)((((int16_t)((int8_t)a_data_u8[BMP280_PRESSURE_CALIB_DIG_P2_MSB]))
            << BMP280_SHIFT_BY_08_BITS) | a_data_u8[BMP280_PRESSURE_CALIB_DIG_P2_LSB]);
    g_bmp280_calib_table.dig_P3 = (int16_t)((((int16_t)((int8_t)a_data_u8[BMP280_PRESSURE_CALIB_DIG_P3_MSB]))
            << BMP280_SHIFT_BY_08_BITS) | a_data_u8[BMP280_PRESSURE_CALIB_DIG_P3_LSB]);
    g_bmp280_calib_table.dig_P4 = (int16_t)((((int16_t)((int8_t)a_data_u8[BMP280_PRESSURE_CALIB_DIG_P4_MSB]))
            << BMP280_SHIFT_BY_08_BITS) | a_data_u8[BMP280_PRESSURE_CALIB_DIG_P4_LSB]);
    g_bmp280_calib_table.dig_P5 = (int16_t)((((int16_t)((int8_t)a_data_u8[BMP280_PRESSURE_CALIB_DIG_P5_MSB]))
            << BMP280_SHIFT_BY_08_BITS) | a_data_u8[BMP280_PRESSURE_CALIB_DIG_P5_LSB]);
    g_bmp280_calib_table.dig_P6 = (int16_t)((((int16_t)((int8_t)a_data_u8[BMP280_PRESSURE_CALIB_DIG_P6_MSB]))
            << BMP280_SHIFT_BY_08_BITS) | a_data_u8[BMP280_PRESSURE_CALIB_DIG_P6_LSB]);
    g_bmp280_calib_table.dig_P7 = (int16_t)((((int16_t)((int8_t)a_data_u8[BMP280_PRESSURE_CALIB_DIG_P7_MSB]))
            << BMP280_SHIFT_BY_08_BITS) | a_data_u8[BMP280_PRESSURE_CALIB_DIG_P7_LSB]);
    g_bmp280_calib_table.dig_P8 = (int16_t)((((int16_t)((int8_t)a_data_u8[BMP280_PRESSURE_CALIB_DIG_P8_MSB]))
            << BMP280_SHIFT_BY_08_BITS) | a_data_u8[BMP280_PRESSURE_CALIB_DIG_P8_LSB]);
    g_bmp280_calib_table.dig_P9 = (int16_t)((((int16_t)((int8_t)a_data_u8[BMP280_PRESSURE_CALIB_DIG_P9_MSB]))
            << BMP280_SHIFT_BY_08_BITS) | a_data_u8[BMP280_PRESSURE_CALIB_DIG_P9_LSB]);

    return 0;
}

static int drv_baro_bosch_bmp280_validate_id(i2c_dev_t* drv, uint8_t id_value)
{
    int     ret = 0;
    uint8_t value = 0;

    if(drv == NULL){
        return -1;
    }
    
    ret = sensor_i2c_read(drv, BMP280_CHIP_ID_REG, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }
    
    if (id_value != value){
        return -1;
    }
    return 0;
}



static int drv_baro_bosch_bmp280_set_work_mode(i2c_dev_t* drv,uint8_t mode)
{
    uint8_t ret = 0;
    uint8_t value = 0;
    uint8_t temp = 0;
    uint8_t baro = 0;

    switch (mode) {
        case BMP280_ULTRA_LOW_POWER_MODE:
            temp = BMP280_OVERSAMP_1X;
            baro = BMP280_OVERSAMP_1X;
            break;

        case BMP280_LOW_POWER_MODE:
            temp = BMP280_OVERSAMP_2X;
            baro = BMP280_OVERSAMP_2X;
            break;

        case BMP280_STANDARD_RESOLUTION_MODE:
            temp = BMP280_OVERSAMP_4X;
            baro = BMP280_OVERSAMP_4X;
            break;

        case BMP280_HIGH_RESOLUTION_MODE:
            temp = BMP280_OVERSAMP_8X;
            baro = BMP280_OVERSAMP_8X;
            break;

        case BMP280_ULTRA_HIGH_RESOLUTION_MODE:
            temp = BMP280_OVERSAMP_16X;
            baro = BMP280_OVERSAMP_16X;
            break;

        default:
            return -1;

    }

    ret = sensor_i2c_read(drv, BMP280_CTRL_MEAS_REG, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }
    value = BMP280_SET_BITSLICE(value,BMP280_CTRL_MEAS_REG_OVERSAMP_PRESSURE,baro);
    value = BMP280_SET_BITSLICE(value,BMP280_CTRL_MEAS_REG_OVERSAMP_TEMPERATURE,temp);
    
    ret = sensor_i2c_write(drv, BMP280_CTRL_MEAS_REG, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }

    return ret;
}




static int drv_baro_bosch_bmp280_set_power_mode(i2c_dev_t* drv, dev_power_mode_e mode)
{
    int     ret = 0;
    uint8_t value = 0x00;
    uint8_t dev_mode;

    
    switch(mode){
        case DEV_POWER_OFF:
        case DEV_SLEEP:{
            dev_mode = (uint8_t)BMP280_SLEEP_MODE;
            break;
            }
        case DEV_POWER_ON:{
            dev_mode = (uint8_t)BMP280_NORMAL_MODE;
            break;
            }
        default:return -1;
    }

    ret = sensor_i2c_read(drv, BMP280_CTRL_MEAS_REG, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }
    
    value = BMP280_SET_BITSLICE(value,BMP280_CTRL_MEAS_REG_POWER_MODE,dev_mode);
    ret = sensor_i2c_write(drv, BMP280_CTRL_MEAS_REG, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }
    
    return 0;
}


static uint8_t drv_baro_bosch_bmp280_hz2odr(int hz)
{
    if(hz > 80)
        return BMP280_STANDBY_TIME_1_MS;
    else if(hz > 13)
        return BMP280_STANDBY_TIME_63_MS;
    else if(hz > 7)
        return BMP280_STANDBY_TIME_125_MS;
    else if(hz > 3)
        return BMP280_STANDBY_TIME_250_MS;
    else
        return BMP280_STANDBY_TIME_500_MS;
}


static int drv_baro_bosch_bmp280_set_odr(i2c_dev_t* drv, uint8_t odr)
{
    int     ret = 0;
    uint8_t v_data_u8 = 0;

    ret = sensor_i2c_read(drv,BMP280_CONFIG_REG_STANDBY_DURN__REG,
                            &v_data_u8,I2C_DATA_LEN,I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }

    v_data_u8 = BMP280_SET_BITSLICE(v_data_u8,BMP280_CONFIG_REG_STANDBY_DURN,odr);
    ret = sensor_i2c_write(drv,BMP280_CONFIG_REG_STANDBY_DURN__REG,
                            &v_data_u8,I2C_DATA_LEN,I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }
    
    return ret;
}


static int  drv_baro_bosch_bmp280_soft_reset(i2c_dev_t* drv)
{
    int     ret = 0;
    uint8_t v_data_u8 = BMP280_SOFT_RESRT_VALUE;
    
    ret = sensor_i2c_write(drv,BMP280_RST_REG,
                            &v_data_u8,I2C_DATA_LEN,I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }
    
    return ret;
}

static int drv_baro_bosch_bmp280_set_default_config(i2c_dev_t* drv)
{
    int     ret = 0;

    ret = drv_baro_bosch_bmp280_set_power_mode(drv, DEV_SLEEP);
    if(unlikely(ret)){
        return ret;
    }

    ret = drv_baro_bosch_bmp280_set_odr(drv, BMP280_DEFAULT_ODR_1HZ);
    if(unlikely(ret)){
        return ret;
    }

    return 0;
}


static int drv_baro_bosch_bmp280_read_uncomp_baro(i2c_dev_t* drv, barometer_data_t* pdata)
{
    int     ret = 0;
    uint8_t data[BMP280_PRESSURE_DATA_SIZE] = {0};

    ret = sensor_i2c_read(drv, BMP280_PRESSURE_MSB_REG, data, BMP280_PRESSURE_DATA_SIZE, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }
    
    pdata->p = (int)((((uint32_t)(data[BMP280_PRESSURE_MSB_DATA]))<< BMP280_SHIFT_BY_12_BITS)
            | (((uint32_t)(data[BMP280_PRESSURE_LSB_DATA]))<< BMP280_SHIFT_BY_04_BITS)
            | ((uint32_t)data[BMP280_PRESSURE_XLSB_DATA]>> BMP280_SHIFT_BY_04_BITS));
    return ret;
}


static int drv_baro_bosch_bmp280_compensate_baro( barometer_data_t* pdata)
{
    int      v_x1_u32r = 0;
    int      v_x2_u32r = 0;
    uint32_t comp_baro = 0;
    
    v_x1_u32r = (((int)g_bmp280_calib_table.t_fine)
            >>BMP280_SHIFT_BY_01_BIT) - (int)64000;

    v_x2_u32r = (((v_x1_u32r >> BMP280_SHIFT_BY_02_BITS)
            * (v_x1_u32r >> BMP280_SHIFT_BY_02_BITS))
            >> BMP280_SHIFT_BY_11_BITS)
            * ((int)g_bmp280_calib_table.dig_P6);
    v_x2_u32r = v_x2_u32r + ((v_x1_u32r *
            ((int)g_bmp280_calib_table.dig_P5))
            << BMP280_SHIFT_BY_01_BIT);
    v_x2_u32r = (v_x2_u32r >> BMP280_SHIFT_BY_02_BITS)
            + (((int)g_bmp280_calib_table.dig_P4)
            << BMP280_SHIFT_BY_16_BITS);

    v_x1_u32r = (((g_bmp280_calib_table.dig_P3
            * (((v_x1_u32r
            >> BMP280_SHIFT_BY_02_BITS) * (v_x1_u32r
            >> BMP280_SHIFT_BY_02_BITS))
            >> BMP280_SHIFT_BY_13_BITS))
            >> BMP280_SHIFT_BY_03_BITS)
            + ((((int)g_bmp280_calib_table.dig_P2)
            * v_x1_u32r)
            >> BMP280_SHIFT_BY_01_BIT))
            >> BMP280_SHIFT_BY_18_BITS;
    v_x1_u32r = ((((32768 + v_x1_u32r))
            * ((int)g_bmp280_calib_table.dig_P1))
            >> BMP280_SHIFT_BY_15_BITS);

    comp_baro = (((uint32_t)(((int)1048576) - pdata->p)
            - (v_x2_u32r >> BMP280_SHIFT_BY_12_BITS)))
            * 3125;

    if (comp_baro < 0x80000000){
        if (v_x1_u32r != 0){
            comp_baro = (comp_baro
                    << BMP280_SHIFT_BY_01_BIT)
                    / ((uint32_t)v_x1_u32r);
        }
        else{
            return -1;
        }
    }
    else if (v_x1_u32r != 0){
        comp_baro = (comp_baro / (uint32_t)v_x1_u32r) * 2;
   }
    else{
        return -1;
   }
    v_x1_u32r = (((int)g_bmp280_calib_table.dig_P9) * ((int)(
            ((comp_baro >> BMP280_SHIFT_BY_03_BITS)
            * (comp_baro >> BMP280_SHIFT_BY_03_BITS))
            >> BMP280_SHIFT_BY_13_BITS)))
            >> BMP280_SHIFT_BY_12_BITS;

    v_x2_u32r = (((int)(comp_baro >>
            BMP280_SHIFT_BY_02_BITS))
            * ((int)g_bmp280_calib_table.dig_P8))
            >> BMP280_SHIFT_BY_13_BITS;

    comp_baro = (uint32_t)((int)comp_baro + ((v_x1_u32r + v_x2_u32r
            + g_bmp280_calib_table.dig_P7)
            >> BMP280_SHIFT_BY_04_BITS));

    pdata->p = comp_baro;
    return 0;
}



static int drv_baro_bosch_bmp280_read_baro(i2c_dev_t* drv, barometer_data_t* pdata)
{
    int ret = 0;
    
    ret = drv_baro_bosch_bmp280_read_uncomp_baro(drv, pdata);
    if(unlikely(ret)){
        return ret;
    }
    ret = drv_baro_bosch_bmp280_compensate_baro(pdata);
    if(unlikely(ret)){
        return ret;
    }

    return 0;
}

static int drv_baro_bosch_bmp280_comp_temp(temperature_data_t* pdata)
{
    int v_x1_u32r = 0;
    int v_x2_u32r = 0;
    v_x1_u32r = ((((pdata->t >> BMP280_SHIFT_BY_03_BITS)
            - ((int)g_bmp280_calib_table.dig_T1
            << BMP280_SHIFT_BY_01_BIT)))
            * ((int)g_bmp280_calib_table.dig_T2))
            >> BMP280_SHIFT_BY_11_BITS;

    v_x2_u32r = (((((pdata->t >> BMP280_SHIFT_BY_04_BITS)
            - ((int)g_bmp280_calib_table.dig_T1))
            * ((pdata->t >> BMP280_SHIFT_BY_04_BITS)
            - ((int)g_bmp280_calib_table.dig_T1)))
            >> BMP280_SHIFT_BY_12_BITS)
            * ((int)g_bmp280_calib_table.dig_T3))
            >> BMP280_SHIFT_BY_14_BITS;
    
    g_bmp280_calib_table.t_fine = v_x1_u32r + v_x2_u32r;

    pdata->t = (g_bmp280_calib_table.t_fine * 5 + 128) >> BMP280_SHIFT_BY_08_BITS;
    return 0;
}



static int drv_baro_bosch_bmp280_cali_temp(i2c_dev_t* drv)
{
    int ret = 0;
    uint8_t data[BMP280_TEMPERATURE_DATA_SIZE] = {0};
    temperature_data_t temp;

    ret  = sensor_i2c_read(drv, BMP280_TEMPERATURE_MSB_REG, data, BMP280_TEMPERATURE_DATA_SIZE, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }

    temp.t = (int)((((uint32_t)(data[BMP280_TEMPERATURE_MSB_DATA]))<< BMP280_SHIFT_BY_12_BITS)
            | (((uint32_t)(data[BMP280_TEMPERATURE_LSB_DATA]))<< BMP280_SHIFT_BY_04_BITS)
            | ((uint32_t)data[BMP280_TEMPERATURE_XLSB_DATA]>> BMP280_SHIFT_BY_04_BITS)); 


    ret = drv_baro_bosch_bmp280_comp_temp(&temp);
    if(unlikely(ret)){
        return ret;
    }

    return 0;
}



static void drv_baro_bosch_bmp280_irq_handle(void)
{
    /* no handle so far */
}

static int drv_baro_bosch_bmp280_open(void)
{
    int ret = 0;

    /* set the default config for the sensor here */
    ret = drv_baro_bosch_bmp280_set_work_mode(&bmp280_ctx,BMP280_ULTRA_LOW_POWER_MODE);
    if(unlikely(ret)){
        return -1;
    }

    ret  =  drv_baro_bosch_bmp280_set_power_mode(&bmp280_ctx, DEV_POWER_ON);
    if(unlikely(ret)){
        return -1;
    }
    
    LOG("%s %s successfully \n", SENSOR_STR, __func__);
    return 0;

}

static int drv_baro_bosch_bmp280_close(void)
{
    int ret = 0;
    ret  = drv_baro_bosch_bmp280_set_power_mode(&bmp280_ctx, DEV_POWER_OFF);
    if(unlikely(ret)){
        return -1;
    }
    LOG("%s %s successfully \n", SENSOR_STR, __func__);
    return 0;
}

static int drv_baro_bosch_bmp280_read(void *buf, size_t len)
{
    int ret = 0;
    size_t size = 0;
    barometer_data_t* pdata = (barometer_data_t*)buf;
    
    if(buf == NULL){
        return -1;
    }
    size = sizeof(barometer_data_t);
    if(len < size){
        return -1;
    }
    
    ret = drv_baro_bosch_bmp280_cali_temp(&bmp280_ctx);
    if(unlikely(ret)){
        return -1;
    }

    ret = drv_baro_bosch_bmp280_read_baro(&bmp280_ctx, pdata);
    if(unlikely(ret)){
        return -1;
    }

    pdata->timestamp = aos_now_ms();
    
    return (int)size;
}

static int drv_baro_bosch_bmp280_write(const void *buf, size_t len)
{
    (void)buf;
    (void)len;
    return 0;
}

static int drv_baro_bosch_bmp280_ioctl(int cmd, unsigned long arg)
{
    int ret = 0;
    
    switch(cmd){
        case SENSOR_IOCTL_ODR_SET:{
            uint8_t odr = drv_baro_bosch_bmp280_hz2odr(arg);
            ret = drv_baro_bosch_bmp280_set_odr(&bmp280_ctx, odr);
            if(unlikely(ret)){
                return -1;
            }
        }break;
        case SENSOR_IOCTL_SET_POWER:{
            ret = drv_baro_bosch_bmp280_set_power_mode(&bmp280_ctx, arg);
            if(unlikely(ret)){
                return -1;
            }
        }break;
        case SENSOR_IOCTL_GET_INFO:{ 
            /* fill the dev info here */
            dev_sensor_info_t *info = (dev_sensor_info_t *)arg;
            info->model = "BMP280";
            info->range_max = 1100;
            info->range_min = 300;
            info->unit = pa;

        }break;
       
       default:break;
    }

    LOG("%s %s successfully \n", SENSOR_STR, __func__);
    return 0;
}

int drv_baro_bosch_bmp280_init(void)
{
    int ret = 0;
    sensor_obj_t sensor;

    /* fill the sensor obj parameters here */
    sensor.tag = TAG_DEV_BARO;
    sensor.path = dev_baro_path;
    sensor.io_port = I2C_PORT;
    sensor.open = drv_baro_bosch_bmp280_open;
    sensor.close = drv_baro_bosch_bmp280_close;
    sensor.read = drv_baro_bosch_bmp280_read;
    sensor.write = drv_baro_bosch_bmp280_write;
    sensor.ioctl = drv_baro_bosch_bmp280_ioctl;
    sensor.irq_handle = drv_baro_bosch_bmp280_irq_handle;

    ret = sensor_create_obj(&sensor);
    if(unlikely(ret)){
        return -1;
    }

    ret = drv_baro_bosch_bmp280_validate_id(&bmp280_ctx, BMP280_CHIP_ID_VAL);
    if(unlikely(ret)){
        return -1;
    }

    ret = drv_baro_bosch_bmp280_soft_reset(&bmp280_ctx);
    if(unlikely(ret)){
        return -1;
    }

    ret = drv_baro_bosch_bmp280_set_default_config(&bmp280_ctx);
    if(unlikely(ret)){
        return -1;
    }
    
    ret = drv_baro_bosch_bmp280_get_calib_param(&bmp280_ctx);
    if(unlikely(ret)){
        return -1;
    }

    LOG("%s %s successfully \n", SENSOR_STR, __func__);
    return 0;
}

