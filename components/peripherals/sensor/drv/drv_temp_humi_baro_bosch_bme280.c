/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 *
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "aos/kernel.h"
#include "network/hal/base.h"
#include "sensor_drv_api.h"
#include "sensor_hal.h"

#define BME280_I2C_ADDR_PRIM (0x76)
#define BME280_I2C_ADDR_SEC (0x77)
#define BME280_CHIP_ID (0x60)
#define BME280_SOFT_RESET (0xB6)

#define BME280_CHIP_ID_ADDR (0xD0)
#define BME280_RESET_ADDR (0xE0)
#define BME280_TEMP_PRESS_CALIB_DATA_ADDR (0x88)
#define BME280_HUMIDITY_CALIB_DATA_ADDR (0xE1)
#define BME280_PWR_CTRL_ADDR (0xF4)
#define BME280_CTRL_HUM_ADDR (0xF2)
#define BME280_CTRL_MEAS_ADDR (0xF4)
#define BME280_CONFIG_ADDR (0xF5)
#define BME280_BARO_DATA_ADDR (0xF7)
#define BME280_TEMP_DATA_ADDR (0xFA)
#define BME280_HUMI_DATA_ADDR (0xFD)

#define BME280_TEMP_PRESS_CALIB_DATA_LEN (26)
#define BME280_HUMIDITY_CALIB_DATA_LEN (7)
#define BME280_BARO_DATA_LEN (3)
#define BME280_HUMI_DATA_LEN (2)
#define BME280_TEMP_DATA_LEN (3)
#define BME280_MAX_DATA_LEN  (3)


#define BME280_SLEEP_MODE (0x00)
#define BME280_FORCED_MODE (0x01)
#define BME280_NORMAL_MODE (0x03)
#define BME280_POWER_BIT_MASK (0x03)

#define BME280_PRESS (1)
#define BME280_TEMP (1 << 1)
#define BME280_HUM (1 << 2)
#define BME280_ALL (0x07)

#define BME280_OSR_PRESS_SEL (1)
#define BME280_OSR_TEMP_SEL (1 << 1)
#define BME280_OSR_HUM_SEL (1 << 2)
#define BME280_FILTER_SEL (1 << 3)
#define BME280_STANDBY_SEL (1 << 4)
#define BME280_ALL_SETTINGS_SEL (0x1F)

#define BME280_NO_OVERSAMPLING (0x00)
#define BME280_OVERSAMPLING_1X (0x01)
#define BME280_OVERSAMPLING_2X (0x02)
#define BME280_OVERSAMPLING_4X (0x03)
#define BME280_OVERSAMPLING_8X (0x04)
#define BME280_OVERSAMPLING_16X (0x05)

#define BME280_STANDBY_TIME_1_MS (0x00)
#define BME280_STANDBY_TIME_62_5_MS (0x01)
#define BME280_STANDBY_TIME_125_MS (0x02)
#define BME280_STANDBY_TIME_250_MS (0x03)
#define BME280_STANDBY_TIME_500_MS (0x04)
#define BME280_STANDBY_TIME_1000_MS (0x05)
#define BME280_STANDBY_TIME_10_MS (0x06)
#define BME280_STANDBY_TIME_20_MS (0x07)

#define BME280_FILTER_COEFF_OFF (0x00)
#define BME280_FILTER_COEFF_2 (0x01)
#define BME280_FILTER_COEFF_4 (0x02)
#define BME280_FILTER_COEFF_8 (0x03)
#define BME280_FILTER_COEFF_16 (0x04)


#define BME280_ULTRA_LOW_POWER_MODE (0x00)
#define BME280_LOW_POWER_MODE (0x01)
#define BME280_STANDARD_RESOLUTION_MODE (0x02)
#define BME280_HIGH_RESOLUTION_MODE (0x03)
#define BME280_ULTRA_HIGH_RESOLUTION_MODE (0x04)


#define BME280_OVERSAMP_SKIPPED (0x00)
#define BME280_OVERSAMP_1X (0x01)
#define BME280_OVERSAMP_2X (0x02)
#define BME280_OVERSAMP_4X (0x03)
#define BME280_OVERSAMP_8X (0x04)
#define BME280_OVERSAMP_16X (0x05)


#define BME280_SENSOR_MODE_MSK (0x03)
#define BME280_SENSOR_MODE_POS (0x00)

#define BME280_CTRL_HUM_MSK (0x07)
#define BME280_CTRL_HUM_POS (0x00)

#define BME280_CTRL_PRESS_MSK (0x1C)
#define BME280_CTRL_PRESS_POS (0x02)

#define BME280_CTRL_TEMP_MSK (0xE0)
#define BME280_CTRL_TEMP_POS (0x05)

#define BME280_FILTER_MSK (0x1C)
#define BME280_FILTER_POS (0x02)

#define BME280_STANDBY_MSK (0xE0)
#define BME280_STANDBY_POS (0x05)

#define BME280_SHIFT_BY_01_BITS (1)
#define BME280_SHIFT_BY_02_BITS (2)
#define BME280_SHIFT_BY_03_BITS (3)
#define BME280_SHIFT_BY_04_BITS (4)
#define BME280_SHIFT_BY_05_BITS (5)
#define BME280_SHIFT_BY_08_BITS (8)
#define BME280_SHIFT_BY_11_BITS (11)
#define BME280_SHIFT_BY_12_BITS (12)
#define BME280_SHIFT_BY_13_BITS (13)
#define BME280_SHIFT_BY_14_BITS (14)
#define BME280_SHIFT_BY_15_BITS (15)
#define BME280_SHIFT_BY_16_BITS (16)
#define BME280_SHIFT_BY_17_BITS (17)
#define BME280_SHIFT_BY_18_BITS (18)
#define BME280_SHIFT_BY_19_BITS (19)
#define BME280_SHIFT_BY_25_BITS (25)
#define BME280_SHIFT_BY_31_BITS (31)

