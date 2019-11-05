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

/* ADI Temperature SENSOR REGISTER MAP */
#define ADT7410_BIT(x) ((uint8_t)x)
#define ADT7410_I2C_ADDR (0x48)
//#define ADT7410_I2C_ADDR_TRANS(n)    ((n)<<1)
//#define ADT7410_I2C_ADDR ADT7410_I2C_ADDR_TRANS(ADT7410_I2C_ADDR1)

// Who Am I
#define ADT7410_WHO_AM_I_REG (uint8_t)0x0B
#define ADT7410_WHO_AM_I_VAL (uint8_t)0xCB

// ADI Temperature REGISTER
#define ADT7410_TEMPREG_MSB (uint8_t)0x00
#define ADT7410_TEMPREG_LSB (uint8_t)0x01
#define ADT7410_STATUS (uint8_t)0x02
#define ADT7410_CONFIG (uint8_t)0x03
#define ADT7410_THIGH_MSB (uint8_t)0x04
#define ADT7410_THIGH_LSB (uint8_t)0x05
#define ADT7410_TLOW_MSB (uint8_t)0x06
#define ADT7410_TLOW_LSB (uint8_t)0x07
#define ADT7410_TCRIT_MSB (uint8_t)0x08
#define ADT7410_TCRIT_LSB (uint8_t)0x09
#define ADT7410_THYST (uint8_t)0x0A
#define ADT7410_SOFT_RESET (uint8_t)0x2F


typedef enum
{
    ADT7410_OD_13bits = (uint8_t)0x00, /*!< Output Data : 13bits */
    ADT7410_OD_16bits = (uint8_t)0x01, /*!< Output Data : 16bits */
} adt7410_od_resolution;

typedef enum
{
    ADT7410_OPMODE_CONTINUOUS =
      (uint8_t)0x00,                        /*!< Operation Mode continuously */
    ADT7410_OPMODE_ONESHOT = (uint8_t)0x01, /*!< Operation Mode one shot */
    ADT7410_OPMODE_FASTONESHOT =
      (uint8_t)0x02,                        /*!< Operation Mode fast one shot */
    ADT7410_OPMODE_SHUTDOWN = (uint8_t)0x03 /*!< Operation Mode shut down */
} adt7410_opmode_s;

i2c_dev_t adt7410_ctx = {
    .port                 = 3,
    .config.address_width = 8,
    .config.dev_addr      = ADT7410_I2C_ADDR << 1,
};

