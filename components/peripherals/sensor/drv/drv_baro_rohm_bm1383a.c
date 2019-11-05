/*
* Copyright (C) 2015-2017 Alibaba Group Holding Limited
*
*	Edit wanjiang-yan 2018-6-24
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "aos/kernel.h"
#include "network/hal/base.h"
#include "sensor_drv_api.h"
#include "sensor_hal.h"

//1.8V offset=0,2.8v offset= -0.6
//#define VALUE_OFFSET						//modify by laoyan
#ifdef VALUE_OFFSET
#define PRESSURE_VALUES_OFFSET		0.6		//modify by laoyan
#endif


//For Get_Attrib Function
#define PRESS_SENSOR_POWER_VAL           (650)
#define PRESS_SENSOR_LOW_POWER_VAL       (30)
#define PRESS_SENSOR_ACTIVE_TIME         (1300)
#define PRESS_SENSOR_DATA_TIME           (147000)
#define PRESS_SENSOR_RANGE_MIN           (0)
#define PRESS_SENSOR_RANGE_MAX           (1)
#define PRESS_SENSOR_ODR_SIZE            (7)
#define PRESS_SENSOR_RES_DECIMAL         (11)
#define PRESS_SENSOR_RES_INTEGER         (11)
#define PRESS_SENSOR_DATA_BYTE           (3)

#define PRESS_SENSOR_INIT_ODR            (6)


/******************************************************************************
   define name for IC
 *****************************************************************************/
/* Address */


/* Compear ID */
#define BM1383A_ID1_VALUE               (0xE0)
#define BM1383A_ID2_VALUE               (0x32)

/* Power Down */
#define BM1383A_POWER_OFF              (0x00)
#define BM1383A_POWER_ON               (0x01)

/* Reset */
#define BM1383A_SW_RESET_VALUE         (1 << 7)
#define BM1383A_INT_RESET_VALUE        (1 << 6)

/* Sleep value */
#define BM1383A_SLEEP_RELEASE          (0x00)
#define BM1383A_SLEEP_ACTIVE           (0x01)


/* AVE_NUM */

#define BM1383A_AVE_NUM_VALUE_1_TIMES   (0x0 << 5) /*000: single */
#define BM1383A_AVE_NUM_VALUE_30_TIMES   (0x3 << 5) /*001: average of 30 times */
#define BM1383A_AVE_NUM_VALUE_60_TIMES  (0x4 << 5) /*001: average of 60 times*/    //50ms
#define BM1383A_AVE_NUM_VALUE_120_TIMES  (0x5 << 5) /*101: average of 120 times*/    //100ms
#define BM1383A_AVE_NUM_VALUE_240_TIMES (0x6 << 5) /*110: average of 240 times*/   //200ms



#define BM1383A_MODE_VALUE_STANDBY      (0x0)    /* 00 stand by        */
#define BM1383A_MODE_VALUE_ONE_SHOT     (0x1)    /* 01 One shot        */
#define BM1383A_MODE_VALUE_CONTINOUS    (0x2)    /* 10 Continuous  */
#define BM1383A_MODE_VALUE_PROHIBITION  (0x3)    /* 11 Prohibition */

#define BM1383A_INT_ENABLE_DREG          (0x1>>4)  //DRDY pin Enable
#define BM1383A_INT_DISABLE_DREG         (0x0>>4)  //DRDY pin Disable

#define BM1383A_MODE_RECV				  (0x2>>2)  //Refer to Operation mode transition



#define BM1383A_INT_CLS_VALUE             (0x1 << 6) /* Clear interrupt */
                                         



/* ROHM SENSOR REGISTER MAP */
#define BM1383A_BIT(x)               ((uint8_t)x)
#define BM1383A_REG_ID1                     (0x0F)
#define BM1383A_REG_ID2                     (0x10)

