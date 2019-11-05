#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "aos/kernel.h"
#include "network/hal/base.h"
#include "sensor_drv_api.h"
#include "sensor_hal.h"

#define LTR390_I2C_SLAVE_ADDR                           0x53

#define LTR390_MAIN_CTRL                                0x00 /* Operation mode control */
#define LTR390_ALS_UVS_MEAS_RATE                        0x04 /* Measurement rate and resolution */
#define LTR390_ALS_UVS_GAIN                             0x05 /* Analog gain range*/
#define LTR390_PART_ID                                  0x06
#define LTR390_MAIN_STATUS                              0x07
#define LTR390_ALS_DATA_0                               0x0D
#define LTR390_ALS_DATA_1                               0x0E
#define LTR390_ALS_DATA_2                               0x0F
#define LTR390_UVS_DATA_0                               0x10
#define LTR390_UVS_DATA_1                               0x11
#define LTR390_UVS_DATA_2                               0x12
#define LTR390_INT_CFG                                  0x19
#define LTR390_INT_PST                                  0x1A /* Interrupt persist setting */
#define LTR390_ALS_UVS_THRES_UP_0                       0x21 /* Interrupt upper threshold, lower byte */
#define LTR390_ALS_UVS_THRES_UP_1                       0x22 /* Interrupt upper threshold, middle byte */
#define LTR390_ALS_UVS_THRES_UP_2                       0x23 /* Interrupt upper threshold, upper byte */
#define LTR390_ALS_UVS_THRES_LOW_0                      0x24 /* Interrupt lower threshold, lower byte */
#define LTR390_ALS_UVS_THRES_LOW_1                      0x25 /* Interrupt lower threshold, middle byte */
#define LTR390_ALS_UVS_THRES_LOW_2                      0x26 /* Interrupt lower threshold, upper byte */

#define LTR390_ADDR_TRANS(n)                            ((n) << 1)
#define LTR390_I2C_ADDR                                 LTR390_ADDR_TRANS(LTR390_I2C_SLAVE_ADDR)
#define LTR390_PART_ID_VAL                              0xB2

#define LTR390_MAIN_CTRL_REG_ALS_UVS_EN__POS            (1)
#define LTR390_MAIN_CTRL_REG_ALS_UVS_EN__MSK            (0x02)
#define LTR390_MAIN_CTRL_REG_ALS_UVS_EN__REG            (LTR390_MAIN_CTRL)

#define LTR390_MAIN_CTRL_REG_UVS_MODE__POS              (3)
#define LTR390_MAIN_CTRL_REG_UVS_MODE__MSK              (0x08)
#define LTR390_MAIN_CTRL_REG_UVS_MODE__REG              (LTR390_MAIN_CTRL)

#define LTR390_MAIN_CTRL_REG_SW_RESET__POS              (4)
#define LTR390_MAIN_CTRL_REG_SW_RESET__MSK              (0x10)
#define LTR390_MAIN_CTRL_REG_SW_RESET__REG              (LTR390_MAIN_CTRL)

#define LTR390_ALS_UVS_MEAS_RATE_REG_MEAS_RATE__POS     (0)
#define LTR390_ALS_UVS_MEAS_RATE_REG_MEAS_RATE__MSK     (0x07)
#define LTR390_ALS_UVS_MEAS_RATE_REG_MEAS_RATE__REG     (LTR390_ALS_UVS_MEAS_RATE)

#define LTR390_ALS_UVS_MEAS_RATE_REG_RESOLUTION__POS    (4)
#define LTR390_ALS_UVS_MEAS_RATE_REG_RESOLUTION__MSK    (0x70)
#define LTR390_ALS_UVS_MEAS_RATE_REG_RESOLUTION__REG    (LTR390_ALS_UVS_MEAS_RATE)

#define LTR390_ALS_UVS_GAIN_REG_GAIN_RANGE__POS         (0)
#define LTR390_ALS_UVS_GAIN_REG_GAIN_RANGE__MSK         (0x07)
#define LTR390_ALS_UVS_GAIN_REG_GAIN_RANGE__REG         (LTR390_ALS_UVS_GAIN)

#define LTR390_MAIN_STATUS_REG_ALS_UVS_DATA_STATUS__POS (3)
#define LTR390_MAIN_STATUS_REG_ALS_UVS_DATA_STATUS__MSK (0x08)
#define LTR390_MAIN_STATUS_REG_ALS_UVS_DATA_STATUS__REG (LTR390_MAIN_STATUS)

#define LTR390_MAIN_STATUS_REG_ALS_UVS_INT_STATUS__POS  (4)
#define LTR390_MAIN_STATUS_REG_ALS_UVS_INT_STATUS__MSK  (0x10)
#define LTR390_MAIN_STATUS_REG_ALS_UVS_INT_STATUS__REG  (LTR390_MAIN_STATUS)

