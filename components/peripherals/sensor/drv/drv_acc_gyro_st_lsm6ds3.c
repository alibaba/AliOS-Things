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


#define LSM6DS3_I2C_ADDR1                   (0x6A)
#define LSM6DS3_I2C_ADDR2                   (0x6B)
#define LSM6DS3_I2C_ADDR_TRANS(n)           ((n)<<1)  
#define LSM6DS3_I2C_ADDR                    LSM6DS3_I2C_ADDR_TRANS(LSM6DS3_I2C_ADDR2)

#define LSM6DS3_ACC_GYRO_FUNC_CFG_ACCESS    0x01
#define LSM6DS3_ACC_GYRO_SENSOR_SYNC_TIME   0x04
#define LSM6DS3_ACC_GYRO_SENSOR_RES_RATIO   0x05
#define LSM6DS3_ACC_GYRO_FIFO_CTRL1         0x06
#define LSM6DS3_ACC_GYRO_FIFO_CTRL2         0x07
#define LSM6DS3_ACC_GYRO_FIFO_CTRL3         0x08
#define LSM6DS3_ACC_GYRO_FIFO_CTRL4         0x09
#define LSM6DS3_ACC_GYRO_FIFO_CTRL5         0x0A
#define LSM6DS3_ACC_GYRO_DRDY_PULSE_CFG_G   0x0B
#define LSM6DS3_ACC_GYRO_INT1_CTRL          0x0D
#define LSM6DS3_ACC_GYRO_INT2_CTRL          0x0E
#define LSM6DS3_ACC_GYRO_WHO_AM_I_REG       0x0F
#define LSM6DS3_ACC_GYRO_CTRL1_XL           0x10
#define LSM6DS3_ACC_GYRO_CTRL2_G            0x11
#define LSM6DS3_ACC_GYRO_CTRL3_C            0x12
#define LSM6DS3_ACC_GYRO_CTRL4_C            0x13
#define LSM6DS3_ACC_GYRO_CTRL5_C            0x14
#define LSM6DS3_ACC_GYRO_CTRL6_C            0x15
#define LSM6DS3_ACC_GYRO_CTRL7_G            0x16
#define LSM6DS3_ACC_GYRO_CTRL8_XL           0x17
#define LSM6DS3_ACC_GYRO_CTRL9_XL           0x18
#define LSM6DS3_ACC_GYRO_CTRL10_C           0x19


#define LSM6DS3_ACC_GYRO_MASTER_CONFIG      0x1A
#define LSM6DS3_ACC_GYRO_WAKE_UP_SRC        0x1B
#define LSM6DS3_ACC_GYRO_TAP_SRC            0x1C
#define LSM6DS3_ACC_GYRO_D6D_SRC            0x1D
#define LSM6DS3_ACC_GYRO_STATUS_REG         0x1E

#define LSM6DS3_ACC_GYRO_OUT_TEMP_L         0x20
#define LSM6DS3_ACC_GYRO_OUT_TEMP_H         0x21
#define LSM6DS3_ACC_GYRO_OUTX_L_G           0x22
#define LSM6DS3_ACC_GYRO_OUTX_H_G           0x23
#define LSM6DS3_ACC_GYRO_OUTY_L_G           0x24
#define LSM6DS3_ACC_GYRO_OUTY_H_G           0x25
#define LSM6DS3_ACC_GYRO_OUTZ_L_G           0x26
#define LSM6DS3_ACC_GYRO_OUTZ_H_G           0x27
#define LSM6DS3_ACC_GYRO_OUTX_L_XL          0x28
#define LSM6DS3_ACC_GYRO_OUTX_H_XL          0x29
#define LSM6DS3_ACC_GYRO_OUTY_L_XL          0x2A
#define LSM6DS3_ACC_GYRO_OUTY_H_XL          0x2B
#define LSM6DS3_ACC_GYRO_OUTZ_L_XL          0x2C
#define LSM6DS3_ACC_GYRO_OUTZ_H_XL          0x2D
#define LSM6DS3_ACC_GYRO_SENSORHUB1_REG     0x2E
#define LSM6DS3_ACC_GYRO_SENSORHUB2_REG     0x2F
#define LSM6DS3_ACC_GYRO_SENSORHUB3_REG     0x30
#define LSM6DS3_ACC_GYRO_SENSORHUB4_REG     0x31
#define LSM6DS3_ACC_GYRO_SENSORHUB5_REG     0x32
#define LSM6DS3_ACC_GYRO_SENSORHUB6_REG     0x33
#define LSM6DS3_ACC_GYRO_SENSORHUB7_REG     0x34
#define LSM6DS3_ACC_GYRO_SENSORHUB8_REG     0x35
#define LSM6DS3_ACC_GYRO_SENSORHUB9_REG     0x36
#define LSM6DS3_ACC_GYRO_SENSORHUB10_REG    0x37
#define LSM6DS3_ACC_GYRO_SENSORHUB11_REG    0x38
#define LSM6DS3_ACC_GYRO_SENSORHUB12_REG    0x39
#define LSM6DS3_ACC_GYRO_FIFO_STATUS1       0x3A
#define LSM6DS3_ACC_GYRO_FIFO_STATUS2       0x3B
#define LSM6DS3_ACC_GYRO_FIFO_STATUS3       0x3C
#define LSM6DS3_ACC_GYRO_FIFO_STATUS4       0x3D
#define LSM6DS3_ACC_GYRO_FIFO_DATA_OUT_L    0x3E
#define LSM6DS3_ACC_GYRO_FIFO_DATA_OUT_H    0x3F
#define LSM6DS3_ACC_GYRO_TIMESTAMP0_REG     0x40
#define LSM6DS3_ACC_GYRO_TIMESTAMP1_REG     0x41
#define LSM6DS3_ACC_GYRO_TIMESTAMP2_REG     0x42

#define LSM6DS3_ACC_GYRO_TIMESTAMP_L        0x49
#define LSM6DS3_ACC_GYRO_TIMESTAMP_H        0x4A

#define LSM6DS3_ACC_GYRO_STEP_COUNTER_L     0x4B
#define LSM6DS3_ACC_GYRO_STEP_COUNTER_H     0x4C

#define LSM6DS3_ACC_GYRO_SENSORHUB13_REG    0x4D
#define LSM6DS3_ACC_GYRO_SENSORHUB14_REG    0x4E
#define LSM6DS3_ACC_GYRO_SENSORHUB15_REG    0x4F
#define LSM6DS3_ACC_GYRO_SENSORHUB16_REG    0x50
#define LSM6DS3_ACC_GYRO_SENSORHUB17_REG    0x51
#define LSM6DS3_ACC_GYRO_SENSORHUB18_REG    0x52

#define LSM6DS3_ACC_GYRO_FUNC_SRC           0x53
#define LSM6DS3_ACC_GYRO_TAP_CFG1           0x58
#define LSM6DS3_ACC_GYRO_TAP_THS_6D         0x59
#define LSM6DS3_ACC_GYRO_INT_DUR2           0x5A
#define LSM6DS3_ACC_GYRO_WAKE_UP_THS        0x5B
#define LSM6DS3_ACC_GYRO_WAKE_UP_DUR        0x5C
#define LSM6DS3_ACC_GYRO_FREE_FALL          0x5D
#define LSM6DS3_ACC_GYRO_MD1_CFG            0x5E
#define LSM6DS3_ACC_GYRO_MD2_CFG            0x5F

