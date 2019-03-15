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

/*#define ST350MC_DEBUG*/

#ifdef ST350MC_DEBUG
#define ST350_LOG  LOG
#else
#define ST350_LOG 
#endif


#define ST350MC_REGISTER_SETTING_TOTAL   	 5  
#define ST350MC_OPERATION_TYPES_REG_TOTAL 	 4


#define ST350_I2C_ADDR2                   0x0D
#define ST350_I2C_ADDR_TRANS(n)           ((n)<<1)  
#define ST350_I2C_ADDR                    ST350_I2C_ADDR_TRANS(ST350_I2C_ADDR2)
#define ST350_BIT(x)                      (x)

#define I_AM_ST350                        0x53
#define ST350_MAG_WHO_AM_I_REG            0x00


#define ST350MC_REG_MINFO				0x01  
#define ST350MC_REG_STAT1				0x02
#define ST350MC_REG_DATAX_L				0x03
#define ST350MC_REG_DATAX_H		    	0x04
#define ST350MC_REG_DATAY_L				0x05
#define ST350MC_REG_DATAY_H		    	0x06 
#define ST350MC_REG_DATAZ_L				0x07
#define ST350MC_REG_DATAZ_H		    	0x08
#define ST350MC_REG_STAT2				0x09
#define ST350MC_REG_CNTRL1				0x0A
#define ST350MC_REG_CNTRL2				0x0B
#define ST350MC_REG_STCR				0x0C
#define ST350MC_REG_CNTRL3				0x0D
#define ST350MC_REG_TEMP_L				0x1C
#define ST350MC_REG_TEMP_H		  		0x1D

#define	ST350MC_REG_AVGCNTL				0x41
#define	ST350MC_REG_DSM1				0x6c

#define ST350MC_REG_X_GAIN_CNTL			0x50
#define ST350MC_REG_Y_GAIN_CNTL			0x52
#define ST350MC_REG_Z_GAIN_CNTL			0x54
#define ST350MC_REG_MHZ_CLK_TUNING  	0x56
#define ST350MC_REG_ANALOG_TEST_CTL3	0x72
#define ST350MC_REG_ANALOG_TEST_CTL7	0x76

#define ST350MC_REG_SSR             	0x42
#define ST350MC_REG_CTR             	0x40
#define ST350MC_REG_BTR             	0x57

#define ST350_MAG_SENSITIVITY_FOR_FS_20GA  3 
#define UTTOUG   						10000
#define UTTOMG   						10

typedef enum
{
ST350MC_SINGLE_MEASURE,
ST350MC_STANDBY,
ST350MC_SUSPEND
}st350_operation_mode_e;


typedef enum
{
  ST350MC_NORMAL,
  ST350MC_HIGH_PRECISION,
  ST350MC_HIGH_SPEED,
  ST350MC_LOW_POWER

}st350mc_operation_types_e;


i2c_dev_t ST350_ctx = {
	.port = 2,
    .config.address_width = 8,
    .config.freq = 400000,
    .config.dev_addr = ST350_I2C_ADDR,
};

UNUSED static int drv_mag_sen_st350_soft_reset(i2c_dev_t* drv)
{
    int ret = 0;
    uint8_t value = 0x1;
    ret = sensor_i2c_write(drv, ST350MC_REG_CNTRL2, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return -1;
    }
    return 0;
}


