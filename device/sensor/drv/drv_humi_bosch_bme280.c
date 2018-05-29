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


#define BME280_I2C_ADDR_PRIM                (0x76)
#define BME280_I2C_ADDR_SEC                 (0x77)
#define BME280_CHIP_ID                      (0x60)
#define BME280_SOFT_RESET                   (0xB6)

#define BME280_CHIP_ID_ADDR                 (0xD0)
#define BME280_RESET_ADDR                   (0xE0)
#define BME280_TEMP_PRESS_CALIB_DATA_ADDR   (0x88)
#define BME280_HUMIDITY_CALIB_DATA_ADDR     (0xE1)
#define BME280_PWR_CTRL_ADDR                (0xF4)
#define BME280_CTRL_HUM_ADDR                (0xF2)
#define BME280_CTRL_MEAS_ADDR               (0xF4)
#define BME280_CONFIG_ADDR                  (0xF5)
#define BME280_TEMP_DATA_ADDR               (0xFA)
#define BME280_HUMI_DATA_ADDR               (0xFD)

#define BME280_TEMP_PRESS_CALIB_DATA_LEN    (26)
#define BME280_HUMIDITY_CALIB_DATA_LEN      (7)
#define BME280_HUMI_DATA_LEN                (2)
#define BME280_TEMP_DATA_LEN                (3)


#define BME280_SLEEP_MODE                   (0x00)
#define BME280_FORCED_MODE                  (0x01)
#define BME280_NORMAL_MODE                  (0x03)
#define BME280_POWER_BIT_MASK	            (0x03)

#define BME280_PRESS                        (1)
#define BME280_TEMP                         (1 << 1)
#define BME280_HUM                          (1 << 2)
#define BME280_ALL                          (0x07)

#define BME280_OSR_PRESS_SEL                (1)
#define BME280_OSR_TEMP_SEL                 (1 << 1)
#define BME280_OSR_HUM_SEL                  (1 << 2)
#define BME280_FILTER_SEL                   (1 << 3)
#define BME280_STANDBY_SEL                  (1 << 4)
#define BME280_ALL_SETTINGS_SEL             (0x1F)

#define BME280_NO_OVERSAMPLING              (0x00)
#define BME280_OVERSAMPLING_1X              (0x01)
#define BME280_OVERSAMPLING_2X              (0x02)
#define BME280_OVERSAMPLING_4X              (0x03)
#define BME280_OVERSAMPLING_8X              (0x04)
#define BME280_OVERSAMPLING_16X             (0x05)

#define BME280_STANDBY_TIME_1_MS            (0x00)
#define BME280_STANDBY_TIME_62_5_MS         (0x01)
#define BME280_STANDBY_TIME_125_MS          (0x02)
#define BME280_STANDBY_TIME_250_MS          (0x03)
#define BME280_STANDBY_TIME_500_MS          (0x04)
#define BME280_STANDBY_TIME_1000_MS         (0x05)
#define BME280_STANDBY_TIME_10_MS           (0x06)
#define BME280_STANDBY_TIME_20_MS           (0x07)

#define BME280_FILTER_COEFF_OFF             (0x00)
#define BME280_FILTER_COEFF_2               (0x01)
#define BME280_FILTER_COEFF_4               (0x02)
#define BME280_FILTER_COEFF_8               (0x03)
#define BME280_FILTER_COEFF_16              (0x04)



#define BME280_ULTRA_LOW_POWER_MODE         (0x00)
#define BME280_LOW_POWER_MODE	            (0x01)
#define BME280_STANDARD_RESOLUTION_MODE     (0x02)
#define BME280_HIGH_RESOLUTION_MODE         (0x03)
#define BME280_ULTRA_HIGH_RESOLUTION_MODE   (0x04)


