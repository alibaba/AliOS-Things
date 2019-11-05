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


#define LIS2DW12_I2C_ADDR1                 		(0x18)
#define LIS2DW12_I2C_ADDR2                 		(0x19)
#define LIS2DW12_I2C_ADDR_TRANS(n)         		((n)<<1)  
#define LIS2DW12_I2C_ADDR                  		LIS2DW12_I2C_ADDR_TRANS(LIS2DW12_I2C_ADDR1)

#define LIS2DW12_STATUS_REG_AUX        			0x07

#define LIS2DW12_ACC_OUT_TEMP_L            		0x0C
#define LIS2DW12_ACC_OUT_TEMP_H            		0x0D
#define LIS2DW12_ACC_WHO_AM_I              		0x0F

#define LIS2DW12_ACC_CTRL_REG1             		0x20
#define LIS2DW12_ACC_CTRL_REG2             		0x21
#define LIS2DW12_ACC_CTRL_REG3             		0x22
#define LIS2DW12_ACC_CTRL_REG4             		0x23
#define LIS2DW12_ACC_CTRL_REG5             		0x24
#define LIS2DW12_ACC_CTRL_REG6            		0x25
#define LIS2DW12_ACC_OUTT_REG					0x26
#define LIS2DW12_ACC_STATUS_REG           		0x27
#define LIS2DW12_ACC_OUT_X_L              		0x28
#define LIS2DW12_ACC_OUT_X_H              		0x29
#define LIS2DW12_ACC_OUT_Y_L              		0x2A
#define LIS2DW12_ACC_OUT_Y_H              		0x2B
#define LIS2DW12_ACC_OUT_Z_L              		0x2C
#define LIS2DW12_ACC_OUT_Z_H              		0x2D
#define LIS2DW12_ACC_FIFO_CTRL_REG        		0x2E
#define LIS2DW12_ACC_FIFO_SRC_REG         		0x2F
#define LIS2DW12_ACC_CTRL_REG7            		0x3F


#define LIS2DW12_ACC_SELFTESTDISABLE           	(0x0)
#define LIS2DW12_ACC_SELFTESTENABLE           	(0x2)
#define LIS2DW12_ACC_SELFTEST_MSK           	(0xC0)
#define LIS2DW12_ACC_SELFTEST_POS           	(6)

#define LIS2DW12_ACC_RANGE_2G               	(0x0)
#define LIS2DW12_ACC_RANGE_4G               	(0x1)
#define LIS2DW12_ACC_RANGE_8G               	(0x2)
#define LIS2DW12_ACC_RANGE_16G              	(0x3)
#define LIS2DW12_ACC_RANGE_MSK              	(0X30)
#define LIS2DW12_ACC_RANGE_POS              	(4)

#define LIS2DW12_ACC_SENSITIVITY_2G         	(244)
#define LIS2DW12_ACC_SENSITIVITY_4G         	(488)
#define LIS2DW12_ACC_SENSITIVITY_8G         	(976)
#define LIS2DW12_ACC_SENSITIVITY_16G        	(1952) 

#define LIS2DW12_ACC_CHIP_ID_VALUE				(0x44)

#define LIS2DW12_SHIFT_EIGHT_BITS           	(8)
#define LIS2DW12_SHIFT_FOUR_BITS             	(4)
#define LIS2DW12_SHIFT_TWO_BITS					(2)

#define LIS2DW12_16_BIT_SHIFT               	(0xFF)
#define LIS2DW12_ACC_MUL                    	(1000)

#define LIS2DW12_ACC_ODR_POWER_DOWN				(0x00)
#define LIS2DW12_ACC_ODR_1_6_HZ            		(0x01)
#define LIS2DW12_ACC_ODR_12_5_HZ               	(0x02)
#define LIS2DW12_ACC_ODR_25_HZ               	(0x03)
#define LIS2DW12_ACC_ODR_50_HZ              	(0x04)
#define LIS2DW12_ACC_ODR_100_HZ              	(0x05)
#define LIS2DW12_ACC_ODR_200_HZ              	(0x06)
#define LIS2DW12_ACC_ODR_400_HZ              	(0x07)
#define LIS2DW12_ACC_ODR_800_HZ            		(0x08)
#define LIS2DW12_ACC_ODR_1600_HZ				(0x09)
#define LIS2DW12_ACC_ODR_MSK                 	(0xF0)
#define LIS2DW12_ACC_ODR_POS                 	(4)

