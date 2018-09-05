/*
* Copyright (C) 2015-2017 Alibaba Group Holding Limited
*
*
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <aos/aos.h>
#include <vfs_conf.h>
#include <vfs_err.h>
#include <vfs_register.h>
#include <hal/base.h>
#include "common.h"
#include "hal/sensor.h"


#define ADPD1080_I2C_ADDR1               (0x64)
#define ADPD1080_I2C_ADDR_TRANS(n)       ((n)<<1)  
#define ADPD1080_I2C_ADDR                ADPD1080_I2C_ADDR_TRANS(ADPD1080_I2C_ADDR1)

#define ADPD1080_STATUS                  0x00
#define ADPD1080_INT_MASK                0x01
#define ADPD1080_FIFOTHRESH           	 0x06
#define ADPD1080_DEVID           		 0x08
#define ADPD1080_SW_RESET				 0x0F
#define ADPD1080_MODE					 0x10
#define ADPD1080_SLOT_EN           	  	 0x11
#define ADPD1080_FSAMPLE           	  	 0x12
#define ADPD1080_PDLED_SEL           	 0x14
#define ADPD1080_NUM_AVG           	  	 0x15
#define ADPD1080_SLOTA_CH1_OFFSET        0x18
#define ADPD1080_SLOTA_CH2_OFFSET        0x19
#define ADPD1080_SLOTA_CH3_OFFSET        0x1A
#define ADPD1080_SLOTA_CH4_OFFSET        0x1B
#define ADPD1080_SLOTB_CH1_OFFSET        0x1E
#define ADPD1080_SLOTB_CH2_OFFSET        0x1F
#define ADPD1080_SLOTB_CH3_OFFSET        0x20
#define ADPD1080_SLOTB_CH4_OFFSET        0x21
#define ADPD1080_LED3_COARSE             0x22
#define ADPD1080_LED1_COARSE             0x23
#define ADPD1080_LED2_COARSE             0x24
#define ADPD1080_LED_FINE                0x25
#define ADPD1080_SLOTA_LED_PULSE         0x30
#define ADPD1080_SLOTA_NUMPULSES         0x31
#define ADPD1080_LED_DISABLE	         0x34
#define ADPD1080_SLOTB_LED_PULSE         0x35
#define ADPD1080_SLOTB_NUMPULSES         0x36
#define ADPD1080_SLOTA_AFE_WINDOW        0x39
#define ADPD1080_SLOTB_AFE_WINDOW        0x3B
#define ADPD1080_AFE_POWER_CFG1          0x3C
#define ADPD1080_SLOTB_FLOAT_LED         0x3F
#define ADPD1080_SLOTA_TIA_CFG           0x42
#define ADPD1080_SLOTA_AFE_CFG           0x43
#define ADPD1080_SLOTB_TIA_CFG           0x44
#define ADPD1080_SLOTB_AFE_CFG           0x45
#define ADPD1080_SAMPLE_CLOCK			 0x4B
#define ADPD1080_CLK32_ADJUST			 0x4D
#define ADPD1080_XXXX         			 0x4E
#define ADPD1080_AFE_POWER_CFG2          0x54
#define ADPD1080_MATH 			         0x58
#define ADPD1080_FLT_CONGIG_B	         0x59
#define ADPD1080_FLT_LED_FIRE	         0x5A
#define ADPD1080_FIFO_ACCESS	         0x60

#define ADPD1080_MEASURE_MODE            0x02
#define ADPD1080_STAND_BY_MODE			 0x00
#define ADPD1080_DEVICE_ID_VALUE         0x160A
#define ADPD1080_SOFT_RESET_VALUE        0x01
#define ADPD1080_DEFAULT_ODR_100HZ       10

i2c_dev_t adpd1080_ctx = {
    .port = 3,
    .config.address_width = 8,
    .config.freq = 400000,
    .config.dev_addr = ADPD1080_I2C_ADDR,
};

static int drv_ecg_adi_adpd1080_validate_id(i2c_dev_t* drv, uint16_t id_value)
{
    uint16_t value;
    int ret = 0;

    if(drv == NULL){
        return -1;
    }
    
    ret = sensor_i2c_read(drv, ADPD1080_DEVID, &value, I2C_DATA_LEN*2, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return -1;
    }

    if (id_value != value){
        return -1;
    }
    
    return 0;
}

static int drv_ecg_adi_adpd1080_soft_reset(i2c_dev_t* drv)
{
    int ret = 0;
    uint8_t value[2] = {0x00, ADPD1080_SOFT_RESET_VALUE};
	
    ret = sensor_i2c_write(drv, ADPD1080_SW_RESET, value, I2C_DATA_LEN*2, I2C_OP_RETRIES);
	if(unlikely(ret)){
        return ret;
    }	
    return 0;
}

static int drv_ecg_adi_adpd1080_set_power_mode(i2c_dev_t* drv, dev_power_mode_e mode)
{
    uint8_t value[2];
    int ret = 0;
    
    ret = sensor_i2c_read(drv, ADPD1080_MODE, value, I2C_DATA_LEN*2, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }

    switch(mode){
        case DEV_POWER_ON:{
            value[0]= 0x00;
			value[1]= ADPD1080_MEASURE_MODE;
            ret = sensor_i2c_write(drv, ADPD1080_MODE, value, I2C_DATA_LEN*2, I2C_OP_RETRIES);
            if(unlikely(ret)){
                return ret;
            }
        }break;
        
        case DEV_POWER_OFF:{
            value[0] = 0x00;
			value[1] = ADPD1080_STAND_BY_MODE;
            ret = sensor_i2c_write(drv, ADPD1080_MODE, value, I2C_DATA_LEN*2, I2C_OP_RETRIES);
            if(unlikely(ret)){
                return ret;
            }
        }break;
		
       default:break;
    }
    return 0;
}

static int drv_ecg_adi_adpd1080_set_odr(i2c_dev_t* drv, uint16_t odr)
{
    int ret = 0;
    uint8_t value[2];
	uint16_t odr_val = 2000 / odr;

	value[0] = (uint8_t) ((odr_val & 0xFF00)>>8);
	value[1] = (uint8_t) (odr_val & 0x00FF);

	ret = sensor_i2c_write(drv, ADPD1080_FSAMPLE, value, I2C_DATA_LEN*2, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }
	
	value[0] = 0x09, value[1] = 0x00;
	ret = sensor_i2c_write(drv, ADPD1080_FIFOTHRESH, value, I2C_DATA_LEN*2, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }
	
	value[0] = 0x02, value[1] = 0x20;
    ret = sensor_i2c_write(drv, ADPD1080_NUM_AVG, value, I2C_DATA_LEN*2, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }
	
    return 0;    
}

static int drv_ecg_adi_adpd1080_set_offset(i2c_dev_t* drv)
{
	int ret = 0;
    uint8_t value[2];
	
	value[0] = 0x30, value[1] = 0x60;
    ret = sensor_i2c_write(drv, ADPD1080_SLOT_EN, value, I2C_DATA_LEN*2, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }
	
	value[0] = 0x04, value[1] = 0x45;
    ret = sensor_i2c_write(drv, ADPD1080_PDLED_SEL, value, I2C_DATA_LEN*2, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }
	
	//SLOT_A_OFFSET
	value[0] = 0x1F, value[1] = 0x00;
    ret = sensor_i2c_write(drv, ADPD1080_SLOTA_CH1_OFFSET, value, I2C_DATA_LEN*2, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }
	value[0] = 0x3F, value[1] = 0xFF;
    ret = sensor_i2c_write(drv, ADPD1080_SLOTA_CH2_OFFSET, value, I2C_DATA_LEN*2, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }
	value[0] = 0x3F, value[1] = 0xFF;
    ret = sensor_i2c_write(drv, ADPD1080_SLOTA_CH3_OFFSET, value, I2C_DATA_LEN*2, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }
	value[0] = 0x3F, value[1] = 0xFF;
    ret = sensor_i2c_write(drv, ADPD1080_SLOTA_CH4_OFFSET, value, I2C_DATA_LEN*2, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }
	//SLOT_B_OFFSET
	value[0] = 0x1F, value[1] = 0x00;
    ret = sensor_i2c_write(drv, ADPD1080_SLOTB_CH1_OFFSET, value, I2C_DATA_LEN*2, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }
	value[0] = 0x3F, value[1] = 0xFF;
    ret = sensor_i2c_write(drv, ADPD1080_SLOTB_CH2_OFFSET, value, I2C_DATA_LEN*2, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }
	value[0] = 0x3F, value[1] = 0xFF;
    ret = sensor_i2c_write(drv, ADPD1080_SLOTB_CH3_OFFSET, value, I2C_DATA_LEN*2, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }
	value[0] = 0x3F, value[1] = 0xFF;
    ret = sensor_i2c_write(drv, ADPD1080_SLOTB_CH4_OFFSET, value, I2C_DATA_LEN*2, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }
		
    return 0;
}

static int drv_ecg_adi_adpd1080_set_led_coarse(i2c_dev_t* drv)
{
	int ret = 0;
    uint8_t value[2];

	value[0] = 0x10, value[1] = 0x33;
    ret = sensor_i2c_write(drv, ADPD1080_LED3_COARSE, value, I2C_DATA_LEN*2, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }
	
	value[0] = 0x10, value[1] = 0x3E;
    ret = sensor_i2c_write(drv, ADPD1080_LED1_COARSE, value, I2C_DATA_LEN*2, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }
	
	value[0] = 0x10, value[1] = 0x30;
    ret = sensor_i2c_write(drv, ADPD1080_LED2_COARSE, value, I2C_DATA_LEN*2, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }
	
	value[0] = 0x63, value[1] = 0x0F;
    ret = sensor_i2c_write(drv, ADPD1080_LED_FINE, value, I2C_DATA_LEN*2, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }
	
	return 0;
}

static int drv_ecg_adi_adpd1080_set_led(i2c_dev_t* drv)
{
	int ret = 0;
    uint8_t value[2];

	value[0] = 0x02, value[1] = 0x20;
    ret = sensor_i2c_write(drv, ADPD1080_SLOTA_LED_PULSE, value, I2C_DATA_LEN*2, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }
	
	value[0] = 0x02, value[1] = 0x0F;
    ret = sensor_i2c_write(drv, ADPD1080_SLOTA_NUMPULSES, value, I2C_DATA_LEN*2, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }
	
	value[0] = 0x01, value[1] = 0x00;
    ret = sensor_i2c_write(drv, ADPD1080_LED_DISABLE, value, I2C_DATA_LEN*2, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }
	
	value[0] = 0x02, value[1] = 0x20;
    ret = sensor_i2c_write(drv, ADPD1080_SLOTB_LED_PULSE, value, I2C_DATA_LEN*2, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }
	
	value[0] = 0x02, value[1] = 0x0F;
    ret = sensor_i2c_write(drv, ADPD1080_SLOTB_NUMPULSES, value, I2C_DATA_LEN*2, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }
	
	return 0;
}

static int drv_ecg_adi_adpd1080_set_afe_tia(i2c_dev_t* drv)
{
	int ret = 0;
    uint8_t value[2];

	value[0] = 0x1A, value[1] = 0xF8;
    ret = sensor_i2c_write(drv, ADPD1080_SLOTA_AFE_WINDOW, value, I2C_DATA_LEN*2, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }
	
	value[0] = 0x1A, value[1] = 0xF8;
    ret = sensor_i2c_write(drv, ADPD1080_SLOTB_AFE_WINDOW, value, I2C_DATA_LEN*2, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }
	
	value[0] = 0x31, value[1] = 0xC6;
    ret = sensor_i2c_write(drv, ADPD1080_AFE_POWER_CFG1, value, I2C_DATA_LEN*2, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }
	
	value[0] = 0x1C, value[1] = 0x35;
    ret = sensor_i2c_write(drv, ADPD1080_SLOTA_TIA_CFG, value, I2C_DATA_LEN*2, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }
	
	value[0] = 0xAD, value[1] = 0xA5;
    ret = sensor_i2c_write(drv, ADPD1080_SLOTA_AFE_CFG, value, I2C_DATA_LEN*2, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }
	
	value[0] = 0x1C, value[1] = 0x34;
    ret = sensor_i2c_write(drv, ADPD1080_SLOTB_TIA_CFG, value, I2C_DATA_LEN*2, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }
	
	value[0] = 0xAD, value[1] = 0xA5;
    ret = sensor_i2c_write(drv, ADPD1080_SLOTB_AFE_CFG, value, I2C_DATA_LEN*2, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }
	
	value[0] = 0x70, value[1] = 0x40;
    ret = sensor_i2c_write(drv, ADPD1080_XXXX, value, I2C_DATA_LEN*2, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }
	
	value[0] = 0x0A, value[1] = 0xA0;
    ret = sensor_i2c_write(drv, ADPD1080_AFE_POWER_CFG2, value, I2C_DATA_LEN*2, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }
	return 0;
}

static int drv_ecg_adi_adpd1080_set_flt(i2c_dev_t* drv)
{
	int ret = 0;
    uint8_t value[2];

	value[0] = 0x03, value[1] = 0x20;
    ret = sensor_i2c_write(drv, ADPD1080_SLOTB_FLOAT_LED, value, I2C_DATA_LEN*2, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }
	
	value[0] = 0x00, value[1] = 0x00;
    ret = sensor_i2c_write(drv, ADPD1080_MATH, value, I2C_DATA_LEN*2, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }
	
	value[0] = 0x08, value[1] = 0x08;
    ret = sensor_i2c_write(drv, ADPD1080_FLT_CONGIG_B, value, I2C_DATA_LEN*2, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }
	
	value[0] = 0x00, value[1] = 0x10;
    ret = sensor_i2c_write(drv, ADPD1080_FLT_LED_FIRE, value, I2C_DATA_LEN*2, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }
	
	return 0;
}

static int drv_ecg_adi_adpd1080_set_program(i2c_dev_t* drv)
{
	uint8_t ret;
	uint8_t value[2];
	
	value[0] = 0x00, value[1] = 0x01;
    ret = sensor_i2c_write(drv, ADPD1080_MODE, value, I2C_DATA_LEN*2, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }
	
	value[0] = 0x80, value[1] = 0xFF;
	ret = sensor_i2c_write(drv, ADPD1080_STATUS, value, I2C_DATA_LEN*2, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }
	
	value[0] = 0x00, value[1] = 0xFF;
	ret = sensor_i2c_write(drv, ADPD1080_INT_MASK, value, I2C_DATA_LEN*2, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }
	
	value[0] = 0x26, value[1] = 0x9A;
	ret = sensor_i2c_write(drv, ADPD1080_SAMPLE_CLOCK, value, I2C_DATA_LEN*2, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }
	
	value[0] = 0x00, value[1] = 0x90;
	ret = sensor_i2c_write(drv, ADPD1080_CLK32_ADJUST, value, I2C_DATA_LEN*2, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }
	
	return 0;
}

static void drv_ecg_adi_adpd1080_irq_handle(void)
{
    /* no handle so far */
}

