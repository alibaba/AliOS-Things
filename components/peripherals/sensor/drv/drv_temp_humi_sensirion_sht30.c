/*
* Copyright (C) 2018 Sensirion Inc.
* Author: Johannes Winkelmann, jwi@sensirion.com
*
* Based on SHTC1 driver
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "aos/kernel.h"
#include "network/hal/base.h"
#include "sensor_drv_api.h"
#include "sensor_hal.h"

#define SHT30_I2C_SLAVE_ADDR_DEFAULT                   0x44
#define SHT30_I2C_SLAVE_ADDR_ALT                       0x45

#define SHT30_ADDR_TRANS(n)                             ((n) << 1)
#define SHT30_I2C_ADDR                                  SHT30_ADDR_TRANS(SHT30_I2C_SLAVE_ADDR_DEFAULT)

/* delays for non-blocking i2c commands, both in ms */
#define SHT30_NONBLOCKING_WAIT_TIME_HRM                 15

#define SHT30_CMD_LENGTH                                2
#define SHT30_RESPONSE_LENGTH                           6

#define SHT30_DATA_READ_MIN_INTERVAL                    200       /* in millisecond */

typedef enum {
    FLAG_INIT_TEMP = 0,
    FLAG_INIT_HUMI,
} FLAG_INIT_BIT;

/*
 * default port = 3
 * Use "GLOBAL_DEFINES += SENSIRION_SHT30_PORT=2" in a Makefile to override
*/
#ifndef SENSIRION_SHT30_PORT
#define SENSIRION_SHT30_PORT 3
#endif /* SENSIRION_SHT30_PORT */

i2c_dev_t sht30_ctx = {
    .port = SENSIRION_SHT30_PORT,
    .config.dev_addr = SHT30_I2C_ADDR,
};

static uint8_t g_init_bitwise = 0;

typedef struct {
    int32_t temperature;
    uint32_t humidity;
} sht30_sensor_data;

static sht30_sensor_data g_sht30_data_new = {0};

static const uint8_t sht30_cmd_list[][SHT30_CMD_LENGTH] = {
    {0x2C, 0x06},                                       /* measure blocking hrm */
};

typedef enum {
    SHT30_CMD_measure_blocking_hrm,
    SHT30_CMD_END
} CMD_SHT30_ENUM;

static int drv_sht30_cmd_write(i2c_dev_t *drv, CMD_SHT30_ENUM cmd)
{
    int ret = 0;

    if (cmd < SHT30_CMD_measure_blocking_hrm || cmd >= SHT30_CMD_END) {
        return -1;
    }
    ret = hal_i2c_master_send(drv, drv->config.dev_addr, sht30_cmd_list[cmd], SHT30_CMD_LENGTH, AOS_WAIT_FOREVER);
    return ret;
}

static int drv_sht30_result_read(i2c_dev_t *drv, uint8_t *data, uint16_t size)
{
    if (data == NULL || size == 0)
        return -1;

    return hal_i2c_master_recv(drv, drv->config.dev_addr, data, size, AOS_WAIT_FOREVER);
}

static int drv_sht30_read_raw_data(i2c_dev_t *drv, uint8_t *data)
{
    int ret = 0;
    CMD_SHT30_ENUM readcmd;

    if (data == NULL)
        return -1;

    readcmd = SHT30_CMD_measure_blocking_hrm;
    ret = drv_sht30_cmd_write(drv, readcmd);
    if (unlikely(ret)) {
        return ret;
    }
    ret = drv_sht30_result_read(drv, data, SHT30_RESPONSE_LENGTH);
    if (unlikely(ret)) {
        return ret;
    }
    return ret;
}

static int drv_sht30_read_temp_and_humi(i2c_dev_t *drv, int32_t *temperature, uint32_t *humidity)
{
    int ret = 0;
    uint8_t data[SHT30_RESPONSE_LENGTH] = {0};
    int32_t temp_raw = 0;
    int32_t humi_raw = 0;

    if (temperature == NULL && humidity == NULL)
        return -1;

    ret = drv_sht30_read_raw_data(drv, data);
    if (unlikely(ret)) {
        return ret;
    }

    temp_raw = (int)(data[0] << 8 | data[1]);
    humi_raw = (int)(data[3] << 8 | data[4]);

    if (temperature != NULL)
        *temperature = (int32_t)((21875 * temp_raw) >> 13) - 45000;
    if (humidity != NULL)
        *humidity = (uint32_t)((12500 * humi_raw) >> 13);

    return 0;
}

static int drv_sht30_update_data(i2c_dev_t *drv)
{
    static uint32_t prev_update_tick = 0;
    uint32_t now_tick = 0;
    int ret = 0;

    now_tick = aos_now_ms();
    if (now_tick - prev_update_tick >= SHT30_DATA_READ_MIN_INTERVAL) {
        ret = drv_sht30_read_temp_and_humi(drv, &g_sht30_data_new.temperature, &g_sht30_data_new.humidity);
        if (unlikely(ret != 0)) {
            return ret;
        }
        prev_update_tick = now_tick;
    }

    return 0;
}

static void drv_temp_sensirion_sht30_irq_handle(void)
{
    /* no handle so far */
}

static int drv_temp_sensirion_sht30_open(void)
{
    LOG("%s %s successfully \n", SENSOR_STR, __func__);

    return 0;
}

static int drv_temp_sensirion_sht30_close(void)
{
    LOG("%s %s successfully \n", SENSOR_STR, __func__);

    return 0;
}

