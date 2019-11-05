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


#define BMI055_ACC_I2C_ADDR1                    (0x18)
#define BMI055_ACC_I2C_ADDR2                    (0x19)
#define BMI055_ACC_I2C_ADDR3                    (0x10)
#define BMI055_ACC_I2C_ADDR4                    (0x11)
#define BMI055_ACC_I2C_ADDR_TRANS(n)            ((n) << 1)
#define BMI055_ACC_I2C_ADDR                     BMI055_ACC_I2C_ADDR_TRANS(BMI055_ACC_I2C_ADDR1)


#define BMI055_ACC_INIT_VALUE                   (0)
#define BMI055_ACC_GEN_READ_WRITE_LENGTH        (1)
#define BMI055_ACC_INTERFACE_IDLE_TIME_DELAY    (1)
#define BMI055_ACC_LSB_MSB_READ_LENGTH          (2)
#define BMI055_ACC_SHIFT_TWO_BITS               (2)
#define BMI055_ACC_SHIFT_FOUR_BITS              (4)
#define BMI055_ACC_SHIFT_FIVE_BITS              (5)
#define BMI055_ACC_SHIFT_SIX_BITS               (6)
#define BMI055_ACC_SHIFT_EIGHT_BITS             (8)
#define BMI055_ACC_12_BIT_SHIFT                 (0xF0)

#define BMI055_ACC_FIFO_MODE_STATUS_RANGE       (2)
#define BMI055_ACC_FIFO_DATA_SELECT_RANGE       (4)
#define BMI055_ACC_FIFO_MODE_RANGE              (4)
#define BMI055_ACC_FIFO_WML_RANGE               (32)
#define BMI055_ACC_FIFO_XYZ_DATA_ENABLED        (0x00)
#define BMI055_ACC_FIFO_X_DATA_ENABLED          (0x01)
#define BMI055_ACC_FIFO_Y_DATA_ENABLED          (0x02)
#define BMI055_ACC_FIFO_Z_DATA_ENABLED          (0x03)
#define BMI055_ACC_FIFO_DATA_ENABLED_MASK       (0x03)
#define BMI055_ACC_FIFO_XYZ_AXES_FRAME_SIZE     (6)
#define BMI055_ACC_FIFO_SINGLE_AXIS_FRAME_SIZE  (2)
#define BMI055_ACC_ACCEL_BW_MIN_RANGE           (7)
#define BMI055_ACC_ACCEL_BW_1000HZ_RANGE        (15)
#define BMI055_ACC_ACCEL_BW_MAX_RANGE           (16)
#define BMI055_ACC_SLEEP_DURN_MIN_RANGE         (4)
#define BMI055_ACC_SLEEP_TIMER_MODE_RANGE       (2)
#define BMI055_ACC_SLEEP_DURN_MAX_RANGE         (16)
#define BMI055_ACC_POWER_MODE_RANGE             (6)
#define BMI055_ACC_SELF_TEST_AXIS_RANGE         (4)
#define BMI055_ACC_SELF_TEST_SIGN_RANGE         (2)

#define BMI055_ACC_EEP_OFFSET                   (0x16)
#define BMI055_ACC_IMAGE_BASE                   (0x38)
#define BMI055_ACC_IMAGE_LEN                    (22)
#define BMI055_ACC_CHIP_ID_ADDR                 (0x00)
#define BMI055_ACC_CHIP_ID_VALUE                (0xFA)
#define BMI055_ACC_X_AXIS_LSB_ADDR              (0x02)
#define BMI055_ACC_X_AXIS_MSB_ADDR              (0x03)
#define BMI055_ACC_Y_AXIS_LSB_ADDR              (0x04)
#define BMI055_ACC_Y_AXIS_MSB_ADDR              (0x05)
#define BMI055_ACC_Z_AXIS_LSB_ADDR              (0x06)
#define BMI055_ACC_Z_AXIS_MSB_ADDR              (0x07)
#define BMI055_ACC_TEMP_ADDR                    (0x08)
#define BMI055_ACC_STAT1_ADDR                   (0x09)
#define BMI055_ACC_STAT2_ADDR                   (0x0A)
#define BMI055_ACC_STAT_TAP_SLOPE_ADDR          (0x0B)
#define BMI055_ACC_STAT_ORIENT_HIGH_ADDR        (0x0C)
#define BMI055_ACC_STAT_FIFO_ADDR               (0x0E)
#define BMI055_ACC_RANGE_SELECT_ADDR            (0x0F)
#define BMI055_ACC_BW_SELECT_ADDR               (0x10)
#define BMI055_ACC_MODE_CTRL_ADDR               (0x11)
#define BMI055_ACC_LOW_NOISE_CTRL_ADDR          (0x12)
#define BMI055_ACC_DATA_CTRL_ADDR               (0x13)
#define BMI055_ACC_RST_ADDR                     (0x14)
#define BMI055_ACC_INTR_ENABLE1_ADDR            (0x16)
#define BMI055_ACC_INTR_ENABLE2_ADDR            (0x17)
#define BMI055_ACC_INTR_SLOW_NO_MOTION_ADDR     (0x18)
#define BMI055_ACC_INTR1_PAD_SELECT_ADDR        (0x19)
#define BMI055_ACC_INTR_DATA_SELECT_ADDR        (0x1A)
#define BMI055_ACC_INTR2_PAD_SELECT_ADDR        (0x1B)
#define BMI055_ACC_INTR_SOURCE_ADDR             (0x1E)
#define BMI055_ACC_INTR_SET_ADDR                (0x20)
#define BMI055_ACC_INTR_CTRL_ADDR               (0x21)
#define BMI055_ACC_LOW_DURN_ADDR                (0x22)
#define BMI055_ACC_LOW_THRES_ADDR               (0x23)
#define BMI055_ACC_LOW_HIGH_HYST_ADDR           (0x24)
#define BMI055_ACC_HIGH_DURN_ADDR               (0x25)
#define BMI055_ACC_HIGH_THRES_ADDR              (0x26)
#define BMI055_ACC_SLOPE_DURN_ADDR              (0x27)
#define BMI055_ACC_SLOPE_THRES_ADDR             (0x28)
#define BMI055_ACC_SLOW_NO_MOTION_THRES_ADDR    (0x29)
#define BMI055_ACC_TAP_PARAM_ADDR               (0x2A)
#define BMI055_ACC_TAP_THRES_ADDR               (0x2B)
#define BMI055_ACC_ORIENT_PARAM_ADDR            (0x2C)
#define BMI055_ACC_THETA_BLOCK_ADDR             (0x2D)
#define BMI055_ACC_THETA_FLAT_ADDR              (0x2E)
#define BMI055_ACC_FLAT_HOLD_TIME_ADDR          (0x2F)
#define BMI055_ACC_SELFTEST_ADDR                (0x32)
#define BMI055_ACC_EEPROM_CTRL_ADDR             (0x33)
#define BMI055_ACC_SERIAL_CTRL_ADDR             (0x34)
#define BMI055_ACC_OFFSET_CTRL_ADDR             (0x36)
#define BMI055_ACC_OFFSET_PARAMS_ADDR           (0x37)
#define BMI055_ACC_OFFSET_X_AXIS_ADDR           (0x38)
#define BMI055_ACC_OFFSET_Y_AXIS_ADDR           (0x39)
#define BMI055_ACC_OFFSET_Z_AXIS_ADDR           (0x3A)
#define BMI055_ACC_GP0_ADDR                     (0x3B)
#define BMI055_ACC_GP1_ADDR                     (0x3C)
#define BMI055_ACC_FIFO_MODE_ADDR               (0x3E)
#define BMI055_ACC_FIFO_DATA_OUTPUT_ADDR        (0x3F)
#define BMI055_ACC_FIFO_WML_TRIG                (0x30)

