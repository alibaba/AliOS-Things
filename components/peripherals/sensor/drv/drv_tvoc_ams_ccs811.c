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


#define CCS811_I2C_ADDR1            (0x5A)          /* When ADDR is high the 7bit I2C address is 0x5B */
#define CCS811_I2C_ADDR_TRANS(n)    ((n)<<1)  
#define CCS811_I2C_ADDR             CCS811_I2C_ADDR_TRANS(CCS811_I2C_ADDR1)

/**
* @brief CCS811 API Constants
*/
#define CCS811_REG_STATUS              0x00
#define CCS811_REG_MEAS_MODE           0x01
#define CCS811_REG_ALG_RESULT_DATA     0x02
#define CCS811_REG_RAW_DATA            0x03
#define CCS811_REG_ALG_CONFIG_DATA     0x04
#define CCS811_REG_ENVIRONMENT_DATA    0x05
#define CCS811_REG_THRESHOLDS          0x10
#define CCS811_CHIP_ID_REG             0x20
#define CCS811_HW_VERSION              0x21
#define CCS811_FW_BOOT_VERSION         0x23
#define CCS811_FW_APP_VERSION          0x24
#define CCS811_ERROR_ID                0xE0
#define CCS811_RST_REG                 0xFF 

#define CCS811_APP_ERASE               0xF1
#define CCS811_APP_DATE                0xF2
#define CCS811_APP_VERIFY              0xF3
#define CCS811_APP_START               0xF4           

#define CCS811_CHIP_ID_VAL             0x81

#define CCS811_MEASUREMENT_MODE_IDLE          0x00
#define CCS811_MEASUREMENT_MODE_VOC_1S        0x10
#define CCS811_MEASUREMENT_MODE_VOC_10S       0x20
#define CCS811_MEASUREMENT_MODE_VOC_60S       0x30

#define CCS811_VOC_DATA_SIZE                  8
#define CCS811_CMD_LENGTH                     1
#define CCS811_REG_LENGTH                     1 
#define CCS811_DATA_LENGTH                    2 
#define CCS811_RESET_LENGTH                   4 


#define CCS811_DATA_READ_MIN_INTERVAL         1000       /* in millisecond */

typedef struct {
    uint16_t eCO2;
    uint16_t voc;
    uint16_t adc;
    uint16_t current;
    uint32_t resistance;
} ccs811_measurement_t;

ccs811_measurement_t g_ccs811_data = {0};

i2c_dev_t ccs811_ctx = {
    .port = 3,
    .config.address_width = 8,
    .config.freq = 100000,
    .config.dev_addr = CCS811_I2C_ADDR,
};

static int drv_voc_ams_ccs811_validate_id(i2c_dev_t* drv, uint8_t id_value)
{
    int     ret = 0;
    uint8_t value = 0;

    if(drv == NULL){
        return -1;
    }
    
    ret = sensor_i2c_read(drv, CCS811_CHIP_ID_REG, &value, CCS811_CMD_LENGTH, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }

    if (id_value != value){
        return -1;
    }
    return 0;
}


UNUSED static int  drv_voc_ams_ccs811_soft_reset(i2c_dev_t* drv)
{
    int     ret = 0;
    uint8_t data[4];
    
    data[0] = 0x11;
    data[1] = 0xE5;
    data[2] = 0x72;
    data[3] = 0x8A;
    
    ret = sensor_i2c_write(drv,CCS811_RST_REG,
                            &data[0],CCS811_RESET_LENGTH,I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }
    
    return ret;
}

static int  drv_voc_ams_ccs811_app_start(i2c_dev_t* drv)
{
    int     ret = 0;
    uint8_t value;  

    ret = sensor_i2c_read(&ccs811_ctx, CCS811_REG_STATUS, &value, CCS811_CMD_LENGTH, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }   

    aos_msleep(2);

    value = CCS811_APP_START;
    ret = hal_i2c_master_send(&ccs811_ctx, CCS811_I2C_ADDR, &value, CCS811_CMD_LENGTH, AOS_WAIT_FOREVER);
    if(unlikely(ret)){
        return ret;
    }

    aos_msleep(250);

    return 0;
}

static int drv_voc_ams_ccs811_set_power_mode(i2c_dev_t* drv, dev_power_mode_e mode)
{
    int     ret = 0;
    uint8_t value = 0x00;

    
    switch(mode){
        case DEV_POWER_OFF:
        case DEV_SLEEP:{
            value = CCS811_MEASUREMENT_MODE_IDLE;
            ret = sensor_i2c_write(&ccs811_ctx, CCS811_REG_MEAS_MODE, &value, CCS811_CMD_LENGTH, I2C_OP_RETRIES);
            if(unlikely(ret)){
                return ret;
            }        
        }break;
        case DEV_POWER_ON:{
            value = CCS811_MEASUREMENT_MODE_VOC_1S;
            ret = sensor_i2c_write(&ccs811_ctx, CCS811_REG_MEAS_MODE, &value, CCS811_CMD_LENGTH, I2C_OP_RETRIES);
            if(unlikely(ret)){
                return ret;
            }        
        }break;

        default:break;
    }
    return 0;
}

