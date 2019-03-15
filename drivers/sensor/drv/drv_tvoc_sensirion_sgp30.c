/*
* Copyright (C) 2018 Sensirion Inc.
* Author: Johannes Winkelmann, jwi@sensirion.com
*
* Driver for SGP30 and SGPC3
* Based on SHTC1 driver
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "aos/kernel.h"
#include "network/hal/base.h"
#include "sensor_drv_api.h"
#include "sensor_hal.h"

#define SGP30_I2C_SLAVE_ADDR                            0x58

#define SGP30_ADDR_TRANS(n)                             ((n) << 1)
#define SGP30_I2C_ADDR                                  SGP30_ADDR_TRANS(SGP30_I2C_SLAVE_ADDR)

/* delays for non-blocking i2c commands, both in ms */
#define SGP30_NONBLOCKING_WAIT_TIME                     50

#define SGP30_CMD_LENGTH                                2
#define SGP30_INFO_RESPONSE_LENGTH                      3
#define SGP30_DATA_RESPONSE_LENGTH                      6
#define SGPC3_DATA_RESPONSE_LENGTH                      3

/*
 * default port = 3
 * Use "GLOBAL_DEFINES += SENSIRION_SGP30_PORT=2" in a Makefile to override
*/
#ifndef SENSIRION_SGP30_PORT
#define SENSIRION_SGP30_PORT 2
#endif /* SENSIRION_SGP30_PORT */

typedef enum {
    TYPE_SGP30,
    TYPE_SGPC3
} SGP30_TYPE_ENUM;

static SGP30_TYPE_ENUM g_sgp_type = TYPE_SGPC3;

i2c_dev_t sgp30_ctx = {
    .port = SENSIRION_SGP30_PORT,
    .config.address_width = 8,
    .config.freq = 100000,
    .config.dev_addr = SGP30_I2C_ADDR,
};


static const uint8_t sgp30_cmd_list[][SGP30_CMD_LENGTH] = {
    /* SGP30 */
    {0x20, 0x03},                                       /* init air quality */
    {0x20, 0x2f},                                       /* get feature set */
    {0x20, 0x08},                                       /* measure air quality */

    /* SGPC3 */
    {0x20, 0xae}                                        /* init air quality */
};

typedef enum {
    SGP30_CMD_INIT_AIR_QUALITY,
    SGP30_CMD_GET_FEATURE_SET,
    SGP30_CMD_MEASURE_AIR_QUALITY,
    SGPC3_CMD_INIT_AIR_QUALITY,
    SGP30_CMD_END
} CMD_SGP30_ENUM;

static void sgp30_delay_ms(uint32_t delay_time)
{
    // TODO: krhino_task_sleep is currently blocking; therefore, I implemented
    // a busy wait loop
    //
    // krhino_task_sleep(krhino_ms_to_ticks(SGP30_NONBLOCKING_WAIT_TIME));
    uint32_t start_ms = aos_now_ms();
    while (aos_now_ms() - start_ms < delay_time) {
        // idle
    }
}

static int drv_sgp30_cmd_write(i2c_dev_t* drv, CMD_SGP30_ENUM cmd)
{
    if (cmd < SGP30_CMD_INIT_AIR_QUALITY || cmd >= SGP30_CMD_END)
        return -1;

    return hal_i2c_master_send(drv, drv->config.dev_addr, sgp30_cmd_list[cmd], SGP30_CMD_LENGTH, AOS_WAIT_FOREVER);
}

static int drv_sgp30_result_read(i2c_dev_t* drv, uint8_t *data, uint16_t size)
{
    if (data == NULL || size == 0)
        return -1;

    return hal_i2c_master_recv(drv, drv->config.dev_addr, data, size, AOS_WAIT_FOREVER);
}

static int drv_sgp30_read_raw_data(i2c_dev_t *drv, integer_data_t *pdata)
{
    int ret = 0;
    uint8_t data[SGP30_DATA_RESPONSE_LENGTH] = {0};

    int data_length = (g_sgp_type == TYPE_SGP30) ?
        SGP30_DATA_RESPONSE_LENGTH :
        SGPC3_DATA_RESPONSE_LENGTH;

    ret = drv_sgp30_cmd_write(drv, SGP30_CMD_MEASURE_AIR_QUALITY);
    if (unlikely(ret)) {
        return ret;
    }
    sgp30_delay_ms(SGP30_NONBLOCKING_WAIT_TIME);
    ret = drv_sgp30_result_read(drv, data, data_length);
    if (unlikely(ret)) {
        return ret;
    }

    if (g_sgp_type == TYPE_SGP30) {
        pdata->data = ((uint16_t)data[3] << 8) | data[4];
    } else {
        pdata->data = ((uint16_t)data[0] << 8) | data[1];
    }

    return ret;
}