#define LSM6DS3_ACC_GYRO_OUT_MAG_RAW_X_L    0x66
#define LSM6DS3_ACC_GYRO_OUT_MAG_RAW_X_H    0x67
#define LSM6DS3_ACC_GYRO_OUT_MAG_RAW_Y_L    0x68
#define LSM6DS3_ACC_GYRO_OUT_MAG_RAW_Y_H    0x69
#define LSM6DS3_ACC_GYRO_OUT_MAG_RAW_Z_L    0x6A
#define LSM6DS3_ACC_GYRO_OUT_MAG_RAW_Z_H    0x6B

#define LSM6DS3_CHIP_ID_VALUE               (0x69)

#define LSM6DS3_RESET_VALUE                 (0x1)
#define LSM6DS3_RESET_MSK                   (0X1)
#define LSM6DS3_RESET_POS                   (0)

#define LSM6DS3_BDU_VALUE                 (0x40)
#define LSM6DS3_BDU_MSK                   (0X40)
#define LSM6DS3_BDU_POS                   (6)

#define LSM6DS3_ACC_ODR_POWER_DOWN          (0X00)
#define LSM6DS3_ACC_ODR_1_6_HZ              (0X0B)
#define LSM6DS3_ACC_ODR_12_5_HZ             (0x01)
#define LSM6DS3_ACC_ODR_26_HZ               (0x02)
#define LSM6DS3_ACC_ODR_52_HZ               (0x03)
#define LSM6DS3_ACC_ODR_104_HZ              (0x04)
#define LSM6DS3_ACC_ODR_208_HZ              (0x05)
#define LSM6DS3_ACC_ODR_416_HZ              (0x06)
#define LSM6DS3_ACC_ODR_833_HZ              (0x07)
#define LSM6DS3_ACC_ODR_1_66_KHZ            (0x08)
#define LSM6DS3_ACC_ODR_3_33_KHZ            (0x09)
#define LSM6DS3_ACC_ODR_6_66_KHZ            (0x0A)
#define LSM6DS3_ACC_ODR_MSK                 (0XF0)
#define LSM6DS3_ACC_ODR_POS                 (4)

#define LSM6DS3_GYRO_ODR_POWER_DOWN         (0X00)
#define LSM6DS3_GYRO_ODR_12_5_HZ            (0x01)
#define LSM6DS3_GYRO_ODR_26_HZ              (0x02)
#define LSM6DS3_GYRO_ODR_52_HZ              (0x03)
#define LSM6DS3_GYRO_ODR_104_HZ             (0x04)
#define LSM6DS3_GYRO_ODR_208_HZ             (0x05)
#define LSM6DS3_GYRO_ODR_416_HZ             (0x06)
#define LSM6DS3_GYRO_ODR_833_HZ             (0x07)
#define LSM6DS3_GYRO_ODR_1_66_KHZ           (0x08)
#define LSM6DS3_GYRO_ODR_3_33_KHZ           (0x09)
#define LSM6DS3_GYRO_ODR_6_66_KHZ           (0x0A)
#define LSM6DS3_GYRO_ODR_MSK                (0XF0)
#define LSM6DS3_GYRO_ODR_POS                (4)

#define LSM6DS3_ACC_RANGE_2G                (0x0)
#define LSM6DS3_ACC_RANGE_4G                (0x2)
#define LSM6DS3_ACC_RANGE_8G                (0x3)
#define LSM6DS3_ACC_RANGE_16G               (0x1)
#define LSM6DS3_ACC_RANGE_MSK               (0X0C)
#define LSM6DS3_ACC_RANGE_POS               (2)

#define LSM6DS3_ACC_SENSITIVITY_2G          (61)
#define LSM6DS3_ACC_SENSITIVITY_4G          (122)
#define LSM6DS3_ACC_SENSITIVITY_8G          (244)
#define LSM6DS3_ACC_SENSITIVITY_16G         (488) 

#define LSM6DS3_GYRO_RANGE_245              (0x0)
#define LSM6DS3_GYRO_RANGE_500              (0x1)
#define LSM6DS3_GYRO_RANGE_1000             (0x2)
#define LSM6DS3_GYRO_RANGE_2000             (0x3)
#define LSM6DS3_GYRO_RANGE_MSK              (0X0C)
#define LSM6DS3_GYRO_RANGE_POS              (2)

#define LSM6DS3_GYRO_SENSITIVITY_245DPS     (8750)
#define LSM6DS3_GYRO_SENSITIVITY_500DPS     (17500)
#define LSM6DS3_GYRO_SENSITIVITY_1000DPS    (35000)
#define LSM6DS3_GYRO_SENSITIVITY_2000DPS    (70000)

#define LSM6DS3_SHIFT_EIGHT_BITS            (8)
#define LSM6DS3_16_BIT_SHIFT                (0xFF)
#define LSM6DS3_ACC_MUL                     (1000)
#define LSM6DS3_GYRO_MUL                    (1)

#define LSM6DS3_ACC_DEFAULT_ODR_100HZ       (100)
#define LSM6DS3_GYRO_DEFAULT_ODR_100HZ      (100)

#define LSM6DS3_ACC_SELF_TEST_MIN_X              (90) // 90mg
#define LSM6DS3_ACC_SELF_TEST_MIN_Y              (90) // 90mg  
#define LSM6DS3_ACC_SELF_TEST_MIN_Z              (90)  // 90mg
#define LSM6DS3_ACC_SELF_TEST_MAX_X              (1700) // 1700mg
#define LSM6DS3_ACC_SELF_TEST_MAX_Y              (1700) // 1700mg
#define LSM6DS3_ACC_SELF_TEST_MAX_Z              (1700) // 1700mg

#define LSM6DS3_ACC_SELF_TEST_DRY_WAIT_CNT  5
#define LSM6DS3_ACC_SELF_TEST_AVG_SAMPLE_CNT    5

#define LSM6DS3_GYRO_SELF_TEST_MIN_X              (150) // 150dps
#define LSM6DS3_GYRO_SELF_TEST_MIN_Y              (150) // 150dps
#define LSM6DS3_GYRO_SELF_TEST_MIN_Z              (150) // 150dps
#define LSM6DS3_GYRO_SELF_TEST_MAX_X             (700) // 700dps
#define LSM6DS3_GYRO_SELF_TEST_MAX_Y              (700) // 700dps
#define LSM6DS3_GYRO_SELF_TEST_MAX_Z              (700) // 700dps

#define LSM6DS3_GYRO_SELF_TEST_DRY_WAIT_CNT  5
#define LSM6DS3_GYRO_SELF_TEST_AVG_SAMPLE_CNT    5

