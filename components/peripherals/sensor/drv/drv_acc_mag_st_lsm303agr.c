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


#define LSM303AGR_MAG_I2C_ADDR1              		0x1E
#define LSM303AGR_MAG_I2C_ADDR_TRANS(n)       		((n)<<1)  
#define LSM303AGR_MAG_I2C_ADDR                		LSM303AGR_MAG_I2C_ADDR_TRANS(LSM303AGR_MAG_I2C_ADDR1)
#define LSM303AGR_MAG_BIT(x)                  		(x)

#define LSM303AGR_ACC_I2C_ADDR1                 	(0x19)
#define LSM303AGR_ACC_I2C_ADDR_TRANS(n)         	((n)<<1)  
#define LSM303AGR_ACC_I2C_ADDR                  	LSM303AGR_ACC_I2C_ADDR_TRANS(LSM303AGR_ACC_I2C_ADDR1)

#define LSM303AGR_ACC_STATUS_REG_AUX        		0x07

#define LSM303AGR_ACC_OUT_TEMP_L            	0x0C
#define LSM303AGR_ACC_OUT_TEMP_H            	0x0D
#define LSM303AGR_ACC_WHO_AM_I              	0x0F
#define LSM303AGR_ACC_CTRL_REG0             	0x1E
#define LSM303AGR_ACC_TEMP_CFG_REG          	0x1F

#define LSM303AGR_ACC_CTRL_REG1             	0x20
#define LSM303AGR_ACC_CTRL_REG2             	0x21
#define LSM303AGR_ACC_CTRL_REG3             	0x22
#define LSM303AGR_ACC_CTRL_REG4             	0x23
#define LSM303AGR_ACC_CTRL_REG5             	0x24
#define LSM303AGR_ACC_CTRL_REG6            		0x25
#define LSM303AGR_ACC_REFERENCE            		0x26
#define LSM303AGR_ACC_STATUS_REG           		0x27
#define LSM303AGR_ACC_OUT_X_L              		0x28
#define LSM303AGR_ACC_OUT_X_H              		0x29
#define LSM303AGR_ACC_OUT_Y_L              		0x2A
#define LSM303AGR_ACC_OUT_Y_H              		0x2B
#define LSM303AGR_ACC_OUT_Z_L              		0x2C
#define LSM303AGR_ACC_OUT_Z_H              		0x2D
#define LSM303AGR_ACC_FIFO_CTRL_REG        		0x2E
#define LSM303AGR_ACC_FIFO_SRC_REG         		0x2F
#define LSM303AGR_ACC_INT1_CFG             		0x30
#define LSM303AGR_ACC_INT1_SRC             		0x31
#define LSM303AGR_ACC_INT1_THS             		0x32
#define LSM303AGR_ACC_INT1_DURATION        		0x33
#define LSM303AGR_ACC_INT2_CFG             		0x34
#define LSM303AGR_ACC_INT2_SRC             		0x35
#define LSM303AGR_ACC_INT2_THS             		0x36
#define LSM303AGR_ACC_INT2_DURATION        		0x37
#define LSM303AGR_ACC_CLICK_CFG            		0x38
#define LSM303AGR_ACC_CLICK_SRC            		0x39
#define LSM303AGR_ACC_CLICK_THS            		0x3A
#define LSM303AGR_ACC_TIME_LIMIT           		0x3B
#define LSM303AGR_ACC_TIME_LATENCY        		0x3C
#define LSM303AGR_ACC_TIME_WINDOW          		0x3D
#define LSM303AGR_ACC_ACT_THS              		0x3E
#define LSM303AGR_ACC_ACT_DUR              		0x3F


#define LSM303AGR_ACC_SELFTESTDISABLE           	(0x0)
#define LSM303AGR_ACC_SELFTESTENABLE           	(0x2)
#define LSM303AGR_ACC_SELFTEST_MSK           	(0x06)
#define LSM303AGR_ACC_SELFTEST_POS           	(2)

#define LSM303AGR_ACC_RANGE_2G               	(0x0)
#define LSM303AGR_ACC_RANGE_4G               	(0x1)
#define LSM303AGR_ACC_RANGE_8G               	(0x2)
#define LSM303AGR_ACC_RANGE_16G              	(0x3)
#define LSM303AGR_ACC_RANGE_MSK              	(0X30)
#define LSM303AGR_ACC_RANGE_POS              	(4)

#define LSM303AGR_ACC_SENSITIVITY_2G         	(1)
#define LSM303AGR_ACC_SENSITIVITY_4G         	(2)
#define LSM303AGR_ACC_SENSITIVITY_8G         	(4)
#define LSM303AGR_ACC_SENSITIVITY_16G        	(12) 

#define LSM303AGR_ACC_CHIP_ID_VALUE				(0x33)

#define LSM303AGR_ACC_SHIFT_EIGHT_BITS           	(8)
#define LSM303AGR_ACC_SHIFT_FOUR_BITS             	(4)

#define LSM303AGR_ACC_16_BIT_SHIFT               	(0xFF)
#define LSM303AGR_ACC_MUL                    	(1000)

#define LSM303AGR_ACC_ODR_POWER_DOWN				(0x00)
#define LSM303AGR_ACC_ODR_1_HZ             		(0x01)
#define LSM303AGR_ACC_ODR_10_HZ               	(0x02)
#define LSM303AGR_ACC_ODR_25_HZ               	(0x03)
#define LSM303AGR_ACC_ODR_50_HZ              	(0x04)
#define LSM303AGR_ACC_ODR_100_HZ              	(0x05)
#define LSM303AGR_ACC_ODR_200_HZ              	(0x06)
#define LSM303AGR_ACC_ODR_400_HZ              	(0x07)
#define LSM303AGR_ACC_ODR_1_62_KHZ            	(0x08)
#define LSM303AGR_ACC_ODR_5_376_KHZ				(0x09)
#define LSM303AGR_ACC_ODR_1_344_HZ				(0x09)
#define LSM303AGR_ACC_ODR_MSK                 	(0XF0)
#define LSM303AGR_ACC_ODR_POS                 	(4)

#define LSM303AGR_ACC_DEFAULT_ODR_100HZ          (100)

#define LSM303AGR_ACC_BDU_ENABLE						(0x80)

#define LSM303AGR_ACC_STATUS_ZYXDA				(0x08)

#define LSM303AGR_ACC_DEFAULT_ODR_100HZ			(100)

#define LSM303AGR_ACC_SELF_TEST_MIN_X              (17 * 4) // 17 counts, per count 4mg
#define LSM303AGR_ACC_SELF_TEST_MIN_Y              (17 * 4) // 17 counts, per count 4mg  
#define LSM303AGR_ACC_SELF_TEST_MIN_Z              (17 * 4)  // 17 counts, per count 4mg
#define LSM303AGR_ACC_SELF_TEST_MAX_X              (360 * 4) // 360 counts, per count 4mg
#define LSM303AGR_ACC_SELF_TEST_MAX_Y              (360 * 4) // 360 counts, per count 4mg
#define LSM303AGR_ACC_SELF_TEST_MAX_Z              (360 * 4) // 360 counts, per count 4mg

