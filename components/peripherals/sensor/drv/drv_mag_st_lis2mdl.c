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


#define LIS2MDL_I2C_ADDR1                   0x1E
#define LIS2MDL_I2C_ADDR_TRANS(n)           ((n)<<1)  
#define LIS2MDL_I2C_ADDR                    LIS2MDL_I2C_ADDR_TRANS(LIS2MDL_I2C_ADDR1)
#define LIS2MDL_BIT(x)                      (x)

#define LIS2MDL_MAG_OFFSET_X_REG_L          0x45
#define LIS2MDL_MAG_OFFSET_X_REG_H          0x46
#define LIS2MDL_MAG_OFFSET_Y_REG_L          0x47
#define LIS2MDL_MAG_OFFSET_Y_REG_H          0x48
#define LIS2MDL_MAG_OFFSET_Z_REG_L          0x49
#define LIS2MDL_MAG_OFFSET_Z_REG_H          0x4A
#define LIS2MDL_MAG_WHO_AM_I                0x4F
#define LIS2MDL_MAG_CFG_REG_A               0x60
#define LIS2MDL_MAG_CFG_REG_B               0x61
#define LIS2MDL_MAG_CFG_REG_C               0x62
#define LIS2MDL_MAG_INT_CRTL_REG            0x63
#define LIS2MDL_MAG_INT_SOURCE_REG          0x64
#define LIS2MDL_MAG_INT_THS_L_REG           0x65
#define LIS2MDL_MAG_INT_THS_H_REG           0x66
#define LIS2MDL_MAG_STATUS_REG              0x67
#define LIS2MDL_MAG_OUTX_L_REG              0x68
#define LIS2MDL_MAG_OUTX_H_REG              0x69
#define LIS2MDL_MAG_OUTY_L_REG              0x6A
#define LIS2MDL_MAG_OUTY_H_REG              0x6B
#define LIS2MDL_MAG_OUTZ_L_REG              0x6C
#define LIS2MDL_MAG_OUTZ_H_REG              0x6D
#define LIS2MDL_MAG_TEMP_OUT_L_REG          0x6E
#define LIS2MDL_MAG_TEMP_OUT_H_REG          0x6F

#define I_AM_LIS2MDL                        0x40

#define LIS2MDL_MAG_ODR_BIT                 LIS2MDL_BIT(0x0C)
#define LIS2MDL_MAG_ODR_10_HZ               0x00 
#define LIS2MDL_MAG_ODR_20_HZ               0x04
#define LIS2MDL_MAG_ODR_50_HZ               0x08
#define LIS2MDL_MAG_ODR_100_HZ              0x0C  

#define LIS2MDL_MAG_SELFTEST_DISABLE        0x00   
#define LIS2MDL_MAG_SELFTEST_ENABLE         0x02

#define LIS2MDL_MAG_FS_50_GA                0x00 

#define LIS2MDL_MAG_REBOOT_DEFAULT          0x00
#define LIS2MDL_MAG_REBOOT_ENABLE           0x40
   
#define LIS2MDL_MAG_SOFT_RESET_DEFAULT      0x00
#define LIS2MDL_MAG_SOFT_RESET_ENABLE       0x02
   
#define LIS2MDL_MAG_CONFIG_LOWPOWER_BIT     LIS2MDL_BIT(0x10)
#define LIS2MDL_MAG_CONFIG_NORMAL_MODE      0x00
#define LIS2MDL_MAG_CONFIG_LOWPOWER_MODE    0x10
   
#define LIS2MDL_MAG_POWERMODE_BIT           LIS2MDL_BIT(0x03)      
#define LIS2MDL_MAG_CONTINUOUS_MODE         0x00
#define LIS2MDL_MAG_SINGLE_MODE             0x01
#define LIS2MDL_MAG_POWERDOWN1_MODE         0x02
#define LIS2MDL_MAG_POWERDOWN2_MODE         0x03

#define LIS2MDL_MAG_COMP_TEMP_EN       		0x80

#define LIS2MDL_MAG_OFF_CANC           		0x02