#define BMI055_ACC_12_RESOLUTION            (0)
#define BMI055_ACC_10_RESOLUTION            (1)
#define BMI055_ACC_14_RESOLUTION            (2)

#define BMI055_ACC_ENABLE_SOFT_RESET_VALUE  (0xB6)
#define BMI055_ACC_RANGE_SELECT_POS         (0)
#define BMI055_ACC_RANGE_SELECT_LEN         (4)
#define BMI055_ACC_RANGE_SELECT_MSK         (0x0F)
#define BMI055_ACC_RANGE_SELECT_REG         BMI055_ACC_RANGE_SELECT_ADDR

#define BMI055_ACC_RANGE_2G     (3)
#define BMI055_ACC_RANGE_4G     (5)
#define BMI055_ACC_RANGE_8G     (8)
#define BMI055_ACC_RANGE_16G    (12)

#define BMI055_ACC_BW_15_63     (15)
#define BMI055_ACC_BW_31_25     (31)
#define BMI055_ACC_BW_62_5      (62)
#define BMI055_ACC_BW_125       (125)
#define BMI055_ACC_BW_250       (250)
#define BMI055_ACC_BW_500       (500)
#define BMI055_ACC_BW_1000      (1000)

#define BMI055_ACC_BW_7_81HZ    (0x08)
#define BMI055_ACC_BW_15_63HZ   (0x09)
#define BMI055_ACC_BW_31_25HZ   (0x0A)
#define BMI055_ACC_BW_62_50HZ   (0x0B)
#define BMI055_ACC_BW_125HZ     (0x0C)
#define BMI055_ACC_BW_250HZ     (0x0D)
#define BMI055_ACC_BW_500HZ     (0x0E)
#define BMI055_ACC_BW_1000HZ    (0x0F)
#define BMI055_ACC_BW_BIT_MASK  (~0x0F)

#define BMI055_ACC_SLEEP_DURN_0_5MS     (0x05)
#define BMI055_ACC_SLEEP_DURN_1MS       (0x06)
#define BMI055_ACC_SLEEP_DURN_2MS       (0x07)
#define BMI055_ACC_SLEEP_DURN_4MS       (0x08)
#define BMI055_ACC_SLEEP_DURN_6MS       (0x09)
#define BMI055_ACC_SLEEP_DURN_10MS      (0x0A)
#define BMI055_ACC_SLEEP_DURN_25MS      (0x0B)
#define BMI055_ACC_SLEEP_DURN_50MS      (0x0C)
#define BMI055_ACC_SLEEP_DURN_100MS     (0x0D)
#define BMI055_ACC_SLEEP_DURN_500MS     (0x0E)
#define BMI055_ACC_SLEEP_DURN_1S        (0x0F)
#define BMI055_ACC_SLEEP_DURN_POS       (1)
#define BMI055_ACC_SLEEP_DURN_LEN       (4)
#define BMI055_ACC_SLEEP_DURN_MSK       (0x1E)
#define BMI055_ACC_SLEEP_DURN_REG       BMI055_ACC_MODE_CTRL_ADDR
#define BMI055_ACC_SLEEP_MODE           (0x40)
#define BMI055_ACC_DEEP_SUSPEND_MODE    (0x20)
#define BMI055_ACC_SUSPEND_MODE         (0x80)
#define BMI055_ACC_NORMAL_MODE          (0x40)

#define BMI055_ACC_LOWPOWER_MODE        (0x40)

#define BMI055_ACC_MODE_CTRL_POS        (5)
#define BMI055_ACC_MODE_CTRL_LEN        (3)
#define BMI055_ACC_MODE_CTRL_MSK        (0xE0)
#define BMI055_ACC_MODE_CTRL_REG        BMI055_ACC_MODE_CTRL_ADDR
#define BMI055_ACC_LOW_POWER_MODE_POS   (6)
#define BMI055_ACC_LOW_POWER_MODE_LEN   (1)
#define BMI055_ACC_LOW_POWER_MODE_MSK   (0x40)
#define BMI055_ACC_LOW_POWER_MODE_REG   BMI055_ACC_LOW_NOISE_CTRL_ADDR

#define BMI055_ACC_DEFAULT_ODR_100HZ    (100)

