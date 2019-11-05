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


#define ADXL355_I2C_ADDR1 (0x1D)
#define ADXL355_I2C_ADDR2 (0x53)
#define ADXL355_I2C_ADDR_TRANS(n) ((n) << 1)
#define ADXL355_I2C_ADDR ADXL355_I2C_ADDR_TRANS(ADXL355_I2C_ADDR1)

#define ADI_ADXL355_ADI_DEVID                                                 \
    0x00u                           /* Analog Devices, Inc., accelerometer ID \
                                     */
#define ADI_ADXL355_MST_DEVID 0x01u /* Analog Devices MEMS device ID */
#define ADI_ADXL355_DEVID 0x02u     /* Device ID */
#define ADI_ADXL355_REVID 0x03u     /* product revision ID*/
#define ADI_ADXL355_STATUS 0x04u    /* Status register */
#define ADI_ADXL355_FIFO_ENTRIES 0x05u /* Valid data samples in the FIFO */
#define ADI_ADXL355_TEMP_DATA_2 \
    0x06u /* Temperature sensor output data MSBs [11:8]*/
#define ADI_ADXL355_TEMP_DATA_1 \
    0x07u                        /* Temperature sensor output data LSBs [7:0]*/
#define ADI_ADXL355_XDATA3 0x08u /* X-axis acceleration data [19:12]*/
#define ADI_ADXL355_XDATA2 0x09u /* X-axis acceleration data [11:4]*/
#define ADI_ADXL355_XDATA1 \
    0x0Au /* X-axis acceleration data [3:0] | LSBs reserved*/
#define ADI_ADXL355_YDATA3 0x0Bu /* Y-axis acceleration data [19:12]*/
#define ADI_ADXL355_YDATA2 0x0Cu /* Y-axis acceleration data [11:4]*/
#define ADI_ADXL355_YDATA1 \
    0x0Du /* Y-axis acceleration data [3:0] | LSBs reserved*/
#define ADI_ADXL355_ZDATA3 0x0Eu /* Z-axis acceleration data [19:12]*/
#define ADI_ADXL355_ZDATA2 0x0Fu /* Z-axis acceleration data [11:4]*/
#define ADI_ADXL355_ZDATA1 \
    0x10u /* Z-axis acceleration data [3:0] | LSBs reserved*/
#define ADI_ADXL355_OFFSET_X_H 0x1Eu   /* X axis offset [15:8] */
#define ADI_ADXL355_OFFSET_X_L 0x1Fu   /* X axis offset [7:0] */
#define ADI_ADYL355_OFFSET_Y_H 0x20u   /* Y axis offset [15:8] */
#define ADI_ADYL355_OFFSET_Y_L 0x21u   /* Y axis offset [7:0] */
#define ADI_ADZL355_OFFSET_Z_H 0x22u   /* Z axis offset [15:8] */
#define ADI_ADZL355_OFFSET_Z_L 0x23u   /* Z axis offset [7:0] */
#define ADI_ADXL355_ACT_EN 0x24u       /* Act Control  */
#define ADI_ADXL355_ACT_THRESH_H 0x25u /* Activity threshold MSBs*/
#define ADI_ADXL355_ACT_THRESH_L 0x26u /* Activity threshold LSBs*/
#define ADI_ADXL355_ACT_COUNT 0x27u    /* Activity Count */
#define ADI_ADXL355_FILTER 0x28u       /* Filter Options */
#define ADI_ADXL355_FIFO_SAMPLES 0x29u /* FIFO samples */
#define ADI_ADXL355_INT_MAP 0x2Au      /* Interrupt mapping control */
#define ADI_ADXL355_SYNC 0x2Bu         /* Sync */
#define ADI_ADXL355_RANGE 0x2Cu        /* Capture Range */
#define ADI_ADXL355_POWER_CTL 0x2Du    /* Power control */
#define ADI_ADXL355_SELF_TEST 0x2Eu    /* Self Test */
#define ADI_ADXL355_SRESET 0x2Fu       /* Soft Reset */

#define ADXL355_ENABLE_SOFT_RESET_VALUE 0x52u
#define ADXL355_DEVICE_ID_VALUE 0xEDu
#define ADXL355_POWER_CTL_MSK 0x01u
#define ADXL355_MEASURE_MODE 0x00u
#define ADXL355_STAND_BY_MODE 0x01u

