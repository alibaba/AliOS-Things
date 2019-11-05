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


#define L3GD20H_I2C_ADDR1                   (0x6A)
#define L3GD20H_I2C_ADDR2                   (0x6B)
#define L3GD20H_I2C_ADDR_TRANS(n)           ((n)<<1)  
#define L3GD20H_I2C_ADDR                    L3GD20H_I2C_ADDR_TRANS(L3GD20H_I2C_ADDR1)

#define L3GD20H_GYRO_WHO_AM_I_REG       	0x0F
#define L3GD20H_GYRO_CTRL1           		0x20
#define L3GD20H_GYRO_CTRL2	            	0x21
#define L3GD20H_GYRO_CTRL3	            	0x22
#define L3GD20H_GYRO_CTRL4	           	 	0x23
#define L3GD20H_GYRO_CTRL5	            	0x24
#define L3GD20H_GYRO_OUT_TEMP         		0x26
#define L3GD20H_GYRO_STATUS         		0x27
#define L3GD20H_GYRO_OUTX_L          		0x28
#define L3GD20H_GYRO_OUTX_H        	  		0x29
#define L3GD20H_GYRO_OUTY_L        		  	0x2A
#define L3GD20H_GYRO_OUTY_H          		0x2B
#define L3GD20H_GYRO_OUTZ_L          		0x2C
#define L3GD20H_GYRO_OUTZ_H          		0x2D
#define L3GD20H_GYRO_LOW_ODR        	  	0x39

#define L3GD20H_CHIP_ID_VALUE        	 	(0xD7)

#define L3GD20H_ENABLE_SOFT_RESET_VALUE 	(0x04)

#define L3GD20H_BDU_ENABLE            		(0x80)

#define L3GD20H_GYRO_ODR_POWER_DOWN         (0x00)
#define L3GD20H_GYRO_ODR_12_5_HZ            (0x0F)
#define L3GD20H_GYRO_ODR_25_HZ              (0x4F)
#define L3GD20H_GYRO_ODR_50_HZ              (0x8F)
#define L3GD20H_GYRO_ODR_100_HZ             (0x0F)
#define L3GD20H_GYRO_ODR_200_HZ             (0x4F)
#define L3GD20H_GYRO_ODR_400_HZ             (0x8F)
#define L3GD20H_GYRO_ODR_800_HZ             (0xCF)

#define L3GD20H_LOW_ODR_ENABLE				(0x01)

#define L3GD20H_GYRO_POWER_ON         		(0x08)
#define L3GD20H_GYRO_SLEEP         			(0x08)
#define L3GD20H_GYRO_SLEEP_MASK    			(0x0F)

#define L3GD20H_GYRO_RANGE_245              (0x0)
#define L3GD20H_GYRO_RANGE_500              (0x1)
#define L3GD20H_GYRO_RANGE_2000             (0x2)
#define L3GD20H_GYRO_RANGE_MSK              (0x30)
#define L3GD20H_GYRO_RANGE_POS              (4)

#define L3GD20H_GYRO_SENSITIVITY_245DPS     87500
#define L3GD20H_GYRO_SENSITIVITY_500DPS     175000
#define L3GD20H_GYRO_SENSITIVITY_2000DPS    700000

#define L3GD20H_GYRO_MUL					1000

#define L3GD20H_SHIFT_EIGHT_BITS            (8)

#define L3GD20H_GYRO_DEFAULT_ODR_100HZ      (100)