#define BMI055_ACC_FIFO_DEPTH_MAX       (32)
#define BMI055_ACC_FIFO_DATA_NUM        (6)
#define BMI055_ACC_FIFO_BUFF_LEN        (BMI055_ACC_FIFO_DEPTH_MAX * BMI055_ACC_FIFO_DATA_NUM)
// bmi055 sensitivity factor table, the unit is LSB/g
#define BMI055_ACC_IRQ_CLEAR_VAL        (0x80)
#define BMI055_ACC_IRQ_LATCHED          (0x0f)
#define BMI055_ACC_IRQ_NON_LATCHED      (0)

#define BMI055_ACC_IRQ_FIFO_STAT        (0x40)
#define BMI055_ACC_IRQ_DATA_READY_STAT  (0x80)

#define BMI055_ACC_IRQ_DATA_READY_ENABLE    (0x10)
#define BMI055_ACC_IRQ_FIFO_FULL_ENABLE     (0x20)
#define BMI055_ACC_IRQ_FIFO_WML_ENABLE      (0x40)

#define BMI055_ACC_IRQ_MAP_DATA_INT2        (0x80)
#define BMI055_ACC_IRQ_MAP_FIFO_WML_INT2    (0x40)
#define BMI055_ACC_IRQ_CONFIG_PUSH_PULL     (0x04)
#define BMI055_ACC_IRQ_FIFO_WML_MODE        (0x40)
#define BMI055_ACC_IRQ_PIN                  (62)
#define BMI055_ACC_FIFO_DEPTH_USD           (20)
#define BMI055_ACC_DEFAULT_ODR              (100)


