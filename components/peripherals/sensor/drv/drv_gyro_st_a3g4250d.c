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


#define A3G4250D_I2C_ADDR1                	(0x68)
#define A3G4250D_I2C_ADDR2                  (0x69)
#define A3G4250D_I2C_ADDR_TRANS(n)        	((n)<<1)  
#define A3G4250D_I2C_ADDR               	A3G4250D_I2C_ADDR_TRANS(A3G4250D_I2C_ADDR1)

#define A3G4250D_GYRO_WHO_AM_I_REG       	0x0F
#define A3G4250D_GYRO_CTRL1           		0x20
#define A3G4250D_GYRO_CTRL2	            	0x21
#define A3G4250D_GYRO_CTRL3	            	0x22
#define A3G4250D_GYRO_CTRL4	           	 	0x23
#define A3G4250D_GYRO_CTRL5	            	0x24
#define A3G4250D_GYRO_OUT_TEMP         		0x26
#define A3G4250D_GYRO_STATUS         		0x27
#define A3G4250D_GYRO_OUTX_L          		0x28
#define A3G4250D_GYRO_OUTX_H        	  	0x29
#define A3G4250D_GYRO_OUTY_L        		0x2A
#define A3G4250D_GYRO_OUTY_H          		0x2B
#define A3G4250D_GYRO_OUTZ_L          		0x2C
#define A3G4250D_GYRO_OUTZ_H          		0x2D

#define A3G4250D_CHIP_ID_VALUE        	 	(0xD3)

#define A3G4250D_GYRO_ODR_POWER_DOWN       	(0x00)
#define A3G4250D_GYRO_ODR_100_HZ          	(0x0F)
#define A3G4250D_GYRO_ODR_200_HZ           	(0x4F)
#define A3G4250D_GYRO_ODR_400_HZ           	(0x8F)
#define A3G4250D_GYRO_ODR_800_HZ           	(0xCF)

#define A3G4250D_GYRO_POWER_ON         		(0x0F)
#define A3G4250D_GYRO_SLEEP         		(0x08)
#define A3G4250D_GYRO_SLEEP_MASK    		(0x0F)

#define A3G4250D_GYRO_RANGE_250           	(0x0)

#define A3G4250D_GYRO_SENSITIVITY_250DPS   	8750

#define A3G4250D_GYRO_MUL					1000

#define A3G4250D_SHIFT_EIGHT_BITS          	(8)

#define A3G4250D_GYRO_DEFAULT_ODR_100HZ     (100)