#define BME280_OVERSAMP_SKIPPED             (0x00)
#define BME280_OVERSAMP_1X                  (0x01)
#define BME280_OVERSAMP_2X                  (0x02)
#define BME280_OVERSAMP_4X                  (0x03)
#define BME280_OVERSAMP_8X                  (0x04)
#define BME280_OVERSAMP_16X                 (0x05)


#define BME280_SENSOR_MODE_MSK              (0x03)
#define BME280_SENSOR_MODE_POS              (0x00)

#define BME280_CTRL_HUM_MSK                 (0x07)
#define BME280_CTRL_HUM_POS                 (0x00)

#define BME280_CTRL_PRESS_MSK               (0x1C)
#define BME280_CTRL_PRESS_POS               (0x02)

#define BME280_CTRL_TEMP_MSK                (0xE0)
#define BME280_CTRL_TEMP_POS                (0x05)

#define BME280_FILTER_MSK                   (0x1C)
#define BME280_FILTER_POS                   (0x02)

#define BME280_STANDBY_MSK                  (0xE0)
#define BME280_STANDBY_POS                  (0x05)

#define BME280_SHIFT_BY_01_BITS             (1)
#define BME280_SHIFT_BY_02_BITS             (2)
#define BME280_SHIFT_BY_03_BITS             (3)
#define BME280_SHIFT_BY_04_BITS             (4)
#define BME280_SHIFT_BY_05_BITS             (5)
#define BME280_SHIFT_BY_08_BITS             (8)
#define BME280_SHIFT_BY_11_BITS             (11)
#define BME280_SHIFT_BY_12_BITS             (12)
#define BME280_SHIFT_BY_13_BITS             (13)
#define BME280_SHIFT_BY_14_BITS             (14)
#define BME280_SHIFT_BY_15_BITS             (15)
#define BME280_SHIFT_BY_16_BITS             (16)
#define BME280_SHIFT_BY_17_BITS             (17)
#define BME280_SHIFT_BY_18_BITS             (18)
#define BME280_SHIFT_BY_19_BITS             (19)
#define BME280_SHIFT_BY_25_BITS             (25)
#define BME280_SHIFT_BY_31_BITS             (31)

#define BME280_I2C_SLAVE_ADDR_LOW           (0X76)
#define BME280_I2C_SLAVE_ADDR_HIGN          (0X77)

#define BME280_HUMI_UNIT_RATIO_10           (100)
#define BME280_HUMI_UNIT_RATIO_1024         (1024)

#define BME280_DEFAULT_ODR_1HZ              (1)

#define BME280_I2C_ADDR_TRANS(n)            ((n)<<1)  
#define BME280_I2C_ADDR                     BME280_I2C_ADDR_TRANS(BME280_I2C_SLAVE_ADDR_LOW)
#define BME280_CONCAT_BYTES(msb, lsb)       (((uint16_t)msb << 8) | (uint16_t)lsb)

#define BME280_SET_BITS(reg_data, bitname, data)        ((reg_data & ~(bitname##_MSK)) | ((data << bitname##_POS) & bitname##_MSK))
#define BME280_SET_BITS_POS_0(reg_data, bitname, data)  ((reg_data & ~(bitname##_MSK)) | (data & bitname##_MSK))

#define BME280_GET_BITS(reg_data, bitname)              ((reg_data & (bitname##_MSK)) >> (bitname##_POS))
#define BME280_GET_BITS_POS_0(reg_data, bitname)        (reg_data & (bitname##_MSK))


typedef struct _bme280_cali_table_t {
    uint16_t dig_T1;
    int16_t  dig_T2;
    int16_t  dig_T3;
    uint16_t dig_P1;
    int16_t  dig_P2;
    int16_t  dig_P3;
    int16_t  dig_P4;
    int16_t  dig_P5;
    int16_t  dig_P6;
    int16_t  dig_P7;
    int16_t  dig_P8;
    int16_t  dig_P9;
    uint8_t  dig_H1;
    int16_t  dig_H2;
    uint8_t  dig_H3;
    int16_t  dig_H4;
    int16_t  dig_H5;
    int8_t   dig_H6;
    int32_t  t_fine;
}bme280_cali_table_t;