#define BMI055_ACC_GET_BITSLICE(regvar, bitname) \
    ((regvar & bitname##_MSK) >> bitname##_POS)

#define BMI055_ACC_SET_BITSLICE(regvar, bitname, val) \
    ((regvar & ~bitname##_MSK) | ((val << bitname##_POS) & bitname##_MSK))



#define BMI055_GYRO_DEFAULT_ODR             (200)

#define BMI055_GYRO_I2C_ADDR1               (0x68 << 1)
#define BMI055_GYRO_I2C_ADDR2               (0x69 << 1)

#define BMI055_GYRO_CHIP_ID_ADDR            (0x00)
#define BMI055_GYRO_CHIP_ID_VALUE           (0x0F)

#define BMI055_GYRO_RATE_X_LSB_ADDR         (0x02)
#define BMI055_GYRO_RATE_X_MSB_ADDR         (0x03)
#define BMI055_GYRO_RATE_Y_LSB_ADDR         (0x04)
#define BMI055_GYRO_RATE_Y_MSB_ADDR         (0x05)
#define BMI055_GYRO_RATE_Z_LSB_ADDR         (0x06)
#define BMI055_GYRO_RATE_Z_MSB_ADDR         (0x07)
#define BMI055_GYRO_TEMP_ADDR               (0x08)
#define BMI055_GYRO_INTR_STAT0_ADDR         (0x09)
#define BMI055_GYRO_INTR_STAT1_ADDR         (0x0A)
#define BMI055_GYRO_INTR_STAT2_ADDR         (0x0B)
#define BMI055_GYRO_INTR_STAT3_ADDR         (0x0C)
#define BMI055_GYRO_FIFO_STAT_ADDR          (0x0E)
#define BMI055_GYRO_RANGE_ADDR              (0x0F)
#define BMI055_GYRO_BW_ADDR                 (0x10)
#define BMI055_GYRO_MODE_LPM1_ADDR          (0x11)
#define BMI055_GYRO_MODE_LPM2_ADDR          (0x12)
#define BMI055_GYRO_HIGH_BW_ADDR            (0x13)
#define BMI055_GYRO_BGW_SOFT_RST_ADDR       (0x14)
#define BMI055_GYRO_INTR_ENABLE0_ADDR       (0x15)
#define BMI055_GYRO_INTR_ENABLE1_ADDR       (0x16)
#define BMI055_GYRO_INTR_MAP_ZERO_ADDR      (0x17)
#define BMI055_GYRO_INTR_MAP_ONE_ADDR       (0x18)
#define BMI055_GYRO_INTR_MAP_TWO_ADDR       (0x19)
#define BMI055_GYRO_INTR_ZERO_ADDR          (0x1A)
#define BMI055_GYRO_INTR_ONE_ADDR           (0x1B)
#define BMI055_GYRO_INTR_TWO_ADDR           (0x1C)
#define BMI055_GYRO_INTR_4_ADDR             (0x1E)
#define BMI055_GYRO_RST_LATCH_ADDR          (0x21)
#define BMI055_GYRO_HIGHRATE_THRES_X_ADDR   (0x22)
#define BMI055_GYRO_HIGHRATE_DURN_X_ADDR    (0x23)
#define BMI055_GYRO_HIGHRATE_THRES_Y_ADDR   (0x24)
#define BMI055_GYRO_HIGHRATE_DURN_Y_ADDR    (0x25)
#define BMI055_GYRO_HIGHRATE_THRES_Z_ADDR   (0x26)
#define BMI055_GYRO_HIGHRATE_DURN_Z_ADDR    (0x27)
#define BMI055_GYRO_SOC_ADDR                (0x31)
#define BMI055_GYRO_A_FOC_ADDR              (0x32)
#define BMI055_GYRO_TRIM_NVM_CTRL_ADDR      (0x33)
#define BMI055_GYRO_BGW_SPI3_WDT_ADDR       (0x34)

#define BMI055_GYRO_OFFSET_OFC1_ADDR        (0x36)
#define BMI055_GYRO_OFC2_ADDR               (0x37)
#define BMI055_GYRO_OFC3_ADDR               (0x38)
#define BMI055_GYRO_OFC4_ADDR               (0x39)
#define BMI055_GYRO_TRIM_GP0_ADDR           (0x3A)
#define BMI055_GYRO_TRIM_GP1_ADDR           (0x3B)
#define BMI055_GYRO_SELFTEST_ADDR           (0x3C)
#define BMI055_GYRO_FIFO_CGF1_ADDR          (0x3D)
#define BMI055_GYRO_FIFO_CGF0_ADDR          (0x3E)

#define BMI055_GYRO_FIFO_DATA_ADDR          (0x3F)


#define BMI055_GYRO_RANGE_RANGE_2000DPS     (0x00)
#define BMI055_GYRO_RANGE_RANGE_1000DPS     (0x01)
#define BMI055_GYRO_RANGE_RANGE_500DPS      (0x02)
#define BMI055_GYRO_RANGE_RANGE_250DPS      (0x03)
#define BMI055_GYRO_RANGE_RANGE_125DPS      (0x04)
#define BMI055_GYRO_RANGE_ADDR_RANGE_POS    (0)
#define BMI055_GYRO_RANGE_ADDR_RANGE_LEN    (3)
#define BMI055_GYRO_RANGE_ADDR_RANGE_MSK    (0x07)
#define BMI055_GYRO_RANGE_ADDR_RANGE_REG    (BMI055_GYRO_RANGE_ADDR)


#define BMI055_GYRO_BW_ADDR_POS             (0)
#define BMI055_GYRO_BW_ADDR_LEN             (3)
#define BMI055_GYRO_BW_ADDR_MSK             (0x07)
#define BMI055_GYRO_BW_ADDR_REG             (BMI055_GYRO_BW_ADDR)

#define BMI055_GYRO_MODE_LPM1_POS           (5)
#define BMI055_GYRO_MODE_LPM1_LEN           (3)
#define BMI055_GYRO_MODE_LPM1_MSK           (0xA0)
#define BMI055_GYRO_MODE_LPM1_REG           (BMI055_GYRO_MODE_LPM1_ADDR)


#define BMI055_GYRO_ODR_100                 (100)
#define BMI055_GYRO_ODR_200                 (200)
#define BMI055_GYRO_ODR_400                 (400)
#define BMI055_GYRO_ODR_1000                (1000)
#define BMI055_GYRO_ODR_2000                (2000)

#define BMI055_GYRO_ODR_100_REG             (0x07)
#define BMI055_GYRO_ODR_200_REG             (0x06)
#define BMI055_GYRO_ODR_400_REG             (0x03)
#define BMI055_GYRO_ODR_1000_REG            (0x02)
#define BMI055_GYRO_ODR_2000_REG            (0x01)

#define BMI055_GYRO_POWER_BIT_MASK          (0xA0)
#define BMI055_GYRO_POWER_NORMAL_REG        (0x00)
#define BMI055_GYRO_POWER_SLEEP_REG         (0x80)
#define BMI055_GYRO_POWER_DEEP_SUSPEND_REG  (0x20)

#define BMI055_GYRO_SHIFT_EIGHT_BITS        (8)
#define BMI055_GYRO_DEFAULT_ODR             (200)

static uint32_t bmi055_acc_factor[4]       = { 1024, 512, 256, 128 };
static uint32_t acc_current_factor         = 0;

static uint32_t bmi055_gyro_factor[5] = { 2624, 1312, 656, 328,
                                     164 }; 
static uint32_t gyro_current_factor   = 0;

i2c_dev_t bmi055_acc_ctx                             = {
    .port                 = 3,
    .config.dev_addr      = BMI055_ACC_I2C_ADDR,
};

i2c_dev_t bmi055_gyro_ctx = {
    .port                 = 3,
    .config.dev_addr      = BMI055_GYRO_I2C_ADDR1,
};

int drv_acc_bosch_bmi055_soft_reset(i2c_dev_t *drv)
{
    int     ret   = 0;
    uint8_t value = BMI055_ACC_ENABLE_SOFT_RESET_VALUE;
    ret           = sensor_i2c_write(drv, BMI055_ACC_RST_ADDR, &value, I2C_DATA_LEN,
                           I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return -1;
    }
    return 0;
}

int drv_acc_bosch_bmi055_validate_id(i2c_dev_t *drv, uint8_t id_value)
{
    uint8_t value = 0x00;
    int     ret   = 0;

    if (drv == NULL) {
        return -1;
    }

    ret = sensor_i2c_read(drv, BMI055_ACC_CHIP_ID_ADDR, &value, I2C_DATA_LEN,
                          I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    if (id_value != value) {
        return -1;
    }

    return 0;
}

int drv_acc_bosch_bmi055_set_power_mode(i2c_dev_t *      drv,
                                               dev_power_mode_e mode)
{
    uint8_t value = 0x00;
    int     ret = 0;

    ret = sensor_i2c_read(drv, BMI055_ACC_MODE_CTRL_REG, &value, I2C_DATA_LEN,
                          I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    switch (mode) {
        case DEV_POWER_ON: {
            if ((value & BMI055_ACC_MODE_CTRL_MSK) == BMI055_ACC_NORMAL_MODE) {
                return 0;
            }
            value |= BMI055_ACC_NORMAL_MODE;
            ret = sensor_i2c_write(drv, BMI055_ACC_MODE_CTRL_REG, &value,
                                   I2C_DATA_LEN, I2C_OP_RETRIES);
            if (unlikely(ret)) {
                return ret;
            }
        } break;

        case DEV_POWER_OFF: {
            if ((value & BMI055_ACC_MODE_CTRL_MSK) == BMI055_ACC_DEEP_SUSPEND_MODE) {
                return 0;
            }

            value |= BMI055_ACC_DEEP_SUSPEND_MODE;
            ret = sensor_i2c_write(drv, BMI055_ACC_MODE_CTRL_REG, &value,
                                   I2C_DATA_LEN, I2C_OP_RETRIES);
            if (unlikely(ret)) {
                return ret;
            }
        } break;

        case DEV_SLEEP: {
            if ((value & BMI055_ACC_MODE_CTRL_MSK) == BMI055_ACC_SLEEP_MODE) {
                return 0;
            }

            value |= BMI055_ACC_SLEEP_MODE;
            ret = sensor_i2c_write(drv, BMI055_ACC_MODE_CTRL_REG, &value,
                                   I2C_DATA_LEN, I2C_OP_RETRIES);
            if (unlikely(ret)) {
                return ret;
            }

        } break;

        case DEV_SUSPEND: {
            if ((value & BMI055_ACC_MODE_CTRL_MSK) == BMI055_ACC_SUSPEND_MODE) {
                return 0;
            }

            value |= BMI055_ACC_SUSPEND_MODE;
            ret = sensor_i2c_write(drv, BMI055_ACC_MODE_CTRL_REG, &value,
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

int drv_acc_bosch_bmi055_set_odr(i2c_dev_t *drv, uint32_t odr)
{
    int      ret   = 0;
    uint8_t  value = 0x00;
    uint32_t bw    = odr / 2;

    ret = sensor_i2c_read(drv, BMI055_ACC_BW_SELECT_ADDR, &value, I2C_DATA_LEN,
                          I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    if (bw >= BMI055_ACC_BW_1000) {
        value &= BMI055_ACC_BW_BIT_MASK;
        value |= BMI055_ACC_BW_1000HZ;
    } else if (bw >= BMI055_ACC_BW_500) {
        value &= BMI055_ACC_BW_BIT_MASK;
        value |= BMI055_ACC_BW_500HZ;
    } else if (bw >= BMI055_ACC_BW_250) {
        value &= BMI055_ACC_BW_BIT_MASK;
        value |= BMI055_ACC_BW_250HZ;
    } else if (bw >= BMI055_ACC_BW_125) {
        value &= BMI055_ACC_BW_BIT_MASK;
        value |= BMI055_ACC_BW_125HZ;
    } else if (bw >= BMI055_ACC_BW_62_5) {
        value &= BMI055_ACC_BW_BIT_MASK;
        value |= BMI055_ACC_BW_62_50HZ;
    } else if (bw >= BMI055_ACC_BW_31_25) {
        value &= BMI055_ACC_BW_BIT_MASK;
        value |= BMI055_ACC_BW_31_25HZ;
    } else {
        value &= BMI055_ACC_BW_BIT_MASK;
        value |= BMI055_ACC_BW_15_63HZ;
    }

    ret = sensor_i2c_write(drv, BMI055_ACC_BW_SELECT_ADDR, &value, I2C_DATA_LEN,
                           I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }
    return 0;
}

int drv_acc_bosch_bmi055_set_range(i2c_dev_t *drv, uint32_t range)
{
    int     ret   = 0;
    uint8_t value = 0x00;

    ret = sensor_i2c_read(drv, BMI055_ACC_RANGE_SELECT_REG, &value, I2C_DATA_LEN,
                          I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    switch (range) {
        case ACC_RANGE_2G: {
            value =
              BMI055_ACC_SET_BITSLICE(value, BMI055_ACC_RANGE_SELECT, BMI055_ACC_RANGE_2G);
        } break;

        case ACC_RANGE_4G: {
            value =
              BMI055_ACC_SET_BITSLICE(value, BMI055_ACC_RANGE_SELECT, BMI055_ACC_RANGE_4G);
        } break;

        case ACC_RANGE_8G: {
            value =
              BMI055_ACC_SET_BITSLICE(value, BMI055_ACC_RANGE_SELECT, BMI055_ACC_RANGE_8G);
        } break;

        case ACC_RANGE_16G: {
            value =
              BMI055_ACC_SET_BITSLICE(value, BMI055_ACC_RANGE_SELECT, BMI055_ACC_RANGE_16G);
        } break;

        default:
            break;
    }

    ret = sensor_i2c_write(drv, BMI055_ACC_RANGE_SELECT_REG, &value, I2C_DATA_LEN,
                           I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    if ((range >= ACC_RANGE_2G) && (range <= ACC_RANGE_16G)) {
        acc_current_factor = bmi055_acc_factor[range];
    }

    return 0;
}


int drv_acc_bosch_bmi055_open(void)
{
    int ret = 0;

    ret = drv_acc_bosch_bmi055_set_power_mode(&bmi055_acc_ctx, DEV_POWER_ON);
    if (unlikely(ret)) {
        return -1;
    }
    return 0;
}

int drv_acc_bosch_bmi055_close(void)
{
    int ret = 0;
    ret     = drv_acc_bosch_bmi055_set_power_mode(&bmi055_acc_ctx, DEV_POWER_OFF);
    if (unlikely(ret)) {
        return -1;
    }
    return 0;
}

int drv_acc_bosch_bmi055_read(void *buf, size_t len)
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

    ret = sensor_i2c_read(&bmi055_acc_ctx, BMI055_ACC_X_AXIS_LSB_ADDR, &reg[0],
                          I2C_REG_LEN, I2C_OP_RETRIES);
    ret |= sensor_i2c_read(&bmi055_acc_ctx, BMI055_ACC_X_AXIS_MSB_ADDR, &reg[1],
                           I2C_REG_LEN, I2C_OP_RETRIES);
    ret |= sensor_i2c_read(&bmi055_acc_ctx, BMI055_ACC_Y_AXIS_LSB_ADDR, &reg[2],
                           I2C_REG_LEN, I2C_OP_RETRIES);
    ret |= sensor_i2c_read(&bmi055_acc_ctx, BMI055_ACC_Y_AXIS_MSB_ADDR, &reg[3],
                           I2C_REG_LEN, I2C_OP_RETRIES);
    ret |= sensor_i2c_read(&bmi055_acc_ctx, BMI055_ACC_Z_AXIS_LSB_ADDR, &reg[4],
                           I2C_REG_LEN, I2C_OP_RETRIES);
    ret |= sensor_i2c_read(&bmi055_acc_ctx, BMI055_ACC_Z_AXIS_MSB_ADDR, &reg[5],
                           I2C_REG_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return -1;
    }

    accel->data[DATA_AXIS_X] =
      (int32_t)((((int32_t)((int8_t)reg[1])) << BMI055_ACC_SHIFT_EIGHT_BITS) |
                (reg[0] & BMI055_ACC_12_BIT_SHIFT));
    accel->data[DATA_AXIS_X] =
      accel->data[DATA_AXIS_X] >> BMI055_ACC_SHIFT_FOUR_BITS;

    accel->data[DATA_AXIS_Y] =
      (int32_t)((((int32_t)((int8_t)reg[3])) << BMI055_ACC_SHIFT_EIGHT_BITS) |
                (reg[2] & BMI055_ACC_12_BIT_SHIFT));
    accel->data[DATA_AXIS_Y] =
      accel->data[DATA_AXIS_Y] >> BMI055_ACC_SHIFT_FOUR_BITS;

    accel->data[DATA_AXIS_Z] =
      (int32_t)((((int32_t)((int8_t)reg[5])) << BMI055_ACC_SHIFT_EIGHT_BITS) |
                (reg[4] & BMI055_ACC_12_BIT_SHIFT));
    accel->data[DATA_AXIS_Z] =
      accel->data[DATA_AXIS_Z] >> BMI055_ACC_SHIFT_FOUR_BITS;

    if (acc_current_factor != 0) {
        // the unit of acc is mg, 1000 mg = 1 g.
        accel->data[DATA_AXIS_X] = accel->data[DATA_AXIS_X] *
                                   ACCELEROMETER_UNIT_FACTOR /
                                   (int32_t)acc_current_factor;
        accel->data[DATA_AXIS_Y] = accel->data[DATA_AXIS_Y] *
                                   ACCELEROMETER_UNIT_FACTOR /
                                   (int32_t)acc_current_factor;
        accel->data[DATA_AXIS_Z] = accel->data[DATA_AXIS_Z] *
                                   ACCELEROMETER_UNIT_FACTOR /
                                   (int32_t)acc_current_factor;
    }
    accel->timestamp = aos_now_ms();

    return (int)size;
}


void drv_acc_bosch_bmi055_irq_handle(void)
{
}

static int drv_acc_bosch_bmi055_ioctl(int cmd, unsigned long arg)
{
    int ret = 0;

    switch (cmd) {
        case SENSOR_IOCTL_ODR_SET: {
            ret = drv_acc_bosch_bmi055_set_odr(&bmi055_acc_ctx, arg);
            if (unlikely(ret)) {
                return -1;
            }
        } break;
        case SENSOR_IOCTL_RANGE_SET: {
            ret = drv_acc_bosch_bmi055_set_range(&bmi055_acc_ctx, arg);
            if (unlikely(ret)) {
                return -1;
            }
        } break;
        case SENSOR_IOCTL_SET_POWER: {
            ret = drv_acc_bosch_bmi055_set_power_mode(&bmi055_acc_ctx, arg);
            if (unlikely(ret)) {
                return -1;
            }
        } break;
        case SENSOR_IOCTL_GET_INFO: {
            /* fill the dev info here */
            dev_sensor_info_t *info = (dev_sensor_info_t *)arg;
            info->model             = "BMI055";
            info->range_max         = 16;
            info->range_min         = 2;
            info->unit              = mg;
        } break;

        default:
            break;
    }

    return 0;
}

int drv_acc_bosch_bmi055_init(void)
{
    int          ret = 0;
    sensor_obj_t sensor;

    memset(&sensor, 0, sizeof(sensor));

    /* fill the sensor obj parameters here */
    sensor.io_port     = I2C_PORT;
    sensor.tag         = TAG_DEV_ACC;
    sensor.path        = dev_acc_path;
    sensor.open        = drv_acc_bosch_bmi055_open;
    sensor.close       = drv_acc_bosch_bmi055_close;
    sensor.read        = drv_acc_bosch_bmi055_read;
    sensor.write       = NULL;
    sensor.ioctl       = drv_acc_bosch_bmi055_ioctl;
    sensor.irq_handle  = drv_acc_bosch_bmi055_irq_handle;
    sensor.mode        = DEV_POLLING;
    sensor.data_len = sizeof(accel_data_t);

    ret = sensor_create_obj(&sensor);
    if (unlikely(ret)) {
        return -1;
    }

    ret = drv_acc_bosch_bmi055_validate_id(&bmi055_acc_ctx, BMI055_ACC_CHIP_ID_VALUE);
    if (unlikely(ret)) {
        return -1;
    }

    ret = drv_acc_bosch_bmi055_soft_reset(&bmi055_acc_ctx);
    if (unlikely(ret)) {
        return -1;
    }

    aos_msleep(5);
    ret = drv_acc_bosch_bmi055_set_range(&bmi055_acc_ctx, ACC_RANGE_8G);
    if (unlikely(ret)) {
        return -1;
    }

    // set odr is 100hz, and will update
    ret = drv_acc_bosch_bmi055_set_odr(&bmi055_acc_ctx, BMI055_ACC_DEFAULT_ODR);
    if (unlikely(ret)) {
        return -1;
    }
    ret = drv_acc_bosch_bmi055_set_power_mode(&bmi055_acc_ctx, DEV_SLEEP);
    if (unlikely(ret)) {
        return -1;
    }

    /* update the phy sensor info to sensor hal */
    LOG("%s %s successfully \n", SENSOR_STR, __func__);
    return 0;
}

static int drv_gyro_bosch_bmi055_validate_id(i2c_dev_t *drv, uint8_t id_value)
{
    uint8_t value = 0x00;
    int     ret   = 0;

    if (drv == NULL) {
        return -1;
    }

    ret = sensor_i2c_read(drv, BMI055_GYRO_CHIP_ID_ADDR, &value, I2C_DATA_LEN,
                          I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    if (id_value != value) {
        return -1;
    }

    return 0;
}

static int drv_gyro_bosch_bmi055_set_power_mode(i2c_dev_t *      drv,
                                                dev_power_mode_e mode)
{
    uint8_t value = 0x00;
    int     ret = 0;

    ret = sensor_i2c_read(drv, BMI055_GYRO_MODE_LPM1_ADDR, &value, I2C_DATA_LEN,
                          I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }
    switch (mode) {
        case DEV_POWER_ON: {
            if ((value & BMI055_GYRO_POWER_BIT_MASK) == BMI055_GYRO_POWER_NORMAL_REG) {
                return 0;
            }
            value &= (~BMI055_GYRO_POWER_BIT_MASK);
            value |= BMI055_GYRO_POWER_NORMAL_REG;
            ret = sensor_i2c_write(drv, BMI055_GYRO_MODE_LPM1_ADDR, &value,
                                   I2C_DATA_LEN, I2C_OP_RETRIES);
            if (unlikely(ret)) {
                return ret;
            }
        } break;

        case DEV_POWER_OFF: {
            if ((value & BMI055_GYRO_POWER_BIT_MASK) ==
                BMI055_GYRO_POWER_DEEP_SUSPEND_REG) {
                return 0;
            }

            value &= (~BMI055_GYRO_POWER_BIT_MASK);
            value |= BMI055_GYRO_POWER_DEEP_SUSPEND_REG;
            ret = sensor_i2c_write(drv, BMI055_GYRO_MODE_LPM1_ADDR, &value,
                                   I2C_DATA_LEN, I2C_OP_RETRIES);
            if (unlikely(ret)) {
                return ret;
            }
        } break;

        case DEV_SLEEP: {
            if ((value & BMI055_GYRO_POWER_BIT_MASK) == BMI055_GYRO_POWER_SLEEP_REG) {
                return 0;
            }

            value &= (~BMI055_GYRO_POWER_BIT_MASK);
            value |= BMI055_GYRO_POWER_SLEEP_REG;
            ret = sensor_i2c_write(drv, BMI055_GYRO_MODE_LPM1_ADDR, &value,
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

static int drv_gyro_bosch_bmi055_set_odr(i2c_dev_t *drv, uint32_t odr)
{
    int     ret   = 0;
    uint8_t value = 0x00;

    ret = sensor_i2c_read(drv, BMI055_GYRO_BW_ADDR, &value, I2C_DATA_LEN,
                          I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    if (odr >= BMI055_GYRO_ODR_2000) {
        value &= BMI055_GYRO_BW_ADDR_MSK;
        value |= BMI055_GYRO_ODR_2000_REG;
    } else if (odr >= BMI055_GYRO_ODR_1000) {
        value &= BMI055_GYRO_BW_ADDR_MSK;
        value |= BMI055_GYRO_ODR_1000_REG;
    } else if (odr >= BMI055_GYRO_ODR_400) {
        value &= BMI055_GYRO_BW_ADDR_MSK;
        value |= BMI055_GYRO_ODR_400_REG;
    } else if (odr >= BMI055_GYRO_ODR_200) {
        value &= BMI055_GYRO_BW_ADDR_MSK;
        value |= BMI055_GYRO_ODR_200_REG;
    } else {
        value &= BMI055_GYRO_BW_ADDR_MSK;
        value |= BMI055_GYRO_ODR_100_REG;
    }

    /* Write the range register 0x0F*/
    ret = sensor_i2c_write(drv, BMI055_GYRO_BW_ADDR, &value, I2C_DATA_LEN,
                           I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }
    return 0;
}

static int drv_gyro_bosch_bmi055_set_range(i2c_dev_t *drv, uint32_t range)
{
    int     ret   = 0;
    uint8_t value = 0x00;

    ret = sensor_i2c_read(drv, BMI055_GYRO_RANGE_ADDR, &value, I2C_DATA_LEN,
                          I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    switch (range) {
        case GYRO_RANGE_125DPS: {
            value &= BMI055_GYRO_RANGE_ADDR_RANGE_MSK;
            value |= BMI055_GYRO_RANGE_RANGE_125DPS;
        } break;

        case GYRO_RANGE_250DPS: {
            value &= BMI055_GYRO_RANGE_ADDR_RANGE_MSK;
            value |= BMI055_GYRO_RANGE_RANGE_250DPS;
        } break;

        case GYRO_RANGE_500DPS: {
            value &= BMI055_GYRO_RANGE_ADDR_RANGE_MSK;
            value |= BMI055_GYRO_RANGE_RANGE_500DPS;
        } break;

        case GYRO_RANGE_1000DPS: {
            value &= BMI055_GYRO_RANGE_ADDR_RANGE_MSK;
            value |= BMI055_GYRO_RANGE_RANGE_1000DPS;
        } break;

        case GYRO_RANGE_2000DPS: {
            value &= BMI055_GYRO_RANGE_ADDR_RANGE_MSK;
            value |= BMI055_GYRO_RANGE_RANGE_2000DPS;
        } break;

        default:
            break;
    }

    /* Write the range register 0x0F*/
    ret = sensor_i2c_write(drv, BMI055_GYRO_RANGE_ADDR, &value, I2C_DATA_LEN,
                           I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    if ((range >= GYRO_RANGE_125DPS) && (range <= GYRO_RANGE_2000DPS)) {
        gyro_current_factor = bmi055_gyro_factor[range];
    }
    return 0;
}

static void drv_gyro_bosch_bmi055_irq_handle(void)
{
    /* no handle so far */
}

static int drv_gyro_bosch_bmi055_open(void)
{
    int ret = 0;
    ret     = drv_gyro_bosch_bmi055_set_power_mode(&bmi055_gyro_ctx, DEV_POWER_ON);
    if (unlikely(ret)) {
        return -1;
    }

    LOG("%s %s successfully \n", SENSOR_STR, __func__);
    return 0;
}

static int drv_gyro_bosch_bmi055_close(void)
{
    int ret = 0;
    ret     = drv_gyro_bosch_bmi055_set_power_mode(&bmi055_gyro_ctx, DEV_POWER_OFF);
    if (unlikely(ret)) {
        return -1;
    }
    return 0;
}

static int drv_gyro_bosch_bmi055_read(void *buf, size_t len)
{
    int          ret  = 0;
    size_t       size = 0;
    uint8_t      reg[6];
    gyro_data_t *gyro = (gyro_data_t *)buf;
    if (buf == NULL) {
        return -1;
    }

    size = sizeof(gyro_data_t);
    if (len < size) {
        return -1;
    }

    ret = sensor_i2c_read(&bmi055_gyro_ctx, BMI055_GYRO_RATE_X_LSB_ADDR, &reg[0],
                          I2C_REG_LEN, I2C_OP_RETRIES);
    ret |= sensor_i2c_read(&bmi055_gyro_ctx, BMI055_GYRO_RATE_X_MSB_ADDR, &reg[1],
                           I2C_REG_LEN, I2C_OP_RETRIES);
    ret |= sensor_i2c_read(&bmi055_gyro_ctx, BMI055_GYRO_RATE_Y_LSB_ADDR, &reg[2],
                           I2C_REG_LEN, I2C_OP_RETRIES);
    ret |= sensor_i2c_read(&bmi055_gyro_ctx, BMI055_GYRO_RATE_Y_MSB_ADDR, &reg[3],
                           I2C_REG_LEN, I2C_OP_RETRIES);
    ret |= sensor_i2c_read(&bmi055_gyro_ctx, BMI055_GYRO_RATE_Z_LSB_ADDR, &reg[4],
                           I2C_REG_LEN, I2C_OP_RETRIES);
    ret |= sensor_i2c_read(&bmi055_gyro_ctx, BMI055_GYRO_RATE_Z_MSB_ADDR, &reg[5],
                           I2C_REG_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return -1;
    }
    gyro->data[DATA_AXIS_X] = (int32_t)(
      (((int32_t)((int8_t)reg[1])) << BMI055_GYRO_SHIFT_EIGHT_BITS) | reg[0]);
    gyro->data[DATA_AXIS_Y] = (int32_t)(
      (((int32_t)((int8_t)reg[3])) << BMI055_GYRO_SHIFT_EIGHT_BITS) | reg[2]);
    gyro->data[DATA_AXIS_Z] = (int32_t)(
      (((int32_t)((int8_t)reg[5])) << BMI055_GYRO_SHIFT_EIGHT_BITS) | reg[4]);

    if (gyro_current_factor != 0) {
        // the unit of gyro is uDPS, 1000 000 uDPS = 1 DPS
        gyro->data[DATA_AXIS_X] = (int32_t)((int64_t)gyro->data[DATA_AXIS_X] * GYROSCOPE_UNIT_FACTOR * 10 / gyro_current_factor);
        gyro->data[DATA_AXIS_Y] = (int32_t)((int64_t)gyro->data[DATA_AXIS_Y] * GYROSCOPE_UNIT_FACTOR * 10 / gyro_current_factor);
        gyro->data[DATA_AXIS_Z] = (int32_t)((int64_t)gyro->data[DATA_AXIS_Z] * GYROSCOPE_UNIT_FACTOR * 10 / gyro_current_factor);
    }

    gyro->timestamp = aos_now_ms();
    return (int)size;
}


static int drv_gyro_bosch_bmi055_ioctl(int cmd, unsigned long arg)
{
    int ret = 0;

    switch (cmd) {
        case SENSOR_IOCTL_ODR_SET: {
            ret = drv_gyro_bosch_bmi055_set_odr(&bmi055_gyro_ctx, arg);
            if (unlikely(ret)) {
                return -1;
            }
        } break;
        case SENSOR_IOCTL_RANGE_SET: {
            ret = drv_gyro_bosch_bmi055_set_range(&bmi055_gyro_ctx, arg);
            if (unlikely(ret)) {
                return -1;
            }
        } break;
        case SENSOR_IOCTL_SET_POWER: {
            ret = drv_gyro_bosch_bmi055_set_power_mode(&bmi055_gyro_ctx, arg);
            if (unlikely(ret)) {
                return -1;
            }
        } break;
        case SENSOR_IOCTL_GET_INFO: {
            /* fill the dev info here */
            dev_sensor_info_t *info = (dev_sensor_info_t *)arg;
            info->model             = "BMI055";
            info->range_max         = 2000;
            info->range_min         = 125;
            info->unit              = udps;
        } break;

        default:
            break;
    }

    return 0;
}

int drv_gyro_bosch_bmi055_init(void)
{
    int          ret = 0;
    sensor_obj_t sensor;
    memset(&sensor, 0, sizeof(sensor));

    /* fill the sensor obj parameters here */
    sensor.io_port    = I2C_PORT;
    sensor.tag        = TAG_DEV_GYRO;
    sensor.path       = dev_gyro_path;
    sensor.open       = drv_gyro_bosch_bmi055_open;
    sensor.close      = drv_gyro_bosch_bmi055_close;
    sensor.read       = drv_gyro_bosch_bmi055_read;
    sensor.write      = NULL;
    sensor.ioctl      = drv_gyro_bosch_bmi055_ioctl;
    sensor.irq_handle = drv_gyro_bosch_bmi055_irq_handle;

    ret = sensor_create_obj(&sensor);
    if (unlikely(ret)) {
        return -1;
    }

    ret = drv_gyro_bosch_bmi055_validate_id(&bmi055_gyro_ctx, BMI055_GYRO_CHIP_ID_VALUE);
    if (unlikely(ret)) {
        return -1;
    }

    ret = drv_gyro_bosch_bmi055_set_range(&bmi055_gyro_ctx, GYRO_RANGE_2000DPS);
    if (unlikely(ret)) {
        return -1;
    }

    ret = drv_gyro_bosch_bmi055_set_odr(&bmi055_gyro_ctx, BMI055_GYRO_DEFAULT_ODR);
    if (unlikely(ret)) {
        return -1;
    }

    ret = drv_gyro_bosch_bmi055_set_power_mode(&bmi055_gyro_ctx, DEV_POWER_OFF);
    if (unlikely(ret)) {
        return -1;
    }

    /* update the phy sensor info to sensor hal */
    LOG("%s %s successfully \n", SENSOR_STR, __func__);
    return 0;
}

SENSOR_DRV_ADD(drv_acc_bosch_bmi055_init);
SENSOR_DRV_ADD(drv_gyro_bosch_bmi055_init);

