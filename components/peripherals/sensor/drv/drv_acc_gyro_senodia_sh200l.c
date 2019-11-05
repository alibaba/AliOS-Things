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


/*#define SH200L_DEBUG*/

#ifdef SH200L_DEBUG
#define SH200L_LOG  LOG
#else
#define SH200L_LOG 
#endif

#define SH200L_I2C_ADDR1                   (0x6C)
#define SH200L_I2C_ADDR2                   (0x6D)
#define SH200L_I2C_ADDR_TRANS(n)           ((n)<<1)  
#define SH200L_I2C_ADDR                    SH200L_I2C_ADDR_TRANS(SH200L_I2C_ADDR2)
#define SH200L_ACC_GYRO_WHO_AM_I_REG       0x30
#define SH200L_CHIP_ID_VALUE               (0x18)

#define SH200L_ACC_CONFIG 				   0x0E
#define SH200L_GYRO_CONFIG 				   0x0F
#define SH200L_GYRO_DLPF 				   0x11
#define SH200L_FIFO_CONFIG 				   0x12
#define SH200L_ACC_RANGE 				   0x16
#define SH200L_GYRO_RANGE 				   0x2B
#define SH200L_OUTPUT_ACC_X 			   0x00
#define SH200L_OUTPUT_GYRO_X 			   0x06
#define SH200L_OUTPUT_TEMP 				   0x0C
#define SH200L_REG_SET1 				   0xBA
#define SH200L_REG_SET2 				   0xCA
#define SH200L_ADC_RESET  				   0xC2
#define SH200L_SOFT_RESET 				   0x7F
#define SH200L_RESET 					   0x75

#define SH200L_ACC_RANGE_2G                (0x0)
#define SH200L_ACC_RANGE_4G                (0x0)
#define SH200L_ACC_RANGE_8G                (0x1)
#define SH200L_ACC_RANGE_16G               (0x2)
#define SH200L_ACC_RANGE_MSK               (0X3)
#define SH200L_ACC_RANGE_POS               (0)

#define SH200L_ACC_SENSITIVITY_2G          (61)
#define SH200L_ACC_SENSITIVITY_4G          (122)
#define SH200L_ACC_SENSITIVITY_8G          (244)
#define SH200L_ACC_SENSITIVITY_16G         (488) 


#define SH200L_ACC_ODR_16_HZ               (0X03)
#define SH200L_ACC_ODR_32_HZ               (0x02)
#define SH200L_ACC_ODR_64_HZ               (0x01)
#define SH200L_ACC_ODR_128_LP_HZ           (0x00)
#define SH200L_ACC_ODR_128_HZ              (0x03)
#define SH200L_ACC_ODR_256_HZ              (0x02)
#define SH200L_ACC_ODR_512_HZ              (0x01)
#define SH200L_ACC_ODR_1024_HZ             (0x00)
#define SH200L_ACC_ODR_MSK                 (0X18)
#define SH200L_ACC_ODR_POS                 (3)


#define SH200L_GYRO_ODR_31_HZ              (0x03)
#define SH200L_GYRO_ODR_250_HZ             (0x02)
#define SH200L_GYRO_ODR_500_HZ             (0x01)
#define SH200L_GYRO_ODR_1000_HZ            (0x00)
#define SH200L_GYRO_ODR_8K_HZ              (0x04)
#define SH200L_GYRO_ODR_16K_HZ             (0x05)
#define SH200L_GYRO_ODR_32K_HZ             (0x06)

#define SH200L_GYRO_ODR_MSK                (0X0E)
#define SH200L_GYRO_ODR_POS                (1)


//#define SH200L_GYRO_RANGE_125			   (0x4)
#define SH200L_GYRO_RANGE_245              (0x3)
#define SH200L_GYRO_RANGE_500              (0x2)
#define SH200L_GYRO_RANGE_1000             (0x1)
#define SH200L_GYRO_RANGE_2000             (0x0)
#define SH200L_GYRO_RANGE_MSK              (0X7)
#define SH200L_GYRO_RANGE_POS              (0)


#define SH200L_GYRO_SENSITIVITY_245DPS     (7633)
#define SH200L_GYRO_SENSITIVITY_500DPS     (15267)
#define SH200L_GYRO_SENSITIVITY_1000DPS    (30487)
#define SH200L_GYRO_SENSITIVITY_2000DPS    (60975)


#define SH200L_SHIFT_EIGHT_BITS            (8)
#define SH200L_16_BIT_SHIFT                (0xFF)
#define SH200L_ACC_MUL                     (1000)
#define SH200L_GYRO_MUL                    (1)

#define SH200L_ACC_DEFAULT_ODR_100HZ       (100)
#define SH200L_GYRO_DEFAULT_ODR_100HZ      (100)

