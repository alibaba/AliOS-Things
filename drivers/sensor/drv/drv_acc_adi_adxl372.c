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


#define ADXL372_I2C_ADDR1 (0x1D)
#define ADXL372_I2C_ADDR2 (0x53)
#define ADXL372_I2C_ADDR_TRANS(n) ((n) << 1)
#define ADXL372_I2C_ADDR ADXL372_I2C_ADDR_TRANS(ADXL372_I2C_ADDR1)

/* ADXL372 registers definition */
#define ADXL372_DEVID 0x00
#define ADXL372_DEVID_MST 0x01
#define ADXL372_PARTID 0x02
#define ADXL372_REVID 0x03
#define ADXL372_STATUS_1 0x04
#define ADXL372_STATUS_2 0x05
#define ADXL372_FIFO_ENTRIES_2 0x06
#define ADXL372_FIFO_ENTRIES_1 0x07
#define ADXL372_X_DATA_H 0x08
#define ADXL372_X_DATA_L 0x09
#define ADXL372_Y_DATA_H 0x0A
#define ADXL372_Y_DATA_L 0x0B
#define ADXL372_Z_DATA_H 0x0C
#define ADXL372_Z_DATA_L 0x0D
#define ADXL372_X_MAXPEAK_H 0x15
#define ADXL372_X_MAXPEAK_L 0x16
#define ADXL372_Y_MAXPEAK_H 0x17
#define ADXL372_Y_MAXPEAK_L 0x18
#define ADXL372_Z_MAXPEAK_H 0x19
#define ADXL372_Z_MAXPEAK_L 0x1A
#define ADXL372_OFFSET_X 0x20
#define ADXL372_OFFSET_Y 0x21
#define ADXL372_OFFSET_Z 0x22
#define ADXL372_X_THRESH_ACT_H 0x23
#define ADXL372_X_THRESH_ACT_L 0x24
#define ADXL372_Y_THRESH_ACT_H 0x25
#define ADXL372_Y_THRESH_ACT_L 0x26
#define ADXL372_Z_THRESH_ACT_H 0x27
#define ADXL372_Z_THRESH_ACT_L 0x28
#define ADXL372_TIME_ACT 0x29
#define ADXL372_X_THRESH_INACT_H 0x2A
#define ADXL372_X_THRESH_INACT_L 0x2B
#define ADXL372_Y_THRESH_INACT_H 0x2C
#define ADXL372_Y_THRESH_INACT_L 0x2D
#define ADXL372_Z_THRESH_INACT_H 0x2E
#define ADXL372_Z_THRESH_INACT_L 0x2F
#define ADXL372_TIME_INACT_H 0x30
#define ADXL372_TIME_INACT_L 0x31
#define ADXL372_X_THRESH_ACT2_H 0x32
#define ADXL372_X_THRESH_ACT2_L 0x33
#define ADXL372_Y_THRESH_ACT2_H 0x34
#define ADXL372_Y_THRESH_ACT2_L 0x35
#define ADXL372_Z_THRESH_ACT2_H 0x36
#define ADXL372_Z_THRESH_ACT2_L 0x37
#define ADXL372_HPF 0x38
#define ADXL372_FIFO_SAMPLES 0x39
#define ADXL372_FIFO_CTL 0x3A
#define ADXL372_INT1_MAP 0x3B
#define ADXL372_INT2_MAP 0x3C
#define ADXL372_TIMING 0x3D
#define ADXL372_MEASURE 0x3E
#define ADXL372_POWER_CTL 0x3F
#define ADXL372_SELF_TEST 0x40
#define ADXL372_RESET 0x41
#define ADXL372_FIFO_DATA 0x42

#define ADXL372_DEVID_VAL 0xAD
#define ADXL372_PARTID_VAL 0xFA
#define ADXL372_RESET_CODE 0x52

#define ADXL372_ODR_6400HZ (0x04 << 5)
#define ADXL372_ODR_3200HZ (0x03 << 5)
#define ADXL372_ODR_1600HZ (0x02 << 5)
#define ADXL372_ODR_800HZ (0x01 << 5)
#define ADXL372_ODR_400HZ (0x00 << 5)
#define ADXL372_ODR_BIT_MASK 0x1F
#define ADXL372_DEFAULT_ODR 400

#define ADXL372_BW_3200HZ 0x04
#define ADXL372_BW_1600HZ 0x03
#define ADXL372_BW_800HZ 0x02
#define ADXL372_BW_400HZ 0x01
#define ADXL372_BW_200HZ 0x00
#define ADXL372_BW_BIT_MASK 0xF8
#define ADXL372_DEFAULT_BW 200

i2c_dev_t adxl372_ctx = {
    .port                 = 3,
    .config.address_width = 8,
    .config.freq          = 400000,
    .config.dev_addr      = ADXL372_I2C_ADDR,
};

static int drv_acc_adi_adxl372_soft_reset(i2c_dev_t *drv)
{
    int     ret   = 0;
    uint8_t value = ADXL372_RESET_CODE;
    ret           = sensor_i2c_write(drv, ADXL372_RESET, &value, I2C_DATA_LEN,
                           I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return -1;
    }
    return 0;
}

