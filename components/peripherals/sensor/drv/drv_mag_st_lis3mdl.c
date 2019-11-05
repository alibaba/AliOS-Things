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


#define LIS3MDL_I2C_ADDR1 0x1C
#define LIS3MDL_I2C_ADDR2 0x1E
#define LIS3MDL_I2C_ADDR_TRANS(n) ((n) << 1)
#define LIS3MDL_I2C_ADDR LIS3MDL_I2C_ADDR_TRANS(LIS3MDL_I2C_ADDR2)
#define LIS3MDL_BIT(x) (x)

#define I_AM_LIS3MDL 0x3D
#define LIS3MDL_MAG_WHO_AM_I_REG 0x0F
#define LIS3MDL_MAG_CTRL_REG1 0x20
#define LIS3MDL_MAG_CTRL_REG2 0x21
#define LIS3MDL_MAG_CTRL_REG3 0x22
#define LIS3MDL_MAG_CTRL_REG4 0x23
#define LIS3MDL_MAG_CTRL_REG5 0x24
#define LIS3MDL_MAG_STATUS_REG 0x27
#define LIS3MDL_MAG_OUTX_L 0x28
#define LIS3MDL_MAG_OUTX_H 0x29
#define LIS3MDL_MAG_OUTY_L 0x2A
#define LIS3MDL_MAG_OUTY_H 0x2B
#define LIS3MDL_MAG_OUTZ_L 0x2C
#define LIS3MDL_MAG_OUTZ_H 0x2D
#define LIS3MDL_MAG_TEMP_OUT_L 0x2E
#define LIS3MDL_MAG_TEMP_OUT_H 0x2F
#define LIS3MDL_MAG_INT_CFG 0x30
#define LIS3MDL_MAG_INT_SRC 0x31
#define LIS3MDL_MAG_INT_THS_L 0x32
#define LIS3MDL_MAG_INT_THS_H 0x33

#define LIS3MDL_MAG_TEMPSENSOR_ENABLE 0x80
#define LIS3MDL_MAG_TEMPSENSOR_DISABLE 0x00

#define LIS3MDL_MAG_OM_XY_BIT LIS3MDL_BIT(0x60)
#define LIS3MDL_MAG_OM_XY_LOWPOWER 0x00
#define LIS3MDL_MAG_OM_XY_MEDIUM 0x20
#define LIS3MDL_MAG_OM_XY_HIGH 0x40
#define LIS3MDL_MAG_OM_XY_ULTRAHIGH 0x60

#define LIS3MDL_MAG_ODR_BIT LIS3MDL_BIT(0x1C)
#define LIS3MDL_MAG_ODR_0_625_HZ 0x00
#define LIS3MDL_MAG_ODR_1_25_HZ 0x04
#define LIS3MDL_MAG_ODR_2_5_HZ 0x08
#define LIS3MDL_MAG_ODR_5_0_HZ 0x0C
#define LIS3MDL_MAG_ODR_10_HZ 0x10
#define LIS3MDL_MAG_ODR_20_HZ 0x14
#define LIS3MDL_MAG_ODR_40_HZ 0x18
#define LIS3MDL_MAG_ODR_80_HZ 0x1C

#define LMS303C_MAG_SELFTEST_DISABLE 0x00
#define LMS303C_MAG_SELFTEST_ENABLE 0x01

#define LIS3MDL_MAG_FS_BIT LIS3MDL_BIT(0x60)
#define LIS3MDL_MAG_FS_DEFAULT 0x00
#define LIS3MDL_MAG_FS_4_GA 0x00
#define LIS3MDL_MAG_FS_8_GA 0x20
#define LIS3MDL_MAG_FS_12_GA 0x40
#define LIS3MDL_MAG_FS_16_GA 0x60

#define LIS3MDL_MAG_REBOOT_DEFAULT 0x00
#define LIS3MDL_MAG_REBOOT_ENABLE 0x08

