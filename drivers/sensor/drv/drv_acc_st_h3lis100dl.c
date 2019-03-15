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


#define H3LIS100DL_I2C_ADDR1                 	(0x18)
#define H3LIS100DL_I2C_ADDR2                 	(0x19)
#define H3LIS100DL_I2C_ADDR_TRANS(n)         	((n)<<1)  
#define H3LIS100DL_I2C_ADDR                  	H3LIS100DL_I2C_ADDR_TRANS(H3LIS100DL_I2C_ADDR1)

#define H3LIS100DL_ACC_WHO_AM_I              	0x0F
#define H3LIS100DL_ACC_CTRL_REG1             	0x20
#define H3LIS100DL_ACC_CTRL_REG2             	0x21
#define H3LIS100DL_ACC_CTRL_REG3            	0x22
#define H3LIS100DL_ACC_CTRL_REG4             	0x23
#define H3LIS100DL_ACC_CTRL_REG5             	0x24
#define H3LIS100DL_ACC_STATUS_REG           	0x27
#define H3LIS100DL_ACC_OUT_X              		0x29
#define H3LIS100DL_ACC_OUT_Y	              	0x2B
#define H3LIS100DL_ACC_OUT_Z	              	0x2D

#define H3LIS100DL_ACC_SENSITIVITY_100G        	780

#define H3LIS100DL_ACC_CHIP_ID_VALUE			(0x32)

#define H3LIS100DL_SHIFT_EIGHT_BITS           	(8)
#define H3LIS100DL_SHIFT_FOUR_BITS             	(4)

#define H3LIS100DL_16_BIT_SHIFT               	(0xFF)

#define H3LIS100DL_ACC_ODR_POWER_DOWN			(0x00)
#define H3LIS100DL_ACC_ODR_0_5_HZ               (0x40)
#define H3LIS100DL_ACC_ODR_1_HZ               	(0x60)
#define H3LIS100DL_ACC_ODR_2_HZ               	(0x80)
#define H3LIS100DL_ACC_ODR_5_HZ               	(0xA0)
#define H3LIS100DL_ACC_ODR_10_HZ               	(0xC0)
#define H3LIS100DL_ACC_ODR_50_HZ              	(0x20)
#define H3LIS100DL_ACC_ODR_100_HZ              	(0x28)
#define H3LIS100DL_ACC_ODR_400_HZ              	(0x30)
#define H3LIS100DL_ACC_ODR_MSK                 	(0xF8)

#define H3LIS100DL_ACC_DEFAULT_ODR_100HZ        (100)

#define H3LIS100DL_ACC_STATUS_ZYXDA				(0x08)

