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


#define BMA280_I2C_ADDR1                    (0x18)
#define BMA280_I2C_ADDR2                    (0x19)
#define BMA280_I2C_ADDR3                    (0x10)
#define BMA280_I2C_ADDR4                    (0x11)
#define BMA280_I2C_ADDR_TRANS(n)            ((n) << 1)
#define BMA280_I2C_ADDR                     BMA280_I2C_ADDR_TRANS(BMA280_I2C_ADDR1)


#define BMA280_INIT_VALUE                   (0)
#define BMA280_GEN_READ_WRITE_LENGTH        (1)
#define BMA280_INTERFACE_IDLE_TIME_DELAY    (1)
#define BMA280_LSB_MSB_READ_LENGTH          (2)
#define BMA280_SHIFT_TWO_BITS               (2)
#define BMA280_SHIFT_FOUR_BITS              (4)
#define BMA280_SHIFT_FIVE_BITS              (5)
#define BMA280_SHIFT_SIX_BITS               (6)
#define BMA280_SHIFT_EIGHT_BITS             (8)
#define BMA280_12_BIT_SHIFT                 (0xF0)

#define BMA280_FIFO_MODE_STATUS_RANGE       (2)
#define BMA280_FIFO_DATA_SELECT_RANGE       (4)
#define BMA280_FIFO_MODE_RANGE              (4)
#define BMA280_FIFO_WML_RANGE               (32)
#define BMA280_FIFO_XYZ_DATA_ENABLED        (0x00)
#define BMA280_FIFO_X_DATA_ENABLED          (0x01)
#define BMA280_FIFO_Y_DATA_ENABLED          (0x02)
#define BMA280_FIFO_Z_DATA_ENABLED          (0x03)
#define BMA280_FIFO_DATA_ENABLED_MASK       (0x03)
#define BMA280_FIFO_XYZ_AXES_FRAME_SIZE     (6)
#define BMA280_FIFO_SINGLE_AXIS_FRAME_SIZE  (2)
#define BMA280_ACCEL_BW_MIN_RANGE           (7)
#define BMA280_ACCEL_BW_1000HZ_RANGE        (15)
#define BMA280_ACCEL_BW_MAX_RANGE           (16)
#define BMA280_SLEEP_DURN_MIN_RANGE         (4)
#define BMA280_SLEEP_TIMER_MODE_RANGE       (2)
#define BMA280_SLEEP_DURN_MAX_RANGE         (16)
#define BMA280_POWER_MODE_RANGE             (6)
#define BMA280_SELF_TEST_AXIS_RANGE         (4)
#define BMA280_SELF_TEST_SIGN_RANGE         (2)