static int drv_acc_adi_adxl372_validate_id(i2c_dev_t *drv, uint8_t id_value)
{
    uint8_t value = 0x00;
    int     ret   = 0;

    if (drv == NULL) {
        return -1;
    }

    ret = sensor_i2c_read(drv, ADXL372_PARTID, &value, I2C_DATA_LEN,
                          I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return -1;
    }

    if (id_value != value) {
        return -1;
    }

    return 0;
}

static int drv_acc_adi_adxl372_set_power_mode(i2c_dev_t *      drv,
                                              dev_power_mode_e mode)
{
    uint8_t value;
    int     ret = 0;

    ret = sensor_i2c_read(drv, ADXL372_POWER_CTL, &value, I2C_DATA_LEN,
                          I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    switch (mode) {
        case DEV_POWER_ON: {
            value &= 0xFC;
            value |= 0x03;
            ret = sensor_i2c_write(drv, ADXL372_POWER_CTL, &value, I2C_DATA_LEN,
                                   I2C_OP_RETRIES);
            if (unlikely(ret)) {
                return ret;
            }
        } break;

        case DEV_POWER_OFF: {
            value &= 0xFC;
            ret = sensor_i2c_write(drv, ADXL372_POWER_CTL, &value, I2C_DATA_LEN,
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

static int drv_acc_adi_adxl372_set_odr(i2c_dev_t *drv, uint32_t odr)
{
    int     ret   = 0;
    uint8_t value = 0x00;

    ret = sensor_i2c_read(drv, ADXL372_TIMING, &value, I2C_DATA_LEN,
                          I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    if (odr >= 6400) {
        value &= ADXL372_ODR_BIT_MASK;
        value |= ADXL372_ODR_6400HZ;
    } else if (odr >= 3200) {
        value &= ADXL372_ODR_BIT_MASK;
        value |= ADXL372_ODR_3200HZ;
    } else if (odr >= 1600) {
        value &= ADXL372_ODR_BIT_MASK;
        value |= ADXL372_ODR_1600HZ;
    } else if (odr >= 800) {
        value &= ADXL372_ODR_BIT_MASK;
        value |= ADXL372_ODR_800HZ;
    } else {
        value &= ADXL372_ODR_BIT_MASK;
        value |= ADXL372_ODR_400HZ;
    }

    ret = sensor_i2c_write(drv, ADXL372_TIMING, &value, I2C_DATA_LEN,
                           I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }
    return 0;
}

static int drv_acc_adi_adxl372_set_bw(i2c_dev_t *drv, uint32_t bw)
{
    int     ret   = 0;
    uint8_t value = 0x00;

    ret = sensor_i2c_read(drv, ADXL372_MEASURE, &value, I2C_DATA_LEN,
                          I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    if (bw >= 3200) {
        value &= ADXL372_BW_BIT_MASK;
        value |= ADXL372_BW_3200HZ;
    } else if (bw >= 1600) {
        value &= ADXL372_BW_BIT_MASK;
        value |= ADXL372_BW_1600HZ;
    } else if (bw >= 800) {
        value &= ADXL372_BW_BIT_MASK;
        value |= ADXL372_BW_800HZ;
    } else if (bw >= 400) {
        value &= ADXL372_BW_BIT_MASK;
        value |= ADXL372_BW_400HZ;
    } else {
        value &= ADXL372_BW_BIT_MASK;
        value |= ADXL372_BW_200HZ;
    }
    ret = sensor_i2c_write(drv, ADXL372_MEASURE, &value, I2C_DATA_LEN,
                           I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    return 0;
}


static void drv_acc_adi_adxl372_irq_handle(void)
{
    /* no handle so far */
}

static int drv_acc_adi_adxl372_open(void)
{
    int ret = 0;
    ret     = drv_acc_adi_adxl372_set_power_mode(&adxl372_ctx, DEV_POWER_ON);
    if (unlikely(ret)) {
        return -1;
    }
    return 0;
}

static int drv_acc_adi_adxl372_close(void)
{
    int ret = 0;
    ret     = drv_acc_adi_adxl372_set_power_mode(&adxl372_ctx, DEV_POWER_OFF);
    if (unlikely(ret)) {
        return -1;
    }
    return 0;
}

static int drv_acc_adi_adxl372_read(void *buf, size_t len)
{
    int     ret = 0;
    size_t  size;
    uint8_t LocalBuf[6];
    int32_t x_val = 0, y_val = 0, z_val = 0;
    int8_t  SignBit;

    accel_data_t *accel = (accel_data_t *)buf;
    if (buf == NULL) {
        return -1;
    }

    size = sizeof(accel_data_t);
    if (len < size) {
        return -1;
    }

    ret = sensor_i2c_read(&adxl372_ctx, ADXL372_X_DATA_H, &LocalBuf[0],
                          I2C_REG_LEN, I2C_OP_RETRIES);
    ret |= sensor_i2c_read(&adxl372_ctx, ADXL372_X_DATA_L, &LocalBuf[1],
                           I2C_REG_LEN, I2C_OP_RETRIES);
    ret |= sensor_i2c_read(&adxl372_ctx, ADXL372_Y_DATA_H, &LocalBuf[2],
                           I2C_REG_LEN, I2C_OP_RETRIES);
    ret |= sensor_i2c_read(&adxl372_ctx, ADXL372_Y_DATA_L, &LocalBuf[3],
                           I2C_REG_LEN, I2C_OP_RETRIES);
    ret |= sensor_i2c_read(&adxl372_ctx, ADXL372_Z_DATA_H, &LocalBuf[4],
                           I2C_REG_LEN, I2C_OP_RETRIES);
    ret |= sensor_i2c_read(&adxl372_ctx, ADXL372_Z_DATA_L, &LocalBuf[5],
                           I2C_REG_LEN, I2C_OP_RETRIES);

    if (unlikely(ret)) {
        return -1;
    }

    x_val = (int32_t)(((LocalBuf[0] << 8) | ((LocalBuf[1] << 0) & 0xF0)) >> 4);
    SignBit = (x_val & (1 << 11)) != 0;
    if (SignBit)
        x_val = x_val | ~((1 << 12) - 1);

    y_val = (int32_t)(((LocalBuf[2] << 8) | ((LocalBuf[3] << 0) & 0xF0)) >> 4);
    SignBit = (y_val & (1 << 11)) != 0;
    if (SignBit)
        y_val = y_val | ~((1 << 12) - 1);

    z_val = (int32_t)(((LocalBuf[4] << 8) | ((LocalBuf[5] << 0) & 0xF0)) >> 4);
    SignBit = (z_val & (1 << 11)) != 0;
    if (SignBit)
        z_val = z_val | ~((1 << 12) - 1);

    accel->data[DATA_AXIS_X] = (int32_t)(x_val * 100);
    accel->data[DATA_AXIS_Y] = (int32_t)(y_val * 100);
    accel->data[DATA_AXIS_Z] = (int32_t)(z_val * 100);
    accel->timestamp         = aos_now_ms();

    return (int)size;
}

static int drv_acc_adi_adxl372_ioctl(int cmd, unsigned long arg)
{
    int                ret  = 0;
    dev_sensor_info_t *info = (dev_sensor_info_t *)arg;
    switch (cmd) {
        case SENSOR_IOCTL_ODR_SET: {
            ret = drv_acc_adi_adxl372_set_odr(&adxl372_ctx, arg);
            if (unlikely(ret)) {
                return -1;
            }
        } break;

        case SENSOR_IOCTL_SET_POWER: {
            ret = drv_acc_adi_adxl372_set_power_mode(&adxl372_ctx, arg);
            if (unlikely(ret)) {
                return -1;
            }
        } break;
        case SENSOR_IOCTL_GET_INFO: {
            /* fill the dev info here */
            info->model     = "ADXL372";
            info->range_max = 200;
            info->range_min = 0;
            info->unit      = mg;
        } break;

        default:
            break;
    }

    return 0;
}

int drv_acc_adi_adxl372_init(void)
{
    int          ret = 0;
    sensor_obj_t sensor;
	
    memset(&sensor, 0, sizeof(sensor));
    /* fill the sensor obj parameters here */
    sensor.io_port    = I2C_PORT;
    sensor.tag        = TAG_DEV_ACC;
    sensor.path       = dev_acc_path;
    sensor.open       = drv_acc_adi_adxl372_open;
    sensor.close      = drv_acc_adi_adxl372_close;
    sensor.read       = drv_acc_adi_adxl372_read;
    sensor.write      = NULL;
    sensor.ioctl      = drv_acc_adi_adxl372_ioctl;
    sensor.irq_handle = drv_acc_adi_adxl372_irq_handle;

    ret = sensor_create_obj(&sensor);
    if (unlikely(ret)) {
        return -1;
    }

    ret = drv_acc_adi_adxl372_validate_id(&adxl372_ctx, ADXL372_PARTID_VAL);
    if (unlikely(ret)) {
        return -1;
    }

    ret = drv_acc_adi_adxl372_soft_reset(&adxl372_ctx);
    if (unlikely(ret)) {
        return -1;
    }

    // set odr is 400hz, and will update
    ret = drv_acc_adi_adxl372_set_odr(&adxl372_ctx, ADXL372_DEFAULT_ODR);
    if (unlikely(ret)) {
        return -1;
    }

    ret = drv_acc_adi_adxl372_set_bw(&adxl372_ctx, ADXL372_DEFAULT_BW);
    if (unlikely(ret)) {
        return -1;
    }

    ret = drv_acc_adi_adxl372_set_power_mode(&adxl372_ctx, DEV_POWER_ON);
    if (unlikely(ret)) {
        return -1;
    }
    /* update the phy sensor info to sensor hal */
    LOG("%s %s successfully \n", SENSOR_STR, __func__);
    return 0;
}

SENSOR_DRV_ADD(drv_acc_adi_adxl372_init);