static int drv_ecg_adi_adpd1080_open(void)
{
    int ret = 0;
    ret  = drv_ecg_adi_adpd1080_set_power_mode(&adpd1080_ctx, DEV_POWER_ON);
    if(unlikely(ret)){
        return -1;
    }
    return 0;

}

static int drv_ecg_adi_adpd1080_close(void)
{
    int ret = 0;
    ret  = drv_ecg_adi_adpd1080_set_power_mode(&adpd1080_ctx, DEV_POWER_OFF);
    if(unlikely(ret)){
        return -1;
    }
    return 0;
}

static int drv_ecg_adi_adpd1080_read(void *buf, size_t len)
{
	
    int ret = 0;
	uint8_t value[2];
	uint8_t fifo_size = 10, frm_cnt;
    size_t size;
	ecg_data_t *ecg = (ecg_data_t *)buf; 
	
	if(buf == NULL){
        return -1;
    }
    size = sizeof(ecg_data_t);
    if(len < size){
        return -1;
    }
	
	ret = sensor_i2c_read(&adpd1080_ctx, ADPD1080_STATUS, value, I2C_DATA_LEN*2, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return -1;
    }
	
	ret = sensor_i2c_read(&adpd1080_ctx, ADPD1080_FIFO_ACCESS, value, I2C_DATA_LEN*2, I2C_OP_RETRIES);
	if(unlikely(ret)){
			return -1;
	}
		
	ecg->raw_data = (uint16_t)(value[0]<<8 | value[1]);
    ecg->timestamp = aos_now_ms();

    return (int)size;
}