#define BMA280_EEP_OFFSET                   (0x16)
#define BMA280_IMAGE_BASE                   (0x38)
#define BMA280_IMAGE_LEN                    (22)
#define BMA280_CHIP_ID_ADDR                 (0x00)
#define BMA280_CHIP_ID_VALUE                (0xFB)
#define BMA280_X_AXIS_LSB_ADDR              (0x02)
#define BMA280_X_AXIS_MSB_ADDR              (0x03)
#define BMA280_Y_AXIS_LSB_ADDR              (0x04)
#define BMA280_Y_AXIS_MSB_ADDR              (0x05)
#define BMA280_Z_AXIS_LSB_ADDR              (0x06)
#define BMA280_Z_AXIS_MSB_ADDR              (0x07)
#define BMA280_TEMP_ADDR                    (0x08)
#define BMA280_STAT1_ADDR                   (0x09)
#define BMA280_STAT2_ADDR                   (0x0A)
#define BMA280_STAT_TAP_SLOPE_ADDR          (0x0B)
#define BMA280_STAT_ORIENT_HIGH_ADDR        (0x0C)
#define BMA280_STAT_FIFO_ADDR               (0x0E)
#define BMA280_RANGE_SELECT_ADDR            (0x0F)
#define BMA280_BW_SELECT_ADDR               (0x10)
#define BMA280_MODE_CTRL_ADDR               (0x11)
#define BMA280_LOW_NOISE_CTRL_ADDR          (0x12)
#define BMA280_DATA_CTRL_ADDR               (0x13)
#define BMA280_RST_ADDR                     (0x14)
#define BMA280_INTR_ENABLE1_ADDR            (0x16)
#define BMA280_INTR_ENABLE2_ADDR            (0x17)
#define BMA280_INTR_SLOW_NO_MOTION_ADDR     (0x18)
#define BMA280_INTR1_PAD_SELECT_ADDR        (0x19)
#define BMA280_INTR_DATA_SELECT_ADDR        (0x1A)
#define BMA280_INTR2_PAD_SELECT_ADDR        (0x1B)
#define BMA280_INTR_SOURCE_ADDR             (0x1E)
#define BMA280_INTR_SET_ADDR                (0x20)
#define BMA280_INTR_CTRL_ADDR               (0x21)
#define BMA280_LOW_DURN_ADDR                (0x22)
#define BMA280_LOW_THRES_ADDR               (0x23)
#define BMA280_LOW_HIGH_HYST_ADDR           (0x24)
#define BMA280_HIGH_DURN_ADDR               (0x25)
#define BMA280_HIGH_THRES_ADDR              (0x26)
#define BMA280_SLOPE_DURN_ADDR              (0x27)
#define BMA280_SLOPE_THRES_ADDR             (0x28)
#define BMA280_SLOW_NO_MOTION_THRES_ADDR    (0x29)
#define BMA280_TAP_PARAM_ADDR               (0x2A)
#define BMA280_TAP_THRES_ADDR               (0x2B)
#define BMA280_ORIENT_PARAM_ADDR            (0x2C)
#define BMA280_THETA_BLOCK_ADDR             (0x2D)
#define BMA280_THETA_FLAT_ADDR              (0x2E)
#define BMA280_FLAT_HOLD_TIME_ADDR          (0x2F)
#define BMA280_SELFTEST_ADDR                (0x32)
#define BMA280_EEPROM_CTRL_ADDR             (0x33)
#define BMA280_SERIAL_CTRL_ADDR             (0x34)
#define BMA280_OFFSET_CTRL_ADDR             (0x36)
#define BMA280_OFFSET_PARAMS_ADDR           (0x37)
#define BMA280_OFFSET_X_AXIS_ADDR           (0x38)
#define BMA280_OFFSET_Y_AXIS_ADDR           (0x39)
#define BMA280_OFFSET_Z_AXIS_ADDR           (0x3A)
#define BMA280_GP0_ADDR                     (0x3B)
#define BMA280_GP1_ADDR                     (0x3C)
#define BMA280_FIFO_MODE_ADDR               (0x3E)
#define BMA280_FIFO_DATA_OUTPUT_ADDR        (0x3F)
#define BMA280_FIFO_WML_TRIG                (0x30)

#define BMA280_12_RESOLUTION            (0)
#define BMA280_10_RESOLUTION            (1)
#define BMA280_14_RESOLUTION            (2)

#define BMA280_ENABLE_SOFT_RESET_VALUE  (0xB6)
#define BMA280_RANGE_SELECT_POS         (0)
#define BMA280_RANGE_SELECT_LEN         (4)
#define BMA280_RANGE_SELECT_MSK         (0x0F)
#define BMA280_RANGE_SELECT_REG         BMA280_RANGE_SELECT_ADDR

#define BMA280_RANGE_2G     (3)
#define BMA280_RANGE_4G     (5)
#define BMA280_RANGE_8G     (8)
#define BMA280_RANGE_16G    (12)

#define BMA280_BW_15_63     (15)
#define BMA280_BW_31_25     (31)
#define BMA280_BW_62_5      (62)
#define BMA280_BW_125       (125)
#define BMA280_BW_250       (250)
#define BMA280_BW_500       (500)
#define BMA280_BW_1000      (1000)

#define BMA280_BW_7_81HZ    (0x08)
#define BMA280_BW_15_63HZ   (0x09)
#define BMA280_BW_31_25HZ   (0x0A)
#define BMA280_BW_62_50HZ   (0x0B)
#define BMA280_BW_125HZ     (0x0C)
#define BMA280_BW_250HZ     (0x0D)
#define BMA280_BW_500HZ     (0x0E)
#define BMA280_BW_1000HZ    (0x0F)
#define BMA280_BW_BIT_MASK  (~0x0F)