#define SH200L_GET_BITSLICE(regvar, bitname)\
((regvar & bitname##_MSK) >> bitname##_POS)

#define SH200L_SET_BITSLICE(regvar, bitname, val)\
((regvar & ~bitname##_MSK) | ((val<<bitname##_POS)&bitname##_MSK))

static int32_t sh200l_acc_factor[ACC_RANGE_MAX] = { SH200L_ACC_SENSITIVITY_2G, SH200L_ACC_SENSITIVITY_4G, 
                                     SH200L_ACC_SENSITIVITY_8G, SH200L_ACC_SENSITIVITY_16G };
static int32_t sh200l_gyro_factor[GYRO_RANGE_MAX] = {0, SH200L_GYRO_SENSITIVITY_245DPS, SH200L_GYRO_SENSITIVITY_500DPS, 
                                     SH200L_GYRO_SENSITIVITY_1000DPS, SH200L_GYRO_SENSITIVITY_2000DPS };
static int32_t cur_acc_factor = 0;
static int32_t cur_gyro_factor = 0;
static int32_t g_sh200lflag = 0;

static int32_t g_sleep = 0;
static int32_t g_powerdown = 0;
static int32_t g_low_odr = 0;





i2c_dev_t sh200l_ctx = {
    .port = 4,
    .config.address_width = 8,
    .config.freq = 400000,
    .config.dev_addr = SH200L_I2C_ADDR,
};

UNUSED static void drv_acc_senodia_sh200l_dump_reg(void)
{

  uint8_t rw_buffer[32] = {0};
  uint8_t reg_map[] = {
      0xc2,
      0x0E, 
      0x0F, 
      0x11, 
      0x12, 
      0x16, 
      0x2B,
      0xba,
  };

  uint8_t i = 0;
  uint16_t n = sizeof(reg_map)/sizeof(reg_map[0]);
  int ret = 0;
 
  for(i=0; i<n;i++) {
    ret = sensor_i2c_read(&sh200l_ctx, reg_map[i], &rw_buffer[i],I2C_REG_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return;
    }

  }
	
}


static int drv_acc_gyro_senodia_sh200l_hw_init(i2c_dev_t* drv)
{

	uint8_t value = 0x00;
	int ret = 0;

    
	/*set acc odr 256hz*/
	value = 0x91;
	ret = sensor_i2c_write(drv, SH200L_ACC_CONFIG,  &value, I2C_DATA_LEN, I2C_OP_RETRIES);
	if(unlikely(ret)){
        return -1;
    }
	
	/*set gyro odr 500hz*/
	value = 0x13;
	ret = sensor_i2c_write(drv, SH200L_GYRO_CONFIG,  &value, I2C_DATA_LEN, I2C_OP_RETRIES);
	if(unlikely(ret)){
        return -1;
    }

	
	/*set gyro dlpf 50hz*/
	value = 0x03;
	ret = sensor_i2c_write(drv, SH200L_GYRO_DLPF,  &value, I2C_DATA_LEN, I2C_OP_RETRIES);
	if(unlikely(ret)){
        return -1;
    }


	/*set no buffer mode*/
	value = 0x00;
	ret = sensor_i2c_write(drv, SH200L_FIFO_CONFIG,  &value, I2C_DATA_LEN, I2C_OP_RETRIES);
	if(unlikely(ret)){
        return -1;
    }

	
	/*set acc range +-8G*/
	value = 0x01;
	ret = sensor_i2c_write(drv, SH200L_ACC_RANGE,  &value, I2C_DATA_LEN, I2C_OP_RETRIES);
	if(unlikely(ret)){
        return -1;
    }


	/*set gyro range +-2000/s*/
	value = 0x00;
	ret = sensor_i2c_write(drv, SH200L_GYRO_RANGE,  &value, I2C_DATA_LEN, I2C_OP_RETRIES);
	if(unlikely(ret)){
        return -1;
    }


	value = 0xC0;
	ret = sensor_i2c_write(drv, SH200L_REG_SET1,  &value, I2C_DATA_LEN, I2C_OP_RETRIES);
	if(unlikely(ret)){
        return -1;
    }

	value = 0x10;
	ret = sensor_i2c_write(drv, SH200L_REG_SET2,  &value, I2C_DATA_LEN, I2C_OP_RETRIES);
	if(unlikely(ret)){
        return -1;
    }

	aos_msleep(1);

        
	value = 0x00;
	ret = sensor_i2c_write(drv, SH200L_REG_SET2,  &value, I2C_DATA_LEN, I2C_OP_RETRIES);
	if(unlikely(ret)){
        return -1;
    }

	
	aos_msleep(10);


	return 0;
}




static int drv_acc_gyro_senodia_sh200l_soft_reset(i2c_dev_t* drv)
{
    int ret = 0;
    uint8_t value = 0x00;


	/*soft reset*/
	value = 0x00;
	sensor_i2c_write(drv, SH200L_SOFT_RESET,  &value, I2C_DATA_LEN, I2C_OP_RETRIES);

	aos_msleep(1);

	/*ADC Reset*/
	value = 0x0E;
	ret = sensor_i2c_write(drv, SH200L_ADC_RESET,  &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return -1;
    }
	
	aos_msleep(1);


	value = 0x0A;
	ret = sensor_i2c_write(drv, SH200L_ADC_RESET,  &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return -1;
    }

    return 0;
}

static int drv_acc_gyro_senodia_sh200l_validate_id(i2c_dev_t* drv, uint8_t id_value)
{
    uint8_t value = 0x00;
    int ret = 0, i = 3;
    (void)ret;

    if(drv == NULL){
        return -1;
    }

	do{
		ret = sensor_i2c_read(drv, SH200L_ACC_GYRO_WHO_AM_I_REG, &value, I2C_DATA_LEN, I2C_OP_RETRIES);	
	}while((value != 0x18) && (i-- > 0));
	
    if (id_value != value){
        return -1;
    }
    
    return 0;
}

static int drv_acc_senodia_sh200l_set_power_mode(i2c_dev_t* drv, dev_power_mode_e mode)
{
    uint8_t value = 0x00;
    int ret = 0;

    ret = sensor_i2c_read(drv, SH200L_ACC_CONFIG, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }

    switch(mode){
        case DEV_POWER_ON:{
			/*1. bit2 ==>0; bit0 ==>1 normal mode && filter enable*/
			value &= (~(1<<2));
			value |= (1<<0);

			/*2. set odr*/
            value = SH200L_SET_BITSLICE(value,SH200L_ACC_ODR, SH200L_ACC_ODR_128_HZ);
            ret = sensor_i2c_write(drv, SH200L_ACC_CONFIG, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
            if(unlikely(ret)){
                return ret;
            }
			g_sleep = 0;
			g_powerdown = 0;
        }break;
        
        case DEV_POWER_OFF:{
			/*1. filter disable*/
            value &= (~(1<<0));
            ret = sensor_i2c_write(drv, SH200L_ACC_CONFIG, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
            if(unlikely(ret)){
                return ret;
            }
			
			/*2. low power mode*/
			value |= (1<<2);
            ret = sensor_i2c_write(drv, SH200L_ACC_CONFIG, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
            if(unlikely(ret)){
                return ret;
            }
			g_powerdown = 1;
        }break;
            
        case DEV_SLEEP:{	
			/*1. filter disable */
            value &= (~(1<<0));
            ret = sensor_i2c_write(drv, SH200L_ACC_CONFIG, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
            if(unlikely(ret)){
                return ret;
            }
			
			/*2. lower power mode*/
			value |= (1<<2);
            ret = sensor_i2c_write(drv, SH200L_ACC_CONFIG, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
            if(unlikely(ret)){
                return ret;
            }
			
			/*3. bit2==>1; bit0 ==> 1  set odr &&  filter enable*/
			value |= (1<<0);
            value = SH200L_SET_BITSLICE(value,SH200L_ACC_ODR, SH200L_ACC_ODR_16_HZ);
            ret = sensor_i2c_write(drv, SH200L_ACC_CONFIG, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
            if(unlikely(ret)){
                return ret;
            }
			g_sleep = 1;
        }break;
        
       default:break;
    }




    return 0;
}

static uint8_t drv_acc_senodia_sh200l_hz2odr(uint32_t hz)
{
    if(hz > 512)
        return SH200L_ACC_ODR_1024_HZ;
    else if(hz > 256)
        return SH200L_ACC_ODR_512_HZ;
    else if(hz > 128)
        return SH200L_ACC_ODR_256_HZ;
    else if(hz > 64){
    	if(g_sleep == 0){
    		g_low_odr = 0;
        	return SH200L_ACC_ODR_128_HZ;
    	}
		else{
			g_low_odr = 1;
        	return SH200L_ACC_ODR_128_LP_HZ;
		}
    }
    else if(hz > 32)
        return SH200L_ACC_ODR_64_HZ;
    else if(hz > 16)
        return SH200L_ACC_ODR_32_HZ;
    else if(hz > 8)
        return SH200L_ACC_ODR_16_HZ;
    else
        return SH200L_ACC_ODR_16_HZ;


	if(hz > 128)
		g_low_odr = 0;
	else if(hz < 64)
		g_low_odr = 1;

}


static int drv_acc_senodia_sh200l_set_odr(i2c_dev_t* drv, uint32_t hz)
{
    int ret = 0;
    uint8_t value = 0x00;
    uint8_t odr = drv_acc_senodia_sh200l_hz2odr(hz);



    ret = sensor_i2c_read(drv, SH200L_ACC_CONFIG, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }


	if(1 == g_low_odr){
        value &= (~(1<<0));
        ret = sensor_i2c_write(drv, SH200L_ACC_CONFIG, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
        if(unlikely(ret)){
             return ret;
        }
			
		value |= (1<<2);
        ret = sensor_i2c_write(drv, SH200L_ACC_CONFIG, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
        if(unlikely(ret)){
             return ret;
        }

		value |= (1<<0);
        ret = sensor_i2c_write(drv, SH200L_ACC_CONFIG, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
        if(unlikely(ret)){
                return ret;
        }
		
	}


    value = SH200L_SET_BITSLICE(value,SH200L_ACC_ODR,odr);

    ret = sensor_i2c_write(drv, SH200L_ACC_CONFIG, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }


    return 0;    
}

static int drv_acc_senodia_sh200l_set_range(i2c_dev_t* drv, uint32_t range)
{
    int ret = 0;
    uint8_t value = 0x00;
    uint8_t tmp = 0;

    ret = sensor_i2c_read(drv, SH200L_ACC_RANGE, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }

    switch (range){
    
        case ACC_RANGE_4G:{
            tmp = SH200L_ACC_RANGE_4G;
        }break;
        
        case ACC_RANGE_8G:{
            tmp = SH200L_ACC_RANGE_8G;
        }break;
            
        case ACC_RANGE_16G:{
            tmp = SH200L_ACC_RANGE_16G;
        }break;
        
        default:break;
    }
    
    value  = SH200L_SET_BITSLICE(value,SH200L_ACC_RANGE,tmp);
    ret = sensor_i2c_write(drv, SH200L_ACC_RANGE, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }

    if((range >= ACC_RANGE_2G)&&(range <= ACC_RANGE_16G)){
        cur_acc_factor = sh200l_acc_factor[range];
    }
    
    return 0;
}


static void drv_acc_senodia_sh200l_irq_handle(void)
{
    /* no handle so far */
}

static int drv_acc_senodia_sh200l_open(void)
{
    int ret = 0;


	drv_acc_gyro_senodia_sh200l_hw_init(&sh200l_ctx);

	ret = drv_acc_senodia_sh200l_set_range(&sh200l_ctx, ACC_RANGE_8G);
    if(unlikely(ret)){
        return -1;
    }

    ret  = drv_acc_senodia_sh200l_set_power_mode(&sh200l_ctx, DEV_POWER_ON);
    if(unlikely(ret)){
        return -1;
    }


    ret = drv_acc_senodia_sh200l_set_odr(&sh200l_ctx, SH200L_ACC_DEFAULT_ODR_100HZ);
    if(unlikely(ret)){
        return -1;
    }

     
    return 0;

}

static int drv_acc_senodia_sh200l_close(void)
{
    int ret = 0;
    ret  = drv_acc_senodia_sh200l_set_power_mode(&sh200l_ctx, DEV_POWER_OFF);
    if(unlikely(ret)){
        return -1;
    }
    return 0;
}




static int drv_acc_senodia_sh200l_read(void *buf, size_t len)
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

    ret  = sensor_i2c_read(&sh200l_ctx, SH200L_OUTPUT_ACC_X,  &reg[0], I2C_REG_LEN, I2C_OP_RETRIES);
    ret |= sensor_i2c_read(&sh200l_ctx, SH200L_OUTPUT_ACC_X + 1,  &reg[1], I2C_REG_LEN, I2C_OP_RETRIES);
    ret |= sensor_i2c_read(&sh200l_ctx, SH200L_OUTPUT_ACC_X + 2,  &reg[2], I2C_REG_LEN, I2C_OP_RETRIES);
    ret |= sensor_i2c_read(&sh200l_ctx, SH200L_OUTPUT_ACC_X + 3,  &reg[3], I2C_REG_LEN, I2C_OP_RETRIES);
    ret |= sensor_i2c_read(&sh200l_ctx, SH200L_OUTPUT_ACC_X + 4,  &reg[4], I2C_REG_LEN, I2C_OP_RETRIES);
    ret |= sensor_i2c_read(&sh200l_ctx, SH200L_OUTPUT_ACC_X + 5,  &reg[5], I2C_REG_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return -1;
    }
    accel->data[DATA_AXIS_X] = (int16_t)((((int16_t)((int8_t)reg[1]))<< SH200L_SHIFT_EIGHT_BITS)|(reg[0]));
    accel->data[DATA_AXIS_Y] = (int16_t)((((int16_t)((int8_t)reg[3]))<< SH200L_SHIFT_EIGHT_BITS)|(reg[2]));
    accel->data[DATA_AXIS_Z] = (int16_t)((((int16_t)((int8_t)reg[5]))<< SH200L_SHIFT_EIGHT_BITS)|(reg[4]));

    if(cur_acc_factor != 0){
        accel->data[DATA_AXIS_X] = (accel->data[DATA_AXIS_X] * cur_acc_factor)/SH200L_ACC_MUL;
        accel->data[DATA_AXIS_Y] = (accel->data[DATA_AXIS_Y] * cur_acc_factor)/SH200L_ACC_MUL;
        accel->data[DATA_AXIS_Z] = (accel->data[DATA_AXIS_Z] * cur_acc_factor)/SH200L_ACC_MUL;
    }
    
    /*SH200L_LOG("acc x = %d, y = %d, z = %d , cur_acc_factor = %d\n",  accel->data[DATA_AXIS_X], accel->data[DATA_AXIS_Y], accel->data[DATA_AXIS_Z], cur_acc_factor);*/
    accel->timestamp = aos_now_ms();


    return (int)size;
}

static int drv_acc_senodia_sh200l_ioctl(int cmd, unsigned long arg)
{
    int ret = 0;

    switch(cmd){
        case SENSOR_IOCTL_ODR_SET:{
            ret = drv_acc_senodia_sh200l_set_odr(&sh200l_ctx, arg);
            if(unlikely(ret)){
                return -1;
            }
        }break;
        case SENSOR_IOCTL_RANGE_SET:{
            ret = drv_acc_senodia_sh200l_set_range(&sh200l_ctx, arg);
            if(unlikely(ret)){
                return -1;
            }
        }break;
        case SENSOR_IOCTL_SET_POWER:{
            ret = drv_acc_senodia_sh200l_set_power_mode(&sh200l_ctx, arg);
            if(unlikely(ret)){
                return -1;
            }
        }break;
        case SENSOR_IOCTL_GET_INFO:{ 
            /* fill the dev info here */
            dev_sensor_info_t *info = (dev_sensor_info_t*)arg;
            info->model = "SH200L";
            info->range_max = 16;
            info->range_min = 4;
            info->unit = mg;
        }break;
       
       default:break;
    }

    return 0;
}


int drv_acc_senodia_sh200l_init(void){
    int ret = 0;
    sensor_obj_t sensor;
    memset(&sensor, 0, sizeof(sensor));

    /* fill the sensor obj parameters here */
    sensor.io_port    = I2C_PORT;
    sensor.tag        = TAG_DEV_ACC;
    sensor.path       = dev_acc_path;
    sensor.open       = drv_acc_senodia_sh200l_open;
    sensor.close      = drv_acc_senodia_sh200l_close;
    sensor.read       = drv_acc_senodia_sh200l_read;
    sensor.write      = NULL;
    sensor.ioctl      = drv_acc_senodia_sh200l_ioctl;
    sensor.irq_handle = drv_acc_senodia_sh200l_irq_handle;

    ret = sensor_create_obj(&sensor);
    if(unlikely(ret)){
        return -1;
    }
    ret = drv_acc_gyro_senodia_sh200l_validate_id(&sh200l_ctx, SH200L_CHIP_ID_VALUE);
    if(unlikely(ret)){
        return -1;
    }

    if(0 == g_sh200lflag){
        ret = drv_acc_gyro_senodia_sh200l_soft_reset(&sh200l_ctx);
        if(unlikely(ret)){
            return -1;
        }

        ret = drv_acc_senodia_sh200l_set_power_mode(&sh200l_ctx, DEV_POWER_OFF);
        if(unlikely(ret)){
            return -1;
        }
	#if 0
		ret = drv_acc_gyro_senodia_sh200l_hw_init(&sh200l_ctx);
		if(unlikely(ret)){
			return -1;
		}
	#endif
        g_sh200lflag = 1;
    }
    else{
    }
    

    return 0;
}


static int drv_gyro_senodia_sh200l_set_power_mode(i2c_dev_t* drv, dev_power_mode_e mode)
{
    uint8_t value = 0x00;
    int ret = 0;
    
    ret = sensor_i2c_read(drv, SH200L_GYRO_CONFIG, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }

    switch(mode){
        case DEV_POWER_ON:{
            
            value = SH200L_SET_BITSLICE(value,SH200L_GYRO_ODR,SH200L_GYRO_ODR_31_HZ);
            ret = sensor_i2c_write(drv, SH200L_GYRO_CONFIG, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
            if(unlikely(ret)){
                return ret;
            }
        }break;
        
        case DEV_POWER_OFF:{
            value &= (~(1<<0)); 
            ret = sensor_i2c_write(drv, SH200L_GYRO_CONFIG, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
            if(unlikely(ret)){
                return ret;
            }
        }break;
            
        case DEV_SLEEP:{
            value = SH200L_SET_BITSLICE(value,SH200L_GYRO_ODR,SH200L_GYRO_ODR_31_HZ);
            ret = sensor_i2c_write(drv, SH200L_GYRO_CONFIG, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
            if(unlikely(ret)){
                return ret;
            }

        }break;
        
       default:break;
    }
    return 0;
}

static uint8_t drv_gyro_senodia_sh200l_hz2odr(uint32_t hz)
{

    if(hz > 1660)
        return SH200L_GYRO_ODR_32K_HZ;
    else if(hz > 833)
        return SH200L_GYRO_ODR_16K_HZ;
    else if(hz > 416)
        return SH200L_GYRO_ODR_8K_HZ;
    else if(hz > 208)
        return SH200L_GYRO_ODR_1000_HZ;
    else if(hz > 104)
        return SH200L_GYRO_ODR_500_HZ;
    else if(hz > 52)
        return SH200L_GYRO_ODR_250_HZ;
    else if(hz > 26)
        return SH200L_GYRO_ODR_31_HZ;
    else
        return SH200L_GYRO_ODR_31_HZ;

}


static int drv_gyro_senodia_sh200l_set_odr(i2c_dev_t* drv, uint32_t hz)
{
    int ret = 0;
    uint8_t value = 0x00;
    uint8_t odr = drv_gyro_senodia_sh200l_hz2odr(hz);

    ret = sensor_i2c_read(drv, SH200L_GYRO_CONFIG, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }

    value = SH200L_SET_BITSLICE(value,SH200L_GYRO_ODR,odr);

    ret = sensor_i2c_write(drv, SH200L_GYRO_CONFIG, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }
    return 0;    
}

static int drv_gyro_senodia_sh200l_set_range(i2c_dev_t* drv, uint32_t range)
{
    int ret = 0;
    uint8_t value = 0x00;
    uint8_t tmp = 0;

    ret = sensor_i2c_read(drv, SH200L_GYRO_RANGE, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }

    switch (range){
        case GYRO_RANGE_250DPS:{
            tmp = SH200L_GYRO_RANGE_245;
        }break;
        
        case GYRO_RANGE_500DPS:{
            tmp = SH200L_GYRO_RANGE_500;
        }break;
        
        case GYRO_RANGE_1000DPS:{
            tmp = SH200L_GYRO_RANGE_1000;
        }break;
            
        case GYRO_RANGE_2000DPS:{
            tmp = SH200L_GYRO_RANGE_2000;
        }break;
        
        default:break;
    }
    
    value  = SH200L_SET_BITSLICE(value,SH200L_GYRO_RANGE,tmp);
    ret = sensor_i2c_write(drv, SH200L_GYRO_RANGE, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }
    
    if((range >= GYRO_RANGE_250DPS)&&(range <= GYRO_RANGE_2000DPS)){
        cur_gyro_factor = sh200l_gyro_factor[range];
    }
    
    return 0;
}


static void drv_gyro_senodia_sh200l_irq_handle(void)
{
    /* no handle so far */
}

static int drv_gyro_senodia_sh200l_open(void)
{
    int ret = 0;

	drv_acc_gyro_senodia_sh200l_hw_init(&sh200l_ctx);


    ret  = drv_gyro_senodia_sh200l_set_power_mode(&sh200l_ctx, DEV_POWER_ON);
    if(unlikely(ret)){
        return -1;
    }

    ret = drv_gyro_senodia_sh200l_set_range(&sh200l_ctx, GYRO_RANGE_2000DPS);
    if(unlikely(ret)){
        return -1;
    }

#ifdef FASTMODE_TEST
	ret = drv_gyro_senodia_sh200l_set_odr(&sh200l_ctx, 200);
	if(unlikely(ret)){
	return -1;
	}

#else
    ret = drv_gyro_senodia_sh200l_set_odr(&sh200l_ctx, SH200L_GYRO_DEFAULT_ODR_100HZ);
    if(unlikely(ret)){
        return -1;
    }
#endif
    
    return 0;

}

static int drv_gyro_senodia_sh200l_close(void)
{
    int ret = 0;
    ret  = drv_gyro_senodia_sh200l_set_power_mode(&sh200l_ctx, DEV_POWER_OFF);
    if(unlikely(ret)){
        return -1;
    }
    return 0;
}

static int drv_gyro_senodia_sh200l_read(void *buf, size_t len)
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

    ret  = sensor_i2c_read(&sh200l_ctx, SH200L_OUTPUT_GYRO_X,  	   &reg[0], I2C_REG_LEN, I2C_OP_RETRIES);
    ret |= sensor_i2c_read(&sh200l_ctx, SH200L_OUTPUT_GYRO_X + 1,  &reg[1], I2C_REG_LEN, I2C_OP_RETRIES);
    ret |= sensor_i2c_read(&sh200l_ctx, SH200L_OUTPUT_GYRO_X + 2,  &reg[2], I2C_REG_LEN, I2C_OP_RETRIES);
    ret |= sensor_i2c_read(&sh200l_ctx, SH200L_OUTPUT_GYRO_X + 3,  &reg[3], I2C_REG_LEN, I2C_OP_RETRIES);
    ret |= sensor_i2c_read(&sh200l_ctx, SH200L_OUTPUT_GYRO_X + 4,  &reg[4], I2C_REG_LEN, I2C_OP_RETRIES);
    ret |= sensor_i2c_read(&sh200l_ctx, SH200L_OUTPUT_GYRO_X + 5,  &reg[5], I2C_REG_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return -1;
    }
    gyro->data[DATA_AXIS_X] = (int16_t)((((int32_t)((int8_t)reg[1]))<< SH200L_SHIFT_EIGHT_BITS)|(reg[0]));
    gyro->data[DATA_AXIS_Y] = (int16_t)((((int32_t)((int8_t)reg[3]))<< SH200L_SHIFT_EIGHT_BITS)|(reg[2]));
    gyro->data[DATA_AXIS_Z] = (int16_t)((((int32_t)((int8_t)reg[5]))<< SH200L_SHIFT_EIGHT_BITS)|(reg[4]));

    if(cur_gyro_factor != 0){
        gyro->data[DATA_AXIS_X] = (gyro->data[DATA_AXIS_X] * cur_gyro_factor)/SH200L_GYRO_MUL;
        gyro->data[DATA_AXIS_Y] = (gyro->data[DATA_AXIS_Y] * cur_gyro_factor)/SH200L_GYRO_MUL;
        gyro->data[DATA_AXIS_Z] = (gyro->data[DATA_AXIS_Z] * cur_gyro_factor)/SH200L_GYRO_MUL;
    }
    gyro->timestamp = aos_now_ms();

    /*SH200L_LOG("gyro x = %d, y = %d, z = %d , cur_gyro_factor = %d\n",  gyro->data[DATA_AXIS_X], gyro->data[DATA_AXIS_Y], gyro->data[DATA_AXIS_Z], cur_gyro_factor);*/

    return (int)size;
}

UNUSED static int drv_acc_gyro_senodia_sh200l_self_test(i2c_dev_t* drv, int32_t* diff)
{

	uint8_t gainRegister[32] =  {147, 148, 149, 153, 154, 155, 159, 160, 161, 187, 198, 199, 200, 203, 204, 205, 206, 207, 208, 209, 210, 211, 212, 213, 214, 215, 219, 220, 221, 222, 223, 224};

	uint8_t gainData[26] =  {0, 4, 0, 0, 4, 0, 0, 4, 0, 0x10,0xC0, 0x40, 0x40, 0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	
	uint8_t gainData1[3] =  { 0xA0, 0x20, 0x20};
	
	uint8_t i, reg, data;

	int16_t c_gyro_self[3] = {0};


	int sumGyroData[3] = {0};
	int16_t avgGyroData[3] = {0};
	int16_t GyroData[3] = {0};
	int16_t avgGyroData1[3] = {0};

	uint8_t self_gain_data[32] = {0};
	uint8_t temp[6]= {0};

	int ret = 0;


	if(NULL  == diff)
		return -1;
	

	for( i = 0 ; i < 32 ; i++ ){
		reg = gainRegister[i];

		ret = sensor_i2c_read(drv, reg,  &(self_gain_data[i]), I2C_DATA_LEN, I2C_OP_RETRIES);
		if(unlikely(ret)){
			return -1;
		}
		aos_msleep(1);
	}

	c_gyro_self[0] = self_gain_data[26] + self_gain_data[27] * 256;
  	c_gyro_self[1] = self_gain_data[28] + self_gain_data[29] * 256;
  	c_gyro_self[2] = self_gain_data[30] + self_gain_data[31] * 256;

	for( i = 0 ; i < 26 ; i++ ){
		reg = gainRegister[i];
		data = gainData[i];
		ret = sensor_i2c_write(drv, reg, &data, I2C_DATA_LEN, I2C_OP_RETRIES);
        if(unlikely(ret)){
                return ret;
        }
		aos_msleep(1);
	}


	for(i=0;i<3;i++){
	  sumGyroData[i] = 0;
	  avgGyroData[i] = 0;
	}
	
	aos_msleep(500);


	for(i=30; i>0; i--){

		aos_msleep(1);

		ret = sensor_i2c_read(drv, SH200L_OUTPUT_GYRO_X,(uint8_t *)  &temp, 6, I2C_OP_RETRIES);
		if(unlikely(ret)){
			return -1;
		}

		GyroData[0]=((int16_t)((temp[1]<<8)|temp[0]));//   
    	GyroData[1]=((int16_t)((temp[3]<<8)|temp[2]));// 
    	GyroData[2]=((int16_t)((temp[5]<<8)|temp[4]));//
   
    	sumGyroData[0] += GyroData[0];
    	sumGyroData[1] += GyroData[1];
    	sumGyroData[2] += GyroData[2];
  	}


	for(i = 0; i < 3; i++){
	    avgGyroData[i] = sumGyroData[i] / 30;
	}



	for( i = 0 ; i < 3 ; i++ ){
		reg = gainRegister[i + 10];
		data = gainData1[i];
		ret = sensor_i2c_write(drv, reg, &data, I2C_DATA_LEN, I2C_OP_RETRIES);
        if(unlikely(ret)){
                return ret;
        }
		aos_msleep(1);
	}


	aos_msleep(500);


	for(i=0;i<3;i++){
		sumGyroData[i] = 0;
		avgGyroData1[i] = 0;
	}
	
	
	for(i=30; i>0; i--){
	
		aos_msleep(1);
	
		ret = sensor_i2c_read(drv, SH200L_OUTPUT_GYRO_X,  (uint8_t *)&temp, 6, I2C_OP_RETRIES);
		if(unlikely(ret)){
				return -1;
		}
	
		GyroData[0]=((int16_t)((temp[1]<<8)|temp[0]));//   
		GyroData[1]=((int16_t)((temp[3]<<8)|temp[2]));// 
		GyroData[2]=((int16_t)((temp[5]<<8)|temp[4]));//
	   
		sumGyroData[0] += GyroData[0];
		sumGyroData[1] += GyroData[1];
		sumGyroData[2] += GyroData[2];
	}
	
	
	for(i = 0; i < 3; i++){
		 avgGyroData1[i] = sumGyroData[i] / 30;
	}


	for(i = 0; i < 3; i++){
		diff[i] = avgGyroData[i] - avgGyroData1[i];
		if((diff[i] > (c_gyro_self[i] + c_gyro_self[i] / 3)) ||(diff[i] < (c_gyro_self[i] - c_gyro_self[i] / 3))){
			ret = -1;
			break;
		}
	}
	
	
	return ret;
}


static int drv_gyro_senodia_sh200l_ioctl(int cmd, unsigned long arg)
{
    int ret = 0;
    
    switch(cmd){
        case SENSOR_IOCTL_ODR_SET:{
            ret = drv_gyro_senodia_sh200l_set_odr(&sh200l_ctx, arg);
            if(unlikely(ret)){
                return -1;
            }
        }break;
        case SENSOR_IOCTL_RANGE_SET:{
            ret = drv_gyro_senodia_sh200l_set_range(&sh200l_ctx, arg);
            if(unlikely(ret)){
                return -1;
            }
        }break;
        case SENSOR_IOCTL_SET_POWER:{
            ret = drv_gyro_senodia_sh200l_set_power_mode(&sh200l_ctx, arg);
            if(unlikely(ret)){
                return -1;
            }
        }break;
        case SENSOR_IOCTL_GET_INFO:{ 
            /* fill the dev info here */
            dev_sensor_info_t *info = (dev_sensor_info_t *)arg;
            info->model = "SH200L";
            info->range_max = 2000;
            info->range_min = 125;
            info->unit = udps;
        }break;
	#if 0
		case SENSOR_IOCTL_SELF_TEST:{
	        ret = drv_acc_gyro_senodia_sh200l_self_test(&sh200l_ctx, info->data);
          	SH200L_LOG("%d   %d   %d\n",info->data[0],info->data[1],info->data[2]);
           return ret;

		}break;
     #endif
       default:break;
    }

    return 0;
}

int drv_gyro_senodia_sh200l_init(void){
    int ret = 0;
    sensor_obj_t sensor;
    memset(&sensor, 0, sizeof(sensor));

    /* fill the sensor obj parameters here */
    sensor.io_port    = I2C_PORT;
    sensor.tag        = TAG_DEV_GYRO;
    sensor.path       = dev_gyro_path;
    sensor.open       = drv_gyro_senodia_sh200l_open;
    sensor.close      = drv_gyro_senodia_sh200l_close;
    sensor.read       = drv_gyro_senodia_sh200l_read;
    sensor.write      = NULL;
    sensor.ioctl      = drv_gyro_senodia_sh200l_ioctl;
    sensor.irq_handle = drv_gyro_senodia_sh200l_irq_handle;

    ret = sensor_create_obj(&sensor);
    if(unlikely(ret)){
        return -1;
    }

    ret = drv_acc_gyro_senodia_sh200l_validate_id(&sh200l_ctx, SH200L_CHIP_ID_VALUE);
    if(unlikely(ret)){
        return -1;
    }

    if(0 == g_sh200lflag){

        ret = drv_acc_gyro_senodia_sh200l_soft_reset(&sh200l_ctx);
        if(unlikely(ret)){
            return -1;
        }

		ret = drv_acc_gyro_senodia_sh200l_hw_init(&sh200l_ctx);
		if(unlikely(ret)){
			return -1;
		}

        g_sh200lflag = 1;	
    }
    else{
    }

    /* update the phy sensor info to sensor hal */
    return 0;
}



SENSOR_DRV_ADD(drv_acc_senodia_sh200l_init);
SENSOR_DRV_ADD(drv_gyro_senodia_sh200l_init);

