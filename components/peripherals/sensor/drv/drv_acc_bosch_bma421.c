/*
* Copyright (C) 2015-2017 Alibaba Group Holding Limited
*
*
*/

#include "aos/kernel.h"
#include "network/hal/base.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sensor_drv_api.h"
#include "sensor_hal.h"

#define BMA421_I2C_ADDR_LOW                     (0x18)
#define BMA421_I2C_ADDR_HIGH                    (0x19)
#define BMA421_I2C_ADDR_TRANS(n)                ((n)<<1)
#define BMA421_I2C_ADDR                         BMA421_I2C_ADDR_TRANS(BMA421_I2C_ADDR_LOW)

#define BMA421_CHIP_ID_ADDR                     UINT8_C(0X00)
#define BMA421_CHIP_ID_VALUE                    (0x11)

#define BMA421_POWER_CONF_ADDR                  UINT8_C(0x7C)
#define BMA421_POWER_CTRL_ADDR                  UINT8_C(0x7D)

#define BMA421_ACCEL_CONFIG_ADDR                UINT8_C(0X40)
#define BMA421_ACCEL_CONFIG1_ADDR               UINT8_C(0X41)

#define BMA421_DEFAULT_ODR_100HZ                (100)
#define BMA421_CMD_ADDR                         UINT8_C(0X7E)

#define BMA421_OUTPUT_DATA_RATE_0_78HZ          UINT8_C(0x01)
#define BMA421_OUTPUT_DATA_RATE_1_56HZ          UINT8_C(0x02)
#define BMA421_OUTPUT_DATA_RATE_3_12HZ          UINT8_C(0x03)
#define BMA421_OUTPUT_DATA_RATE_6_25HZ          UINT8_C(0x04)
#define BMA421_OUTPUT_DATA_RATE_12_5HZ          UINT8_C(0x05)
#define BMA421_OUTPUT_DATA_RATE_25HZ            UINT8_C(0x06)
#define BMA421_OUTPUT_DATA_RATE_50HZ            UINT8_C(0x07)
#define BMA421_OUTPUT_DATA_RATE_100HZ           UINT8_C(0x08)
#define BMA421_OUTPUT_DATA_RATE_200HZ           UINT8_C(0x09)
#define BMA421_OUTPUT_DATA_RATE_400HZ           UINT8_C(0x0A)
#define BMA421_OUTPUT_DATA_RATE_800HZ           UINT8_C(0x0B)
#define BMA421_OUTPUT_DATA_RATE_1600HZ          UINT8_C(0x0C)

#define BMA421_ACCEL_OSR4_AVG1                  UINT8_C(0)
#define BMA421_ACCEL_OSR2_AVG2                  UINT8_C(1)
#define BMA421_ACCEL_NORMAL_AVG4                UINT8_C(2)
#define BMA421_ACCEL_CIC_AVG8                   UINT8_C(3)
#define BMA421_ACCEL_RES_AVG16                  UINT8_C(4)
#define BMA421_ACCEL_RES_AVG32                  UINT8_C(5)
#define BMA421_ACCEL_RES_AVG64                  UINT8_C(6)
#define BMA421_ACCEL_RES_AVG128                 UINT8_C(7)

#define BMA421_ACCEL_RANGE_2G                   UINT8_C(0)
#define BMA421_ACCEL_RANGE_4G                   UINT8_C(1)
#define BMA421_ACCEL_RANGE_8G                   UINT8_C(2)
#define BMA421_ACCEL_RANGE_16G                  UINT8_C(3)

#define BMA421_DATA_0_ADDR                      UINT8_C(0X0A)
#define BMA421_DATA_8_ADDR                      UINT8_C(0X12)
#define BMA421_ACCEL_DATA_LENGTH                UINT8_C(6)

#define BMA421_ENABLE                           UINT8_C(0x01)
#define BMA421_DISABLE                          UINT8_C(0x00)

