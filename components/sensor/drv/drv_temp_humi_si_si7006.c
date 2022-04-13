/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */



#include "ulog/ulog.h"
#include "aos/hal/i2c.h"
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "aos/kernel.h"
#include "sensor_drv_api.h"
#include "sensor_hal.h"

#define SI7006_I2C_ADDRESS 0x40
#define SI7006_I2C_ADDR_TRANS(n) ((n) << 1)
#define SI7006_TAG     0x06

#define SI7006_MEAS_REL_HUMIDITY_MASTER_MODE    0xE5
#define SI7006_MEAS_REL_HUMIDITY_NO_MASTER_MODE 0xF5
#define SI7006_MEAS_TEMP_MASTER_MODE            0xE3
#define SI7006_MEAS_TEMP_NO_MASTER_MODE         0xF3
#define SI7006_READ_OLD_TEMP                    0xE0
#define SI7006_RESET                            0xFE
#define SI7006_READ_ID_LOW_0                    0xFA
#define SI7006_READ_ID_LOW_1                    0x0F
#define SI7006_READ_ID_HIGH_0                   0xFC
#define SI7006_READ_ID_HIGH_1                   0xC9
#define SI7006_READ_Firmware_Revision_0         0x84
#define SI7006_READ_Firmware_Revision_1         0xB8


i2c_dev_t SI7006_ctx = {
    .port                 = 1,
    .config.address_width = I2C_HAL_ADDRESS_WIDTH_7BIT,
    .config.freq          = I2C_BUS_BIT_RATES_100K,
    .config.mode          = I2C_MODE_MASTER,
    .config.dev_addr      = SI7006_I2C_ADDRESS,
};


static int drv_temp_si_si7006_write(const void *buf, size_t len)
{
    return 0;
}

static int drv_temp_si_si7006_read(void *buf, size_t len)
{
    int          ret = 0;
    size_t       size;
    float      temp;
    uint8_t      read_data[2] = {0};
    unsigned int value;
    temperature_data_t *pdata = (temperature_data_t *)buf;

    // printf("\n enter %s : %d\n", __func__, __LINE__);

    if (buf == NULL) {
        return -1;
    }
    //printf("\n enter %s : %d\n", __func__, __LINE__);

    size = sizeof(temperature_data_t);
    if (len < size) {
        return -1;
    }
    //printf("\n enter %s : %d\n", __func__, __LINE__);
    uint8_t  reg = SI7006_MEAS_TEMP_NO_MASTER_MODE;

    // hal_i2c_master_send(&SI7006_ctx, SI7006_ctx.config.dev_addr, &reg, 1, 1000);
    ret = sensor_i2c_master_send(&SI7006_ctx, &reg, 1, 1000);
    if (ret < 0) {
        LOG("%s Error on i2c master send\n", SENSOR_STR);
        return -1;
    }

    aos_msleep(30);
    // hal_i2c_master_recv(&SI7006_ctx, SI7006_ctx.config.dev_addr, read_data, 2, 1000);
    ret = sensor_i2c_master_recv(&SI7006_ctx, read_data, 2, 1000);
    if (ret < 0) {
        LOG("%s Error on i2c master recv\n", SENSOR_STR);
        return -1;
    }
    value = (read_data[0] << 8) | read_data[1];
    //LOGI("APP", "%0x -- %0x -->0x%x\n", read_data[0], read_data[1], value);
    //LOG("%s %0x -- %0x -->0x%x\n", SENSOR_STR, read_data[0], read_data[1], value);
    // A temperature measurement will always return XXXXXX00 in the LSB field.
    if (value & 0xFFFC) {
        temp = (175.72f * (float)value) / 65536.0f - 46.85f;
        //LOGI("APP", "temperature: %2f\n", temp);
        LOG("%s temperature: %2f\n", SENSOR_STR, temp);
        pdata->t = (int)(temp * 10);
        pdata->timestamp = aos_now_ms();
    } else {
        LOG("%s Error on temp\n", SENSOR_STR);
        return -1;
    }
    return (int)size;
}

static int drv_temp_si_si7006_ioctl(int cmd, unsigned long arg)
{

}

static void drv_temp_si_si7006_irq_handle(void)
{
    /* no handle so far */
}

static int drv_temp_si_si7006_close(void)
{
#if 0
    int ret = 0;
    ret     = drv_temp_si_si7006_set_power_mode(&SI7006_ctx, DEV_POWER_OFF);
    if (unlikely(ret)) {
        return -1;
    }
#endif
    LOG("%s %s successfully \n", SENSOR_STR, __func__);
    return 0;
}

static int drv_temp_si_si7006_open(void)
{
#if 0
    int ret = 0;
    ret     = drv_temp_si_si7006_set_power_mode(&SI7006_ctx, DEV_POWER_ON);
    if (unlikely(ret)) {
        return -1;
    }
#endif
    LOG("%s %s successfully \n", SENSOR_STR, __func__);
    return 0;
}