static int drv_temp_adi_adt7410_validate_id(i2c_dev_t *drv, uint8_t id_value)
{
    uint8_t value = 0x00;
    int     ret   = 0;

    if (drv == NULL) {
        return -1;
    }

    ret = sensor_i2c_read(drv, ADT7410_WHO_AM_I_REG, &value, I2C_DATA_LEN,
                          I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    if (id_value != value) {
        return -1;
    }
    return 0;
}

static int drv_temp_adi_adt7410_set_power_mode(i2c_dev_t *      drv,
                                               dev_power_mode_e mode)
{
    uint8_t value = 0x00;
    int     ret   = 0;

    ret = sensor_i2c_read(drv, ADT7410_CONFIG, &value, I2C_DATA_LEN,
                          I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    switch (mode) {
        case DEV_POWER_ON: {
            value &= ~(0x03 << 5);
            value |= (ADT7410_OPMODE_CONTINUOUS << 5);
            ret = sensor_i2c_write(drv, ADT7410_CONFIG, &value, I2C_DATA_LEN,
                                   I2C_OP_RETRIES);
            if (unlikely(ret)) {
                return ret;
            }
        } break;

        case DEV_POWER_OFF: {
            value &= ~(0x03 << 5);
            value |= (ADT7410_OPMODE_SHUTDOWN << 5);
            ret = sensor_i2c_write(drv, ADT7410_CONFIG, &value, I2C_DATA_LEN,
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

static int drv_temp_adi_adt7410_set_od_res(i2c_dev_t *           drv,
                                           adt7410_od_resolution odr)
{
    uint8_t value = 0x00;
    int     ret   = 0;

    ret = sensor_i2c_read(drv, ADT7410_CONFIG, &value, I2C_DATA_LEN,
                          I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }
    value &= ~0x80;
    value |= (uint8_t)(odr << 7);

    ret = sensor_i2c_write(drv, ADT7410_CONFIG, &value, I2C_DATA_LEN,
                           I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }
    return 0;
}

static int drv_temp_adi_adt7410_set_default_config(i2c_dev_t *drv)
{
    int     ret   = 0;

    ret = drv_temp_adi_adt7410_set_power_mode(drv, DEV_POWER_OFF);
    if (unlikely(ret)) {
        return ret;
    }
    ret = drv_temp_adi_adt7410_set_od_res(drv, ADT7410_OD_16bits);
    if (unlikely(ret)) {
        return ret;
    }

    /* you also can set the low-pass filter and cut off config here */
    return 0;
}


static void drv_temp_adi_adt7410_irq_handle(void)
{
    /* no handle so far */
}

static int drv_temp_adi_adt7410_open(void)
{
    int ret = 0;
    ret     = drv_temp_adi_adt7410_set_power_mode(&adt7410_ctx, DEV_POWER_ON);
    if (unlikely(ret)) {
        return -1;
    }
    LOG("%s %s successfully \n", SENSOR_STR, __func__);
    return 0;
}

static int drv_temp_adi_adt7410_close(void)
{
    int ret = 0;
    ret     = drv_temp_adi_adt7410_set_power_mode(&adt7410_ctx, DEV_POWER_OFF);
    if (unlikely(ret)) {
        return -1;
    }
    LOG("%s %s successfully \n", SENSOR_STR, __func__);
    return 0;
}


static int drv_temp_adi_adt7410_read(void *buf, size_t len)
{
    int                 ret = 0;
    size_t              size;
    uint8_t             data[2], buffer[2];
    int16_t             tmp;
    int16_t             temp;
    temperature_data_t *pdata = (temperature_data_t *)buf;
    if (buf == NULL) {
        return -1;
    }

    size = sizeof(temperature_data_t);
    if (len < size) {
        return -1;
    }

    ret = sensor_i2c_read(&adt7410_ctx, ADT7410_TEMPREG_MSB, buffer, 2,
                          I2C_OP_RETRIES);
    tmp = (((uint16_t)buffer[0]) << 8) | buffer[1];
    if (tmp > 0)
        temp = (tmp * 10) / 128;
    else
        temp = ((tmp - 65536) * 10) / 128;
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

static int drv_temp_adi_adt7410_write(const void *buf, size_t len)
{
    return 0;
}

static int drv_temp_adi_adt7410_ioctl(int cmd, unsigned long arg)
{
    int ret = 0;

    switch (cmd) {
        case SENSOR_IOCTL_ODR_SET: {
            ret = drv_temp_adi_adt7410_set_od_res(&adt7410_ctx, arg);
            if (unlikely(ret)) {
                return -1;
            }
        } break;
        case SENSOR_IOCTL_SET_POWER: {
            ret = drv_temp_adi_adt7410_set_power_mode(&adt7410_ctx, arg);
            if (unlikely(ret)) {
                return -1;
            }
        } break;
        case SENSOR_IOCTL_GET_INFO: {
            /* fill the dev info here */
            dev_sensor_info_t *info = (dev_sensor_info_t *)arg;
            info->model             = "adt7410";
            info->range_max         = 105;
            info->range_min         = -40;
            info->unit              = dCelsius;

        } break;

        default:
            break;
    }

    LOG("%s %s successfully \n", SENSOR_STR, __func__);
    return 0;
}

int drv_temp_adi_adt7410_init(void)
{
    int          ret = 0;
    sensor_obj_t sensor;
    memset(&sensor, 0, sizeof(sensor));

    /* fill the sensor obj parameters here */
    sensor.tag        = TAG_DEV_TEMP;
    sensor.path       = dev_temp_path;
    sensor.io_port    = I2C_PORT;
    sensor.open       = drv_temp_adi_adt7410_open;
    sensor.close      = drv_temp_adi_adt7410_close;
    sensor.read       = drv_temp_adi_adt7410_read;
    sensor.write      = drv_temp_adi_adt7410_write;
    sensor.ioctl      = drv_temp_adi_adt7410_ioctl;
    sensor.irq_handle = drv_temp_adi_adt7410_irq_handle;


    ret = sensor_create_obj(&sensor);
    if (unlikely(ret)) {
        return -1;
    }

    ret = drv_temp_adi_adt7410_validate_id(&adt7410_ctx, ADT7410_WHO_AM_I_VAL);
    if (unlikely(ret)) {
        return -1;
    }

    /* set the default config for the sensor here */
    ret = drv_temp_adi_adt7410_set_default_config(&adt7410_ctx);
    if (unlikely(ret)) {
        return -1;
    }

    LOG("%s %s successfully \n", SENSOR_STR, __func__);

    return 0;
}

SENSOR_DRV_ADD(drv_temp_adi_adt7410_init);

