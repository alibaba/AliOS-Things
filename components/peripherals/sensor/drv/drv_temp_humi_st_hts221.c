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

/* ST humi SENSOR REGISTER MAP */
#define HTS221_BIT(x)              ((uint8_t)x)
#define HTS221_I2C_ADDR1           (0x5F)
#define HTS221_I2C_ADDR_TRANS(n)   ((n) << 1)
#define HTS221_I2C_ADDR            HTS221_I2C_ADDR_TRANS(HTS221_I2C_ADDR1)
#define HTS221_DriverVersion_Major (uint8_t)1
#define HTS221_DriverVersion_Minor (uint8_t)0
#define HTS221_DriverVersion_Point (uint8_t)0

// who Am I
#define HTS221_WHO_AM_I_REG (uint8_t)0x0F
#define HTS221_WHO_AM_I_VAL (uint8_t)0xBC

// brief Humidity and temperature average mode
#define HTS221_AV_CONF_REG (uint8_t)0x10
#define HTS221_AVGT_BIT    HTS221_BIT(3)
#define HTS221_AVGH_BIT    HTS221_BIT(0)
#define HTS221_AVGT_MASK   (uint8_t)0x38
#define HTS221_AVGH_MASK   (uint8_t)0x07

// brief Control register 1.
#define HTS221_CTRL_REG1 (uint8_t)0x20
#define HTS221_PD_BIT    HTS221_BIT(7)
#define HTS221_BDU_BIT   HTS221_BIT(2)
#define HTS221_ODR_BIT   HTS221_BIT(0)
#define HTS221_PD_MASK   (uint8_t)0x80
#define HTS221_BDU_MASK  (uint8_t)0x04
#define HTS221_ODR_MASK  (uint8_t)0x03

// brief Control register 2.
#define HTS221_CTRL_REG2     (uint8_t)0x21
#define HTS221_BOOT_BIT      HTS221_BIT(7)
#define HTS221_HEATHER_BIT   HTS221_BIT(1)
#define HTS221_ONESHOT_BIT   HTS221_BIT(0)
#define HTS221_BOOT_MASK     (uint8_t)0x80
#define HTS221_HEATHER_MASK  (uint8_t)0x02
#define HTS221_ONE_SHOT_MASK (uint8_t)0x01

// brief Control register 3.
#define HTS221_CTRL_REG3     (uint8_t)0x22
#define HTS221_DRDY_H_L_BIT  HTS221_BIT(7)
#define HTS221_PP_OD_BIT     HTS221_BIT(6)
#define HTS221_DRDY_BIT      HTS221_BIT(2)
#define HTS221_DRDY_H_L_MASK (uint8_t)0x80
#define HTS221_PP_OD_MASK    (uint8_t)0x40
#define HTS221_DRDY_MASK     (uint8_t)0x04

// brief Status register.
#define HTS221_STATUS_REG (uint8_t)0x27
#define HTS221_H_DA_BIT   HTS221_BIT(1)
#define HTS221_T_DA_BIT   HTS221_BIT(0)
#define HTS221_HDA_MASK   (uint8_t)0x02
#define HTS221_TDA_MASK   (uint8_t)0x01

// brief Humidity data (LSB)
#define HTS221_HR_OUT_L_REG (uint8_t)0x28

// brief Humidity data (MSB)
#define HTS221_HR_OUT_H_REG (uint8_t)0x29

// brief Temperature data (LSB)
#define HTS221_TEMP_OUT_L_REG (uint8_t)0x2A

// brief Temperature data (MSB)
#define HTS221_TEMP_OUT_H_REG (uint8_t)0x2B

// brief Calibration registers
#define HTS221_H0_RH_X2      (uint8_t)0x30
#define HTS221_H1_RH_X2      (uint8_t)0x31
#define HTS221_T0_DEGC_X8    (uint8_t)0x32
#define HTS221_T1_DEGC_X8    (uint8_t)0x33
#define HTS221_T0_T1_DEGC_H2 (uint8_t)0x35
#define HTS221_H0_T0_OUT_L   (uint8_t)0x36
#define HTS221_H0_T0_OUT_H   (uint8_t)0x37
#define HTS221_H1_T0_OUT_L   (uint8_t)0x3A
#define HTS221_H1_T0_OUT_H   (uint8_t)0x3B
#define HTS221_T0_OUT_L      (uint8_t)0x3C
#define HTS221_T0_OUT_H      (uint8_t)0x3D
#define HTS221_T1_OUT_L      (uint8_t)0x3E
#define HTS221_T1_OUT_H      (uint8_t)0x3F