#define BMA421_ADVANCE_POWER_SAVE_MSK           UINT8_C(0x01)

#define BMA421_ACCEL_ENABLE_POS                 UINT8_C(2)
#define BMA421_ACCEL_ENABLE_MSK                 UINT8_C(0x04)

#define BMA421_ACCEL_PERF_POS                   UINT8_C(7)

#define BMA421_ACCEL_ODR_POS                    UINT8_C(0)
#define BMA421_ACCEL_ODR_MSK                    UINT8_C(0x0F)

#define BMA421_ACCEL_RANGE_POS                  UINT8_C(0)
#define BMA421_ACCEL_RANGE_MSK                  UINT8_C(0x03)

#define BMA421_ENABLE_SOFT_RESET_VALUE          UINT8_C(0XB6)

#define BMA421_GET_BITSLICE(regvar, bitname)\
  ((regvar & bitname##_MSK) >> bitname##_POS)
#define BMA421_SET_BITSLICE(regvar, bitname, val)\
  ((regvar & ~bitname##_MSK) | \
  ((val<<bitname##_POS)&bitname##_MSK))
#define BMA421_SET_BITS_POS_0(reg_data, bitname, data) \
    ((reg_data & ~(bitname##_MSK)) | \
    (data & bitname##_MSK))

#define BMA421_GET_BITS_POS_0(reg_data, bitname) (reg_data & (bitname##_MSK))

static uint32_t bma421_factor[4] = { 16384, 8192, 4096, 2048 };
static uint32_t current_factor = 0;
static uint32_t set_range_failed = 0;

i2c_dev_t bma421_ctx = {
    .port = 3,
    .config.dev_addr = BMA421_I2C_ADDR,
};

int drv_acc_bosch_bma421_soft_reset(i2c_dev_t* drv)
{
    int ret = 0;
    uint8_t value = BMA421_ENABLE_SOFT_RESET_VALUE;
    ret = sensor_i2c_write(drv, BMA421_CMD_ADDR, &value,
                           I2C_DATA_LEN, I2C_OP_RETRIES);
    aos_msleep(2);
    if(unlikely(ret)) {
        return -1;
    }
    return 0;
}

int drv_acc_bosch_bma421_validate_id(i2c_dev_t* drv)
{
    uint8_t value = 0x00;
    int ret = 0;

    if(drv == NULL) {
        return -1;
    }

    ret = sensor_i2c_read(drv, BMA421_CHIP_ID_ADDR, &value, I2C_DATA_LEN,
                          I2C_OP_RETRIES);
    if(unlikely(ret)) {
        LOG("%s %s  sensor_i2c_read \n", SENSOR_STR, __func__);
        return ret;
    }

    if(BMA421_CHIP_ID_VALUE != value) {
        LOG("%s %s  value=%x \n", SENSOR_STR, __func__, value);
        return -1;
    }

    return 0;
}

int drv_acc_bosch_bma421_set_power_mode(i2c_dev_t* drv,
                                               dev_power_mode_e mode)
{

    uint8_t value, value1 = 0x00;
    int ret = 0;
    switch(mode) {
        case DEV_POWER_ON: {
            ret = sensor_i2c_read(drv, BMA421_ACCEL_CONFIG_ADDR, &value, I2C_DATA_LEN,
                                  I2C_OP_RETRIES);
            if(unlikely(ret)) {
                return ret;
            }
            value |= (BMA421_ENABLE << BMA421_ACCEL_PERF_POS);
            ret = sensor_i2c_write(drv, BMA421_ACCEL_CONFIG_ADDR, &value, I2C_DATA_LEN,
                                   I2C_OP_RETRIES);
            aos_msleep(2);
            if(unlikely(ret)) {
                return ret;
            }

            ret = sensor_i2c_read(drv, BMA421_POWER_CTRL_ADDR, &value1, I2C_DATA_LEN,
                                  I2C_OP_RETRIES);
            if(unlikely(ret)) {
                return ret;
            }
            value1 = BMA421_SET_BITSLICE(value1, BMA421_ACCEL_ENABLE, BMA421_ENABLE);
            ret = sensor_i2c_write(drv, BMA421_POWER_CTRL_ADDR, &value1, I2C_DATA_LEN,
                                   I2C_OP_RETRIES);
            aos_msleep(2);
            if(unlikely(ret)) {
                return ret;
            }
        }
        break;
        case DEV_POWER_OFF:
        case DEV_SLEEP:
        case DEV_SUSPEND: {
            ret = sensor_i2c_read(drv, BMA421_POWER_CTRL_ADDR, &value, I2C_DATA_LEN,
                                  I2C_OP_RETRIES);
            if(unlikely(ret)) {
                return ret;
            }
            value = BMA421_SET_BITSLICE(value, BMA421_ACCEL_ENABLE, BMA421_DISABLE);
            ret = sensor_i2c_write(drv, BMA421_POWER_CTRL_ADDR, &value, I2C_DATA_LEN,
                                   I2C_OP_RETRIES);
            aos_msleep(2);
            if(unlikely(ret)) {
                return ret;
            }

            ret = sensor_i2c_read(drv, BMA421_POWER_CONF_ADDR, &value1, I2C_DATA_LEN,
                                  I2C_OP_RETRIES);
            if(unlikely(ret)) {
                return ret;
            }
            value1 = BMA421_SET_BITS_POS_0(value1, BMA421_ADVANCE_POWER_SAVE, BMA421_ENABLE);
            ret = sensor_i2c_write(drv, BMA421_POWER_CONF_ADDR, &value1, I2C_DATA_LEN,
                                   I2C_OP_RETRIES);
            aos_msleep(2);
            if(unlikely(ret)) {
                return ret;
            }
        }
        break;

        default:
            break;
    }
    return 0;
}

int drv_acc_bosch_bma421_set_odr(i2c_dev_t* drv, uint32_t hz)
{
    int ret = 0;
    uint8_t value = 0x00;
    uint8_t odr = 0x00;

    ret = sensor_i2c_read(drv, BMA421_ACCEL_CONFIG_ADDR, &value, I2C_DATA_LEN,
                          I2C_OP_RETRIES);
    if(unlikely(ret)) {
        return ret;
    }

    if(hz >= 800) {
        odr = BMA421_OUTPUT_DATA_RATE_1600HZ;
    } else if(hz >= 400) {
        odr = BMA421_OUTPUT_DATA_RATE_800HZ;
    } else if(hz >= 200) {
        odr = BMA421_OUTPUT_DATA_RATE_400HZ;
    } else if(hz >= 100) {
        odr = BMA421_OUTPUT_DATA_RATE_200HZ;
    } else if(hz >= 50) {
        odr = BMA421_OUTPUT_DATA_RATE_100HZ;
    } else if(hz >= 25) {
        odr = BMA421_OUTPUT_DATA_RATE_50HZ;
    } else if(hz >= 12) {
        odr = BMA421_OUTPUT_DATA_RATE_25HZ;
    } else if(hz >= 6) {
        odr = BMA421_OUTPUT_DATA_RATE_12_5HZ;
    } else if(hz >= 3) {
        odr = BMA421_OUTPUT_DATA_RATE_6_25HZ;
    } else {
        odr = BMA421_OUTPUT_DATA_RATE_3_12HZ;
    }

    value = BMA421_SET_BITSLICE(value, BMA421_ACCEL_ODR, odr);
    ret = sensor_i2c_write(drv, BMA421_ACCEL_CONFIG_ADDR, &value, I2C_DATA_LEN,
                           I2C_OP_RETRIES);
    aos_msleep(2);
    if(unlikely(ret)) {
        return ret;
    }
    return 0;
}

int drv_acc_bosch_bma421_set_range(i2c_dev_t* drv, uint32_t range)
{
    int ret = 0;
    uint8_t value = 0x00;
    uint8_t acc_range = 0x00;

    ret = sensor_i2c_read(drv, BMA421_ACCEL_CONFIG1_ADDR, &value, I2C_DATA_LEN,
                          I2C_OP_RETRIES);
    if(unlikely(ret)) {
        set_range_failed = 1;
        return ret;
    }

    switch(range) {
        case ACC_RANGE_2G: {
            acc_range = BMA421_ACCEL_RANGE_2G;
        }
        break;

        case ACC_RANGE_4G: {
            acc_range = BMA421_ACCEL_RANGE_4G;
        }
        break;

        case ACC_RANGE_8G: {
            acc_range = BMA421_ACCEL_RANGE_8G;
        }
        break;

        case ACC_RANGE_16G: {
            acc_range = BMA421_ACCEL_RANGE_16G;
        }
        break;

        default:
            break;
    }

    value = BMA421_SET_BITSLICE(value, BMA421_ACCEL_RANGE, acc_range);
    ret = sensor_i2c_write(drv, BMA421_ACCEL_CONFIG1_ADDR, &value, I2C_DATA_LEN,
                           I2C_OP_RETRIES);
    aos_msleep(2);
    if(unlikely(ret)) {
        set_range_failed = 2;
        return ret;
    }

    if((range >= ACC_RANGE_2G)&&(range <= ACC_RANGE_16G)) {
        current_factor = bma421_factor[range];
    }

    set_range_failed = 3;
    return 0;
}

void drv_acc_bosch_bma421_irq_handle(void)
{
    /* no handle so far */
}

int drv_acc_bosch_bma421_open(void)
{
    int ret = 0;
    ret  = drv_acc_bosch_bma421_set_power_mode(&bma421_ctx, DEV_POWER_ON);
    if(unlikely(ret)) {
        return -1;
    }
    return 0;

}


int drv_acc_bosch_bma421_close(void)
{
    int ret = 0;
    ret  = drv_acc_bosch_bma421_set_power_mode(&bma421_ctx, DEV_POWER_OFF);
    if(unlikely(ret)) {
        return -1;
    }
    return 0;
}

int drv_acc_bosch_bma421_read(void *buf, size_t len)
{
    int ret = 0;
    size_t size;
    uint8_t reg[6];
    uint8_t range = 0;
    accel_data_t *accel = (accel_data_t *)buf;

    if(buf == NULL) {
        return -1;
    }

    size = sizeof(accel_data_t);
    if(len < size) {
        return -1;
    }
    ret  = sensor_i2c_read(&bma421_ctx, 0x41,  &range, I2C_REG_LEN, I2C_OP_RETRIES);
    ret  = sensor_i2c_read(&bma421_ctx, BMA421_DATA_8_ADDR,  &reg[0], I2C_REG_LEN,
                           I2C_OP_RETRIES);
    ret |= sensor_i2c_read(&bma421_ctx, BMA421_DATA_8_ADDR+1,  &reg[1], I2C_REG_LEN,
                           I2C_OP_RETRIES);
    ret |= sensor_i2c_read(&bma421_ctx, BMA421_DATA_8_ADDR+2,  &reg[2], I2C_REG_LEN,
                           I2C_OP_RETRIES);
    ret |= sensor_i2c_read(&bma421_ctx, BMA421_DATA_8_ADDR+3,  &reg[3], I2C_REG_LEN,
                           I2C_OP_RETRIES);
    ret |= sensor_i2c_read(&bma421_ctx, BMA421_DATA_8_ADDR+4,  &reg[4], I2C_REG_LEN,
                           I2C_OP_RETRIES);
    ret |= sensor_i2c_read(&bma421_ctx, BMA421_DATA_8_ADDR+5,  &reg[5], I2C_REG_LEN,
                           I2C_OP_RETRIES);

    if(unlikely(ret)) {
        return -1;
    }

    accel->data[DATA_AXIS_X] = ((int16_t)((reg[1] << 8) | reg[0]));
    accel->data[DATA_AXIS_Y] = ((int16_t)((reg[3] << 8) | reg[2]));
    accel->data[DATA_AXIS_Z] = ((int16_t)((reg[5] << 8) | reg[4]));
    if(current_factor != 0) {
        accel->data[DATA_AXIS_X] = accel->data[DATA_AXIS_X] *
                                   ACCELEROMETER_UNIT_FACTOR / (int32_t)current_factor;
        accel->data[DATA_AXIS_Y] = accel->data[DATA_AXIS_Y] *
                                   ACCELEROMETER_UNIT_FACTOR / (int32_t)current_factor;
        accel->data[DATA_AXIS_Z] = accel->data[DATA_AXIS_Z] *
                                   ACCELEROMETER_UNIT_FACTOR / (int32_t)current_factor;
    }
    accel->timestamp = aos_now_ms();

    return (int)size;
}

static int drv_acc_bosch_bma421_ioctl(int cmd, unsigned long arg)
{
    int ret = 0;

    switch(cmd) {
        case SENSOR_IOCTL_ODR_SET: {
            ret = drv_acc_bosch_bma421_set_odr(&bma421_ctx, arg);
            if(unlikely(ret)) {
                return -1;
            }
        }
        break;
        case SENSOR_IOCTL_RANGE_SET: {
            ret = drv_acc_bosch_bma421_set_range(&bma421_ctx, arg);
            if(unlikely(ret)) {
                return -1;
            }
        }
        break;
        case SENSOR_IOCTL_SET_POWER: {
            ret = drv_acc_bosch_bma421_set_power_mode(&bma421_ctx, arg);
            if(unlikely(ret)) {
                return -1;
            }
        }
        break;
        case SENSOR_IOCTL_GET_INFO: {
            dev_sensor_info_t *info = (dev_sensor_info_t *)arg;
            info->model = "BMA421";
            info->range_max = 16;
            info->range_min = 2;
            info->unit = mg;
        }
        break;

        default:
            break;
    }

    return 0;
}

int drv_acc_bosch_bma421_init(void) {
    int ret = 0;
    sensor_obj_t sensor;
    memset(&sensor, 0, sizeof(sensor));

    /* fill the sensor obj parameters here */
    sensor.io_port    = I2C_PORT;
    sensor.tag        = TAG_DEV_ACC;
    sensor.path       = dev_acc_path;
    sensor.open       = drv_acc_bosch_bma421_open;
    sensor.close      = drv_acc_bosch_bma421_close;
    sensor.read       = drv_acc_bosch_bma421_read;
    sensor.write      = NULL;
    sensor.ioctl      = drv_acc_bosch_bma421_ioctl;
    sensor.irq_handle = drv_acc_bosch_bma421_irq_handle;
    ret = sensor_create_obj(&sensor);
    if(unlikely(ret)) {
        return -1;
    }

    ret = drv_acc_bosch_bma421_validate_id(&bma421_ctx);
    if(unlikely(ret)) {
        return -1;
    }

    ret = drv_acc_bosch_bma421_soft_reset(&bma421_ctx);
    if(unlikely(ret)) {
        return -1;
    }

    ret = drv_acc_bosch_bma421_set_range(&bma421_ctx, ACC_RANGE_8G);
    if(unlikely(ret)) {
        return -1;
    }

    /* set odr is 100hz, and will update */
    ret = drv_acc_bosch_bma421_set_odr(&bma421_ctx, BMA421_DEFAULT_ODR_100HZ);
    if(unlikely(ret)) {
        return -1;
    }
    ret = drv_acc_bosch_bma421_set_power_mode(&bma421_ctx, DEV_SLEEP);
    if(unlikely(ret)) {
        return -1;
    }

    /* update the phy sensor info to sensor hal */
    LOG("%s %s successfully \n", SENSOR_STR, __func__);
    return 0;
}


SENSOR_DRV_ADD(drv_acc_bosch_bma421_init);

