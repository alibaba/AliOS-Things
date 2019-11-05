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


#define LIS2HH12_I2C_ADDR1                 		(0x1E)
#define LIS2HH12_I2C_ADDR2                 		(0x1D)
#define LIS2HH12_I2C_ADDR_TRANS(n)         		((n)<<1)  
#define LIS2HH12_I2C_ADDR                  		LIS2HH12_I2C_ADDR_TRANS(LIS2HH12_I2C_ADDR2)

#define LIS2HH12_ACC_TEMP_L            			0x0B
#define LIS2HH12_ACC_TEMP_H            			0x0C

#define LIS2HH12_ACC_WHO_AM_I              		0x0F
#define LIS2HH12_ACC_ACT_THS             		0x1E
#define LIS2HH12_ACC_ACT_DUR	          		0x1F

#define LIS2HH12_ACC_CTRL_REG1             		0x20
#define LIS2HH12_ACC_CTRL_REG2             		0x21
#define LIS2HH12_ACC_CTRL_REG3             		0x22
#define LIS2HH12_ACC_CTRL_REG4             		0x23
#define LIS2HH12_ACC_CTRL_REG5             		0x24
#define LIS2HH12_ACC_CTRL_REG6            		0x25
#define LIS2HH12_ACC_CTRL_REG7            		0x26
#define LIS2HH12_ACC_STATUS_REG           		0x27
#define LIS2HH12_ACC_OUT_X_L              		0x28
#define LIS2HH12_ACC_OUT_X_H              		0x29
#define LIS2HH12_ACC_OUT_Y_L              		0x2A
#define LIS2HH12_ACC_OUT_Y_H              		0x2B
#define LIS2HH12_ACC_OUT_Z_L              		0x2C
#define LIS2HH12_ACC_OUT_Z_H              		0x2D

#define LIS2HH12_ACC_RANGE_2G               	(0x0)
#define LIS2HH12_ACC_RANGE_4G               	(0x2)
#define LIS2HH12_ACC_RANGE_8G               	(0x3)
#define LIS2HH12_ACC_RANGE_MSK              	(0x30)
#define LIS2HH12_ACC_RANGE_POS              	(4)

#define LIS2HH12_ACC_SENSITIVITY_2G         	(61)
#define LIS2HH12_ACC_SENSITIVITY_4G         	(122)
#define LIS2HH12_ACC_SENSITIVITY_8G         	(244)

#define LIS2HH12_ACC_CHIP_ID_VALUE				(0x41)

#define LIS2HH12_SHIFT_EIGHT_BITS           	(8)
#define LIS2HH12_SHIFT_FOUR_BITS             	(4)

#define LIS2HH12_16_BIT_SHIFT               	(0xFF)

#define LIS2HH12_ACC_ODR_POWER_DOWN				(0x00)
#define LIS2HH12_ACC_ODR_10_HZ               	(0x01)
#define LIS2HH12_ACC_ODR_50_HZ              	(0x02)
#define LIS2HH12_ACC_ODR_100_HZ              	(0x03)
#define LIS2HH12_ACC_ODR_200_HZ              	(0x04)
#define LIS2HH12_ACC_ODR_400_HZ              	(0x05)
#define LIS2HH12_ACC_ODR_800_HZ              	(0x06)
#define LIS2HH12_ACC_ODR_MSK                 	(0X70)
#define LIS2HH12_ACC_ODR_POS                 	(4)

#define LIS2HH12_ENABLE_SOFT_RESET_VALUE		(0x40)
#define LIS2HH12_ACC_DEFAULT_ODR_100HZ          (100)

#define LIS2HH12_BDU_ENABLE						(0x08)

#define LIS2HH12_ACC_STATUS_ZYXDA				(0x08)