#define LIS3MDL_MAG_SOFT_RESET_DEFAULT 0x00
#define LIS3MDL_MAG_SOFT_RESET_ENABLE 0x04

#define LIS3MDL_MAG_SIM_4_WIRE 0x00
#define LIS3MDL_MAG_SIM_3_WIRE 0x04

#define LIS3MDL_MAG_CONFIG_LOWPOWER_BIT LIS3MDL_BIT(0x20)
#define LIS3MDL_MAG_CONFIG_NORMAL_MODE 0x00
#define LIS3MDL_MAG_CONFIG_LOWPOWER_MODE 0x20

#define LIS3MDL_MAG_POWERMODE_BIT LIS3MDL_BIT(0x03)
#define LIS3MDL_MAG_CONTINUOUS_MODE 0x00
#define LIS3MDL_MAG_SINGLE_MODE 0x01
#define LIS3MDL_MAG_POWERDOWN1_MODE 0x02
#define LIS3MDL_MAG_POWERDOWN2_MODE 0x03

#define LIS3MDL_MAG_OM_Z_BIT LIS3MDL_BIT(0x0C)
#define LIS3MDL_MAG_OM_Z_LOWPOWER 0x00
#define LIS3MDL_MAG_OM_Z_MEDIUM 0x04
#define LIS3MDL_MAG_OM_Z_HIGH 0x08
#define LIS3MDL_MAG_OM_Z_ULTRAHIGH 0x0C

#define LIS3MDL_MAG_BLE_BIT LIS3MDL_BIT(0x02)
#define LIS3MDL_MAG_BLE_LSB 0x00
#define LIS3MDL_MAG_BLE_MSB 0x02

#define LIS3MDL_MAG_BDU_BIT LIS3MDL_BIT(0x40)
#define LIS3MDL_MAG_BDU_CONTINUOUS 0x00
#define LIS3MDL_MAG_BDU_MSBLSB 0x40

#define LIS3MDL_MAG_SENSITIVITY_FOR_FS_4GA (140)
#define LIS3MDL_MAG_SENSITIVITY_FOR_FS_8GA (290)
#define LIS3MDL_MAG_SENSITIVITY_FOR_FS_12GA (430)
#define LIS3MDL_MAG_SENSITIVITY_FOR_FS_16GA (580)


i2c_dev_t LIS3MDL_ctx = {
    .config.dev_addr = LIS3MDL_I2C_ADDR,
};

static int drv_mag_st_lis3mdl_soft_reset(i2c_dev_t *drv)
{
    int     ret   = 0;
    uint8_t value = LIS3MDL_MAG_SOFT_RESET_ENABLE;
    ret = sensor_i2c_write(drv, LIS3MDL_MAG_CTRL_REG2, &value, I2C_DATA_LEN,
                           I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return -1;
    }
    return 0;
}

UNUSED static int drv_mag_st_lis3mdl_selftest(i2c_dev_t *drv)
{
    int     ret   = 0;
    uint8_t value = LMS303C_MAG_SELFTEST_ENABLE;
    ret = sensor_i2c_write(drv, LIS3MDL_MAG_CTRL_REG1, &value, I2C_DATA_LEN,
                           I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return -1;
    }
    return 0;
}

UNUSED static int drv_mag_st_lis3mdl_reboot(i2c_dev_t *drv)
{
    int     ret   = 0;
    uint8_t value = LIS3MDL_MAG_REBOOT_ENABLE;
    ret = sensor_i2c_write(drv, LIS3MDL_MAG_CTRL_REG2, &value, I2C_DATA_LEN,
                           I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return -1;
    }
    return 0;
}