#define LSM6DS3_GET_BITSLICE(regvar, bitname)\
((regvar & bitname##_MSK) >> bitname##_POS)

#define LSM6DS3_SET_BITSLICE(regvar, bitname, val)\
((regvar & ~bitname##_MSK) | ((val<<bitname##_POS)&bitname##_MSK))

static int32_t lsm6ds3_acc_factor[ACC_RANGE_MAX] = { LSM6DS3_ACC_SENSITIVITY_2G, LSM6DS3_ACC_SENSITIVITY_4G, 
                                     LSM6DS3_ACC_SENSITIVITY_8G, LSM6DS3_ACC_SENSITIVITY_16G };
static int32_t lsm6ds3_gyro_factor[GYRO_RANGE_MAX] = {0, LSM6DS3_GYRO_SENSITIVITY_245DPS, LSM6DS3_GYRO_SENSITIVITY_500DPS, 
                                     LSM6DS3_GYRO_SENSITIVITY_1000DPS, LSM6DS3_GYRO_SENSITIVITY_2000DPS };
static int32_t cur_acc_factor = 0;
static int32_t cur_gyro_factor = 0;
static int32_t g_lsm6ds3flag = 0;

static i2c_dev_t lsm6ds3_ctx = {
    //.port = 4,
    .port = 3,
    .config.address_width = 8,
    .config.freq = 400000,
    .config.dev_addr = LSM6DS3_I2C_ADDR,
};

static int drv_acc_gyro_st_lsm6ds3_soft_reset(i2c_dev_t* drv)
{
    int ret = 0;
    uint8_t value = 0x00;

    ret = sensor_i2c_read(drv, LSM6DS3_ACC_GYRO_CTRL3_C, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }
    
    value |= LSM6DS3_RESET_VALUE;
    
    ret = sensor_i2c_write(drv, LSM6DS3_ACC_GYRO_CTRL3_C, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return -1;
    }
    return 0;
}

static int drv_acc_gyro_st_lsm6ds3_validate_id(i2c_dev_t* drv, uint8_t id_value)
{
    uint8_t value = 0x00;
    int ret = 0;

    if(drv == NULL){
        return -1;
    }

    ret = sensor_i2c_read(drv, LSM6DS3_ACC_GYRO_WHO_AM_I_REG, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    //LOG("%s %s right id (0x%02x), read id(0x%02x)\n", SENSOR_STR, __func__, id_value, value);

    if(unlikely(ret)){
        return ret;
    }
    
    if (id_value != value){
        return -1;
    }
    
    return 0;
}

static int drv_acc_st_lsm6ds3_set_power_mode(i2c_dev_t* drv, dev_power_mode_e mode)
{
    uint8_t value = 0x00;
    int ret = 0;
    
    ret = sensor_i2c_read(drv, LSM6DS3_ACC_GYRO_CTRL1_XL, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }

    switch(mode){
        case DEV_POWER_ON:{
            
            value = LSM6DS3_SET_BITSLICE(value,LSM6DS3_ACC_ODR,LSM6DS3_ACC_ODR_12_5_HZ);
            ret = sensor_i2c_write(drv, LSM6DS3_ACC_GYRO_CTRL1_XL, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
            if(unlikely(ret)){
                return ret;
            }
        }break;
        
        case DEV_POWER_OFF:{
            value = LSM6DS3_SET_BITSLICE(value,LSM6DS3_ACC_ODR,LSM6DS3_ACC_ODR_POWER_DOWN);
            ret = sensor_i2c_write(drv, LSM6DS3_ACC_GYRO_CTRL1_XL, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
            if(unlikely(ret)){
                return ret;
            }
        }break;
            
        case DEV_SLEEP:{
            value = LSM6DS3_SET_BITSLICE(value,LSM6DS3_ACC_ODR,LSM6DS3_ACC_ODR_12_5_HZ);
            ret = sensor_i2c_write(drv, LSM6DS3_ACC_GYRO_CTRL1_XL, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
            if(unlikely(ret)){
                return ret;
            }

        }break;
        
       default:break;
    }
    return 0;
}

static int drv_acc_gyro_st_lsm6ds3_set_bdu(i2c_dev_t* drv)
{
    int ret = 0;
    uint8_t value = 0x00;

    ret = sensor_i2c_read(drv, LSM6DS3_ACC_GYRO_CTRL3_C, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }

    if (value & LSM6DS3_BDU_VALUE)
        return 0;
    
    value |= LSM6DS3_BDU_VALUE;
    
    ret = sensor_i2c_write(drv, LSM6DS3_ACC_GYRO_CTRL3_C, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return -1;
    }
    return 0;
}

static uint8_t drv_acc_st_lsm6ds3_hz2odr(uint32_t hz)
{
    if(hz > 3330)
        return LSM6DS3_ACC_ODR_6_66_KHZ;
    else if(hz > 1660)
        return LSM6DS3_ACC_ODR_3_33_KHZ;
    else if(hz > 833)
        return LSM6DS3_ACC_ODR_1_66_KHZ;
    else if(hz > 416)
        return LSM6DS3_ACC_ODR_833_HZ;
    else if(hz > 208)
        return LSM6DS3_ACC_ODR_416_HZ;
    else if(hz > 104)
        return LSM6DS3_ACC_ODR_208_HZ;
    else if(hz > 52)
        return LSM6DS3_ACC_ODR_104_HZ;
    else if(hz > 26)
        return LSM6DS3_ACC_ODR_52_HZ;
    else if(hz > 13)
        return LSM6DS3_ACC_ODR_26_HZ;
    else if(hz >= 2)
        return LSM6DS3_ACC_ODR_12_5_HZ;
    else
        return LSM6DS3_ACC_ODR_1_6_HZ;

}


static int drv_acc_st_lsm6ds3_set_odr(i2c_dev_t* drv, uint32_t hz)
{
    int ret = 0;
    uint8_t value = 0x00;
    uint8_t odr = drv_acc_st_lsm6ds3_hz2odr(hz);

    ret = sensor_i2c_read(drv, LSM6DS3_ACC_GYRO_CTRL1_XL, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }

    value = LSM6DS3_SET_BITSLICE(value,LSM6DS3_ACC_ODR,odr);

    ret = sensor_i2c_write(drv, LSM6DS3_ACC_GYRO_CTRL1_XL, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }
    return 0;    
}

static int drv_acc_st_lsm6ds3_set_range(i2c_dev_t* drv, uint32_t range)
{
    int ret = 0;
    uint8_t value = 0x00;
    uint8_t tmp = 0;

    ret = sensor_i2c_read(drv, LSM6DS3_ACC_GYRO_CTRL1_XL, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }

    switch (range){
        case ACC_RANGE_2G:{
            tmp = LSM6DS3_ACC_RANGE_2G;
        }break;
        
        case ACC_RANGE_4G:{
            tmp = LSM6DS3_ACC_RANGE_4G;
        }break;
        
        case ACC_RANGE_8G:{
            tmp = LSM6DS3_ACC_RANGE_8G;
        }break;
            
        case ACC_RANGE_16G:{
            tmp = LSM6DS3_ACC_RANGE_16G;
        }break;
        
        default:break;
    }
    
    value  = LSM6DS3_SET_BITSLICE(value,LSM6DS3_ACC_RANGE,tmp);
    ret = sensor_i2c_write(drv, LSM6DS3_ACC_GYRO_CTRL1_XL, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }

    if((range >= ACC_RANGE_2G)&&(range <= ACC_RANGE_16G)){
        cur_acc_factor = lsm6ds3_acc_factor[range];
    }
    
    return 0;
}

static int drv_acc_st_lsm6ds3_st_discard(i2c_dev_t* drv)
{
    uint8_t i;
    uint8_t value = 0x00;
    int ret = 0;
    uint8_t buffer[6];

    for (i = 0; i < LSM6DS3_ACC_SELF_TEST_DRY_WAIT_CNT; i ++) {
        ret = sensor_i2c_read(drv, LSM6DS3_ACC_GYRO_STATUS_REG, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
        if(unlikely(ret)){
            LOG("%s %s %s %d\n", SENSOR_STR, __func__, ERROR_LINE, __LINE__);
            return ret;
        }
        if (value & 0x01)
            break;

        aos_msleep(20);
    }

    if (i >= LSM6DS3_ACC_SELF_TEST_DRY_WAIT_CNT) {
        LOG("%s %s %s %d\n", SENSOR_STR, __func__, ERROR_LINE, __LINE__);
        return -1;
    }

    ret = sensor_i2c_read(drv, LSM6DS3_ACC_GYRO_OUTX_L_XL, buffer, 6, I2C_OP_RETRIES);
    if(unlikely(ret)){
        LOG("%s %s %s %d\n", SENSOR_STR, __func__, ERROR_LINE, __LINE__);
        return ret;
    }

    return ret;
}

static int drv_acc_st_lsm6ds3_st_data(i2c_dev_t* drv,int32_t* data)
{
    uint8_t i, j;
    int16_t x_raw, y_raw, z_raw;
    int32_t x_mg, y_mg, z_mg;
    int32_t x_sum, y_sum, z_sum;
    uint8_t value = 0x00;
    int ret = 0;
    uint8_t buffer[6];

    x_sum = 0; y_sum = 0; z_sum = 0;
    
    for (i = 0; i < LSM6DS3_ACC_SELF_TEST_AVG_SAMPLE_CNT; i ++) {
        for (j = 0; j < LSM6DS3_ACC_SELF_TEST_DRY_WAIT_CNT; j ++) {
            ret = sensor_i2c_read(drv, LSM6DS3_ACC_GYRO_STATUS_REG, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
            if(unlikely(ret)){
                LOG("%s %s %s %d\n", SENSOR_STR, __func__, ERROR_LINE, __LINE__);
                return ret;
            }
            if (value & 0x01)
                break;
        
            aos_msleep(20);
        }

        if (j >= LSM6DS3_ACC_SELF_TEST_DRY_WAIT_CNT) {
            LOG("%s %s %s %d\n", SENSOR_STR, __func__, ERROR_LINE, __LINE__);
            return -1;
        }

        ret = sensor_i2c_read(drv,  LSM6DS3_ACC_GYRO_OUTX_L_XL, buffer, 6, I2C_OP_RETRIES);
        if(unlikely(ret)){
            LOG("%s %s %s %d\n", SENSOR_STR, __func__, ERROR_LINE, __LINE__);
            return ret;
        }

        x_raw = (buffer[1] << 8) + buffer[0];
        y_raw = (buffer[3] << 8) + buffer[2];
        z_raw = (buffer[5] << 8) + buffer[4];
        //LOG("%s %s %d: i(%d), raw(%d, %d, %d)\n", SENSOR_STR, __func__, __LINE__, i, x_raw, y_raw, z_raw);

        x_mg = x_raw * LSM6DS3_ACC_SENSITIVITY_4G / 1000;
        y_mg = y_raw * LSM6DS3_ACC_SENSITIVITY_4G / 1000;
        z_mg = z_raw  * LSM6DS3_ACC_SENSITIVITY_4G / 1000;
        //LOG("%s %s %d: i(%d), mg(%d, %d, %d)\n", SENSOR_STR, __func__, __LINE__, i, x_mg, y_mg, z_mg);

        x_sum += x_mg;
        y_sum += y_mg;
        z_sum += z_mg;
    }

    data[0] = x_sum / LSM6DS3_ACC_SELF_TEST_AVG_SAMPLE_CNT;
    data[1] = y_sum / LSM6DS3_ACC_SELF_TEST_AVG_SAMPLE_CNT;
    data[2] = z_sum / LSM6DS3_ACC_SELF_TEST_AVG_SAMPLE_CNT;
    return ret;
}

static int drv_acc_st_lsm6ds3_self_test(i2c_dev_t* drv,int32_t* data)
{
    uint8_t i;
    uint8_t value = 0x00;
    int ret = 0;
    uint8_t ctrl_reg[10];
    int32_t out_nost[3];
    int32_t out_st[3];
    int32_t out_diff[3];
    const uint8_t ctrl_val[10] = { 0x38, 0, 0x44, 0, 0, 0, 0, 0, 0, 0 };

    // Save cfg registers which will be modified during self-test
    ret = sensor_i2c_read(drv, LSM6DS3_ACC_GYRO_CTRL1_XL, ctrl_reg, 10, I2C_OP_RETRIES);
    if(unlikely(ret)){
        LOG("%s %s %s %d\n", SENSOR_STR, __func__, ERROR_LINE, __LINE__);
        return ret;
    }

    // Initialize Sensor, turn on sensor, enable X/Y/Z axes 
    // Set BDU=1, FS=4G, ODR = 52Hz
    for (i = 0; i < 10; i ++) {
        value = ctrl_val[i];
        ret = sensor_i2c_write(drv, LSM6DS3_ACC_GYRO_CTRL1_XL + i, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
        if(unlikely(ret)){
            LOG("%s %s %s %d\n", SENSOR_STR, __func__, ERROR_LINE, __LINE__);
            goto restore;
        }
    }

    aos_msleep(100);

    // Discard the first sample
    ret = drv_acc_st_lsm6ds3_st_discard(drv);
    if(unlikely(ret)){
        LOG("%s %s %s %d\n", SENSOR_STR, __func__, ERROR_LINE, __LINE__);
        goto restore;
    }

    // Read some samples, and averate them
    ret = drv_acc_st_lsm6ds3_st_data(drv, out_nost);
    if(unlikely(ret)){
        LOG("%s %s %s %d\n", SENSOR_STR, __func__, ERROR_LINE, __LINE__);
        goto restore;
    }

    // Enable seft-test
    value = 0x01;
    ret = sensor_i2c_write(drv, LSM6DS3_ACC_GYRO_CTRL5_C, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        LOG("%s %s %s %d\n", SENSOR_STR, __func__, ERROR_LINE, __LINE__);
        goto restore;
    }
    aos_msleep(100);
    
    // Discard the first sample
    ret = drv_acc_st_lsm6ds3_st_discard(drv);
    if(unlikely(ret)){
        LOG("%s %s %s %d\n", SENSOR_STR, __func__, ERROR_LINE, __LINE__);
        goto restore;
    }

    // Read some samples, and average them
    ret = drv_acc_st_lsm6ds3_st_data(drv, out_st);
    if(unlikely(ret)){
        LOG("%s %s %s %d\n", SENSOR_STR, __func__, ERROR_LINE, __LINE__);
        goto restore;
    }

    // Check if the differences are between min and max
    for (i = 0; i < 3; i ++) {
        out_diff[i] = abs(out_st[i] - out_nost[i]);
        data[i] = out_diff[i];
    }
    
    if ((LSM6DS3_ACC_SELF_TEST_MIN_X > out_diff[0]) || (out_diff[0] > LSM6DS3_ACC_SELF_TEST_MAX_X)) {
        LOG("%s %s %s %d\n", SENSOR_STR, __func__, ERROR_LINE, __LINE__);
        ret = -1;
        goto restore;
    }
    if ((LSM6DS3_ACC_SELF_TEST_MIN_Y > out_diff[1]) || (out_diff[1] > LSM6DS3_ACC_SELF_TEST_MAX_Y)) {
        LOG("%s %s %s %d\n", SENSOR_STR, __func__, ERROR_LINE, __LINE__);
        ret = -1;
        goto restore;
    }
    if ((LSM6DS3_ACC_SELF_TEST_MIN_Z > out_diff[2]) || (out_diff[2] > LSM6DS3_ACC_SELF_TEST_MAX_Z)) {
        LOG("%s %s %s %d\n", SENSOR_STR, __func__, ERROR_LINE, __LINE__);
        ret = -1;
        goto restore;
    }

restore:
    ret += sensor_i2c_write(drv, LSM6DS3_ACC_GYRO_CTRL1_XL, ctrl_reg, 10, I2C_OP_RETRIES);
    return ret;
}


static void drv_acc_st_lsm6ds3_irq_handle(void)
{
    /* no handle so far */
}

static int drv_acc_st_lsm6ds3_open(void)
{
    int ret = 0;
    
    ret  = drv_acc_st_lsm6ds3_set_power_mode(&lsm6ds3_ctx, DEV_POWER_ON);
    if(unlikely(ret)){
        return -1;
    }

    ret  = drv_acc_gyro_st_lsm6ds3_set_bdu(&lsm6ds3_ctx);
    if(unlikely(ret)){
        return -1;
    }

    ret = drv_acc_st_lsm6ds3_set_range(&lsm6ds3_ctx, ACC_RANGE_8G);
    if(unlikely(ret)){
        return -1;
    }

    ret = drv_acc_st_lsm6ds3_set_odr(&lsm6ds3_ctx, LSM6DS3_ACC_DEFAULT_ODR_100HZ);
    if(unlikely(ret)){
        return -1;
    }
     
    return 0;

}

static int drv_acc_st_lsm6ds3_close(void)
{
    int ret = 0;
    ret  = drv_acc_st_lsm6ds3_set_power_mode(&lsm6ds3_ctx, DEV_POWER_OFF);
    if(unlikely(ret)){
        return -1;
    }
    return 0;
}

static int drv_acc_st_lsm6ds3_read(void *buf, size_t len)
{
    int ret = 0;
    size_t size;
    uint8_t reg[6];
    accel_data_t *accel = (accel_data_t *)buf;
    if(buf == NULL){
        return -1;
    }

    size = sizeof(accel_data_t);
    if(len < size){
        return -1;
    }

    ret  = sensor_i2c_read(&lsm6ds3_ctx, LSM6DS3_ACC_GYRO_OUTX_L_XL,  &reg[0], I2C_REG_LEN, I2C_OP_RETRIES);
    ret |= sensor_i2c_read(&lsm6ds3_ctx, LSM6DS3_ACC_GYRO_OUTX_H_XL,  &reg[1], I2C_REG_LEN, I2C_OP_RETRIES);
    ret |= sensor_i2c_read(&lsm6ds3_ctx, LSM6DS3_ACC_GYRO_OUTY_L_XL,  &reg[2], I2C_REG_LEN, I2C_OP_RETRIES);
    ret |= sensor_i2c_read(&lsm6ds3_ctx, LSM6DS3_ACC_GYRO_OUTY_H_XL,  &reg[3], I2C_REG_LEN, I2C_OP_RETRIES);
    ret |= sensor_i2c_read(&lsm6ds3_ctx, LSM6DS3_ACC_GYRO_OUTZ_L_XL,  &reg[4], I2C_REG_LEN, I2C_OP_RETRIES);
    ret |= sensor_i2c_read(&lsm6ds3_ctx, LSM6DS3_ACC_GYRO_OUTZ_H_XL,  &reg[5], I2C_REG_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return -1;
    }
    accel->data[DATA_AXIS_X] = (int16_t)((((int16_t)((int8_t)reg[1]))<< LSM6DS3_SHIFT_EIGHT_BITS)|(reg[0]));
    accel->data[DATA_AXIS_Y] = (int16_t)((((int16_t)((int8_t)reg[3]))<< LSM6DS3_SHIFT_EIGHT_BITS)|(reg[2]));
    accel->data[DATA_AXIS_Z] = (int16_t)((((int16_t)((int8_t)reg[5]))<< LSM6DS3_SHIFT_EIGHT_BITS)|(reg[4]));

    if(cur_acc_factor != 0){
        accel->data[DATA_AXIS_X] = (accel->data[DATA_AXIS_X] * cur_acc_factor)/LSM6DS3_ACC_MUL;
        accel->data[DATA_AXIS_Y] = (accel->data[DATA_AXIS_Y] * cur_acc_factor)/LSM6DS3_ACC_MUL;
        accel->data[DATA_AXIS_Z] = (accel->data[DATA_AXIS_Z] * cur_acc_factor)/LSM6DS3_ACC_MUL;
    }
    
    accel->timestamp = aos_now_ms();

    return (int)size;
}

static int drv_acc_st_lsm6ds3_ioctl(int cmd, unsigned long arg)
{
    int ret = 0;
    dev_sensor_info_t *info = (dev_sensor_info_t *)arg;
    
    switch(cmd){
        case SENSOR_IOCTL_ODR_SET:{
            ret = drv_acc_st_lsm6ds3_set_odr(&lsm6ds3_ctx, arg);
            if(unlikely(ret)){
                return -1;
            }
        }break;
        case SENSOR_IOCTL_RANGE_SET:{
            ret = drv_acc_st_lsm6ds3_set_range(&lsm6ds3_ctx, arg);
            if(unlikely(ret)){
                return -1;
            }
        }break;
        case SENSOR_IOCTL_SET_POWER:{
            ret = drv_acc_st_lsm6ds3_set_power_mode(&lsm6ds3_ctx, arg);
            if(unlikely(ret)){
                return -1;
            }
        }break;
        case SENSOR_IOCTL_GET_INFO:{ 
            /* fill the dev info here */
            //dev_sensor_info_t *info = (dev_sensor_info_t*)arg;
            info->model = "LSM6DS3";
            info->range_max = 16;
            info->range_min = 2;
            info->unit = mg;
        }break;
	case SENSOR_IOCTL_SELF_TEST:{
	   ret = drv_acc_st_lsm6ds3_self_test(&lsm6ds3_ctx, (int32_t*)info->data);
	   //printf("%d	%d	 %d\n",info->data[0],info->data[1],info->data[2]);
           LOG("%s %s: %d, %d, %d\n", SENSOR_STR, __func__, info->data[0],info->data[1],info->data[2]);
	   return ret;
	}
       default:break;
    }

    return 0;
}


int drv_acc_st_lsm6ds3_init(void){
    int ret = 0;
    sensor_obj_t sensor;
    memset(&sensor, 0, sizeof(sensor));

    /* fill the sensor obj parameters here */
    sensor.io_port    = I2C_PORT;
    sensor.tag        = TAG_DEV_ACC;
    sensor.path       = dev_acc_path;
    sensor.open       = drv_acc_st_lsm6ds3_open;
    sensor.close      = drv_acc_st_lsm6ds3_close;
    sensor.read       = drv_acc_st_lsm6ds3_read;
    sensor.write      = NULL;
    sensor.ioctl      = drv_acc_st_lsm6ds3_ioctl;
    sensor.irq_handle = drv_acc_st_lsm6ds3_irq_handle;

    ret = sensor_create_obj(&sensor);
    if(unlikely(ret)){
        return -1;
    }
    ret = drv_acc_gyro_st_lsm6ds3_validate_id(&lsm6ds3_ctx, LSM6DS3_CHIP_ID_VALUE);
    if(unlikely(ret)){
        return -1;
    }

    if(0 == g_lsm6ds3flag)
    {
        ret = drv_acc_gyro_st_lsm6ds3_soft_reset(&lsm6ds3_ctx);
        if(unlikely(ret)){
            return -1;
        }
        ret = drv_acc_st_lsm6ds3_set_power_mode(&lsm6ds3_ctx, DEV_POWER_OFF);
        if(unlikely(ret)){
            return -1;
        }
        g_lsm6ds3flag = 1;
    }
    else
    {
        LOG("%s %s acc do not need reset\n", SENSOR_STR, __func__);
    }
    
    LOG("%s %s successfully \n", SENSOR_STR, __func__);
    return 0;
}


static int drv_gyro_st_lsm6ds3_set_power_mode(i2c_dev_t* drv, dev_power_mode_e mode)
{
    uint8_t value = 0x00;
    int ret = 0;
    
    ret = sensor_i2c_read(drv, LSM6DS3_ACC_GYRO_CTRL2_G, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }

    switch(mode){
        case DEV_POWER_ON:{
            
            value = LSM6DS3_SET_BITSLICE(value,LSM6DS3_GYRO_ODR,LSM6DS3_GYRO_ODR_12_5_HZ);
            ret = sensor_i2c_write(drv, LSM6DS3_ACC_GYRO_CTRL2_G, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
            if(unlikely(ret)){
                return ret;
            }
        }break;
        
        case DEV_POWER_OFF:{
            value = LSM6DS3_SET_BITSLICE(value,LSM6DS3_GYRO_ODR,LSM6DS3_GYRO_ODR_POWER_DOWN);
            ret = sensor_i2c_write(drv, LSM6DS3_ACC_GYRO_CTRL2_G, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
            if(unlikely(ret)){
                return ret;
            }
        }break;
            
        case DEV_SLEEP:{
            value = LSM6DS3_SET_BITSLICE(value,LSM6DS3_GYRO_ODR,LSM6DS3_GYRO_ODR_12_5_HZ);
            ret = sensor_i2c_write(drv, LSM6DS3_ACC_GYRO_CTRL2_G, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
            if(unlikely(ret)){
                return ret;
            }

        }break;
        
       default:break;
    }
    return 0;
}

static uint8_t drv_gyro_st_lsm6ds3_hz2odr(uint32_t hz)
{
    if(hz > 3330)
        return LSM6DS3_GYRO_ODR_6_66_KHZ;
    else if(hz > 1660)
        return LSM6DS3_GYRO_ODR_3_33_KHZ;
    else if(hz > 833)
        return LSM6DS3_GYRO_ODR_1_66_KHZ;
    else if(hz > 416)
        return LSM6DS3_GYRO_ODR_833_HZ;
    else if(hz > 208)
        return LSM6DS3_GYRO_ODR_416_HZ;
    else if(hz > 104)
        return LSM6DS3_GYRO_ODR_208_HZ;
    else if(hz > 52)
        return LSM6DS3_GYRO_ODR_104_HZ;
    else if(hz > 26)
        return LSM6DS3_GYRO_ODR_52_HZ;
    else if(hz > 13)
        return LSM6DS3_GYRO_ODR_26_HZ;
    else
        return LSM6DS3_GYRO_ODR_12_5_HZ;

}


static int drv_gyro_st_lsm6ds3_set_odr(i2c_dev_t* drv, uint32_t hz)
{
    int ret = 0;
    uint8_t value = 0x00;
    uint8_t odr = drv_gyro_st_lsm6ds3_hz2odr(hz);

    ret = sensor_i2c_read(drv, LSM6DS3_ACC_GYRO_CTRL2_G, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }

    value = LSM6DS3_SET_BITSLICE(value,LSM6DS3_GYRO_ODR,odr);

    ret = sensor_i2c_write(drv, LSM6DS3_ACC_GYRO_CTRL2_G, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }
    return 0;    
}

static int drv_gyro_st_lsm6ds3_set_range(i2c_dev_t* drv, uint32_t range)
{
    int ret = 0;
    uint8_t value = 0x00;
    uint8_t tmp = 0;

    ret = sensor_i2c_read(drv, LSM6DS3_ACC_GYRO_CTRL2_G, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }

    switch (range){
        case GYRO_RANGE_250DPS:{
            tmp = LSM6DS3_GYRO_RANGE_245;
        }break;
        
        case GYRO_RANGE_500DPS:{
            tmp = LSM6DS3_GYRO_RANGE_500;
        }break;
        
        case GYRO_RANGE_1000DPS:{
            tmp = LSM6DS3_GYRO_RANGE_1000;
        }break;
            
        case GYRO_RANGE_2000DPS:{
            tmp = LSM6DS3_GYRO_RANGE_2000;
        }break;
        
        default:break;
    }
    
    value  = LSM6DS3_SET_BITSLICE(value,LSM6DS3_GYRO_RANGE,tmp);
    ret = sensor_i2c_write(drv, LSM6DS3_ACC_GYRO_CTRL2_G, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }
    
    if((range >= GYRO_RANGE_250DPS)&&(range <= GYRO_RANGE_2000DPS)){
        cur_gyro_factor = lsm6ds3_gyro_factor[range];
    }
    
    return 0;
}

static int drv_gyro_st_lsm6ds3_st_discard(i2c_dev_t* drv)
{
    uint8_t i;
    uint8_t value = 0x00;
    int ret = 0;
    uint8_t buffer[6];

    for (i = 0; i < LSM6DS3_GYRO_SELF_TEST_DRY_WAIT_CNT; i ++) {
        ret = sensor_i2c_read(drv, LSM6DS3_ACC_GYRO_STATUS_REG, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
        if(unlikely(ret)){
            LOG("%s %s %s %d\n", SENSOR_STR, __func__, ERROR_LINE, __LINE__);
            return ret;
        }
        if (value & 0x02)
            break;

        aos_msleep(20);
    }

    if (i >= LSM6DS3_GYRO_SELF_TEST_DRY_WAIT_CNT) {
        LOG("%s %s %s %d\n", SENSOR_STR, __func__, ERROR_LINE, __LINE__);
        return -1;
    }

    ret = sensor_i2c_read(drv, LSM6DS3_ACC_GYRO_OUTX_L_G, buffer, 6, I2C_OP_RETRIES);
    if(unlikely(ret)){
        LOG("%s %s %s %d\n", SENSOR_STR, __func__, ERROR_LINE, __LINE__);
        return ret;
    }

    return ret;
}

static int drv_gyro_st_lsm6ds3_st_data(i2c_dev_t* drv,int32_t* data)
{
    uint8_t i, j;
    int16_t x_raw, y_raw, z_raw;
    int32_t x_dps, y_dps, z_dps;
    int32_t x_sum, y_sum, z_sum;
    uint8_t value = 0x00;
    int ret = 0;
    uint8_t buffer[6];

    x_sum = 0; y_sum = 0; z_sum = 0;
    
    for (i = 0; i < LSM6DS3_GYRO_SELF_TEST_AVG_SAMPLE_CNT; i ++) {
        for (j = 0; j < LSM6DS3_GYRO_SELF_TEST_DRY_WAIT_CNT; j ++) {
            ret = sensor_i2c_read(drv, LSM6DS3_ACC_GYRO_STATUS_REG, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
            if(unlikely(ret)){
                LOG("%s %s %s %d\n", SENSOR_STR, __func__, ERROR_LINE, __LINE__);
                return ret;
            }
            if (value & 0x02)
                break;
        
            aos_msleep(5);
        }

        if (j >= LSM6DS3_GYRO_SELF_TEST_DRY_WAIT_CNT) {
            LOG("%s %s %s %d\n", SENSOR_STR, __func__, ERROR_LINE, __LINE__);
            return -1;
        }

        ret = sensor_i2c_read(drv,  LSM6DS3_ACC_GYRO_OUTX_L_G, buffer, 6, I2C_OP_RETRIES);
        if(unlikely(ret)){
            LOG("%s %s %s %d\n", SENSOR_STR, __func__, ERROR_LINE, __LINE__);
            return ret;
        }

        x_raw = (buffer[1] << 8) + buffer[0];
        y_raw = (buffer[3] << 8) + buffer[2];
        z_raw = (buffer[5] << 8) + buffer[4];
        //LOG("%s %s %d: i(%d), raw(%d, %d, %d)\n", SENSOR_STR, __func__, __LINE__, i, x_raw, y_raw, z_raw);

        x_dps = x_raw * LSM6DS3_GYRO_SENSITIVITY_2000DPS / 1000000;
        y_dps = y_raw * LSM6DS3_GYRO_SENSITIVITY_2000DPS / 1000000;
        z_dps = z_raw  * LSM6DS3_GYRO_SENSITIVITY_2000DPS / 1000000;
        //LOG("%s %s %d: i(%d), dps(%d, %d, %d)\n", SENSOR_STR, __func__, __LINE__, i, x_dps, y_dps, z_dps);

        x_sum += x_dps;
        y_sum += y_dps;
        z_sum += z_dps;
    }

    data[0] = x_sum / LSM6DS3_ACC_SELF_TEST_AVG_SAMPLE_CNT;
    data[1] = y_sum / LSM6DS3_ACC_SELF_TEST_AVG_SAMPLE_CNT;
    data[2] = z_sum / LSM6DS3_ACC_SELF_TEST_AVG_SAMPLE_CNT;
    return ret;
}

static int drv_gyro_st_lsm6ds3_self_test(i2c_dev_t* drv,int32_t* data)
{
    uint8_t i;
    uint8_t value = 0x00;
    int ret = 0;
    uint8_t ctrl_reg[10];
    int32_t out_nost[3];
    int32_t out_st[3];
    int32_t out_diff[3];
    const uint8_t ctrl_val[10] = { 0, 0x5c, 0x44, 0, 0, 0, 0, 0, 0, 0 };

    // Save cfg registers which will be modified during self-test
    ret = sensor_i2c_read(drv, LSM6DS3_ACC_GYRO_CTRL1_XL, ctrl_reg, 10, I2C_OP_RETRIES);
    if(unlikely(ret)){
        LOG("%s %s %s %d\n", SENSOR_STR, __func__, ERROR_LINE, __LINE__);
        return ret;
    }

    // Initialize Sensor, turn on sensor, enable P/R/Y 
    // Set BDU=1, ODR = 208Hz, FS=2000dps
    for (i = 0; i < 10; i ++) {
        value = ctrl_val[i];
        ret = sensor_i2c_write(drv, LSM6DS3_ACC_GYRO_CTRL1_XL + i, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
        if(unlikely(ret)){
            LOG("%s %s %s %d\n", SENSOR_STR, __func__, ERROR_LINE, __LINE__);
            goto restore;
        }
    }

    aos_msleep(150);

    // Discard the first sample
    ret = drv_gyro_st_lsm6ds3_st_discard(drv);
    if(unlikely(ret)){
        LOG("%s %s %s %d\n", SENSOR_STR, __func__, ERROR_LINE, __LINE__);
        goto restore;
    }

    // Read some samples, and averate them
    ret = drv_gyro_st_lsm6ds3_st_data(drv, out_nost);
    if(unlikely(ret)){
        LOG("%s %s %s %d\n", SENSOR_STR, __func__, ERROR_LINE, __LINE__);
        goto restore;
    }

    // Enable seft-test
    value = 0x04;
    ret = sensor_i2c_write(drv, LSM6DS3_ACC_GYRO_CTRL5_C, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        LOG("%s %s %s %d\n", SENSOR_STR, __func__, ERROR_LINE, __LINE__);
        goto restore;
    }
    aos_msleep(50);
    
    // Discard the first sample
    ret = drv_gyro_st_lsm6ds3_st_discard(drv);
    if(unlikely(ret)){
        LOG("%s %s %s %d\n", SENSOR_STR, __func__, ERROR_LINE, __LINE__);
        goto restore;
    }

    // Read some samples, and average them
    ret = drv_gyro_st_lsm6ds3_st_data(drv, out_st);
    if(unlikely(ret)){
        LOG("%s %s %s %d\n", SENSOR_STR, __func__, ERROR_LINE, __LINE__);
        goto restore;
    }

    // Check if the differences are between min and max
    for (i = 0; i < 3; i ++) {
        out_diff[i] = abs(out_st[i] - out_nost[i]);
        data[i] = out_diff[i];
    }
    
    if ((LSM6DS3_GYRO_SELF_TEST_MIN_X > out_diff[0]) || (out_diff[0] > LSM6DS3_GYRO_SELF_TEST_MAX_X)) {
        LOG("%s %s %s %d\n", SENSOR_STR, __func__, ERROR_LINE, __LINE__);
        ret = -1;
        goto restore;
    }
    if ((LSM6DS3_GYRO_SELF_TEST_MIN_Y > out_diff[1]) || (out_diff[1] > LSM6DS3_GYRO_SELF_TEST_MAX_Y)) {
        LOG("%s %s %s %d\n", SENSOR_STR, __func__, ERROR_LINE, __LINE__);
        ret = -1;
        goto restore;
    }
    if ((LSM6DS3_GYRO_SELF_TEST_MIN_Z > out_diff[2]) || (out_diff[2] > LSM6DS3_GYRO_SELF_TEST_MAX_Z)) {
        LOG("%s %s %s %d\n", SENSOR_STR, __func__, ERROR_LINE, __LINE__);
        ret = -1;
        goto restore;
    }

restore:
    ret += sensor_i2c_write(drv, LSM6DS3_ACC_GYRO_CTRL1_XL, ctrl_reg, 10, I2C_OP_RETRIES);
    return ret;
}


static void drv_gyro_st_lsm6ds3_irq_handle(void)
{
    /* no handle so far */
}

static int drv_gyro_st_lsm6ds3_open(void)
{
    int ret = 0;
    ret  = drv_gyro_st_lsm6ds3_set_power_mode(&lsm6ds3_ctx, DEV_POWER_ON);
    if(unlikely(ret)){
        return -1;
    }

    ret  = drv_acc_gyro_st_lsm6ds3_set_bdu(&lsm6ds3_ctx);
    if(unlikely(ret)){
        return -1;
    }

    ret = drv_gyro_st_lsm6ds3_set_range(&lsm6ds3_ctx, GYRO_RANGE_1000DPS);
    if(unlikely(ret)){
        return -1;
    }

    ret = drv_gyro_st_lsm6ds3_set_odr(&lsm6ds3_ctx, LSM6DS3_GYRO_DEFAULT_ODR_100HZ);
    if(unlikely(ret)){
        return -1;
    }
    
    return 0;

}

static int drv_gyro_st_lsm6ds3_close(void)
{
    int ret = 0;
    ret  = drv_gyro_st_lsm6ds3_set_power_mode(&lsm6ds3_ctx, DEV_POWER_OFF);
    if(unlikely(ret)){
        return -1;
    }
    return 0;
}

static int drv_gyro_st_lsm6ds3_read(void *buf, size_t len)
{
    int ret = 0;
    size_t size;
    uint8_t reg[6];
    gyro_data_t *gyro = (gyro_data_t *)buf;
    if(buf == NULL){
        return -1;
    }

    size = sizeof(gyro_data_t);
    if(len < size){
        return -1;
    }

    ret  = sensor_i2c_read(&lsm6ds3_ctx, LSM6DS3_ACC_GYRO_OUTX_L_G,  &reg[0], I2C_REG_LEN, I2C_OP_RETRIES);
    ret |= sensor_i2c_read(&lsm6ds3_ctx, LSM6DS3_ACC_GYRO_OUTX_H_G,  &reg[1], I2C_REG_LEN, I2C_OP_RETRIES);
    ret |= sensor_i2c_read(&lsm6ds3_ctx, LSM6DS3_ACC_GYRO_OUTY_L_G,  &reg[2], I2C_REG_LEN, I2C_OP_RETRIES);
    ret |= sensor_i2c_read(&lsm6ds3_ctx, LSM6DS3_ACC_GYRO_OUTY_H_G,  &reg[3], I2C_REG_LEN, I2C_OP_RETRIES);
    ret |= sensor_i2c_read(&lsm6ds3_ctx, LSM6DS3_ACC_GYRO_OUTZ_L_G,  &reg[4], I2C_REG_LEN, I2C_OP_RETRIES);
    ret |= sensor_i2c_read(&lsm6ds3_ctx, LSM6DS3_ACC_GYRO_OUTZ_H_G,  &reg[5], I2C_REG_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return -1;
    }
    gyro->data[DATA_AXIS_X] = (int16_t)((((int32_t)((int8_t)reg[1]))<< LSM6DS3_SHIFT_EIGHT_BITS)|(reg[0]));
    gyro->data[DATA_AXIS_Y] = (int16_t)((((int32_t)((int8_t)reg[3]))<< LSM6DS3_SHIFT_EIGHT_BITS)|(reg[2]));
    gyro->data[DATA_AXIS_Z] = (int16_t)((((int32_t)((int8_t)reg[5]))<< LSM6DS3_SHIFT_EIGHT_BITS)|(reg[4]));

    if(cur_gyro_factor != 0){
        gyro->data[DATA_AXIS_X] = (gyro->data[DATA_AXIS_X] * cur_gyro_factor)/LSM6DS3_GYRO_MUL;
        gyro->data[DATA_AXIS_Y] = (gyro->data[DATA_AXIS_Y] * cur_gyro_factor)/LSM6DS3_GYRO_MUL;
        gyro->data[DATA_AXIS_Z] = (gyro->data[DATA_AXIS_Z] * cur_gyro_factor)/LSM6DS3_GYRO_MUL;
    }
    gyro->timestamp = aos_now_ms();

    return (int)size;
}

static int drv_gyro_st_lsm6ds3_ioctl(int cmd, unsigned long arg)
{
    int ret = 0;
    
    dev_sensor_info_t *info = (dev_sensor_info_t *)arg;

    switch(cmd){
        case SENSOR_IOCTL_ODR_SET:{
            ret = drv_gyro_st_lsm6ds3_set_odr(&lsm6ds3_ctx, arg);
            if(unlikely(ret)){
                return -1;
            }
        }break;
        case SENSOR_IOCTL_RANGE_SET:{
            ret = drv_gyro_st_lsm6ds3_set_range(&lsm6ds3_ctx, arg);
            if(unlikely(ret)){
                return -1;
            }
        }break;
        case SENSOR_IOCTL_SET_POWER:{
            ret = drv_gyro_st_lsm6ds3_set_power_mode(&lsm6ds3_ctx, arg);
            if(unlikely(ret)){
                return -1;
            }
        }break;
        case SENSOR_IOCTL_GET_INFO:{ 
            /* fill the dev info here */
            //dev_sensor_info_t *info = (dev_sensor_info_t *)arg;
            info->model = "LSM6DS3";
            info->range_max = 2000;
            info->range_min = 125;
            info->unit = udps;
        }break;
        case SENSOR_IOCTL_SELF_TEST:{
           ret = drv_gyro_st_lsm6ds3_self_test(&lsm6ds3_ctx, (int32_t*)info->data);
           //printf("%d %d   %d\n",info->data[0],info->data[1],info->data[2]);
           LOG("%s %s: %d, %d, %d\n", SENSOR_STR, __func__, info->data[0],info->data[1],info->data[2]);
           return ret;
        }
       
       default:break;
    }

    return 0;
}

int drv_gyro_st_lsm6ds3_init(void){
    int ret = 0;
    sensor_obj_t sensor;
    memset(&sensor, 0, sizeof(sensor));

    /* fill the sensor obj parameters here */
    sensor.io_port    = I2C_PORT;
    sensor.tag        = TAG_DEV_GYRO;
    sensor.path       = dev_gyro_path;
    sensor.open       = drv_gyro_st_lsm6ds3_open;
    sensor.close      = drv_gyro_st_lsm6ds3_close;
    sensor.read       = drv_gyro_st_lsm6ds3_read;
    sensor.write      = NULL;
    sensor.ioctl      = drv_gyro_st_lsm6ds3_ioctl;
    sensor.irq_handle = drv_gyro_st_lsm6ds3_irq_handle;

    ret = sensor_create_obj(&sensor);
    if(unlikely(ret)){
        return -1;
    }

    ret = drv_acc_gyro_st_lsm6ds3_validate_id(&lsm6ds3_ctx, LSM6DS3_CHIP_ID_VALUE);
    if(unlikely(ret)){
        return -1;
    }

    if(0 == g_lsm6ds3flag){
        ret = drv_acc_gyro_st_lsm6ds3_soft_reset(&lsm6ds3_ctx);
        if(unlikely(ret)){
            return -1;
        }

        ret = drv_gyro_st_lsm6ds3_set_power_mode(&lsm6ds3_ctx, DEV_POWER_OFF);
        if(unlikely(ret)){
            return -1;
        }
        g_lsm6ds3flag = 1;
    }
    else{
        LOG("%s %s gyro do not need reset\n", SENSOR_STR, __func__);
    }

    /* update the phy sensor info to sensor hal */
    LOG("%s %s successfully \n", SENSOR_STR, __func__);
    return 0;
}



SENSOR_DRV_ADD(drv_acc_st_lsm6ds3_init);
SENSOR_DRV_ADD(drv_gyro_st_lsm6ds3_init);