#define LIS2MDL_MAG_BLE_BIT                 LIS2MDL_BIT(0x08)
#define LIS2MDL_MAG_BLE_LSB                 0x00
#define LIS2MDL_MAG_BLE_MSB                 0x08

#define LIS2MDL_MAG_BDU_BIT                 LIS2MDL_BIT(0x10)
#define LIS2MDL_MAG_BDU_CONTINUOUS          0x00
#define LIS2MDL_MAG_BDU_MSBLSB              0x10
   
//#define LIS2MDL_MAG_SENSITIVITY_FOR_FS_50GA   15/10  
#define LIS2MDL_MAG_SENSITIVITY_FOR_FS_50GA   1500  

#define LIS2MDL_MAG_SELF_TEST_MIN_X              (15) // 15mGuass
#define LIS2MDL_MAG_SELF_TEST_MIN_Y              (15) // 15mGuass    
#define LIS2MDL_MAG_SELF_TEST_MIN_Z              (15)  // 15mGuass
#define LIS2MDL_MAG_SELF_TEST_MAX_X              (500) // 500mGuass
#define LIS2MDL_MAG_SELF_TEST_MAX_Y              (500) // 500mGuass    
#define LIS2MDL_MAG_SELF_TEST_MAX_Z              (500)  // 500mGuass

#define LIS2MDL_MAG_SELF_TEST_DRY_WAIT_CNT  5
#define LIS2MDL_MAG_SELF_TEST_AVG_SAMPLE_CNT    50

i2c_dev_t LIS2MDL_ctx = {
    .port = 3,
    .config.address_width = 8,
    .config.freq = 400000,
    .config.dev_addr = LIS2MDL_I2C_ADDR,
};

UNUSED static int drv_mag_st_lis2mdl_soft_reset(i2c_dev_t* drv)
{
    int ret = 0;
    uint8_t value = LIS2MDL_MAG_SOFT_RESET_ENABLE;
    //ret = sensor_i2c_write(drv, LIS2MDL_MAG_CFG_REG_A, LIS2MDL_MAG_SOFT_RESET_ENABLE, I2C_DATA_LEN, I2C_OP_RETRIES);
    ret = sensor_i2c_write(drv, LIS2MDL_MAG_CFG_REG_A, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return -1;
    }
    return 0;
}

UNUSED static int drv_mag_st_lis2mdl_selftest(i2c_dev_t* drv)
{
    int ret = 0;
    uint8_t value = LIS2MDL_MAG_SELFTEST_ENABLE;
    //ret = sensor_i2c_write(drv, LIS2MDL_MAG_CFG_REG_C, LIS2MDL_MAG_SELFTEST_ENABLE, I2C_DATA_LEN, I2C_OP_RETRIES);
    ret = sensor_i2c_write(drv, LIS2MDL_MAG_CFG_REG_C, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return -1;
    }
    return 0;
}

UNUSED static int drv_mag_st_lis2mdl_reboot(i2c_dev_t* drv)
{
    int ret = 0;
    uint8_t value = LIS2MDL_MAG_REBOOT_ENABLE;
    //ret = sensor_i2c_write(drv, LIS2MDL_MAG_CFG_REG_A, LIS2MDL_MAG_REBOOT_ENABLE, I2C_DATA_LEN, I2C_OP_RETRIES);
    ret = sensor_i2c_write(drv, LIS2MDL_MAG_CFG_REG_A, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return -1;
    }
    return 0;
}