typedef enum
{
    HTS221_ODR_ONE_SHOT = (uint8_t)0x00, /*!< Output Data Rate: one shot */
    HTS221_ODR_1HZ      = (uint8_t)0x01, /*!< Output Data Rate: 1Hz */
    HTS221_ODR_7HZ      = (uint8_t)0x02, /*!< Output Data Rate: 7Hz */
    HTS221_ODR_12_5HZ   = (uint8_t)0x03, /*!< Output Data Rate: 12_5Hz */
} HTS221_odr_e;

typedef enum
{
    HTS221_BDU_CONTINUOUS_UPDATE =
      (uint8_t)0x00, /*!< Data updated continuously */
    HTS221_BDU_NO_UPDATE =
      (uint8_t)0x04 /*!< Data updated after a read operation */
} HTS221_bdu_e;

i2c_dev_t HTS221_ctx = {
    .port                 = 1,
    .config.address_width = 8,
    .config.freq          = 400000,
    .config.dev_addr      = HTS221_I2C_ADDR,
};

static HTS221_odr_e drv_temp_humi_st_hts221_hz2odr(int hz)
{
    if (hz > 10)
        return HTS221_ODR_12_5HZ;
    else if (hz > 5)
        return HTS221_ODR_7HZ;
    else
        return HTS221_ODR_1HZ;
}