#define LIS2DW12_ACC_HIGH_PERFORMANCE			(0x04)
#define LIS2DW12_ACC_LOW_POWER_1				(0x00)
#define LIS2DW12_ACC_LOW_POWER_2				(0x01)
#define LIS2DW12_ACC_LOW_POWER_3				(0x02)
#define LIS2DW12_ACC_LOW_POWER_4				(0x03)
#define LIS2DW12_ACC_MODE_MSK                 	(0x0F)
#define LIS2DW12_ACC_MODE_POS                 	(0)

#define LIS2DW12_ACC_DEFAULT_ODR_100HZ          (100)

#define LIS2DW12_BDU_ENABLE						(0x08)

#define LIS2DW12_ACC_STATUS_ZYXDA				(0x01)

#define LIS2DW12_DEFAULT_ODR_100HZ				(100)

#define LIS2DW12_ACC_SELF_TEST_MIN_X          	(17 * 4) // 17 counts, per count 4mg
#define LIS2DW12_ACC_SELF_TEST_MIN_Y           	(17 * 4) // 17 counts, per count 4mg  
#define LIS2DW12_ACC_SELF_TEST_MIN_Z           	(17 * 4)  // 17 counts, per count 4mg
#define LIS2DW12_ACC_SELF_TEST_MAX_X           	(375 * 4) // 375 counts, per count 4mg
#define LIS2DW12_ACC_SELF_TEST_MAX_Y           	(375 * 4) // 375 counts, per count 4mg
#define LIS2DW12_ACC_SELF_TEST_MAX_Z           	(375 * 4) // 375 counts, per count 4mg

#define LIS2DW12_ACC_SELF_TEST_DRY_WAIT_CNT  	5
#define LIS2DW12_ACC_SELF_TEST_AVG_SAMPLE_CNT   5


