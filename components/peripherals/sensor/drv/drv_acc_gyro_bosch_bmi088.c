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


#define BMI088_ACC_I2C_ADDR_LOW                     (0x18)
#define BMI088_ACC_I2C_ADDR_HIGH                    (0x19)
#define BMI088_ACC_I2C_ADDR_TRANS(n)                ((n)<<1)
#define BMI088_ACC_I2C_ADDR                         BMI088_ACC_I2C_ADDR_TRANS(BMI088_ACC_I2C_ADDR_LOW)

#define BMI088_ACC_CHIP_ID_ADDR                     UINT8_C(0X00)
#define BMI088_ACC_CHIP_ID_VALUE                    (0x1E)

#define BMI088_ACC_POWER_CONF_ADDR                  UINT8_C(0x7C)
#define BMI088_ACC_POWER_CTRL_ADDR                  UINT8_C(0x7D)

#define BMI088_ACC_CONFIG_ADDR                      UINT8_C(0X40)
#define BMI088_ACC_CONFIG1_ADDR                     UINT8_C(0X41)

#define BMI088_ACC_DEFAULT_ODR_100HZ                (100)
#define BMI088_ACC_CMD_ADDR                         UINT8_C(0X7E)

#define BMI088_ACC_OUTPUT_DATA_RATE_0_78HZ          UINT8_C(0x01)
#define BMI088_ACC_OUTPUT_DATA_RATE_1_56HZ          UINT8_C(0x02)
#define BMI088_ACC_OUTPUT_DATA_RATE_3_12HZ          UINT8_C(0x03)
#define BMI088_ACC_OUTPUT_DATA_RATE_6_25HZ          UINT8_C(0x04)
#define BMI088_ACC_OUTPUT_DATA_RATE_12_5HZ          UINT8_C(0x05)
#define BMI088_ACC_OUTPUT_DATA_RATE_25HZ            UINT8_C(0x06)
#define BMI088_ACC_OUTPUT_DATA_RATE_50HZ            UINT8_C(0x07)
#define BMI088_ACC_OUTPUT_DATA_RATE_100HZ           UINT8_C(0x08)
#define BMI088_ACC_OUTPUT_DATA_RATE_200HZ           UINT8_C(0x09)
#define BMI088_ACC_OUTPUT_DATA_RATE_400HZ           UINT8_C(0x0A)
#define BMI088_ACC_OUTPUT_DATA_RATE_800HZ           UINT8_C(0x0B)
#define BMI088_ACC_OUTPUT_DATA_RATE_1600HZ          UINT8_C(0x0C)

#define BMI088_ACC_OSR4_AVG1                        UINT8_C(0)
#define BMI088_ACC_OSR2_AVG2                        UINT8_C(1)
#define BMI088_ACC_NORMAL_AVG4                      UINT8_C(2)
#define BMI088_ACC_CIC_AVG8                         UINT8_C(3)
#define BMI088_ACC_RES_AVG16                        UINT8_C(4)
#define BMI088_ACC_RES_AVG32                        UINT8_C(5)
#define BMI088_ACC_RES_AVG64                        UINT8_C(6)
#define BMI088_ACC_RES_AVG128                       UINT8_C(7)

#define BMI088_ACC_RANGE_2G                         UINT8_C(0)
#define BMI088_ACC_RANGE_4G                         UINT8_C(1)
#define BMI088_ACC_RANGE_8G                         UINT8_C(2)
#define BMI088_ACC_RANGE_16G                        UINT8_C(3)

#define BMI088_ACC_DATA_0_ADDR                      UINT8_C(0X0A)
#define BMI088_ACC_DATA_8_ADDR                      UINT8_C(0X12)
#define BMI088_ACC_DATA_LENGTH                      UINT8_C(6)

#define BMI088_ACC_ENABLE                           UINT8_C(0x01)
#define BMI088_ACC_DISABLE                          UINT8_C(0x00)

#define BMI088_ACC_ADVANCE_POWER_SAVE_MSK           UINT8_C (0x01)

#define BMI088_ACC_ENABLE_POS                       UINT8_C(2)
#define BMI088_ACC_ENABLE_MSK                       UINT8_C(0x04)

#define BMI088_ACC_PERF_POS                         UINT8_C(7)

#define BMI088_ACC_ODR_POS                          UINT8_C(0)
#define BMI088_ACC_ODR_MSK                          UINT8_C(0x0F)

#define BMI088_ACC_RANGE_POS                        UINT8_C(0)
#define BMI088_ACC_RANGE_MSK                        UINT8_C(0x03)

#define BMI088_ACC_DEFAULT_ODR                      (100)

#define BMI088_ACC_ENABLE_SOFT_RESET_VALUE          UINT8_C(0XB6)