static int drv_temp_sensirion_sht30_read(void *buf, size_t len)
{
    int ret = 0;
    const size_t size = sizeof(temperature_data_t);
    temperature_data_t *pdata = (temperature_data_t*)buf;

    if (buf == NULL){
        return -1;
    }

    if (len < size){
        return -1;
    }
    
    ret = drv_sht30_update_data(&sht30_ctx);
    if (ret != 0)
        return -1;

    pdata->t = g_sht30_data_new.temperature / 100.0f;

    pdata->timestamp = aos_now_ms();

    return (int)size;
}

static int drv_temp_sensirion_sht30_write(const void *buf, size_t len)
{
    (void)buf;
    (void)len;
    return 0;
}


static int drv_temp_sensirion_sht30_ioctl(int cmd, unsigned long arg)
{
    switch (cmd) {
        case SENSOR_IOCTL_GET_INFO:{
            /* fill the dev info here */
            dev_sensor_info_t *info = (dev_sensor_info_t *)arg;
            info->model = "SHT30";
            info->unit = dCelsius;
        }break;
        default:
            return -1;
    }

    LOG("%s %s successfully \n", SENSOR_STR, __func__);
    return 0;
}

static void drv_humi_sensirion_sht30_irq_handle(void)
{
    /* no handle so far */
}

static int drv_humi_sensirion_sht30_open(void)
{
    LOG("%s %s successfully \n", SENSOR_STR, __func__);

    return 0;
}

static int drv_humi_sensirion_sht30_close(void)
{
    LOG("%s %s successfully \n", SENSOR_STR, __func__);

    return 0;
}

static int drv_humi_sensirion_sht30_read(void *buf, size_t len)
{
    int ret = 0;
    const size_t size = sizeof(humidity_data_t);
    humidity_data_t *pdata = (humidity_data_t*)buf;

    if (buf == NULL){
        return -1;
    }

    if (len < size){
        return -1;
    }

    ret = drv_sht30_update_data(&sht30_ctx);
    if (ret != 0)
        return -1;

    pdata->h = g_sht30_data_new.humidity / 100;

    pdata->timestamp = aos_now_ms();

    return (int)size;
}

static int drv_humi_sensirion_sht30_write(const void *buf, size_t len)
{
    (void)buf;
    (void)len;
    return 0;
}

static int drv_humi_sensirion_sht30_ioctl(int cmd, unsigned long arg)
{
    switch (cmd) {
        case SENSOR_IOCTL_GET_INFO:{
            /* fill the dev info here */
            dev_sensor_info_t *info = (dev_sensor_info_t *)arg;
            info->model = "SHT30";
            info->unit = permillage;
        }break;
        default:
            return -1;
    }

    LOG("%s %s successfully \n", SENSOR_STR, __func__);
    return 0;
}

int drv_temp_sensirion_sht30_init(void)
{
    int ret = 0;
    sensor_obj_t sensor_temp;
    memset(&sensor_temp,0, sizeof(sensor_temp));

    if (!(g_init_bitwise & (1 << FLAG_INIT_TEMP))) {
        /* fill the sensor_temp obj parameters here */
        sensor_temp.tag = TAG_DEV_TEMP;
        sensor_temp.path = dev_temp_path;
        sensor_temp.io_port = I2C_PORT;
        sensor_temp.open = drv_temp_sensirion_sht30_open;
        sensor_temp.close = drv_temp_sensirion_sht30_close;
        sensor_temp.read = drv_temp_sensirion_sht30_read;
        sensor_temp.write = drv_temp_sensirion_sht30_write;
        sensor_temp.ioctl = drv_temp_sensirion_sht30_ioctl;
        sensor_temp.irq_handle = drv_temp_sensirion_sht30_irq_handle;

        ret = sensor_create_obj(&sensor_temp);
        if (unlikely(ret)) {
            return -1;
        }

        g_init_bitwise |= 1 << FLAG_INIT_TEMP;
    }

    LOG("%s %s successfully \n", SENSOR_STR, __func__);
    return 0;
}

int drv_humi_sensirion_sht30_init(void)
{
    int ret = 0;
    sensor_obj_t sensor_humi;
    memset(&sensor_humi, 0, sizeof(sensor_humi));
    if (!(g_init_bitwise & (1 << FLAG_INIT_HUMI))) {
        /* fill the sensor_humi obj parameters here */
        sensor_humi.tag = TAG_DEV_HUMI;
        sensor_humi.path = dev_humi_path;
        sensor_humi.io_port = I2C_PORT;
        sensor_humi.open = drv_humi_sensirion_sht30_open;
        sensor_humi.close = drv_humi_sensirion_sht30_close;
        sensor_humi.read = drv_humi_sensirion_sht30_read;
        sensor_humi.write = drv_humi_sensirion_sht30_write;
        sensor_humi.ioctl = drv_humi_sensirion_sht30_ioctl;
        sensor_humi.irq_handle = drv_humi_sensirion_sht30_irq_handle;

        ret = sensor_create_obj(&sensor_humi);
        if (unlikely(ret)) {
            return -1;
        }

        g_init_bitwise |= 1 << FLAG_INIT_HUMI;
    }

    LOG("%s %s successfully \n", SENSOR_STR, __func__);
    return 0;
}

SENSOR_DRV_ADD(drv_humi_sensirion_sht30_init);
SENSOR_DRV_ADD(drv_temp_sensirion_sht30_init);