static int drv_mag_st_lis2mdl_validate_id(i2c_dev_t* drv, uint8_t id_value)
{
    uint8_t value = 0x00;
    int ret = 0;

    if(drv == NULL){
        return -1;
    }
    
    ret = sensor_i2c_read(drv, LIS2MDL_MAG_WHO_AM_I, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    LOG("%s %s right id (0x%02x), read id(0x%02x)\n", SENSOR_STR, __func__, id_value, value);
    if(unlikely(ret)){
        return ret;
    }
	
    if (id_value != value){
        return -1;
    }
    
    return 0;
}

static int drv_mag_st_lis2mdl_set_power_mode(i2c_dev_t* drv, dev_power_mode_e mode)
{
    uint8_t value = 0x00;
    int ret = 0;
    
    ret = sensor_i2c_read(drv, LIS2MDL_MAG_CFG_REG_A, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }

    if (mode == DEV_POWER_ON)
    {
        value &= ~LIS2MDL_MAG_POWERMODE_BIT;
        value |= LIS2MDL_MAG_CONTINUOUS_MODE;
    }
    else{
        value &= ~LIS2MDL_MAG_POWERMODE_BIT;
        value |= LIS2MDL_MAG_POWERDOWN2_MODE;
    }
    
    ret = sensor_i2c_write(drv, LIS2MDL_MAG_CFG_REG_A, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }

    return 0;
}

static int drv_mag_st_lis2mdl_set_odr(i2c_dev_t* drv, uint8_t odr)
{
    uint8_t value = 0x00;
    int ret = 0;
    
    ret = sensor_i2c_read(drv, LIS2MDL_MAG_CFG_REG_A, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }
    value &= ~LIS2MDL_MAG_ODR_BIT;
    value |= (uint8_t)odr;
	
    ret = sensor_i2c_write(drv, LIS2MDL_MAG_CFG_REG_A, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }
    return 0;
}

static int drv_mag_st_lis2mdl_enable_off_canc(i2c_dev_t* drv)
{
    uint8_t value = 0x00;
    int ret = 0;
    
    ret = sensor_i2c_read(drv, LIS2MDL_MAG_CFG_REG_B, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }
    value |= LIS2MDL_MAG_OFF_CANC;
    ret = sensor_i2c_write(drv, LIS2MDL_MAG_CFG_REG_B, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }
    return 0;
}

static int drv_mag_st_lis2mdl_enable_temp(i2c_dev_t* drv)
{
    uint8_t value = 0x00;
    int ret = 0;
    
    ret = sensor_i2c_read(drv, LIS2MDL_MAG_CFG_REG_A, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }
    value |= LIS2MDL_MAG_COMP_TEMP_EN;
    ret = sensor_i2c_write(drv, LIS2MDL_MAG_CFG_REG_A, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }
    return 0;
}

UNUSED static int drv_mag_st_lis2mdl_lowpower_mode(i2c_dev_t* drv, uint8_t lowpower_mode)
{
    int ret = 0;
    uint8_t value = 0x00;
    
    ret = sensor_i2c_read(drv, LIS2MDL_MAG_CFG_REG_A, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }
    
    if (lowpower_mode == LIS2MDL_MAG_CONFIG_LOWPOWER_MODE)
    {
        value &= ~LIS2MDL_MAG_CONFIG_LOWPOWER_BIT;
        value |= LIS2MDL_MAG_CONFIG_LOWPOWER_MODE;
    }
    else{
        value &= ~LIS2MDL_MAG_CONFIG_LOWPOWER_BIT;
        value |= LIS2MDL_MAG_CONFIG_NORMAL_MODE;
    }

    ret = sensor_i2c_write(drv, LIS2MDL_MAG_CFG_REG_A, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }
    
    return 0;
}

static int drv_mag_st_lis2mdl_set_range(i2c_dev_t* drv, uint32_t range)
{
	// default FS +/-50Gauss
    return 0;
}

static int drv_mag_st_lis2mdl_set_ble(i2c_dev_t* drv, uint8_t ble)
{
    uint8_t value = 0x00;
    int ret = 0;
    
    ret = sensor_i2c_read(drv, LIS2MDL_MAG_CFG_REG_C, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }
     
    if (ble == LIS2MDL_MAG_BLE_LSB)
    {
        value &= ~LIS2MDL_MAG_BLE_BIT;
        value |= LIS2MDL_MAG_BLE_LSB;
    }
    else{
        value &= ~LIS2MDL_MAG_BLE_BIT;
        value |= LIS2MDL_MAG_BLE_MSB;
    }
    
    ret = sensor_i2c_write(drv, LIS2MDL_MAG_CFG_REG_C, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }
    return 0;
}

static int drv_mag_st_lis2mdl_set_bdu(i2c_dev_t* drv, uint8_t bdu)
{
    uint8_t value = 0x00;
    int ret = 0;
    
    ret = sensor_i2c_read(drv, LIS2MDL_MAG_CFG_REG_C, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }
 
    if (bdu == LIS2MDL_MAG_BDU_CONTINUOUS)
    {
        value &= ~LIS2MDL_MAG_BDU_BIT;
        value |= LIS2MDL_MAG_BDU_CONTINUOUS;
    }
    else{
        value &= ~LIS2MDL_MAG_BDU_BIT;
        value |= LIS2MDL_MAG_BDU_MSBLSB;
    }
    
    ret = sensor_i2c_write(drv, LIS2MDL_MAG_CFG_REG_C, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        return ret;
    }
    return 0;
}

static int drv_mag_st_lis2mdl_set_default_config(i2c_dev_t* drv)
{
    int ret = 0;
    ret = drv_mag_st_lis2mdl_set_power_mode(drv, DEV_POWER_OFF);
    if(unlikely(ret)){
        return ret;
    }
    ret = drv_mag_st_lis2mdl_set_odr(drv, LIS2MDL_MAG_ODR_10_HZ);
    if(unlikely(ret)){
        return ret;
    }
    ret = drv_mag_st_lis2mdl_set_range(drv, LIS2MDL_MAG_FS_50_GA);
    if(unlikely(ret)){
        return -1;
    }
    ret = drv_mag_st_lis2mdl_set_ble(drv, LIS2MDL_MAG_BLE_LSB);
    if(unlikely(ret)){
        return -1;
    }
    ret = drv_mag_st_lis2mdl_set_bdu(drv, LIS2MDL_MAG_BDU_MSBLSB);
    if(unlikely(ret)){
        return -1;
    }
    
    return 0;
}

static int drv_mag_st_lis2mdl_st_discard(i2c_dev_t* drv)
{
    uint8_t i;
    uint8_t value = 0x00;
    int ret = 0;
    uint8_t buffer[6];

    for (i = 0; i < LIS2MDL_MAG_SELF_TEST_DRY_WAIT_CNT; i ++) {
        ret = sensor_i2c_read(drv, LIS2MDL_MAG_STATUS_REG, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
        if(unlikely(ret)){
            LOG("%s %s %s %d\n", SENSOR_STR, __func__, ERROR_LINE, __LINE__);
            return ret;
        }
        if (value & 0x08)
            break;

        aos_msleep(10);
    }

    if (i >= LIS2MDL_MAG_SELF_TEST_DRY_WAIT_CNT) {
        LOG("%s %s %s %d\n", SENSOR_STR, __func__, ERROR_LINE, __LINE__);
        return -1;
    }

    ret = sensor_i2c_read(drv, (LIS2MDL_MAG_OUTX_L_REG | 0x80), buffer, 6, I2C_OP_RETRIES);
    if(unlikely(ret)){
        LOG("%s %s %s %d\n", SENSOR_STR, __func__, ERROR_LINE, __LINE__);
        return ret;
    }

    return ret;
}

static int drv_mag_st_lis2mdl_st_data(i2c_dev_t* drv,int32_t* data)
{
    uint8_t i, j;
    int16_t x_raw, y_raw, z_raw;
    int32_t x_mg, y_mg, z_mg;
    int32_t x_sum, y_sum, z_sum;
    uint8_t value = 0x00;
    int ret = 0;
    uint8_t buffer[6];

    x_sum = 0; y_sum = 0; z_sum = 0;
    
    for (i = 0; i < LIS2MDL_MAG_SELF_TEST_AVG_SAMPLE_CNT; i ++) {
        for (j = 0; j < LIS2MDL_MAG_SELF_TEST_DRY_WAIT_CNT; j ++) {
            ret = sensor_i2c_read(drv, LIS2MDL_MAG_STATUS_REG, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
            if(unlikely(ret)){
                LOG("%s %s %s %d\n", SENSOR_STR, __func__, ERROR_LINE, __LINE__);
                return ret;
            }
            if (value & 0x08)
                break;
        
            aos_msleep(10);
        }

        if (j >= LIS2MDL_MAG_SELF_TEST_DRY_WAIT_CNT) {
            LOG("%s %s %s %d\n", SENSOR_STR, __func__, ERROR_LINE, __LINE__);
            return -1;
        }

        ret = sensor_i2c_read(drv,  (LIS2MDL_MAG_OUTX_L_REG | 0x80), buffer, 6, I2C_OP_RETRIES);
        if(unlikely(ret)){
            LOG("%s %s %s %d\n", SENSOR_STR, __func__, ERROR_LINE, __LINE__);
            return ret;
        }

        x_raw = (buffer[1] << 8) + buffer[0];
        y_raw = (buffer[3] << 8) + buffer[2];
        z_raw = (buffer[5] << 8) + buffer[4];

        x_mg = x_raw * LIS2MDL_MAG_SENSITIVITY_FOR_FS_50GA / 1000;
        y_mg = y_raw * LIS2MDL_MAG_SENSITIVITY_FOR_FS_50GA / 1000;
        z_mg = z_raw * LIS2MDL_MAG_SENSITIVITY_FOR_FS_50GA / 1000;

        x_sum += x_mg;
        y_sum += y_mg;
        z_sum += z_mg;
    }

    data[0] = x_sum / LIS2MDL_MAG_SELF_TEST_AVG_SAMPLE_CNT;
    data[1] = y_sum / LIS2MDL_MAG_SELF_TEST_AVG_SAMPLE_CNT;
    data[2] = z_sum / LIS2MDL_MAG_SELF_TEST_AVG_SAMPLE_CNT;
    return ret;
}

static int drv_mag_st_lis2mdl_self_test(i2c_dev_t* drv,int32_t* data)
{
    uint8_t i;
    uint8_t value = 0x00;
    int ret = 0;
    uint8_t cfg_reg[3];
    int32_t out_nost[3];
    int32_t out_st[3];
    int32_t out_diff[3];

    // Save cfg registers which will be modified during self-test
    ret = sensor_i2c_read(drv, (LIS2MDL_MAG_CFG_REG_A | 0x80), cfg_reg, 3, I2C_OP_RETRIES);
    if(unlikely(ret)){
        LOG("%s %s %s %d\n", SENSOR_STR, __func__, ERROR_LINE, __LINE__);
        return ret;
    }

    // Initialize Sensor, turn on sensor, COMP_TEMP_EN, BDU, Continueous-Measurement, 
    //  Enable offset concellation, ODR = 100Hz
    value = 0x8c;
    ret = sensor_i2c_write(drv, LIS2MDL_MAG_CFG_REG_A, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        LOG("%s %s %s %d\n", SENSOR_STR, __func__, ERROR_LINE, __LINE__);
        goto restore;
    }
    value = 0x02;
    ret = sensor_i2c_write(drv, LIS2MDL_MAG_CFG_REG_B, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        LOG("%s %s %s %d\n", SENSOR_STR, __func__, ERROR_LINE, __LINE__);
        goto restore;
    }
    
    value = 0x10;
    ret = sensor_i2c_write(drv, LIS2MDL_MAG_CFG_REG_C, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        LOG("%s %s %s %d\n", SENSOR_STR, __func__, ERROR_LINE, __LINE__);
        goto restore;
    }

    aos_msleep(20);

    // Discard the first sample
    ret = drv_mag_st_lis2mdl_st_discard(drv);
    if(unlikely(ret)){
        LOG("%s %s %s %d\n", SENSOR_STR, __func__, ERROR_LINE, __LINE__);
        goto restore;
    }

    // Read some samples, and averate them
    ret = drv_mag_st_lis2mdl_st_data(drv, out_nost);
    if(unlikely(ret)){
        LOG("%s %s %s %d\n", SENSOR_STR, __func__, ERROR_LINE, __LINE__);
        goto restore;
    }

    // Enable seft-test
    value = 0x12;
    ret = sensor_i2c_write(drv, LIS2MDL_MAG_CFG_REG_C, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)){
        LOG("%s %s %s %d\n", SENSOR_STR, __func__, ERROR_LINE, __LINE__);
        goto restore;
    }
    aos_msleep(60);
    
    // Discard the first sample
    ret = drv_mag_st_lis2mdl_st_discard(drv);
    if(unlikely(ret)){
        LOG("%s %s %s %d\n", SENSOR_STR, __func__, ERROR_LINE, __LINE__);
        goto restore;
    }

    // Read some samples, and average them
    ret = drv_mag_st_lis2mdl_st_data(drv, out_st);
    if(unlikely(ret)){
        LOG("%s %s %s %d\n", SENSOR_STR, __func__, ERROR_LINE, __LINE__);
        goto restore;
    }

    // Check if the differences are between min and max
    for (i = 0; i < 3; i ++) {
        out_diff[i] = abs(out_st[i] - out_nost[i]);
        data[i] = out_diff[i];
    }
    
    if ((LIS2MDL_MAG_SELF_TEST_MIN_X > out_diff[0]) || (out_diff[0] > LIS2MDL_MAG_SELF_TEST_MAX_X)) {
        LOG("%s %s %s %d\n", SENSOR_STR, __func__, ERROR_LINE, __LINE__);
        ret = -1;
        goto restore;
    }
    if ((LIS2MDL_MAG_SELF_TEST_MIN_Y > out_diff[1]) || (out_diff[1] > LIS2MDL_MAG_SELF_TEST_MAX_Y)) {
        LOG("%s %s %s %d\n", SENSOR_STR, __func__, ERROR_LINE, __LINE__);
        ret = -1;
        goto restore;
    }
    if ((LIS2MDL_MAG_SELF_TEST_MIN_Z > out_diff[2]) || (out_diff[2] > LIS2MDL_MAG_SELF_TEST_MAX_Z)) {
        LOG("%s %s %s %d\n", SENSOR_STR, __func__, ERROR_LINE, __LINE__);
        ret = -1;
        goto restore;
    }

restore:
    ret += sensor_i2c_write(drv, LIS2MDL_MAG_CFG_REG_A | 0x80, cfg_reg, 3, I2C_OP_RETRIES);
    return ret;
}

static void drv_mag_st_lis2mdl_irq_handle(void)
{
    /* no handle so far */
}

static int drv_mag_st_lis2mdl_open(void)
{
    int ret = 0;
    ret  = drv_mag_st_lis2mdl_set_power_mode(&LIS2MDL_ctx, DEV_POWER_ON);
    if(unlikely(ret)){
        return -1;
    }
    return 0;

}

static int drv_mag_st_lis2mdl_close(void)
{
    int ret = 0;
    ret  = drv_mag_st_lis2mdl_set_power_mode(&LIS2MDL_ctx, DEV_POWER_OFF);
    if(unlikely(ret)){
        return -1;
    }
    return 0;
}

static int drv_mag_st_lis2mdl_read(void* buf, size_t len)
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
	
  ret = sensor_i2c_read(&LIS2MDL_ctx, (LIS2MDL_MAG_OUTX_L_REG | 0x80), buffer, 6, I2C_OP_RETRIES);
  if(unlikely(ret)){
        return -1;
  }
  for(i=0; i<3; i++)
  {
    pnRawData[i]=((((uint16_t)buffer[2*i+1]) << 8) | (uint16_t)buffer[2*i]);
  }
  //LOG("%s %s: %d, %d, %d \n", SENSOR_STR, __func__, pnRawData[0], pnRawData[1], pnRawData[2]);
  
  sensitivity = LIS2MDL_MAG_SENSITIVITY_FOR_FS_50GA;  
  
  for(i=0; i<3; i++)
  {
    //pdata->data[i] = ( int16_t )(pnRawData[i] * sensitivity);
    pdata->data[i] = pnRawData[i] * sensitivity / 1000;
  }
  pdata->timestamp = aos_now_ms();

  return (int)size;
}


static int drv_mag_st_lis2mdl_ioctl(int cmd, unsigned long arg)
{
    int ret = 0;
    dev_sensor_info_t *info = (dev_sensor_info_t *)arg;
    
    switch(cmd){
        case SENSOR_IOCTL_ODR_SET:{
            ret = drv_mag_st_lis2mdl_set_odr(&LIS2MDL_ctx, arg);
            if(unlikely(ret)){
                return -1;
            }
        }break;
        case SENSOR_IOCTL_RANGE_SET:{
            ret = drv_mag_st_lis2mdl_set_range(&LIS2MDL_ctx, arg);
            if(unlikely(ret)){
                return -1;
            }
        }break;
        case SENSOR_IOCTL_SET_POWER:{
            ret = drv_mag_st_lis2mdl_set_power_mode(&LIS2MDL_ctx, arg);
            if(unlikely(ret)){
                return -1;
            }
        }break;
        case SENSOR_IOCTL_GET_INFO:{ 
            /* fill the dev info here */
            //dev_sensor_info_t *info = (dev_sensor_info_t *)arg;
            info->model = "LIS2MDL";
            info->range_max = 50;
            info->range_min = 50;
            info->unit = mGauss;
        }break;
       
       case SENSOR_IOCTL_SELF_TEST:{
           ret = drv_mag_st_lis2mdl_self_test(&LIS2MDL_ctx, (int32_t*)info->data);
           //printf("%d   %d   %d\n",info->data[0],info->data[1],info->data[2]);
           LOG("%s %s: %d, %d, %d\n", SENSOR_STR, __func__, info->data[0],info->data[1],info->data[2]);
           return ret;
       }
       
       default:break;
    }

    return 0;
}

int drv_mag_st_lis2mdl_init(void){
    int ret = 0;
    sensor_obj_t sensor;
    memset(&sensor, 0, sizeof(sensor));
    /* fill the sensor obj parameters here */
    sensor.io_port    = I2C_PORT;
    sensor.tag        = TAG_DEV_MAG;
    sensor.path       = dev_mag_path;
    sensor.open       = drv_mag_st_lis2mdl_open;
    sensor.close      = drv_mag_st_lis2mdl_close;
    sensor.read       = drv_mag_st_lis2mdl_read;
    sensor.write      = NULL;
    sensor.ioctl      = drv_mag_st_lis2mdl_ioctl;
    sensor.irq_handle = drv_mag_st_lis2mdl_irq_handle;

    ret = sensor_create_obj(&sensor);
    if(unlikely(ret)){
        LOG("%s %s %s %d\n", SENSOR_STR, __func__, ERROR_LINE, __LINE__);
        return -1;
    }
	
    ret = drv_mag_st_lis2mdl_validate_id(&LIS2MDL_ctx, I_AM_LIS2MDL);
    if(unlikely(ret)){
        LOG("%s %s %s %d\n", SENSOR_STR, __func__, ERROR_LINE, __LINE__);
        return -1;
    }
    
    ret = drv_mag_st_lis2mdl_soft_reset(&LIS2MDL_ctx);
    if(unlikely(ret)){
        LOG("%s %s %s %d\n", SENSOR_STR, __func__, ERROR_LINE, __LINE__);
        return -1;
    }
    
    ret = drv_mag_st_lis2mdl_set_default_config(&LIS2MDL_ctx);
    if(unlikely(ret)){
        LOG("%s %s %s %d\n", SENSOR_STR, __func__, ERROR_LINE, __LINE__);
        return -1;
    }
    
    ret = drv_mag_st_lis2mdl_enable_temp(&LIS2MDL_ctx);
    if(unlikely(ret)){
        LOG("%s %s %s %d\n", SENSOR_STR, __func__, ERROR_LINE, __LINE__);
        return -1;
    }
	
    ret = drv_mag_st_lis2mdl_enable_off_canc(&LIS2MDL_ctx);
    if(unlikely(ret)){
        LOG("%s %s %s %d\n", SENSOR_STR, __func__, ERROR_LINE, __LINE__);
        return -1;
    }
    /* update the phy sensor info to sensor hal */
    LOG("%s %s successfully \n", SENSOR_STR, __func__);
    return 0;
}

SENSOR_DRV_ADD(drv_mag_st_lis2mdl_init);