int drv_temp_si_si7006_init(void)
{
    int          ret = 0;
    sensor_obj_t sensor;

    memset(&sensor, 0, sizeof(sensor));

    /* fill the sensor obj parameters here */
    sensor.tag        = TAG_DEV_TEMP;
    sensor.path       = dev_temp_path;
    sensor.io_port    = I2C_PORT;
    sensor.open       = drv_temp_si_si7006_open;
    sensor.close      = drv_temp_si_si7006_close;
    sensor.read       = drv_temp_si_si7006_read;
    sensor.write      = drv_temp_si_si7006_write;
    sensor.ioctl      = drv_temp_si_si7006_ioctl;
    sensor.irq_handle = drv_temp_si_si7006_irq_handle;

    ret = sensor_create_obj(&sensor);
    if (unlikely(ret)) {
        return -1;
    }

    /* update the phy sensor info to sensor hal */
    LOG("%s %s successfully \n", SENSOR_STR, __func__);
    return 0;
}


static int drv_humi_si_si7006_write(const void *buf, size_t len)
{
    return 0;
}

static int drv_humi_si_si7006_read(void *buf, size_t len)
{
    int          ret = 0;
    size_t       size;
    uint8_t      read_data[3] = {0};
    unsigned int value;
    float          humi;
    uint8_t  reg  = SI7006_MEAS_REL_HUMIDITY_NO_MASTER_MODE;

    humidity_data_t *pdata = (humidity_data_t *)buf;

    //printf("\n enter %s : %d\n", __func__, __LINE__);

    if (buf == NULL) {
        return -1;
    }
    //printf("\n enter %s : %d\n", __func__, __LINE__);

    size = sizeof(humidity_data_t);
    if (len < size) {
        return -1;
    }
    //printf("\n enter %s : %d\n", __func__, __LINE__);

    // hal_i2c_master_send(&SI7006_ctx, SI7006_ctx.config.dev_addr, &reg, 1, 1000);
    ret = sensor_i2c_master_send(&SI7006_ctx, &reg, 1, 1000);
    if (ret < 0) {
        LOG("%s Error on i2c master send\n", SENSOR_STR);
        return -1;
    }
    aos_msleep(30);

    // hal_i2c_master_recv(&SI7006_ctx, SI7006_ctx.config.dev_addr, read_data, 2, 1000);
    ret = sensor_i2c_master_recv(&SI7006_ctx, read_data, 2, 1000);
    if (ret < 0) {
        LOG("%s Error on i2c master recv\n", SENSOR_STR);
        return -1;
    }
    value = (read_data[0] << 8) | read_data[1];
    //LOGI("APP", "%0x -- %0x -->0x%x\n", read_data[0], read_data[1], value);
    if (value & 0xFFFE) {
        humi = ((125.0f * (float)value) / 65535.0f) - 6.0f;
        LOG("%s humidity: %2f\n", SENSOR_STR, humi);
        pdata->h = (int)(humi * 10);
        pdata->timestamp = aos_now_ms();

    } else {
        LOG("%s Error on humidity\n", SENSOR_STR);
        return 1;
    }
    return (int)size;
}

static int drv_humi_si_si7006_ioctl(int cmd, unsigned long arg)
{

}

static void drv_humi_si_si7006_irq_handle(void)
{
    /* no handle so far */
}

static int drv_humi_si_si7006_close(void)
{
#if 0
    int ret = 0;
    ret     = drv_temp_si_si7006_set_power_mode(&SI7006_ctx, DEV_POWER_OFF);
    if (unlikely(ret)) {
        return -1;
    }
#endif
    LOG("%s %s successfully \n", SENSOR_STR, __func__);
    return 0;
}

static int drv_humi_si_si7006_open(void)
{
#if 0
    int ret = 0;
    ret     = drv_temp_si_si7006_set_power_mode(&SI7006_ctx, DEV_POWER_ON);
    if (unlikely(ret)) {
        return -1;
    }
#endif
    LOG("%s %s successfully \n", SENSOR_STR, __func__);
    return 0;
}

int drv_humi_si_si7006_init(void)
{

    int          ret = 0;
    sensor_obj_t sensor;

    memset(&sensor, 0, sizeof(sensor));
    /* fill the sensor obj parameters here */
    sensor.tag        = TAG_DEV_HUMI;
    sensor.path       = dev_humi_path;
    sensor.io_port    = I2C_PORT;
    sensor.open       = drv_humi_si_si7006_open;
    sensor.close      = drv_humi_si_si7006_close;
    sensor.read       = drv_humi_si_si7006_read;
    sensor.write      = drv_humi_si_si7006_write;
    sensor.ioctl      = drv_humi_si_si7006_ioctl;
    sensor.irq_handle = drv_humi_si_si7006_irq_handle;

    ret = sensor_create_obj(&sensor);
    if (unlikely(ret)) {
        return -1;
    }

    LOG("%s %s successfully \n", SENSOR_STR, __func__);

    return 0;
}

SENSOR_DRV_ADD(drv_temp_si_si7006_init);
SENSOR_DRV_ADD(drv_humi_si_si7006_init);

