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


#define LSM6DSR_I2C_ADDR1                   (0x6A)
#define LSM6DSR_I2C_ADDR2                   (0x6B)
#define LSM6DSR_I2C_ADDR_TRANS(n)           ((n)<<1)  
#define LSM6DSR_I2C_ADDR                    LSM6DSR_I2C_ADDR_TRANS(LSM6DSR_I2C_ADDR2)

#define LSM6DSR_ACC_GYRO_FUNC_CFG_ACCESS    0x01
#define LSM6DSR_ACC_GYRO_PIN_CTRL   	0x02
#define LSM6DSR_ACC_GYRO_FIFO_CTRL1         0x07
#define LSM6DSR_ACC_GYRO_FIFO_CTRL2         0x08
#define LSM6DSR_ACC_GYRO_FIFO_CTRL3         0x09
#define LSM6DSR_ACC_GYRO_FIFO_CTRL4         0x0A
#define LSM6DSR_ACC_GYRO_COUNTER_BDR_REG1   0x0B
#define LSM6DSR_ACC_GYRO_COUNTER_BDR_REG2   0x0C
#define LSM6DSR_ACC_GYRO_INT1_CTRL          0x0D
#define LSM6DSR_ACC_GYRO_INT2_CTRL          0x0E
#define LSM6DSR_ACC_GYRO_WHO_AM_I_REG       0x0F
#define LSM6DSR_ACC_GYRO_CTRL1_XL           0x10
#define LSM6DSR_ACC_GYRO_CTRL2_G            0x11
#define LSM6DSR_ACC_GYRO_CTRL3_C            0x12
#define LSM6DSR_ACC_GYRO_CTRL4_C            0x13
#define LSM6DSR_ACC_GYRO_CTRL5_C            0x14
#define LSM6DSR_ACC_GYRO_CTRL6_C            0x15
#define LSM6DSR_ACC_GYRO_CTRL7_G            0x16
#define LSM6DSR_ACC_GYRO_CTRL8_XL           0x17
#define LSM6DSR_ACC_GYRO_CTRL9_XL           0x18
#define LSM6DSR_ACC_GYRO_CTRL10_C           0x19

#define LSM6DSR_ACC_GYRO_ALL_INT_SRC      0x1A
#define LSM6DSR_ACC_GYRO_WAKE_UP_SRC        0x1B
#define LSM6DSR_ACC_GYRO_TAP_SRC            0x1C
#define LSM6DSR_ACC_GYRO_D6D_SRC            0x1D
#define LSM6DSR_ACC_GYRO_STATUS_REG         0x1E

#define LSM6DSR_ACC_GYRO_OUT_TEMP_L         0x20
#define LSM6DSR_ACC_GYRO_OUT_TEMP_H         0x21
#define LSM6DSR_ACC_GYRO_OUTX_L_G           0x22
#define LSM6DSR_ACC_GYRO_OUTX_H_G           0x23
#define LSM6DSR_ACC_GYRO_OUTY_L_G           0x24
#define LSM6DSR_ACC_GYRO_OUTY_H_G           0x25
#define LSM6DSR_ACC_GYRO_OUTZ_L_G           0x26
#define LSM6DSR_ACC_GYRO_OUTZ_H_G           0x27
#define LSM6DSR_ACC_GYRO_OUTX_L_XL          0x28
#define LSM6DSR_ACC_GYRO_OUTX_H_XL          0x29
#define LSM6DSR_ACC_GYRO_OUTY_L_XL          0x2A
#define LSM6DSR_ACC_GYRO_OUTY_H_XL          0x2B
#define LSM6DSR_ACC_GYRO_OUTZ_L_XL          0x2C
#define LSM6DSR_ACC_GYRO_OUTZ_H_XL          0x2D

#define LSM6DSR_ACC_GYRO_FIFO_STATUS1       0x3A
#define LSM6DSR_ACC_GYRO_FIFO_STATUS2       0x3B

#define LSM6DSR_ACC_GYRO_TIMESTAMP0_REG     0x40
#define LSM6DSR_ACC_GYRO_TIMESTAMP1_REG     0x41
#define LSM6DSR_ACC_GYRO_TIMESTAMP2_REG     0x42
#define LSM6DSR_ACC_GYRO_TIMESTAMP3_REG     0x43

