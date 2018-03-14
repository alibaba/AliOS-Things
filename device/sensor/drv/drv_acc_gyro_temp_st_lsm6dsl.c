/*
* Copyright (C) 2015-2017 Alibaba Group Holding Limited
*
*
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <aos/aos.h>
#include <vfs_conf.h>
#include <vfs_err.h>
#include <vfs_register.h>
#include <hal/base.h>
#include "k_api.h"
#include "common.h"
#include "hal/sensor.h"
#include "drv_acc_st_lsm6dsl.h"
#include "drv_gyro_st_lsm6dsl.h"
#include "drv_temp_st_lsm6dsl.h"

#define LSM6DSL_FUNC_CFG_ACCESS                         0x01
#define LSM6DSL_SENSOR_SYNC_TIME_FRAME                  0x04
#define LSM6DSL_SENSOR_SYNC_RES_RATIO                   0x05
#define LSM6DSL_FIFO_CTRL1                              0x06
#define LSM6DSL_FIFO_CTRL2                              0x07
#define LSM6DSL_FIFO_CTRL3                              0x08
#define LSM6DSL_FIFO_CTRL4                              0x09
#define LSM6DSL_FIFO_CTRL5                              0x0A
#define LSM6DSL_DRDY_PULSE_CFG_G                        0x0B
#define LSM6DSL_INT1_CTRL                               0x0D
#define LSM6DSL_INT2_CTRL                               0x0E
#define LSM6DSL_WHO_AM_I                                0x0F
#define LSM6DSL_CTRL1_XL                                0x10
#define LSM6DSL_CTRL2_G                                 0x11
#define LSM6DSL_CTRL3_C                                 0x12
#define LSM6DSL_CTRL4_C                                 0x13
#define LSM6DSL_CTRL5_C                                 0x14
#define LSM6DSL_CTRL6_C                                 0x15
#define LSM6DSL_CTRL7_G                                 0x16
#define LSM6DSL_CTRL8_XL                                0x17
#define LSM6DSL_CTRL9_XL                                0x18
#define LSM6DSL_CTRL10_C                                0x19
#define LSM6DSL_MASTER_CONFIG                           0x1A
#define LSM6DSL_WAKE_UP_SRC                             0x1B
#define LSM6DSL_TAP_SRC                                 0x1C
#define LSM6DSL_D6D_SRC                                 0x1D
#define LSM6DSL_STATUS_REG                              0x1E
#define LSM6DSL_OUT_TEMP_L                              0x20
#define LSM6DSL_OUT_TEMP_H                              0x21
#define LSM6DSL_OUTX_L_G                                0x22
#define LSM6DSL_OUTX_H_G                                0x23
#define LSM6DSL_OUTY_L_G                                0x24
#define LSM6DSL_OUTY_H_G                                0x25
#define LSM6DSL_OUTZ_L_G                                0x26
#define LSM6DSL_OUTZ_H_G                                0x27
#define LSM6DSL_OUTX_L_XL                               0x28
#define LSM6DSL_OUTX_H_XL                               0x29
#define LSM6DSL_OUTY_L_XL                               0x2A
#define LSM6DSL_OUTY_H_XL                               0x2B
#define LSM6DSL_OUTZ_L_XL                               0x2C
#define LSM6DSL_OUTZ_H_XL                               0x2D
#define LSM6DSL_SENSORHUB1_REG                          0x2E
#define LSM6DSL_SENSORHUB2_REG                          0x2F
#define LSM6DSL_SENSORHUB3_REG                          0x30
#define LSM6DSL_SENSORHUB4_REG                          0x31
#define LSM6DSL_SENSORHUB5_REG                          0x32
#define LSM6DSL_SENSORHUB6_REG                          0x33
#define LSM6DSL_SENSORHUB7_REG                          0x34
#define LSM6DSL_SENSORHUB8_REG                          0x35
#define LSM6DSL_SENSORHUB9_REG                          0x36
#define LSM6DSL_SENSORHUB10_REG                         0x37
#define LSM6DSL_SENSORHUB11_REG                         0x38
#define LSM6DSL_SENSORHUB12_REG                         0x39
#define LSM6DSL_FIFO_STATUS1                            0x3A
#define LSM6DSL_FIFO_STATUS2                            0x3B
#define LSM6DSL_FIFO_STATUS3                            0x3C
#define LSM6DSL_FIFO_STATUS4                            0x3D
#define LSM6DSL_FIFO_DATA_OUT_L                         0x3E
#define LSM6DSL_FIFO_DATA_OUT_H                         0x3F
#define LSM6DSL_TIMESTAMP0_REG                          0x40
#define LSM6DSL_TIMESTAMP1_REG                          0x41
#define LSM6DSL_TIMESTAMP2_REG                          0x42
#define LSM6DSL_STEP_TIMESTAMP_L                        0x49
#define LSM6DSL_STEP_TIMESTAMP_H                        0x4A
#define LSM6DSL_STEP_COUNTER_L                          0x4B
#define LSM6DSL_STEP_COUNTER_H                          0x4C
#define LSM6DSL_SENSORHUB13_REG                         0x4D
#define LSM6DSL_SENSORHUB14_REG                         0x4E
#define LSM6DSL_SENSORHUB15_REG                         0x4F
#define LSM6DSL_SENSORHUB16_REG                         0x50
#define LSM6DSL_SENSORHUB17_REG                         0x51
#define LSM6DSL_SENSORHUB18_REG                         0x52
#define LSM6DSL_FUNC_SRC1                               0x53
#define LSM6DSL_FUNC_SRC2                               0x54
#define LSM6DSL_WRIST_TILT_IA                           0x55
#define LSM6DSL_TAP_CFG                                 0x58
#define LSM6DSL_TAP_THS_6D                              0x59
#define LSM6DSL_INT_DUR2                                0x5A
#define LSM6DSL_WAKE_UP_THS                             0x5B
#define LSM6DSL_WAKE_UP_DUR                             0x5C
#define LSM6DSL_FREE_FALL                               0x5D
#define LSM6DSL_MD1_CFG                                 0x5E
#define LSM6DSL_MD2_CFG                                 0x5F
#define LSM6DSL_MASTER_CMD_CODE                         0x60
#define LSM6DSL_SENS_SYNC_SPI_ERROR_CODE                0x61
#define LSM6DSL_OUT_MAG_RAW_X_L                         0x66
#define LSM6DSL_OUT_MAG_RAW_X_H                         0x67
#define LSM6DSL_OUT_MAG_RAW_Y_L                         0x68
#define LSM6DSL_OUT_MAG_RAW_Y_H                         0x69
#define LSM6DSL_OUT_MAG_RAW_Z_L                         0x6A
#define LSM6DSL_OUT_MAG_RAW_Z_H                         0x6B
#define LSM6DSL_X_OFS_USR                               0x73
#define LSM6DSL_Y_OFS_USR                               0x74
#define LSM6DSL_Z_OFS_USR                               0x75

#define LSM6DSL_CTRL3_C_REG_SW_RESET__POS               (0)
#define LSM6DSL_CTRL3_C_REG_SW_RESET__MSK               (0x01)
#define LSM6DSL_CTRL3_C_REG_SW_RESET__REG               (LSM6DSL_CTRL3_C)

#define LSM6DSL_CTRL3_C_REG_BDU__POS                    (6)
#define LSM6DSL_CTRL3_C_REG_BDU__MSK                    (0x01)
#define LSM6DSL_CTRL3_C_REG_BDU__REG                    (LSM6DSL_CTRL3_C)

#define LSM6DSL_CTRL1_XL_REG_ODR_XL__POS                (4)
#define LSM6DSL_CTRL1_XL_REG_ODR_XL__MSK                (0xf0)
#define LSM6DSL_CTRL1_XL_REG_ODR_XL__REG                (LSM6DSL_CTRL1_XL)

#define LSM6DSL_CTRL1_XL_REG_FS_XL__POS                 (2)
#define LSM6DSL_CTRL1_XL_REG_FS_XL__MSK                 (0x0c)
#define LSM6DSL_CTRL1_XL_REG_FS_XL__REG                 (LSM6DSL_CTRL1_XL)

#define LSM6DSL_CTRL1_XL_REG_BW0_XL__POS                (0)
#define LSM6DSL_CTRL1_XL_REG_BW0_XL__MSK                (0x01)
#define LSM6DSL_CTRL1_XL_REG_BW0_XL__REG                (LSM6DSL_CTRL1_XL)

#define LSM6DSL_CTRL8_XL_REG_INPUT_COMPOSITE__POS       (3)
#define LSM6DSL_CTRL8_XL_REG_INPUT_COMPOSITE__MSK       (0x64)
#define LSM6DSL_CTRL8_XL_REG_INPUT_COMPOSITE__REG       (LSM6DSL_CTRL8_XL)

#define LSM6DSL_CTRL8_XL_REG_LPF2_XL_EN__POS            (7)
#define LSM6DSL_CTRL8_XL_REG_LPF2_XL_EN__MSK            (0x80)
#define LSM6DSL_CTRL8_XL_REG_LPF2_XL_EN__REG            (LSM6DSL_CTRL8_XL)

#define LSM6DSL_CTRL8_XL_REG_HP_SLOPE_XL_EN__POS        (2)
#define LSM6DSL_CTRL8_XL_REG_HP_SLOPE_XL_EN__MSK        (0x04)
#define LSM6DSL_CTRL8_XL_REG_HP_SLOPE_XL_EN__REG        (LSM6DSL_CTRL8_XL)

#define LSM6DSL_CTRL2_G_REG_ODR_G__POS                  (4)
#define LSM6DSL_CTRL2_G_REG_ODR_G__MSK                  (0xf0)
#define LSM6DSL_CTRL2_G_REG_ODR_G__REG                  (LSM6DSL_CTRL2_G)

#define LSM6DSL_CTRL2_G_REG_FS_G__POS                   (2)
#define LSM6DSL_CTRL2_G_REG_FS_G__MSK                   (0x0c)
#define LSM6DSL_CTRL2_G_REG_FS_G__REG                   (LSM6DSL_CTRL2_G)

#define LSM6DSL_CTRL7_G_REG_HPM_G__POS                  (4)
#define LSM6DSL_CTRL7_G_REG_HPM_G__MSK                  (0x30)
#define LSM6DSL_CTRL7_G_REG_HPM_G__REG                  (LSM6DSL_CTRL7_G)

#define LSM6DSL_CTRL7_G_REG_HP_EN_G__POS                (6)
#define LSM6DSL_CTRL7_G_REG_HP_EN_G__MSK                (0x40)
#define LSM6DSL_CTRL7_G_REG_HP_EN_G__REG                (LSM6DSL_CTRL7_G)

#define LSM6DSL_CTRL6_C_REG_FTYPE__POS                  (0)
#define LSM6DSL_CTRL6_C_REG_FTYPE__MSK                  (0x03)
#define LSM6DSL_CTRL6_C_REG_FTYPE__REG                  (LSM6DSL_CTRL6_C)

#define LSM6DSL_CTRL4_C_REG_LPF1_SEL_G__POS             (1)
#define LSM6DSL_CTRL4_C_REG_LPF1_SEL_G__MSK             (0x02)
#define LSM6DSL_CTRL4_C_REG_LPF1_SEL_G__REG             (LSM6DSL_CTRL4_C)

#define LSM6DSL_GET_BITSLICE(regvar, bitname)           ((regvar & bitname##__MSK) >> bitname##__POS)
#define LSM6DSL_SET_BITSLICE(regvar, bitname, val)      ((regvar & ~bitname##__MSK) | ((val<<bitname##__POS)&bitname##__MSK))

typedef enum {
    NORMAL_MODE = 0,
    RESET_MODE = 1,
} SW_Reset_mode;

typedef enum {
    CONTINUOUS_UPDATE = 0,
    UNTIL_OUTPUT_HAVE_BEEN_READ = 1,
} Block_Data_Update;

typedef enum {
    LSM6DSL_XL_ODR_OFF      =  0,
    LSM6DSL_XL_ODR_12Hz5    =  1,
    LSM6DSL_XL_ODR_26Hz     =  2,
    LSM6DSL_XL_ODR_52Hz     =  3,
    LSM6DSL_XL_ODR_104Hz    =  4,
    LSM6DSL_XL_ODR_208Hz    =  5,
    LSM6DSL_XL_ODR_416Hz    =  6,
    LSM6DSL_XL_ODR_833Hz    =  7,
    LSM6DSL_XL_ODR_1k66Hz   =  8,
    LSM6DSL_XL_ODR_3k33Hz   =  9,
    LSM6DSL_XL_ODR_6k66Hz   = 10,
    LSM6DSL_XL_ODR_1Hz6     = 11,
} lsm6dsl_odr_xl_t;

typedef enum {
    LSM6DSL_2g    = 0,
    LSM6DSL_16g   = 1,
    LSM6DSL_4g    = 2,
    LSM6DSL_8g    = 3,
} lsm6dsl_fs_xl_t;

typedef enum {
    LSM6DSL_XL_ANA_BW_1k5Hz = 0,
    LSM6DSL_XL_ANA_BW_400Hz = 1,
} lsm6dsl_bw0_xl_t;

typedef enum {
    LSM6DSL_XL_LOW_LAT_LP_ODR_DIV_50     = 0x0,
    LSM6DSL_XL_LOW_LAT_LP_ODR_DIV_100    = 0x4,
    LSM6DSL_XL_LOW_LAT_LP_ODR_DIV_9      = 0x8,
    LSM6DSL_XL_LOW_LAT_LP_ODR_DIV_400    = 0xc,
    LSM6DSL_XL_LOW_NOISE_LP_ODR_DIV_50   = 0x1,
    LSM6DSL_XL_LOW_NOISE_LP_ODR_DIV_100  = 0x5,
    LSM6DSL_XL_LOW_NOISE_LP_ODR_DIV_9    = 0x9,
    LSM6DSL_XL_LOW_NOISE_LP_ODR_DIV_400  = 0xd,
} lsm6dsl_input_composite_t;

typedef enum {
    LSM6DSL_GY_ODR_OFF    =  0,
    LSM6DSL_GY_ODR_12Hz5  =  1,
    LSM6DSL_GY_ODR_26Hz   =  2,
    LSM6DSL_GY_ODR_52Hz   =  3,
    LSM6DSL_GY_ODR_104Hz  =  4,
    LSM6DSL_GY_ODR_208Hz  =  5,
    LSM6DSL_GY_ODR_416Hz  =  6,
    LSM6DSL_GY_ODR_833Hz  =  7,
    LSM6DSL_GY_ODR_1k66Hz =  8,
    LSM6DSL_GY_ODR_3k33Hz =  9,
    LSM6DSL_GY_ODR_6k66Hz = 10,
} lsm6dsl_odr_g_t;

typedef enum {
    LSM6DSL_250dps   = 0,
    LSM6DSL_125dps   = 1,
    LSM6DSL_500dps   = 2,
    LSM6DSL_1000dps  = 4,
    LSM6DSL_2000dps  = 6,
} lsm6dsl_fs_g_t;

typedef enum {
    LSM6DSL_LP2_ONLY                    = 0x00,

    LSM6DSL_HP_16mHz_LP2                = 0x80,
    LSM6DSL_HP_65mHz_LP2                = 0x90,
    LSM6DSL_HP_260mHz_LP2               = 0xA0,
    LSM6DSL_HP_1Hz04_LP2                = 0xB0,

    LSM6DSL_HP_DISABLE_LP1_LIGHT        = 0x0A,
    LSM6DSL_HP_DISABLE_LP1_NORMAL       = 0x09,
    LSM6DSL_HP_DISABLE_LP_STRONG        = 0x08,
    LSM6DSL_HP_DISABLE_LP1_AGGRESSIVE   = 0x0B,

    LSM6DSL_HP_16mHz_LP1_LIGHT          = 0x8A,
    LSM6DSL_HP_65mHz_LP1_NORMAL         = 0x99,
    LSM6DSL_HP_260mHz_LP1_STRONG        = 0xA8,
    LSM6DSL_HP_1Hz04_LP1_AGGRESSIVE     = 0xBB,
} lsm6dsl_lpf1_sel_g_t;

#define LSM6DSL_FROM_FS_2g_TO_mg(lsb)                   (lsb *  61) / 1000
#define LSM6DSL_FROM_FS_4g_TO_mg(lsb)                   (lsb * 122) / 1000
#define LSM6DSL_FROM_FS_8g_TO_mg(lsb)                   (lsb * 244) / 1000
#define LSM6DSL_FROM_FS_16g_TO_mg(lsb)                  (lsb * 488) / 1000

#define LSM6DSL_FROM_FS_125dps_TO_udps(lsb)             (lsb * 4375)
#define LSM6DSL_FROM_FS_250dps_TO_udps(lsb)             (lsb *  875)
#define LSM6DSL_FROM_FS_500dps_TO_udps(lsb)             (lsb * 1750)
#define LSM6DSL_FROM_FS_1000dps_TO_udps(lsb)            (lsb *   35)
#define LSM6DSL_FROM_FS_2000dps_TO_udps(lsb)            (lsb *   70)

#define LSM6DSL_FROM_LSB_TO_degC(lsb)                   (((int16_t)lsb >> 8) + 25)

#define LSM6DSL_SWRESET_TIMEOUT                         10

#define LSM6DSL_ACC_DATA_SIZE                           6
#define LSM6DSL_GYRO_DATA_SIZE                          6
#define LSM6DSL_TEMP_DATA_SIZE                          2

#define LSM6DSL_ID                                      0x6a

#define LSM6DSL_I2C_SLAVE_ADDR_LOW                      0x6a
#define LSM6DSL_I2C_SLAVE_ADDR_HIGH                     0x6b
#define LSM6DSL_ADDR_TRANS(n)                           ((n)<<1)

typedef enum {
    FLAG_INIT_ACC = 0,
    FLAG_INIT_GYRO,
    FLAG_INIT_TEMP,
} FLAG_INIT_BIT;

i2c_dev_t lsm6dsl_ctx = {
    .config.address_width = 7,
    .config.freq = 100000,
};

static uint8_t g_new_data = 0;
static uint8_t g_init_bitwise = 0;

static int drv_acc_gyro_temp_st_lsm6dsl_validate_id(i2c_dev_t* drv, uint8_t id_value)
{
    int     ret = 0;
    uint8_t value = 0;

    if(drv == NULL){
        return -1;
    }

    ret = sensor_i2c_read(drv, LSM6DSL_WHO_AM_I, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)) {
        return ret;
    }

    if (id_value != value){
        return -1;
    }

    return 0;
}

static int lsm6dsl_sw_reset(i2c_dev_t* drv, uint32_t timeout)
{
    int     ret = 0;
    uint8_t value = 0;
    uint32_t wait_time = 0;

    if(drv == NULL){
        return -1;
    }
    
    ret = sensor_i2c_read(drv, LSM6DSL_CTRL3_C, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)) {
        return ret;
    }
    
    value = LSM6DSL_SET_BITSLICE(value, LSM6DSL_CTRL3_C_REG_SW_RESET, RESET_MODE);

    ret = sensor_i2c_write(drv, LSM6DSL_CTRL3_C, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)) {
        return ret;
    }
    
    do {
        krhino_task_sleep(krhino_ms_to_ticks(10));
        wait_time += 10;
        ret = sensor_i2c_read(drv, LSM6DSL_CTRL3_C, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
        if(unlikely(ret)) {
            return ret;
        }
    } while (LSM6DSL_GET_BITSLICE(value, LSM6DSL_CTRL3_C_REG_SW_RESET) != NORMAL_MODE && wait_time < timeout);

    return 0;
}

static int lsm6dsl_block_data_update_set(i2c_dev_t* drv)
{
    int     ret = 0;
    uint8_t value = 0;

    if(drv == NULL){
        return -1;
    }
    
    ret = sensor_i2c_read(drv, LSM6DSL_CTRL3_C, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)) {
        return ret;
    }
    
    value = LSM6DSL_SET_BITSLICE(value, LSM6DSL_CTRL3_C_REG_BDU, UNTIL_OUTPUT_HAVE_BEEN_READ);
    
    ret = sensor_i2c_write(drv, LSM6DSL_CTRL3_C, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)) {
        return ret;
    }
    
    return 0;
}

static int lsm6dsl_gy_band_pass_set(i2c_dev_t* drv, lsm6dsl_lpf1_sel_g_t val)
{
    int     ret = 0;
    uint8_t value = 0;

    if(drv == NULL){
        return -1;
    }
    
    ret = sensor_i2c_read(drv, LSM6DSL_CTRL7_G, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)) {
        return ret;
    }
    
    value = LSM6DSL_SET_BITSLICE(value, LSM6DSL_CTRL7_G_REG_HPM_G, (val & 0x30) >> 4);
    value = LSM6DSL_SET_BITSLICE(value, LSM6DSL_CTRL7_G_REG_HP_EN_G, (val & 0x80) >> 7);
    
    ret = sensor_i2c_write(drv, LSM6DSL_CTRL7_G, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)) {
        return ret;
    }
    
    ret = sensor_i2c_read(drv, LSM6DSL_CTRL6_C, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)) {
        return ret;
    }
    
    value = LSM6DSL_SET_BITSLICE(value, LSM6DSL_CTRL6_C_REG_FTYPE, val & 0x03);
    
    ret = sensor_i2c_write(drv, LSM6DSL_CTRL6_C, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)) {
        return ret;
    }
    
    ret = sensor_i2c_read(drv, LSM6DSL_CTRL4_C, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)) {
        return ret;
    }
    
    value = LSM6DSL_SET_BITSLICE(value, LSM6DSL_CTRL4_C_REG_LPF1_SEL_G, (val & 0x08) >> 3);
    
    ret = sensor_i2c_write(drv, LSM6DSL_CTRL4_C, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)) {
        return ret;
    }
    
    return 0;
}

static int lsm6dsl_is_new_data_available(i2c_dev_t *drv, FLAG_INIT_BIT bit_wise)
{
    int     ret = 0;
    uint8_t value = 0;
    
    if (g_new_data & 1 << bit_wise)
        return 1;
    
    ret = sensor_i2c_read(drv, LSM6DSL_STATUS_REG, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)) {
        return 0;
    }
    
    g_new_data = value;
    
    return (value & 1 << bit_wise) ? 1 : 0;
}

static void lsm6dsl_clear_new_data_available(FLAG_INIT_BIT bit_wise)
{
    g_new_data &= ~(1 << bit_wise);
}

static int drv_acc_st_lsm6dsl_set_default_config(i2c_dev_t *drv)
{
    int     ret = 0;
    uint8_t value = 0;
    
    if(drv == NULL){
        return -1;
    }
    
    ret = sensor_i2c_read(drv, LSM6DSL_CTRL1_XL, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)) {
        return ret;
    }
    
    value = LSM6DSL_SET_BITSLICE(value, LSM6DSL_CTRL1_XL_REG_ODR_XL, LSM6DSL_XL_ODR_12Hz5);
    value = LSM6DSL_SET_BITSLICE(value, LSM6DSL_CTRL1_XL_REG_FS_XL, LSM6DSL_2g);
    value = LSM6DSL_SET_BITSLICE(value, LSM6DSL_CTRL1_XL_REG_BW0_XL, LSM6DSL_XL_ANA_BW_400Hz);
    
    ret = sensor_i2c_write(drv, LSM6DSL_CTRL1_XL, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)) {
        return ret;
    }
    
    ret = sensor_i2c_read(drv, LSM6DSL_CTRL8_XL, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)) {
        return ret;
    }
    
    value = LSM6DSL_SET_BITSLICE(value, LSM6DSL_CTRL8_XL_REG_INPUT_COMPOSITE, LSM6DSL_XL_LOW_NOISE_LP_ODR_DIV_100);
    value = LSM6DSL_SET_BITSLICE(value, LSM6DSL_CTRL8_XL_REG_LPF2_XL_EN, 1);
    value = LSM6DSL_SET_BITSLICE(value, LSM6DSL_CTRL8_XL_REG_HP_SLOPE_XL_EN, 0);
    
    ret = sensor_i2c_write(drv, LSM6DSL_CTRL8_XL, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)) {
        return ret;
    }
    
    return 0;
}

static int drv_gyro_st_lsm6dsl_set_default_config(i2c_dev_t *drv)
{
    int     ret = 0;
    uint8_t value = 0;
    
    ret = sensor_i2c_read(drv, LSM6DSL_CTRL2_G, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)) {
        return ret;
    }
    
    value = LSM6DSL_SET_BITSLICE(value, LSM6DSL_CTRL2_G_REG_ODR_G, LSM6DSL_GY_ODR_12Hz5);
    value = LSM6DSL_SET_BITSLICE(value, LSM6DSL_CTRL2_G_REG_FS_G, LSM6DSL_2000dps);
    
    ret = sensor_i2c_write(drv, LSM6DSL_CTRL2_G, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)) {
        return ret;
    }
    
    ret = lsm6dsl_gy_band_pass_set(drv, LSM6DSL_HP_260mHz_LP1_STRONG);
    if(unlikely(ret)) {
        return ret;
    }
    
    return 0;
}

static void drv_acc_st_lsm6dsl_irq_handle(void)
{
    /* no handle so far */
}