#define ADXL355_ODR_3_9HZ 0x0Au
#define ADXL355_ODR_7_8HZ 0x09u
#define ADXL355_ODR_15_6HZ 0x08u
#define ADXL355_ODR_31_2HZ 0x07u
#define ADXL355_ODR_62_5HZ 0x06u
#define ADXL355_ODR_125HZ 0x05u
#define ADXL355_ODR_250HZ 0x04u
#define ADXL355_ODR_500HZ 0x03u
#define ADXL355_ODR_1000HZ 0x02u
#define ADXL355_ODR_2000HZ 0x01u
#define ADXL355_ODR_4000HZ 0x00u
#define ADXL355_ODR_BIT_MASK 0xF0u
#define ADXL355_DEFAULT_ODR_125HZ 125

#define BITP_ADXL355_RANGE 0x00u
#define BITM_ADXL355_RANGE 0x03u
#define BITM_ADXL355_ACT_EN 0x03u /*ACT_Z, ACT_Y and ACT_X */
#define BITM_ADXL355_INT_POL 0x40u
#define BITM_ADXL355_INT_DATA_READY 0x01u
#define BITM_ADXL355_MEASURE_EN 0x01u
#define BITM_ADXL355_ODR 0x0Fu
#define BITM_ADXL355_SELFTEST_EN 0x03u

// adxl355 sensitivity factor table, the unit is LSB/g

typedef enum _adxl355_self_test_type_
{
    en_adxl355_test_disable,
    en_adxl355_test_x,
    en_adxl355_test_y,
    en_adxl355_test_z,
    en_adxl355_test_invalid,
} adxl355_self_test_type;


i2c_dev_t adxl355_ctx = {
    .port                 = 3,
    .config.address_width = 8,
    .config.freq          = 400000,
    .config.dev_addr      = ADXL355_I2C_ADDR,
};

// adxl355 sensitivity factor table, the unit is LSB/g
uint32_t current_factor  = 10240;
uint32_t g_adxl355_range = ACC_RANGE_2G;

// static int drv_acc_adi_adxl355_self_test(i2c_dev_t* drv,int32_t* data);

UNUSED static int drv_acc_adi_adxl355_soft_reset(i2c_dev_t *drv)
{
    int     ret   = 0;
    uint8_t value = ADXL355_ENABLE_SOFT_RESET_VALUE;
    ret = sensor_i2c_write(drv, ADI_ADXL355_SRESET, &value, I2C_DATA_LEN,
                           I2C_OP_RETRIES);

    return ret;
}

static int drv_acc_adi_adxl355_validate_id(i2c_dev_t *drv, uint8_t id_value)
{
    uint8_t value = 0x00;
    int     ret   = 0;

    if (drv == NULL) {
        return -1;
    }

    ret = sensor_i2c_read(drv, ADI_ADXL355_DEVID, &value, I2C_DATA_LEN,
                          I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return -1;
    }

    if (id_value != value) {
        return -1;
    }

    return 0;
}

