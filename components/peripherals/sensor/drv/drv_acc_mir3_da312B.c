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

#define NSA_REG_SPI_I2C                                 0x00
#define NSA_REG_WHO_AM_I                                0x01
#define NSA_REG_ACC_X_LSB                               0x02
#define NSA_REG_ACC_X_MSB                               0x03
#define NSA_REG_ACC_Y_LSB                               0x04
#define NSA_REG_ACC_Y_MSB                               0x05
#define NSA_REG_ACC_Z_LSB                               0x06
#define NSA_REG_ACC_Z_MSB                               0x07
#define NSA_REG_MOTION_FLAG                             0x09
#define NSA_REG_NEWDATA_FLAG                            0x0A
#define NSA_REG_G_RANGE                                 0x0f
#define NSA_REG_ODR_AXIS_DISABLE                        0x10
#define NSA_REG_POWERMODE_BW                            0x11
#define NSA_REG_SWAP_POLARITY                           0x12
#define NSA_REG_INTERRUPT_SETTINGS0                     0x15
#define NSA_REG_INTERRUPT_SETTINGS1                     0x16
#define NSA_REG_INTERRUPT_SETTINGS2                     0x17
#define NSA_REG_INTERRUPT_MAPPING1                      0x19
#define NSA_REG_INTERRUPT_MAPPING2                      0x1a
#define NSA_REG_INTERRUPT_MAPPING3                      0x1b
#define NSA_REG_INT_PIN_CONFIG                          0x20
#define NSA_REG_INT_LATCH                               0x21
#define NSA_REG_ACTIVE_DURATION                         0x27
#define NSA_REG_ACTIVE_THRESHOLD                        0x28
#define NSA_REG_CUSTOM_OFFSET_X                         0x38
#define NSA_REG_CUSTOM_OFFSET_Y                         0x39
#define NSA_REG_CUSTOM_OFFSET_Z                         0x3a
#define NSA_REG_ENGINEERING_MODE                        0x7f
#define NSA_REG_SENSITIVITY_TRIM_X                      0x80
#define NSA_REG_SENSITIVITY_TRIM_Y                      0x81
#define NSA_REG_SENSITIVITY_TRIM_Z                      0x82
#define NSA_REG_COARSE_OFFSET_TRIM_X                    0x83
#define NSA_REG_COARSE_OFFSET_TRIM_Y                    0x84
#define NSA_REG_COARSE_OFFSET_TRIM_Z                    0x85
#define NSA_REG_FINE_OFFSET_TRIM_X                      0x86
#define NSA_REG_FINE_OFFSET_TRIM_Y                      0x87
#define NSA_REG_FINE_OFFSET_TRIM_Z                      0x88
#define NSA_REG_SENS_COMP                               0x8c
#define NSA_REG_SENS_COARSE_TRIM                        0xd1

#define DA312B_NORMAL_MODE                               0x00
#define DA312B_SUSPEND_MODE                              0x01

#define DA312B_I2C_SLAVE_ADDR                            (0x27)

#define DA312B_ACC_DATA_SIZE                             6

#define DA312B_CHIP_ID_VAL                               0x13
#define DA312B_ADDR_TRANS(n)                             ((n) << 1)

#define DA312B_GET_BITSLICE(regvar, bitname)             ((regvar & bitname##__MSK) >> bitname##__POS)
#define DA312B_SET_BITSLICE(regvar, bitname, val)        ((regvar & ~bitname##__MSK) | ((val<<bitname##__POS)&bitname##__MSK))

i2c_dev_t da312B_ctx = {
    .port = 2,
    .config.address_width = 8,
    .config.freq = 100000,
    .config.dev_addr = DA312B_ADDR_TRANS(DA312B_I2C_SLAVE_ADDR)
};

static int drv_acc_mir3_da312B_validate_id(i2c_dev_t* drv, uint8_t id_value)
{
    int     ret = 0;
    uint8_t value = 0;

    if(drv == NULL){
        return -1;
    }

    ret = sensor_i2c_read(drv, NSA_REG_WHO_AM_I, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)) {
        return ret;
    }
    
    if (id_value != value){
        return -1;
    }
    return 0;
}

static int drv_acc_mir3_da312B_set_power_mode(i2c_dev_t* drv, dev_power_mode_e mode)
{
    int     ret = 0;
    uint8_t dev_mode;


    switch(mode){
        case DEV_POWER_OFF:
        case DEV_SLEEP:{
            dev_mode = (uint8_t)0x80;
            break;
            }
        case DEV_POWER_ON:{
            dev_mode = (uint8_t)0x34;
            break;
            }
        default:return -1;
    }

    ret = sensor_i2c_write(drv, NSA_REG_POWERMODE_BW, &dev_mode, I2C_DATA_LEN, I2C_OP_RETRIES);
    if(unlikely(ret)) {
        return ret;
    }

    return 0;
}