#define LSM303AGR_ACC_SELF_TEST_DRY_WAIT_CNT  5
#define LSM303AGR_ACC_SELF_TEST_AVG_SAMPLE_CNT    5

#define LSM303AGR_ACC_GET_BITSLICE(regvar, bitname)\
((regvar & bitname##_MSK) >> bitname##_POS)
	
#define LSM303AGR_ACC_SET_BITSLICE(regvar, bitname, val)\
	((regvar & ~bitname##_MSK) | ((val<<bitname##_POS)&bitname##_MSK))


static int32_t lsm303agr_acc_factor[ACC_RANGE_MAX] = { LSM303AGR_ACC_SENSITIVITY_2G, LSM303AGR_ACC_SENSITIVITY_4G, 
                                     LSM303AGR_ACC_SENSITIVITY_8G, LSM303AGR_ACC_SENSITIVITY_16G };
static int32_t cur_acc_factor = 0;

#define LSM303AGR_BIT(x)                      (x)

#define LSM303AGR_MAG_OFFSET_X_REG_L          0x45
#define LSM303AGR_MAG_OFFSET_X_REG_H          0x46
#define LSM303AGR_MAG_OFFSET_Y_REG_L          0x47
#define LSM303AGR_MAG_OFFSET_Y_REG_H          0x48
#define LSM303AGR_MAG_OFFSET_Z_REG_L          0x49
#define LSM303AGR_MAG_OFFSET_Z_REG_H          0x4A
#define LSM303AGR_MAG_WHO_AM_I                0x4F
#define LSM303AGR_MAG_CFG_REG_A               0x60
#define LSM303AGR_MAG_CFG_REG_B               0x61
#define LSM303AGR_MAG_CFG_REG_C               0x62
#define LSM303AGR_MAG_INT_CRTL_REG            0x63
#define LSM303AGR_MAG_INT_SOURCE_REG          0x64
#define LSM303AGR_MAG_INT_THS_L_REG           0x65
#define LSM303AGR_MAG_INT_THS_H_REG           0x66
#define LSM303AGR_MAG_STATUS_REG              0x67
#define LSM303AGR_MAG_OUTX_L_REG              0x68
#define LSM303AGR_MAG_OUTX_H_REG              0x69
#define LSM303AGR_MAG_OUTY_L_REG              0x6A
#define LSM303AGR_MAG_OUTY_H_REG              0x6B
#define LSM303AGR_MAG_OUTZ_L_REG              0x6C
#define LSM303AGR_MAG_OUTZ_H_REG              0x6D
#define LSM303AGR_MAG_TEMP_OUT_L_REG          0x6E
#define LSM303AGR_MAG_TEMP_OUT_H_REG          0x6F

#define I_AM_LSM303AGR                        0x40

#define LSM303AGR_MAG_ODR_BIT                 LSM303AGR_BIT(0x0C)
#define LSM303AGR_MAG_ODR_10_HZ               0x00 
#define LSM303AGR_MAG_ODR_20_HZ               0x04
#define LSM303AGR_MAG_ODR_50_HZ               0x08
#define LSM303AGR_MAG_ODR_100_HZ              0x0C  

#define LSM303AGR_MAG_SELFTEST_DISABLE        0x00   
#define LSM303AGR_MAG_SELFTEST_ENABLE         0x02

#define LSM303AGR_MAG_FS_50_GA                0x00 

#define LSM303AGR_MAG_REBOOT_DEFAULT          0x00
#define LSM303AGR_MAG_REBOOT_ENABLE           0x40
   
#define LSM303AGR_MAG_SOFT_RESET_DEFAULT      0x00
#define LSM303AGR_MAG_SOFT_RESET_ENABLE       0x02
   
#define LSM303AGR_MAG_CONFIG_LOWPOWER_BIT     LSM303AGR_BIT(0x10)
#define LSM303AGR_MAG_CONFIG_NORMAL_MODE      0x00
#define LSM303AGR_MAG_CONFIG_LOWPOWER_MODE    0x10
   
#define LSM303AGR_MAG_POWERMODE_BIT           LSM303AGR_BIT(0x03)      
#define LSM303AGR_MAG_CONTINUOUS_MODE         0x00
#define LSM303AGR_MAG_SINGLE_MODE             0x01
#define LSM303AGR_MAG_POWERDOWN1_MODE         0x02
#define LSM303AGR_MAG_POWERDOWN2_MODE         0x03

#define LSM303AGR_MAG_COMP_TEMP_EN       		0x80

#define LSM303AGR_MAG_OFF_CANC           		0x02

#define LSM303AGR_MAG_BLE_BIT                 LSM303AGR_BIT(0x08)
#define LSM303AGR_MAG_BLE_LSB                 0x00
#define LSM303AGR_MAG_BLE_MSB                 0x08

#define LSM303AGR_MAG_BDU_BIT                 LSM303AGR_BIT(0x10)
#define LSM303AGR_MAG_BDU_CONTINUOUS          0x00
#define LSM303AGR_MAG_BDU_MSBLSB              0x10
   
//#define LSM303AGR_MAG_SENSITIVITY_FOR_FS_50GA   15/10  
#define LSM303AGR_MAG_SENSITIVITY_FOR_FS_50GA   1500  

#define LSM303AGR_MAG_SELF_TEST_MIN_X              (15) // 15mGuass
#define LSM303AGR_MAG_SELF_TEST_MIN_Y              (15) // 15mGuass    
#define LSM303AGR_MAG_SELF_TEST_MIN_Z              (15)  // 15mGuass
#define LSM303AGR_MAG_SELF_TEST_MAX_X              (500) // 500mGuass
#define LSM303AGR_MAG_SELF_TEST_MAX_Y              (500) // 500mGuass    
#define LSM303AGR_MAG_SELF_TEST_MAX_Z              (500)  // 500mGuass

#define LSM303AGR_MAG_SELF_TEST_DRY_WAIT_CNT  5
#define LSM303AGR_MAG_SELF_TEST_AVG_SAMPLE_CNT    50

i2c_dev_t lsm303agr_mag_ctx = {
    .port = 3,
    .config.address_width = 8,
    .config.freq = 400000,
    .config.dev_addr = LSM303AGR_MAG_I2C_ADDR,
};

i2c_dev_t lsm303agr_acc_ctx = {
    .port = 3,
    .config.address_width = 8,
    .config.freq = 400000,
    .config.dev_addr = LSM303AGR_ACC_I2C_ADDR,
};

static int drv_acc_st_lsm303agr_validate_id(i2c_dev_t* drv, uint8_t id_value)
{
    uint8_t value = 0x00;
    int ret = 0;

    if(drv == NULL){
        return -1;
    }
    
    ret = sensor_i2c_read(drv, LSM303AGR_ACC_WHO_AM_I, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }
    
    if (id_value != value){
        return -1;
    }
    
    return 0;
}

static int drv_acc_st_lsm303agr_set_power_mode(i2c_dev_t* drv, dev_power_mode_e mode)
{
    uint8_t value = 0x00;
    int ret = 0;
    
    ret = sensor_i2c_read(drv, LSM303AGR_ACC_CTRL_REG1, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }

    switch(mode){
        case DEV_POWER_ON:{
            value = LSM303AGR_ACC_SET_BITSLICE(value,LSM303AGR_ACC_ODR,LSM303AGR_ACC_ODR_10_HZ);
            ret = sensor_i2c_write(drv, LSM303AGR_ACC_CTRL_REG1, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
            if(unlikely(ret)){
                return ret;
            }
        }break;
        
        case DEV_POWER_OFF:{
            value = LSM303AGR_ACC_SET_BITSLICE(value,LSM303AGR_ACC_ODR,LSM303AGR_ACC_ODR_POWER_DOWN);
            ret = sensor_i2c_write(drv, LSM303AGR_ACC_CTRL_REG1, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
            if(unlikely(ret)){
                return ret;
            }
        }break;
            
        case DEV_SLEEP:{
            value = LSM303AGR_ACC_SET_BITSLICE(value,LSM303AGR_ACC_ODR,LSM303AGR_ACC_ODR_10_HZ);
            ret = sensor_i2c_write(drv, LSM303AGR_ACC_CTRL_REG1, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
            if(unlikely(ret)){
                return ret;
            }

        }break;

       default:break;
    }
    return 0;
}

static uint8_t drv_acc_st_lsm303agr_hz2odr(uint32_t hz)
{
    if(hz > 1620)
        return LSM303AGR_ACC_ODR_5_376_KHZ;
    else if(hz > 1344)
        return LSM303AGR_ACC_ODR_1_62_KHZ;
    else if(hz > 400)
        return LSM303AGR_ACC_ODR_1_344_HZ;
    else if(hz > 200)
        return LSM303AGR_ACC_ODR_400_HZ;
    else if(hz > 100)
        return LSM303AGR_ACC_ODR_200_HZ;
    else if(hz > 50)
        return LSM303AGR_ACC_ODR_100_HZ;
    else if(hz > 25)
        return LSM303AGR_ACC_ODR_50_HZ;
    else if(hz > 10)
        return LSM303AGR_ACC_ODR_25_HZ;
    else if(hz >= 1)
        return LSM303AGR_ACC_ODR_10_HZ;
    else
        return LSM303AGR_ACC_ODR_1_HZ;

}

static int drv_acc_st_lsm303agr_set_odr(i2c_dev_t* drv, uint32_t hz)
{
    int ret = 0;
    uint8_t value = 0x00;
    uint8_t odr = drv_acc_st_lsm303agr_hz2odr(hz);

    ret = sensor_i2c_read(drv, LSM303AGR_ACC_CTRL_REG1, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }

    value = LSM303AGR_ACC_SET_BITSLICE(value,LSM303AGR_ACC_ODR,odr);
	
    ret = sensor_i2c_write(drv, LSM303AGR_ACC_CTRL_REG1, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }
    return 0;    
}

static int drv_acc_st_lsm303agr_set_range(i2c_dev_t* drv, uint32_t range)
{

    int ret = 0;
    uint8_t value = 0x00;
    uint8_t tmp = 0;

    ret = sensor_i2c_read(drv, LSM303AGR_ACC_CTRL_REG4, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }

    switch (range){
        case ACC_RANGE_2G:{
            tmp = LSM303AGR_ACC_RANGE_2G;
        }break;
        
        case ACC_RANGE_4G:{
            tmp = LSM303AGR_ACC_RANGE_4G;
        }break;
        
        case ACC_RANGE_8G:{
            tmp = LSM303AGR_ACC_RANGE_8G;
        }break;
            
        case ACC_RANGE_16G:{
            tmp = LSM303AGR_ACC_RANGE_16G;
        }break;
        
        default:break;
    }
    
    value  = LSM303AGR_ACC_SET_BITSLICE(value,LSM303AGR_ACC_RANGE,tmp);
    
    ret = sensor_i2c_write(drv, LSM303AGR_ACC_CTRL_REG4, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }

    if((range >= ACC_RANGE_2G)&&(range <= ACC_RANGE_16G)){
        cur_acc_factor = lsm303agr_acc_factor[range];
    }
    
    return 0;
}


static int drv_acc_st_lsm303agr_set_bdu(i2c_dev_t* drv)
{
    uint8_t value = 0x00;
    int ret = 0;
    
    ret = sensor_i2c_read(drv, LSM303AGR_ACC_CTRL_REG4, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }
    value |= LSM303AGR_ACC_BDU_ENABLE;
    
    ret = sensor_i2c_write(drv, LSM303AGR_ACC_CTRL_REG4, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }
    return 0;
}

static int drv_acc_st_lsm303agr_st_discard(i2c_dev_t* drv)
{
    uint8_t i;
    uint8_t value = 0x00;
    int ret = 0;
    uint8_t buffer[6];

    for (i = 0; i < LSM303AGR_ACC_SELF_TEST_DRY_WAIT_CNT; i ++) {
        ret = sensor_i2c_read(drv, LSM303AGR_ACC_STATUS_REG, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
        if(unlikely(ret)){
            LOG("%s %s %s %d\n", SENSOR_STR, __func__, ERROR_LINE, __LINE__);
            return ret;
        }
        if (value & 0x08)
            break;

        aos_msleep(20);
    }

    if (i >= LSM303AGR_ACC_SELF_TEST_DRY_WAIT_CNT) {
        LOG("%s %s %s %d\n", SENSOR_STR, __func__, ERROR_LINE, __LINE__);
        return -1;
    }

    ret = sensor_i2c_read(drv, (LSM303AGR_ACC_OUT_X_L | 0x80), buffer, 6, I2C_OP_RETRIES);
    if(unlikely(ret)){
        LOG("%s %s %s %d\n", SENSOR_STR, __func__, ERROR_LINE, __LINE__);
        return ret;
    }

    return ret;
}

static int drv_acc_st_lsm303agr_st_data(i2c_dev_t* drv,int32_t* data)
{
    uint8_t i, j;
    int16_t x_raw, y_raw, z_raw;
    int32_t x_mg, y_mg, z_mg;
    int32_t x_sum, y_sum, z_sum;
    uint8_t value = 0x00;
    int ret = 0;
    uint8_t buffer[6];

    x_sum = 0; y_sum = 0; z_sum = 0;
    
    for (i = 0; i < LSM303AGR_ACC_SELF_TEST_AVG_SAMPLE_CNT; i ++) {
        for (j = 0; j < LSM303AGR_ACC_SELF_TEST_DRY_WAIT_CNT; j ++) {
            ret = sensor_i2c_read(drv, LSM303AGR_ACC_STATUS_REG, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
            if(unlikely(ret)){
                LOG("%s %s %s %d\n", SENSOR_STR, __func__, ERROR_LINE, __LINE__);
                return ret;
            }
            if (value & 0x08)
                break;
        
            aos_msleep(20);
        }

        if (j >= LSM303AGR_ACC_SELF_TEST_DRY_WAIT_CNT) {
            LOG("%s %s %s %d\n", SENSOR_STR, __func__, ERROR_LINE, __LINE__);
            return -1;
        }

        ret = sensor_i2c_read(drv,  (LSM303AGR_ACC_OUT_X_L | 0x80), buffer, 6, I2C_OP_RETRIES);
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

    data[0] = x_sum / LSM303AGR_ACC_SELF_TEST_AVG_SAMPLE_CNT;
    data[1] = y_sum / LSM303AGR_ACC_SELF_TEST_AVG_SAMPLE_CNT;
    data[2] = z_sum / LSM303AGR_ACC_SELF_TEST_AVG_SAMPLE_CNT;
    return ret;
}

static int drv_acc_st_lsm303agr_self_test(i2c_dev_t* drv,int32_t* data)
{
    uint8_t i;
    uint8_t value = 0x00;
    int ret = 0;
    uint8_t ctrl_reg[4];
    int32_t out_nost[3];
    int32_t out_st[3];
    int32_t out_diff[3];

    // Save cfg registers which will be modified during self-test
    ret = sensor_i2c_read(drv, (LSM303AGR_ACC_CTRL_REG1 | 0x80), ctrl_reg, 4, I2C_OP_RETRIES);
    if(unlikely(ret)){
        LOG("%s %s %s %d\n", SENSOR_STR, __func__, ERROR_LINE, __LINE__);
        return ret;
    }

    // Initialize Sensor, turn on sensor, enable X/Y/Z axes 
    // Set BDU=1, FS=2G, Normal mode, ODR = 50Hz
    value = 0;
    ret = sensor_i2c_write(drv, LSM303AGR_ACC_CTRL_REG2, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        LOG("%s %s %s %d\n", SENSOR_STR, __func__, ERROR_LINE, __LINE__);
        goto restore;
    }
    value = 0;
    ret = sensor_i2c_write(drv, LSM303AGR_ACC_CTRL_REG3, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        LOG("%s %s %s %d\n", SENSOR_STR, __func__, ERROR_LINE, __LINE__);
        goto restore;
    }
    
    value = 0x80;
    ret = sensor_i2c_write(drv, LSM303AGR_ACC_CTRL_REG4, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        LOG("%s %s %s %d\n", SENSOR_STR, __func__, ERROR_LINE, __LINE__);
        goto restore;
    }

    value = 0x47;
    ret = sensor_i2c_write(drv, LSM303AGR_ACC_CTRL_REG1, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        LOG("%s %s %s %d\n", SENSOR_STR, __func__, ERROR_LINE, __LINE__);
        goto restore;
    }

    aos_msleep(90);

    // Discard the first sample
    ret = drv_acc_st_lsm303agr_st_discard(drv);
    if(unlikely(ret)){
        LOG("%s %s %s %d\n", SENSOR_STR, __func__, ERROR_LINE, __LINE__);
        goto restore;
    }

    // Read some samples, and averate them
    ret = drv_acc_st_lsm303agr_st_data(drv, out_nost);
    if(unlikely(ret)){
        LOG("%s %s %s %d\n", SENSOR_STR, __func__, ERROR_LINE, __LINE__);
        goto restore;
    }

    // Enable seft-test
    value = 0x82;
    ret = sensor_i2c_write(drv, LSM303AGR_ACC_CTRL_REG4, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        LOG("%s %s %s %d\n", SENSOR_STR, __func__, ERROR_LINE, __LINE__);
        goto restore;
    }
    aos_msleep(90);
    
    // Discard the first sample
    ret = drv_acc_st_lsm303agr_st_discard(drv);
    if(unlikely(ret)){
        LOG("%s %s %s %d\n", SENSOR_STR, __func__, ERROR_LINE, __LINE__);
        goto restore;
    }

    // Read some samples, and average them
    ret = drv_acc_st_lsm303agr_st_data(drv, out_st);
    if(unlikely(ret)){
        LOG("%s %s %s %d\n", SENSOR_STR, __func__, ERROR_LINE, __LINE__);
        goto restore;
    }

    // Check if the differences are between min and max
    for (i = 0; i < 3; i ++) {
        out_diff[i] = abs(out_st[i] - out_nost[i]);
        data[i] = out_diff[i];
    }
    
    if ((LSM303AGR_ACC_SELF_TEST_MIN_X > out_diff[0]) || (out_diff[0] > LSM303AGR_ACC_SELF_TEST_MAX_X)) {
        LOG("%s %s %s %d\n", SENSOR_STR, __func__, ERROR_LINE, __LINE__);
        ret = -1;
        goto restore;
    }
    if ((LSM303AGR_ACC_SELF_TEST_MIN_Y > out_diff[1]) || (out_diff[1] > LSM303AGR_ACC_SELF_TEST_MAX_Y)) {
        LOG("%s %s %s %d\n", SENSOR_STR, __func__, ERROR_LINE, __LINE__);
        ret = -1;
        goto restore;
    }
    if ((LSM303AGR_ACC_SELF_TEST_MIN_Z > out_diff[2]) || (out_diff[2] > LSM303AGR_ACC_SELF_TEST_MAX_Z)) {
        LOG("%s %s %s %d\n", SENSOR_STR, __func__, ERROR_LINE, __LINE__);
        ret = -1;
        goto restore;
    }

restore:
    ret += sensor_i2c_write(drv, LSM303AGR_ACC_CTRL_REG1 | 0x80, ctrl_reg, 4, I2C_OP_RETRIES);
    return ret;
}

static void drv_acc_st_lsm303agr_irq_handle(void)
{
    /* no handle so far */
}

static int drv_acc_st_lsm303agr_open(void)
{
    int ret = 0;
    ret  = drv_acc_st_lsm303agr_set_power_mode(&lsm303agr_acc_ctx, DEV_POWER_ON);
    if(unlikely(ret)){
        return -1;
    }

    ret = drv_acc_st_lsm303agr_set_range(&lsm303agr_acc_ctx, ACC_RANGE_8G);
    if(unlikely(ret)){
        return -1;
    }

    ret = drv_acc_st_lsm303agr_set_odr(&lsm303agr_acc_ctx, LSM303AGR_ACC_DEFAULT_ODR_100HZ);
    if(unlikely(ret)){
        return -1;
    }
     
    return 0;

}

static int drv_acc_st_lsm303agr_close(void)
{
    int ret = 0;
    ret  = drv_acc_st_lsm303agr_set_power_mode(&lsm303agr_acc_ctx, DEV_POWER_OFF);
    if(unlikely(ret)){
        return -1;
    }
    return 0;
}

static int drv_acc_st_lsm303agr_read(void *buf, size_t len)
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
	
	ret = sensor_i2c_read(&lsm303agr_acc_ctx, (LSM303AGR_ACC_OUT_X_L | 0x80), reg, 6, I2C_OP_RETRIES);
	
	if(unlikely(ret)){
        return -1;
    }
    accel->data[DATA_AXIS_X] = (int16_t)((((int16_t)((int8_t)reg[1]))<< LSM303AGR_ACC_SHIFT_EIGHT_BITS)|(reg[0]));
	accel->data[DATA_AXIS_X] = accel->data[DATA_AXIS_X] >> LSM303AGR_ACC_SHIFT_FOUR_BITS;
    accel->data[DATA_AXIS_Y] = (int16_t)((((int16_t)((int8_t)reg[3]))<< LSM303AGR_ACC_SHIFT_EIGHT_BITS)|(reg[2]));
	accel->data[DATA_AXIS_Y] = accel->data[DATA_AXIS_Y] >> LSM303AGR_ACC_SHIFT_FOUR_BITS;
    accel->data[DATA_AXIS_Z] = (int16_t)((((int16_t)((int8_t)reg[5]))<< LSM303AGR_ACC_SHIFT_EIGHT_BITS)|(reg[4]));
	accel->data[DATA_AXIS_Z] = accel->data[DATA_AXIS_Z] >> LSM303AGR_ACC_SHIFT_FOUR_BITS;

    if(cur_acc_factor != 0){
        accel->data[DATA_AXIS_X] = accel->data[DATA_AXIS_X] * cur_acc_factor;
        accel->data[DATA_AXIS_Y] = accel->data[DATA_AXIS_Y] * cur_acc_factor;
        accel->data[DATA_AXIS_Z] = accel->data[DATA_AXIS_Z] * cur_acc_factor;
    }
    
    accel->timestamp = aos_now_ms();

    return (int)size;
}


static int drv_acc_st_lsm303agr_ioctl(int cmd, unsigned long arg)
{
    int ret = 0;
    dev_sensor_info_t *info = (dev_sensor_info_t *)arg;
    
    switch(cmd){
        case SENSOR_IOCTL_ODR_SET:{
            ret = drv_acc_st_lsm303agr_set_odr(&lsm303agr_acc_ctx, arg);
            if(unlikely(ret)){
                return -1;
            }
        }break;
        case SENSOR_IOCTL_RANGE_SET:{
            ret = drv_acc_st_lsm303agr_set_range(&lsm303agr_acc_ctx, arg);
            if(unlikely(ret)){
                return -1;
            }
        }break;
        case SENSOR_IOCTL_SET_POWER:{
            ret = drv_acc_st_lsm303agr_set_power_mode(&lsm303agr_acc_ctx, arg);
            if(unlikely(ret)){
                return -1;
            }
        }break;
        case SENSOR_IOCTL_GET_INFO:{ 
            /* fill the dev info here */
            info->model = "LSM303AGR_ACC";
            info->range_max = 16;
            info->range_min = 2;
            info->unit = mg;
			
        }break;
	   	case SENSOR_IOCTL_SELF_TEST:{
		   ret = drv_acc_st_lsm303agr_self_test(&lsm303agr_acc_ctx, (int32_t*)info->data);
		   //printf("%d	%d	 %d\n",info->data[0],info->data[1],info->data[2]);
           LOG("%s %s: %d, %d, %d\n", SENSOR_STR, __func__, info->data[0],info->data[1],info->data[2]);
		   return ret;
	   }
       default:break;
    }

    return 0;
}

int drv_acc_st_lsm303agr_init(void){
    int ret = 0;
    sensor_obj_t sensor;
    memset(&sensor, 0, sizeof(sensor));

    /* fill the sensor obj parameters here */
    sensor.io_port    = I2C_PORT;
    sensor.tag        = TAG_DEV_ACC;
    sensor.path       = dev_acc_path;
    sensor.open       = drv_acc_st_lsm303agr_open;
    sensor.close      = drv_acc_st_lsm303agr_close;
    sensor.read       = drv_acc_st_lsm303agr_read;
    sensor.write      = NULL;
    sensor.ioctl      = drv_acc_st_lsm303agr_ioctl;
    sensor.irq_handle = drv_acc_st_lsm303agr_irq_handle;

    ret = sensor_create_obj(&sensor);
    if(unlikely(ret)){
        return -1;
    }

    ret = drv_acc_st_lsm303agr_validate_id(&lsm303agr_acc_ctx, LSM303AGR_ACC_CHIP_ID_VALUE);
    if(unlikely(ret)){
        return -1;
    }

    ret = drv_acc_st_lsm303agr_set_range(&lsm303agr_acc_ctx, ACC_RANGE_8G);
    if(unlikely(ret)){
        return -1;
    }

    //set odr is 100hz, and will update
    ret = drv_acc_st_lsm303agr_set_odr(&lsm303agr_acc_ctx, LSM303AGR_ACC_DEFAULT_ODR_100HZ);
    if(unlikely(ret)){
        return -1;
    }

	//set bdu
    ret = drv_acc_st_lsm303agr_set_bdu(&lsm303agr_acc_ctx);
    if(unlikely(ret)){
        return -1;
    }

    /* update the phy sensor info to sensor hal */
    LOG("%s %s successfully \n", SENSOR_STR, __func__);
    return 0;
}

static int drv_mag_st_lsm303agr_soft_reset(i2c_dev_t* drv)
{
    int ret = 0;
    uint8_t value = LSM303AGR_MAG_SOFT_RESET_ENABLE;
    //ret = sensor_i2c_write(drv, LSM303AGR_MAG_CFG_REG_A, LSM303AGR_MAG_SOFT_RESET_ENABLE, I2C_DATA_LEN, I2C_OP_RETRIES);
    ret = sensor_i2c_write(drv, LSM303AGR_MAG_CFG_REG_A, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return -1;
    }
    return 0;
}

UNUSED static int drv_mag_st_lsm303agr_selftest(i2c_dev_t* drv)
{
    int ret = 0;
    uint8_t value = LSM303AGR_MAG_SELFTEST_ENABLE;
    //ret = sensor_i2c_write(drv, LSM303AGR_MAG_CFG_REG_C, LSM303AGR_MAG_SELFTEST_ENABLE, I2C_DATA_LEN, I2C_OP_RETRIES);
    ret = sensor_i2c_write(drv, LSM303AGR_MAG_CFG_REG_C, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return -1;
    }
    return 0;
}

UNUSED static int drv_mag_st_lsm303agr_reboot(i2c_dev_t* drv)
{
    int ret = 0;
    uint8_t value = LSM303AGR_MAG_REBOOT_ENABLE;
    //ret = sensor_i2c_write(drv, LSM303AGR_MAG_CFG_REG_A, LSM303AGR_MAG_REBOOT_ENABLE, I2C_DATA_LEN, I2C_OP_RETRIES);
    ret = sensor_i2c_write(drv, LSM303AGR_MAG_CFG_REG_A, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return -1;
    }
    return 0;
}

static int drv_mag_st_lsm303agr_validate_id(i2c_dev_t* drv, uint8_t id_value)
{
    uint8_t value = 0x00;
    int ret = 0;

    if(drv == NULL){
        return -1;
    }
    
    ret = sensor_i2c_read(drv, LSM303AGR_MAG_WHO_AM_I, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    LOG("%s %s right id (0x%02x), read id(0x%02x)\n", SENSOR_STR, __func__, id_value, value);
    if(unlikely(ret)){
        return ret;
    }
	
    if (id_value != value){
        return -1;
    }
    
    return 0;
}

static int drv_mag_st_lsm303agr_set_power_mode(i2c_dev_t* drv, dev_power_mode_e mode)
{
    uint8_t value = 0x00;
    int ret = 0;
    
    ret = sensor_i2c_read(drv, LSM303AGR_MAG_CFG_REG_A, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }

    if (mode == DEV_POWER_ON)
    {
        value &= ~LSM303AGR_MAG_POWERMODE_BIT;
        value |= LSM303AGR_MAG_CONTINUOUS_MODE;
    }
    else{
        value &= ~LSM303AGR_MAG_POWERMODE_BIT;
        value |= LSM303AGR_MAG_POWERDOWN2_MODE;
    }
    
    ret = sensor_i2c_write(drv, LSM303AGR_MAG_CFG_REG_A, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }

    return 0;
}

static int drv_mag_st_lsm303agr_set_odr(i2c_dev_t* drv, uint8_t odr)
{
    uint8_t value = 0x00;
    int ret = 0;
    
    ret = sensor_i2c_read(drv, LSM303AGR_MAG_CFG_REG_A, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }
    value &= ~LSM303AGR_MAG_ODR_BIT;
    value |= (uint8_t)odr;
	
    ret = sensor_i2c_write(drv, LSM303AGR_MAG_CFG_REG_A, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }
    return 0;
}

static int drv_mag_st_lsm303agr_enable_off_canc(i2c_dev_t* drv)
{
    uint8_t value = 0x00;
    int ret = 0;
    
    ret = sensor_i2c_read(drv, LSM303AGR_MAG_CFG_REG_B, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }
    value |= LSM303AGR_MAG_OFF_CANC;
    ret = sensor_i2c_write(drv, LSM303AGR_MAG_CFG_REG_B, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }
    return 0;
}

static int drv_mag_st_lsm303agr_enable_temp(i2c_dev_t* drv)
{
    uint8_t value = 0x00;
    int ret = 0;
    
    ret = sensor_i2c_read(drv, LSM303AGR_MAG_CFG_REG_A, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }
    value |= LSM303AGR_MAG_COMP_TEMP_EN;
    ret = sensor_i2c_write(drv, LSM303AGR_MAG_CFG_REG_A, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }
    return 0;
}

UNUSED static int drv_mag_st_lsm303agr_lowpower_mode(i2c_dev_t* drv, uint8_t lowpower_mode)
{
    int ret = 0;
    uint8_t value = 0x00;
    
    ret = sensor_i2c_read(drv, LSM303AGR_MAG_CFG_REG_A, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }
    
    if (lowpower_mode == LSM303AGR_MAG_CONFIG_LOWPOWER_MODE)
    {
        value &= ~LSM303AGR_MAG_CONFIG_LOWPOWER_BIT;
        value |= LSM303AGR_MAG_CONFIG_LOWPOWER_MODE;
    }
    else{
        value &= ~LSM303AGR_MAG_CONFIG_LOWPOWER_BIT;
        value |= LSM303AGR_MAG_CONFIG_NORMAL_MODE;
    }

    ret = sensor_i2c_write(drv, LSM303AGR_MAG_CFG_REG_A, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }
    
    return 0;
}

static int drv_mag_st_lsm303agr_set_range(i2c_dev_t* drv, uint32_t range)
{
	// default FS +/-50Gauss
    return 0;
}

static int drv_mag_st_lsm303agr_set_ble(i2c_dev_t* drv, uint8_t ble)
{
    uint8_t value = 0x00;
    int ret = 0;
    
    ret = sensor_i2c_read(drv, LSM303AGR_MAG_CFG_REG_C, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }
     
    if (ble == LSM303AGR_MAG_BLE_LSB)
    {
        value &= ~LSM303AGR_MAG_BLE_BIT;
        value |= LSM303AGR_MAG_BLE_LSB;
    }
    else{
        value &= ~LSM303AGR_MAG_BLE_BIT;
        value |= LSM303AGR_MAG_BLE_MSB;
    }
    
    ret = sensor_i2c_write(drv, LSM303AGR_MAG_CFG_REG_C, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }
    return 0;
}

static int drv_mag_st_lsm303agr_set_bdu(i2c_dev_t* drv, uint8_t bdu)
{
    uint8_t value = 0x00;
    int ret = 0;
    
    ret = sensor_i2c_read(drv, LSM303AGR_MAG_CFG_REG_C, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }
 
    if (bdu == LSM303AGR_MAG_BDU_CONTINUOUS)
    {
        value &= ~LSM303AGR_MAG_BDU_BIT;
        value |= LSM303AGR_MAG_BDU_CONTINUOUS;
    }
    else{
        value &= ~LSM303AGR_MAG_BDU_BIT;
        value |= LSM303AGR_MAG_BDU_MSBLSB;
    }
    
    ret = sensor_i2c_write(drv, LSM303AGR_MAG_CFG_REG_C, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }
    return 0;
}

static int drv_mag_st_lsm303agr_set_default_config(i2c_dev_t* drv)
{
    int ret = 0;
    ret = drv_mag_st_lsm303agr_set_power_mode(drv, DEV_POWER_OFF);
    if(unlikely(ret)){
        return ret;
    }
    ret = drv_mag_st_lsm303agr_set_odr(drv, LSM303AGR_MAG_ODR_10_HZ);
    if(unlikely(ret)){
        return ret;
    }
    ret = drv_mag_st_lsm303agr_set_range(drv, LSM303AGR_MAG_FS_50_GA);
    if(unlikely(ret)){
        return -1;
    }
    ret = drv_mag_st_lsm303agr_set_ble(drv, LSM303AGR_MAG_BLE_LSB);
    if(unlikely(ret)){
        return -1;
    }
    ret = drv_mag_st_lsm303agr_set_bdu(drv, LSM303AGR_MAG_BDU_MSBLSB);
    if(unlikely(ret)){
        return -1;
    }
    
    return 0;
}

static int drv_mag_st_lsm303agr_st_discard(i2c_dev_t* drv)
{
    uint8_t i;
    uint8_t value = 0x00;
    int ret = 0;
    uint8_t buffer[6];

    for (i = 0; i < LSM303AGR_MAG_SELF_TEST_DRY_WAIT_CNT; i ++) {
        ret = sensor_i2c_read(drv, LSM303AGR_MAG_STATUS_REG, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
        if(unlikely(ret)){
            LOG("%s %s %s %d\n", SENSOR_STR, __func__, ERROR_LINE, __LINE__);
            return ret;
        }
        if (value & 0x08)
            break;

        aos_msleep(10);
    }

    if (i >= LSM303AGR_MAG_SELF_TEST_DRY_WAIT_CNT) {
        LOG("%s %s %s %d\n", SENSOR_STR, __func__, ERROR_LINE, __LINE__);
        return -1;
    }

    ret = sensor_i2c_read(drv, (LSM303AGR_MAG_OUTX_L_REG | 0x80), buffer, 6, I2C_OP_RETRIES);
    if(unlikely(ret)){
        LOG("%s %s %s %d\n", SENSOR_STR, __func__, ERROR_LINE, __LINE__);
        return ret;
    }

    return ret;
}

static int drv_mag_st_lsm303agr_st_data(i2c_dev_t* drv,int32_t* data)
{
    uint8_t i, j;
    int16_t x_raw, y_raw, z_raw;
    int32_t x_mg, y_mg, z_mg;
    int32_t x_sum, y_sum, z_sum;
    uint8_t value = 0x00;
    int ret = 0;
    uint8_t buffer[6];

    x_sum = 0; y_sum = 0; z_sum = 0;
    
    for (i = 0; i < LSM303AGR_MAG_SELF_TEST_AVG_SAMPLE_CNT; i ++) {
        for (j = 0; j < LSM303AGR_MAG_SELF_TEST_DRY_WAIT_CNT; j ++) {
            ret = sensor_i2c_read(drv, LSM303AGR_MAG_STATUS_REG, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
            if(unlikely(ret)){
                LOG("%s %s %s %d\n", SENSOR_STR, __func__, ERROR_LINE, __LINE__);
                return ret;
            }
            if (value & 0x08)
                break;
        
            aos_msleep(10);
        }

        if (j >= LSM303AGR_MAG_SELF_TEST_DRY_WAIT_CNT) {
            LOG("%s %s %s %d\n", SENSOR_STR, __func__, ERROR_LINE, __LINE__);
            return -1;
        }

        ret = sensor_i2c_read(drv,  (LSM303AGR_MAG_OUTX_L_REG | 0x80), buffer, 6, I2C_OP_RETRIES);
        if(unlikely(ret)){
            LOG("%s %s %s %d\n", SENSOR_STR, __func__, ERROR_LINE, __LINE__);
            return ret;
        }

        x_raw = (buffer[1] << 8) + buffer[0];
        y_raw = (buffer[3] << 8) + buffer[2];
        z_raw = (buffer[5] << 8) + buffer[4];

        x_mg = x_raw * LSM303AGR_MAG_SENSITIVITY_FOR_FS_50GA / 1000;
        y_mg = y_raw * LSM303AGR_MAG_SENSITIVITY_FOR_FS_50GA / 1000;
        z_mg = z_raw * LSM303AGR_MAG_SENSITIVITY_FOR_FS_50GA / 1000;

        x_sum += x_mg;
        y_sum += y_mg;
        z_sum += z_mg;
    }

    data[0] = x_sum / LSM303AGR_MAG_SELF_TEST_AVG_SAMPLE_CNT;
    data[1] = y_sum / LSM303AGR_MAG_SELF_TEST_AVG_SAMPLE_CNT;
    data[2] = z_sum / LSM303AGR_MAG_SELF_TEST_AVG_SAMPLE_CNT;
    return ret;
}

static int drv_mag_st_lsm303agr_self_test(i2c_dev_t* drv,int32_t* data)
{
    uint8_t i;
    uint8_t value = 0x00;
    int ret = 0;
    uint8_t cfg_reg[3];
    int32_t out_nost[3];
    int32_t out_st[3];
    int32_t out_diff[3];

    // Save cfg registers which will be modified during self-test
    ret = sensor_i2c_read(drv, (LSM303AGR_MAG_CFG_REG_A | 0x80), cfg_reg, 3, I2C_OP_RETRIES);
    if(unlikely(ret)){
        LOG("%s %s %s %d\n", SENSOR_STR, __func__, ERROR_LINE, __LINE__);
        return ret;
    }

    // Initialize Sensor, turn on sensor, COMP_TEMP_EN, BDU, Continueous-Measurement, 
    //  Enable offset concellation, ODR = 100Hz
    value = 0x8c;
    ret = sensor_i2c_write(drv, LSM303AGR_MAG_CFG_REG_A, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        LOG("%s %s %s %d\n", SENSOR_STR, __func__, ERROR_LINE, __LINE__);
        goto restore;
    }
    value = 0x02;
    ret = sensor_i2c_write(drv, LSM303AGR_MAG_CFG_REG_B, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        LOG("%s %s %s %d\n", SENSOR_STR, __func__, ERROR_LINE, __LINE__);
        goto restore;
    }
    
    value = 0x10;
    ret = sensor_i2c_write(drv, LSM303AGR_MAG_CFG_REG_C, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        LOG("%s %s %s %d\n", SENSOR_STR, __func__, ERROR_LINE, __LINE__);
        goto restore;
    }

    aos_msleep(20);

    // Discard the first sample
    ret = drv_mag_st_lsm303agr_st_discard(drv);
    if(unlikely(ret)){
        LOG("%s %s %s %d\n", SENSOR_STR, __func__, ERROR_LINE, __LINE__);
        goto restore;
    }

    // Read some samples, and averate them
    ret = drv_mag_st_lsm303agr_st_data(drv, out_nost);
    if(unlikely(ret)){
        LOG("%s %s %s %d\n", SENSOR_STR, __func__, ERROR_LINE, __LINE__);
        goto restore;
    }

    // Enable seft-test
    value = 0x12;
    ret = sensor_i2c_write(drv, LSM303AGR_MAG_CFG_REG_C, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        LOG("%s %s %s %d\n", SENSOR_STR, __func__, ERROR_LINE, __LINE__);
        goto restore;
    }
    aos_msleep(60);
    
    // Discard the first sample
    ret = drv_mag_st_lsm303agr_st_discard(drv);
    if(unlikely(ret)){
        LOG("%s %s %s %d\n", SENSOR_STR, __func__, ERROR_LINE, __LINE__);
        goto restore;
    }

    // Read some samples, and average them
    ret = drv_mag_st_lsm303agr_st_data(drv, out_st);
    if(unlikely(ret)){
        LOG("%s %s %s %d\n", SENSOR_STR, __func__, ERROR_LINE, __LINE__);
        goto restore;
    }

    // Check if the differences are between min and max
    for (i = 0; i < 3; i ++) {
        out_diff[i] = abs(out_st[i] - out_nost[i]);
        data[i] = out_diff[i];
    }
    
    if ((LSM303AGR_MAG_SELF_TEST_MIN_X > out_diff[0]) || (out_diff[0] > LSM303AGR_MAG_SELF_TEST_MAX_X)) {
        LOG("%s %s %s %d\n", SENSOR_STR, __func__, ERROR_LINE, __LINE__);
        ret = -1;
        goto restore;
    }
    if ((LSM303AGR_MAG_SELF_TEST_MIN_Y > out_diff[1]) || (out_diff[1] > LSM303AGR_MAG_SELF_TEST_MAX_Y)) {
        LOG("%s %s %s %d\n", SENSOR_STR, __func__, ERROR_LINE, __LINE__);
        ret = -1;
        goto restore;
    }
    if ((LSM303AGR_MAG_SELF_TEST_MIN_Z > out_diff[2]) || (out_diff[2] > LSM303AGR_MAG_SELF_TEST_MAX_Z)) {
        LOG("%s %s %s %d\n", SENSOR_STR, __func__, ERROR_LINE, __LINE__);
        ret = -1;
        goto restore;
    }

restore:
    ret += sensor_i2c_write(drv, LSM303AGR_MAG_CFG_REG_A | 0x80, cfg_reg, 3, I2C_OP_RETRIES);
    return ret;
}

static void drv_mag_st_lsm303agr_irq_handle(void)
{
    /* no handle so far */
}

static int drv_mag_st_lsm303agr_open(void)
{
    int ret = 0;
    ret  = drv_mag_st_lsm303agr_set_power_mode(&lsm303agr_mag_ctx, DEV_POWER_ON);
    if(unlikely(ret)){
        return -1;
    }
    return 0;

}

static int drv_mag_st_lsm303agr_close(void)
{
    int ret = 0;
    ret  = drv_mag_st_lsm303agr_set_power_mode(&lsm303agr_mag_ctx, DEV_POWER_OFF);
    if(unlikely(ret)){
        return -1;
    }
    return 0;
}

static int drv_mag_st_lsm303agr_read(void* buf, size_t len)
{
  int ret = 0;
  size_t size;
  int16_t pnRawData[3];
  uint8_t buffer[6];
  uint8_t i = 0;
  uint16_t sensitivity = 0;
  
  mag_data_t* pdata = (mag_data_t*)buf;
    if(buf == NULL){
        LOG("%s %s %s %d\n", SENSOR_STR, __func__, ERROR_LINE, __LINE__);
        return -1;
    }

    size = sizeof(mag_data_t);
    if(len < size){
        LOG("%s %s %s %d\n", SENSOR_STR, __func__, ERROR_LINE, __LINE__);
        return -1;
    }
	
  ret = sensor_i2c_read(&lsm303agr_mag_ctx, (LSM303AGR_MAG_OUTX_L_REG | 0x80), buffer, 6, I2C_OP_RETRIES);
  if(unlikely(ret)){
      return -1;
  }
  for(i=0; i<3; i++)
  {
    pnRawData[i]=((((uint16_t)buffer[2*i+1]) << 8) | (uint16_t)buffer[2*i]);
  }
  //LOG("%s %s: %d, %d, %d \n", SENSOR_STR, __func__, pnRawData[0], pnRawData[1], pnRawData[2]);
  
  sensitivity = LSM303AGR_MAG_SENSITIVITY_FOR_FS_50GA;  
  
  for(i=0; i<3; i++)
  {
    //pdata->data[i] = ( int16_t )(pnRawData[i] * sensitivity);
    pdata->data[i] = pnRawData[i] * sensitivity / 1000;
  }
  pdata->timestamp = aos_now_ms();

  return (int)size;
}


static int drv_mag_st_lsm303agr_ioctl(int cmd, unsigned long arg)
{
    int ret = 0;
    dev_sensor_info_t *info = (dev_sensor_info_t *)arg;
    
    switch(cmd){
        case SENSOR_IOCTL_ODR_SET:{
            ret = drv_mag_st_lsm303agr_set_odr(&lsm303agr_mag_ctx, arg);
            if(unlikely(ret)){
                return -1;
            }
        }break;
        case SENSOR_IOCTL_RANGE_SET:{
            ret = drv_mag_st_lsm303agr_set_range(&lsm303agr_mag_ctx, arg);
            if(unlikely(ret)){
                return -1;
            }
        }break;
        case SENSOR_IOCTL_SET_POWER:{
            ret = drv_mag_st_lsm303agr_set_power_mode(&lsm303agr_mag_ctx, arg);
            if(unlikely(ret)){
                return -1;
            }
        }break;
        case SENSOR_IOCTL_GET_INFO:{ 
            /* fill the dev info here */
            //dev_sensor_info_t *info = (dev_sensor_info_t *)arg;
            info->model = "LSM303AGR";
            info->range_max = 50;
            info->range_min = 50;
            info->unit = mGauss;
        }break;
       
       case SENSOR_IOCTL_SELF_TEST:{
           ret = drv_mag_st_lsm303agr_self_test(&lsm303agr_mag_ctx, (int32_t*)info->data);
           //printf("%d   %d   %d\n",info->data[0],info->data[1],info->data[2]);
           LOG("%s %s: %d, %d, %d\n", SENSOR_STR, __func__, info->data[0],info->data[1],info->data[2]);
           return ret;
       }
       
       default:break;
    }

    return 0;
}

int drv_mag_st_lsm303agr_init(void){
    int ret = 0;
    sensor_obj_t sensor;
    memset(&sensor, 0, sizeof(sensor));
    /* fill the sensor obj parameters here */
    sensor.io_port    = I2C_PORT;
    sensor.tag        = TAG_DEV_MAG;
    sensor.path       = dev_mag_path;
    sensor.open       = drv_mag_st_lsm303agr_open;
    sensor.close      = drv_mag_st_lsm303agr_close;
    sensor.read       = drv_mag_st_lsm303agr_read;
    sensor.write      = NULL;
    sensor.ioctl      = drv_mag_st_lsm303agr_ioctl;
    sensor.irq_handle = drv_mag_st_lsm303agr_irq_handle;

    ret = sensor_create_obj(&sensor);
    if(unlikely(ret)){
        LOG("%s %s %s %d\n", SENSOR_STR, __func__, ERROR_LINE, __LINE__);
        return -1;
    }
	
    ret = drv_mag_st_lsm303agr_validate_id(&lsm303agr_mag_ctx, I_AM_LSM303AGR);
    if(unlikely(ret)){
        LOG("%s %s %s %d\n", SENSOR_STR, __func__, ERROR_LINE, __LINE__);
        return -1;
    }
    
    ret = drv_mag_st_lsm303agr_soft_reset(&lsm303agr_mag_ctx);
    if(unlikely(ret)){
        LOG("%s %s %s %d\n", SENSOR_STR, __func__, ERROR_LINE, __LINE__);
        return -1;
    }
    
    ret = drv_mag_st_lsm303agr_set_default_config(&lsm303agr_mag_ctx);
    if(unlikely(ret)){
        LOG("%s %s %s %d\n", SENSOR_STR, __func__, ERROR_LINE, __LINE__);
        return -1;
    }
    
    ret = drv_mag_st_lsm303agr_enable_temp(&lsm303agr_mag_ctx);
    if(unlikely(ret)){
        LOG("%s %s %s %d\n", SENSOR_STR, __func__, ERROR_LINE, __LINE__);
        return -1;
    }
	
    ret = drv_mag_st_lsm303agr_enable_off_canc(&lsm303agr_mag_ctx);
    if(unlikely(ret)){
        LOG("%s %s %s %d\n", SENSOR_STR, __func__, ERROR_LINE, __LINE__);
        return -1;
    }
    /* update the phy sensor info to sensor hal */
    LOG("%s %s successfully \n", SENSOR_STR, __func__);
    return 0;
}


SENSOR_DRV_ADD(drv_acc_st_lsm303agr_init);
SENSOR_DRV_ADD(drv_mag_st_lsm303agr_init);