#define LTR390_MAIN_STATUS_REG_POWER_ON_STATUS__POS     (5)
#define LTR390_MAIN_STATUS_REG_POWER_ON_STATUS__MSK     (0x20)
#define LTR390_MAIN_STATUS_REG_POWER_ON_STATUS__REG     (LTR390_MAIN_STATUS)

#define LTR390_INT_CFG_REG_LS_INT_EN__POS               (2)
#define LTR390_INT_CFG_REG_LS_INT_EN__MSK               (0x04)
#define LTR390_INT_CFG_REG_LS_INT_EN__REG               (LTR390_INT_CFG)

#define LTR390_INT_CFG_REG_LS_INT_SEL__POS              (4)
#define LTR390_INT_CFG_REG_LS_INT_SEL__MSK              (0x30)
#define LTR390_INT_CFG_REG_LS_INT_SEL__REG              (LTR390_INT_CFG)

#define LTR390_INT_PST_REG_ALS_UV_PERSIST__POS          (4)
#define LTR390_INT_PST_REG_ALS_UV_PERSIST__MSK          (0xF0)
#define LTR390_INT_PST_REG_ALS_UV_PERSIST__REG          (LTR390_INT_PST)

#define LTR390_GET_BITSLICE(regvar, bitname)            ((regvar & LTR390_##bitname##__MSK) >> LTR390_##bitname##__POS)
#define LTR390_SET_BITSLICE(regvar, bitname, val)       ((regvar & ~LTR390_##bitname##__MSK) | ((val<<LTR390_##bitname##__POS)&LTR390_##bitname##__MSK))

#define LTR390_WAIT_TIME_PER_CHECK                      (10)
#define LTR390_WAIT_TIME_TOTAL                          (100)

typedef enum {
    LTR390_ALS_UVS_STANDBY = 0x00,
    LTR390_ALS_UVS_ACTIVE = 0x01, 
} LTR390_CFG_ALS_UVS_EN;

typedef enum {
    LTR390_ALS_MODE = 0x00,
    LTR390_UVS_MODE = 0x01, 
} LTR390_CFG_ALS_UVS_MODE;

typedef enum {
    LTR390_SW_RESET_FALSE = 0x00,
    LTR390_SW_RESET_TRUE = 0x01, 
} LTR390_CFG_SW_RESET;

typedef enum {
    LTR390_ALS_UVS_MEAS_RATE_25 = 0x00,                      /* Measurement Repeat Rate = 25ms */
    LTR390_ALS_UVS_MEAS_RATE_50 = 0x01,                      /* Measurement Repeat Rate = 50ms */
    LTR390_ALS_UVS_MEAS_RATE_100 = 0x02,                     /* Measurement Repeat Rate = 100ms (default) */
    LTR390_ALS_UVS_MEAS_RATE_200 = 0x03,                     /* Measurement Repeat Rate = 200ms */
    LTR390_ALS_UVS_MEAS_RATE_500 = 0x04,                     /* Measurement Repeat Rate = 500ms */
    LTR390_ALS_UVS_MEAS_RATE_1000 = 0x05,                    /* Measurement Repeat Rate = 1000ms */
    LTR390_ALS_UVS_MEAS_RATE_2000 = 0x06,                    /* Measurement Repeat Rate = 2000ms */
} LTR390_CFG_ALS_UVS_MEAS_RATE;

typedef enum {
    LTR390_ALS_UVS_RESOLUTION_20BIT = 0x00,                  /* 20 bit, conversion time = 400ms */
    LTR390_ALS_UVS_RESOLUTION_19BIT = 0x01,                  /* 19 bit, conversion time = 200ms */
    LTR390_ALS_UVS_RESOLUTION_18BIT = 0x02,                  /* 18 bit, conversion time = 100ms (default) */
    LTR390_ALS_UVS_RESOLUTION_17BIT = 0x03,                  /* 17 bit, conversion time = 50ms */
    LTR390_ALS_UVS_RESOLUTION_16BIT = 0x04,                  /* 16 bit, conversion time = 25ms */
    LTR390_ALS_UVS_RESOLUTION_13BIT = 0x05,                  /* 13 bit, conversion time = 12.5ms */
} LTR390_CFG_ALS_UVS_RESOLUTION;

typedef enum {
    LTR390_ALS_UVS_GAIN_RANGE_1 = 0x00,
    LTR390_ALS_UVS_GAIN_RANGE_3 = 0x01,
    LTR390_ALS_UVS_GAIN_RANGE_6 = 0x02,
    LTR390_ALS_UVS_GAIN_RANGE_9 = 0x03,
    LTR390_ALS_UVS_GAIN_RANGE_18 = 0x04,
} LTR390_CFG_ALS_UVS_GAIN_RANGE;