#define LIS2HH12_GET_BITSLICE(regvar, bitname)\
((regvar & bitname##_MSK) >> bitname##_POS)
	
#define LIS2HH12_SET_BITSLICE(regvar, bitname, val)\
	((regvar & ~bitname##_MSK) | ((val<<bitname##_POS)&bitname##_MSK))


static int32_t lis2hh12_acc_factor[ACC_RANGE_MAX] = { LIS2HH12_ACC_SENSITIVITY_2G, LIS2HH12_ACC_SENSITIVITY_4G, 
                                     LIS2HH12_ACC_SENSITIVITY_8G};
static int32_t cur_acc_factor = 0;


i2c_dev_t lis2hh12_ctx = {
    .port = 3,
    .config.address_width = 8,
    .config.freq = 400000,
    .config.dev_addr = LIS2HH12_I2C_ADDR,
};


static int drv_acc_st_lis2hh12_soft_reset(i2c_dev_t* drv)
{
    int ret = 0;
    uint8_t value = LIS2HH12_ENABLE_SOFT_RESET_VALUE;
    ret = sensor_i2c_write(drv, LIS2HH12_ACC_CTRL_REG5, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return -1;
    }
    return 0;
}

static int drv_acc_st_lis2hh12_validate_id(i2c_dev_t* drv, uint8_t id_value)
{
    uint8_t value = 0x00;
    int ret = 0;

    if(drv == NULL){
        return -1;
    }
    
	aos_msleep(20);
	
    ret = sensor_i2c_read(drv, LIS2HH12_ACC_WHO_AM_I, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }
	
    if (id_value != value){
        return -1;
    }
    
    return 0;
}

static int drv_acc_st_lis2hh12_set_power_mode(i2c_dev_t* drv, dev_power_mode_e mode)
{
    uint8_t value = 0x00;
    int ret = 0;
    
    ret = sensor_i2c_read(drv, LIS2HH12_ACC_CTRL_REG1, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }

    switch(mode){
        case DEV_POWER_ON:{
            value = LIS2HH12_SET_BITSLICE(value,LIS2HH12_ACC_ODR,LIS2HH12_ACC_ODR_10_HZ);
            ret = sensor_i2c_write(drv, LIS2HH12_ACC_CTRL_REG1, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
            if(unlikely(ret)){
                return ret;
            }
        }break;
        
        case DEV_POWER_OFF:{
            value = LIS2HH12_SET_BITSLICE(value,LIS2HH12_ACC_ODR,LIS2HH12_ACC_ODR_POWER_DOWN);
            ret = sensor_i2c_write(drv, LIS2HH12_ACC_CTRL_REG1, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
            if(unlikely(ret)){
                return ret;
            }
        }break;
            
        case DEV_SLEEP:{
            value = LIS2HH12_SET_BITSLICE(value,LIS2HH12_ACC_ODR,LIS2HH12_ACC_ODR_10_HZ);
            ret = sensor_i2c_write(drv, LIS2HH12_ACC_CTRL_REG1, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
            if(unlikely(ret)){
                return ret;
            }

        }break;

       default:break;
    }
    return 0;
}

static uint8_t drv_acc_st_lis2hh12_hz2odr(uint32_t hz)
{
    if(hz > 400)
        return LIS2HH12_ACC_ODR_800_HZ;
    else if(hz > 200)
        return LIS2HH12_ACC_ODR_400_HZ;
    else if(hz > 100)
        return LIS2HH12_ACC_ODR_200_HZ;
    else if(hz > 50)
        return LIS2HH12_ACC_ODR_100_HZ;
    else if(hz > 10)
        return LIS2HH12_ACC_ODR_50_HZ;
    else
        return LIS2HH12_ACC_ODR_10_HZ;

}

static int drv_acc_st_lis2hh12_set_odr(i2c_dev_t* drv, uint32_t hz)
{
    int ret = 0;
    uint8_t value = 0x00;
    uint8_t odr = drv_acc_st_lis2hh12_hz2odr(hz);

    ret = sensor_i2c_read(drv, LIS2HH12_ACC_CTRL_REG1, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }

    value = LIS2HH12_SET_BITSLICE(value,LIS2HH12_ACC_ODR,odr);
	
    ret = sensor_i2c_write(drv, LIS2HH12_ACC_CTRL_REG1, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }
    return 0;    
}

static int drv_acc_st_lis2hh12_set_range(i2c_dev_t* drv, uint32_t range)
{

    int ret = 0;
    uint8_t value = 0x00;
    uint8_t tmp = 0;

    ret = sensor_i2c_read(drv, LIS2HH12_ACC_CTRL_REG4, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }

    switch (range){
        case ACC_RANGE_2G:{
            tmp = LIS2HH12_ACC_RANGE_2G;
        }break;
        
        case ACC_RANGE_4G:{
            tmp = LIS2HH12_ACC_RANGE_4G;
        }break;
        
        case ACC_RANGE_8G:{
            tmp = LIS2HH12_ACC_RANGE_8G;
        }break;
        
        default:break;
    }
    
    value  = LIS2HH12_SET_BITSLICE(value,LIS2HH12_ACC_RANGE,tmp);
    
	LOG("LIS2HH12 SetRang %2x:0x%2x\n", range, value);
    ret = sensor_i2c_write(drv, LIS2HH12_ACC_CTRL_REG4, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }

    if((range >= ACC_RANGE_2G)&&(range <= ACC_RANGE_8G)){
        cur_acc_factor = lis2hh12_acc_factor[range];
    }
    
    return 0;
}


static int drv_acc_st_lis2hh12_set_bdu(i2c_dev_t* drv)
{
    uint8_t value = 0x00;
    int ret = 0;
    
    ret = sensor_i2c_read(drv, LIS2HH12_ACC_CTRL_REG1, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }
    value |= LIS2HH12_BDU_ENABLE;
    
    ret = sensor_i2c_write(drv, LIS2HH12_ACC_CTRL_REG1, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }
    return 0;
}

static void drv_acc_st_lis2hh12_irq_handle(void)
{
    /* no handle so far */
}

static int drv_acc_st_lis2hh12_open(void)
{
    int ret = 0;
    ret  = drv_acc_st_lis2hh12_set_power_mode(&lis2hh12_ctx, DEV_POWER_ON);
    if(unlikely(ret)){
        return -1;
    }

    ret = drv_acc_st_lis2hh12_set_range(&lis2hh12_ctx, ACC_RANGE_8G);
    if(unlikely(ret)){
        return -1;
    }

    ret = drv_acc_st_lis2hh12_set_odr(&lis2hh12_ctx, LIS2HH12_ACC_DEFAULT_ODR_100HZ);
    if(unlikely(ret)){
        return -1;
    }
     
    return 0;

}

static int drv_acc_st_lis2hh12_close(void)
{
    int ret = 0;
    ret  = drv_acc_st_lis2hh12_set_power_mode(&lis2hh12_ctx, DEV_POWER_OFF);
    if(unlikely(ret)){
        return -1;
    }
    return 0;
}

static int drv_acc_st_lis2hh12_read(void *buf, size_t len)
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
	
	ret = sensor_i2c_read(&lis2hh12_ctx, (LIS2HH12_ACC_OUT_X_L | 0x80), reg, 6, I2C_OP_RETRIES);
	
	if(unlikely(ret)){
        return -1;
    }
    accel->data[DATA_AXIS_X] = (int16_t)((((int16_t)((int8_t)reg[1]))<< LIS2HH12_SHIFT_EIGHT_BITS)|(reg[0]));
    accel->data[DATA_AXIS_Y] = (int16_t)((((int16_t)((int8_t)reg[3]))<< LIS2HH12_SHIFT_EIGHT_BITS)|(reg[2]));
    accel->data[DATA_AXIS_Z] = (int16_t)((((int16_t)((int8_t)reg[5]))<< LIS2HH12_SHIFT_EIGHT_BITS)|(reg[4]));
	
    if(cur_acc_factor != 0){
        accel->data[DATA_AXIS_X] = accel->data[DATA_AXIS_X] * cur_acc_factor / 1000;
        accel->data[DATA_AXIS_Y] = accel->data[DATA_AXIS_Y] * cur_acc_factor / 1000;
        accel->data[DATA_AXIS_Z] = accel->data[DATA_AXIS_Z] * cur_acc_factor / 1000;
    }
    
    accel->timestamp = aos_now_ms();

    return (int)size;
}

static int drv_acc_st_lis2hh12_ioctl(int cmd, unsigned long arg)
{
    int ret = 0;
    dev_sensor_info_t *info = (dev_sensor_info_t *)arg;
    
    switch(cmd){
        case SENSOR_IOCTL_ODR_SET:{
            ret = drv_acc_st_lis2hh12_set_odr(&lis2hh12_ctx, arg);
            if(unlikely(ret)){
                return -1;
            }
        }break;
        case SENSOR_IOCTL_RANGE_SET:{
            ret = drv_acc_st_lis2hh12_set_range(&lis2hh12_ctx, arg);
            if(unlikely(ret)){
                return -1;
            }
        }break;
        case SENSOR_IOCTL_SET_POWER:{
            ret = drv_acc_st_lis2hh12_set_power_mode(&lis2hh12_ctx, arg);
            if(unlikely(ret)){
                return -1;
            }
        }break;
        case SENSOR_IOCTL_GET_INFO:{ 
            /* fill the dev info here */
            info->model = "LIS2HH12";
            info->range_max = 8;
            info->range_min = 2;
            info->unit = mg;
			
        }break;
       default:break;
    }

    return 0;
}

int drv_acc_st_lis2hh12_init(void){
    int ret = 0;
    sensor_obj_t sensor;
    memset(&sensor, 0, sizeof(sensor));

    /* fill the sensor obj parameters here */
    sensor.io_port    = I2C_PORT;
    sensor.tag        = TAG_DEV_ACC;
    sensor.path       = dev_acc_path;
    sensor.open       = drv_acc_st_lis2hh12_open;
    sensor.close      = drv_acc_st_lis2hh12_close;
    sensor.read       = drv_acc_st_lis2hh12_read;
    sensor.write      = NULL;
    sensor.ioctl      = drv_acc_st_lis2hh12_ioctl;
    sensor.irq_handle = drv_acc_st_lis2hh12_irq_handle;

    ret = sensor_create_obj(&sensor);
    if(unlikely(ret)){
        return -1;
    }

    ret = drv_acc_st_lis2hh12_validate_id(&lis2hh12_ctx, LIS2HH12_ACC_CHIP_ID_VALUE);
    if(unlikely(ret)){
        return -1;
    }
	
    ret = drv_acc_st_lis2hh12_soft_reset(&lis2hh12_ctx);
    if(unlikely(ret)){
        return -1;
    }

    ret = drv_acc_st_lis2hh12_set_range(&lis2hh12_ctx, ACC_RANGE_8G);
    if(unlikely(ret)){
        return -1;
    }

	ret = drv_acc_st_lis2hh12_set_bdu(&lis2hh12_ctx);
    if(unlikely(ret)){
        return -1;
    }

    //set odr is 100hz, and will update
    ret = drv_acc_st_lis2hh12_set_odr(&lis2hh12_ctx, LIS2HH12_ACC_DEFAULT_ODR_100HZ);
    if(unlikely(ret)){
        return -1;
    }

    /* update the phy sensor info to sensor hal */
    LOG("%s %s successfully \n", SENSOR_STR, __func__);
    return 0;
}

SENSOR_DRV_ADD(drv_acc_st_lis2hh12_init);