static int drv_sgp30_init_sensor(i2c_dev_t* drv)
{
    int ret = 0;
    uint8_t data[SGPC3_DATA_RESPONSE_LENGTH] = {0};

    ret = drv_sgp30_cmd_write(drv, SGP30_CMD_GET_FEATURE_SET);
    if (unlikely(ret)) {
        return ret;
    }
    sgp30_delay_ms(SGP30_NONBLOCKING_WAIT_TIME);
    ret = drv_sgp30_result_read(drv, data, SGPC3_DATA_RESPONSE_LENGTH);
    if (unlikely(ret)) {
        return ret;
    }
    
    CMD_SGP30_ENUM init_cmd = SGP30_CMD_INIT_AIR_QUALITY;

    int product_id = data[0] >> 4;
    int major      = (data[1] & 0xe0) >> 5;
    int minor      = data[1] & 0x1f;

    if (product_id == 1) {
        if (unlikely(major != 0 || minor < 6)) {            // unsupported sensor
            return -1;
        }
        g_sgp_type = TYPE_SGPC3;
        init_cmd   = SGPC3_CMD_INIT_AIR_QUALITY;
    } else if (product_id == 0) {
        if (unlikely(major != 1)) {            // unsupported sensor
            return -1;
        }
        g_sgp_type = TYPE_SGP30;
        init_cmd = SGP30_CMD_INIT_AIR_QUALITY;
    }

    ret = drv_sgp30_cmd_write(drv, init_cmd);
    if (unlikely(ret)) {
        LOG("%s %s: Init air quality failed", SENSOR_STR, __func__);
        return ret;
    }

    return ret;
}

static void drv_tvoc_sensirion_sgp30_irq_handle(void)
{
    /* no handle so far */
}

static int drv_tvoc_sensirion_sgp30_open(void)
{
    LOG("%s %s successfully \n", SENSOR_STR, __func__);

    return 0;
}

static int drv_tvoc_sensirion_sgp30_close(void)
{
    LOG("%s %s successfully \n", SENSOR_STR, __func__);

    return 0;
}

static int drv_tvoc_sensirion_sgp30_read(void *buf, size_t len)
{
    int ret = 0;
    size_t size;
    integer_data_t *pdata = (integer_data_t *)buf;

    if (buf == NULL){
        return -1;
    }

    size = sizeof(integer_data_t);
    if (len < size){
        return -1;
    }
    
    ret = drv_sgp30_read_raw_data(&sgp30_ctx, pdata);
    if (ret != 0)
        return -1;

    pdata->timestamp = aos_now_ms();

    return (int)size;
}

static int drv_tvoc_sensirion_sgp30_write(const void *buf, size_t len)
{
    (void)buf;
    (void)len;
    return 0;
}

static int drv_tvoc_sensirion_sgp30_ioctl(int cmd, unsigned long arg)
{
    switch (cmd) {
        case SENSOR_IOCTL_GET_INFO:{
            /* fill the dev info here */
            dev_sensor_info_t *info = (dev_sensor_info_t *)arg;
            info->model = (g_sgp_type == TYPE_SGP30) ? "SGP30" : "SGPC3";
            //info->unit = ppb;
        }break;
        default:
            return -1;
    }

    LOG("%s %s successfully \n", SENSOR_STR, __func__);
    return 0;
}


int drv_tvoc_sensirion_sgp30_init(void)
{
    int ret = 0;
    sensor_obj_t sensor_temp;
    memset(&sensor_temp, 0, sizeof(sensor_temp));

    ret = drv_sgp30_init_sensor(&sgp30_ctx);
    if (unlikely(ret)) {
        return -1;
    }

    /* fill the sensor_temp obj parameters here */
    sensor_temp.tag = TAG_DEV_TVOC;
    sensor_temp.path = dev_tvoc_path;
    sensor_temp.io_port = I2C_PORT;
    sensor_temp.open = drv_tvoc_sensirion_sgp30_open;
    sensor_temp.close = drv_tvoc_sensirion_sgp30_close;
    sensor_temp.read = drv_tvoc_sensirion_sgp30_read;
    sensor_temp.write = drv_tvoc_sensirion_sgp30_write;
    sensor_temp.ioctl = drv_tvoc_sensirion_sgp30_ioctl;
    sensor_temp.irq_handle = drv_tvoc_sensirion_sgp30_irq_handle;

    ret = sensor_create_obj(&sensor_temp);
    if (unlikely(ret)) {
        return -1;
    }

    LOG("%s %s successfully \n", SENSOR_STR, __func__);
    return 0;
}

SENSOR_DRV_ADD(drv_tvoc_sensirion_sgp30_init);