typedef enum {
    LTR390_ALS_UVS_DATA_STATUS_OLD = 0x00,
    LTR390_ALS_UVS_DATA_STATUS_NEW = 0x01,
} LTR390_CFG_ALS_UVS_DATA_STATUS;

typedef enum {
    LTR390_ALS_UVS_INT_STATUS_INACTIVE = 0x00,
    LTR390_ALS_UVS_INT_STATUS_ACTIVE = 0x01,
} LTR390_CFG_ALS_UVS_INT_STATUS;

typedef enum {
    LTR390_ALS_UVS_POWER_ON_STATUS_FALSE = 0x00,
    LTR390_ALS_UVS_POWER_ON_STATUS_TRUE = 0x01,
} LTR390_CFG_ALS_UVS_POWER_ON_STATUS;

typedef enum {
    LTR390_LS_INT_DISABLE = 0x00,
    LTR390_LS_INT_ENABLE = 0x01,
} LTR390_CFG_LS_INT_EN;

typedef enum {
    LTR390_LS_INT_SEL_ALS_CHANNEL = 0x01,
    LTR390_LS_INT_SEL_UVS_CHANNEL = 0x03,
} LTR390_CFG_LS_INT_SEL;

i2c_dev_t ltr390_ctx = {
    .port = 3,
    .config.address_width = 8,
    .config.freq = 100000,
    .config.dev_addr = LTR390_I2C_ADDR,
};

static uint8_t g_init_bitwise = 0;