static int drv_acc_mir3_da312B_set_default_config(i2c_dev_t* drv)
{
    int     ret = 0;
    uint8_t value = 0;

    value = 0x83;
    ret = sensor_i2c_write(drv, NSA_REG_ENGINEERING_MODE,
                            &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    value = 0x69;
    ret = sensor_i2c_write(drv, NSA_REG_ENGINEERING_MODE,
                            &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    value = 0xbd;
    ret = sensor_i2c_write(drv, NSA_REG_ENGINEERING_MODE,
                            &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    ret = sensor_i2c_read(drv, 0x8e, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }
    if (value == 0) {
        value = 0x50;
        ret = sensor_i2c_write(drv, 0x8e, &value, I2C_DATA_LEN, I2C_OP_RETRIES);
        if (unlikely(ret)) {
            return ret;
        }
    }

    value = 0x40;
    ret = sensor_i2c_write(drv, NSA_REG_G_RANGE,
                            &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    value = 0x00;
    ret = sensor_i2c_write(drv, NSA_REG_INT_PIN_CONFIG,
                            &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    ret = drv_acc_mir3_da312B_set_power_mode(drv, DEV_SLEEP);
    if (unlikely(ret)) {
        return ret;
    }

    value = 0x07;
    ret = sensor_i2c_write(drv, NSA_REG_ODR_AXIS_DISABLE,
                            &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    value = 0x04;
    ret = sensor_i2c_write(drv, NSA_REG_INTERRUPT_MAPPING2,
                            &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    value = 0x04;
    ret = sensor_i2c_write(drv, NSA_REG_INTERRUPT_SETTINGS0,
                            &value, I2C_DATA_LEN, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return ret;
    }

    return 0;
}

static void drv_acc_mir3_da312B_irq_handle(void)
{
    /* no handle so far */
}

static int drv_acc_mir3_da312B_open(void)
{
    int ret = 0;

    ret = drv_acc_mir3_da312B_set_power_mode(&da312B_ctx, DEV_POWER_ON);
    if(unlikely(ret)) {
        return -1;
    }

    LOG("%s %s successfully \n", SENSOR_STR, __func__);
    return 0;

}

static int drv_acc_mir3_da312B_close(void)
{
    int ret = 0;

    ret = drv_acc_mir3_da312B_set_power_mode(&da312B_ctx, DEV_POWER_OFF);
    if(unlikely(ret)) {
        return -1;
    }

    LOG("%s %s successfully \n", SENSOR_STR, __func__);
    return 0;
}

static int drv_acc_mir3_da312B_read(void *buf, size_t len)
{
    int ret = 0;
    size_t size;
    uint8_t acc_raw[DA312B_ACC_DATA_SIZE] = {0};
    accel_data_t* pdata = (accel_data_t*)buf;

    if(buf == NULL){
        return -1;
    }

    size = sizeof(accel_data_t);
    if(len < size){
        return -1;
    }

    ret = sensor_i2c_read(&da312B_ctx, NSA_REG_ACC_X_LSB, acc_raw, DA312B_ACC_DATA_SIZE, I2C_OP_RETRIES);
    if (unlikely(ret)) {
        return -1;
    }

    pdata->data[0] = (int32_t)((int16_t)(acc_raw[1] << 8 | acc_raw[0]) >> 4);
    pdata->data[1] = (int32_t)((int16_t)(acc_raw[3] << 8 | acc_raw[2]) >> 4);
    pdata->data[2] = (int32_t)((int16_t)(acc_raw[5] << 8 | acc_raw[4]) >> 4);

    pdata->timestamp = aos_now_ms();

    return (int)size;
}

static int drv_acc_mir3_da312B_write(const void *buf, size_t len)
{
    (void)buf;
    (void)len;
    return 0;
}

static int drv_acc_mir3_da312B_ioctl(int cmd, unsigned long arg)
{
    int ret = 0;

    switch (cmd) {
        case SENSOR_IOCTL_SET_POWER:{
            ret = drv_acc_mir3_da312B_set_power_mode(&da312B_ctx, arg);
            if(unlikely(ret)) {
                return -1;
            }
        }break;
        case SENSOR_IOCTL_GET_INFO:{
            /* fill the dev info here */
            dev_sensor_info_t *info = (dev_sensor_info_t *)arg;
            info->model = "DA312B";
            info->unit = mg;
        }break;
        default:
            return -1;
    }

    LOG("%s %s successfully \n", SENSOR_STR, __func__);
    return 0;
}

int drv_acc_mir3_da312B_init(void)
{
    int ret = 0;
    sensor_obj_t sensor;
    memset(&sensor, 0, sizeof(sensor));

    /* fill the sensor obj parameters here */
    sensor.tag = TAG_DEV_ACC;
    sensor.path = dev_acc_path;
    sensor.io_port = I2C_PORT;
    sensor.open = drv_acc_mir3_da312B_open;
    sensor.close = drv_acc_mir3_da312B_close;
    sensor.read = drv_acc_mir3_da312B_read;
    sensor.write = drv_acc_mir3_da312B_write;
    sensor.ioctl = drv_acc_mir3_da312B_ioctl;
    sensor.irq_handle = drv_acc_mir3_da312B_irq_handle;
    ret = sensor_create_obj(&sensor);
    if(unlikely(ret)) {
        return -1;
    }

    ret = drv_acc_mir3_da312B_validate_id(&da312B_ctx, DA312B_CHIP_ID_VAL);
    if(unlikely(ret)) {
        return -1;
    }

    ret = drv_acc_mir3_da312B_set_default_config(&da312B_ctx);
    if(unlikely(ret)) {
        return -1;
    }

    LOG("%s %s successfully \n", SENSOR_STR, __func__);
    return 0;
}

SENSOR_DRV_ADD(drv_acc_mir3_da312B_init);