static int drv_temp_humi_st_hts221_validate_id(i2c_dev_t *drv, uint8_t id_value)
{
    uint8_t value = 0x00;
    int     ret   = 0;

    if (drv == NULL) {
        return -1;
    }

    ret = sensor_i2c_read(drv, HTS221_WHO_AM_I_REG, &value, I2C_DATA_LEN,
                          I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    if (id_value != value) {
        return -1;
    }
    return 0;
}

static int drv_temp_humi_st_hts221_set_power_mode(i2c_dev_t       *drv,
                                                  dev_power_mode_e mode)
{
    uint8_t value = 0x00;
    int     ret   = 0;

    ret = sensor_i2c_read(drv, HTS221_CTRL_REG1, &value, I2C_DATA_LEN,
                          I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    switch (mode) {
        case DEV_POWER_ON: {
            value |= HTS221_PD_MASK;
            ret = sensor_i2c_write(drv, HTS221_CTRL_REG1, &value, I2C_DATA_LEN,
                                   I2C_OP_RETRIES);
            if (unlikely(ret)) {
                return ret;
            }
        } break;

        case DEV_POWER_OFF: {
            value &= ~HTS221_PD_MASK;
            ret = sensor_i2c_write(drv, HTS221_CTRL_REG1, &value, I2C_DATA_LEN,
                                   I2C_OP_RETRIES);
            if (unlikely(ret)) {
                return ret;
            }
        } break;

        default:
            break;
    }
    return 0;
}

static int drv_temp_humi_st_hts221_set_odr(i2c_dev_t *drv, HTS221_odr_e odr)
{
    uint8_t value = 0x00;
    int     ret   = 0;

    ret = sensor_i2c_read(drv, HTS221_CTRL_REG1, &value, I2C_DATA_LEN,
                          I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }
    value &= ~HTS221_ODR_MASK;
    value |= (uint8_t)odr;

    ret = sensor_i2c_write(drv, HTS221_CTRL_REG1, &value, I2C_DATA_LEN,
                           I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }
    return 0;
}

static int drv_temp_humi_st_hts221_set_bdu(i2c_dev_t *drv, HTS221_bdu_e bdu)
{
    uint8_t value = 0x00;
    int     ret   = 0;

    ret = sensor_i2c_read(drv, HTS221_CTRL_REG1, &value, I2C_DATA_LEN,
                          I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }
    value &= ~HTS221_BDU_MASK;
    value |= (uint8_t)bdu;

    ret = sensor_i2c_write(drv, HTS221_CTRL_REG1, &value, I2C_DATA_LEN,
                           I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }
    return 0;
}

static int drv_temp_humi_st_hts221_set_default_config(i2c_dev_t *drv)
{
    int     ret   = 0;

    ret = drv_temp_humi_st_hts221_set_power_mode(drv, DEV_POWER_OFF);
    if (unlikely(ret)) {
        return ret;
    }
    ret = drv_temp_humi_st_hts221_set_odr(drv, HTS221_ODR_1HZ);
    if (unlikely(ret)) {
        return ret;
    }

    ret = drv_temp_humi_st_hts221_set_bdu(drv, HTS221_BDU_NO_UPDATE);
    if (unlikely(ret)) {
        return ret;
    }
    /* you also can set the low-pass filter and cut off config here */
    return 0;
}


static void drv_temp_st_hts221_irq_handle(void)
{
    /* no handle so far */
}

static int drv_temp_st_hts221_open(void)
{
    int ret = 0;
    ret     = drv_temp_humi_st_hts221_set_power_mode(&HTS221_ctx, DEV_POWER_ON);
    if (unlikely(ret)) {
        return -1;
    }
    LOG("%s %s successfully \n", SENSOR_STR, __func__);
    return 0;
}

static int drv_temp_st_hts221_close(void)
{
    int ret = 0;
    ret = drv_temp_humi_st_hts221_set_power_mode(&HTS221_ctx, DEV_POWER_OFF);
    if (unlikely(ret)) {
        return -1;
    }
    LOG("%s %s successfully \n", SENSOR_STR, __func__);
    return 0;
}


static int drv_temp_st_hts221_read(void *buf, size_t len)
{
    int                 ret = 0;
    size_t              size;
    uint8_t             data[2], buffer[4], tmp;
    int16_t             T0_out, T1_out, T_out, T0_degC_x8_u16, T1_degC_x8_u16;
    int16_t             T0_degC, T1_degC;
    int16_t             temp;
    temperature_data_t *pdata = (temperature_data_t *)buf;
    if (buf == NULL) {
        return -1;
    }

    size = sizeof(temperature_data_t);
    if (len < size) {
        return -1;
    }

    ret = sensor_i2c_read(&HTS221_ctx, (HTS221_T0_DEGC_X8 | 0x80), buffer, 2,
                          I2C_OP_RETRIES);
    ret = sensor_i2c_read(&HTS221_ctx, HTS221_T0_T1_DEGC_H2, &tmp, I2C_DATA_LEN,
                          I2C_OP_RETRIES);
    T0_degC_x8_u16 = (((uint16_t)(tmp & 0x03)) << 8) | ((uint16_t)buffer[0]);
    T1_degC_x8_u16 = (((uint16_t)(tmp & 0x0C)) << 6) | ((uint16_t)buffer[1]);
    T0_degC        = T0_degC_x8_u16 >> 3;
    T1_degC        = T1_degC_x8_u16 >> 3;
    ret |= sensor_i2c_read(&HTS221_ctx, (HTS221_T0_OUT_L | 0x80), buffer, 4,
                           I2C_OP_RETRIES);
    T0_out = (((uint16_t)buffer[1]) << 8) | (uint16_t)buffer[0];
    T1_out = (((uint16_t)buffer[3]) << 8) | (uint16_t)buffer[2];
    ret |= sensor_i2c_read(&HTS221_ctx, (HTS221_TEMP_OUT_L_REG | 0x80), buffer,
                           2, I2C_OP_RETRIES);
    T_out = (((uint16_t)buffer[1]) << 8) | (uint16_t)buffer[0];

    temp = 10 * (T_out - T0_out) * (T1_degC - T0_degC) / (T1_out - T0_out) +
           10 * T0_degC;

    data[0] = temp & 0x0FF;
    data[1] = temp >> 8;

    if (unlikely(ret)) {
        return -1;
    }

    /* hatch the temp data here*/
    for (int i = 0; i < 2; i++) {
        pdata->t |= (((uint32_t)data[i]) << (8 * i));
    }

    pdata->timestamp = aos_now_ms();
    return (int)size;
}

static int drv_temp_st_hts221_write(const void *buf, size_t len)
{
    return 0;
}

static int drv_temp_st_hts221_ioctl(int cmd, unsigned long arg)
{
    int ret = 0;

    switch (cmd) {
        case SENSOR_IOCTL_ODR_SET: {
            HTS221_odr_e odr = drv_temp_humi_st_hts221_hz2odr(arg);
            ret = drv_temp_humi_st_hts221_set_odr(&HTS221_ctx, odr);
            if (unlikely(ret)) {
                return -1;
            }
        } break;
        case SENSOR_IOCTL_SET_POWER: {
            ret = drv_temp_humi_st_hts221_set_power_mode(&HTS221_ctx, arg);
            if (unlikely(ret)) {
                return -1;
            }
        } break;
        case SENSOR_IOCTL_GET_INFO: {
            /* fill the dev info here */
            dev_sensor_info_t *info = (dev_sensor_info_t *)arg;
            info->model             = "HTS221";
            info->range_max         = 120;
            info->range_min         = -40;
            info->unit              = dCelsius;

        } break;

        default:
            break;
    }

    LOG("%s %s successfully \n", SENSOR_STR, __func__);
    return 0;
}

int drv_temp_st_hts221_init(void)
{
    int          ret = 0;
    sensor_obj_t sensor;

    memset(&sensor, 0, sizeof(sensor));

    /* fill the sensor obj parameters here */
    sensor.tag        = TAG_DEV_TEMP;
    sensor.path       = dev_temp_path;
    sensor.io_port    = I2C_PORT;
    sensor.open       = drv_temp_st_hts221_open;
    sensor.close      = drv_temp_st_hts221_close;
    sensor.read       = drv_temp_st_hts221_read;
    sensor.write      = drv_temp_st_hts221_write;
    sensor.ioctl      = drv_temp_st_hts221_ioctl;
    sensor.irq_handle = drv_temp_st_hts221_irq_handle;


    ret = sensor_create_obj(&sensor);
    if (unlikely(ret)) {
        return -1;
    }

    ret = drv_temp_humi_st_hts221_validate_id(&HTS221_ctx, HTS221_WHO_AM_I_VAL);
    if (unlikely(ret)) {
        return -1;
    }

    /* set the default config for the sensor here */
    ret = drv_temp_humi_st_hts221_set_default_config(&HTS221_ctx);
    if (unlikely(ret)) {
        return -1;
    }

    LOG("%s %s successfully \n", SENSOR_STR, __func__);

    return 0;
}


static void drv_humi_st_hts221_irq_handle(void)
{
    /* no handle so far */
}

static int drv_humi_st_hts221_open(void)
{
    int ret = 0;
    ret     = drv_temp_humi_st_hts221_set_power_mode(&HTS221_ctx, DEV_POWER_ON);
    if (unlikely(ret)) {
        return -1;
    }
    LOG("%s %s successfully \n", SENSOR_STR, __func__);
    return 0;
}

static int drv_humi_st_hts221_close(void)
{
    int ret = 0;
    ret = drv_temp_humi_st_hts221_set_power_mode(&HTS221_ctx, DEV_POWER_OFF);
    if (unlikely(ret)) {
        return -1;
    }
    LOG("%s %s successfully \n", SENSOR_STR, __func__);
    return 0;
}


static int drv_humi_st_hts221_read(void *buf, size_t len)
{
    int              ret = 0;
    size_t           size;
    uint8_t          data[2], buffer[2];
    int16_t          H0_T0_out, H1_T0_out, H_T_out;
    int16_t          H0_rh, H1_rh;
    int16_t          humi;
    humidity_data_t *pdata = (humidity_data_t *)buf;
    if (buf == NULL) {
        return -1;
    }

    size = sizeof(humidity_data_t);
    if (len < size) {
        return -1;
    }

    ret = sensor_i2c_read(&HTS221_ctx, (HTS221_H0_RH_X2 | 0x80), buffer, 2,
                          I2C_OP_RETRIES);

    H0_rh = buffer[0] >> 1;
    H1_rh = buffer[1] >> 1;

    ret |= sensor_i2c_read(&HTS221_ctx, (HTS221_H0_T0_OUT_L | 0x80), buffer, 2,
                           I2C_OP_RETRIES);
    H0_T0_out = (((uint16_t)buffer[1]) << 8) | (uint16_t)buffer[0];

    ret |= sensor_i2c_read(&HTS221_ctx, (HTS221_H1_T0_OUT_L | 0x80), buffer, 2,
                           I2C_OP_RETRIES);
    H1_T0_out = (((uint16_t)buffer[1]) << 8) | (uint16_t)buffer[0];

    ret |= sensor_i2c_read(&HTS221_ctx, (HTS221_HR_OUT_L_REG | 0x80), buffer, 2,
                           I2C_OP_RETRIES);
    H_T_out = (((uint16_t)buffer[1]) << 8) | (uint16_t)buffer[0];

    humi =
      (H_T_out - H0_T0_out) * (H1_rh - H0_rh) / (H1_T0_out - H0_T0_out) + H0_rh;
    humi *= 10;
    humi = (humi > 1000) ? 1000 : (humi < 0) ? 0 : humi;

    humi /= 10;

    data[1] = humi >> 8;
    data[0] = humi & 0x0FF;

    if (unlikely(ret)) {
        return -1;
    }

    /* hatch the humi data here*/
    for (int i = 0; i < 2; i++) {
        pdata->h |= (((uint32_t)data[i]) << (8 * i));
    }
    pdata->timestamp = aos_now_ms();

    return (int)size;
}

static int drv_humi_st_hts221_write(const void *buf, size_t len)
{
    return 0;
}

static int drv_humi_st_hts221_ioctl(int cmd, unsigned long arg)
{
    int ret = 0;

    switch (cmd) {
        case SENSOR_IOCTL_ODR_SET: {
            HTS221_odr_e odr = drv_temp_humi_st_hts221_hz2odr(arg);
            ret = drv_temp_humi_st_hts221_set_odr(&HTS221_ctx, odr);
            if (unlikely(ret)) {
                return -1;
            }
        } break;
        case SENSOR_IOCTL_SET_POWER: {
            ret = drv_temp_humi_st_hts221_set_power_mode(&HTS221_ctx, arg);
            if (unlikely(ret)) {
                return -1;
            }
        } break;
        case SENSOR_IOCTL_GET_INFO: {
            /* fill the dev info here */
            dev_sensor_info_t *info = (dev_sensor_info_t *)arg;
            info->model             = "HTS221";
            info->range_max         = 100;
            info->range_min         = 0;
            info->unit              = permillage;

        } break;

        default:
            break;
    }

    LOG("%s %s successfully \n", SENSOR_STR, __func__);
    return 0;
}


int drv_humi_st_hts221_init(void)
{

    int          ret = 0;
    sensor_obj_t sensor;

    memset(&sensor, 0, sizeof(sensor));
    /* fill the sensor obj parameters here */
    sensor.tag        = TAG_DEV_HUMI;
    sensor.path       = dev_humi_path;
    sensor.io_port    = I2C_PORT;
    sensor.open       = drv_humi_st_hts221_open;
    sensor.close      = drv_humi_st_hts221_close;
    sensor.read       = drv_humi_st_hts221_read;
    sensor.write      = drv_humi_st_hts221_write;
    sensor.ioctl      = drv_humi_st_hts221_ioctl;
    sensor.irq_handle = drv_humi_st_hts221_irq_handle;


    ret = sensor_create_obj(&sensor);
    if (unlikely(ret)) {
        return -1;
    }

    ret = drv_temp_humi_st_hts221_validate_id(&HTS221_ctx, HTS221_WHO_AM_I_VAL);
    if (unlikely(ret)) {
        return -1;
    }

    /* set the default config for the sensor here */
    ret = drv_temp_humi_st_hts221_set_default_config(&HTS221_ctx);
    if (unlikely(ret)) {
        return -1;
    }

    LOG("%s %s successfully \n", SENSOR_STR, __func__);

    return 0;
}

SENSOR_DRV_ADD(drv_temp_st_hts221_init);
SENSOR_DRV_ADD(drv_humi_st_hts221_init);