static int drv_acc_st_lsm6dsl_open(void)
{
    lsm6dsl_clear_new_data_available(FLAG_INIT_ACC);
    LOG("%s %s successfully \n", SENSOR_STR, __func__);
    return 0;
}

static int drv_acc_st_lsm6dsl_close(void)
{
    LOG("%s %s successfully \n", SENSOR_STR, __func__);
    return 0;
}

static int drv_acc_st_lsm6dsl_read(void *buf, size_t len)
{
    int ret = 0;
    uint8_t acc_raw[LSM6DSL_ACC_DATA_SIZE] = {0};
    accel_data_t* pdata = (accel_data_t*)buf;
    
    if(buf == NULL || !lsm6dsl_is_new_data_available(&lsm6dsl_ctx, FLAG_INIT_ACC)){
        return -1;
    }
    
    ret = sensor_i2c_read(&lsm6dsl_ctx, LSM6DSL_OUTX_L_XL, acc_raw, LSM6DSL_ACC_DATA_SIZE, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }
    
    pdata->data[0] = LSM6DSL_FROM_FS_2g_TO_mg((int32_t)(int16_t)(acc_raw[1] << 8 | acc_raw[0]));
    pdata->data[1] = LSM6DSL_FROM_FS_2g_TO_mg((int32_t)(int16_t)(acc_raw[3] << 8 | acc_raw[2]));
    pdata->data[2] = LSM6DSL_FROM_FS_2g_TO_mg((int32_t)(int16_t)(acc_raw[5] << 8 | acc_raw[4]));
    
    pdata->timestamp = aos_now_ms();
    len = sizeof(accel_data_t);
    
    lsm6dsl_clear_new_data_available(FLAG_INIT_ACC);
    
    return 0;
}