static int drv_uv_liteon_ltr390_validate_id(i2c_dev_t* drv, uint8_t part_id)
{
    int     ret = 0;
    uint8_t part_id_value = 0;

    if (drv == NULL) {
        return -1;
    }

    ret = sensor_i2c_read(drv, LTR390_PART_ID, &part_id_value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    if (part_id_value != part_id) {
        return -1;
    }

    return 0;
}

static int drv_uv_liteon_ltr390_set_power_mode(i2c_dev_t* drv, dev_power_mode_e mode)
{
    int     ret = 0;
    uint8_t dev_mode = 0;
    uint8_t value = 0;

    ret = sensor_i2c_read(drv, LTR390_MAIN_CTRL, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    switch (mode) {
        case DEV_POWER_OFF:
        case DEV_SLEEP:
            dev_mode = LTR390_SET_BITSLICE(value, MAIN_CTRL_REG_ALS_UVS_EN, LTR390_ALS_UVS_STANDBY);
            break;
        case DEV_POWER_ON:
            dev_mode = LTR390_SET_BITSLICE(value, MAIN_CTRL_REG_ALS_UVS_EN, LTR390_ALS_UVS_ACTIVE);
            break;
        default:
            return -1;
    }

    ret = sensor_i2c_write(drv, LTR390_MAIN_CTRL, &dev_mode, I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    return 0;
}

UNUSED static int drv_uv_liteon_ltr390_is_ready(i2c_dev_t* drv)
{
    int     ret = 0;
    uint8_t value = 0;

    ret = sensor_i2c_read(drv, LTR390_MAIN_STATUS, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return 0;
    }

    ret = (LTR390_GET_BITSLICE(value, MAIN_STATUS_REG_ALS_UVS_DATA_STATUS) == LTR390_ALS_UVS_DATA_STATUS_NEW) ? 1 : 0;

    return ret;
}

static int drv_uv_liteon_ltr390_set_default_config(i2c_dev_t* drv)
{
    int     ret = 0;
    uint8_t value = 0;

    ret = sensor_i2c_read(drv, LTR390_MAIN_CTRL, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }
    value = LTR390_SET_BITSLICE(value, MAIN_CTRL_REG_UVS_MODE, LTR390_UVS_MODE);
    ret = sensor_i2c_write(drv, LTR390_MAIN_CTRL, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    value = 0;
    value = LTR390_SET_BITSLICE(value, ALS_UVS_MEAS_RATE_REG_MEAS_RATE, LTR390_ALS_UVS_MEAS_RATE_100);
    value = LTR390_SET_BITSLICE(value, ALS_UVS_MEAS_RATE_REG_RESOLUTION, LTR390_ALS_UVS_RESOLUTION_16BIT);
    ret = sensor_i2c_write(drv, LTR390_ALS_UVS_MEAS_RATE, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    value = 0;
    value = LTR390_SET_BITSLICE(value, ALS_UVS_GAIN_REG_GAIN_RANGE, LTR390_ALS_UVS_GAIN_RANGE_18);
    ret = sensor_i2c_write(drv, LTR390_ALS_UVS_GAIN, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    value = 0;
    value = LTR390_SET_BITSLICE(value, INT_CFG_REG_LS_INT_EN, LTR390_LS_INT_DISABLE);
    value = LTR390_SET_BITSLICE(value, INT_CFG_REG_LS_INT_SEL, LTR390_LS_INT_SEL_ALS_CHANNEL);
    ret = sensor_i2c_write(drv, LTR390_INT_CFG, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    return 0;
}

static void drv_uv_liteon_ltr390_irq_handle(void)
{
    /* no handle so far */
}

static int drv_uv_liteon_ltr390_open(void)
{
    int ret = 0;

    ret = drv_uv_liteon_ltr390_set_power_mode(&ltr390_ctx, DEV_POWER_ON);
    if (unlikely(ret)) {
        return -1;
    }

    LOG("%s %s successfully \n", SENSOR_STR, __func__);
    return 0;

}

static int drv_uv_liteon_ltr390_close(void)
{
    int ret = 0;

    ret = drv_uv_liteon_ltr390_set_power_mode(&ltr390_ctx, DEV_POWER_OFF);
    if (unlikely(ret)) {
        return -1;
    }

    LOG("%s %s successfully \n", SENSOR_STR, __func__);
    return 0;
}

static int drv_uv_liteon_ltr390_read(void *buf, size_t len)
{
    int ret = 0;
    size_t size;
    uint8_t reg_data[3] = { 0 };
    uv_data_t * pdata = (uv_data_t *) buf;

    if (buf == NULL) {
        return -1;
    }

    size = sizeof(uv_data_t);
    if (len < size) {
        return -1;
    }

    ret = sensor_i2c_read(&ltr390_ctx, LTR390_UVS_DATA_0, &reg_data[0], I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return -1;
    }
    ret = sensor_i2c_read(&ltr390_ctx, LTR390_UVS_DATA_1, &reg_data[1], I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return -1;
    }
    ret = sensor_i2c_read(&ltr390_ctx, LTR390_UVS_DATA_2, &reg_data[2], I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return -1;
    }

    pdata->uvi = ((((uint16_t) (reg_data[1] & 0xFF)) << 8) | reg_data[0]);
    pdata->timestamp = aos_now_ms();

    return (int) size;
}

static int drv_uv_liteon_ltr390_write(const void *buf, size_t len)
{
    (void) buf;
    (void) len;

    return 0;
}

static int drv_uv_liteon_ltr390_ioctl(int cmd, unsigned long arg)
{
    int ret = 0;

    switch (cmd) {
        case SENSOR_IOCTL_SET_POWER: {
            ret = drv_uv_liteon_ltr390_set_power_mode(&ltr390_ctx, arg);
            if (unlikely(ret)) {
                return -1;
            }
        } break;
        case SENSOR_IOCTL_GET_INFO: {
            /* fill the dev info here */
            dev_sensor_info_t *info = (dev_sensor_info_t *) arg;
            info->vendor = DEV_SENSOR_VENDOR_LITEON;
            info->model = "LTR390";
            info->unit = lux;
        } break;
        default:
            return -1;
    }

    LOG("%s %s successfully \n", SENSOR_STR, __func__);
    return 0;
}

int drv_uv_liteon_ltr390_init(void)
{
    int ret = 0;
    sensor_obj_t sensor_uv;
    memset(&sensor_uv, 0, sizeof(sensor_uv));

    if (!g_init_bitwise) {
        ret = drv_uv_liteon_ltr390_validate_id(&ltr390_ctx, LTR390_PART_ID_VAL);
        if (unlikely(ret)) {
            return -1;
        }
    }

    if (!g_init_bitwise) {
        /* fill the sensor_uv obj parameters here */
        sensor_uv.tag = TAG_DEV_UV;
        sensor_uv.path = dev_uv_path;
        sensor_uv.io_port = I2C_PORT;
        sensor_uv.mode = DEV_POLLING;
        sensor_uv.power = DEV_POWER_OFF;
        sensor_uv.open = drv_uv_liteon_ltr390_open;
        sensor_uv.close = drv_uv_liteon_ltr390_close;
        sensor_uv.read = drv_uv_liteon_ltr390_read;
        sensor_uv.write = drv_uv_liteon_ltr390_write;
        sensor_uv.ioctl = drv_uv_liteon_ltr390_ioctl;
        sensor_uv.irq_handle = drv_uv_liteon_ltr390_irq_handle;

        ret = sensor_create_obj(&sensor_uv);
        if (unlikely(ret)) {
            return -1;
        }

        ret = drv_uv_liteon_ltr390_set_default_config(&ltr390_ctx);
        if (unlikely(ret)) {
            return -1;
        }

        g_init_bitwise = 1;
    }

    LOG("%s %s successfully \n", SENSOR_STR, __func__);
    return 0;
}

SENSOR_DRV_ADD(drv_uv_liteon_ltr390_init);