static int drv_mag_st_lis3mdl_validate_id(i2c_dev_t *drv, uint8_t id_value)
{
    uint8_t value = 0x00;
    int     ret   = 0;

    if (drv == NULL) {
        return -1;
    }

    ret = sensor_i2c_read(drv, LIS3MDL_MAG_WHO_AM_I_REG, &value, I2C_DATA_LEN,
                          I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    if (id_value != value) {
        return -1;
    }

    return 0;
}

static int drv_mag_st_lis3mdl_set_power_mode(i2c_dev_t *      drv,
                                             dev_power_mode_e mode)
{
    uint8_t value = 0x00;
    int     ret   = 0;

    ret = sensor_i2c_read(drv, LIS3MDL_MAG_CTRL_REG3, &value, I2C_DATA_LEN,
                          I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    if (mode == DEV_POWER_ON) {
        value &= ~LIS3MDL_MAG_POWERMODE_BIT;
        value |= LIS3MDL_MAG_CONTINUOUS_MODE;
    } else {
        value &= ~LIS3MDL_MAG_POWERMODE_BIT;
        value |= LIS3MDL_MAG_POWERDOWN2_MODE;
    }

    ret = sensor_i2c_write(drv, LIS3MDL_MAG_CTRL_REG3, &value, I2C_DATA_LEN,
                           I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    return 0;
}

static int drv_mag_st_lis3mdl_set_odr(i2c_dev_t *drv, uint8_t odr)
{
    uint8_t value = 0x00;
    int     ret   = 0;

    ret = sensor_i2c_read(drv, LIS3MDL_MAG_CTRL_REG1, &value, I2C_DATA_LEN,
                          I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }
    value &= ~LIS3MDL_MAG_ODR_BIT;
    value |= (uint8_t)odr;
    ret = sensor_i2c_write(drv, LIS3MDL_MAG_CTRL_REG1, &value, I2C_DATA_LEN,
                           I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }
    return 0;
}

static int drv_mag_st_lis3mdl_set_xyaxes(i2c_dev_t *drv, uint8_t data_rate)
{
    uint8_t value = 0x00;
    int     ret   = 0;

    ret = sensor_i2c_read(drv, LIS3MDL_MAG_CTRL_REG1, &value, I2C_DATA_LEN,
                          I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }
    switch (data_rate) {
        case LIS3MDL_MAG_OM_XY_LOWPOWER: {
            value &= ~LIS3MDL_MAG_OM_XY_BIT;
            value |= LIS3MDL_MAG_OM_XY_LOWPOWER;
        } break;

        case LIS3MDL_MAG_OM_XY_MEDIUM: {
            value &= ~LIS3MDL_MAG_OM_XY_BIT;
            value |= LIS3MDL_MAG_OM_XY_MEDIUM;
        } break;

        case LIS3MDL_MAG_OM_XY_HIGH: {
            value &= ~LIS3MDL_MAG_OM_XY_BIT;
            value |= LIS3MDL_MAG_OM_XY_HIGH;
        } break;

        case LIS3MDL_MAG_OM_XY_ULTRAHIGH: {
            value &= ~LIS3MDL_MAG_OM_XY_BIT;
            value |= LIS3MDL_MAG_OM_XY_ULTRAHIGH;
        } break;

        default:
            break;
    }

    ret = sensor_i2c_write(drv, LIS3MDL_MAG_CTRL_REG1, &value, I2C_DATA_LEN,
                           I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }
    return 0;
}

static int drv_mag_st_lis3mdl_set_zaxes(i2c_dev_t *drv, uint8_t data_rate)
{
    uint8_t value = 0x00;
    int     ret   = 0;

    ret = sensor_i2c_read(drv, LIS3MDL_MAG_CTRL_REG4, &value, I2C_DATA_LEN,
                          I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }
    switch (data_rate) {
        case LIS3MDL_MAG_OM_Z_LOWPOWER: {
            value &= ~LIS3MDL_MAG_OM_Z_BIT;
            value |= LIS3MDL_MAG_OM_Z_LOWPOWER;
        } break;

        case LIS3MDL_MAG_OM_Z_MEDIUM: {
            value &= ~LIS3MDL_MAG_OM_Z_BIT;
            value |= LIS3MDL_MAG_OM_Z_MEDIUM;
        } break;

        case LIS3MDL_MAG_OM_Z_HIGH: {
            value &= ~LIS3MDL_MAG_OM_Z_BIT;
            value |= LIS3MDL_MAG_OM_Z_HIGH;
        } break;

        case LIS3MDL_MAG_OM_Z_ULTRAHIGH: {
            value &= ~LIS3MDL_MAG_OM_Z_BIT;
            value |= LIS3MDL_MAG_OM_Z_ULTRAHIGH;
        } break;

        default:
            break;
    }

    ret = sensor_i2c_write(drv, LIS3MDL_MAG_CTRL_REG4, &value, I2C_DATA_LEN,
                           I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }
    return 0;
}

UNUSED static int drv_mag_st_lis3mdl_lowpower_mode(i2c_dev_t *drv,
                                            uint8_t    lowpower_mode)
{
    int     ret   = 0;
    uint8_t value = 0x00;

    ret = sensor_i2c_read(drv, LIS3MDL_MAG_CTRL_REG3, &value, I2C_DATA_LEN,
                          I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    if (lowpower_mode == LIS3MDL_MAG_CONFIG_LOWPOWER_MODE) {
        value &= ~LIS3MDL_MAG_CONFIG_LOWPOWER_BIT;
        value |= LIS3MDL_MAG_CONFIG_LOWPOWER_MODE;
    } else {
        value &= ~LIS3MDL_MAG_CONFIG_LOWPOWER_BIT;
        value |= LIS3MDL_MAG_CONFIG_NORMAL_MODE;
    }

    ret = sensor_i2c_write(drv, LIS3MDL_MAG_CTRL_REG3, &value, I2C_DATA_LEN,
                           I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    return 0;
}

static int drv_mag_st_lis3mdl_set_range(i2c_dev_t *drv, uint32_t range)
{
    int     ret   = 0;
    uint8_t value = 0x00;

    ret = sensor_i2c_read(drv, LIS3MDL_MAG_CTRL_REG2, &value, I2C_DATA_LEN,
                          I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    switch (range) {
        case MAG_RANGE_4GAUSS: {
            value &= ~LIS3MDL_MAG_FS_BIT;
            value |= LIS3MDL_MAG_FS_4_GA;
        } break;

        case MAG_RANGE_8GAUSS: {
            value &= ~LIS3MDL_MAG_FS_BIT;
            value |= LIS3MDL_MAG_FS_8_GA;
        } break;

        case MAG_RANGE_12GAUSS: {
            value &= ~LIS3MDL_MAG_FS_BIT;
            value |= LIS3MDL_MAG_FS_12_GA;
        } break;

        case MAG_RANGE_16GAUSS: {
            value &= ~LIS3MDL_MAG_FS_BIT;
            value |= LIS3MDL_MAG_FS_16_GA;
        } break;

        default:
            break;
    }

    ret = sensor_i2c_write(drv, LIS3MDL_MAG_CTRL_REG2, &value, I2C_DATA_LEN,
                           I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    return 0;
}

static int drv_mag_st_lis3mdl_set_ble(i2c_dev_t *drv, uint8_t ble)
{
    uint8_t value = 0x00;
    int     ret   = 0;

    ret = sensor_i2c_read(drv, LIS3MDL_MAG_CTRL_REG4, &value, I2C_DATA_LEN,
                          I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    if (ble == LIS3MDL_MAG_BLE_LSB) {
        value &= ~LIS3MDL_MAG_BLE_BIT;
        value |= LIS3MDL_MAG_BLE_LSB;
    } else {
        value &= ~LIS3MDL_MAG_BLE_BIT;
        value |= LIS3MDL_MAG_BLE_MSB;
    }

    ret = sensor_i2c_write(drv, LIS3MDL_MAG_CTRL_REG4, &value, I2C_DATA_LEN,
                           I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }
    return 0;
}

static int drv_mag_st_lis3mdl_set_bdu(i2c_dev_t *drv, uint8_t bdu)
{
    uint8_t value = 0x00;
    int     ret   = 0;

    ret = sensor_i2c_read(drv, LIS3MDL_MAG_CTRL_REG5, &value, I2C_DATA_LEN,
                          I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    if (bdu == LIS3MDL_MAG_BDU_CONTINUOUS) {
        value &= ~LIS3MDL_MAG_BDU_BIT;
        value |= LIS3MDL_MAG_BDU_CONTINUOUS;
    } else {
        value &= ~LIS3MDL_MAG_BDU_BIT;
        value |= LIS3MDL_MAG_BDU_MSBLSB;
    }

    ret = sensor_i2c_write(drv, LIS3MDL_MAG_CTRL_REG5, &value, I2C_DATA_LEN,
                           I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }
    return 0;
}

static int drv_mag_st_lis3mdl_set_default_config(i2c_dev_t *drv)
{
    int     ret   = 0;
    ret           = drv_mag_st_lis3mdl_set_power_mode(drv, DEV_POWER_OFF);
    if (unlikely(ret)) {
        return ret;
    }
    ret = drv_mag_st_lis3mdl_set_odr(drv, LIS3MDL_MAG_ODR_2_5_HZ);
    if (unlikely(ret)) {
        return ret;
    }
    ret = drv_mag_st_lis3mdl_set_xyaxes(drv, LIS3MDL_MAG_OM_XY_MEDIUM);
    if (unlikely(ret)) {
        return ret;
    }
    ret = drv_mag_st_lis3mdl_set_zaxes(drv, LIS3MDL_MAG_OM_Z_MEDIUM);
    if (unlikely(ret)) {
        return ret;
    }
    ret = drv_mag_st_lis3mdl_set_range(drv, MAG_RANGE_8GAUSS);
    if (unlikely(ret)) {
        return -1;
    }
    ret = drv_mag_st_lis3mdl_set_ble(drv, LIS3MDL_MAG_BLE_LSB);
    if (unlikely(ret)) {
        return -1;
    }
    ret = drv_mag_st_lis3mdl_set_bdu(drv, LIS3MDL_MAG_BDU_MSBLSB);
    if (unlikely(ret)) {
        return -1;
    }

    return 0;
}

static void drv_mag_st_lis3mdl_irq_handle(void)
{
    /* no handle so far */
}

static int drv_mag_st_lis3mdl_open(void)
{
    int ret = 0;
    ret     = drv_mag_st_lis3mdl_set_power_mode(&LIS3MDL_ctx, DEV_POWER_ON);
    if (unlikely(ret)) {
        return -1;
    }
    return 0;
}

static int drv_mag_st_lis3mdl_close(void)
{
    int ret = 0;
    ret     = drv_mag_st_lis3mdl_set_power_mode(&LIS3MDL_ctx, DEV_POWER_OFF);
    if (unlikely(ret)) {
        return -1;
    }
    return 0;
}

static int drv_mag_st_lis3mdl_read(void *buf, size_t len)
{
    int      ret = 0;
    size_t   size;
    int16_t  pnRawData[3];
    uint8_t  ctrlm = 0;
    uint8_t  buffer[6];
    uint8_t  i           = 0;
    uint16_t sensitivity = 0;

    mag_data_t *pdata = (mag_data_t *)buf;
    if (buf == NULL) {
        return -1;
    }

    size = sizeof(mag_data_t);
    if (len < size) {
        return -1;
    }
    ret |= sensor_i2c_read(&LIS3MDL_ctx, LIS3MDL_MAG_CTRL_REG2, &ctrlm,
                          I2C_DATA_LEN, I2C_OP_RETRIES);

    ret |= sensor_i2c_read(&LIS3MDL_ctx, (LIS3MDL_MAG_OUTX_L | 0x80), buffer, 6,
                          I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return -1;
    }

    for (i = 0; i < 3; i++) {
        pnRawData[i] =
          ((((uint16_t)buffer[2 * i + 1]) << 8) | (uint16_t)buffer[2 * i]);
    }

    switch (ctrlm & LIS3MDL_MAG_FS_BIT) {
        case LIS3MDL_MAG_FS_4_GA:
            sensitivity = LIS3MDL_MAG_SENSITIVITY_FOR_FS_4GA;
            break;
        case LIS3MDL_MAG_FS_8_GA:
            sensitivity = LIS3MDL_MAG_SENSITIVITY_FOR_FS_8GA;
            break;
        case LIS3MDL_MAG_FS_12_GA:
            sensitivity = LIS3MDL_MAG_SENSITIVITY_FOR_FS_12GA;
            break;
        case LIS3MDL_MAG_FS_16_GA:
            sensitivity = LIS3MDL_MAG_SENSITIVITY_FOR_FS_16GA;
            break;
    }

    for (i = 0; i < 3; i++) {
        pdata->data[i] = (int16_t)((pnRawData[i] * sensitivity) >> 10);
    }
    pdata->timestamp = aos_now_ms();

    return (int)size;
}


static int drv_mag_st_lis3mdl_ioctl(int cmd, unsigned long arg)
{
    int ret = 0;

    switch (cmd) {
        case SENSOR_IOCTL_ODR_SET: {
            ret = drv_mag_st_lis3mdl_set_odr(&LIS3MDL_ctx, arg);
            if (unlikely(ret)) {
                return -1;
            }
        } break;
        case SENSOR_IOCTL_RANGE_SET: {
            ret = drv_mag_st_lis3mdl_set_range(&LIS3MDL_ctx, arg);
            if (unlikely(ret)) {
                return -1;
            }
        } break;
        case SENSOR_IOCTL_SET_POWER: {
            ret = drv_mag_st_lis3mdl_set_power_mode(&LIS3MDL_ctx, arg);
            if (unlikely(ret)) {
                return -1;
            }
        } break;
        case SENSOR_IOCTL_GET_INFO: {
            /* fill the dev info here */
            dev_sensor_info_t *info = (dev_sensor_info_t *)arg;
            info->model             = "LIS3MDL";
            info->range_max         = 16;
            info->range_min         = 4;
            info->unit              = mGauss;
        } break;

        default:
            break;
    }

    return 0;
}

int drv_mag_st_lis3mdl_init(void)
{
    int          ret = 0;
    sensor_obj_t sensor;

    memset(&sensor, 0, sizeof(sensor));

    /* fill the sensor obj parameters here */
    sensor.io_port    = I2C_PORT;
    sensor.tag        = TAG_DEV_MAG;
    sensor.path       = dev_mag_path;
    sensor.open       = drv_mag_st_lis3mdl_open;
    sensor.close      = drv_mag_st_lis3mdl_close;
    sensor.read       = drv_mag_st_lis3mdl_read;
    sensor.write      = NULL;
    sensor.ioctl      = drv_mag_st_lis3mdl_ioctl;
    sensor.irq_handle = drv_mag_st_lis3mdl_irq_handle;

    ret = sensor_create_obj(&sensor);
    if (unlikely(ret)) {
        return -1;
    }

    ret = drv_mag_st_lis3mdl_validate_id(&LIS3MDL_ctx, I_AM_LIS3MDL);
    if (unlikely(ret)) {
        return -1;
    }

    ret = drv_mag_st_lis3mdl_soft_reset(&LIS3MDL_ctx);
    if (unlikely(ret)) {
        return -1;
    }

    ret = drv_mag_st_lis3mdl_set_default_config(&LIS3MDL_ctx);
    if (unlikely(ret)) {
        return -1;
    }

    /* update the phy sensor info to sensor hal */
    LOG("%s %s successfully \n", SENSOR_STR, __func__);
    return 0;
}

SENSOR_DRV_ADD(drv_mag_st_lis3mdl_init);