typedef struct _bme280_config_t {
    uint8_t osr_p;
    uint8_t osr_t;
    uint8_t osr_h;
    uint8_t filter;
    uint8_t standby_time;
}bme280_config_t;

static bme280_cali_table_t g_cali_table;
static bme280_config_t g_bme280_config;

i2c_dev_t bme280_ctx = {
    .port = 1,
    .config.address_width = 8,
    .config.freq = 400000,
    .config.dev_addr = BME280_I2C_ADDR,
};

static int drv_humi_bosch_bme280_get_cali_temp(i2c_dev_t* drv)
{
    int ret;
    uint8_t calib_data[BME280_TEMP_PRESS_CALIB_DATA_LEN] = {0};
    
    ret = sensor_i2c_read(drv, BME280_TEMP_PRESS_CALIB_DATA_ADDR, calib_data, BME280_TEMP_PRESS_CALIB_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }
    
    g_cali_table.dig_T1 = BME280_CONCAT_BYTES(calib_data[1], calib_data[0]);
    g_cali_table.dig_T2 = (int16_t)BME280_CONCAT_BYTES(calib_data[3], calib_data[2]);
    g_cali_table.dig_T3 = (int16_t)BME280_CONCAT_BYTES(calib_data[5], calib_data[4]);
    g_cali_table.dig_P1 = BME280_CONCAT_BYTES(calib_data[7], calib_data[6]);
    g_cali_table.dig_P2 = (int16_t)BME280_CONCAT_BYTES(calib_data[9], calib_data[8]);
    g_cali_table.dig_P3 = (int16_t)BME280_CONCAT_BYTES(calib_data[11], calib_data[10]);
    g_cali_table.dig_P4 = (int16_t)BME280_CONCAT_BYTES(calib_data[13], calib_data[12]);
    g_cali_table.dig_P5 = (int16_t)BME280_CONCAT_BYTES(calib_data[15], calib_data[14]);
    g_cali_table.dig_P6 = (int16_t)BME280_CONCAT_BYTES(calib_data[17], calib_data[16]);
    g_cali_table.dig_P7 = (int16_t)BME280_CONCAT_BYTES(calib_data[19], calib_data[18]);
    g_cali_table.dig_P8 = (int16_t)BME280_CONCAT_BYTES(calib_data[21], calib_data[20]);
    g_cali_table.dig_P9 = (int16_t)BME280_CONCAT_BYTES(calib_data[23], calib_data[22]);
    g_cali_table.dig_H1 = calib_data[25];

    return 0;
}

static int drv_humi_bosch_bme280_get_cali_humi(i2c_dev_t* drv)
{
    int ret = 0;
    int16_t dig_H4_lsb;
    int16_t dig_H4_msb;
    int16_t dig_H5_lsb;
    int16_t dig_H5_msb;
    
    uint8_t table[BME280_HUMIDITY_CALIB_DATA_LEN] = {0};
    ret = sensor_i2c_read(drv, BME280_HUMIDITY_CALIB_DATA_ADDR, table, BME280_HUMIDITY_CALIB_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return -1;
    }

    g_cali_table.dig_H2 = (int16_t)BME280_CONCAT_BYTES(table[1], table[0]);
    g_cali_table.dig_H3 = table[2];

    dig_H4_msb = (int16_t)(int8_t)table[3] * 16;
    dig_H4_lsb = (int16_t)(table[4] & 0x0F);
    g_cali_table.dig_H4 = dig_H4_msb | dig_H4_lsb;

    dig_H5_msb = (int16_t)(int8_t)table[5] * 16;
    dig_H5_lsb = (int16_t)(table[4] >> 4);
    g_cali_table.dig_H5 = dig_H5_msb | dig_H5_lsb;
    g_cali_table.dig_H6 = (int8_t)table[6];

    return 0;
}