#define L3GD20H_GET_BITSLICE(regvar, bitname)\
((regvar & bitname##_MSK) >> bitname##_POS)

#define L3GD20H_SET_BITSLICE(regvar, bitname, val)\
((regvar & ~bitname##_MSK) | ((val<<bitname##_POS)&bitname##_MSK))

static int32_t l3gd20h_gyro_factor[GYRO_RANGE_MAX] = {0, L3GD20H_GYRO_SENSITIVITY_245DPS, L3GD20H_GYRO_SENSITIVITY_500DPS, 0,
                                      L3GD20H_GYRO_SENSITIVITY_2000DPS };

static int32_t cur_gyro_factor = 0;

i2c_dev_t l3gd20h_ctx = {
    .port = 3,
    .config.address_width = 8,
    .config.freq = 400000,
    .config.dev_addr = L3GD20H_I2C_ADDR,
};

UNUSED static int drv_gyro_st_l3gd20h_soft_reset(i2c_dev_t* drv)
{
    int ret = 0;
    uint8_t value = 0x00;

    ret = sensor_i2c_read(drv, L3GD20H_GYRO_LOW_ODR, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }
    
    value |= L3GD20H_ENABLE_SOFT_RESET_VALUE;
    
    ret = sensor_i2c_write(drv, L3GD20H_GYRO_LOW_ODR, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return -1;
    }
    return 0;
}

static int drv_gyro_st_l3gd20h_validate_id(i2c_dev_t* drv, uint8_t id_value)
{
    uint8_t value = 0x00;
    int ret = 0;

    if(drv == NULL){
        return -1;
    }

    ret = sensor_i2c_read(drv, L3GD20H_GYRO_WHO_AM_I_REG, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }
    
    if (id_value != value){
        return -1;
    }
    
    return 0;
}

static int drv_gyro_st_l3gd20h_set_bdu(i2c_dev_t* drv)
{
    int ret = 0;
    uint8_t value = 0x00;

    ret = sensor_i2c_read(drv, L3GD20H_GYRO_CTRL4, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }

    if (value & L3GD20H_BDU_ENABLE)
        return 0;
    
    value |= L3GD20H_BDU_ENABLE;
    
    ret = sensor_i2c_write(drv, L3GD20H_GYRO_CTRL4, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return -1;
    }
    return 0;
}

static int drv_gyro_st_l3gd20h_set_power_mode(i2c_dev_t* drv, dev_power_mode_e mode)
{
    uint8_t value  = 0x00;
    int ret = 0;
    
    ret = sensor_i2c_read(drv, L3GD20H_GYRO_CTRL1, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }

    switch(mode){
        case DEV_POWER_ON:{
            
            value |= L3GD20H_GYRO_POWER_ON;
            ret = sensor_i2c_write(drv, L3GD20H_GYRO_CTRL1, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
            if(unlikely(ret)){
                return ret;
            }
        }break;
        
        case DEV_POWER_OFF:{
            value &= ~L3GD20H_GYRO_POWER_ON;
            ret = sensor_i2c_write(drv, L3GD20H_GYRO_CTRL1, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
            if(unlikely(ret)){
                return ret;
            }
        }break;
            
        case DEV_SLEEP:{
            value &= ~L3GD20H_GYRO_SLEEP_MASK;
            value |= L3GD20H_GYRO_SLEEP;
            ret = sensor_i2c_write(drv, L3GD20H_GYRO_CTRL1, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
            if(unlikely(ret)){
                return ret;
            }

        }break;
        
       default:break;
    }
    return 0;
}

static uint8_t drv_gyro_st_l3gd20h_hz2odr(uint32_t hz)
{
    if(hz > 400)
        return L3GD20H_GYRO_ODR_800_HZ;
    else if(hz > 200)
        return L3GD20H_GYRO_ODR_400_HZ;
    else if(hz > 100)
        return L3GD20H_GYRO_ODR_200_HZ;
    else if(hz > 50)
        return L3GD20H_GYRO_ODR_100_HZ;
    else if(hz > 25)
        return L3GD20H_GYRO_ODR_50_HZ;
    else if(hz > 13)
        return L3GD20H_GYRO_ODR_25_HZ;
    else
        return L3GD20H_GYRO_ODR_12_5_HZ;

}


static int drv_gyro_st_l3gd20h_set_odr(i2c_dev_t* drv, uint32_t hz)
{
    int ret = 0;
    uint8_t value = 0x00;
    uint8_t odr = drv_gyro_st_l3gd20h_hz2odr(hz);

	if(hz < 100){
	    ret = sensor_i2c_read(drv, L3GD20H_GYRO_LOW_ODR, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
	    if(unlikely(ret)){
	        return ret;
	    }

		value |= L3GD20H_LOW_ODR_ENABLE;
		
	    ret = sensor_i2c_write(drv, L3GD20H_GYRO_LOW_ODR, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
	    if(unlikely(ret)){
	        return ret;
	    }
	}else{
	    ret = sensor_i2c_read(drv, L3GD20H_GYRO_LOW_ODR, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
	    if(unlikely(ret)){
	        return ret;
	    }

		value &= ~L3GD20H_LOW_ODR_ENABLE;
		
	    ret = sensor_i2c_write(drv, L3GD20H_GYRO_LOW_ODR, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
	    if(unlikely(ret)){
	        return ret;
	    }
	}
		
    ret = sensor_i2c_write(drv, L3GD20H_GYRO_CTRL1, &odr, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }
    return 0;    
}

static int drv_gyro_st_l3gd20h_set_range(i2c_dev_t* drv, uint32_t range)
{
    int ret = 0;
    uint8_t value = 0x00;
    uint8_t tmp = 0;

    ret = sensor_i2c_read(drv, L3GD20H_GYRO_CTRL4, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }

    switch (range){
        case GYRO_RANGE_250DPS:{
            tmp = L3GD20H_GYRO_RANGE_245;
        }break;
        
        case GYRO_RANGE_500DPS:{
            tmp = L3GD20H_GYRO_RANGE_500;
        }break;
        case GYRO_RANGE_2000DPS:{
            tmp = L3GD20H_GYRO_RANGE_2000;
        }break;
        
        default:break;
    }
    
    value  = L3GD20H_SET_BITSLICE(value,L3GD20H_GYRO_RANGE,tmp);
    ret = sensor_i2c_write(drv, L3GD20H_GYRO_CTRL4, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }
    
    if((range >= GYRO_RANGE_250DPS)&&(range <= GYRO_RANGE_2000DPS)){
        cur_gyro_factor = l3gd20h_gyro_factor[range];
    }
    
    return 0;
}


static void drv_gyro_st_l3gd20h_irq_handle(void)
{
    /* no handle so far */
}

static int drv_gyro_st_l3gd20h_open(void)
{
    int ret = 0;
    ret  = drv_gyro_st_l3gd20h_set_power_mode(&l3gd20h_ctx, DEV_POWER_ON);
    if(unlikely(ret)){
        return -1;
    }

    ret  = drv_gyro_st_l3gd20h_set_bdu(&l3gd20h_ctx);
    if(unlikely(ret)){
        return -1;
    }

    ret = drv_gyro_st_l3gd20h_set_range(&l3gd20h_ctx, GYRO_RANGE_2000DPS);
    if(unlikely(ret)){
        return -1;
    }

    ret = drv_gyro_st_l3gd20h_set_odr(&l3gd20h_ctx, L3GD20H_GYRO_DEFAULT_ODR_100HZ);
    if(unlikely(ret)){
        return -1;
    }
    
    return 0;

}

static int drv_gyro_st_l3gd20h_close(void)
{
    int ret = 0;
    ret  = drv_gyro_st_l3gd20h_set_power_mode(&l3gd20h_ctx, DEV_POWER_OFF);
    if(unlikely(ret)){
        return -1;
    }
    return 0;
}

static int drv_gyro_st_l3gd20h_read(void *buf, size_t len)
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

	ret = sensor_i2c_read(&l3gd20h_ctx, (L3GD20H_GYRO_OUTX_L | 0x80), reg, 6, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return -1;
    }
    gyro->data[DATA_AXIS_X] = (int16_t)((((int32_t)((int8_t)reg[1]))<< L3GD20H_SHIFT_EIGHT_BITS)|(reg[0]));
    gyro->data[DATA_AXIS_Y] = (int16_t)((((int32_t)((int8_t)reg[3]))<< L3GD20H_SHIFT_EIGHT_BITS)|(reg[2]));
    gyro->data[DATA_AXIS_Z] = (int16_t)((((int32_t)((int8_t)reg[5]))<< L3GD20H_SHIFT_EIGHT_BITS)|(reg[4]));

    if(cur_gyro_factor != 0){
        gyro->data[DATA_AXIS_X] = (gyro->data[DATA_AXIS_X] * cur_gyro_factor);
        gyro->data[DATA_AXIS_Y] = (gyro->data[DATA_AXIS_Y] * cur_gyro_factor);
        gyro->data[DATA_AXIS_Z] = (gyro->data[DATA_AXIS_Z] * cur_gyro_factor);
    }
    gyro->timestamp = aos_now_ms();

    return (int)size;
}

static int drv_gyro_st_l3gd20h_ioctl(int cmd, unsigned long arg)
{
    int ret = 0;
    
    switch(cmd){
        case SENSOR_IOCTL_ODR_SET:{
            ret = drv_gyro_st_l3gd20h_set_odr(&l3gd20h_ctx, arg);
            if(unlikely(ret)){
                return -1;
            }
        }break;
        case SENSOR_IOCTL_RANGE_SET:{
            ret = drv_gyro_st_l3gd20h_set_range(&l3gd20h_ctx, arg);
            if(unlikely(ret)){
                return -1;
            }
        }break;
        case SENSOR_IOCTL_SET_POWER:{
            ret = drv_gyro_st_l3gd20h_set_power_mode(&l3gd20h_ctx, arg);
            if(unlikely(ret)){
                return -1;
            }
        }break;
        case SENSOR_IOCTL_GET_INFO:{ 
            /* fill the dev info here */
            dev_sensor_info_t *info = (dev_sensor_info_t *)arg;
            info->model = "L3GD20H";
            info->range_max = 2000;
            info->range_min = 245;
            info->unit = udps;
        }break;
       
       default:break;
    }

    return 0;
}

int drv_gyro_st_l3gd20h_init(void){
    int ret = 0;
    sensor_obj_t sensor;
    memset(&sensor, 0, sizeof(sensor));

    /* fill the sensor obj parameters here */
    sensor.io_port    = I2C_PORT;
    sensor.tag        = TAG_DEV_GYRO;
    sensor.path       = dev_gyro_path;
    sensor.open       = drv_gyro_st_l3gd20h_open;
    sensor.close      = drv_gyro_st_l3gd20h_close;
    sensor.read       = drv_gyro_st_l3gd20h_read;
    sensor.write      = NULL;
    sensor.ioctl      = drv_gyro_st_l3gd20h_ioctl;
    sensor.irq_handle = drv_gyro_st_l3gd20h_irq_handle;

    ret = sensor_create_obj(&sensor);
    if(unlikely(ret)){
        return -1;
    }

    ret = drv_gyro_st_l3gd20h_validate_id(&l3gd20h_ctx, L3GD20H_CHIP_ID_VALUE);
    if(unlikely(ret)){
        return -1;
    }

    ret = drv_gyro_st_l3gd20h_set_range(&l3gd20h_ctx, GYRO_RANGE_2000DPS);
    if(unlikely(ret)){
        return -1;
    }
    
    ret = drv_gyro_st_l3gd20h_set_odr(&l3gd20h_ctx, L3GD20H_GYRO_ODR_100_HZ);
    if(unlikely(ret)){
        return -1;
    }

    ret = drv_gyro_st_l3gd20h_set_power_mode(&l3gd20h_ctx, DEV_POWER_OFF);
    if(unlikely(ret)){
        return -1;
    }

    /* update the phy sensor info to sensor hal */
    LOG("%s %s successfully \n", SENSOR_STR, __func__);
    return 0;
}


SENSOR_DRV_ADD(drv_gyro_st_l3gd20h_init);

