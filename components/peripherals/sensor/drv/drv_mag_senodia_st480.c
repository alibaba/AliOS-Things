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

#define ST480_DEBUG

#ifdef ST480_DEBUG
#define ST480_LOG  LOG
#else
#define ST480_LOG 
#endif

//#define ST480_I2C_ADDR1                 0x1C
#define ST480_I2C_ADDR2                   0x0C
#define ST480_I2C_ADDR_TRANS(n)           ((n)<<1)  
#define ST480_I2C_ADDR                    ST480_I2C_ADDR_TRANS(ST480_I2C_ADDR2)
#define ST480_BIT(x)                      (x)

#define I_AM_ST480                        0x7C  //TODO
#define ST480_MAG_WHO_AM_I_REG            0x00


#define WRITE_REGISTER_CMD 				  0x60
#define READ_REGISTER_CMD 			      0x50
#define EXIT_REGISTER_CMD 			      0x80
#define MEMORY_RECALL_CMD 			      0xD0
#define MEMORY_STORE_CMD 			      0xE0
#define RESET_CMD 					      0xF0
#define ST480_REG_DRR_SHIFT 			  2
#define CALIBRATION_REG  				  (0x02 << ST480_REG_DRR_SHIFT)
#define CALIBRATION_DATA_LOW			  0x18
#define CALIBRATION_DATA_HIGH 			  0x00

#define ONE_INIT_DATA_LOW 				  0x7C
#define ONE_INIT_DATA_HIGH 				  0x00
#define ONE_INIT_REG 					  (0x00 << ST480_REG_DRR_SHIFT)

#define TWO_INIT_DATA_LOW 				  0x00
#define TWO_INIT_DATA_HIGH 				  0x00
#define TWO_INIT_REG 					  (0x02 << ST480_REG_DRR_SHIFT)

#define ST480_SINGLE_MODE				  1
#define SINGLE_MEASUREMENT_MODE_CMD 	  0x3F


#define READ_MEASUREMENT_CMD 			  0x4F

#define ST480_MAG_XY_SENSITIVITY_FOR_48GA  15
#define ST480_MAG_Z_SENSITIVITY_FOR_48GA   25


i2c_dev_t ST480_ctx = {
	.port = 4,
    .config.address_width = 8,
    .config.freq = 200000,
    .config.dev_addr = ST480_I2C_ADDR,
};


static int32_t sensor_i2c_single_write(i2c_dev_t *i2c, uint8_t *data, uint16_t size, uint32_t timeout)
{
	int32_t ret = 0;
	if(i2c == NULL){
		return -1;
	}

	ret = hal_i2c_master_send(i2c, i2c->config.dev_addr, data, size, AOS_WAIT_FOREVER);

	return ret;

}

static int32_t sensor_i2c_single_read(i2c_dev_t *i2c, uint8_t *data, uint16_t size, uint32_t timeout)
{
    int32_t ret = 0;
    if(i2c == NULL){
        return -1;
    }
    ret = hal_i2c_master_recv(i2c, i2c->config.dev_addr, data, size, AOS_WAIT_FOREVER);
    return ret;

}



static int drv_mag_sen_st480_validate_id(i2c_dev_t* drv, uint8_t id_value)
{
    uint8_t value = 0x00;
    int ret = 0, i = 1;
	uint8_t buf[5];

    if(drv == NULL){
        return -1;
    }

	do{
		buf[0] = READ_REGISTER_CMD;
		buf[1] = 0x00;

		ret = sensor_i2c_single_write(drv, buf, 2, I2C_OP_RETRIES);
		if(unlikely(ret)){
			return ret;
		}

		ret = sensor_i2c_single_read(drv, buf, 3, I2C_OP_RETRIES);
		if(unlikely(ret)){
			return ret;
		}

		ST480_LOG("senodia st480 id = %d.", buf[2]);

		value = buf[2];
	}while((buf[2] != I_AM_ST480) && (i-- > 0));


    if (id_value != value){
        return -1;
    }
    
    return 0;
}