static int drv_mag_sen_st350_validate_id(i2c_dev_t* drv, uint8_t id_value)
{
    uint8_t value = 0x00;
    int ret = 0, i = 3;

    (void)ret;
    if(drv == NULL){
        return -1;
    }

	do{
		ret = sensor_i2c_read(drv, ST350_MAG_WHO_AM_I_REG, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
	}while((value != I_AM_ST350) && (i-- > 0));

	
    if (id_value != value){
        return -1;
    }
    
    return 0;
}

static int drv_mag_sen_st350_set_operation_types(i2c_dev_t* drv, st350mc_operation_types_e types)
{
	uint8_t ST350MCreg[ST350MC_OPERATION_TYPES_REG_TOTAL]  = {0x09,0x41,0x42,0x6C};
	uint8_t ST350MCSetRegData[ST350MC_OPERATION_TYPES_REG_TOTAL];
	uint8_t data, reg , i;
	int ret = 0;

	
	ST350MCSetRegData[0] = 0x02;

	ST350MCSetRegData[1] = 0x03;

	ST350MCSetRegData[2] = 0x40;

	ST350MCSetRegData[3] = 0x05;
		
	switch(types){
		case ST350MC_HIGH_PRECISION:

		ST350MCSetRegData[3] |= 0x50;
		break;
		
		case ST350MC_HIGH_SPEED:
	
		ST350MCSetRegData[3] |= 0x30;
		break;
		
		case ST350MC_LOW_POWER:

		ST350MCSetRegData[0] |= 0xA0;

		ST350MCSetRegData[3] |= 0x30;	
		break;
		
		case ST350MC_NORMAL:
		default:

		ST350MCSetRegData[3] |= 0x40;
	}
	
	for( i = 0 ; i < ST350MC_OPERATION_TYPES_REG_TOTAL ; i++){
		reg = ST350MCreg[i];
		data = ST350MCSetRegData[i];

    	ret = sensor_i2c_write(drv, reg, &data, I2C_DATA_LEN, I2C_OP_RETRIES);
    	if(unlikely(ret)){
        return ret;
    	}

#ifdef ST350MC_DEBUG

		data =	0x0;

		ret = sensor_i2c_read(&ST350_ctx, reg, &data, I2C_DATA_LEN, I2C_OP_RETRIES);
		if(unlikely(ret)){
			return -1;
		}
		
		ST350_LOG("operation types >>>>>0x%x = %x", reg, data);
#endif
	}

	return 0;
}


static int drv_mag_sen_st350_set_operation_mode(i2c_dev_t* drv, st350_operation_mode_e mode)
{

	uint8_t data, ret = 0, reg , i;
	uint8_t regBuff[2],dataBuff[2];

	switch(mode){
		case ST350MC_SINGLE_MEASURE:
		regBuff[0] = 0x20;
		dataBuff[0] = 0;
		regBuff[1] = 0x0A;
		dataBuff[1] = 1;		
		break;
		
		case ST350MC_SUSPEND:
		regBuff[0] = 0x0A;
		dataBuff[0] = 0;
		regBuff[1] = 0x20;
		dataBuff[1] = 1;
		break;
		
		case ST350MC_STANDBY:
		default:
		regBuff[0] = 0x20;
		dataBuff[0] = 0;
		regBuff[1] = 0x0A;
		dataBuff[1] = 0;
	}

	for(i = 0; i < 2; i++){
		reg = regBuff[i];
		data = dataBuff[i];

    	ret = sensor_i2c_write(drv, reg, &data, I2C_DATA_LEN, I2C_OP_RETRIES);
    	if(unlikely(ret)){
        return ret;
    	}

#ifdef ST350MC_DEBUG
		data =	0x0;

		ret = sensor_i2c_read(&ST350_ctx, reg, &data, I2C_DATA_LEN, I2C_OP_RETRIES);
		if(unlikely(ret)){
			return -1;
		}

		ST350_LOG("operation mode >>>>>0x%x = %x", reg, data);
#endif	
	}


	return 0;
}


static int drv_mag_sen_st350_set_power_mode(i2c_dev_t* drv, dev_power_mode_e mode)
{
    
    if (mode == DEV_POWER_ON){
        drv_mag_sen_st350_set_operation_mode(&ST350_ctx, ST350MC_STANDBY);

    }
    else{
        drv_mag_sen_st350_set_operation_mode(&ST350_ctx, ST350MC_SUSPEND);
    }
    

    return 0;
}

UNUSED static int drv_mag_sen_st350_set_odr(i2c_dev_t* drv, uint8_t odr)
{
    return 0;
}


UNUSED static int drv_mag_sen_st350_lowpower_mode(i2c_dev_t* drv, uint8_t lowpower_mode)
{
    int ret = 0;

    ret = drv_mag_sen_st350_set_operation_types(&ST350_ctx, ST350MC_LOW_POWER);
    if(unlikely(ret)){
        return -1;
    }
    
    return 0;
}

UNUSED static int drv_mag_sen_st350_set_range(i2c_dev_t* drv, uint32_t range)
{

    return 0;
}

static int drv_mag_sen_st350_set_default_config(i2c_dev_t* drv)
{
	int ret = 0;
	uint8_t ST350MCreg[ST350MC_REGISTER_SETTING_TOTAL]  = 		{0x0D,0x56,0x57,0x72,0x76};
	uint8_t ST350MCSetRegData[ST350MC_REGISTER_SETTING_TOTAL] = {0x00,0x1F,0x00,0x2B,0x00};

	uint8_t data, reg , i;


	reg = 0xBA;
	data = 0x0;

	ret = sensor_i2c_read(&ST350_ctx, reg, &data, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return -1;
    }
	else{
		if((data == 0x10) || (data == 0xFF)){
			ST350MCSetRegData[4] = 0x20;
		}
	}


	for( i = 0 ; i < ST350MC_REGISTER_SETTING_TOTAL ; i++ )
	{
		reg = ST350MCreg[i];
		data = ST350MCSetRegData[i];

    	ret = sensor_i2c_write(drv, reg, &data, I2C_DATA_LEN, I2C_OP_RETRIES);
    	if(unlikely(ret)){
        return ret;
    	}

#ifdef ST350MC_DEBUG
		data =	0x0;

		ret = sensor_i2c_read(&ST350_ctx, reg, &data, I2C_DATA_LEN, I2C_OP_RETRIES);
		if(unlikely(ret)){
			return -1;
		}

		ST350_LOG(">>>>>0x%x = %x", reg, data);
#endif	
	}
 
    return 0;
}

static void drv_mag_sen_st350_irq_handle(void)
{
    /* no handle so far */
}


UNUSED static int drv_mag_sen_st350_dump_reg(void)
{ 
  uint8_t rw_buffer[32] = {0};
  uint8_t reg_map[] = {
      0x0D,
      0x56, 
      0x57, 
      0x72, 
      0x76, 
      0x20, 
      0x0A,
      0x09,
      0x41,
      0x42,
      0x6C
  };

  uint8_t i = 0;
  uint16_t n = sizeof(reg_map)/sizeof(reg_map[0]);
  int ret = 0;
 
  for(i=0; i<n;i++) {
    ret = sensor_i2c_read(&ST350_ctx, reg_map[i], &rw_buffer[i],I2C_REG_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return -1;
    }

  }

  return 0;
}



static int drv_mag_sen_st350_open(void)
{
    int ret = 0;

    ret  = drv_mag_sen_st350_set_power_mode(&ST350_ctx, DEV_POWER_ON);
    if(unlikely(ret)){
        return -1;
    }

	ret = drv_mag_sen_st350_set_operation_mode(&ST350_ctx, ST350MC_SINGLE_MEASURE);
    if(unlikely(ret)){
        return -1;
    }


    return 0;

}

static int drv_mag_sen_st350_close(void)
{
    int ret = 0;
	
    ret  = drv_mag_sen_st350_set_power_mode(&ST350_ctx, DEV_POWER_OFF);
    if(unlikely(ret)){
        return -1;
    }
    return 0;
}



static int drv_mag_sen_st350_read(void* buf, size_t len)
{
  int ret = 0, data;
  size_t size;
  int16_t pnRawData[3];
  uint8_t buffer[6];
  uint8_t i = 0;
  
  mag_data_t* pdata = (mag_data_t*)buf;
  if(buf == NULL){
        return -1;
  }

  size = sizeof(mag_data_t);
  if(len < size){
        return -1;
  }


  ret = sensor_i2c_read(&ST350_ctx, ST350MC_REG_DATAX_L, buffer, 6, I2C_OP_RETRIES);
  if(unlikely(ret)){
        return -1;
  }
  
  for(i=0; i<3; i++)
  {
    pnRawData[i]=(int16_t)((((uint16_t)buffer[2*i+1]) << 8) | ((uint16_t)buffer[2*i]));
  }

  /*ST350_LOG("raw uint st350 x = %d, y = %d, z = %d", pnRawData[0], pnRawData[1], pnRawData[2]);*/

  for(i=0; i<3; i++)
  {
    pdata->data[i] =(int32_t)(pnRawData[i] * ST350_MAG_SENSITIVITY_FOR_FS_20GA * UTTOMG / 10);
  }

  
  pdata->timestamp = aos_now_ms();

  data = 1;
  ret = sensor_i2c_write(&ST350_ctx, 0xa, (uint8_t *)&data, I2C_DATA_LEN, I2C_OP_RETRIES);
  if(unlikely(ret)){
	return ret;
  }

  aos_msleep(1);

  return (int)size;
}


static int drv_mag_sen_st350_ioctl(int cmd, unsigned long arg)
{
    int ret = 0;
    
    switch(cmd){
        case SENSOR_IOCTL_ODR_SET:{

        }break;
        case SENSOR_IOCTL_RANGE_SET:{
		
        }break;
        case SENSOR_IOCTL_SET_POWER:{
            ret = drv_mag_sen_st350_set_power_mode(&ST350_ctx, arg);
            if(unlikely(ret)){
                return -1;
            }
        }break;
        case SENSOR_IOCTL_GET_INFO:{ 
            /* fill the dev info here */
            dev_sensor_info_t *info = (dev_sensor_info_t *)arg;
            info->model = "ST350";
            info->range_max = 25;
            info->range_min = 20;
            info->unit = mGauss;
        }break;
       
       default:break;
    }

    return 0;
}


int drv_mag_sen_st350_init(void){
    int ret = 0;
    sensor_obj_t sensor;
    memset(&sensor, 0, sizeof(sensor));
    /* fill the sensor obj parameters here */
    sensor.io_port    = I2C_PORT;
    sensor.tag        = TAG_DEV_MAG;
    sensor.path       = dev_mag_path;
    sensor.open       = drv_mag_sen_st350_open;
    sensor.close      = drv_mag_sen_st350_close;
    sensor.read       = drv_mag_sen_st350_read;
    sensor.write      = NULL;
    sensor.ioctl      = drv_mag_sen_st350_ioctl;
    sensor.irq_handle = drv_mag_sen_st350_irq_handle;

    ret = sensor_create_obj(&sensor);
    if(unlikely(ret)){
        return -1;
    }

    ret = drv_mag_sen_st350_validate_id(&ST350_ctx, I_AM_ST350);
    if(unlikely(ret)){
        return -1;
    }

	ret = drv_mag_sen_st350_set_power_mode(&ST350_ctx, DEV_POWER_ON);
    if(unlikely(ret)){
        return -1;
    }

    ret = drv_mag_sen_st350_set_default_config(&ST350_ctx);
    if(unlikely(ret)){
        return -1;
    }

	aos_msleep(1);

	ret = drv_mag_sen_st350_set_operation_types(&ST350_ctx, ST350MC_NORMAL);
	if(unlikely(ret)){
		return -1;
	}
	
	aos_msleep(1);

	
    /* update the phy sensor info to sensor hal */
    LOG("%s %s successfully \n", SENSOR_STR, __func__);
    return 0;
}

SENSOR_DRV_ADD(drv_mag_sen_st350_init);