#define A3G4250D_GET_BITSLICE(regvar, bitname)\
((regvar & bitname##_MSK) >> bitname##_POS)

#define A3G4250D_SET_BITSLICE(regvar, bitname, val)\
((regvar & ~bitname##_MSK) | ((val<<bitname##_POS)&bitname##_MSK))

static int32_t a3g4250d_gyro_factor[GYRO_RANGE_MAX] = {0, A3G4250D_GYRO_SENSITIVITY_250DPS, 0, 0, 
                                      0 };

static int32_t cur_gyro_factor = 0;

i2c_dev_t a3g4250d_ctx = {
    .port = 3,
    .config.address_width = 8,
    .config.freq = 400000,
    .config.dev_addr = A3G4250D_I2C_ADDR,
};

UNUSED static int drv_gyro_st_a3g4250d_soft_reset(i2c_dev_t* drv)
{
    return 0;
}

static int drv_gyro_st_a3g4250d_validate_id(i2c_dev_t* drv, uint8_t id_value)
{
    uint8_t value = 0x00;
    int ret = 0;

    if(drv == NULL){
        return -1;
    }

    ret = sensor_i2c_read(drv, A3G4250D_GYRO_WHO_AM_I_REG, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }
    
    if (id_value != value){
        return -1;
    }
    
    return 0;
}

static int drv_gyro_st_a3g4250d_set_bdu(i2c_dev_t* drv)
{
    return 0;
}

static int drv_gyro_st_a3g4250d_set_power_mode(i2c_dev_t* drv, dev_power_mode_e mode)
{
    uint8_t value = 0x00;
    int ret = 0;
    
    ret = sensor_i2c_read(drv, A3G4250D_GYRO_CTRL1, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }

    switch(mode){
        case DEV_POWER_ON:{
            
            value |= A3G4250D_GYRO_POWER_ON;
            ret = sensor_i2c_write(drv, A3G4250D_GYRO_CTRL1, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
            if(unlikely(ret)){
                return ret;
            }
        }break;
        
        case DEV_POWER_OFF:{
            value &= ~A3G4250D_GYRO_POWER_ON;
            ret = sensor_i2c_write(drv, A3G4250D_GYRO_CTRL1, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
            if(unlikely(ret)){
                return ret;
            }
        }break;
            
        case DEV_SLEEP:{
            value &= ~A3G4250D_GYRO_SLEEP_MASK;
            value |= A3G4250D_GYRO_SLEEP;
            ret = sensor_i2c_write(drv, A3G4250D_GYRO_CTRL1, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
            if(unlikely(ret)){
                return ret;
            }

        }break;
        
       default:break;
    }
    return 0;
}

static uint8_t drv_gyro_st_a3g4250d_hz2odr(uint32_t hz)
{
    if(hz > 400)
        return A3G4250D_GYRO_ODR_800_HZ;
    else if(hz > 200)
        return A3G4250D_GYRO_ODR_400_HZ;
    else if(hz > 100)
        return A3G4250D_GYRO_ODR_200_HZ;
    else
        return A3G4250D_GYRO_ODR_100_HZ;

}


static int drv_gyro_st_a3g4250d_set_odr(i2c_dev_t* drv, uint32_t hz)
{
    int ret = 0;
    uint8_t odr = drv_gyro_st_a3g4250d_hz2odr(hz);
		
    ret = sensor_i2c_write(drv, A3G4250D_GYRO_CTRL1, &odr, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }
    return 0;    
}

static int drv_gyro_st_a3g4250d_set_range(i2c_dev_t* drv, uint32_t range)
{
    
    if(range == GYRO_RANGE_250DPS){
        cur_gyro_factor = a3g4250d_gyro_factor[range];
    }
	
    return 0;
}


static void drv_gyro_st_a3g4250d_irq_handle(void)
{
    /* no handle so far */
}

static int drv_gyro_st_a3g4250d_open(void)
{
    int ret = 0;
    ret  = drv_gyro_st_a3g4250d_set_power_mode(&a3g4250d_ctx, DEV_POWER_ON);
    if(unlikely(ret)){
        return -1;
    }

    ret  = drv_gyro_st_a3g4250d_set_bdu(&a3g4250d_ctx);
    if(unlikely(ret)){
        return -1;
    }

    ret = drv_gyro_st_a3g4250d_set_range(&a3g4250d_ctx, GYRO_RANGE_250DPS);
    if(unlikely(ret)){
        return -1;
    }

    ret = drv_gyro_st_a3g4250d_set_odr(&a3g4250d_ctx, A3G4250D_GYRO_DEFAULT_ODR_100HZ);
    if(unlikely(ret)){
        return -1;
    }
    
    return 0;

}

static int drv_gyro_st_a3g4250d_close(void)
{
    int ret = 0;
    ret  = drv_gyro_st_a3g4250d_set_power_mode(&a3g4250d_ctx, DEV_POWER_OFF);
    if(unlikely(ret)){
        return -1;
    }
    return 0;
}

static int drv_gyro_st_a3g4250d_read(void *buf, size_t len)
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

	ret = sensor_i2c_read(&a3g4250d_ctx, (A3G4250D_GYRO_OUTX_L | 0x80), reg, 6, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return -1;
    }
    gyro->data[DATA_AXIS_X] = (int16_t)((((int32_t)((int8_t)reg[1]))<< A3G4250D_SHIFT_EIGHT_BITS)|(reg[0]));
    gyro->data[DATA_AXIS_Y] = (int16_t)((((int32_t)((int8_t)reg[3]))<< A3G4250D_SHIFT_EIGHT_BITS)|(reg[2]));
    gyro->data[DATA_AXIS_Z] = (int16_t)((((int32_t)((int8_t)reg[5]))<< A3G4250D_SHIFT_EIGHT_BITS)|(reg[4]));
//	LOG("A3G4250D X: %x; Y: %x; Z: %x\n",gyro->data[DATA_AXIS_X], gyro->data[DATA_AXIS_Y], gyro->data[DATA_AXIS_Z]);
    if(cur_gyro_factor != 0){
        gyro->data[DATA_AXIS_X] = (gyro->data[DATA_AXIS_X] * cur_gyro_factor);
        gyro->data[DATA_AXIS_Y] = (gyro->data[DATA_AXIS_Y] * cur_gyro_factor);
        gyro->data[DATA_AXIS_Z] = (gyro->data[DATA_AXIS_Z] * cur_gyro_factor);
    }
    gyro->timestamp = aos_now_ms();

    return (int)size;
}

static int drv_gyro_st_a3g4250d_ioctl(int cmd, unsigned long arg)
{
    int ret = 0;
    
    switch(cmd){
        case SENSOR_IOCTL_ODR_SET:{
            ret = drv_gyro_st_a3g4250d_set_odr(&a3g4250d_ctx, arg);
            if(unlikely(ret)){
                return -1;
            }
        }break;
        case SENSOR_IOCTL_RANGE_SET:{
            ret = drv_gyro_st_a3g4250d_set_range(&a3g4250d_ctx, arg);
            if(unlikely(ret)){
                return -1;
            }
        }break;
        case SENSOR_IOCTL_SET_POWER:{
            ret = drv_gyro_st_a3g4250d_set_power_mode(&a3g4250d_ctx, arg);
            if(unlikely(ret)){
                return -1;
            }
        }break;
        case SENSOR_IOCTL_GET_INFO:{ 
            /* fill the dev info here */
            dev_sensor_info_t *info = (dev_sensor_info_t *)arg;
            info->model = "A3G4250D";
            info->range_max = 250;
            info->range_min = 250;
            info->unit = udps;
        }break;
       
       default:break;
    }

    return 0;
}

int drv_gyro_st_a3g4250d_init(void){
    int ret = 0;
    sensor_obj_t sensor;
    memset(&sensor, 0, sizeof(sensor));

    /* fill the sensor obj parameters here */
    sensor.io_port    = I2C_PORT;
    sensor.tag        = TAG_DEV_GYRO;
    sensor.path       = dev_gyro_path;
    sensor.open       = drv_gyro_st_a3g4250d_open;
    sensor.close      = drv_gyro_st_a3g4250d_close;
    sensor.read       = drv_gyro_st_a3g4250d_read;
    sensor.write      = NULL;
    sensor.ioctl      = drv_gyro_st_a3g4250d_ioctl;
    sensor.irq_handle = drv_gyro_st_a3g4250d_irq_handle;

    ret = sensor_create_obj(&sensor);
    if(unlikely(ret)){
        return -1;
    }

    ret = drv_gyro_st_a3g4250d_validate_id(&a3g4250d_ctx, A3G4250D_CHIP_ID_VALUE);
    if(unlikely(ret)){
        return -1;
    }

    ret = drv_gyro_st_a3g4250d_set_range(&a3g4250d_ctx, GYRO_RANGE_250DPS);
    if(unlikely(ret)){
        return -1;
    }
    
    ret = drv_gyro_st_a3g4250d_set_odr(&a3g4250d_ctx, A3G4250D_GYRO_ODR_100_HZ);
    if(unlikely(ret)){
        return -1;
    }

    ret = drv_gyro_st_a3g4250d_set_power_mode(&a3g4250d_ctx, DEV_POWER_OFF);
    if(unlikely(ret)){
        return -1;
    }

    /* update the phy sensor info to sensor hal */
    LOG("%s %s successfully \n", SENSOR_STR, __func__);
    return 0;
}


SENSOR_DRV_ADD(drv_gyro_st_a3g4250d_init);