#define LSM6DSR_ACC_GYRO_TAP_CFG0           0x56
#define LSM6DSR_ACC_GYRO_TAP_CFG1           0x57
#define LSM6DSR_ACC_GYRO_TAP_CFG2           0x58
#define LSM6DSR_ACC_GYRO_TAP_THS_6D         0x59
#define LSM6DSR_ACC_GYRO_INT_DUR2           0x5A
#define LSM6DSR_ACC_GYRO_WAKE_UP_THS        0x5B
#define LSM6DSR_ACC_GYRO_WAKE_UP_DUR        0x5C
#define LSM6DSR_ACC_GYRO_FREE_FALL          0x5D
#define LSM6DSR_ACC_GYRO_MD1_CFG            0x5E
#define LSM6DSR_ACC_GYRO_MD2_CFG            0x5F

#define LSM6DSR_ACC_GYRO_X_OFS_USR          0x73
#define LSM6DSR_ACC_GYRO_Y_OFS_USR          0x74
#define LSM6DSR_ACC_GYRO_Z_OFS_USR          0x75

#define LSM6DSR_CHIP_ID_VALUE               (0x6B)

#define LSM6DSR_RESET_VALUE                 (0x1)
#define LSM6DSR_RESET_MSK                   (0X1)
#define LSM6DSR_RESET_POS                   (0)

#define LSM6DSR_BDU_VALUE                 (0x40)
#define LSM6DSR_BDU_MSK                   (0X40)
#define LSM6DSR_BDU_POS                   (6)

#define LSM6DSR_ACC_ODR_POWER_DOWN          (0X00)
#define LSM6DSR_ACC_ODR_1_6_HZ              (0X0B)
#define LSM6DSR_ACC_ODR_12_5_HZ             (0x01)
#define LSM6DSR_ACC_ODR_26_HZ               (0x02)
#define LSM6DSR_ACC_ODR_52_HZ               (0x03)
#define LSM6DSR_ACC_ODR_104_HZ              (0x04)
#define LSM6DSR_ACC_ODR_208_HZ              (0x05)
#define LSM6DSR_ACC_ODR_416_HZ              (0x06)
#define LSM6DSR_ACC_ODR_833_HZ              (0x07)
#define LSM6DSR_ACC_ODR_1_66_KHZ            (0x08)
#define LSM6DSR_ACC_ODR_3_33_KHZ            (0x09)
#define LSM6DSR_ACC_ODR_6_66_KHZ            (0x0A)
#define LSM6DSR_ACC_ODR_MSK                 (0XF0)
#define LSM6DSR_ACC_ODR_POS                 (4)

#define LSM6DSR_GYRO_ODR_POWER_DOWN         (0X00)
#define LSM6DSR_GYRO_ODR_12_5_HZ            (0x01)
#define LSM6DSR_GYRO_ODR_26_HZ              (0x02)
#define LSM6DSR_GYRO_ODR_52_HZ              (0x03)
#define LSM6DSR_GYRO_ODR_104_HZ             (0x04)
#define LSM6DSR_GYRO_ODR_208_HZ             (0x05)
#define LSM6DSR_GYRO_ODR_416_HZ             (0x06)
#define LSM6DSR_GYRO_ODR_833_HZ             (0x07)
#define LSM6DSR_GYRO_ODR_1_66_KHZ           (0x08)
#define LSM6DSR_GYRO_ODR_3_33_KHZ           (0x09)
#define LSM6DSR_GYRO_ODR_6_66_KHZ           (0x0A)
#define LSM6DSR_GYRO_ODR_MSK                (0XF0)
#define LSM6DSR_GYRO_ODR_POS                (4)

#define LSM6DSR_ACC_RANGE_2G                (0x0)
#define LSM6DSR_ACC_RANGE_4G                (0x2)
#define LSM6DSR_ACC_RANGE_8G                (0x3)
#define LSM6DSR_ACC_RANGE_16G               (0x1)
#define LSM6DSR_ACC_RANGE_MSK               (0X0C)
#define LSM6DSR_ACC_RANGE_POS               (2)

#define LSM6DSR_ACC_SENSITIVITY_2G          (61)
#define LSM6DSR_ACC_SENSITIVITY_4G          (122)
#define LSM6DSR_ACC_SENSITIVITY_8G          (244)
#define LSM6DSR_ACC_SENSITIVITY_16G         (488) 

#define LSM6DSR_GYRO_RANGE_245              (0x0)
#define LSM6DSR_GYRO_RANGE_500              (0x1)
#define LSM6DSR_GYRO_RANGE_1000             (0x2)
#define LSM6DSR_GYRO_RANGE_2000             (0x3)
#define LSM6DSR_GYRO_RANGE_MSK              (0X0C)
#define LSM6DSR_GYRO_RANGE_POS              (2)