#define BMA280_SLEEP_DURN_0_5MS     (0x05)
#define BMA280_SLEEP_DURN_1MS       (0x06)
#define BMA280_SLEEP_DURN_2MS       (0x07)
#define BMA280_SLEEP_DURN_4MS       (0x08)
#define BMA280_SLEEP_DURN_6MS       (0x09)
#define BMA280_SLEEP_DURN_10MS      (0x0A)
#define BMA280_SLEEP_DURN_25MS      (0x0B)
#define BMA280_SLEEP_DURN_50MS      (0x0C)
#define BMA280_SLEEP_DURN_100MS     (0x0D)
#define BMA280_SLEEP_DURN_500MS     (0x0E)
#define BMA280_SLEEP_DURN_1S        (0x0F)
#define BMA280_SLEEP_DURN_POS       (1)
#define BMA280_SLEEP_DURN_LEN       (4)
#define BMA280_SLEEP_DURN_MSK       (0x1E)
#define BMA280_SLEEP_DURN_REG       BMA280_MODE_CTRL_ADDR
#define BMA280_SLEEP_MODE           (0x40)
#define BMA280_DEEP_SUSPEND_MODE    (0x20)
#define BMA280_SUSPEND_MODE         (0x80)
#define BMA280_NORMAL_MODE          (0x40)

#define BMA280_LOWPOWER_MODE        (0x40)

#define BMA280_MODE_CTRL_POS        (5)
#define BMA280_MODE_CTRL_LEN        (3)
#define BMA280_MODE_CTRL_MSK        (0xE0)
#define BMA280_MODE_CTRL_REG        BMA280_MODE_CTRL_ADDR
#define BMA280_LOW_POWER_MODE_POS   (6)
#define BMA280_LOW_POWER_MODE_LEN   (1)
#define BMA280_LOW_POWER_MODE_MSK   (0x40)
#define BMA280_LOW_POWER_MODE_REG   BMA280_LOW_NOISE_CTRL_ADDR

#define BMA280_DEFAULT_ODR_100HZ    (100)

#define BMA280_FIFO_DEPTH_MAX       (32)
#define BMA280_FIFO_DATA_NUM        (6)
#define BMA280_FIFO_BUFF_LEN        (BMA280_FIFO_DEPTH_MAX * BMA280_FIFO_DATA_NUM)
// bma280 sensitivity factor table, the unit is LSB/g
#define BMA280_IRQ_CLEAR_VAL        (0x80)
#define BMA280_IRQ_LATCHED          (0x0f)
#define BMA280_IRQ_NON_LATCHED      (0)

#define BMA280_IRQ_FIFO_STAT        (0x40)
#define BMA280_IRQ_DATA_READY_STAT  (0x80)

#define BMA280_IRQ_DATA_READY_ENABLE    (0x10)
#define BMA280_IRQ_FIFO_FULL_ENABLE     (0x20)
#define BMA280_IRQ_FIFO_WML_ENABLE      (0x40)