static int drv_acc_st_lsm6dsl_write(const void *buf, size_t len)
{
    (void)buf;
    (void)len;
    return 0;
}

static int drv_acc_st_lsm6dsl_ioctl(int cmd, unsigned long arg)
{
    switch (cmd) {
        case SENSOR_IOCTL_GET_INFO:
            /* fill the dev info here */
            dev_sensor_info_t *info = (dev_sensor_info_t *)arg;
            info->model = "LSM6DSL";
            info->unit = mg;
            break;
       default:
            return -1;
    }

    LOG("%s %s successfully \n", SENSOR_STR, __func__);
    return 0;
}

int drv_acc_st_lsm6dsl_init(i2c_dev_t *i2c_dev, ACC_ST_LSM6DSL_ADDR_SEL i2c_addr_sel)
{
    int ret = 0;
    sensor_obj_t sensor_acc;
    uint16_t addr_val = 0;

    if (i2c_dev == NULL) {
        return -1;
    }
    
    if (!g_init_bitwise) {
        switch (i2c_addr_sel) {
            case ACC_ST_LSM6DSL_ADDR_LOW:
                addr_val = LSM6DSL_I2C_SLAVE_ADDR_LOW;
                break;
            case ACC_ST_LSM6DSL_ADDR_HIGH:
                addr_val = LSM6DSL_I2C_SLAVE_ADDR_HIGH;
                break;
            default:
                return -1;
        }
        
        lsm6dsl_ctx.port = i2c_dev->port;
        lsm6dsl_ctx.priv = i2c_dev->priv;
        lsm6dsl_ctx.config.dev_addr = LSM6DSL_ADDR_TRANS(addr_val);
    
        ret = drv_acc_gyro_temp_st_lsm6dsl_validate_id(&lsm6dsl_ctx, LSM6DSL_ID);
        if(unlikely(ret)) {
            return -1;
        }
        
        ret = lsm6dsl_sw_reset(&lsm6dsl_ctx, LSM6DSL_SWRESET_TIMEOUT);
        if(unlikely(ret)) {
            return -1;
        }
        
        ret = lsm6dsl_block_data_update_set(&lsm6dsl_ctx);
        if(unlikely(ret)) {
            return -1;
        }
    }

    if (!(g_init_bitwise & (1 << FLAG_INIT_ACC))) {
        /* fill the sensor_acc obj parameters here */
        sensor_acc.tag = TAG_DEV_ACC;
        sensor_acc.path = dev_acc_path;
        sensor_acc.io_port = I2C_PORT;
        sensor_acc.open = drv_acc_st_lsm6dsl_open;
        sensor_acc.close = drv_acc_st_lsm6dsl_close;
        sensor_acc.read = drv_acc_st_lsm6dsl_read;
        sensor_acc.write = drv_acc_st_lsm6dsl_write;
        sensor_acc.ioctl = drv_acc_st_lsm6dsl_ioctl;
        sensor_acc.irq_handle = drv_acc_st_lsm6dsl_irq_handle;
        sensor_acc.bus = &lsm6dsl_ctx;

        ret = sensor_create_obj(&sensor_acc);
        if(unlikely(ret)) {
            return -1;
        }

        ret = drv_acc_st_lsm6dsl_set_default_config(&lsm6dsl_ctx);
        if(unlikely(ret)) {
            return -1;
        }

        g_init_bitwise |= 1 << FLAG_INIT_ACC;
    }

    LOG("%s %s successfully \n", SENSOR_STR, __func__);
    return 0;
}

