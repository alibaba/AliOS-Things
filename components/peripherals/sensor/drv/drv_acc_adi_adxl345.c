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


#define ADXL345_I2C_ADDR1 (0x1D)
#define ADXL345_I2C_ADDR2 (0x53)
#define ADXL345_I2C_ADDR_TRANS(n) ((n) << 1)
#define ADXL345_I2C_ADDR ADXL345_I2C_ADDR_TRANS(ADXL345_I2C_ADDR2)

#define ADXL345_DEVID 0x00
#define ADXL345_RESERVED1 0x01
#define ADXL345_THRESH_TAP 0x1d
#define ADXL345_OFSX 0x1e
#define ADXL345_OFSY 0x1f
#define ADXL345_OFSZ 0x20
#define ADXL345_DUR 0x21
#define ADXL345_LATENT 0x22
#define ADXL345_WINDOW 0x23
#define ADXL345_THRESH_ACT 0x24
#define ADXL345_THRESH_INACT 0x25
#define ADXL345_TIME_INACT 0x26
#define ADXL345_ACT_INACT_CTL 0x27
#define ADXL345_THRESH_FF 0x28
#define ADXL345_TIME_FF 0x29
#define ADXL345_TAP_AXES 0x2a
#define ADXL345_ACT_TAP_STATUS 0x2b
#define ADXL345_BW_RATE 0x2c
#define ADXL345_POWER_CTL 0x2d
#define ADXL345_INT_ENABLE 0x2e
#define ADXL345_INT_MAP 0x2f
#define ADXL345_INT_SOURCE 0x30
#define ADXL345_DATA_FORMAT 0x31
#define ADXL345_DATAX0 0x32
#define ADXL345_DATAX1 0x33
#define ADXL345_DATAY0 0x34
#define ADXL345_DATAY1 0x35
#define ADXL345_DATAZ0 0x36
#define ADXL345_DATAZ1 0x37
#define ADXL345_FIFO_CTL 0x38
#define ADXL345_FIFO_STATUS 0x39

#define ADXL345_DEVICE_ID_VALUE 0xE5
#define ADXL345_POWER_CTL_MSK 0xC0
#define ADXL345_MEASURE_MODE (0x01 << 3)
#define ADXL345_STAND_BY_MODE (0x00 << 3)
#define ADXL345_SLEEP_MODE (0X01 << 2)

#define ADXL345_ODR_3200HZ 0x0F // 3200Hz
#define ADXL345_ODR_1600HZ 0x0E // 1600Hz
#define ADXL345_ODR_800HZ 0x0D  // 800Hz
#define ADXL345_ODR_400HZ 0x0C  // 400Hz
#define ADXL345_ODR_200HZ 0x0B  // 200Hz
#define ADXL345_ODR_100HZ 0x0A  // 100Hz
#define ADXL345_ODR_50HZ 0x09   // 50Hz
#define ADXL345_ODR_25HZ 0x08   // 25Hz
#define ADXL345_ODR_12_5HZ 0x07 // 12.5Hz
#define ADXL345_ODR_6_25HZ 0x06 // 6.25Hz
#define ADXL345_ODR_3_13HZ 0x05 // 3.13Hz
#define ADXL345_ODR_1_56HZ 0x04 // 1.56Hz
#define ADXL345_ODR_0_78HZ 0x03 // 0.78Hz
#define ADXL345_ODR_0_39HZ 0x02 // 0.39Hz
#define ADXL345_ODR_0_2HZ 0x01  // 0.2Hz
#define ADXL345_ODR_0_1HZ 0x00  // 0.1Hz
#define ADXL345_ODR_BIT_MASK 0xF0
#define ADXL345_DEFAULT_ODR_100HZ 100

#define ADXL345_RANGE_2G 0x00
#define ADXL345_RANGE_4G 0x01
#define ADXL345_RANGE_8G 0x02
#define ADXL345_RANGE_16G 0x03
#define ADXL345_RANGE_MSK 0xFC
#define ADXL345_DEFAULT_RANG_2G ACC_RANGE_2G
// adxl345 sensitivity factor table, the unit is LSB/g
int32_t  adxl345_factor  = 1;
uint32_t g_adxl345_range = ACC_RANGE_2G;