#define BMI088_ACC_GET_BITSLICE(regvar, bitname)\
  ((regvar & bitname##_MSK) >> bitname##_POS)
#define BMI088_ACC_SET_BITSLICE(regvar, bitname, val)\
  ((regvar & ~bitname##_MSK) | \
  ((val<<bitname##_POS)&bitname##_MSK))
#define BMI088_ACC_SET_BITS_POS_0(reg_data, bitname, data) \
    ((reg_data & ~(bitname##_MSK)) | \
    (data & bitname##_MSK))

#define BMI088_ACC_GET_BITS_POS_0(reg_data, bitname) (reg_data & (bitname##_MSK))



#define BMI088_GYRO_I2C_ADDR1               (0x68 << 1)
#define BMI088_GYRO_I2C_ADDR2               (0x69 << 1)

#define BMI088_GYRO_CHIP_ID_ADDR            (0x00)
#define BMI088_GYRO_CHIP_ID_VALUE           (0x0F)

#define BMI088_GYRO_RATE_X_LSB_ADDR         (0x02)
#define BMI088_GYRO_RATE_X_MSB_ADDR         (0x03)
#define BMI088_GYRO_RATE_Y_LSB_ADDR         (0x04)
#define BMI088_GYRO_RATE_Y_MSB_ADDR         (0x05)
#define BMI088_GYRO_RATE_Z_LSB_ADDR         (0x06)
#define BMI088_GYRO_RATE_Z_MSB_ADDR         (0x07)
#define BMI088_GYRO_TEMP_ADDR               (0x08)
#define BMI088_GYRO_INTR_STAT0_ADDR         (0x09)
#define BMI088_GYRO_INTR_STAT1_ADDR         (0x0A)
#define BMI088_GYRO_INTR_STAT2_ADDR         (0x0B)
#define BMI088_GYRO_INTR_STAT3_ADDR         (0x0C)
#define BMI088_GYRO_FIFO_STAT_ADDR          (0x0E)
#define BMI088_GYRO_RANGE_ADDR              (0x0F)
#define BMI088_GYRO_BW_ADDR                 (0x10)
#define BMI088_GYRO_MODE_LPM1_ADDR          (0x11)
#define BMI088_GYRO_MODE_LPM2_ADDR          (0x12)
#define BMI088_GYRO_HIGH_BW_ADDR            (0x13)
#define BMI088_GYRO_BGW_SOFT_RST_ADDR       (0x14)
#define BMI088_GYRO_INTR_ENABLE0_ADDR       (0x15)
#define BMI088_GYRO_INTR_ENABLE1_ADDR       (0x16)
#define BMI088_GYRO_INTR_MAP_ZERO_ADDR      (0x17)
#define BMI088_GYRO_INTR_MAP_ONE_ADDR       (0x18)
#define BMI088_GYRO_INTR_MAP_TWO_ADDR       (0x19)
#define BMI088_GYRO_INTR_ZERO_ADDR          (0x1A)
#define BMI088_GYRO_INTR_ONE_ADDR           (0x1B)
#define BMI088_GYRO_INTR_TWO_ADDR           (0x1C)
#define BMI088_GYRO_INTR_4_ADDR             (0x1E)
#define BMI088_GYRO_RST_LATCH_ADDR          (0x21)
#define BMI088_GYRO_HIGHRATE_THRES_X_ADDR   (0x22)
#define BMI088_GYRO_HIGHRATE_DURN_X_ADDR    (0x23)
#define BMI088_GYRO_HIGHRATE_THRES_Y_ADDR   (0x24)
#define BMI088_GYRO_HIGHRATE_DURN_Y_ADDR    (0x25)
#define BMI088_GYRO_HIGHRATE_THRES_Z_ADDR   (0x26)
#define BMI088_GYRO_HIGHRATE_DURN_Z_ADDR    (0x27)
#define BMI088_GYRO_SOC_ADDR                (0x31)
#define BMI088_GYRO_A_FOC_ADDR              (0x32)
#define BMI088_GYRO_TRIM_NVM_CTRL_ADDR      (0x33)
#define BMI088_GYRO_BGW_SPI3_WDT_ADDR       (0x34)

#define BMI088_GYRO_OFFSET_OFC1_ADDR        (0x36)
#define BMI088_GYRO_OFC2_ADDR               (0x37)
#define BMI088_GYRO_OFC3_ADDR               (0x38)
#define BMI088_GYRO_OFC4_ADDR               (0x39)
#define BMI088_GYRO_TRIM_GP0_ADDR           (0x3A)
#define BMI088_GYRO_TRIM_GP1_ADDR           (0x3B)
#define BMI088_GYRO_SELFTEST_ADDR           (0x3C)
#define BMI088_GYRO_FIFO_CGF1_ADDR          (0x3D)
#define BMI088_GYRO_FIFO_CGF0_ADDR          (0x3E)

#define BMI088_GYRO_FIFO_DATA_ADDR          (0x3F)


#define BMI088_GYRO_RANGE_RANGE_2000DPS     (0x00)
#define BMI088_GYRO_RANGE_RANGE_1000DPS     (0x01)
#define BMI088_GYRO_RANGE_RANGE_500DPS      (0x02)
#define BMI088_GYRO_RANGE_RANGE_250DPS      (0x03)
#define BMI088_GYRO_RANGE_RANGE_125DPS      (0x04)
#define BMI088_GYRO_RANGE_ADDR_RANGE_POS    (0)
#define BMI088_GYRO_RANGE_ADDR_RANGE_LEN    (3)
#define BMI088_GYRO_RANGE_ADDR_RANGE_MSK    (0x07)
#define BMI088_GYRO_RANGE_ADDR_RANGE_REG    (BMI088_GYRO_RANGE_ADDR)


#define BMI088_GYRO_BW_ADDR_POS             (0)
#define BMI088_GYRO_BW_ADDR_LEN             (3)
#define BMI088_GYRO_BW_ADDR_MSK             (0x07)
#define BMI088_GYRO_BW_ADDR_REG             (BMI088_GYRO_BW_ADDR)

#define BMI088_GYRO_MODE_LPM1_POS           (5)
#define BMI088_GYRO_MODE_LPM1_LEN           (3)
#define BMI088_GYRO_MODE_LPM1_MSK           (0xA0)
#define BMI088_GYRO_MODE_LPM1_REG           (BMI088_GYRO_MODE_LPM1_ADDR)


#define BMI088_GYRO_ODR_100                 (100)
#define BMI088_GYRO_ODR_200                 (200)
#define BMI088_GYRO_ODR_400                 (400)
#define BMI088_GYRO_ODR_1000                (1000)
#define BMI088_GYRO_ODR_2000                (2000)

#define BMI088_GYRO_ODR_100_REG             (0x07)
#define BMI088_GYRO_ODR_200_REG             (0x06)
#define BMI088_GYRO_ODR_400_REG             (0x03)
#define BMI088_GYRO_ODR_1000_REG            (0x02)
#define BMI088_GYRO_ODR_2000_REG            (0x01)

#define BMI088_GYRO_POWER_BIT_MASK          (0xA0)
#define BMI088_GYRO_POWER_NORMAL_REG        (0x00)
#define BMI088_GYRO_POWER_SLEEP_REG         (0x80)
#define BMI088_GYRO_POWER_DEEP_SUSPEND_REG  (0x20)

#define BMI088_GYRO_SHIFT_EIGHT_BITS        (8)
#define BMI088_GYRO_DEFAULT_ODR             (200)

static uint32_t bmi088_acc_factor[4] = { 16384, 8192, 4096, 2048 };
static uint32_t acc_current_factor = 0;
static uint32_t set_range_failed = 0;
static uint32_t bmi088_gyro_factor[5] = { 2624, 1312, 656, 328,
                                     164 }; 
static uint32_t gyro_current_factor   = 0;

i2c_dev_t bmi088_acc_ctx = {
    .port               = 3,
    .config.dev_addr    = BMI088_ACC_I2C_ADDR,
};

i2c_dev_t bmi088_gyro_ctx = {
    .port               = 3,
    .config.dev_addr    = BMI088_GYRO_I2C_ADDR1,
};

static int drv_acc_bosch_bmi088_soft_reset(i2c_dev_t* drv)
{
    int ret = 0;
    uint8_t value = BMI088_ACC_ENABLE_SOFT_RESET_VALUE;
    ret = sensor_i2c_write(drv, BMI088_ACC_CMD_ADDR, &value,
                           I2C_DATA_LEN, I2C_OP_RETRIES);
    aos_msleep(2);
    if(unlikely(ret) != 0) {
        return -1;
    }
    return 0;
}


static int drv_acc_bosch_bmi088_validate_id(i2c_dev_t* drv)
{
    uint8_t value = 0x00;
    int ret = 0;

    if(drv == NULL) {
        return -1;
    }

    ret = sensor_i2c_read(drv, BMI088_ACC_CHIP_ID_ADDR, &value, I2C_DATA_LEN,
                          I2C_OP_RETRIES);
    if(unlikely(ret) != 0) {
        return ret;
    }

    if(BMI088_ACC_CHIP_ID_VALUE != value) {
        return -1;
    }

    return 0;
}

static int drv_acc_bosch_bmi088_set_power_mode(i2c_dev_t* drv,
                                               dev_power_mode_e mode)
{

    uint8_t value, value1 = 0x00;
    int ret = 0;
    switch(mode) {
        case DEV_POWER_ON: {
            ret = sensor_i2c_read(drv, BMI088_ACC_CONFIG_ADDR, &value, I2C_DATA_LEN,
                                  I2C_OP_RETRIES);
            if(unlikely(ret) != 0) {
                return ret;
            }
            value |= (BMI088_ACC_ENABLE << BMI088_ACC_PERF_POS);
            ret = sensor_i2c_write(drv, BMI088_ACC_CONFIG_ADDR, &value, I2C_DATA_LEN,
                                   I2C_OP_RETRIES);
            aos_msleep(2);
            if(unlikely(ret) != 0) {
                return ret;
            }

            ret = sensor_i2c_read(drv, BMI088_ACC_POWER_CTRL_ADDR, &value1, I2C_DATA_LEN,
                                  I2C_OP_RETRIES);
            if(unlikely(ret) != 0) {
                return ret;
            }
            value1 = BMI088_ACC_SET_BITSLICE(value1, BMI088_ACC_ENABLE, BMI088_ACC_ENABLE);
            ret = sensor_i2c_write(drv, BMI088_ACC_POWER_CTRL_ADDR, &value1, I2C_DATA_LEN,
                                   I2C_OP_RETRIES);
            aos_msleep(2);
            if(unlikely(ret) != 0) {
                return ret;
            }
        }
        break;
        case DEV_POWER_OFF:
        case DEV_SLEEP:
        case DEV_SUSPEND: {

            ret = sensor_i2c_read(drv, BMI088_ACC_POWER_CTRL_ADDR, &value, I2C_DATA_LEN,
                                  I2C_OP_RETRIES);
            if(unlikely(ret) != 0) {
                return ret;
            }
            value = BMI088_ACC_SET_BITSLICE(value, BMI088_ACC_ENABLE, BMI088_ACC_DISABLE);
            ret = sensor_i2c_write(drv, BMI088_ACC_POWER_CTRL_ADDR, &value, I2C_DATA_LEN,
                                   I2C_OP_RETRIES);
            aos_msleep(2);
            if(unlikely(ret) != 0) {
                return ret;
            }

            ret = sensor_i2c_read(drv, BMI088_ACC_POWER_CONF_ADDR, &value1, I2C_DATA_LEN,
                                  I2C_OP_RETRIES);
            if(unlikely(ret) != 0) {
                return ret;
            }
            value1 = BMI088_ACC_SET_BITS_POS_0(value1, BMI088_ACC_ADVANCE_POWER_SAVE, BMI088_ACC_ENABLE);
            ret = sensor_i2c_write(drv, BMI088_ACC_POWER_CONF_ADDR, &value1, I2C_DATA_LEN,
                                   I2C_OP_RETRIES);
            aos_msleep(2);
            if(unlikely(ret) != 0) {
                return ret;
            }
        }
        break;

        default:
            break;
    }
    return 0;
}

static int drv_acc_bosch_bmi088_set_odr(i2c_dev_t* drv, uint32_t hz)
{
    int ret = 0;
    uint8_t value = 0x00;
    uint8_t odr = 0x00;

    ret = sensor_i2c_read(drv, BMI088_ACC_CONFIG_ADDR, &value, I2C_DATA_LEN,
                          I2C_OP_RETRIES);
    if(unlikely(ret) != 0) {
        return ret;
    }

    if(hz >= 800) {
        odr = BMI088_ACC_OUTPUT_DATA_RATE_1600HZ;
    } else if(hz >= 400) {
        odr = BMI088_ACC_OUTPUT_DATA_RATE_800HZ;
    } else if(hz >= 200) {
        odr = BMI088_ACC_OUTPUT_DATA_RATE_400HZ;
    } else if(hz >= 100) {
        odr = BMI088_ACC_OUTPUT_DATA_RATE_200HZ;
    } else if(hz >= 50) {
        odr = BMI088_ACC_OUTPUT_DATA_RATE_100HZ;
    } else if(hz >= 25) {
        odr = BMI088_ACC_OUTPUT_DATA_RATE_50HZ;
    } else if(hz >= 12) {
        odr = BMI088_ACC_OUTPUT_DATA_RATE_25HZ;
    } else if(hz >= 6) {
        odr = BMI088_ACC_OUTPUT_DATA_RATE_12_5HZ;
    } else if(hz >= 3) {
        odr = BMI088_ACC_OUTPUT_DATA_RATE_6_25HZ;
    } else {
        odr = BMI088_ACC_OUTPUT_DATA_RATE_3_12HZ;
    }

    value = BMI088_ACC_SET_BITSLICE(value, BMI088_ACC_ODR, odr);
    ret = sensor_i2c_write(drv, BMI088_ACC_CONFIG_ADDR, &value, I2C_DATA_LEN,
                           I2C_OP_RETRIES);
    aos_msleep(2);
    if(unlikely(ret) != 0) {
        return ret;
    }
    return 0;
}

static int drv_acc_bosch_bmi088_set_range(i2c_dev_t* drv, uint32_t range)
{
    int ret = 0;
    uint8_t value = 0x00;
    uint8_t acc_range = 0x00;

    ret = sensor_i2c_read(drv, BMI088_ACC_CONFIG1_ADDR, &value, I2C_DATA_LEN,
                          I2C_OP_RETRIES);
    if(unlikely(ret) != 0) {
        set_range_failed = 1;
        return ret;
    }

    switch(range) {
        case ACC_RANGE_2G: {
            acc_range = BMI088_ACC_RANGE_2G;
        }
        break;

        case ACC_RANGE_4G: {
            acc_range = BMI088_ACC_RANGE_4G;
        }
        break;

        case ACC_RANGE_8G: {
            acc_range = BMI088_ACC_RANGE_8G;
        }
        break;

        case ACC_RANGE_16G: {
            acc_range = BMI088_ACC_RANGE_16G;
        }
        break;

        default:
            break;
    }

    value = BMI088_ACC_SET_BITSLICE(value, BMI088_ACC_RANGE, acc_range);
    ret = sensor_i2c_write(drv, BMI088_ACC_CONFIG1_ADDR, &value, I2C_DATA_LEN,
                           I2C_OP_RETRIES);
    aos_msleep(2);
    if(unlikely(ret) != 0) {
        set_range_failed = 2;
        return ret;
    }

    if((range >= ACC_RANGE_2G)&&(range <= ACC_RANGE_16G)) {
        acc_current_factor = bmi088_acc_factor[range];
    }

    set_range_failed = 3;
    return 0;
}


static void drv_acc_bosch_bmi088_irq_handle(void)
{
    /* no handle so far */
}


static int drv_acc_bosch_bmi088_open(void)
{
    int ret = 0;
    ret  = drv_acc_bosch_bmi088_set_power_mode(&bmi088_acc_ctx, DEV_POWER_ON);
    if(unlikely(ret) != 0) {
        return -1;
    }
    return 0;

}


static int drv_acc_bosch_bmi088_close(void)
{
    int ret = 0;
    ret  = drv_acc_bosch_bmi088_set_power_mode(&bmi088_acc_ctx, DEV_POWER_OFF);
    if(unlikely(ret) != 0) {
        return -1;
    }
    return 0;
}


static int drv_acc_bosch_bmi088_read(void *buf, size_t len)
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
    ret  = sensor_i2c_read(&bmi088_acc_ctx, 0x41,  &range, I2C_REG_LEN, I2C_OP_RETRIES);
    ret  = sensor_i2c_read(&bmi088_acc_ctx, BMI088_ACC_DATA_8_ADDR,  &reg[0], I2C_REG_LEN,
                           I2C_OP_RETRIES);
    ret |= sensor_i2c_read(&bmi088_acc_ctx, BMI088_ACC_DATA_8_ADDR+1,  &reg[1], I2C_REG_LEN,
                           I2C_OP_RETRIES);
    ret |= sensor_i2c_read(&bmi088_acc_ctx, BMI088_ACC_DATA_8_ADDR+2,  &reg[2], I2C_REG_LEN,
                           I2C_OP_RETRIES);
    ret |= sensor_i2c_read(&bmi088_acc_ctx, BMI088_ACC_DATA_8_ADDR+3,  &reg[3], I2C_REG_LEN,
                           I2C_OP_RETRIES);
    ret |= sensor_i2c_read(&bmi088_acc_ctx, BMI088_ACC_DATA_8_ADDR+4,  &reg[4], I2C_REG_LEN,
                           I2C_OP_RETRIES);
    ret |= sensor_i2c_read(&bmi088_acc_ctx, BMI088_ACC_DATA_8_ADDR+5,  &reg[5], I2C_REG_LEN,
                           I2C_OP_RETRIES);

    if(unlikely(ret) != 0) {
        return -1;
    }

    accel->data[DATA_AXIS_X] = ((int16_t)((reg[1] << 8) | reg[0]));
    accel->data[DATA_AXIS_Y] = ((int16_t)((reg[3] << 8) | reg[2]));
    accel->data[DATA_AXIS_Z] = ((int16_t)((reg[5] << 8) | reg[4]));
    if(acc_current_factor != 0) {
        /* the unit of acc is mg, 1000 mg = 1 g */
        accel->data[DATA_AXIS_X] = accel->data[DATA_AXIS_X] *
                                   ACCELEROMETER_UNIT_FACTOR / (int32_t)acc_current_factor;
        accel->data[DATA_AXIS_Y] = accel->data[DATA_AXIS_Y] *
                                   ACCELEROMETER_UNIT_FACTOR / (int32_t)acc_current_factor;
        accel->data[DATA_AXIS_Z] = accel->data[DATA_AXIS_Z] *
                                   ACCELEROMETER_UNIT_FACTOR / (int32_t)acc_current_factor;
    }
    accel->timestamp = aos_now_ms();

    return (int)size;
}

static int drv_acc_bosch_bmi088_ioctl(int cmd, unsigned long arg)
{
    int ret = 0;

    switch(cmd) {
        case SENSOR_IOCTL_ODR_SET: {
            ret = drv_acc_bosch_bmi088_set_odr(&bmi088_acc_ctx, arg);
            if(unlikely(ret) != 0) {
                return -1;
            }
        }
        break;
        case SENSOR_IOCTL_RANGE_SET: {
            ret = drv_acc_bosch_bmi088_set_range(&bmi088_acc_ctx, arg);
            if(unlikely(ret) != 0) {
                return -1;
            }
        }
        break;
        case SENSOR_IOCTL_SET_POWER: {
            ret = drv_acc_bosch_bmi088_set_power_mode(&bmi088_acc_ctx, arg);
            if(unlikely(ret) != 0) {
                return -1;
            }
        }
        break;
        case SENSOR_IOCTL_GET_INFO: {
            /* fill the dev info here */
            dev_sensor_info_t *info = (dev_sensor_info_t *)arg;
            info->model = "BMI088";
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


int drv_acc_bosch_bmi088_init(void) {
    int ret = 0;
    sensor_obj_t sensor;
    memset(&sensor, 0, sizeof(sensor));

    /* fill the sensor obj parameters here */
    sensor.io_port    = I2C_PORT;
    sensor.tag        = TAG_DEV_ACC;
    sensor.path       = dev_acc_path;
    sensor.open       = drv_acc_bosch_bmi088_open;
    sensor.close      = drv_acc_bosch_bmi088_close;
    sensor.read       = drv_acc_bosch_bmi088_read;
    sensor.write      = NULL;
    sensor.ioctl      = drv_acc_bosch_bmi088_ioctl;
    sensor.irq_handle = drv_acc_bosch_bmi088_irq_handle;
    ret = sensor_create_obj(&sensor);
    if(unlikely(ret) != 0) {
        return -1;
    }

    ret = drv_acc_bosch_bmi088_validate_id(&bmi088_acc_ctx);
    if(unlikely(ret) != 0) {
        return -1;
    }

    ret = drv_acc_bosch_bmi088_soft_reset(&bmi088_acc_ctx);
    if(unlikely(ret) != 0) {
        return -1;
    }

    ret = drv_acc_bosch_bmi088_set_range(&bmi088_acc_ctx, ACC_RANGE_8G);
    if(unlikely(ret) != 0) {
        return -1;
    }

    /* set odr is 100hz, and will update */
    ret = drv_acc_bosch_bmi088_set_odr(&bmi088_acc_ctx, BMI088_ACC_DEFAULT_ODR);
    if(unlikely(ret) != 0) {
        return -1;
    }
    ret = drv_acc_bosch_bmi088_set_power_mode(&bmi088_acc_ctx, DEV_SLEEP);
    if(unlikely(ret) != 0) {
        return -1;
    }
    LOG("%s %s successfully \n", SENSOR_STR, __func__);
    /* update the phy sensor info to sensor hal */
    return 0;
}


static int drv_gyro_bosch_bmi088_validate_id(i2c_dev_t *drv, uint8_t id_value)
{
    uint8_t value = 0x00;
    int     ret   = 0;

    if (drv == NULL) {
        return -1;
    }

    ret = sensor_i2c_read(drv, BMI088_GYRO_CHIP_ID_ADDR, &value, I2C_DATA_LEN,
                          I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    if (id_value != value) {
        return -1;
    }

    return 0;
}

static int drv_gyro_bosch_bmi088_set_power_mode(i2c_dev_t *      drv,
                                                dev_power_mode_e mode)
{
    uint8_t value = 0x00;
    int     ret = 0;

    ret = sensor_i2c_read(drv, BMI088_GYRO_MODE_LPM1_ADDR, &value, I2C_DATA_LEN,
                          I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }
    switch (mode) {
        case DEV_POWER_ON: {
            if ((value & BMI088_GYRO_POWER_BIT_MASK) == BMI088_GYRO_POWER_NORMAL_REG) {
                return 0;
            }
            value &= (~BMI088_GYRO_POWER_BIT_MASK);
            value |= BMI088_GYRO_POWER_NORMAL_REG;
            ret = sensor_i2c_write(drv, BMI088_GYRO_MODE_LPM1_ADDR, &value,
                                   I2C_DATA_LEN, I2C_OP_RETRIES);
            if (unlikely(ret)) {
                return ret;
            }
        } break;

        case DEV_POWER_OFF: {
            if ((value & BMI088_GYRO_POWER_BIT_MASK) ==
                BMI088_GYRO_POWER_DEEP_SUSPEND_REG) {
                return 0;
            }

            value &= (~BMI088_GYRO_POWER_BIT_MASK);
            value |= BMI088_GYRO_POWER_DEEP_SUSPEND_REG;
            ret = sensor_i2c_write(drv, BMI088_GYRO_MODE_LPM1_ADDR, &value,
                                   I2C_DATA_LEN, I2C_OP_RETRIES);
            if (unlikely(ret)) {
                return ret;
            }
        } break;

        case DEV_SLEEP: {
            if ((value & BMI088_GYRO_POWER_BIT_MASK) == BMI088_GYRO_POWER_SLEEP_REG) {
                return 0;
            }

            value &= (~BMI088_GYRO_POWER_BIT_MASK);
            value |= BMI088_GYRO_POWER_SLEEP_REG;
            ret = sensor_i2c_write(drv, BMI088_GYRO_MODE_LPM1_ADDR, &value,
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

static int drv_gyro_bosch_bmi088_set_odr(i2c_dev_t *drv, uint32_t odr)
{
    int     ret   = 0;
    uint8_t value = 0x00;

    ret = sensor_i2c_read(drv, BMI088_GYRO_BW_ADDR, &value, I2C_DATA_LEN,
                          I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    if (odr >= BMI088_GYRO_ODR_2000) {
        value &= BMI088_GYRO_BW_ADDR_MSK;
        value |= BMI088_GYRO_ODR_2000_REG;
    } else if (odr >= BMI088_GYRO_ODR_1000) {
        value &= BMI088_GYRO_BW_ADDR_MSK;
        value |= BMI088_GYRO_ODR_1000_REG;
    } else if (odr >= BMI088_GYRO_ODR_400) {
        value &= BMI088_GYRO_BW_ADDR_MSK;
        value |= BMI088_GYRO_ODR_400_REG;
    } else if (odr >= BMI088_GYRO_ODR_200) {
        value &= BMI088_GYRO_BW_ADDR_MSK;
        value |= BMI088_GYRO_ODR_200_REG;
    } else {
        value &= BMI088_GYRO_BW_ADDR_MSK;
        value |= BMI088_GYRO_ODR_100_REG;
    }

    ret = sensor_i2c_write(drv, BMI088_GYRO_BW_ADDR, &value, I2C_DATA_LEN,
                           I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }
    return 0;
}

static int drv_gyro_bosch_bmi088_set_range(i2c_dev_t *drv, uint32_t range)
{
    int     ret   = 0;
    uint8_t value = 0x00;

    ret = sensor_i2c_read(drv, BMI088_GYRO_RANGE_ADDR, &value, I2C_DATA_LEN,
                          I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    switch (range) {
        case GYRO_RANGE_125DPS: {
            value &= BMI088_GYRO_RANGE_ADDR_RANGE_MSK;
            value |= BMI088_GYRO_RANGE_RANGE_125DPS;
        } break;

        case GYRO_RANGE_250DPS: {
            value &= BMI088_GYRO_RANGE_ADDR_RANGE_MSK;
            value |= BMI088_GYRO_RANGE_RANGE_250DPS;
        } break;

        case GYRO_RANGE_500DPS: {
            value &= BMI088_GYRO_RANGE_ADDR_RANGE_MSK;
            value |= BMI088_GYRO_RANGE_RANGE_500DPS;
        } break;

        case GYRO_RANGE_1000DPS: {
            value &= BMI088_GYRO_RANGE_ADDR_RANGE_MSK;
            value |= BMI088_GYRO_RANGE_RANGE_1000DPS;
        } break;

        case GYRO_RANGE_2000DPS: {
            value &= BMI088_GYRO_RANGE_ADDR_RANGE_MSK;
            value |= BMI088_GYRO_RANGE_RANGE_2000DPS;
        } break;

        default:
            break;
    }

    ret = sensor_i2c_write(drv, BMI088_GYRO_RANGE_ADDR, &value, I2C_DATA_LEN,
                           I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    if ((range >= GYRO_RANGE_125DPS) && (range <= GYRO_RANGE_2000DPS)) {
        gyro_current_factor = bmi088_gyro_factor[range];
    }
    return 0;
}

static void drv_gyro_bosch_bmi088_irq_handle(void)
{
    /* no handle so far */
}

static int drv_gyro_bosch_bmi088_open(void)
{
    int ret = 0;
    ret     = drv_gyro_bosch_bmi088_set_power_mode(&bmi088_gyro_ctx, DEV_POWER_ON);
    if (unlikely(ret)) {
        return -1;
    }

    LOG("%s %s successfully \n", SENSOR_STR, __func__);
    return 0;
}

static int drv_gyro_bosch_bmi088_close(void)
{
    int ret = 0;
    ret     = drv_gyro_bosch_bmi088_set_power_mode(&bmi088_gyro_ctx, DEV_POWER_OFF);
    if (unlikely(ret)) {
        return -1;
    }
    return 0;
}

static int drv_gyro_bosch_bmi088_read(void *buf, size_t len)
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

    ret = sensor_i2c_read(&bmi088_gyro_ctx, BMI088_GYRO_RATE_X_LSB_ADDR, &reg[0],
                          I2C_REG_LEN, I2C_OP_RETRIES);
    ret |= sensor_i2c_read(&bmi088_gyro_ctx, BMI088_GYRO_RATE_X_MSB_ADDR, &reg[1],
                           I2C_REG_LEN, I2C_OP_RETRIES);
    ret |= sensor_i2c_read(&bmi088_gyro_ctx, BMI088_GYRO_RATE_Y_LSB_ADDR, &reg[2],
                           I2C_REG_LEN, I2C_OP_RETRIES);
    ret |= sensor_i2c_read(&bmi088_gyro_ctx, BMI088_GYRO_RATE_Y_MSB_ADDR, &reg[3],
                           I2C_REG_LEN, I2C_OP_RETRIES);
    ret |= sensor_i2c_read(&bmi088_gyro_ctx, BMI088_GYRO_RATE_Z_LSB_ADDR, &reg[4],
                           I2C_REG_LEN, I2C_OP_RETRIES);
    ret |= sensor_i2c_read(&bmi088_gyro_ctx, BMI088_GYRO_RATE_Z_MSB_ADDR, &reg[5],
                           I2C_REG_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return -1;
    }
    gyro->data[DATA_AXIS_X] = (int32_t)(
      (((int32_t)((int8_t)reg[1])) << BMI088_GYRO_SHIFT_EIGHT_BITS) | reg[0]);
    gyro->data[DATA_AXIS_Y] = (int32_t)(
      (((int32_t)((int8_t)reg[3])) << BMI088_GYRO_SHIFT_EIGHT_BITS) | reg[2]);
    gyro->data[DATA_AXIS_Z] = (int32_t)(
      (((int32_t)((int8_t)reg[5])) << BMI088_GYRO_SHIFT_EIGHT_BITS) | reg[4]);

    if (gyro_current_factor != 0) {
        // the unit of gyro is uDPS, 1000 000 uDPS = 1 DPS
        gyro->data[DATA_AXIS_X] = (int32_t)((int64_t)gyro->data[DATA_AXIS_X] * GYROSCOPE_UNIT_FACTOR * 10 / gyro_current_factor);
        gyro->data[DATA_AXIS_Y] = (int32_t)((int64_t)gyro->data[DATA_AXIS_Y] * GYROSCOPE_UNIT_FACTOR * 10 / gyro_current_factor);
        gyro->data[DATA_AXIS_Z] = (int32_t)((int64_t)gyro->data[DATA_AXIS_Z] * GYROSCOPE_UNIT_FACTOR * 10 / gyro_current_factor);
    }

    gyro->timestamp = aos_now_ms();
    return (int)size;
}

static int drv_gyro_bosch_bmi088_ioctl(int cmd, unsigned long arg)
{
    int ret = 0;

    switch (cmd) {
        case SENSOR_IOCTL_ODR_SET: {
            ret = drv_gyro_bosch_bmi088_set_odr(&bmi088_gyro_ctx, arg);
            if (unlikely(ret)) {
                return -1;
            }
        } break;
        case SENSOR_IOCTL_RANGE_SET: {
            ret = drv_gyro_bosch_bmi088_set_range(&bmi088_gyro_ctx, arg);
            if (unlikely(ret)) {
                return -1;
            }
        } break;
        case SENSOR_IOCTL_SET_POWER: {
            ret = drv_gyro_bosch_bmi088_set_power_mode(&bmi088_gyro_ctx, arg);
            if (unlikely(ret)) {
                return -1;
            }
        } break;
        case SENSOR_IOCTL_GET_INFO: {
            /* fill the dev info here */
            dev_sensor_info_t *info = (dev_sensor_info_t *)arg;
            info->model             = "BMI088";
            info->range_max         = 2000;
            info->range_min         = 125;
            info->unit              = udps;
        } break;

        default:
            break;
    }

    return 0;
}

int drv_gyro_bosch_bmi088_init(void)
{
    int          ret = 0;
    sensor_obj_t sensor;
    memset(&sensor, 0, sizeof(sensor));

    /* fill the sensor obj parameters here */
    sensor.io_port    = I2C_PORT;
    sensor.tag        = TAG_DEV_GYRO;
    sensor.path       = dev_gyro_path;
    sensor.open       = drv_gyro_bosch_bmi088_open;
    sensor.close      = drv_gyro_bosch_bmi088_close;
    sensor.read       = drv_gyro_bosch_bmi088_read;
    sensor.write      = NULL;
    sensor.ioctl      = drv_gyro_bosch_bmi088_ioctl;
    sensor.irq_handle = drv_gyro_bosch_bmi088_irq_handle;

    ret = sensor_create_obj(&sensor);
    if (unlikely(ret)) {
        return -1;
    }

    ret = drv_gyro_bosch_bmi088_validate_id(&bmi088_gyro_ctx, BMI088_GYRO_CHIP_ID_VALUE);
    if (unlikely(ret)) {
        return -1;
    }

    ret = drv_gyro_bosch_bmi088_set_range(&bmi088_gyro_ctx, GYRO_RANGE_2000DPS);
    if (unlikely(ret)) {
        return -1;
    }

    ret = drv_gyro_bosch_bmi088_set_odr(&bmi088_gyro_ctx, BMI088_GYRO_DEFAULT_ODR);
    if (unlikely(ret)) {
        return -1;
    }

    ret = drv_gyro_bosch_bmi088_set_power_mode(&bmi088_gyro_ctx, DEV_POWER_OFF);
    if (unlikely(ret)) {
        return -1;
    }

    /* update the phy sensor info to sensor hal */
    LOG("%s %s successfully \n", SENSOR_STR, __func__);
    return 0;
}

SENSOR_DRV_ADD(drv_acc_bosch_bmi088_init);
SENSOR_DRV_ADD(drv_gyro_bosch_bmi088_init);