static void drv_gyro_st_lsm6dsl_irq_handle(void)
{
    /* no handle so far */
}

static int drv_gyro_st_lsm6dsl_open(void)
{
    lsm6dsl_clear_new_data_available(FLAG_INIT_GYRO);
    LOG("%s %s successfully \n", SENSOR_STR, __func__);
    return 0;
}

static int drv_gyro_st_lsm6dsl_close(void)
{
    LOG("%s %s successfully \n", SENSOR_STR, __func__);
    return 0;
}

static int drv_gyro_st_lsm6dsl_read(void *buf, size_t len)
{
    int ret = 0;
    uint8_t gyro_raw[LSM6DSL_GYRO_DATA_SIZE] = {0};
    gyro_data_t* pdata = (gyro_data_t*)buf;
    
    if(buf == NULL || !lsm6dsl_is_new_data_available(&lsm6dsl_ctx, FLAG_INIT_GYRO)){
        return -1;
    }
    
    ret = sensor_i2c_read(&lsm6dsl_ctx, LSM6DSL_OUTX_L_G, gyro_raw, LSM6DSL_GYRO_DATA_SIZE, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }
    
    pdata->data[0] = LSM6DSL_FROM_FS_2000dps_TO_udps((int32_t)(int16_t)(gyro_raw[1] << 8 | gyro_raw[0]));
    pdata->data[1] = LSM6DSL_FROM_FS_2000dps_TO_udps((int32_t)(int16_t)(gyro_raw[3] << 8 | gyro_raw[2]));
    pdata->data[2] = LSM6DSL_FROM_FS_2000dps_TO_udps((int32_t)(int16_t)(gyro_raw[5] << 8 | gyro_raw[4]));
    
    pdata->timestamp = aos_now_ms();
    len = sizeof(gyro_data_t);
    
    lsm6dsl_clear_new_data_available(FLAG_INIT_GYRO);
    
    return 0;
}