static int drv_acc_adi_adxl355_set_power_mode(i2c_dev_t *      drv,
                                              dev_power_mode_e mode)
{
    uint8_t value;
    int     ret = 0;

    ret = sensor_i2c_read(drv, ADI_ADXL355_POWER_CTL, &value, I2C_DATA_LEN,
                          I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    switch (mode) {
        case DEV_POWER_ON: {
            value &= 0xFE;
            ret = sensor_i2c_write(drv, ADI_ADXL355_POWER_CTL, &value,
                                   I2C_DATA_LEN, I2C_OP_RETRIES);
            if (unlikely(ret)) {
                return ret;
            }
        } break;

        case DEV_POWER_OFF: {
            value |= 0x01;
            ret = sensor_i2c_write(drv, ADI_ADXL355_POWER_CTL, &value,
                                   I2C_DATA_LEN, I2C_OP_RETRIES);
            if (unlikely(ret)) {
                return ret;
            }
        } break;

        default:
            break;
    }
    return 0;
}

static int drv_acc_adi_adxl355_set_odr(i2c_dev_t *drv, uint32_t odr)
{
    int     ret   = 0;
    uint8_t value = 0x00;

    ret = sensor_i2c_read(drv, ADI_ADXL355_FILTER, &value, I2C_DATA_LEN,
                          I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    if (odr > 2000) {
        value &= ADXL355_ODR_BIT_MASK;
        value |= ADXL355_ODR_4000HZ;
    } else if (odr > 1000) {
        value &= ADXL355_ODR_BIT_MASK;
        value |= ADXL355_ODR_2000HZ;
    } else if (odr > 500) {
        value &= ADXL355_ODR_BIT_MASK;
        value |= ADXL355_ODR_1000HZ;
    } else if (odr > 250) {
        value &= ADXL355_ODR_BIT_MASK;
        value |= ADXL355_ODR_500HZ;
    } else if (odr > 125) {
        value &= ADXL355_ODR_BIT_MASK;
        value |= ADXL355_ODR_250HZ;
    } else if (odr > 63) {
        value &= ADXL355_ODR_BIT_MASK;
        value |= ADXL355_ODR_125HZ;
    } else if (odr > 32) {
        value &= ADXL355_ODR_BIT_MASK;
        value |= ADXL355_ODR_62_5HZ;
    } else if (odr > 16) {
        value &= ADXL355_ODR_BIT_MASK;
        value |= ADXL355_ODR_31_2HZ;
    } else if (odr > 8) {
        value &= ADXL355_ODR_BIT_MASK;
        value |= ADXL355_ODR_15_6HZ;
    } else if (odr > 4) {
        value &= ADXL355_ODR_BIT_MASK;
        value |= ADXL355_ODR_7_8HZ;
    } else {
        value &= ADXL355_ODR_BIT_MASK;
        value |= ADXL355_ODR_3_9HZ;
    }

    ret = sensor_i2c_write(drv, ADI_ADXL355_FILTER, &value, I2C_DATA_LEN,
                           I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }
    return 0;
}

static int drv_acc_adi_adxl355_set_range(i2c_dev_t *drv, uint32_t range)
{
    int     ret   = 0;
    uint8_t value = 0x00;

    ret = sensor_i2c_read(drv, ADI_ADXL355_RANGE, &value, I2C_DATA_LEN,
                          I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    switch (range) {
        case ACC_RANGE_2G: {
            value &= 0xFC;
            value |= 0x01;
        } break;

        case ACC_RANGE_4G: {
            value &= 0xFC;
            value |= 0x02;
        } break;

        case ACC_RANGE_8G: {
            value &= 0xFC;
            value |= 0x03;
        } break;

        default:
            break;
    }

    /* Write the range register 0x2C*/
    ret = sensor_i2c_write(drv, ADI_ADXL355_RANGE, &value, I2C_DATA_LEN,
                           I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    g_adxl355_range = range;
    return 0;
}


static void drv_acc_adi_adxl355_irq_handle(void)
{
    /* no handle so far */
}

static int drv_acc_adi_adxl355_open(void)
{
    int ret = 0;
    ret     = drv_acc_adi_adxl355_set_power_mode(&adxl355_ctx, DEV_POWER_ON);
    if (unlikely(ret)) {
        return -1;
    }
    return 0;
}

static int drv_acc_adi_adxl355_close(void)
{
    int ret = 0;
    ret     = drv_acc_adi_adxl355_set_power_mode(&adxl355_ctx, DEV_POWER_OFF);
    if (unlikely(ret)) {
        return -1;
    }
    return 0;
}

static int drv_acc_adi_adxl355_read(void *buf, size_t len)
{
    int     ret = 0;
    size_t  size;
    uint8_t LocalBuf[9];
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

    ret = sensor_i2c_read(&adxl355_ctx, ADI_ADXL355_XDATA3, &LocalBuf[0],
                          I2C_REG_LEN, I2C_OP_RETRIES);
    ret |= sensor_i2c_read(&adxl355_ctx, ADI_ADXL355_XDATA2, &LocalBuf[1],
                           I2C_REG_LEN, I2C_OP_RETRIES);
    ret |= sensor_i2c_read(&adxl355_ctx, ADI_ADXL355_XDATA1, &LocalBuf[2],
                           I2C_REG_LEN, I2C_OP_RETRIES);
    ret |= sensor_i2c_read(&adxl355_ctx, ADI_ADXL355_YDATA3, &LocalBuf[3],
                           I2C_REG_LEN, I2C_OP_RETRIES);
    ret |= sensor_i2c_read(&adxl355_ctx, ADI_ADXL355_YDATA2, &LocalBuf[4],
                           I2C_REG_LEN, I2C_OP_RETRIES);
    ret |= sensor_i2c_read(&adxl355_ctx, ADI_ADXL355_YDATA1, &LocalBuf[5],
                           I2C_REG_LEN, I2C_OP_RETRIES);
    ret |= sensor_i2c_read(&adxl355_ctx, ADI_ADXL355_ZDATA3, &LocalBuf[6],
                           I2C_REG_LEN, I2C_OP_RETRIES);
    ret |= sensor_i2c_read(&adxl355_ctx, ADI_ADXL355_ZDATA2, &LocalBuf[7],
                           I2C_REG_LEN, I2C_OP_RETRIES);
    ret |= sensor_i2c_read(&adxl355_ctx, ADI_ADXL355_ZDATA1, &LocalBuf[8],
                           I2C_REG_LEN, I2C_OP_RETRIES);

    if (unlikely(ret)) {
        return -1;
    }

    x_val   = (int32_t)(((LocalBuf[0] << 16) | (LocalBuf[1] << 8) |
                       ((LocalBuf[2] << 0) & 0xF0)) >>
                      4);
    SignBit = (x_val & (1 << 19)) != 0;
    if (SignBit)
        x_val = x_val | ~((1 << 20) - 1);

    y_val   = (int32_t)(((LocalBuf[3] << 16) | (LocalBuf[4] << 8) |
                       ((LocalBuf[5] << 0) & 0xF0)) >>
                      4);
    SignBit = (y_val & (1 << 19)) != 0;
    if (SignBit)
        y_val = y_val | ~((1 << 20) - 1);

    z_val   = (int32_t)(((LocalBuf[6] << 16) | (LocalBuf[7] << 8) |
                       ((LocalBuf[8] << 0) & 0xF0)) >>
                      4);
    SignBit = (z_val & (1 << 19)) != 0;
    if (SignBit)
        z_val = z_val | ~((1 << 20) - 1);

    if (g_adxl355_range == ACC_RANGE_4G) {
        current_factor = 5120;
    } else if (g_adxl355_range == ACC_RANGE_8G) {
        current_factor = 2160;
    }

    accel->data[DATA_AXIS_X] = (int32_t)(x_val * 39 / (int32_t)current_factor);
    accel->data[DATA_AXIS_Y] = (int32_t)(y_val * 39 / (int32_t)current_factor);
    accel->data[DATA_AXIS_Z] = (int32_t)(z_val * 39 / (int32_t)current_factor);
    accel->timestamp         = aos_now_ms();

    return (int)size;
}

static int drv_acc_adi_adxl355_ioctl(int cmd, unsigned long arg)
{
    int                ret  = 0;
    dev_sensor_info_t *info = (dev_sensor_info_t *)arg;
    switch (cmd) {
        case SENSOR_IOCTL_ODR_SET: {
            ret = drv_acc_adi_adxl355_set_odr(&adxl355_ctx, arg);
            if (unlikely(ret)) {
                return -1;
            }
        } break;
        case SENSOR_IOCTL_RANGE_SET: {
            ret = drv_acc_adi_adxl355_set_range(&adxl355_ctx, arg);
            if (unlikely(ret)) {
                return -1;
            }
        } break;
        case SENSOR_IOCTL_SET_POWER: {
            ret = drv_acc_adi_adxl355_set_power_mode(&adxl355_ctx, arg);
            if (unlikely(ret)) {
                return -1;
            }
        } break;
        case SENSOR_IOCTL_GET_INFO: {
            /* fill the dev info here */
            info->model     = "ADXL355";
            info->range_max = 8;
            info->range_min = 2;
            info->unit      = mg;
        } break;

        default:
            break;
    }

    return 0;
}

int drv_acc_adi_adxl355_init(void)
{
    int          ret = 0;
    sensor_obj_t sensor;
    memset(&sensor, 0, sizeof(sensor));

    /* fill the sensor obj parameters here */
    sensor.io_port    = I2C_PORT;
    sensor.tag        = TAG_DEV_ACC;
    sensor.path       = dev_acc_path;
    sensor.open       = drv_acc_adi_adxl355_open;
    sensor.close      = drv_acc_adi_adxl355_close;
    sensor.read       = drv_acc_adi_adxl355_read;
    sensor.write      = NULL;
    sensor.ioctl      = drv_acc_adi_adxl355_ioctl;
    sensor.irq_handle = drv_acc_adi_adxl355_irq_handle;

    ret = sensor_create_obj(&sensor);
    if (unlikely(ret)) {
        return -1;
    }

    ret =
      drv_acc_adi_adxl355_validate_id(&adxl355_ctx, ADXL355_DEVICE_ID_VALUE);
    if (unlikely(ret)) {
        return -1;
    }

    ret = drv_acc_adi_adxl355_set_range(&adxl355_ctx, ACC_RANGE_4G);
    if (unlikely(ret)) {
        return -1;
    }

    // set odr is 125hz, and will update
    ret = drv_acc_adi_adxl355_set_odr(&adxl355_ctx, ADXL355_DEFAULT_ODR_125HZ);
    if (unlikely(ret)) {
        return -1;
    }
    ret = drv_acc_adi_adxl355_set_power_mode(&adxl355_ctx, DEV_POWER_ON);
    if (unlikely(ret)) {
        return -1;
    }
    /* update the phy sensor info to sensor hal */
    LOG("%s %s successfully \n", SENSOR_STR, __func__);
    return 0;
}

SENSOR_DRV_ADD(drv_acc_adi_adxl355_init);