static int drv_ecg_adi_adpd1080_ioctl(int cmd, unsigned long arg)
{
    int ret = 0;
    dev_sensor_info_t *info = (dev_sensor_info_t *)arg;
    switch(cmd){
        case SENSOR_IOCTL_ODR_SET:{
            ret = drv_ecg_adi_adpd1080_set_odr(&adpd1080_ctx, arg);
            if(unlikely(ret)){
                return -1;
            }
        }break;
        case SENSOR_IOCTL_SET_POWER:{
            ret = drv_ecg_adi_adpd1080_set_power_mode(&adpd1080_ctx, arg);
            if(unlikely(ret)){
                return -1;
            }
        }break;
        case SENSOR_IOCTL_GET_INFO:{ 
            /* fill the dev info here */
            info->model = "ADPD1080";
            info->range_max = 150;
            info->range_min = 1;
            info->unit = bpm;
        }break;

       default:break;
    }

    return 0;
}

int drv_ecg_adi_adpd1080_init(void){
    int ret = 0;
	uint8_t value[2]; 
    sensor_obj_t sensor;

    /* fill the sensor obj parameters here */
    sensor.io_port    = I2C_PORT;
	sensor.tag        = TAG_DEV_HR;
    sensor.path       = dev_hr_path;
    sensor.open       = drv_ecg_adi_adpd1080_open;
    sensor.close      = drv_ecg_adi_adpd1080_close;
    sensor.read       = drv_ecg_adi_adpd1080_read;
    sensor.write      = NULL;
    sensor.ioctl      = drv_ecg_adi_adpd1080_ioctl;
    sensor.irq_handle = drv_ecg_adi_adpd1080_irq_handle;

    ret = sensor_create_obj(&sensor);
    if(unlikely(ret)){
        return -1;
    }

    ret = drv_ecg_adi_adpd1080_validate_id(&adpd1080_ctx, ADPD1080_DEVICE_ID_VALUE);
    if(unlikely(ret)){
        return -1;
    }
	ret = drv_ecg_adi_adpd1080_soft_reset(&adpd1080_ctx);
    if(unlikely(ret)){
        return -1;
    }
	ret = drv_ecg_adi_adpd1080_set_program(&adpd1080_ctx);
	if(unlikely(ret)){
        return -1;
    }
    //set odr is 100hz, and will update
    ret = drv_ecg_adi_adpd1080_set_odr(&adpd1080_ctx, ADPD1080_DEFAULT_ODR_100HZ);
    if(unlikely(ret)){
        return -1;
    }
	
	ret = drv_ecg_adi_adpd1080_set_offset(&adpd1080_ctx);
    if(unlikely(ret)){
        return -1;
    }
	
    ret = drv_ecg_adi_adpd1080_set_led_coarse(&adpd1080_ctx);
    if(unlikely(ret)){
        return -1;
    }
	
    ret = drv_ecg_adi_adpd1080_set_led(&adpd1080_ctx);
    if(unlikely(ret)){
        return -1;
    }
	
    ret = drv_ecg_adi_adpd1080_set_afe_tia(&adpd1080_ctx);
    if(unlikely(ret)){
        return -1;
    }
	
    ret = drv_ecg_adi_adpd1080_set_flt(&adpd1080_ctx);
    if(unlikely(ret)){
        return -1;
    }
	
    ret = drv_ecg_adi_adpd1080_set_power_mode(&adpd1080_ctx, DEV_POWER_ON);
    if(unlikely(ret)){
        return -1;
    }

	ret = sensor_i2c_read(&adpd1080_ctx, ADPD1080_MODE, value, I2C_DATA_LEN*2, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }
    /* update the phy sensor info to sensor hal */
    LOG("%s %s successfully \n", SENSOR_STR, __func__);
    return 0;
}