static int drv_gyro_st_lsm6dsl_write(const void *buf, size_t len)
{
    (void)buf;
    (void)len;
    return 0;
}

static int drv_gyro_st_lsm6dsl_ioctl(int cmd, unsigned long arg)
{
    switch (cmd) {
        case SENSOR_IOCTL_GET_INFO:
            /* fill the dev info here */
            dev_sensor_info_t *info = (dev_sensor_info_t *)arg;
            info->model = "LSM6DSL";
            info->unit = udps;
            break;
       default:
            return -1;
    }

    LOG("%s %s successfully \n", SENSOR_STR, __func__);
    return 0;
}

int drv_gyro_st_lsm6dsl_init(i2c_dev_t *i2c_dev, GYRO_ST_LSM6DSL_ADDR_SEL i2c_addr_sel)
{
    int ret = 0;
    sensor_obj_t sensor_gyro;
    uint16_t addr_val = 0;

    if (i2c_dev == NULL) {
        return -1;
    }
    
    if (!g_init_bitwise) {
        switch (i2c_addr_sel) {
            case GYRO_ST_LSM6DSL_ADDR_LOW:
                addr_val = LSM6DSL_I2C_SLAVE_ADDR_LOW;
                break;
            case GYRO_ST_LSM6DSL_ADDR_HIGH:
                addr_val = LSM6DSL_I2C_SLAVE_ADDR_HIGH;
                break;
            default:
                return -1;
        }
        
        lsm6dsl_ctx.port = i2c_dev->port;
        lsm6dsl_ctx.priv = i2c_dev->priv;
        lsm6dsl_ctx.config.dev_addr = LSM6DSL_ADDR_TRANS(addr_val);
    
        ret = drv_acc_gyro_temp_st_lsm6dsl_validate_id(&lsm6dsl_ctx, LSM6DSL_ID);
        if(unlikely(ret)) {
            return -1;
        }
        
        ret = lsm6dsl_sw_reset(&lsm6dsl_ctx, LSM6DSL_SWRESET_TIMEOUT);
        if(unlikely(ret)) {
            return -1;
        }
        
        ret = lsm6dsl_block_data_update_set(&lsm6dsl_ctx);
        if(unlikely(ret)) {
            return -1;
        }
    }

    if (!(g_init_bitwise & (1 << FLAG_INIT_GYRO))) {
        /* fill the sensor_gyro obj parameters here */
        sensor_gyro.tag = TAG_DEV_GYRO;
        sensor_gyro.path = dev_gyro_path;
        sensor_gyro.io_port = I2C_PORT;
        sensor_gyro.open = drv_gyro_st_lsm6dsl_open;
        sensor_gyro.close = drv_gyro_st_lsm6dsl_close;
        sensor_gyro.read = drv_gyro_st_lsm6dsl_read;
        sensor_gyro.write = drv_gyro_st_lsm6dsl_write;
        sensor_gyro.ioctl = drv_gyro_st_lsm6dsl_ioctl;
        sensor_gyro.irq_handle = drv_gyro_st_lsm6dsl_irq_handle;
        sensor_gyro.bus = &lsm6dsl_ctx;

        ret = sensor_create_obj(&sensor_gyro);
        if(unlikely(ret)) {
            return -1;
        }

        ret = drv_gyro_st_lsm6dsl_set_default_config(&lsm6dsl_ctx);
        if(unlikely(ret)) {
            return -1;
        }

        g_init_bitwise |= 1 << FLAG_INIT_GYRO;
    }

    LOG("%s %s successfully \n", SENSOR_STR, __func__);
    return 0;
}