static int drv_humi_bosch_bme280_get_cali_parm(i2c_dev_t* drv)
{
    int ret = 0;
    uint8_t table[BME280_HUMIDITY_CALIB_DATA_LEN] = {0};
    
    ret = drv_humi_bosch_bme280_get_cali_temp(drv);
    if(unlikely(ret)){
        return -1;
    }
    
    ret = drv_humi_bosch_bme280_get_cali_humi(drv);
    if(unlikely(ret)){
        return -1;
    }

    return 0;
}


static int drv_humi_bosch_bme280_soft_reset(i2c_dev_t* drv)
{
    int ret = 0;
    ret = sensor_i2c_write(drv, BME280_RESET_ADDR, BME280_SOFT_RESET, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return -1;
    }
    return 0;
}

static int drv_humi_bosch_bme280_validate_id(i2c_dev_t* drv, uint8_t id_value)
{
    uint8_t value = 0x00;
    int ret = 0;

    if(drv == NULL){
        return -1;
    }
    
    ret = sensor_i2c_read(drv, BME280_CHIP_ID_ADDR, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }
    
    if (id_value != value){
        return -1;
    }
    
    return 0;
}

static int drv_humi_bosch_bme280_enter_sleep_mode(i2c_dev_t* drv)
{
    int ret = 0;
    uint8_t ctrl_hum;
    uint8_t ctrl_meas;
    uint8_t data[4];
    uint8_t reg_addr = BME280_CTRL_HUM_ADDR;
    if(drv == NULL){
        return -1;
    }
    
    ret = sensor_i2c_read(drv, BME280_CTRL_HUM_ADDR, data, 4, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }

    g_bme280_config.osr_h = BME280_GET_BITS_POS_0(data[0], BME280_CTRL_HUM);
    g_bme280_config.osr_p = BME280_GET_BITS(data[2], BME280_CTRL_PRESS);
    g_bme280_config.osr_t = BME280_GET_BITS(data[2], BME280_CTRL_TEMP);
    g_bme280_config.filter = BME280_GET_BITS(data[3], BME280_FILTER);
    g_bme280_config.standby_time = BME280_GET_BITS(data[3], BME280_STANDBY);
    
    ret = drv_humi_bosch_bme280_soft_reset(&bme280_ctx);
    if(unlikely(ret)){
        return ret;
    }
    
    return 0;
}


static int drv_humi_bosch_bme280_set_work_mode(i2c_dev_t* drv,uint8_t mode)
{
    uint8_t ret = 0;
    uint8_t value = 0;
    uint8_t temp = 0;
    uint8_t baro = 0;
    uint8_t humi = 0;

    switch (mode) {
        case BME280_ULTRA_LOW_POWER_MODE:
            temp = BME280_OVERSAMP_1X;
            baro = BME280_OVERSAMP_1X;
            humi = BME280_OVERSAMP_1X;
            break;

        case BME280_LOW_POWER_MODE:
            temp = BME280_OVERSAMP_2X;
            baro = BME280_OVERSAMP_2X;
            humi = BME280_OVERSAMP_2X;
            break;

        case BME280_STANDARD_RESOLUTION_MODE:
            temp = BME280_OVERSAMP_4X;
            baro = BME280_OVERSAMP_4X;
            humi = BME280_OVERSAMP_4X;
            break;

        case BME280_HIGH_RESOLUTION_MODE:
            temp = BME280_OVERSAMP_8X;
            baro = BME280_OVERSAMP_8X;
            humi = BME280_OVERSAMP_8X;
            break;

        case BME280_ULTRA_HIGH_RESOLUTION_MODE:
            temp = BME280_OVERSAMP_16X;
            baro = BME280_OVERSAMP_16X;
            humi = BME280_OVERSAMP_16X;
            break;

        default:
            return -1;

    }

    ret = sensor_i2c_read(drv, BME280_CTRL_HUM_ADDR, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }
    
    value = BME280_SET_BITS(value,BME280_CTRL_HUM,humi);
    
    ret = sensor_i2c_write(drv, BME280_CTRL_HUM_ADDR, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }

    value = 0;
    ret = sensor_i2c_read(drv, BME280_PWR_CTRL_ADDR, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }

    value = BME280_SET_BITS(value,BME280_CTRL_PRESS,baro);
    value = BME280_SET_BITS(value,BME280_CTRL_TEMP,temp);
    
    ret = sensor_i2c_write(drv, BME280_PWR_CTRL_ADDR, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }

    return ret;
}