#define BMA280_IRQ_MAP_DATA_INT2        (0x80)
#define BMA280_IRQ_MAP_FIFO_WML_INT2    (0x40)
#define BMA280_IRQ_CONFIG_PUSH_PULL     (0x04)
#define BMA280_IRQ_FIFO_WML_MODE        (0x40)
#define BMA280_IRQ_PIN                  (62)
#define BMA280_FIFO_DEPTH_USD           (20)
#define BMA280_GET_BITSLICE(regvar, bitname) \
    ((regvar & bitname##_MSK) >> bitname##_POS)

#define BMA280_SET_BITSLICE(regvar, bitname, val) \
    ((regvar & ~bitname##_MSK) | ((val << bitname##_POS) & bitname##_MSK))

// bma280 sensitivity factor table, the unit is LSB/g
static uint32_t           bma280_factor[4]       = { 1024, 512, 256, 128 };
static uint32_t           current_factor         = 0;

i2c_dev_t bma280_ctx                             = {
    .port                 = 3,
    .config.dev_addr      = BMA280_I2C_ADDR,
};
static int drv_acc_bosch_bma280_data_ready_init(void);

int drv_acc_bosch_bma280_soft_reset(i2c_dev_t *drv)
{
    int     ret   = 0;
    uint8_t value = BMA280_ENABLE_SOFT_RESET_VALUE;
    ret           = sensor_i2c_write(drv, BMA280_RST_ADDR, &value, I2C_DATA_LEN,
                           I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return -1;
    }
    return 0;
}

int drv_acc_bosch_bma280_validate_id(i2c_dev_t *drv, uint8_t id_value)
{
    uint8_t value = 0x00;
    int     ret   = 0;

    if (drv == NULL) {
        return -1;
    }

    ret = sensor_i2c_read(drv, BMA280_CHIP_ID_ADDR, &value, I2C_DATA_LEN,
                          I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    if (id_value != value) {
        return -1;
    }

    return 0;
}

int drv_acc_bosch_bma280_set_power_mode(i2c_dev_t *      drv,
                                               dev_power_mode_e mode)
{
    uint8_t value = 0x00;
    int     ret = 0;

    ret = sensor_i2c_read(drv, BMA280_MODE_CTRL_REG, &value, I2C_DATA_LEN,
                          I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    switch (mode) {
        case DEV_POWER_ON: {
            if ((value & BMA280_MODE_CTRL_MSK) == BMA280_NORMAL_MODE) {
                return 0;
            }
            value |= BMA280_NORMAL_MODE;
            ret = sensor_i2c_write(drv, BMA280_MODE_CTRL_REG, &value,
                                   I2C_DATA_LEN, I2C_OP_RETRIES);
            if (unlikely(ret)) {
                return ret;
            }
        } break;

        case DEV_POWER_OFF: {
            if ((value & BMA280_MODE_CTRL_MSK) == BMA280_DEEP_SUSPEND_MODE) {
                return 0;
            }

            value |= BMA280_DEEP_SUSPEND_MODE;
            ret = sensor_i2c_write(drv, BMA280_MODE_CTRL_REG, &value,
                                   I2C_DATA_LEN, I2C_OP_RETRIES);
            if (unlikely(ret)) {
                return ret;
            }
        } break;

        case DEV_SLEEP: {
            if ((value & BMA280_MODE_CTRL_MSK) == BMA280_SLEEP_MODE) {
                return 0;
            }

            value |= BMA280_SLEEP_MODE;
            ret = sensor_i2c_write(drv, BMA280_MODE_CTRL_REG, &value,
                                   I2C_DATA_LEN, I2C_OP_RETRIES);
            if (unlikely(ret)) {
                return ret;
            }

        } break;

        case DEV_SUSPEND: {
            if ((value & BMA280_MODE_CTRL_MSK) == BMA280_SUSPEND_MODE) {
                return 0;
            }

            value |= BMA280_SUSPEND_MODE;
            ret = sensor_i2c_write(drv, BMA280_MODE_CTRL_REG, &value,
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

int drv_acc_bosch_bma280_set_odr(i2c_dev_t *drv, uint32_t odr)
{
    int      ret   = 0;
    uint8_t  value = 0x00;
    uint32_t bw    = odr / 2;

    ret = sensor_i2c_read(drv, BMA280_BW_SELECT_ADDR, &value, I2C_DATA_LEN,
                          I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    if (bw >= BMA280_BW_1000) {
        value &= BMA280_BW_BIT_MASK;
        value |= BMA280_BW_1000HZ;
    } else if (bw >= BMA280_BW_500) {
        value &= BMA280_BW_BIT_MASK;
        value |= BMA280_BW_500HZ;
    } else if (bw >= BMA280_BW_250) {
        value &= BMA280_BW_BIT_MASK;
        value |= BMA280_BW_250HZ;
    } else if (bw >= BMA280_BW_125) {
        value &= BMA280_BW_BIT_MASK;
        value |= BMA280_BW_125HZ;
    } else if (bw >= BMA280_BW_62_5) {
        value &= BMA280_BW_BIT_MASK;
        value |= BMA280_BW_62_50HZ;
    } else if (bw >= BMA280_BW_31_25) {
        value &= BMA280_BW_BIT_MASK;
        value |= BMA280_BW_31_25HZ;
    } else {
        value &= BMA280_BW_BIT_MASK;
        value |= BMA280_BW_15_63HZ;
    }

    ret = sensor_i2c_write(drv, BMA280_BW_SELECT_ADDR, &value, I2C_DATA_LEN,
                           I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }
    return 0;
}

int drv_acc_bosch_bma280_set_range(i2c_dev_t *drv, uint32_t range)
{
    int     ret   = 0;
    uint8_t value = 0x00;

    ret = sensor_i2c_read(drv, BMA280_RANGE_SELECT_REG, &value, I2C_DATA_LEN,
                          I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    switch (range) {
        case ACC_RANGE_2G: {
            value =
              BMA280_SET_BITSLICE(value, BMA280_RANGE_SELECT, BMA280_RANGE_2G);
        } break;

        case ACC_RANGE_4G: {
            value =
              BMA280_SET_BITSLICE(value, BMA280_RANGE_SELECT, BMA280_RANGE_4G);
        } break;

        case ACC_RANGE_8G: {
            value =
              BMA280_SET_BITSLICE(value, BMA280_RANGE_SELECT, BMA280_RANGE_8G);
        } break;

        case ACC_RANGE_16G: {
            value =
              BMA280_SET_BITSLICE(value, BMA280_RANGE_SELECT, BMA280_RANGE_16G);
        } break;

        default:
            break;
    }

    /* Write the range register 0x0F*/
    ret = sensor_i2c_write(drv, BMA280_RANGE_SELECT_REG, &value, I2C_DATA_LEN,
                           I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    if (range <= ACC_RANGE_16G) {
        current_factor = bma280_factor[range];
    }

    return 0;
}


int drv_acc_bosch_bma280_open(void)
{
    int ret = 0;
    ret = drv_acc_bosch_bma280_data_ready_init();
    if (unlikely(ret)) {
        return -1;
    }

    ret = drv_acc_bosch_bma280_set_power_mode(&bma280_ctx, DEV_POWER_ON);
    if (unlikely(ret)) {
        return -1;
    }
    return 0;
}

int drv_acc_bosch_bma280_close(void)
{
    int ret = 0;
    ret     = drv_acc_bosch_bma280_set_power_mode(&bma280_ctx, DEV_POWER_OFF);
    if (unlikely(ret)) {
        return -1;
    }
    return 0;
}

static int drv_acc_bosch_bma280_data_ready_init(void)
{
    int     ret;
    uint8_t value;
    value = BMA280_IRQ_DATA_READY_ENABLE;
    ret   = sensor_i2c_write(&bma280_ctx, BMA280_INTR_ENABLE2_ADDR, &value,
                           I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    value = BMA280_IRQ_MAP_DATA_INT2;
    ret   = sensor_i2c_write(&bma280_ctx, BMA280_INTR_DATA_SELECT_ADDR, &value,
                           I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    value = BMA280_IRQ_CONFIG_PUSH_PULL;
    ret   = sensor_i2c_write(&bma280_ctx, BMA280_INTR_SET_ADDR, &value,
                           I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }


    value = BMA280_IRQ_CLEAR_VAL | BMA280_IRQ_NON_LATCHED;
    ret   = sensor_i2c_write(&bma280_ctx, BMA280_INTR_CTRL_ADDR, &value,
                           I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }
    return 0;
}

int drv_acc_bosch_bma280_read(void *buf, size_t len)
{
    int           ret = 0;
    size_t        size;
    uint8_t       reg[6];
    accel_data_t *accel = (accel_data_t *)buf;
    if (buf == NULL) {
        return -1;
    }

    size = sizeof(accel_data_t);
    if (len < size) {
        return -1;
    }

    ret = sensor_i2c_read(&bma280_ctx, BMA280_X_AXIS_LSB_ADDR, &reg[0],
                          I2C_REG_LEN, I2C_OP_RETRIES);
    ret |= sensor_i2c_read(&bma280_ctx, BMA280_X_AXIS_MSB_ADDR, &reg[1],
                           I2C_REG_LEN, I2C_OP_RETRIES);
    ret |= sensor_i2c_read(&bma280_ctx, BMA280_Y_AXIS_LSB_ADDR, &reg[2],
                           I2C_REG_LEN, I2C_OP_RETRIES);
    ret |= sensor_i2c_read(&bma280_ctx, BMA280_Y_AXIS_MSB_ADDR, &reg[3],
                           I2C_REG_LEN, I2C_OP_RETRIES);
    ret |= sensor_i2c_read(&bma280_ctx, BMA280_Z_AXIS_LSB_ADDR, &reg[4],
                           I2C_REG_LEN, I2C_OP_RETRIES);
    ret |= sensor_i2c_read(&bma280_ctx, BMA280_Z_AXIS_MSB_ADDR, &reg[5],
                           I2C_REG_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return -1;
    }

    accel->data[DATA_AXIS_X] =
      (int32_t)((((int32_t)((int8_t)reg[1])) << BMA280_SHIFT_EIGHT_BITS) |
                (reg[0] & BMA280_12_BIT_SHIFT));
    accel->data[DATA_AXIS_X] =
      accel->data[DATA_AXIS_X] >> BMA280_SHIFT_FOUR_BITS;

    accel->data[DATA_AXIS_Y] =
      (int32_t)((((int32_t)((int8_t)reg[3])) << BMA280_SHIFT_EIGHT_BITS) |
                (reg[2] & BMA280_12_BIT_SHIFT));
    accel->data[DATA_AXIS_Y] =
      accel->data[DATA_AXIS_Y] >> BMA280_SHIFT_FOUR_BITS;

    accel->data[DATA_AXIS_Z] =
      (int32_t)((((int32_t)((int8_t)reg[5])) << BMA280_SHIFT_EIGHT_BITS) |
                (reg[4] & BMA280_12_BIT_SHIFT));
    accel->data[DATA_AXIS_Z] =
      accel->data[DATA_AXIS_Z] >> BMA280_SHIFT_FOUR_BITS;

    if (current_factor != 0) {
        // the unit of acc is mg, 1000 mg = 1 g.
        accel->data[DATA_AXIS_X] = accel->data[DATA_AXIS_X] *
                                   ACCELEROMETER_UNIT_FACTOR /
                                   (int32_t)current_factor;
        accel->data[DATA_AXIS_Y] = accel->data[DATA_AXIS_Y] *
                                   ACCELEROMETER_UNIT_FACTOR /
                                   (int32_t)current_factor;
        accel->data[DATA_AXIS_Z] = accel->data[DATA_AXIS_Z] *
                                   ACCELEROMETER_UNIT_FACTOR /
                                   (int32_t)current_factor;
    }
    accel->timestamp = aos_now_ms();

    return (int)size;

}

void drv_acc_bosch_bma280_irq_handle(void)
{

}

static int drv_acc_bosch_bma280_ioctl(int cmd, unsigned long arg)
{
    int ret = 0;

    switch (cmd) {
        case SENSOR_IOCTL_ODR_SET: {
            ret = drv_acc_bosch_bma280_set_odr(&bma280_ctx, arg);
            if (unlikely(ret)) {
                return -1;
            }
        } break;
        case SENSOR_IOCTL_RANGE_SET: {
            ret = drv_acc_bosch_bma280_set_range(&bma280_ctx, arg);
            if (unlikely(ret)) {
                return -1;
            }
        } break;
        case SENSOR_IOCTL_SET_POWER: {
            ret = drv_acc_bosch_bma280_set_power_mode(&bma280_ctx, arg);
            if (unlikely(ret)) {
                return -1;
            }
        } break;
        case SENSOR_IOCTL_GET_INFO: {
            /* fill the dev info here */
            dev_sensor_info_t *info = (dev_sensor_info_t *)arg;
            info->model             = "BMA280";
            info->range_max         = 16;
            info->range_min         = 2;
            info->unit              = mg;
        } break;

        default:
            break;
    }

    return 0;
}

int drv_acc_bosch_bma280_init(void)
{
    int          ret = 0;
    sensor_obj_t sensor;

    memset(&sensor, 0, sizeof(sensor));

    /* fill the sensor obj parameters here */
    sensor.io_port     = I2C_PORT;
    sensor.tag         = TAG_DEV_ACC;
    sensor.path        = dev_acc_path;
    sensor.open        = drv_acc_bosch_bma280_open;
    sensor.close       = drv_acc_bosch_bma280_close;
    sensor.read        = drv_acc_bosch_bma280_read;
    sensor.write       = NULL;
    sensor.ioctl       = drv_acc_bosch_bma280_ioctl;
    sensor.irq_handle  = drv_acc_bosch_bma280_irq_handle;
    sensor.mode        = DEV_POLLING;
    sensor.data_len = sizeof(accel_data_t);

    ret = sensor_create_obj(&sensor);
    if (unlikely(ret)) {
        return -1;
    }

    ret = drv_acc_bosch_bma280_validate_id(&bma280_ctx, BMA280_CHIP_ID_VALUE);
    if (unlikely(ret)) {
        return -1;
    }

    ret = drv_acc_bosch_bma280_soft_reset(&bma280_ctx);
    if (unlikely(ret)) {
        return -1;
    }

    aos_msleep(5);
    ret = drv_acc_bosch_bma280_set_range(&bma280_ctx, ACC_RANGE_8G);
    if (unlikely(ret)) {
        return -1;
    }

    // set odr is 100hz, and will update
    ret = drv_acc_bosch_bma280_set_odr(&bma280_ctx, BMA280_DEFAULT_ODR_100HZ);
    if (unlikely(ret)) {
        return -1;
    }
    ret = drv_acc_bosch_bma280_set_power_mode(&bma280_ctx, DEV_SLEEP);
    if (unlikely(ret)) {
        return -1;
    }

    /* update the phy sensor info to sensor hal */
    LOG("%s %s successfully \n", SENSOR_STR, __func__);
    return 0;
}

SENSOR_DRV_ADD(drv_acc_bosch_bma280_init);