#define BME280_I2C_SLAVE_ADDR_LOW (0X76)
#define BME280_I2C_SLAVE_ADDR_HIGN (0X77)

#define BME280_HUMI_UNIT_RATIO_10 (10)
#define BME280_HUMI_UNIT_RATIO_1024 (1024)

#define BME280_TEMP_UNIT_RATIO_10 (10)

#define BME280_BARO_UNIT_RATIO_256 (256)
#define BME280_DEFAULT_ODR_1HZ (1)

#define BME280_I2C_ADDR_TRANS(n) ((n) << 1)
#define BME280_I2C_ADDR BME280_I2C_ADDR_TRANS(BME280_I2C_SLAVE_ADDR_LOW)
#define BME280_CONCAT_BYTES(msb, lsb) (((uint16_t)msb << 8) | (uint16_t)lsb)

#define BME280_SET_BITS(reg_data, bitname, data) \
    ((reg_data & ~(bitname##_MSK)) | ((data << bitname##_POS) & bitname##_MSK))
#define BME280_SET_BITS_POS_0(reg_data, bitname, data) \
    ((reg_data & ~(bitname##_MSK)) | (data & bitname##_MSK))

#define BME280_GET_BITS(reg_data, bitname) \
    ((reg_data & (bitname##_MSK)) >> (bitname##_POS))
#define BME280_GET_BITS_POS_0(reg_data, bitname) (reg_data & (bitname##_MSK))

#if AOS_SENSOR_BARO_BME280_SPI || AOS_SENSOR_TEMP_BME280_SPI || AOS_SENSOR_HUMI_BME280_SPI
#define  BME280_IO_PORT SPI_PORT
#else
#define  BME280_IO_PORT I2C_PORT
#endif

#define BMA280_REG_ADDR(n) ((n) << 8)
#define BMA280_SPI_REG_INDX (1)
#define BMA280_SPI_DATA_INDX (0)
#define BMA280_SPI_DATA_REG_SIZE (2)
#define BMA280_SPI_WRITE_FLAG (0X7F)
#define BMA280_SPI_READ_FLAG (0X80)
typedef struct _bme280_cali_table_t
{
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
} bme280_cali_table_t;

typedef struct _bme280_config_t
{
    uint8_t osr_p;
    uint8_t osr_t;
    uint8_t osr_h;
    uint8_t filter;
    uint8_t standby_time;
} bme280_config_t;

typedef enum{
    bme_280_temp,
    bme_280_humi,
    bme_280_baro,
    bme_280_max
} bme280_type_e;

typedef struct _bme280_raw_data_t
{
    uint32_t raw_t;
    uint32_t raw_h;
    uint32_t raw_p;
} bme280_raw_data_t;

typedef struct _bme280_comp_data_t
{
    int32_t  comp_t;
    uint32_t comp_h;
    uint32_t comp_p;
} bme280_comp_data_t;


static bme280_cali_table_t g_cali_table;

i2c_dev_t bme280_ctx_i2c = {
    .port                 = 3,
    .config.address_width = 8,
    .config.freq          = 400000,
    .config.dev_addr      = BME280_I2C_ADDR,
};

spi_dev_t bme280_ctx_spi = {
    .port = 2,
};

sensor_io_dev_t bme280_ctx;

sensor_io_func_st bme280_api = { NULL, NULL };

uint32_t g_bme280_init_flag = 0;
uint32_t g_bme280_open_flag = 0;

static int bme280_io_read(uint16_t reg, uint8_t *data, uint16_t size)
{
    if (unlikely(NULL == bme280_api.read)) {
        return -1;
    }

    return bme280_api.read(reg, data, size);
}

static int bme280_io_write(uint16_t reg, uint8_t *data, uint16_t size)
{
    if (NULL == bme280_api.write) {
        return -1;
    }

    return bme280_api.write(reg, data, size);
}
static int drv_humi_bosch_bme280_spi_read(uint16_t reg, uint8_t *data,
                                          uint16_t size)
{
    int i;
    int ret;

    uint8_t *tx_data = malloc(BMA280_SPI_DATA_REG_SIZE * size);
    if (NULL == tx_data) {
        return -1;
    }

    for (i = 0; i < size; i++) {
        tx_data[BMA280_SPI_DATA_REG_SIZE * i + BMA280_SPI_REG_INDX] =
          (uint8_t)(reg + i) | BMA280_SPI_READ_FLAG;
    }

    ret =
      sensor_io_read(&bme280_ctx, tx_data, tx_data, size, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        free(tx_data);
        return -1;
    }

    for (i = 0; i < size; i++) {
        data[i] = tx_data[BMA280_SPI_DATA_REG_SIZE * i + BMA280_SPI_DATA_INDX];
    }

    free(tx_data);
    return ret;
}


static int drv_humi_bosch_bme280_spi_write(uint16_t reg, uint8_t *data,
                                           uint16_t size)
{
    uint16_t i;
    int      ret;
    uint8_t *tx_data = malloc(BMA280_SPI_DATA_REG_SIZE * size);
    if (NULL == tx_data) {
        return -1;
    }

    for (i = 0; i < size; i++) {
        tx_data[BMA280_SPI_DATA_REG_SIZE * i + BMA280_SPI_DATA_INDX] = data[i];
        tx_data[BMA280_SPI_DATA_REG_SIZE * i + BMA280_SPI_REG_INDX] =
          (uint8_t)(reg + i) & BMA280_SPI_WRITE_FLAG;
    }

    ret = sensor_io_write(&bme280_ctx, NULL, tx_data, size, I2C_OP_RETRIES);

    free(tx_data);
    return ret;
}


static int drv_humi_bosch_bme280_i2c_read(uint16_t reg, uint8_t *data,
                                          uint16_t size)
{
    uint16_t addr = reg;
    if(bme280_ctx.io_port != I2C_PORT){
        return -1;
    }
    return sensor_io_read(&bme280_ctx, (uint8_t*)&addr, data, size, I2C_OP_RETRIES);
}

static int drv_humi_bosch_bme280_i2c_write(uint16_t reg, uint8_t *data,
                                           uint16_t size)
{
    uint16_t addr = reg;
    if(bme280_ctx.io_port != I2C_PORT){
        return -1;
    }
    return sensor_io_write(&bme280_ctx, (uint8_t*)&addr, data, size, I2C_OP_RETRIES);
}

static int drv_bosch_bme280_io_init(int io_port)
{

    memset(&bme280_api, 0, sizeof(bme280_api));
    memset(&bme280_ctx, 0, sizeof(bme280_ctx));
    if (io_port == I2C_PORT) {
        bme280_ctx.io_port  = I2C_PORT;
        memcpy(&bme280_ctx.dev_i2c,&bme280_ctx_i2c,sizeof(bme280_ctx.dev_i2c));
        bme280_api.read  = drv_humi_bosch_bme280_i2c_read;
        bme280_api.write = drv_humi_bosch_bme280_i2c_write;
        return 0;
    } else if (io_port == SPI_PORT) {
        bme280_ctx.io_port  = SPI_PORT;
        memcpy(&bme280_ctx.dev_spi,&bme280_ctx_spi,sizeof(bme280_ctx.dev_spi));
        bme280_api.read  = drv_humi_bosch_bme280_spi_read;
        bme280_api.write = drv_humi_bosch_bme280_spi_write;
        return 0;
    }
    return -1;
}

static int drv_bosch_bme280_get_cali_temp_baro(void)
{
    int     ret;
    uint8_t calib_data[BME280_TEMP_PRESS_CALIB_DATA_LEN] = { 0 };

    ret = bme280_io_read(BME280_TEMP_PRESS_CALIB_DATA_ADDR, calib_data,
                         BME280_TEMP_PRESS_CALIB_DATA_LEN);
    if (unlikely(ret)) {
        return ret;
    }

    g_cali_table.dig_T1 = BME280_CONCAT_BYTES(calib_data[1], calib_data[0]);
    g_cali_table.dig_T2 =
      (int16_t)BME280_CONCAT_BYTES(calib_data[3], calib_data[2]);
    g_cali_table.dig_T3 =
      (int16_t)BME280_CONCAT_BYTES(calib_data[5], calib_data[4]);
    g_cali_table.dig_P1 = BME280_CONCAT_BYTES(calib_data[7], calib_data[6]);
    g_cali_table.dig_P2 =
      (int16_t)BME280_CONCAT_BYTES(calib_data[9], calib_data[8]);
    g_cali_table.dig_P3 =
      (int16_t)BME280_CONCAT_BYTES(calib_data[11], calib_data[10]);
    g_cali_table.dig_P4 =
      (int16_t)BME280_CONCAT_BYTES(calib_data[13], calib_data[12]);
    g_cali_table.dig_P5 =
      (int16_t)BME280_CONCAT_BYTES(calib_data[15], calib_data[14]);
    g_cali_table.dig_P6 =
      (int16_t)BME280_CONCAT_BYTES(calib_data[17], calib_data[16]);
    g_cali_table.dig_P7 =
      (int16_t)BME280_CONCAT_BYTES(calib_data[19], calib_data[18]);
    g_cali_table.dig_P8 =
      (int16_t)BME280_CONCAT_BYTES(calib_data[21], calib_data[20]);
    g_cali_table.dig_P9 =
      (int16_t)BME280_CONCAT_BYTES(calib_data[23], calib_data[22]);
    g_cali_table.dig_H1 = calib_data[25];

    return 0;
}

static int drv_bosch_bme280_get_cali_humi(void)
{
    int     ret = 0;
    int16_t dig_H4_lsb;
    int16_t dig_H4_msb;
    int16_t dig_H5_lsb;
    int16_t dig_H5_msb;

    uint8_t table[BME280_HUMIDITY_CALIB_DATA_LEN] = { 0 };
    ret = bme280_io_read(BME280_HUMIDITY_CALIB_DATA_ADDR, table,
                         BME280_HUMIDITY_CALIB_DATA_LEN);
    if (unlikely(ret)) {
        return -1;
    }

    g_cali_table.dig_H2 = (int16_t)BME280_CONCAT_BYTES(table[1], table[0]);
    g_cali_table.dig_H3 = table[2];

    dig_H4_msb          = (int16_t)(int8_t)table[3] * 16;
    dig_H4_lsb          = (int16_t)(table[4] & 0x0F);
    g_cali_table.dig_H4 = dig_H4_msb | dig_H4_lsb;

    dig_H5_msb          = (int16_t)(int8_t)table[5] * 16;
    dig_H5_lsb          = (int16_t)(table[4] >> 4);
    g_cali_table.dig_H5 = dig_H5_msb | dig_H5_lsb;
    g_cali_table.dig_H6 = (int8_t)table[6];

    return 0;
}

static int drv_bosch_bme280_get_cali_parm(void)
{
    int     ret                                   = 0;

    ret = drv_bosch_bme280_get_cali_temp_baro();
    if (unlikely(ret)) {
        return -1;
    }

    ret = drv_bosch_bme280_get_cali_humi();
    if (unlikely(ret)) {
        return -1;
    }

    return 0;
}


static int drv_bosch_bme280_soft_reset(void)
{
    int     ret   = 0;
    uint8_t value = BME280_SOFT_RESET;
    ret           = bme280_io_write(BME280_RESET_ADDR, &value, I2C_DATA_LEN);
    if (unlikely(ret)) {
        return -1;
    }
    return 0;
}

static int drv_bosch_bme280_validate_id(uint8_t id_value)
{
    uint8_t value = 0x00;
    int     ret   = 0;


    ret = bme280_io_read(BME280_CHIP_ID_ADDR, &value, I2C_DATA_LEN);
    if (unlikely(ret)) {
        return ret;
    }

    if (id_value != value) {
        return -1;
    }

    return 0;
}


static int drv_bosch_bme280_set_work_mode(bme280_type_e type, uint8_t mode)
{
    uint8_t ret   = 0;
    uint8_t value = 0;
    uint8_t data  = 0;

    if(type >= bme_280_max){
        return -1;
    }

    switch (mode) {
        case BME280_ULTRA_LOW_POWER_MODE:
            data = BME280_OVERSAMP_1X;
            break;

        case BME280_LOW_POWER_MODE:
            data = BME280_OVERSAMP_2X;
            break;

        case BME280_STANDARD_RESOLUTION_MODE:
            data = BME280_OVERSAMP_4X;
            break;

        case BME280_HIGH_RESOLUTION_MODE:
            data = BME280_OVERSAMP_8X;
            break;

        case BME280_ULTRA_HIGH_RESOLUTION_MODE:
            data = BME280_OVERSAMP_16X;
            break;

        default:
            return -1;
    }

    value = 0;
    ret   = bme280_io_read(BME280_PWR_CTRL_ADDR, &value, I2C_DATA_LEN);
    if (unlikely(ret)) {
        return ret;
    }

    value = BME280_SET_BITS(value, BME280_CTRL_TEMP, data);

    ret = bme280_io_write(BME280_PWR_CTRL_ADDR, &value, I2C_DATA_LEN);
    if (unlikely(ret)) {
        return ret;
    }


    if(type == bme_280_humi){
        value = 0;
        ret = bme280_io_read(BME280_CTRL_HUM_ADDR, &value, I2C_DATA_LEN);
        if (unlikely(ret)) {
            return ret;
        }

        value = BME280_SET_BITS(value, BME280_CTRL_HUM, data);

        ret = bme280_io_write(BME280_CTRL_HUM_ADDR, &value, I2C_DATA_LEN);
        if (unlikely(ret)) {
            return ret;
        }
    }

    if(type == bme_280_baro){
        value = 0;
        ret   = bme280_io_read(BME280_PWR_CTRL_ADDR, &value, I2C_DATA_LEN);
        if (unlikely(ret)) {
            return ret;
        }
        value = BME280_SET_BITS(value, BME280_CTRL_PRESS, data);
        ret = bme280_io_write(BME280_PWR_CTRL_ADDR, &value, I2C_DATA_LEN);
        if (unlikely(ret)) {
            return ret;
        }
    }

    return ret;
}


static int drv_bosch_bme280_set_power_mode(dev_power_mode_e mode)
{
    uint8_t value = 0x00;
    int     ret   = 0;

    ret = bme280_io_read(BME280_PWR_CTRL_ADDR, &value, I2C_DATA_LEN);
    if (unlikely(ret)) {
        return ret;
    }

    switch (mode) {
        case DEV_POWER_ON: {
            if ((value & BME280_POWER_BIT_MASK) == BME280_NORMAL_MODE) {
                return 0;
            }
            value |= BME280_NORMAL_MODE;
            ret = bme280_io_write(BME280_PWR_CTRL_ADDR, &value, I2C_DATA_LEN);
            if (unlikely(ret)) {
                return ret;
            }
        } break;

        case DEV_POWER_OFF:
        case DEV_SLEEP: {
            if ((value & BME280_POWER_BIT_MASK) == BME280_SLEEP_MODE) {
                return 0;
            }

            value &= (~BME280_POWER_BIT_MASK);
            ret = bme280_io_write(BME280_PWR_CTRL_ADDR, &value, I2C_DATA_LEN);
            if (unlikely(ret)) {
                return ret;
            }

        } break;

        default:
            break;
    }
    return 0;
}

static uint8_t drv_bosch_bme280_hz2odr(int hz)
{
    if (hz > 80)
        return BME280_STANDBY_TIME_1_MS;
    else if (hz > 13)
        return BME280_STANDBY_TIME_62_5_MS;
    else if (hz > 7)
        return BME280_STANDBY_TIME_125_MS;
    else if (hz > 3)
        return BME280_STANDBY_TIME_250_MS;
    else
        return BME280_STANDBY_TIME_500_MS;
}

static int drv_bosch_bme280_set_odr(uint8_t odr)
{
    int     ret       = 0;
    uint8_t v_data_u8 = 0;

    ret = bme280_io_read(BME280_CONFIG_ADDR, &v_data_u8, I2C_DATA_LEN);
    if (unlikely(ret)) {
        return ret;
    }

    v_data_u8 = BME280_SET_BITS(v_data_u8, BME280_STANDBY, odr);
    ret       = bme280_io_write(BME280_CONFIG_ADDR, &v_data_u8, I2C_DATA_LEN);
    if (unlikely(ret)) {
        return ret;
    }

    return ret;
}

static int drv_bosch_bme280_set_default_config(void)
{
    int ret = 0;

    ret = drv_bosch_bme280_set_power_mode(DEV_SLEEP);
    if (unlikely(ret)) {
        return ret;
    }

    ret = drv_bosch_bme280_set_odr(BME280_DEFAULT_ODR_1HZ);
    if (unlikely(ret)) {
        return ret;
    }

    return 0;
}


static int drv_bosch_bme280_comp_temp(bme280_raw_data_t* rawdata,bme280_comp_data_t* compdata)
{
    int32_t var1;
    int32_t var2;
    int32_t temperature;
    int32_t temperature_min = -4000;
    int32_t temperature_max = 8500;

    if(rawdata == NULL){
        return -1;
    }

    if(compdata == NULL){
        return -1;
    }

    var1 = (int32_t)((rawdata->raw_t / 8) - ((int32_t)g_cali_table.dig_T1 * 2));
    var1 = (var1 * ((int32_t)g_cali_table.dig_T2)) / 2048;
    var2 = (int32_t)((rawdata->raw_t / 16) - ((int32_t)g_cali_table.dig_T1));
    var2 = (((var2 * var2) / 4096) * ((int32_t)g_cali_table.dig_T3)) / 16384;
    g_cali_table.t_fine = var1 + var2;
    temperature         = (g_cali_table.t_fine * 5 + 128) / 256;

    if (temperature < temperature_min)
        temperature = temperature_min;
    else if (temperature > temperature_max)
        temperature = temperature_max;

    compdata->comp_t = (temperature/ BME280_TEMP_UNIT_RATIO_10);

    return 0;
}


static int drv_bosch_bme280_comp_humi(bme280_raw_data_t* rawdata, bme280_comp_data_t* compdata)
{
    int32_t  var1         = 0;
    int32_t  var2         = 0;
    int32_t  var3         = 0;
    int32_t  var4         = 0;
    int32_t  var5         = 0;
    uint32_t humidity     = 0;
    uint32_t humidity_max = 100000;

    if(rawdata == NULL){
        return -1;
    }

    if(compdata == NULL){
        return -1;
    }

    var1     = g_cali_table.t_fine - ((int32_t)76800);
    var2     = (int32_t)((rawdata->raw_h) * 16384);
    var3     = (int32_t)(((int32_t)g_cali_table.dig_H4) * 1048576);
    var4     = ((int32_t)g_cali_table.dig_H5) * var1;
    var5     = (((var2 - var3) - var4) + (int32_t)16384) / 32768;
    var2     = (var1 * ((int32_t)g_cali_table.dig_H6)) / 1024;
    var3     = (var1 * ((int32_t)g_cali_table.dig_H3)) / 2048;
    var4     = ((var2 * (var3 + (int32_t)32768)) / 1024) + (int32_t)2097152;
    var2     = ((var4 * ((int32_t)g_cali_table.dig_H2)) + 8192) / 16384;
    var3     = var5 * var2;
    var4     = ((var3 / 32768) * (var3 / 32768)) / 128;
    var5     = var3 - ((var4 * ((int32_t)g_cali_table.dig_H1)) / 16);
    var5     = (var5 < 0 ? 0 : var5);
    var5     = (var5 > 419430400 ? 419430400 : var5);
    humidity = (uint32_t)(var5 / 4096);

    if (humidity > humidity_max) {
        humidity = humidity_max;
    }
    compdata->comp_h =
      (humidity * BME280_HUMI_UNIT_RATIO_10) / BME280_HUMI_UNIT_RATIO_1024;
    return 0;
}

static int drv_bosch_bme280_comp_baro(bme280_raw_data_t* rawdata, bme280_comp_data_t* compdata)
{
    int32_t  var1         = 0;
    int32_t  var2         = 0;
    int32_t  var3         = 0;
    int32_t  var4         = 0;
    uint32_t var5         = 0;
    uint32_t pressure     = 0;
    uint32_t pressure_min = 30000;
    uint32_t pressure_max = 110000;

    if(rawdata == NULL){
        return -1;
    }

    if(compdata == NULL){
        return -1;
    }

    var1 = (((int32_t)g_cali_table.t_fine) / 2) - (int32_t)64000;
    var2 = (((var1 / 4) * (var1 / 4)) / 2048) * ((int32_t)g_cali_table.dig_P6);
    var2 = var2 + ((var1 * ((int32_t)g_cali_table.dig_P5)) * 2);
    var2 = (var2 / 4) + (((int32_t)g_cali_table.dig_P4) * 65536);
    var3 = (g_cali_table.dig_P3 * (((var1 / 4) * (var1 / 4)) / 8192)) / 8;
    var4 = (((int32_t)g_cali_table.dig_P2) * var1) / 2;
    var1 = (var3 + var4) / 262144;
    var1 = (((32768 + var1)) * ((int32_t)g_cali_table.dig_P1)) / 32768;

    if (var1) {
        var5 = (uint32_t)((uint32_t)1048576) - rawdata->raw_p;
        pressure = ((uint32_t)(var5 - (uint32_t)(var2 / 4096))) * 3125;
        if (pressure < 0x80000000){
            pressure = (pressure << 1) / ((uint32_t)var1);
        }
        else{
            pressure = (pressure / (uint32_t)var1) * 2;
        }

        var1 = (((int32_t)g_cali_table.dig_P9) * ((int32_t)(((pressure / 8) * (pressure / 8)) / 8192))) / 4096;
        var2 = (((int32_t)(pressure / 4)) * ((int32_t)g_cali_table.dig_P8)) / 8192;
        pressure = (uint32_t)((int32_t)pressure + ((var1 + var2 + g_cali_table.dig_P7) / 16));

        if (pressure < pressure_min){
            pressure = pressure_min;
        }
        else if (pressure > pressure_max){
            pressure = pressure_max;
        }
    } else {
        pressure = pressure_min;
    }
    compdata->comp_p = pressure ;

    return 0;
}


static int drv_bosch_bme280_comp(bme280_type_e type, bme280_raw_data_t* rawdata, bme280_comp_data_t* compdata)
{
    int ret = 0;

    if(type >= bme_280_max){
        return -1;
    }

    if(rawdata == NULL){
        return -1;
    }

    if(compdata == NULL){
        return -1;
    }

    ret = drv_bosch_bme280_comp_temp(rawdata,compdata);
    if (unlikely(ret)) {
        return ret;
    }

    if(type == bme_280_humi){
        ret = drv_bosch_bme280_comp_humi(rawdata,compdata);
        if (unlikely(ret)) {
            return ret;
        }
    }

    if(type == bme_280_baro){
        ret = drv_bosch_bme280_comp_baro(rawdata,compdata);
        if (unlikely(ret)) {
            return ret;
        }
    }

    return 0;
}

static int drv_bosch_bme280_read_rawdata(bme280_type_e type, bme280_raw_data_t* rawdata)
{
    int                ret                        = 0;
    uint8_t            data[BME280_TEMP_DATA_LEN] = { 0 };

    if(type >= bme_280_max){
        return -1;
    }

    if(rawdata == NULL){
        return -1;
    }

    rawdata->raw_t = 0;
    rawdata->raw_h = 0;
    rawdata->raw_p = 0;

    ret = bme280_io_read(BME280_TEMP_DATA_ADDR, data, BME280_TEMP_DATA_LEN);
    if (unlikely(ret)) {
        return ret;
    }

    rawdata->raw_t = (uint32_t)((uint32_t)data[0] << BME280_SHIFT_BY_12_BITS) |
             ((uint32_t)data[1] << BME280_SHIFT_BY_04_BITS) |
             ((uint32_t)data[2] >> BME280_SHIFT_BY_04_BITS);

    if(type == bme_280_humi){
        ret = bme280_io_read(BME280_HUMI_DATA_ADDR, data, BME280_HUMI_DATA_LEN);
        if (unlikely(ret)) {
            return ret;
        }
        rawdata->raw_h = ((uint32_t)data[0] << BME280_SHIFT_BY_08_BITS) | ((uint32_t)data[1]);
    }

    if(type == bme_280_baro){
        ret = bme280_io_read(BME280_BARO_DATA_ADDR, data, BME280_BARO_DATA_LEN);
        if (unlikely(ret)) {
            return ret;
        }
        rawdata->raw_p = ((uint32_t)data[0] << BME280_SHIFT_BY_12_BITS) |
                        ((uint32_t)data[1] << BME280_SHIFT_BY_04_BITS) |
                        ((uint32_t)data[2] >> BME280_SHIFT_BY_04_BITS) ;
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
    ret = drv_bosch_bme280_set_power_mode(DEV_SLEEP);
    if (unlikely(ret)) {
        return -1;
    }

    ret = drv_bosch_bme280_set_work_mode(bme_280_humi,BME280_STANDARD_RESOLUTION_MODE);
    if (unlikely(ret)) {
        return -1;
    }

    ret = drv_bosch_bme280_set_power_mode(DEV_POWER_ON);
    if (unlikely(ret)) {
        return -1;
    }

    return 0;
}

static int drv_humi_bosch_bme280_close(void)
{
    int ret = 0;
    g_bme280_open_flag--;
    if(!g_bme280_open_flag){
        ret     = drv_bosch_bme280_set_power_mode(DEV_POWER_OFF);
        if (unlikely(ret)) {
            return -1;
        }
    }
    return 0;
}

static int drv_humi_bosch_bme280_read(void *buf, size_t len)
{
    int ret        = 0;
    size_t  size       = 0;
    bme280_raw_data_t raw_data;
    bme280_comp_data_t comp_data;

    humidity_data_t *pdata = (humidity_data_t *)buf;
    if (buf == NULL) {
        return -1;
    }

    size = sizeof(humidity_data_t);
    if (len < size) {
        return -1;
    }

    ret = drv_bosch_bme280_read_rawdata(bme_280_humi, &raw_data);
    if (unlikely(ret)) {
        return -1;
    }

    ret = drv_bosch_bme280_comp(bme_280_humi, &raw_data, &comp_data);
    if (unlikely(ret)) {
        return -1;
    }

    pdata->h = comp_data.comp_h;
    pdata->timestamp = aos_now_ms();

    return (int)size;
}

static int drv_humi_bosch_bme280_ioctl(int cmd, unsigned long arg)
{
    int ret = 0;

    switch (cmd) {
        case SENSOR_IOCTL_ODR_SET: {
            uint8_t odr = drv_bosch_bme280_hz2odr(arg);
            ret         = drv_bosch_bme280_set_odr(odr);
            if (unlikely(ret)) {
                return -1;
            }
        } break;
        case SENSOR_IOCTL_SET_POWER: {
            ret = drv_bosch_bme280_set_power_mode(arg);
            if (unlikely(ret)) {
                return -1;
            }
        } break;
        case SENSOR_IOCTL_GET_INFO: {
            /* fill the dev info here */
            dev_sensor_info_t *info = (dev_sensor_info_t *)arg;
            info->model             = "BME280";
            info->range_max         = 16;
            info->range_min         = 4;
            info->unit              = permillage;
        } break;

        default:
            break;
    }

    return 0;
}

int drv_humi_bosch_bme280_init(void)
{
    int          ret = 0;
    sensor_obj_t sensor;

    memset(&sensor, 0, sizeof(sensor));

    /* fill the sensor obj parameters here */
    sensor.io_port    = BME280_IO_PORT;
    sensor.tag        = TAG_DEV_HUMI;
    sensor.path       = dev_humi_path;
    sensor.open       = drv_humi_bosch_bme280_open;
    sensor.close      = drv_humi_bosch_bme280_close;
    sensor.read       = drv_humi_bosch_bme280_read;
    sensor.write      = NULL;
    sensor.ioctl      = drv_humi_bosch_bme280_ioctl;
    sensor.irq_handle = drv_humi_bosch_bme280_irq_handle;

    ret = sensor_create_obj(&sensor);
    if (unlikely(ret)) {
        return -1;
    }
    if (!g_bme280_init_flag){
        ret = drv_bosch_bme280_io_init(sensor.io_port);
        if (unlikely(ret)) {
            return -1;
        }

        ret = drv_bosch_bme280_validate_id(BME280_CHIP_ID);
        if (unlikely(ret)) {
            return -1;
        }

        ret = drv_bosch_bme280_soft_reset();
        if (unlikely(ret)) {
            return -1;
        }
        aos_msleep(5);

        ret = drv_bosch_bme280_set_default_config();
        if (unlikely(ret)) {
            return -1;
        }
        aos_msleep(5);

        ret = drv_bosch_bme280_get_cali_parm();
        if (unlikely(ret)) {
            return -1;
        }

        g_bme280_init_flag = 1;
    }
    /* update the phy sensor info to sensor hal */
    LOG("%s %s successfully \n", SENSOR_STR, __func__);
    return 0;
}

static void drv_temp_bosch_bme280_irq_handle(void)
{
    /* no handle so far */
}

static int drv_temp_bosch_bme280_open(void)
{
    int ret = 0;

    ret = drv_bosch_bme280_set_power_mode(DEV_SLEEP);
    if (unlikely(ret)) {
        return -1;
    }


    ret = drv_bosch_bme280_set_work_mode(bme_280_temp,BME280_STANDARD_RESOLUTION_MODE);
    if (unlikely(ret)) {
        return -1;
    }

    ret = drv_bosch_bme280_set_power_mode(DEV_POWER_ON);
    if (unlikely(ret)) {
        return -1;
    }

    return 0;
}

static int drv_temp_bosch_bme280_close(void)
{
    int ret = 0;

    g_bme280_open_flag--;
    if(!g_bme280_open_flag){
        ret     = drv_bosch_bme280_set_power_mode(DEV_POWER_OFF);
        if (unlikely(ret)) {
            return -1;
        }
    }
    return 0;
}

static int drv_temp_bosch_bme280_read(void *buf, size_t len)
{
    int ret        = 0;
    size_t  size       = 0;
    bme280_raw_data_t raw_data;
    bme280_comp_data_t comp_data;

    temperature_data_t *pdata = (temperature_data_t *)buf;
    if (buf == NULL) {
        return -1;
    }
    size = sizeof(temperature_data_t);
    if (len < size) {
        return -1;
    }

    ret = drv_bosch_bme280_read_rawdata(bme_280_temp, &raw_data);
    if (unlikely(ret)) {
        return -1;
    }

    ret = drv_bosch_bme280_comp(bme_280_temp, &raw_data, &comp_data);
    if (unlikely(ret)) {
        return -1;
    }

    pdata->t = comp_data.comp_t;
    pdata->timestamp = aos_now_ms();
    return (int)size;
}

static int drv_temp_bosch_bme280_ioctl(int cmd, unsigned long arg)
{
    int ret = 0;

    switch (cmd) {
        case SENSOR_IOCTL_ODR_SET: {
            uint8_t odr = drv_bosch_bme280_hz2odr(arg);
            ret         = drv_bosch_bme280_set_odr(odr);
            if (unlikely(ret)) {
                return -1;
            }
        } break;
        case SENSOR_IOCTL_SET_POWER: {
            ret = drv_bosch_bme280_set_power_mode(arg);
            if (unlikely(ret)) {
                return -1;
            }
        } break;
        case SENSOR_IOCTL_GET_INFO: {
            /* fill the dev info here */
            dev_sensor_info_t *info = (dev_sensor_info_t *)arg;
            info->model             = "BME280";
            info->range_max         = 85;
            info->range_min         = 0;
            info->unit              = dCelsius;
        } break;

        default:
            break;
    }

    return 0;
}

int drv_temp_bosch_bme280_init(void)
{
    int          ret = 0;
    sensor_obj_t sensor;
#if 1
    memset(&sensor, 0, sizeof(sensor));

    /* fill the sensor obj parameters here */
    sensor.io_port    = BME280_IO_PORT;
    sensor.tag        = TAG_DEV_TEMP;
    sensor.path       = dev_temp_path;
    sensor.open       = drv_temp_bosch_bme280_open;
    sensor.close      = drv_temp_bosch_bme280_close;
    sensor.read       = drv_temp_bosch_bme280_read;
    sensor.write      = NULL;
    sensor.ioctl      = drv_temp_bosch_bme280_ioctl;
    sensor.irq_handle = drv_temp_bosch_bme280_irq_handle;

    ret = sensor_create_obj(&sensor);
    if (unlikely(ret)) {
        return -1;
    }
    if (!g_bme280_init_flag){
        ret = drv_bosch_bme280_io_init(sensor.io_port);
        if (unlikely(ret)) {
            return -1;
        }

        ret = drv_bosch_bme280_validate_id(BME280_CHIP_ID);
        if (unlikely(ret)) {
            return -1;
        }

        ret = drv_bosch_bme280_soft_reset();
        if (unlikely(ret)) {
            return -1;
        }
        aos_msleep(5);

        ret = drv_bosch_bme280_set_default_config();
        if (unlikely(ret)) {
            return -1;
        }
        aos_msleep(5);

        ret = drv_bosch_bme280_get_cali_parm();
        if (unlikely(ret)) {
            return -1;
        }

        g_bme280_init_flag = 1;
    }

    #endif
    /* update the phy sensor info to sensor hal */
    LOG("%s %s successfully \n", SENSOR_STR, __func__);
    return 0;
}



static void drv_baro_bosch_bme280_irq_handle(void)
{
    /* no handle so far */
}

static int drv_baro_bosch_bme280_open(void)
{
    int ret = 0;

    ret = drv_bosch_bme280_set_power_mode(DEV_SLEEP);
    if (unlikely(ret)) {
        return -1;
    }

    ret = drv_bosch_bme280_set_work_mode(bme_280_baro,BME280_STANDARD_RESOLUTION_MODE);
    if (unlikely(ret)) {
        return -1;
    }

    ret = drv_bosch_bme280_set_power_mode(DEV_POWER_ON);
    if (unlikely(ret)) {
        return -1;
    }

    return 0;
}

static int drv_baro_bosch_bme280_close(void)
{
    int ret = 0;
    g_bme280_open_flag--;
    if(!g_bme280_open_flag){
        ret     = drv_bosch_bme280_set_power_mode(DEV_POWER_OFF);
        if (unlikely(ret)) {
            return -1;
        }
    }
    return 0;
}

static int drv_baro_bosch_bme280_read(void *buf, size_t len)
{
    int ret        = 0;
    size_t  size       = 0;
    bme280_raw_data_t raw_data;
    bme280_comp_data_t comp_data;

    barometer_data_t *pdata = (barometer_data_t *)buf;
    if (buf == NULL) {
        return -1;
    }

    size = sizeof(barometer_data_t);
    if (len < size) {
        return -1;
    }

    ret = drv_bosch_bme280_read_rawdata(bme_280_baro, &raw_data);
    if (unlikely(ret)) {
        return -1;
    }

    ret = drv_bosch_bme280_comp(bme_280_baro, &raw_data, &comp_data);
    if (unlikely(ret)) {
        return -1;
    }

    pdata->p = comp_data.comp_p;
    pdata->timestamp = aos_now_ms();

    return (int)size;
}

static int drv_baro_bosch_bme280_ioctl(int cmd, unsigned long arg)
{
    int ret = 0;

    switch (cmd) {
        case SENSOR_IOCTL_ODR_SET: {
            uint8_t odr = drv_bosch_bme280_hz2odr(arg);
            ret         = drv_bosch_bme280_set_odr(odr);
            if (unlikely(ret)) {
                return -1;
            }
        } break;
        case SENSOR_IOCTL_SET_POWER: {
            ret = drv_bosch_bme280_set_power_mode(arg);
            if (unlikely(ret)) {
                return -1;
            }
        } break;
        case SENSOR_IOCTL_GET_INFO: {
            /* fill the dev info here */
            dev_sensor_info_t *info = (dev_sensor_info_t *)arg;
            info->model             = "BME280";
            info->range_max         = 16;
            info->range_min         = 4;
            info->unit              = permillage;
        } break;

        default:
            break;
    }

    return 0;
}
int drv_baro_bosch_bme280_init(void)
{
    int          ret = 0;
    sensor_obj_t sensor;

    memset(&sensor, 0, sizeof(sensor));

    /* fill the sensor obj parameters here */
    sensor.io_port    = BME280_IO_PORT;
    sensor.tag        = TAG_DEV_BARO;
    sensor.path       = dev_baro_path;
    sensor.open       = drv_baro_bosch_bme280_open;
    sensor.close      = drv_baro_bosch_bme280_close;
    sensor.read       = drv_baro_bosch_bme280_read;
    sensor.write      = NULL;
    sensor.ioctl      = drv_baro_bosch_bme280_ioctl;
    sensor.irq_handle = drv_baro_bosch_bme280_irq_handle;

    ret = sensor_create_obj(&sensor);
    if (unlikely(ret)) {
        return -1;
    }
    if (!g_bme280_init_flag){

        ret = drv_bosch_bme280_io_init(sensor.io_port);
        if (unlikely(ret)) {
            return -1;
        }

        ret = drv_bosch_bme280_validate_id(BME280_CHIP_ID);
        if (unlikely(ret)) {
            return -1;
        }

        ret = drv_bosch_bme280_soft_reset();
        if (unlikely(ret)) {
            return -1;
        }
        aos_msleep(5);

        ret = drv_bosch_bme280_set_default_config();
        if (unlikely(ret)) {
            return -1;
        }
        aos_msleep(5);

        ret = drv_bosch_bme280_get_cali_parm();
        if (unlikely(ret)) {
            return -1;
        }

        g_bme280_init_flag = 1;
    }
    /* update the phy sensor info to sensor hal */
    LOG("%s %s successfully \n", SENSOR_STR, __func__);
    return 0;
}

SENSOR_DRV_ADD(drv_temp_bosch_bme280_init);
SENSOR_DRV_ADD(drv_humi_bosch_bme280_init);
SENSOR_DRV_ADD(drv_baro_bosch_bme280_init);