i2c_dev_t adxl345_ctx = {
    .port                 = 3,
    .config.address_width = 8,
    .config.freq          = 400000,
    .config.dev_addr      = ADXL345_I2C_ADDR,
};

static int drv_acc_adi_adxl345_validate_id(i2c_dev_t *drv, uint8_t id_value)
{
    uint8_t value = 0x00;
    int     ret   = 0;

    if (drv == NULL) {
        return -1;
    }

    ret =
      sensor_i2c_read(drv, ADXL345_DEVID, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return -1;
    }

    if (id_value != value) {
        return -1;
    }

    return 0;
}

static int drv_acc_adi_adxl345_set_power_mode(i2c_dev_t *      drv,
                                              dev_power_mode_e mode)
{
    uint8_t value;
    int     ret = 0;

    ret = sensor_i2c_read(drv, ADXL345_POWER_CTL, &value, I2C_DATA_LEN,
                          I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    switch (mode) {
        case DEV_POWER_ON: {
            value &= ADXL345_POWER_CTL_MSK;
            value |= ADXL345_MEASURE_MODE;
            ret = sensor_i2c_write(drv, ADXL345_POWER_CTL, &value, I2C_DATA_LEN,
                                   I2C_OP_RETRIES);
            if (unlikely(ret)) {
                return ret;
            }
        } break;

        case DEV_POWER_OFF: {
            value &= ADXL345_POWER_CTL_MSK;
            value |= ADXL345_STAND_BY_MODE;
            ret = sensor_i2c_write(drv, ADXL345_POWER_CTL, &value, I2C_DATA_LEN,
                                   I2C_OP_RETRIES);
            if (unlikely(ret)) {
                return ret;
            }
        } break;

        case DEV_SLEEP: {
            value &= ADXL345_POWER_CTL_MSK;
            value |= ADXL345_SLEEP_MODE;
            ret = sensor_i2c_write(drv, ADXL345_POWER_CTL, &value, I2C_DATA_LEN,
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

static int drv_acc_adi_adxl345_set_odr(i2c_dev_t *drv, float odr)
{
    int     ret   = 0;
    uint8_t value = 0x00;

    ret = sensor_i2c_read(drv, ADXL345_BW_RATE, &value, I2C_DATA_LEN,
                          I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    if (odr >= 3200) {
        value &= ADXL345_ODR_BIT_MASK;
        value |= ADXL345_ODR_3200HZ;
    } else if (odr >= 1600) {
        value &= ADXL345_ODR_BIT_MASK;
        value |= ADXL345_ODR_1600HZ;
    } else if (odr >= 800) {
        value &= ADXL345_ODR_BIT_MASK;
        value |= ADXL345_ODR_800HZ;
    } else if (odr >= 400) {
        value &= ADXL345_ODR_BIT_MASK;
        value |= ADXL345_ODR_400HZ;
    } else if (odr >= 200) {
        value &= ADXL345_ODR_BIT_MASK;
        value |= ADXL345_ODR_200HZ;
    } else if (odr >= 100) {
        value &= ADXL345_ODR_BIT_MASK;
        value |= ADXL345_ODR_100HZ;
    } else if (odr >= 50) {
        value &= ADXL345_ODR_BIT_MASK;
        value |= ADXL345_ODR_50HZ;
    } else if (odr >= 25) {
        value &= ADXL345_ODR_BIT_MASK;
        value |= ADXL345_ODR_25HZ;
    } else if (odr >= 12.5) {
        value &= ADXL345_ODR_BIT_MASK;
        value |= ADXL345_ODR_12_5HZ;
    } else if (odr >= 6.25) {
        value &= ADXL345_ODR_BIT_MASK;
        value |= ADXL345_ODR_6_25HZ;
    } else if (odr >= 3.13) {
        value &= ADXL345_ODR_BIT_MASK;
        value |= ADXL345_ODR_3_13HZ;
    } else if (odr >= 1.56) {
        value &= ADXL345_ODR_BIT_MASK;
        value |= ADXL345_ODR_1_56HZ;
    } else if (odr >= 0.78) {
        value &= ADXL345_ODR_BIT_MASK;
        value |= ADXL345_ODR_0_78HZ;
    } else if (odr >= 0.39) {
        value &= ADXL345_ODR_BIT_MASK;
        value |= ADXL345_ODR_0_39HZ;
    } else if (odr >= 0.20) {
        value &= ADXL345_ODR_BIT_MASK;
        value |= ADXL345_ODR_0_2HZ;
    } else {
        value &= ADXL345_ODR_BIT_MASK;
        value |= ADXL345_ODR_0_1HZ;
    }

    ret = sensor_i2c_write(drv, ADXL345_BW_RATE, &value, I2C_DATA_LEN,
                           I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }
    return 0;
}

static int drv_acc_adi_adxl345_set_range(i2c_dev_t *drv, uint32_t range)
{
    int     ret   = 0;
    uint8_t value = 0x00;

    ret = sensor_i2c_read(drv, ADXL345_DATA_FORMAT, &value, I2C_DATA_LEN,
                          I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    switch (range) {
        case ACC_RANGE_2G: {
            value &= ADXL345_RANGE_MSK;
            value |= ADXL345_RANGE_2G;
        } break;

        case ACC_RANGE_4G: {
            value &= ADXL345_RANGE_MSK;
            value |= ADXL345_RANGE_4G;
            adxl345_factor = 2;
        } break;

        case ACC_RANGE_8G: {
            value &= ADXL345_RANGE_MSK;
            value |= ADXL345_RANGE_8G;
            adxl345_factor = 4;
        } break;

        case ACC_RANGE_16G: {
            value &= ADXL345_RANGE_MSK;
            value |= ADXL345_RANGE_16G;
            adxl345_factor = 8;
        } break;

        default:
            break;
    }

    /* Write the range register 0x31*/
    ret = sensor_i2c_write(drv, ADXL345_DATA_FORMAT, &value, I2C_DATA_LEN,
                           I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    g_adxl345_range = range;
    return 0;
}


static void drv_acc_adi_adxl345_irq_handle(void)
{
    /* no handle so far */
}

static int drv_acc_adi_adxl345_open(void)
{
    int ret = 0;
    ret     = drv_acc_adi_adxl345_set_power_mode(&adxl345_ctx, DEV_POWER_ON);
    if (unlikely(ret)) {
        return -1;
    }
    return 0;
}

static int drv_acc_adi_adxl345_close(void)
{
    int ret = 0;
    ret     = drv_acc_adi_adxl345_set_power_mode(&adxl345_ctx, DEV_POWER_OFF);
    if (unlikely(ret)) {
        return -1;
    }
    return 0;
}

static int drv_acc_adi_adxl345_read(void *buf, size_t len)
{
    int     ret = 0;
    size_t  size;
    uint8_t LocalBuf[6];
    int32_t x_val = 0, y_val = 0, z_val = 0;

    accel_data_t *accel = (accel_data_t *)buf;
    if (buf == NULL) {
        return -1;
    }

    size = sizeof(accel_data_t);
    if (len < size) {
        return -1;
    }

    ret = sensor_i2c_read(&adxl345_ctx, ADXL345_DATAX0, &LocalBuf[0],
                          I2C_REG_LEN, I2C_OP_RETRIES);
    ret |= sensor_i2c_read(&adxl345_ctx, ADXL345_DATAX1, &LocalBuf[1],
                           I2C_REG_LEN, I2C_OP_RETRIES);
    ret |= sensor_i2c_read(&adxl345_ctx, ADXL345_DATAY0, &LocalBuf[2],
                           I2C_REG_LEN, I2C_OP_RETRIES);
    ret |= sensor_i2c_read(&adxl345_ctx, ADXL345_DATAY1, &LocalBuf[3],
                           I2C_REG_LEN, I2C_OP_RETRIES);
    ret |= sensor_i2c_read(&adxl345_ctx, ADXL345_DATAZ0, &LocalBuf[4],
                           I2C_REG_LEN, I2C_OP_RETRIES);
    ret |= sensor_i2c_read(&adxl345_ctx, ADXL345_DATAZ1, &LocalBuf[5],
                           I2C_REG_LEN, I2C_OP_RETRIES);

    if (unlikely(ret)) {
        return -1;
    }

    x_val = (int16_t)((LocalBuf[1] << 8) | LocalBuf[0]);

    y_val = (int16_t)((LocalBuf[3] << 8) | LocalBuf[2]);

    z_val = (int16_t)((LocalBuf[5] << 8) | LocalBuf[4]);

    accel->data[DATA_AXIS_X] = (int32_t)(x_val * 39 * adxl345_factor / 10);
    accel->data[DATA_AXIS_Y] = (int32_t)(y_val * 39 * adxl345_factor / 10);
    accel->data[DATA_AXIS_Z] = (int32_t)(z_val * 39 * adxl345_factor / 10);
    accel->timestamp         = aos_now_ms();

    return (int)size;
}

static int drv_acc_adi_adxl345_ioctl(int cmd, unsigned long arg)
{
    int                ret  = 0;
    dev_sensor_info_t *info = (dev_sensor_info_t *)arg;
    switch (cmd) {
        case SENSOR_IOCTL_ODR_SET: {
            ret = drv_acc_adi_adxl345_set_odr(&adxl345_ctx, arg);
            if (unlikely(ret)) {
                return -1;
            }
        } break;
        case SENSOR_IOCTL_RANGE_SET: {
            ret = drv_acc_adi_adxl345_set_range(&adxl345_ctx, arg);
            if (unlikely(ret)) {
                return -1;
            }
        } break;
        case SENSOR_IOCTL_SET_POWER: {
            ret = drv_acc_adi_adxl345_set_power_mode(&adxl345_ctx, arg);
            if (unlikely(ret)) {
                return -1;
            }
        } break;
        case SENSOR_IOCTL_GET_INFO: {
            /* fill the dev info here */
            info->model     = "ADXL345";
            info->range_max = 16;
            info->range_min = 2;
            info->unit      = mg;
        } break;

        default:
            break;
    }

    return 0;
}

int drv_acc_adi_adxl345_init(void)
{
    int          ret = 0;
    sensor_obj_t sensor;
    memset(&sensor, 0, sizeof(sensor));

    /* fill the sensor obj parameters here */
    sensor.io_port    = I2C_PORT;
    sensor.tag        = TAG_DEV_ACC;
    sensor.path       = dev_acc_path;
    sensor.open       = drv_acc_adi_adxl345_open;
    sensor.close      = drv_acc_adi_adxl345_close;
    sensor.read       = drv_acc_adi_adxl345_read;
    sensor.write      = NULL;
    sensor.ioctl      = drv_acc_adi_adxl345_ioctl;
    sensor.irq_handle = drv_acc_adi_adxl345_irq_handle;

    ret = sensor_create_obj(&sensor);
    if (unlikely(ret)) {
        return -1;
    }

    ret =
      drv_acc_adi_adxl345_validate_id(&adxl345_ctx, ADXL345_DEVICE_ID_VALUE);
    if (unlikely(ret)) {
        return -1;
    }

    ret = drv_acc_adi_adxl345_set_range(&adxl345_ctx, ACC_RANGE_4G);
    if (unlikely(ret)) {
        return -1;
    }

    // set odr is 125hz, and will update
    ret = drv_acc_adi_adxl345_set_odr(&adxl345_ctx, ADXL345_DEFAULT_ODR_100HZ);
    if (unlikely(ret)) {
        return -1;
    }
    ret = drv_acc_adi_adxl345_set_power_mode(&adxl345_ctx, DEV_POWER_ON);
    if (unlikely(ret)) {
        return -1;
    }
    /* update the phy sensor info to sensor hal */
    LOG("%s %s successfully \n", SENSOR_STR, __func__);
    return 0;
}

SENSOR_DRV_ADD(drv_acc_adi_adxl345_init);