#define BM1383A_REG_POWER_DOWN             (0x12)
#define BM1383A_REG_RESET_CONTROL          (0x13)
#define BM1383A_REG_MODE_CONTROL           (0x14)
#define BM1383A_REG_STATUS                 (0x19)

#define BM1383A_REG_PRESSURE_MSB_H           (0x1A)
#define BM1383A_REG_PRESSURE_LSB_M           (0x1B)
#define BM1383A_REG_PRESSURE_LSB_L           (0x1C)
#define BM1383A_REG_TEMPERATURE_MSB           (0x1D)
#define BM1383A_REG_TEMPERATURE_LSB           (0x1E)


#define BM1383A_I2C_ADDR            (0x5D)
#define BM1383A_I2C_ADDR_TRANS(n)    ((n)<<1)  
#define BM1383A_I2C_ADDR2             BM1383A_I2C_ADDR_TRANS(BM1383A_I2C_ADDR)
#define BM1383A_ODR_MASK             (uint8_t)0x1c

typedef enum {
  BM1383A_ODR_ONE_SHOT        = (uint8_t)0x01,         /*!< Output Data Rate: one shot */
  BM1383A_ODR_1HZ             = (uint8_t)0xc2,         /*!< Output Data Rate: 1Hz */
  BM1383A_ODR_5HZ            = (uint8_t)0xa2,         /*!< Output Data Rate: 4Hz */
  BM1383A_ODR_10HZ            = (uint8_t)0x82,         /*!< Output Data Rate: 8Hz */
  BM1383A_ODR_20HZ            = (uint8_t)0x62,         /*!< Output Data Rate: 15Hz */
} bm1383a_odr_e;



typedef enum {
  BM1383A_BDU_CONTINUOUS_UPDATE     =  (uint8_t)0x00,  /*!< Data updated continuously */
  BM1383A_BDU_NO_UPDATE             =  (uint8_t)0x02   /*!< Data updated after a read operation */
} bm1383a_bdu_e;

i2c_dev_t bm1383a_ctx = {
    .port = 3,
    .config.address_width = 8,
    .config.freq = 400000,
    .config.dev_addr = BM1383A_I2C_ADDR2,
};

static bm1383a_odr_e drv_baro_rohm_bm1383a_hz2odr(int hz)
{
    if(hz > 10)
        return BM1383A_ODR_20HZ;
    else if(hz > 5)
        return BM1383A_ODR_10HZ;
    else if(hz > 1)
        return BM1383A_ODR_5HZ;
    else
        return BM1383A_ODR_1HZ;
}