#define LIS2DW12_GET_BITSLICE(regvar, bitname)\
((regvar & bitname##_MSK) >> bitname##_POS)
	
#define LIS2DW12_SET_BITSLICE(regvar, bitname, val)\
	((regvar & ~bitname##_MSK) | ((val<<bitname##_POS)&bitname##_MSK))


static int32_t lis2dw12_acc_factor[ACC_RANGE_MAX] = { LIS2DW12_ACC_SENSITIVITY_2G, LIS2DW12_ACC_SENSITIVITY_4G, 
                                     LIS2DW12_ACC_SENSITIVITY_8G, LIS2DW12_ACC_SENSITIVITY_16G };
static int32_t cur_acc_factor = 0;


i2c_dev_t lis2dw12_ctx = {
    .port = 3,
    .config.address_width = 8,
    .config.freq = 400000,
    .config.dev_addr = LIS2DW12_I2C_ADDR,
};

static int drv_acc_st_lis2dw12_validate_id(i2c_dev_t* drv, uint8_t id_value)
{
    uint8_t value = 0x00;
    int ret = 0;

    if(drv == NULL){
        return -1;
    }
    
    ret = sensor_i2c_read(drv, LIS2DW12_ACC_WHO_AM_I, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }
    
    if (id_value != value){
        return -1;
    }
    
    return 0;
}

static int drv_acc_st_lis2dw12_set_power_mode(i2c_dev_t* drv, dev_power_mode_e mode)
{
    uint8_t value  = 0x00;
    int ret = 0;
    
    ret = sensor_i2c_read(drv, LIS2DW12_ACC_CTRL_REG1, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }

    switch(mode){
        case DEV_POWER_ON:{
            value = LIS2DW12_SET_BITSLICE(value,LIS2DW12_ACC_ODR,LIS2DW12_ACC_ODR_12_5_HZ);
            value = LIS2DW12_SET_BITSLICE(value,LIS2DW12_ACC_MODE,LIS2DW12_ACC_HIGH_PERFORMANCE);
            ret = sensor_i2c_write(drv, LIS2DW12_ACC_CTRL_REG1, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
            if(unlikely(ret)){
                return ret;
            }
        }break;
        
        case DEV_POWER_OFF:{
            value = LIS2DW12_SET_BITSLICE(value,LIS2DW12_ACC_ODR,LIS2DW12_ACC_ODR_POWER_DOWN);
            ret = sensor_i2c_write(drv, LIS2DW12_ACC_CTRL_REG1, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
            if(unlikely(ret)){
                return ret;
            }
        }break;
            
        case DEV_SLEEP:{
            value = LIS2DW12_SET_BITSLICE(value,LIS2DW12_ACC_ODR,LIS2DW12_ACC_ODR_1_6_HZ);
            value = LIS2DW12_SET_BITSLICE(value,LIS2DW12_ACC_MODE,LIS2DW12_ACC_LOW_POWER_1);
            ret = sensor_i2c_write(drv, LIS2DW12_ACC_CTRL_REG1, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
            if(unlikely(ret)){
                return ret;
            }

        }break;

       default:break;
    }
    return 0;
}

static uint8_t drv_acc_st_lis2dw12_hz2odr(uint32_t hz)
{
    if(hz > 800)
        return LIS2DW12_ACC_ODR_1600_HZ;
    else if(hz > 400)
        return LIS2DW12_ACC_ODR_800_HZ;
    else if(hz > 200)
        return LIS2DW12_ACC_ODR_400_HZ;
    else if(hz > 100)
        return LIS2DW12_ACC_ODR_200_HZ;
    else if(hz > 50)
        return LIS2DW12_ACC_ODR_100_HZ;
    else if(hz > 25)
        return LIS2DW12_ACC_ODR_50_HZ;
    else if(hz > 13)
        return LIS2DW12_ACC_ODR_25_HZ;
    else if(hz >= 1)
        return LIS2DW12_ACC_ODR_12_5_HZ;
    else
        return LIS2DW12_ACC_ODR_1_6_HZ;

}

static int drv_acc_st_lis2dw12_set_odr(i2c_dev_t* drv, uint32_t hz)
{
    int ret = 0;
    uint8_t value = 0x00;
    uint8_t odr = drv_acc_st_lis2dw12_hz2odr(hz);

    ret = sensor_i2c_read(drv, LIS2DW12_ACC_CTRL_REG1, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }

    value = LIS2DW12_SET_BITSLICE(value,LIS2DW12_ACC_ODR,odr);
	
    ret = sensor_i2c_write(drv, LIS2DW12_ACC_CTRL_REG1, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }
    return 0;    
}

static int drv_acc_st_lis2dw12_set_range(i2c_dev_t* drv, uint32_t range)
{

    int ret = 0;
    uint8_t value = 0x00;
    uint8_t tmp = 0;

    ret = sensor_i2c_read(drv, LIS2DW12_ACC_CTRL_REG6, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }

    switch (range){
        case ACC_RANGE_2G:{
            tmp = LIS2DW12_ACC_RANGE_2G;
        }break;
        
        case ACC_RANGE_4G:{
            tmp = LIS2DW12_ACC_RANGE_4G;
        }break;
        
        case ACC_RANGE_8G:{
            tmp = LIS2DW12_ACC_RANGE_8G;
        }break;
            
        case ACC_RANGE_16G:{
            tmp = LIS2DW12_ACC_RANGE_16G;
        }break;
        
        default:break;
    }
    
    value  = LIS2DW12_SET_BITSLICE(value,LIS2DW12_ACC_RANGE,tmp);
    
    ret = sensor_i2c_write(drv, LIS2DW12_ACC_CTRL_REG6, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }

    if((range >= ACC_RANGE_2G)&&(range <= ACC_RANGE_16G)){
        cur_acc_factor = lis2dw12_acc_factor[range];
    }
    
    return 0;
}


static int drv_acc_st_lis2dw12_set_bdu(i2c_dev_t* drv)
{
    uint8_t value = 0x00;
    int ret = 0;
    
    ret = sensor_i2c_read(drv, LIS2DW12_ACC_CTRL_REG2, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }
    value |= LIS2DW12_BDU_ENABLE;
    
    ret = sensor_i2c_write(drv, LIS2DW12_ACC_CTRL_REG2, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }
    return 0;
}

static int drv_acc_st_lis2dw12_st_discard(i2c_dev_t* drv)
{
    uint8_t i;
    uint8_t value = 0x00;
    int ret = 0;
    uint8_t buffer[6];

    for (i = 0; i < LIS2DW12_ACC_SELF_TEST_DRY_WAIT_CNT; i ++) {
        ret = sensor_i2c_read(drv, LIS2DW12_ACC_STATUS_REG, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
        if(unlikely(ret)){
            LOG("%s %s %s %d\n", SENSOR_STR, __func__, ERROR_LINE, __LINE__);
            return ret;
        }
        if (value & 0x01)
            break;

        aos_msleep(20);
    }

    if (i >= LIS2DW12_ACC_SELF_TEST_DRY_WAIT_CNT) {
        LOG("%s %s %s %d\n", SENSOR_STR, __func__, ERROR_LINE, __LINE__);
        return -1;
    }

    ret = sensor_i2c_read(drv, (LIS2DW12_ACC_OUT_X_L | 0x80), buffer, 6, I2C_OP_RETRIES);
    if(unlikely(ret)){
        LOG("%s %s %s %d\n", SENSOR_STR, __func__, ERROR_LINE, __LINE__);
        return ret;
    }

    return ret;
}

static int drv_acc_st_lis2dw12_st_data(i2c_dev_t* drv,int32_t* data)
{
    uint8_t i, j;
    int16_t x_raw, y_raw, z_raw;
    int32_t x_mg, y_mg, z_mg;
    int32_t x_sum, y_sum, z_sum;
    uint8_t value = 0x00;
    int ret = 0;
    uint8_t buffer[6];

    x_sum = 0; y_sum = 0; z_sum = 0;
    
    for (i = 0; i < LIS2DW12_ACC_SELF_TEST_AVG_SAMPLE_CNT; i ++) {
        for (j = 0; j < LIS2DW12_ACC_SELF_TEST_DRY_WAIT_CNT; j ++) {
            ret = sensor_i2c_read(drv, LIS2DW12_ACC_STATUS_REG, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
            if(unlikely(ret)){
                LOG("%s %s %s %d\n", SENSOR_STR, __func__, ERROR_LINE, __LINE__);
                return ret;
            }
            if (value & 0x08)
                break;
        
            aos_msleep(20);
        }

        if (j >= LIS2DW12_ACC_SELF_TEST_DRY_WAIT_CNT) {
            LOG("%s %s %s %d\n", SENSOR_STR, __func__, ERROR_LINE, __LINE__);
            return -1;
        }

        ret = sensor_i2c_read(drv,  (LIS2DW12_ACC_OUT_X_L | 0x80), buffer, 6, I2C_OP_RETRIES);
        if(unlikely(ret)){
            LOG("%s %s %s %d\n", SENSOR_STR, __func__, ERROR_LINE, __LINE__);
            return ret;
        }

        x_raw = (buffer[1] << 8) + buffer[0];
        y_raw = (buffer[3] << 8) + buffer[2];
        z_raw = (buffer[5] << 8) + buffer[4];
        //LOG("%s %s %d: i(%d), raw(%d, %d, %d)\n", SENSOR_STR, __func__, __LINE__, i, x_raw, y_raw, z_raw);

        x_mg = x_raw >> 4;
        y_mg = y_raw >> 4;
        z_mg = z_raw  >> 4;
        //LOG("%s %s %d: i(%d), mg(%d, %d, %d)\n", SENSOR_STR, __func__, __LINE__, i, x_mg, y_mg, z_mg);

        x_sum += x_mg;
        y_sum += y_mg;
        z_sum += z_mg;
    }

    data[0] = x_sum / LIS2DW12_ACC_SELF_TEST_AVG_SAMPLE_CNT;
    data[1] = y_sum / LIS2DW12_ACC_SELF_TEST_AVG_SAMPLE_CNT;
    data[2] = z_sum / LIS2DW12_ACC_SELF_TEST_AVG_SAMPLE_CNT;
    return ret;
}

static int drv_acc_st_lis2dw12_self_test(i2c_dev_t* drv,int32_t* data)
{
    uint8_t i;
    uint8_t value = 0x00;
    int ret = 0;
    uint8_t ctrl_reg[6];
    int32_t out_nost[3];
    int32_t out_st[3];
    int32_t out_diff[3];

    // Save cfg registers which will be modified during self-test
    ret = sensor_i2c_read(drv, (LIS2DW12_ACC_CTRL_REG1 | 0x80), ctrl_reg, 6, I2C_OP_RETRIES);
    if(unlikely(ret)){
        LOG("%s %s %s %d\n", SENSOR_STR, __func__, ERROR_LINE, __LINE__);
        return ret;
    }

    // Initialize Sensor, turn on sensor, enable X/Y/Z axes 
    // Set BDU=1, FS=4G, High-Performance mode, ODR = 50Hz
    value = 0x08;
    ret = sensor_i2c_write(drv, LIS2DW12_ACC_CTRL_REG2, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        LOG("%s %s %s %d\n", SENSOR_STR, __func__, ERROR_LINE, __LINE__);
        goto restore;
    }
    value = 0x00;
    ret = sensor_i2c_write(drv, LIS2DW12_ACC_CTRL_REG3, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        LOG("%s %s %s %d\n", SENSOR_STR, __func__, ERROR_LINE, __LINE__);
        goto restore;
    }
    
    value = 0x00;
    ret = sensor_i2c_write(drv, LIS2DW12_ACC_CTRL_REG4, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        LOG("%s %s %s %d\n", SENSOR_STR, __func__, ERROR_LINE, __LINE__);
        goto restore;
    }

    value = 0x00;
    ret = sensor_i2c_write(drv, LIS2DW12_ACC_CTRL_REG5, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        LOG("%s %s %s %d\n", SENSOR_STR, __func__, ERROR_LINE, __LINE__);
        goto restore;
    }

    value = 0x10;
    ret = sensor_i2c_write(drv, LIS2DW12_ACC_CTRL_REG6, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        LOG("%s %s %s %d\n", SENSOR_STR, __func__, ERROR_LINE, __LINE__);
        goto restore;
    }

    value = 0x44;
    ret = sensor_i2c_write(drv, LIS2DW12_ACC_CTRL_REG1, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        LOG("%s %s %s %d\n", SENSOR_STR, __func__, ERROR_LINE, __LINE__);
        goto restore;
    }

    aos_msleep(100);

    // Discard the first sample
    ret = drv_acc_st_lis2dw12_st_discard(drv);
    if(unlikely(ret)){
        LOG("%s %s %s %d\n", SENSOR_STR, __func__, ERROR_LINE, __LINE__);
        goto restore;
    }

    // Read some samples, and averate them
    ret = drv_acc_st_lis2dw12_st_data(drv, out_nost);
    if(unlikely(ret)){
        LOG("%s %s %s %d\n", SENSOR_STR, __func__, ERROR_LINE, __LINE__);
        goto restore;
    }

    // Enable seft-test
    value = 0x40;
    ret = sensor_i2c_write(drv, LIS2DW12_ACC_CTRL_REG3, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        LOG("%s %s %s %d\n", SENSOR_STR, __func__, ERROR_LINE, __LINE__);
        goto restore;
    }
    aos_msleep(100);
    
    // Discard the first sample
    ret = drv_acc_st_lis2dw12_st_discard(drv);
    if(unlikely(ret)){
        LOG("%s %s %s %d\n", SENSOR_STR, __func__, ERROR_LINE, __LINE__);
        goto restore;
    }

    // Read some samples, and average them
    ret = drv_acc_st_lis2dw12_st_data(drv, out_st);
    if(unlikely(ret)){
        LOG("%s %s %s %d\n", SENSOR_STR, __func__, ERROR_LINE, __LINE__);
        goto restore;
    }

    // Check if the differences are between min and max
    for (i = 0; i < 3; i ++) {
        out_diff[i] = abs(out_st[i] - out_nost[i]);
        data[i] = out_diff[i];
    }
    
    if ((LIS2DW12_ACC_SELF_TEST_MIN_X > out_diff[0]) || (out_diff[0] > LIS2DW12_ACC_SELF_TEST_MAX_X)) {
        LOG("%s %s %s %d\n", SENSOR_STR, __func__, ERROR_LINE, __LINE__);
        ret = -1;
        goto restore;
    }
    if ((LIS2DW12_ACC_SELF_TEST_MIN_Y > out_diff[1]) || (out_diff[1] > LIS2DW12_ACC_SELF_TEST_MAX_Y)) {
        LOG("%s %s %s %d\n", SENSOR_STR, __func__, ERROR_LINE, __LINE__);
        ret = -1;
        goto restore;
    }
    if ((LIS2DW12_ACC_SELF_TEST_MIN_Z > out_diff[2]) || (out_diff[2] > LIS2DW12_ACC_SELF_TEST_MAX_Z)) {
        LOG("%s %s %s %d\n", SENSOR_STR, __func__, ERROR_LINE, __LINE__);
        ret = -1;
        goto restore;
    }

restore:
    ret += sensor_i2c_write(drv, LIS2DW12_ACC_CTRL_REG1 | 0x80, ctrl_reg, 6, I2C_OP_RETRIES);
    return ret;
}

static void drv_acc_st_lis2dw12_irq_handle(void)
{
    /* no handle so far */
}

static int drv_acc_st_lis2dw12_open(void)
{
    int ret = 0;
    ret  = drv_acc_st_lis2dw12_set_power_mode(&lis2dw12_ctx, DEV_POWER_ON);
    if(unlikely(ret)){
        return -1;
    }

    ret = drv_acc_st_lis2dw12_set_range(&lis2dw12_ctx, ACC_RANGE_8G);
    if(unlikely(ret)){
        return -1;
    }

    ret = drv_acc_st_lis2dw12_set_odr(&lis2dw12_ctx, LIS2DW12_ACC_DEFAULT_ODR_100HZ);
    if(unlikely(ret)){
        return -1;
    }
     
    return 0;

}

static int drv_acc_st_lis2dw12_close(void)
{
    int ret = 0;
    ret  = drv_acc_st_lis2dw12_set_power_mode(&lis2dw12_ctx, DEV_POWER_OFF);
    if(unlikely(ret)){
        return -1;
    }
    return 0;
}

static int drv_acc_st_lis2dw12_read(void *buf, size_t len)
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
	
	ret = sensor_i2c_read(&lis2dw12_ctx, (LIS2DW12_ACC_OUT_X_L | 0x80), reg, 6, I2C_OP_RETRIES);
	
	if(unlikely(ret)){
        return -1;
    }
    accel->data[DATA_AXIS_X] = (int16_t)((((int16_t)((int8_t)reg[1]))<< LIS2DW12_SHIFT_EIGHT_BITS)|(reg[0]));
	accel->data[DATA_AXIS_X] = accel->data[DATA_AXIS_X] >> LIS2DW12_SHIFT_TWO_BITS;
    accel->data[DATA_AXIS_Y] = (int16_t)((((int16_t)((int8_t)reg[3]))<< LIS2DW12_SHIFT_EIGHT_BITS)|(reg[2]));
	accel->data[DATA_AXIS_Y] = accel->data[DATA_AXIS_Y] >> LIS2DW12_SHIFT_TWO_BITS;
    accel->data[DATA_AXIS_Z] = (int16_t)((((int16_t)((int8_t)reg[5]))<< LIS2DW12_SHIFT_EIGHT_BITS)|(reg[4]));
	accel->data[DATA_AXIS_Z] = accel->data[DATA_AXIS_Z] >> LIS2DW12_SHIFT_TWO_BITS;

    if(cur_acc_factor != 0){
        accel->data[DATA_AXIS_X] = accel->data[DATA_AXIS_X] * cur_acc_factor / LIS2DW12_ACC_MUL;
        accel->data[DATA_AXIS_Y] = accel->data[DATA_AXIS_Y] * cur_acc_factor / LIS2DW12_ACC_MUL;
        accel->data[DATA_AXIS_Z] = accel->data[DATA_AXIS_Z] * cur_acc_factor / LIS2DW12_ACC_MUL;
    }
    
    accel->timestamp = aos_now_ms();

    return (int)size;
}


static int drv_acc_st_lis2dw12_ioctl(int cmd, unsigned long arg)
{
    int ret = 0;
    dev_sensor_info_t *info = (dev_sensor_info_t *)arg;
    
    switch(cmd){
        case SENSOR_IOCTL_ODR_SET:{
            ret = drv_acc_st_lis2dw12_set_odr(&lis2dw12_ctx, arg);
            if(unlikely(ret)){
                return -1;
            }
        }break;
        case SENSOR_IOCTL_RANGE_SET:{
            ret = drv_acc_st_lis2dw12_set_range(&lis2dw12_ctx, arg);
            if(unlikely(ret)){
                return -1;
            }
        }break;
        case SENSOR_IOCTL_SET_POWER:{
            ret = drv_acc_st_lis2dw12_set_power_mode(&lis2dw12_ctx, arg);
            if(unlikely(ret)){
                return -1;
            }
        }break;
        case SENSOR_IOCTL_GET_INFO:{ 
            /* fill the dev info here */
            info->model = "LIS2DW12";
            info->range_max = 16;
            info->range_min = 2;
            info->unit = mg;
			
        }break;
	   	case SENSOR_IOCTL_SELF_TEST:{
		   ret = drv_acc_st_lis2dw12_self_test(&lis2dw12_ctx, (int32_t*)info->data);
		   //printf("%d	%d	 %d\n",info->data[0],info->data[1],info->data[2]);
           LOG("%s %s: %d, %d, %d\n", SENSOR_STR, __func__, info->data[0],info->data[1],info->data[2]);
		   return ret;
	   }
       default:break;
    }

    return 0;
}

int drv_acc_st_lis2dw12_init(void){
    int ret = 0;
    sensor_obj_t sensor;
    memset(&sensor, 0, sizeof(sensor));

    /* fill the sensor obj parameters here */
    sensor.io_port    = I2C_PORT;
    sensor.tag        = TAG_DEV_ACC;
    sensor.path       = dev_acc_path;
    sensor.open       = drv_acc_st_lis2dw12_open;
    sensor.close      = drv_acc_st_lis2dw12_close;
    sensor.read       = drv_acc_st_lis2dw12_read;
    sensor.write      = NULL;
    sensor.ioctl      = drv_acc_st_lis2dw12_ioctl;
    sensor.irq_handle = drv_acc_st_lis2dw12_irq_handle;

    ret = sensor_create_obj(&sensor);
    if(unlikely(ret)){
        return -1;
    }

    ret = drv_acc_st_lis2dw12_validate_id(&lis2dw12_ctx, LIS2DW12_ACC_CHIP_ID_VALUE);
    if(unlikely(ret)){
        return -1;
    }

    ret = drv_acc_st_lis2dw12_set_range(&lis2dw12_ctx, ACC_RANGE_8G);
    if(unlikely(ret)){
        return -1;
    }

    //set odr is 100hz, and will update
    ret = drv_acc_st_lis2dw12_set_odr(&lis2dw12_ctx, LIS2DW12_DEFAULT_ODR_100HZ);
    if(unlikely(ret)){
        return -1;
    }

	//set bdu
    ret = drv_acc_st_lis2dw12_set_bdu(&lis2dw12_ctx);
    if(unlikely(ret)){
        return -1;
    }

    /* update the phy sensor info to sensor hal */
    LOG("%s %s successfully \n", SENSOR_STR, __func__);
    return 0;
}


SENSOR_DRV_ADD(drv_acc_st_lis2dw12_init);