#define LSM6DSR_GYRO_SENSITIVITY_245DPS     (8750)
#define LSM6DSR_GYRO_SENSITIVITY_500DPS     (17500)
#define LSM6DSR_GYRO_SENSITIVITY_1000DPS    (35000)
#define LSM6DSR_GYRO_SENSITIVITY_2000DPS    (70000)

#define LSM6DSR_SHIFT_EIGHT_BITS            (8)
#define LSM6DSR_16_BIT_SHIFT                (0xFF)
#define LSM6DSR_ACC_MUL                     (1000)
#define LSM6DSR_GYRO_MUL                    (1)

#define LSM6DSR_ACC_DEFAULT_ODR_100HZ       (100)
#define LSM6DSR_GYRO_DEFAULT_ODR_100HZ      (100)

#define LSM6DSR_GET_BITSLICE(regvar, bitname)\
((regvar & bitname##_MSK) >> bitname##_POS)

#define LSM6DSR_SET_BITSLICE(regvar, bitname, val)\
((regvar & ~bitname##_MSK) | ((val<<bitname##_POS)&bitname##_MSK))

static int32_t lsm6dsr_acc_factor[ACC_RANGE_MAX] = { LSM6DSR_ACC_SENSITIVITY_2G, LSM6DSR_ACC_SENSITIVITY_4G, 
                                     LSM6DSR_ACC_SENSITIVITY_8G, LSM6DSR_ACC_SENSITIVITY_16G };
static int32_t lsm6dsr_gyro_factor[GYRO_RANGE_MAX] = {0, LSM6DSR_GYRO_SENSITIVITY_245DPS, LSM6DSR_GYRO_SENSITIVITY_500DPS, 
                                     LSM6DSR_GYRO_SENSITIVITY_1000DPS, LSM6DSR_GYRO_SENSITIVITY_2000DPS };
static int32_t cur_acc_factor = 0;
static int32_t cur_gyro_factor = 0;
static int32_t g_lsm6dsrflag = 0;

i2c_dev_t lsm6dsr_ctx = {
    //.port = 4,
    .port = 3,
    .config.address_width = 8,
    .config.freq = 400000,
    .config.dev_addr = LSM6DSR_I2C_ADDR,
};

static int drv_acc_gyro_st_lsm6dsr_soft_reset(i2c_dev_t* drv)
{
    int ret = 0;
    uint8_t value = 0x00;

    ret = sensor_i2c_read(drv, LSM6DSR_ACC_GYRO_CTRL3_C, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }
    
    value |= LSM6DSR_RESET_VALUE;
    
    ret = sensor_i2c_write(drv, LSM6DSR_ACC_GYRO_CTRL3_C, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return -1;
    }
    return 0;
}

static int drv_acc_gyro_st_lsm6dsr_validate_id(i2c_dev_t* drv, uint8_t id_value)
{
    uint8_t value = 0x00;
    int ret = 0;

    if(drv == NULL){
        return -1;
    }

    ret = sensor_i2c_read(drv, LSM6DSR_ACC_GYRO_WHO_AM_I_REG, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }
    
    LOG("%s %s right id (0x%02x), read id(0x%02x)\n", SENSOR_STR, __func__, id_value, value);
    if (id_value != value){
        return -1;
    }
    
    return 0;
}

static int drv_acc_st_lsm6dsr_set_power_mode(i2c_dev_t* drv, dev_power_mode_e mode)
{
    uint8_t value = 0x00;
    int ret = 0;
    
    ret = sensor_i2c_read(drv, LSM6DSR_ACC_GYRO_CTRL1_XL, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }

    switch(mode){
        case DEV_POWER_ON:{
            
            value = LSM6DSR_SET_BITSLICE(value,LSM6DSR_ACC_ODR,LSM6DSR_ACC_ODR_12_5_HZ);
            ret = sensor_i2c_write(drv, LSM6DSR_ACC_GYRO_CTRL1_XL, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
            if(unlikely(ret)){
                return ret;
            }
        }break;
        
        case DEV_POWER_OFF:{
            value = LSM6DSR_SET_BITSLICE(value,LSM6DSR_ACC_ODR,LSM6DSR_ACC_ODR_POWER_DOWN);
            ret = sensor_i2c_write(drv, LSM6DSR_ACC_GYRO_CTRL1_XL, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
            if(unlikely(ret)){
                return ret;
            }
        }break;
            
        case DEV_SLEEP:{
            value = LSM6DSR_SET_BITSLICE(value,LSM6DSR_ACC_ODR,LSM6DSR_ACC_ODR_12_5_HZ);
            ret = sensor_i2c_write(drv, LSM6DSR_ACC_GYRO_CTRL1_XL, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
            if(unlikely(ret)){
                return ret;
            }

        }break;
        
       default:break;
    }
    return 0;
}

static int drv_acc_gyro_st_lsm6dsr_set_bdu(i2c_dev_t* drv)
{
    int ret = 0;
    uint8_t value = 0x00;

    ret = sensor_i2c_read(drv, LSM6DSR_ACC_GYRO_CTRL3_C, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }

    if (value & LSM6DSR_BDU_VALUE)
        return 0;
    
    value |= LSM6DSR_BDU_VALUE;
    
    ret = sensor_i2c_write(drv, LSM6DSR_ACC_GYRO_CTRL3_C, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return -1;
    }
    return 0;
}

static uint8_t drv_acc_st_lsm6dsr_hz2odr(uint32_t hz)
{
    if(hz > 3330)
        return LSM6DSR_ACC_ODR_6_66_KHZ;
    else if(hz > 1660)
        return LSM6DSR_ACC_ODR_3_33_KHZ;
    else if(hz > 833)
        return LSM6DSR_ACC_ODR_1_66_KHZ;
    else if(hz > 416)
        return LSM6DSR_ACC_ODR_833_HZ;
    else if(hz > 208)
        return LSM6DSR_ACC_ODR_416_HZ;
    else if(hz > 104)
        return LSM6DSR_ACC_ODR_208_HZ;
    else if(hz > 52)
        return LSM6DSR_ACC_ODR_104_HZ;
    else if(hz > 26)
        return LSM6DSR_ACC_ODR_52_HZ;
    else if(hz > 13)
        return LSM6DSR_ACC_ODR_26_HZ;
    else if(hz >= 2)
        return LSM6DSR_ACC_ODR_12_5_HZ;
    else
        return LSM6DSR_ACC_ODR_1_6_HZ;

}


static int drv_acc_st_lsm6dsr_set_odr(i2c_dev_t* drv, uint32_t hz)
{
    int ret = 0;
    uint8_t value = 0x00;
    uint8_t odr = drv_acc_st_lsm6dsr_hz2odr(hz);

    ret = sensor_i2c_read(drv, LSM6DSR_ACC_GYRO_CTRL1_XL, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }

    value = LSM6DSR_SET_BITSLICE(value,LSM6DSR_ACC_ODR,odr);

    ret = sensor_i2c_write(drv, LSM6DSR_ACC_GYRO_CTRL1_XL, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }
    return 0;    
}

static int drv_acc_st_lsm6dsr_set_range(i2c_dev_t* drv, uint32_t range)
{
    int ret = 0;
    uint8_t value = 0x00;
    uint8_t tmp = 0;

    ret = sensor_i2c_read(drv, LSM6DSR_ACC_GYRO_CTRL1_XL, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }

    switch (range){
        case ACC_RANGE_2G:{
            tmp = LSM6DSR_ACC_RANGE_2G;
        }break;
        
        case ACC_RANGE_4G:{
            tmp = LSM6DSR_ACC_RANGE_4G;
        }break;
        
        case ACC_RANGE_8G:{
            tmp = LSM6DSR_ACC_RANGE_8G;
        }break;
            
        case ACC_RANGE_16G:{
            tmp = LSM6DSR_ACC_RANGE_16G;
        }break;
        
        default:break;
    }
    
    value  = LSM6DSR_SET_BITSLICE(value,LSM6DSR_ACC_RANGE,tmp);
    ret = sensor_i2c_write(drv, LSM6DSR_ACC_GYRO_CTRL1_XL, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }

    if((range >= ACC_RANGE_2G)&&(range <= ACC_RANGE_16G)){
        cur_acc_factor = lsm6dsr_acc_factor[range];
    }
    
    return 0;
}


static void drv_acc_st_lsm6dsr_irq_handle(void)
{
    /* no handle so far */
}

static int drv_acc_st_lsm6dsr_open(void)
{
    int ret = 0;
    
    ret  = drv_acc_st_lsm6dsr_set_power_mode(&lsm6dsr_ctx, DEV_POWER_ON);
    if(unlikely(ret)){
        return -1;
    }

    ret  = drv_acc_gyro_st_lsm6dsr_set_bdu(&lsm6dsr_ctx);
    if(unlikely(ret)){
        return -1;
    }

    ret = drv_acc_st_lsm6dsr_set_range(&lsm6dsr_ctx, ACC_RANGE_8G);
    if(unlikely(ret)){
        return -1;
    }

    ret = drv_acc_st_lsm6dsr_set_odr(&lsm6dsr_ctx, LSM6DSR_ACC_DEFAULT_ODR_100HZ);
    if(unlikely(ret)){
        return -1;
    }
     
    return 0;

}

static int drv_acc_st_lsm6dsr_close(void)
{
    int ret = 0;
    ret  = drv_acc_st_lsm6dsr_set_power_mode(&lsm6dsr_ctx, DEV_POWER_OFF);
    if(unlikely(ret)){
        return -1;
    }
    return 0;
}

static int drv_acc_st_lsm6dsr_read(void *buf, size_t len)
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

    ret  = sensor_i2c_read(&lsm6dsr_ctx, LSM6DSR_ACC_GYRO_OUTX_L_XL,  &reg[0], I2C_REG_LEN, I2C_OP_RETRIES);
    ret |= sensor_i2c_read(&lsm6dsr_ctx, LSM6DSR_ACC_GYRO_OUTX_H_XL,  &reg[1], I2C_REG_LEN, I2C_OP_RETRIES);
    ret |= sensor_i2c_read(&lsm6dsr_ctx, LSM6DSR_ACC_GYRO_OUTY_L_XL,  &reg[2], I2C_REG_LEN, I2C_OP_RETRIES);
    ret |= sensor_i2c_read(&lsm6dsr_ctx, LSM6DSR_ACC_GYRO_OUTY_H_XL,  &reg[3], I2C_REG_LEN, I2C_OP_RETRIES);
    ret |= sensor_i2c_read(&lsm6dsr_ctx, LSM6DSR_ACC_GYRO_OUTZ_L_XL,  &reg[4], I2C_REG_LEN, I2C_OP_RETRIES);
    ret |= sensor_i2c_read(&lsm6dsr_ctx, LSM6DSR_ACC_GYRO_OUTZ_H_XL,  &reg[5], I2C_REG_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return -1;
    }
    accel->data[DATA_AXIS_X] = (int16_t)((((int16_t)((int8_t)reg[1]))<< LSM6DSR_SHIFT_EIGHT_BITS)|(reg[0]));
    accel->data[DATA_AXIS_Y] = (int16_t)((((int16_t)((int8_t)reg[3]))<< LSM6DSR_SHIFT_EIGHT_BITS)|(reg[2]));
    accel->data[DATA_AXIS_Z] = (int16_t)((((int16_t)((int8_t)reg[5]))<< LSM6DSR_SHIFT_EIGHT_BITS)|(reg[4]));

    if(cur_acc_factor != 0){
        accel->data[DATA_AXIS_X] = (accel->data[DATA_AXIS_X] * cur_acc_factor)/LSM6DSR_ACC_MUL;
        accel->data[DATA_AXIS_Y] = (accel->data[DATA_AXIS_Y] * cur_acc_factor)/LSM6DSR_ACC_MUL;
        accel->data[DATA_AXIS_Z] = (accel->data[DATA_AXIS_Z] * cur_acc_factor)/LSM6DSR_ACC_MUL;
    }
    
    accel->timestamp = aos_now_ms();

    return (int)size;
}

static int drv_acc_st_lsm6dsr_ioctl(int cmd, unsigned long arg)
{
    int ret = 0;
    
    switch(cmd){
        case SENSOR_IOCTL_ODR_SET:{
            ret = drv_acc_st_lsm6dsr_set_odr(&lsm6dsr_ctx, arg);
            if(unlikely(ret)){
                return -1;
            }
        }break;
        case SENSOR_IOCTL_RANGE_SET:{
            ret = drv_acc_st_lsm6dsr_set_range(&lsm6dsr_ctx, arg);
            if(unlikely(ret)){
                return -1;
            }
        }break;
        case SENSOR_IOCTL_SET_POWER:{
            ret = drv_acc_st_lsm6dsr_set_power_mode(&lsm6dsr_ctx, arg);
            if(unlikely(ret)){
                return -1;
            }
        }break;
        case SENSOR_IOCTL_GET_INFO:{ 
            /* fill the dev info here */
            dev_sensor_info_t *info = (dev_sensor_info_t*)arg;
            info->model = "LSM6DSR";
            info->range_max = 16;
            info->range_min = 2;
            info->unit = mg;
        }break;
       
       default:break;
    }

    return 0;
}


int drv_acc_st_lsm6dsr_init(void){
    int ret = 0;
    sensor_obj_t sensor;
    memset(&sensor, 0, sizeof(sensor));

    /* fill the sensor obj parameters here */
    sensor.io_port    = I2C_PORT;
    sensor.tag        = TAG_DEV_ACC;
    sensor.path       = dev_acc_path;
    sensor.open       = drv_acc_st_lsm6dsr_open;
    sensor.close      = drv_acc_st_lsm6dsr_close;
    sensor.read       = drv_acc_st_lsm6dsr_read;
    sensor.write      = NULL;
    sensor.ioctl      = drv_acc_st_lsm6dsr_ioctl;
    sensor.irq_handle = drv_acc_st_lsm6dsr_irq_handle;

    ret = sensor_create_obj(&sensor);
    if(unlikely(ret)){
        LOG("%s %s %s %d\n", SENSOR_STR, __func__, ERROR_LINE, __LINE__);
        return -1;
    }
    ret = drv_acc_gyro_st_lsm6dsr_validate_id(&lsm6dsr_ctx, LSM6DSR_CHIP_ID_VALUE);
    if(unlikely(ret)){
        LOG("%s %s %s %d\n", SENSOR_STR, __func__, ERROR_LINE, __LINE__);
        return -1;
    }

    if(0 == g_lsm6dsrflag)
    {
        ret = drv_acc_gyro_st_lsm6dsr_soft_reset(&lsm6dsr_ctx);
        if(unlikely(ret)){
	    LOG("%s %s %s %d\n", SENSOR_STR, __func__, ERROR_LINE, __LINE__);
            return -1;
        }
        ret = drv_acc_st_lsm6dsr_set_power_mode(&lsm6dsr_ctx, DEV_POWER_OFF);
        if(unlikely(ret)){
	    LOG("%s %s %s %d\n", SENSOR_STR, __func__, ERROR_LINE, __LINE__);
            return -1;
        }
        g_lsm6dsrflag = 1;
    }
    else
    {
        LOG("%s %s acc do not need reset\n", SENSOR_STR, __func__);
    }
    
    LOG("%s %s successfully \n", SENSOR_STR, __func__);
    return 0;
}


static int drv_gyro_st_lsm6dsr_set_power_mode(i2c_dev_t* drv, dev_power_mode_e mode)
{
    uint8_t value = 0x00;
    int ret = 0;
    
    ret = sensor_i2c_read(drv, LSM6DSR_ACC_GYRO_CTRL2_G, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }

    switch(mode){
        case DEV_POWER_ON:{
            
            value = LSM6DSR_SET_BITSLICE(value,LSM6DSR_GYRO_ODR,LSM6DSR_GYRO_ODR_12_5_HZ);
            ret = sensor_i2c_write(drv, LSM6DSR_ACC_GYRO_CTRL2_G, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
            if(unlikely(ret)){
                return ret;
            }
        }break;
        
        case DEV_POWER_OFF:{
            value = LSM6DSR_SET_BITSLICE(value,LSM6DSR_GYRO_ODR,LSM6DSR_GYRO_ODR_POWER_DOWN);
            ret = sensor_i2c_write(drv, LSM6DSR_ACC_GYRO_CTRL2_G, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
            if(unlikely(ret)){
                return ret;
            }
        }break;
            
        case DEV_SLEEP:{
            value = LSM6DSR_SET_BITSLICE(value,LSM6DSR_GYRO_ODR,LSM6DSR_GYRO_ODR_12_5_HZ);
            ret = sensor_i2c_write(drv, LSM6DSR_ACC_GYRO_CTRL2_G, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
            if(unlikely(ret)){
                return ret;
            }

        }break;
        
       default:break;
    }
    return 0;
}

static uint8_t drv_gyro_st_lsm6dsr_hz2odr(uint32_t hz)
{
    if(hz > 3330)
        return LSM6DSR_GYRO_ODR_6_66_KHZ;
    else if(hz > 1660)
        return LSM6DSR_GYRO_ODR_3_33_KHZ;
    else if(hz > 833)
        return LSM6DSR_GYRO_ODR_1_66_KHZ;
    else if(hz > 416)
        return LSM6DSR_GYRO_ODR_833_HZ;
    else if(hz > 208)
        return LSM6DSR_GYRO_ODR_416_HZ;
    else if(hz > 104)
        return LSM6DSR_GYRO_ODR_208_HZ;
    else if(hz > 52)
        return LSM6DSR_GYRO_ODR_104_HZ;
    else if(hz > 26)
        return LSM6DSR_GYRO_ODR_52_HZ;
    else if(hz > 13)
        return LSM6DSR_GYRO_ODR_26_HZ;
    else
        return LSM6DSR_GYRO_ODR_12_5_HZ;

}


static int drv_gyro_st_lsm6dsr_set_odr(i2c_dev_t* drv, uint32_t hz)
{
    int ret = 0;
    uint8_t value = 0x00;
    uint8_t odr = drv_gyro_st_lsm6dsr_hz2odr(hz);

    ret = sensor_i2c_read(drv, LSM6DSR_ACC_GYRO_CTRL2_G, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }

    value = LSM6DSR_SET_BITSLICE(value,LSM6DSR_GYRO_ODR,odr);

    ret = sensor_i2c_write(drv, LSM6DSR_ACC_GYRO_CTRL2_G, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }
    return 0;    
}

static int drv_gyro_st_lsm6dsr_set_range(i2c_dev_t* drv, uint32_t range)
{
    int ret = 0;
    uint8_t value = 0x00;
    uint8_t tmp = 0;

    ret = sensor_i2c_read(drv, LSM6DSR_ACC_GYRO_CTRL2_G, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }

    switch (range){
        case GYRO_RANGE_250DPS:{
            tmp = LSM6DSR_GYRO_RANGE_245;
        }break;
        
        case GYRO_RANGE_500DPS:{
            tmp = LSM6DSR_GYRO_RANGE_500;
        }break;
        
        case GYRO_RANGE_1000DPS:{
            tmp = LSM6DSR_GYRO_RANGE_1000;
        }break;
            
        case GYRO_RANGE_2000DPS:{
            tmp = LSM6DSR_GYRO_RANGE_2000;
        }break;
        
        default:break;
    }
    
    value  = LSM6DSR_SET_BITSLICE(value,LSM6DSR_GYRO_RANGE,tmp);
    ret = sensor_i2c_write(drv, LSM6DSR_ACC_GYRO_CTRL2_G, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }
    
    if((range >= GYRO_RANGE_250DPS)&&(range <= GYRO_RANGE_2000DPS)){
        cur_gyro_factor = lsm6dsr_gyro_factor[range];
    }
    
    return 0;
}


static void drv_gyro_st_lsm6dsr_irq_handle(void)
{
    /* no handle so far */
}

static int drv_gyro_st_lsm6dsr_open(void)
{
    int ret = 0;
    ret  = drv_gyro_st_lsm6dsr_set_power_mode(&lsm6dsr_ctx, DEV_POWER_ON);
    if(unlikely(ret)){
        return -1;
    }

    ret  = drv_acc_gyro_st_lsm6dsr_set_bdu(&lsm6dsr_ctx);
    if(unlikely(ret)){
        return -1;
    }

    ret = drv_gyro_st_lsm6dsr_set_range(&lsm6dsr_ctx, GYRO_RANGE_1000DPS);
    if(unlikely(ret)){
        return -1;
    }

    ret = drv_gyro_st_lsm6dsr_set_odr(&lsm6dsr_ctx, LSM6DSR_GYRO_DEFAULT_ODR_100HZ);
    if(unlikely(ret)){
        return -1;
    }
    
    return 0;

}

static int drv_gyro_st_lsm6dsr_close(void)
{
    int ret = 0;
    ret  = drv_gyro_st_lsm6dsr_set_power_mode(&lsm6dsr_ctx, DEV_POWER_OFF);
    if(unlikely(ret)){
        return -1;
    }
    return 0;
}

static int drv_gyro_st_lsm6dsr_read(void *buf, size_t len)
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

    ret  = sensor_i2c_read(&lsm6dsr_ctx, LSM6DSR_ACC_GYRO_OUTX_L_G,  &reg[0], I2C_REG_LEN, I2C_OP_RETRIES);
    ret |= sensor_i2c_read(&lsm6dsr_ctx, LSM6DSR_ACC_GYRO_OUTX_H_G,  &reg[1], I2C_REG_LEN, I2C_OP_RETRIES);
    ret |= sensor_i2c_read(&lsm6dsr_ctx, LSM6DSR_ACC_GYRO_OUTY_L_G,  &reg[2], I2C_REG_LEN, I2C_OP_RETRIES);
    ret |= sensor_i2c_read(&lsm6dsr_ctx, LSM6DSR_ACC_GYRO_OUTY_H_G,  &reg[3], I2C_REG_LEN, I2C_OP_RETRIES);
    ret |= sensor_i2c_read(&lsm6dsr_ctx, LSM6DSR_ACC_GYRO_OUTZ_L_G,  &reg[4], I2C_REG_LEN, I2C_OP_RETRIES);
    ret |= sensor_i2c_read(&lsm6dsr_ctx, LSM6DSR_ACC_GYRO_OUTZ_H_G,  &reg[5], I2C_REG_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return -1;
    }
    gyro->data[DATA_AXIS_X] = (int16_t)((((int32_t)((int8_t)reg[1]))<< LSM6DSR_SHIFT_EIGHT_BITS)|(reg[0]));
    gyro->data[DATA_AXIS_Y] = (int16_t)((((int32_t)((int8_t)reg[3]))<< LSM6DSR_SHIFT_EIGHT_BITS)|(reg[2]));
    gyro->data[DATA_AXIS_Z] = (int16_t)((((int32_t)((int8_t)reg[5]))<< LSM6DSR_SHIFT_EIGHT_BITS)|(reg[4]));

    if(cur_gyro_factor != 0){
        gyro->data[DATA_AXIS_X] = (gyro->data[DATA_AXIS_X] * cur_gyro_factor)/LSM6DSR_GYRO_MUL;
        gyro->data[DATA_AXIS_Y] = (gyro->data[DATA_AXIS_Y] * cur_gyro_factor)/LSM6DSR_GYRO_MUL;
        gyro->data[DATA_AXIS_Z] = (gyro->data[DATA_AXIS_Z] * cur_gyro_factor)/LSM6DSR_GYRO_MUL;
    }
    gyro->timestamp = aos_now_ms();

    return (int)size;
}

static int drv_gyro_st_lsm6dsr_ioctl(int cmd, unsigned long arg)
{
    int ret = 0;
    
    switch(cmd){
        case SENSOR_IOCTL_ODR_SET:{
            ret = drv_gyro_st_lsm6dsr_set_odr(&lsm6dsr_ctx, arg);
            if(unlikely(ret)){
                return -1;
            }
        }break;
        case SENSOR_IOCTL_RANGE_SET:{
            ret = drv_gyro_st_lsm6dsr_set_range(&lsm6dsr_ctx, arg);
            if(unlikely(ret)){
                return -1;
            }
        }break;
        case SENSOR_IOCTL_SET_POWER:{
            ret = drv_gyro_st_lsm6dsr_set_power_mode(&lsm6dsr_ctx, arg);
            if(unlikely(ret)){
                return -1;
            }
        }break;
        case SENSOR_IOCTL_GET_INFO:{ 
            /* fill the dev info here */
            dev_sensor_info_t *info = (dev_sensor_info_t *)arg;
            info->model = "LSM6DSR";
            info->range_max = 2000;
            info->range_min = 125;
            info->unit = udps;
        }break;
       
       default:break;
    }

    return 0;
}

int drv_gyro_st_lsm6dsr_init(void){
    int ret = 0;
    sensor_obj_t sensor;
    memset(&sensor, 0, sizeof(sensor));

    /* fill the sensor obj parameters here */
    sensor.io_port    = I2C_PORT;
    sensor.tag        = TAG_DEV_GYRO;
    sensor.path       = dev_gyro_path;
    sensor.open       = drv_gyro_st_lsm6dsr_open;
    sensor.close      = drv_gyro_st_lsm6dsr_close;
    sensor.read       = drv_gyro_st_lsm6dsr_read;
    sensor.write      = NULL;
    sensor.ioctl      = drv_gyro_st_lsm6dsr_ioctl;
    sensor.irq_handle = drv_gyro_st_lsm6dsr_irq_handle;

    ret = sensor_create_obj(&sensor);
    if(unlikely(ret)){
        LOG("%s %s %s %d\n", SENSOR_STR, __func__, ERROR_LINE, __LINE__);
        return -1;
    }

    ret = drv_acc_gyro_st_lsm6dsr_validate_id(&lsm6dsr_ctx, LSM6DSR_CHIP_ID_VALUE);
    if(unlikely(ret)){
        LOG("%s %s %s %d\n", SENSOR_STR, __func__, ERROR_LINE, __LINE__);
        return -1;
    }

    if(0 == g_lsm6dsrflag){
        ret = drv_acc_gyro_st_lsm6dsr_soft_reset(&lsm6dsr_ctx);
        if(unlikely(ret)){
	    LOG("%s %s %s %d\n", SENSOR_STR, __func__, ERROR_LINE, __LINE__);
            return -1;
        }

        ret = drv_gyro_st_lsm6dsr_set_power_mode(&lsm6dsr_ctx, DEV_POWER_OFF);
        if(unlikely(ret)){
	    LOG("%s %s %s %d\n", SENSOR_STR, __func__, ERROR_LINE, __LINE__);
            return -1;
        }
        g_lsm6dsrflag = 1;
    }
    else{
        LOG("%s %s gyro do not need reset\n", SENSOR_STR, __func__);
    }

    /* update the phy sensor info to sensor hal */
    LOG("%s %s successfully \n", SENSOR_STR, __func__);
    return 0;
}


SENSOR_DRV_ADD(drv_acc_st_lsm6dsr_init);
SENSOR_DRV_ADD(drv_gyro_st_lsm6dsr_init);