static int drv_mag_sen_st480_set_power_mode(i2c_dev_t* drv, dev_power_mode_e mode)
{
    //no use

    return 0;
}

static int drv_mag_sen_st480_set_odr(i2c_dev_t* drv, uint8_t odr)
{

	//no use
	
    return 0;
}


static int drv_mag_sen_st480_set_range(i2c_dev_t* drv, uint32_t range)
{

	//no use
    
    return 0;
}

static int drv_mag_sen_st480_set_default_config(i2c_dev_t* drv)
{
    uint8_t buf[5] = {0};
    int ret = 0;

	//init register step 1
	buf[0] = WRITE_REGISTER_CMD;
	buf[1] = ONE_INIT_DATA_HIGH;
	buf[2] = ONE_INIT_DATA_LOW;
	buf[3] = ONE_INIT_REG; 
	ret = sensor_i2c_single_write(drv, buf, 4, I2C_OP_RETRIES);
	if(unlikely(ret)){
		return ret;
	}

	ret = sensor_i2c_single_read(drv, buf, 1, I2C_OP_RETRIES);
	if(unlikely(ret)){
		return ret;
	}

	//init register step 2
	buf[0] = WRITE_REGISTER_CMD;
	buf[1] = TWO_INIT_DATA_HIGH;
	buf[2] = TWO_INIT_DATA_LOW;
	buf[3] = TWO_INIT_REG;
	ret = sensor_i2c_single_write(drv, buf, 4, I2C_OP_RETRIES);
	if(unlikely(ret)){
		return ret;
	}

	ret = sensor_i2c_single_read(drv, buf, 1, I2C_OP_RETRIES);
	if(unlikely(ret)){
		return ret;
	}
	
	//set calibration register
	buf[0] = WRITE_REGISTER_CMD;
	buf[1] = CALIBRATION_DATA_HIGH;
	buf[2] = CALIBRATION_DATA_LOW;
	buf[3] = CALIBRATION_REG;
	ret = sensor_i2c_single_write(drv, buf, 4, I2C_OP_RETRIES);
	if(unlikely(ret)){
		return ret;
	}

	ret = sensor_i2c_single_read(drv, buf, 1, I2C_OP_RETRIES);
	if(unlikely(ret)){
		return ret;
	}

	//set mode config
  	buf[0] = SINGLE_MEASUREMENT_MODE_CMD;
  	ret = sensor_i2c_single_write(&ST480_ctx, buf, 1, I2C_OP_RETRIES);
  	if(unlikely(ret)){
		  return ret;
  	}

  	ret = sensor_i2c_single_read(&ST480_ctx, buf, 1, I2C_OP_RETRIES);
  	if(unlikely(ret)){
		  return ret;
  	}

    return 0;
}

static void drv_mag_sen_st480_irq_handle(void)
{
    /* no handle so far */
}

static int drv_mag_sen_st480_open(void)
{
    //no use
    ST480_LOG("drv_mag_sen_st480_open:");

    return 0;

}

static int drv_mag_sen_st480_close(void)
{

    //no use
    ST480_LOG("drv_mag_sen_st480_close:");

    return 0;
}