#define H3LIS100DL_GET_BITSLICE(regvar, bitname)\
((regvar & bitname##_MSK) >> bitname##_POS)
	
#define H3LIS100DL_SET_BITSLICE(regvar, bitname, val)\
	((regvar & ~bitname##_MSK) | ((val<<bitname##_POS)&bitname##_MSK))

#if 0
static int32_t h3lis100dl_acc_factor[ACC_RANGE_MAX] = { 0, 0, 0, 0, 0, 0, 0, H3LIS100DL_ACC_SENSITIVITY_100G};
static int32_t cur_acc_factor = 0;
#endif


i2c_dev_t h3lis100dl_ctx = {
    .port = 3,
    .config.address_width = 8,
    .config.freq = 400000,
    .config.dev_addr = H3LIS100DL_I2C_ADDR,
};

static int drv_acc_st_h3lis100dl_validate_id(i2c_dev_t* drv, uint8_t id_value)
{
    uint8_t value = 0x00;
    int ret = 0;

    if(drv == NULL){
        return -1;
    }
    
    ret = sensor_i2c_read(drv, H3LIS100DL_ACC_WHO_AM_I, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
	
    if(unlikely(ret)){
        return ret;
    }
    
    if (id_value != value){
        return -1;
    }
    
    return 0;
}

static int drv_acc_st_h3lis100dl_set_power_mode(i2c_dev_t* drv, dev_power_mode_e mode)
{
    uint8_t value = 0x00;
    int ret = 0;
    
    ret = sensor_i2c_read(drv, H3LIS100DL_ACC_CTRL_REG1, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }

    switch(mode){
        case DEV_POWER_ON:{
			value &= ~H3LIS100DL_ACC_ODR_MSK;
			value |= H3LIS100DL_ACC_ODR_10_HZ;	
            ret = sensor_i2c_write(drv, H3LIS100DL_ACC_CTRL_REG1, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
            if(unlikely(ret)){
                return ret;
            }
        }break;
        
        case DEV_POWER_OFF:{
			value &= ~H3LIS100DL_ACC_ODR_MSK;
			value |= H3LIS100DL_ACC_ODR_POWER_DOWN;	
            ret = sensor_i2c_write(drv, H3LIS100DL_ACC_CTRL_REG1, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
            if(unlikely(ret)){
                return ret;
            }
        }break;
            
        case DEV_SLEEP:{
			value &= ~H3LIS100DL_ACC_ODR_MSK;
			value |= H3LIS100DL_ACC_ODR_10_HZ;	
            ret = sensor_i2c_write(drv, H3LIS100DL_ACC_CTRL_REG1, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
            if(unlikely(ret)){
                return ret;
            }

        }break;

       default:break;
    }
    return 0;
}

static uint8_t drv_acc_st_h3lis100dl_hz2odr(uint32_t hz)
{
    if(hz > 100)
        return H3LIS100DL_ACC_ODR_400_HZ;
    else if(hz > 50)
        return H3LIS100DL_ACC_ODR_100_HZ;
    else if(hz > 10)
        return H3LIS100DL_ACC_ODR_50_HZ;
    else if(hz > 5)
        return H3LIS100DL_ACC_ODR_10_HZ;
    else if(hz > 2)
        return H3LIS100DL_ACC_ODR_5_HZ;
    else if(hz > 1)
        return H3LIS100DL_ACC_ODR_2_HZ;
	else
		return H3LIS100DL_ACC_ODR_1_HZ;

}

static int drv_acc_st_h3lis100dl_set_odr(i2c_dev_t* drv, uint32_t hz)
{
    int ret = 0;
    uint8_t value = 0x00;
    uint8_t odr = drv_acc_st_h3lis100dl_hz2odr(hz);

    ret = sensor_i2c_read(drv, H3LIS100DL_ACC_CTRL_REG1, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }

	value &= ~H3LIS100DL_ACC_ODR_MSK;
    value |= odr;
	
    ret = sensor_i2c_write(drv, H3LIS100DL_ACC_CTRL_REG1, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }
    return 0;    
}

static void drv_acc_st_h3lis100dl_irq_handle(void)
{
    /* no handle so far */
}

static int drv_acc_st_h3lis100dl_open(void)
{
    int ret = 0;
    ret  = drv_acc_st_h3lis100dl_set_power_mode(&h3lis100dl_ctx, DEV_POWER_ON);
    if(unlikely(ret)){
        return -1;
    }

    ret = drv_acc_st_h3lis100dl_set_odr(&h3lis100dl_ctx, H3LIS100DL_ACC_DEFAULT_ODR_100HZ);
    if(unlikely(ret)){
        return -1;
    }
     
    return 0;

}

static int drv_acc_st_h3lis100dl_close(void)
{
    int ret = 0;
    ret  = drv_acc_st_h3lis100dl_set_power_mode(&h3lis100dl_ctx, DEV_POWER_OFF);
    if(unlikely(ret)){
        return -1;
    }
    return 0;
}

static int drv_acc_st_h3lis100dl_read(void *buf, size_t len)
{

    int ret = 0;
    size_t size;
    uint8_t reg[6];
    accel_data_t *accel = (accel_data_t *)buf;
    if(buf == NULL){
        return -1;
    }

    ret = sensor_i2c_read(&h3lis100dl_ctx, H3LIS100DL_ACC_CTRL_REG1, &reg[5], I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }
	
    size = sizeof(accel_data_t);
    if(len < size){
        return -1;
    }
	
	ret = sensor_i2c_read(&h3lis100dl_ctx, H3LIS100DL_ACC_OUT_X, &reg[0], 1, I2C_OP_RETRIES);
	ret = sensor_i2c_read(&h3lis100dl_ctx, H3LIS100DL_ACC_OUT_Y, &reg[1], 1, I2C_OP_RETRIES);
	ret = sensor_i2c_read(&h3lis100dl_ctx, H3LIS100DL_ACC_OUT_Z, &reg[2], 1, I2C_OP_RETRIES);
	
	if(unlikely(ret)){
        return -1;
    }
    accel->data[DATA_AXIS_X] = (int8_t)reg[0];	
    accel->data[DATA_AXIS_Y] = (int8_t)reg[1];
	accel->data[DATA_AXIS_Z] = (int8_t)reg[2];
	
	accel->data[DATA_AXIS_X] = accel->data[DATA_AXIS_X] * H3LIS100DL_ACC_SENSITIVITY_100G;
	accel->data[DATA_AXIS_Y] = accel->data[DATA_AXIS_Y] * H3LIS100DL_ACC_SENSITIVITY_100G;
	accel->data[DATA_AXIS_Z] = accel->data[DATA_AXIS_Z] * H3LIS100DL_ACC_SENSITIVITY_100G;

    
    accel->timestamp = aos_now_ms();

    return (int)size;
}

static int drv_acc_st_h3lis100dl_ioctl(int cmd, unsigned long arg)
{
    int ret = 0;
    dev_sensor_info_t *info = (dev_sensor_info_t *)arg;
    
    switch(cmd){
        case SENSOR_IOCTL_ODR_SET:{
            ret = drv_acc_st_h3lis100dl_set_odr(&h3lis100dl_ctx, arg);
            if(unlikely(ret)){
                return -1;
            }
        }break;
        case SENSOR_IOCTL_SET_POWER:{
            ret = drv_acc_st_h3lis100dl_set_power_mode(&h3lis100dl_ctx, arg);
            if(unlikely(ret)){
                return -1;
            }
        }break;
        case SENSOR_IOCTL_GET_INFO:{ 
            /* fill the dev info here */
            info->model = "H3LIS100DL";
            info->range_max = 100;
            info->range_min = 100;
            info->unit = mg;
			
        }break;
       default:break;
    }

    return 0;
}

int drv_acc_st_h3lis100dl_init(void){
    int ret = 0;
    sensor_obj_t sensor;
    memset(&sensor, 0, sizeof(sensor));

    /* fill the sensor obj parameters here */
    sensor.io_port    = I2C_PORT;
    sensor.tag        = TAG_DEV_ACC;
    sensor.path       = dev_acc_path;
    sensor.open       = drv_acc_st_h3lis100dl_open;
    sensor.close      = drv_acc_st_h3lis100dl_close;
    sensor.read       = drv_acc_st_h3lis100dl_read;
    sensor.write      = NULL;
    sensor.ioctl      = drv_acc_st_h3lis100dl_ioctl;
    sensor.irq_handle = drv_acc_st_h3lis100dl_irq_handle;

    ret = sensor_create_obj(&sensor);
    if(unlikely(ret)){
        return -1;
    }

    ret = drv_acc_st_h3lis100dl_validate_id(&h3lis100dl_ctx, H3LIS100DL_ACC_CHIP_ID_VALUE);
    if(unlikely(ret)){
        return -1;
    }

    //set odr is 100hz, and will update
    ret = drv_acc_st_h3lis100dl_set_odr(&h3lis100dl_ctx, H3LIS100DL_ACC_DEFAULT_ODR_100HZ);
    if(unlikely(ret)){
        return -1;
    }

    /* update the phy sensor info to sensor hal */
    LOG("%s %s successfully \n", SENSOR_STR, __func__);
    return 0;
}

SENSOR_DRV_ADD(drv_acc_st_h3lis100dl_init);