static int drv_humi_bosch_bme280_set_power_mode(i2c_dev_t* drv, dev_power_mode_e mode)
{
    uint8_t value = 0x00;
    int ret = 0;
    uint8_t buf[4];
    
    ret = sensor_i2c_read(drv, BME280_PWR_CTRL_ADDR, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }

    switch(mode){
        case DEV_POWER_ON:{
            if((value & BME280_POWER_BIT_MASK) == BME280_NORMAL_MODE){
                return 0;
            }
            value |= BME280_NORMAL_MODE;
            ret = sensor_i2c_write(drv, BME280_PWR_CTRL_ADDR, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
            if(unlikely(ret)){
                return ret;
            }
        }break;
        
        case DEV_POWER_OFF:
        case DEV_SLEEP:{
            if((value & BME280_POWER_BIT_MASK) == BME280_SLEEP_MODE){
                return 0;
            }

            value &= (~BME280_POWER_BIT_MASK);
            ret = sensor_i2c_write(drv, BME280_PWR_CTRL_ADDR, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
            if(unlikely(ret)){
                return ret;
            }

        }break;
        
       default:break;
    }
    return 0;
}

static uint8_t drv_humi_bosch_bme280_hz2odr(int hz)
{
    if(hz > 80)
        return BME280_STANDBY_TIME_1_MS;
    else if(hz > 13)
        return BME280_STANDBY_TIME_62_5_MS;
    else if(hz > 7)
        return BME280_STANDBY_TIME_125_MS;
    else if(hz > 3)
        return BME280_STANDBY_TIME_250_MS;
    else
        return BME280_STANDBY_TIME_500_MS;

}

static int drv_humi_bosch_bme280_set_odr(i2c_dev_t* drv, uint8_t odr)
{
    int     ret = 0;
    uint8_t v_data_u8 = 0;

    ret = sensor_i2c_read(drv,BME280_CONFIG_ADDR,&v_data_u8,I2C_DATA_LEN,I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }

    v_data_u8 = BME280_SET_BITS(v_data_u8,BME280_STANDBY,odr);
    ret = sensor_i2c_write(drv,BME280_CONFIG_ADDR,
                            &v_data_u8,I2C_DATA_LEN,I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }
    
    return ret;
}


static int drv_humi_bosch_bme280_set_default_config(i2c_dev_t* drv)
{
    int ret = 0;

    ret = drv_humi_bosch_bme280_set_power_mode(drv, DEV_SLEEP);
    if(unlikely(ret)){
        return ret;
    }

    ret = drv_humi_bosch_bme280_set_odr(drv, BME280_DEFAULT_ODR_1HZ);
    if(unlikely(ret)){
        return ret;
    }

    return 0;
}


static int drv_humi_bosch_bme280_comp_temp(temperature_data_t *uncomp_data)
{
    int32_t var1;
    int32_t var2;
    int32_t temperature;
    int32_t temperature_min = -4000;
    int32_t temperature_max = 8500;

    var1 = (int32_t)((uncomp_data->t / 8) - ((int32_t)g_cali_table.dig_T1 * 2));
    var1 = (var1 * ((int32_t)g_cali_table.dig_T2)) / 2048;
    var2 = (int32_t)((uncomp_data->t / 16) - ((int32_t)g_cali_table.dig_T1));
    var2 = (((var2 * var2) / 4096) * ((int32_t)g_cali_table.dig_T3)) / 16384;
    g_cali_table.t_fine = var1 + var2;
    temperature = (g_cali_table.t_fine * 5 + 128) / 256;

    if (temperature < temperature_min)
        temperature = temperature_min;
    else if (temperature > temperature_max)
        temperature = temperature_max;
    
    uncomp_data->t = temperature;

    return 0;
}


static int drv_humi_bosch_bme280_comp_humi(humidity_data_t* pdata)
{
    int32_t var1 = 0;
    int32_t var2 = 0;
    int32_t var3 = 0;
    int32_t var4 = 0;
    int32_t var5 = 0;
    uint32_t humidity = 0;
    uint32_t humidity_max = 100000;

    var1 = g_cali_table.t_fine - ((int32_t)76800);
    var2 = (int32_t)((pdata->h) * 16384);
    var3 = (int32_t)(((int32_t)g_cali_table.dig_H4) * 1048576);
    var4 = ((int32_t)g_cali_table.dig_H5) * var1;
    var5 = (((var2 - var3) - var4) + (int32_t)16384) / 32768;
    var2 = (var1 * ((int32_t)g_cali_table.dig_H6)) / 1024;
    var3 = (var1 * ((int32_t)g_cali_table.dig_H3)) / 2048;
    var4 = ((var2 * (var3 + (int32_t)32768)) / 1024) + (int32_t)2097152;
    var2 = ((var4 * ((int32_t)g_cali_table.dig_H2)) + 8192) / 16384;
    var3 = var5 * var2;
    var4 = ((var3 / 32768) * (var3 / 32768)) / 128;
    var5 = var3 - ((var4 * ((int32_t)g_cali_table.dig_H1)) / 16);
    var5 = (var5 < 0 ? 0 : var5);
    var5 = (var5 > 419430400 ? 419430400 : var5);
    humidity = (uint32_t)(var5 / 4096);

    if (humidity > humidity_max){
        humidity = humidity_max;
    }
    pdata->h = (humidity * BME280_HUMI_UNIT_RATIO_10)/BME280_HUMI_UNIT_RATIO_1024;
    return 0;
}


static int drv_humi_bosch_bme280_cali_temp(void)
{
    int ret = 0;
    uint8_t data[BME280_TEMP_DATA_LEN] = {0};
    temperature_data_t temp;

    ret  = sensor_i2c_read(&bme280_ctx, BME280_TEMP_DATA_ADDR, data, BME280_TEMP_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }

    temp.t = (int32_t)((uint32_t)data[0]<<BME280_SHIFT_BY_12_BITS) | ((uint32_t)data[1]<<BME280_SHIFT_BY_04_BITS) |((uint32_t)data[0]>>BME280_SHIFT_BY_04_BITS); 

    ret = drv_humi_bosch_bme280_comp_temp(&temp);
    if(unlikely(ret)){
        return ret;
    }

    return 0;
}


static int drv_humi_bosch_bme280_read_humi(humidity_data_t* pdata)
{
    int ret = 0;
    uint8_t data[BME280_HUMI_DATA_LEN] = {0};
    uint32_t data_lsb;
    uint32_t data_msb;

    ret  = sensor_i2c_read(&bme280_ctx, BME280_HUMI_DATA_ADDR, data, BME280_HUMI_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }

    data_lsb = (uint32_t)data[0] << 8;
    data_msb = (uint32_t)data[1];
    pdata->h = data_msb | data_lsb;

    ret = drv_humi_bosch_bme280_comp_humi(pdata);
    if(unlikely(ret)){
        return ret;
    }
    return 0;
}


static void drv_humi_bosch_bme280_irq_handle(void)
{
    /* no handle so far */
}

static int drv_humi_bosch_bme280_open(void)
{
    int ret = 0;

    ret  = drv_humi_bosch_bme280_set_work_mode(&bme280_ctx, BME280_STANDARD_RESOLUTION_MODE);
    if(unlikely(ret)){
        return -1;
    }

    ret  = drv_humi_bosch_bme280_set_power_mode(&bme280_ctx, DEV_POWER_ON);
    if(unlikely(ret)){
        return -1;
    }
    return 0;
}

static int drv_humi_bosch_bme280_close(void)
{
    int ret = 0;
    ret  = drv_humi_bosch_bme280_set_power_mode(&bme280_ctx, DEV_POWER_OFF);
    if(unlikely(ret)){
        return -1;
    }
    return 0;
}

static int drv_humi_bosch_bme280_read(void *buf, size_t len)
{
    int ret = 0;
    size_t size = 0;
    uint8_t data[BME280_TEMP_DATA_LEN] = {0};
    uint32_t data_lsb;
    uint32_t data_msb;

    humidity_data_t*   pdata = (humidity_data_t*)buf;
    if(buf == NULL){
        return -1;
    }
    
    size = sizeof(humidity_data_t);
    if(len < size){
        return -1;
    }

    ret  = drv_humi_bosch_bme280_cali_temp();
    if(unlikely(ret)){
        return -1;
    }

    ret  = drv_humi_bosch_bme280_read_humi(pdata);
    if(unlikely(ret)){
        return -1;
    }
    
    pdata->timestamp = aos_now_ms();
    return (int)size;
}

static int drv_humi_bosch_bme280_ioctl(int cmd, unsigned long arg)
{
    int ret = 0;
    
    switch(cmd){
        case SENSOR_IOCTL_ODR_SET:{
            uint8_t odr = drv_humi_bosch_bme280_hz2odr(arg);
            ret = drv_humi_bosch_bme280_set_odr(&bme280_ctx, arg);
            if(unlikely(ret)){
                return -1;
            }
        }break;
        case SENSOR_IOCTL_SET_POWER:{
            ret = drv_humi_bosch_bme280_set_power_mode(&bme280_ctx, arg);
            if(unlikely(ret)){
                return -1;
            }
        }break;
        case SENSOR_IOCTL_GET_INFO:{ 
            /* fill the dev info here */
            dev_sensor_info_t *info = (dev_sensor_info_t *)arg;
            info->model = "BME280";
            info->range_max = 16;
            info->range_min = 4;
            info->unit = permillage;
        }break;
       
       default:break;
    }

    return 0;
}

int drv_humi_bosch_bme280_init(void){
    int ret = 0;
    sensor_obj_t sensor;

    /* fill the sensor obj parameters here */
    sensor.io_port    = I2C_PORT;
    sensor.tag        = TAG_DEV_HUMI;
    sensor.path       = dev_humi_path;
    sensor.open       = drv_humi_bosch_bme280_open;
    sensor.close      = drv_humi_bosch_bme280_close;
    sensor.read       = drv_humi_bosch_bme280_read;
    sensor.write      = NULL;
    sensor.ioctl      = drv_humi_bosch_bme280_ioctl;
    sensor.irq_handle = drv_humi_bosch_bme280_irq_handle;

    ret = sensor_create_obj(&sensor);
    if(unlikely(ret)){
        return -1;
    }

    ret = drv_humi_bosch_bme280_validate_id(&bme280_ctx, BME280_CHIP_ID);
    if(unlikely(ret)){
        return -1;
    }

    ret = drv_humi_bosch_bme280_soft_reset(&bme280_ctx);
    if(unlikely(ret)){
        return -1;
    }


    ret = drv_humi_bosch_bme280_set_default_config(&bme280_ctx);
    if(unlikely(ret)){
        return -1;
    }
    
    ret = drv_humi_bosch_bme280_get_cali_parm(&bme280_ctx);
    if(unlikely(ret)){
        return -1;
    }

    /* update the phy sensor info to sensor hal */
    LOG("%s %s successfully \n", SENSOR_STR, __func__);
    return 0;
}