static int drv_mag_sen_st480_read(void* buf, size_t len)
{
  int ret = 0;
  size_t size;
  int16_t pnRawData[3] = {0};
  uint8_t buffer[9];
  uint8_t i = 0;
  
  mag_data_t* pdata = (mag_data_t*)buf;
   if(buf == NULL){
        return -1;
  }

  size = sizeof(mag_data_t);
  if(len < size){
        return -1;
  }

  buffer[0] = READ_MEASUREMENT_CMD;
  ret = sensor_i2c_single_write(&ST480_ctx, buffer, 1, I2C_OP_RETRIES);
  if(unlikely(ret)){
	  return ret;
  }
 
  ret = sensor_i2c_single_read(&ST480_ctx, buffer, 9, I2C_OP_RETRIES);
  if(unlikely(ret)){
	  return ret;
  }

  if (!((buffer[0]>>4) & 0X01)) {
  	
	  for(i=0; i<3; i++)
  	  {
    	pnRawData[i]=(int16_t)((((uint16_t)buffer[2*i + 3]) << 8) | ((uint16_t)buffer[2*i + 4]));
  	  }
  }


  pdata->data[0] =(int32_t)(pnRawData[0] * ST480_MAG_XY_SENSITIVITY_FOR_48GA / 10);
  pdata->data[1] =(int32_t)(pnRawData[1] * ST480_MAG_XY_SENSITIVITY_FOR_48GA / 10);
  pdata->data[2] =(int32_t)(pnRawData[2] * ST480_MAG_Z_SENSITIVITY_FOR_48GA / 10);


  pdata->timestamp = aos_now_ms();

  //set mode config
  buffer[0] = SINGLE_MEASUREMENT_MODE_CMD;
  ret = sensor_i2c_single_write(&ST480_ctx, buffer, 1, I2C_OP_RETRIES);
  if(unlikely(ret)){
		return ret;
  }
  
  ret = sensor_i2c_single_read(&ST480_ctx, buffer, 1, I2C_OP_RETRIES);
  if(unlikely(ret)){
		return ret;
  }

  return (int)size;
}


static int drv_mag_sen_st480_ioctl(int cmd, unsigned long arg)
{
    int ret = 0;
    
    switch(cmd){
        case SENSOR_IOCTL_ODR_SET:{
            ret = drv_mag_sen_st480_set_odr(&ST480_ctx, arg);
            if(unlikely(ret)){
                return -1;
            }
        }break;
        case SENSOR_IOCTL_RANGE_SET:{
            ret = drv_mag_sen_st480_set_range(&ST480_ctx, arg);
            if(unlikely(ret)){
                return -1;
            }
        }break;
        case SENSOR_IOCTL_SET_POWER:{
            ret = drv_mag_sen_st480_set_power_mode(&ST480_ctx, arg);
            if(unlikely(ret)){
                return -1;
            }
        }break;
        case SENSOR_IOCTL_GET_INFO:{ 
            /* fill the dev info here */
            dev_sensor_info_t *info = (dev_sensor_info_t *)arg;
            info->model = "ST480";
            info->range_max = 48;
            info->range_min = 48;
            info->unit = mGauss;
        }break;
       
       default:break;
    }

    return 0;
}

int drv_mag_sen_st480_init(void){
    int ret = 0;
    sensor_obj_t sensor;
    memset(&sensor, 0, sizeof(sensor));
    /* fill the sensor obj parameters here */
    sensor.io_port    = I2C_PORT;
    sensor.tag        = TAG_DEV_MAG;
    sensor.path       = dev_mag_path;
    sensor.open       = drv_mag_sen_st480_open;
    sensor.close      = drv_mag_sen_st480_close;
    sensor.read       = drv_mag_sen_st480_read;
    sensor.write      = NULL;
    sensor.ioctl      = drv_mag_sen_st480_ioctl;
    sensor.irq_handle = drv_mag_sen_st480_irq_handle;


    ret = sensor_create_obj(&sensor);
    if(unlikely(ret)){
        return -1;
    }

    ret = drv_mag_sen_st480_validate_id(&ST480_ctx, I_AM_ST480);
    if(unlikely(ret)){
        return -1;
    }

    ret = drv_mag_sen_st480_set_default_config(&ST480_ctx);
    if(unlikely(ret)){
        return -1;
    }
    
    /* update the phy sensor info to sensor hal */
    LOG("%s %s successfully \n", SENSOR_STR, __func__);
    return 0;
}

SENSOR_DRV_ADD(drv_mag_sen_st480_init);