//��֤оƬ�������Ĵ����Ƿ���Ĭ�ϵļĴ�������
static int drv_baro_rohm_bm1383a_validate_id(i2c_dev_t* drv)
{
    uint8_t value = 0x00;
    int ret = 0;

    if(drv == NULL){
        return -1;
    }
    
    ret = sensor_i2c_read(drv, BM1383A_REG_ID1, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }
    
    if (BM1383A_ID1_VALUE != value){
        return -1;
    }
	
	ret = sensor_i2c_read(drv, BM1383A_REG_ID2, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
	if(unlikely(ret)){
		return ret;
	}
	
	if (BM1383A_ID2_VALUE != value){
		return -1;
	}

	
    return 0;
}

static int drv_baro_rohm_bm1383a_set_power_mode(i2c_dev_t* drv, dev_power_mode_e mode)
{
    uint8_t value = 0x00;
    int ret = 0;

	//ret = sensor_i2c_read(drv, BM1383A_RES_CONF_REG, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    //if(unlikely(ret)){
    //    return ret;
    //}

    switch(mode){
        case DEV_POWER_ON:{
            value = BM1383A_POWER_ON;
            ret = sensor_i2c_write(drv, BM1383A_REG_POWER_DOWN, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
            if(unlikely(ret)){
                return ret;
            }
			//It must wait 2ms (i2c)
			aos_msleep(2);
			value = BM1383A_SLEEP_ACTIVE;
			ret = sensor_i2c_write(drv, BM1383A_REG_RESET_CONTROL, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
			if(unlikely(ret)){
				return ret;
			}
			value = BM1383A_AVE_NUM_VALUE_60_TIMES|BM1383A_MODE_VALUE_CONTINOUS;
			ret = sensor_i2c_write(drv, BM1383A_REG_MODE_CONTROL, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
			if(unlikely(ret)){
			    return ret;
			}
			aos_msleep(100);

			
        }break;
        
        case DEV_POWER_OFF:{
			//sleep status
			value = BM1383A_SLEEP_RELEASE;
            ret = sensor_i2c_write(drv, BM1383A_REG_RESET_CONTROL, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
            if(unlikely(ret)){
                return ret;
            }
			//power off
            value = BM1383A_POWER_OFF;
            ret = sensor_i2c_write(drv, BM1383A_REG_POWER_DOWN, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
            if(unlikely(ret)){
                return ret;
            }
        }break;
        
       default:break;
    }
    return 0;
}

static int drv_baro_rohm_bm1383a_set_odr(i2c_dev_t* drv, bm1383a_odr_e odr)
{
    uint8_t value = 0x00;
    int ret = 0;
    
    ret = sensor_i2c_read(drv, BM1383A_REG_MODE_CONTROL, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }
	value &= ~BM1383A_ODR_MASK;
    value |= (uint8_t)odr;
    
    ret = sensor_i2c_write(drv, BM1383A_REG_MODE_CONTROL, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }
    return 0;
}

//
static int drv_baro_rohm_bm1383a_set_bdu(i2c_dev_t* drv, bm1383a_bdu_e bdu)
{
    /*
    ret = sensor_i2c_read(drv, BM1383A_REG_MODE_CONTROL, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }
    value &= ~BM1383A_BDU_MASK;
    value |= (uint8_t)bdu;
    
    ret = sensor_i2c_write(drv, BM1383A_REG_MODE_CONTROL, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }
    */
    return 0;
}

static int drv_baro_rohm_bm1383a_set_default_config(i2c_dev_t* drv)
{
    int ret = 0;
    ret = drv_baro_rohm_bm1383a_set_power_mode(drv, DEV_POWER_OFF);
    if(unlikely(ret)){
        return ret;
    }
    ret = drv_baro_rohm_bm1383a_set_odr(drv, BM1383A_ODR_20HZ);
    if(unlikely(ret)){
        return ret;
    }
    ret = drv_baro_rohm_bm1383a_set_bdu(drv, BM1383A_BDU_NO_UPDATE);
    if(unlikely(ret)){
        return ret;
    }
    /* you also can set the low-pass filter and cut off config here */
    return 0;
}


static void drv_baro_rohm_bm1383a_irq_handle(void)
{
    /* no handle so far */
}

static int drv_baro_rohm_bm1383a_open(void)
{
    int ret = 0;
    ret  = drv_baro_rohm_bm1383a_set_power_mode(&bm1383a_ctx, DEV_POWER_ON);
    if(unlikely(ret)){
        return -1;
    }
    LOG("%s %s successfully \n", SENSOR_STR, __func__);
    return 0;

}

static int drv_baro_rohm_bm1383a_close(void)
{
    int ret = 0;
    ret  = drv_baro_rohm_bm1383a_set_power_mode(&bm1383a_ctx, DEV_POWER_OFF);
    if(unlikely(ret)){
        return -1;
    }
    LOG("%s %s successfully \n", SENSOR_STR, __func__);
    return 0;
}


static int drv_baro_rohm_bm1383a_read(void *buf, size_t len)
{
    int ret = 0;
    size_t size;
    uint8_t data[3];
    barometer_data_t* pdata = (barometer_data_t*)buf;
    if(buf == NULL){
        return -1;
    }

    size = sizeof(barometer_data_t);
    if(len < size){
        return -1;
    }
    
    ret  = sensor_i2c_read(&bm1383a_ctx, BM1383A_REG_PRESSURE_LSB_L,  &data[0], I2C_DATA_LEN, I2C_OP_RETRIES);
    ret |= sensor_i2c_read(&bm1383a_ctx, BM1383A_REG_PRESSURE_LSB_M,   &data[1], I2C_DATA_LEN, I2C_OP_RETRIES);
    ret |= sensor_i2c_read(&bm1383a_ctx, BM1383A_REG_PRESSURE_MSB_H,   &data[2], I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return -1;
    }

   /* hatch the baro data here*/
    for(int i=0; i<3; i++){
      pdata->p |= (((uint32_t)data[i]) << (8*i));
    }
	// left 2 bit
    //pdata->p = pdata->p >> 2;
	
    /* convert the 2's complement 24 bit to 2's complement 32 bit */
    if((pdata->p & 0x00800000) != 0){
        pdata->p |= 0xFF000000;
    }
    pdata->p = (pdata->p * 25)/2048;
    //pdata->p = pdata->p/100;
#ifdef VALUE_OFFSET
	pdata->p -= PRESSURE_VALUES_OFFSET;
#endif

	
    pdata->timestamp = aos_now_ms();
    
    return (int)size;
}

static int drv_baro_rohm_bm1383a_write(const void *buf, size_t len)
{
    return 0;
}

static int drv_baro_rohm_bm1383a_ioctl(int cmd, unsigned long arg)
{
    int ret = 0;
    
    switch(cmd){
        case SENSOR_IOCTL_ODR_SET:{
            bm1383a_odr_e odr = drv_baro_rohm_bm1383a_hz2odr(arg);
            ret = drv_baro_rohm_bm1383a_set_odr(&bm1383a_ctx, odr);
            if(unlikely(ret)){
                return -1;
            }
        }break;
        case SENSOR_IOCTL_SET_POWER:{
            ret = drv_baro_rohm_bm1383a_set_power_mode(&bm1383a_ctx, arg);
            if(unlikely(ret)){
                return -1;
            }
        }break;
        case SENSOR_IOCTL_GET_INFO:{ 
            /* fill the dev info here */
            dev_sensor_info_t *info = (dev_sensor_info_t *)arg;
            info->model = "BM1383A";
            info->range_max = 110000;
            info->range_min = 30000;
            info->unit = pa;

        }break;
       
       default:break;
    }

    LOG("%s %s successfully \n", SENSOR_STR, __func__);
    return 0;
}

int drv_baro_rohm_bm1383a_init(void){
    int ret = 0;
    sensor_obj_t sensor;
    memset(&sensor, 0, sizeof(sensor));

    /* fill the sensor obj parameters here */
    sensor.tag = TAG_DEV_BARO;
    sensor.path = dev_baro_path;
    sensor.io_port = I2C_PORT;
    sensor.open = drv_baro_rohm_bm1383a_open;
    sensor.close = drv_baro_rohm_bm1383a_close;
    sensor.read = drv_baro_rohm_bm1383a_read;
    sensor.write = drv_baro_rohm_bm1383a_write;
    sensor.ioctl = drv_baro_rohm_bm1383a_ioctl;
    sensor.irq_handle = drv_baro_rohm_bm1383a_irq_handle;


    ret = sensor_create_obj(&sensor);
    if(unlikely(ret)){
        return -1;
    }

    ret = drv_baro_rohm_bm1383a_validate_id(&bm1383a_ctx);
    if(unlikely(ret)){
        return -1;
    }
    /* set the default config for the sensor here */
    ret = drv_baro_rohm_bm1383a_set_default_config(&bm1383a_ctx);
    if(unlikely(ret)){
        return -1;
    }

    LOG("%s %s successfully \n", SENSOR_STR, __func__);
    return 0;
}

SENSOR_DRV_ADD(drv_baro_rohm_bm1383a_init);