static void drv_temp_st_lsm6dsl_irq_handle(void)
{
    /* no handle so far */
}

static int drv_temp_st_lsm6dsl_open(void)
{
    lsm6dsl_clear_new_data_available(FLAG_INIT_TEMP);
    LOG("%s %s successfully \n", SENSOR_STR, __func__);
    return 0;
}

static int drv_temp_st_lsm6dsl_close(void)
{
    LOG("%s %s successfully \n", SENSOR_STR, __func__);
    return 0;
}

static int drv_temp_st_lsm6dsl_read(void *buf, size_t len)
{
    int ret = 0;
    uint8_t temp_raw[LSM6DSL_TEMP_DATA_SIZE] = {0};
    temperature_data_t* pdata = (temperature_data_t*)buf;
    
    if(buf == NULL || !lsm6dsl_is_new_data_available(&lsm6dsl_ctx, FLAG_INIT_TEMP)){
        return -1;
    }
    
    ret = sensor_i2c_read(&lsm6dsl_ctx, LSM6DSL_OUT_TEMP_L, temp_raw, LSM6DSL_TEMP_DATA_SIZE, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }
    
    pdata->t = LSM6DSL_FROM_LSB_TO_degC((int32_t)(int16_t)(temp_raw[1] << 8 | temp_raw[0]));
    
    pdata->timestamp = aos_now_ms();
    len = sizeof(temperature_data_t);
    
    lsm6dsl_clear_new_data_available(FLAG_INIT_TEMP);
    
    return 0;
}