static int drv_voc_ams_ccs811_set_default_config(i2c_dev_t* drv)
{
    int     ret = 0;
    uint8_t value = 0x00;  

    ret = sensor_i2c_read(&ccs811_ctx, CCS811_REG_STATUS, &value, CCS811_CMD_LENGTH, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }

    ret = drv_voc_ams_ccs811_set_power_mode(&ccs811_ctx, DEV_SLEEP);
    if(unlikely(ret)){
        return ret;
    }

    return 0;

}

UNUSED static int drv_voc_ams_ccs811_set_work_mode(i2c_dev_t* drv,uint8_t mode)
{
    uint8_t ret = 0;
    uint8_t value = 0;

    ret = sensor_i2c_read(&ccs811_ctx, CCS811_REG_STATUS, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }

    value = mode;

    ret = sensor_i2c_write(&ccs811_ctx, CCS811_REG_MEAS_MODE, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
    return ret;
    }   

    return 0;
   }


static void drv_voc_ams_ccs811_irq_handle(void)
{
    /* no handle so far */
}

static int drv_voc_ams_ccs811_open(void)
{
    int ret = 0;
    uint8_t value = 0x00;  

    ret = sensor_i2c_read(&ccs811_ctx, CCS811_REG_STATUS, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return -1;
    }

    ret = drv_voc_ams_ccs811_set_power_mode(&ccs811_ctx, DEV_POWER_ON);
    if(unlikely(ret)){
        return -1;
    }

    LOG("%s %s successfully \n", SENSOR_STR, __func__);
    return 0;
}

static int drv_voc_ams_ccs811_close(void)
{
    int ret = 0;
    uint8_t value = 0x00;  

    ret = sensor_i2c_read(&ccs811_ctx, CCS811_REG_STATUS, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return 0;
    }

    ret = drv_voc_ams_ccs811_set_power_mode(&ccs811_ctx, DEV_SLEEP);
    if(unlikely(ret)){
        return 0;
    }

    LOG("%s %s successfully \n", SENSOR_STR, __func__);
    return 0;
}

static int drv_voc_ams_ccs811_read(void *buf, size_t len)
{
    int ret = 0;
    size_t size = 0;
    uint8_t buffer[8];
    uint16_t data[2];
    integer_data_t* pdata = (integer_data_t*)buf;

    if(buf == NULL){
        return -1;
    }

    size = sizeof(integer_data_t);
    if(len < size){
        return -1;
    }   

    ret  = sensor_i2c_read(&ccs811_ctx, CCS811_REG_ALG_RESULT_DATA, buffer, 8, I2C_OP_RETRIES);

    data[0] = (uint16_t)(buffer[0] << 8) | (uint16_t)(buffer[1] << 0);
    data[1] = (uint16_t)(buffer[2] << 8) | (uint16_t)(buffer[3] << 0);

     if(unlikely(ret)){
        return -1;
    }

    if (data[1] > 32768)
    data[1] = 32768;

    pdata->data = data[1];
    pdata->timestamp = aos_now_ms();

    return (int)size;
}

static int drv_voc_ams_ccs811_write(const void *buf, size_t len)
{
    return 0;
}

static int drv_voc_ams_ccs811_ioctl(int cmd, unsigned long arg)
{
    int ret = 0;
    
    switch(cmd){
        case SENSOR_IOCTL_SET_POWER:{
            ret = drv_voc_ams_ccs811_set_power_mode(&ccs811_ctx, arg);
            if(unlikely(ret)){
                return -1;
            }
        }break;
        case SENSOR_IOCTL_GET_INFO:{ 
            /* fill the dev info here */
            dev_sensor_info_t *info = (dev_sensor_info_t *)arg;
            info->model = "CCS811";
            info->range_max = 32768;
            info->range_min = 0;
            //info->unit = ppb;

        }break;
       
       default:break;
    }
    LOG("%s %s successfully \n", SENSOR_STR, __func__);
    return 0;
}

int drv_voc_ams_ccs811_init(void)
{
    int ret = 0;
    sensor_obj_t sensor;

    memset(&sensor, 0, sizeof(sensor));
    /* fill the sensor obj parameters here */
    sensor.tag = TAG_DEV_TVOC;
    sensor.path = dev_tvoc_path;
    sensor.io_port = I2C_PORT;
    sensor.open = drv_voc_ams_ccs811_open;
    sensor.close = drv_voc_ams_ccs811_close;
    sensor.read = drv_voc_ams_ccs811_read;
    sensor.write = drv_voc_ams_ccs811_write;
    sensor.ioctl = drv_voc_ams_ccs811_ioctl;
    sensor.irq_handle = drv_voc_ams_ccs811_irq_handle;


    ret = sensor_create_obj(&sensor);
    if(unlikely(ret)){
        return -1;
    }

    ret = drv_voc_ams_ccs811_validate_id(&ccs811_ctx, CCS811_CHIP_ID_VAL);
    if(unlikely(ret)){
        return -1;
    }

/*
    ret = drv_voc_ams_ccs811_soft_reset(&ccs811_ctx);
    if(unlikely(ret)){
        return -1;
    }
*/ 

    ret = drv_voc_ams_ccs811_app_start(&ccs811_ctx);
    if(unlikely(ret)){
        return -1;
    }

    ret = drv_voc_ams_ccs811_set_default_config(&ccs811_ctx);
    if(unlikely(ret)){
        return -1;
    }

    LOG("%s %s successfully \n", SENSOR_STR, __func__);
    return 0;
}

SENSOR_DRV_ADD(drv_voc_ams_ccs811_init);