static int drv_temp_st_lsm6dsl_write(const void *buf, size_t len)
{
    (void)buf;
    (void)len;
    return 0;
}

static int drv_temp_st_lsm6dsl_ioctl(int cmd, unsigned long arg)
{
    switch (cmd) {
        case SENSOR_IOCTL_GET_INFO:
            /* fill the dev info here */
            dev_sensor_info_t *info = (dev_sensor_info_t *)arg;
            info->model = "LSM6DSL";
            info->unit = centigrade;
            break;
       default:
            return -1;
    }

    LOG("%s %s successfully \n", SENSOR_STR, __func__);
    return 0;
}

int drv_temp_st_lsm6dsl_init(i2c_dev_t *i2c_dev, TEMP_ST_LSM6DSL_ADDR_SEL i2c_addr_sel)
{
    int ret = 0;
    sensor_obj_t sensor_temp;
    uint16_t addr_val = 0;

    if (i2c_dev == NULL) {
        return -1;
    }
    
    if (!g_init_bitwise) {
        switch (i2c_addr_sel) {
            case TEMP_ST_LSM6DSL_ADDR_LOW:
                addr_val = LSM6DSL_I2C_SLAVE_ADDR_LOW;
                break;
            case TEMP_ST_LSM6DSL_ADDR_HIGH:
                addr_val = LSM6DSL_I2C_SLAVE_ADDR_HIGH;
                break;
            default:
                return -1;
        }
        
        lsm6dsl_ctx.port = i2c_dev->port;
        lsm6dsl_ctx.priv = i2c_dev->priv;
        lsm6dsl_ctx.config.dev_addr = LSM6DSL_ADDR_TRANS(addr_val);
    
        ret = drv_acc_gyro_temp_st_lsm6dsl_validate_id(&lsm6dsl_ctx, LSM6DSL_ID);
        if(unlikely(ret)) {
            return -1;
        }
        
        ret = lsm6dsl_sw_reset(&lsm6dsl_ctx, LSM6DSL_SWRESET_TIMEOUT);
        if(unlikely(ret)) {
            return -1;
        }
        
        ret = lsm6dsl_block_data_update_set(&lsm6dsl_ctx);
        if(unlikely(ret)) {
            return -1;
        }
    }

    if (!(g_init_bitwise & (1 << FLAG_INIT_TEMP))) {
        /* fill the sensor_temp obj parameters here */
        sensor_temp.tag = TAG_DEV_TEMP;
        sensor_temp.path = dev_temp_path;
        sensor_temp.io_port = I2C_PORT;
        sensor_temp.open = drv_temp_st_lsm6dsl_open;
        sensor_temp.close = drv_temp_st_lsm6dsl_close;
        sensor_temp.read = drv_temp_st_lsm6dsl_read;
        sensor_temp.write = drv_temp_st_lsm6dsl_write;
        sensor_temp.ioctl = drv_temp_st_lsm6dsl_ioctl;
        sensor_temp.irq_handle = drv_temp_st_lsm6dsl_irq_handle;
        sensor_temp.bus = &lsm6dsl_ctx;

        ret = sensor_create_obj(&sensor_temp);
        if(unlikely(ret)) {
            return -1;
        }

        g_init_bitwise |= 1 << FLAG_INIT_TEMP;
    }

    LOG("%s %s successfully \n", SENSOR_STR, __func__);
    return 0;
}





