/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 *
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "aos/kernel.h"
#include "ulog/ulog.h"

#include <k_api.h>
#include "network/hal/base.h"
#include "sensor_drv_api.h"
#include "sensor_hal.h"

#define SHTC1_I2C_SLAVE_ADDR 0x70

#define SHTC1_ADDR_TRANS(n) ((n) << 1)
#define SHTC1_I2C_ADDR SHTC1_ADDR_TRANS(SHTC1_I2C_SLAVE_ADDR)

/* delays for non-blocking i2c commands, both in ms */
#define SHTC1_NONBLOCKING_WAIT_TIME_HPM 15
#define SHTC1_NONBLOCKING_WAIT_TIME_LPM 1

#define SHTC1_CHIP_ID_VAL 0x07
#define SHTC1_ID_REG_MASK 0x1f

#define SHTC1_CMD_LENGTH 2
#define SHTC1_RESPONSE_LENGTH 6

#define SHTC1_DATA_READ_MIN_INTERVAL 200 /* in millisecond */

typedef enum
{
    FLAG_INIT_TEMP = 0,
    FLAG_INIT_HUMI,
} FLAG_INIT_BIT;

i2c_dev_t shtc1_ctx = {
    .port                 = 2,
    .config.address_width = 8,
    .config.freq          = 100000,
    .config.dev_addr      = SHTC1_I2C_ADDR,
};

static uint8_t g_init_bitwise = 0;

typedef struct
{
    int32_t  temperature;
    uint32_t humidity;
} shtc1_sensor_data;

/* shtc1 configration */
static int g_is_blocking_io    = 1;
static int g_is_high_precision = 1;

shtc1_sensor_data g_data_new = { 0 };

static const uint8_t shtc1_cmd_list[][SHTC1_CMD_LENGTH] = {
    { 0xef, 0xc8 }, /* read id reg */
    { 0x7c, 0xa2 }, /* measure blocking hpm */
    { 0x78, 0x66 }, /* measure nonblocking hpm */
    { 0x64, 0x58 }, /* measure blocking lpm */
    { 0x60, 0x9c }, /* measure nonblocking lpm */
};

typedef enum
{
    SHTC1_CMD_read_id_reg,
    SHTC1_CMD_measure_blocking_hpm,
    SHTC1_CMD_measure_nonblocking_hpm,
    SHTC1_CMD_measure_blocking_lpm,
    SHTC1_CMD_measure_nonblocking_lpm,
    SHTC1_CMD_END
} CMD_SHTC1_ENUM;

static int drv_shtc1_cmd_write(i2c_dev_t *drv, CMD_SHTC1_ENUM cmd)
{

    if (cmd < SHTC1_CMD_read_id_reg || cmd >= SHTC1_CMD_END) {
        return -1;
    }

    return hal_i2c_master_send(drv, drv->config.dev_addr, shtc1_cmd_list[cmd],
                               SHTC1_CMD_LENGTH, AOS_WAIT_FOREVER);
}

static int drv_shtc1_result_read(i2c_dev_t *drv, uint8_t *data, uint16_t size)
{
    if (data == NULL || size == 0) {
        return -1;
    }

    return hal_i2c_master_recv(drv, drv->config.dev_addr, data, size,
                               AOS_WAIT_FOREVER);
}

static int drv_shtc1_read_raw_data(i2c_dev_t *drv, uint8_t *data)
{
    int            ret = 0;
    CMD_SHTC1_ENUM readcmd;
    int            nonblocking_wait_time = 0;

    if (data == NULL) {
        return -1;
    }

    if (g_is_high_precision) {
        readcmd = g_is_blocking_io ? SHTC1_CMD_measure_blocking_hpm
                                   : SHTC1_CMD_measure_nonblocking_hpm;
        nonblocking_wait_time = SHTC1_NONBLOCKING_WAIT_TIME_HPM;
    } else {
        readcmd = g_is_blocking_io ? SHTC1_CMD_measure_blocking_lpm
                                   : SHTC1_CMD_measure_nonblocking_lpm;
        nonblocking_wait_time = SHTC1_NONBLOCKING_WAIT_TIME_LPM;
    }

    ret = drv_shtc1_cmd_write(drv, readcmd);
    if (unlikely(ret)) {
        return ret;
    }
    if (!g_is_blocking_io) {
        krhino_task_sleep(krhino_ms_to_ticks(nonblocking_wait_time));
    }
    ret = drv_shtc1_result_read(drv, data, SHTC1_RESPONSE_LENGTH);
    if (unlikely(ret)) {
        return ret;
    }

    return ret;
}

static int drv_shtc1_read_temp_and_humi(i2c_dev_t *drv, int32_t *temperature,
                                        uint32_t *humidity)
{
    int     ret                         = 0;
    uint8_t data[SHTC1_RESPONSE_LENGTH] = { 0 };
    int     temp_raw                    = 0;
    int     humi_raw                    = 0;

    if (temperature == NULL && humidity == NULL) {
        return -1;
    }

    ret = drv_shtc1_read_raw_data(drv, data);
    if (unlikely(ret)) {
        return ret;
    }

    temp_raw = (int)(data[0] << 8 | data[1]);
    humi_raw = (int)(data[3] << 8 | data[4]);

    if (temperature != NULL) {
        *temperature = (int32_t)((21875 * temp_raw) >> 13) - 45000;
    }
    if (humidity != NULL) {
        *humidity = (uint32_t)((12500 * humi_raw) >> 13);
    }

    return 0;
}

static int drv_shtc1_update_data(i2c_dev_t *drv)
{
    static uint32_t prev_update_tick = 0;
    uint32_t        now_tick         = 0;
    int             ret              = 0;

    now_tick = aos_now_ms();
    if (now_tick - prev_update_tick > SHTC1_DATA_READ_MIN_INTERVAL) {
        ret = drv_shtc1_read_temp_and_humi(drv, &g_data_new.temperature,
                                           &g_data_new.humidity);
        if (ret != 0) {
            return ret;
        }
        prev_update_tick = now_tick;
    }

    return 0;
}

static int drv_temp_humi_sensirion_shtc1_validate_id(i2c_dev_t *drv,
                                                     uint8_t    id_value,
                                                     uint8_t    id_mask)
{
    int     ret      = 0;
    uint8_t value[2] = { 0 };

    if (drv == NULL) {
        return -1;
    }

    ret = drv_shtc1_cmd_write(drv, SHTC1_CMD_read_id_reg);
    if (unlikely(ret)) {
        return ret;
    }

    ret = drv_shtc1_result_read(drv, value, 2);
    if (unlikely(ret)) {
        return ret;
    }

    if ((value[1] & id_mask) != id_value) {
        return -1;
    }

    return 0;
}

static void drv_temp_sensirion_shtc1_irq_handle(void)
{
    /* no handle so far */
}

static int drv_temp_sensirion_shtc1_open(void)
{
    LOG("%s %s successfully \n", SENSOR_STR, __func__);

    return 0;
}

static int drv_temp_sensirion_shtc1_close(void)
{
    LOG("%s %s successfully \n", SENSOR_STR, __func__);

    return 0;
}

static int drv_temp_sensirion_shtc1_read(void *buf, size_t len)
{
    int                 ret = 0;
    size_t              size;
    temperature_data_t *pdata = (temperature_data_t *)buf;

    if (buf == NULL) {
        return -1;
    }

    size = sizeof(temperature_data_t);
    if (len < size) {
        return -1;
    }

    ret = drv_shtc1_update_data(&shtc1_ctx);
    if (ret != 0) {
        return -1;
    }

    pdata->t = g_data_new.temperature / 100;

    pdata->timestamp = aos_now_ms();

    return (int)size;
}

static int drv_temp_sensirion_shtc1_write(const void *buf, size_t len)
{
    (void)buf;
    (void)len;
    return 0;
}

static int drv_temp_sensirion_shtc1_ioctl(int cmd, unsigned long arg)
{
    switch (cmd) {
        case SENSOR_IOCTL_GET_INFO: {
            /* fill the dev info here */
            dev_sensor_info_t *info = (dev_sensor_info_t *)arg;
            info->model             = "SHTC1";
            info->unit              = dCelsius;
        } break;
        default:
            return -1;
    }

    LOG("%s %s successfully \n", SENSOR_STR, __func__);
    return 0;
}

static void drv_humi_sensirion_shtc1_irq_handle(void)
{
    /* no handle so far */
}

static int drv_humi_sensirion_shtc1_open(void)
{
    LOG("%s %s successfully \n", SENSOR_STR, __func__);

    return 0;
}

static int drv_humi_sensirion_shtc1_close(void)
{
    LOG("%s %s successfully \n", SENSOR_STR, __func__);

    return 0;
}

static int drv_humi_sensirion_shtc1_read(void *buf, size_t len)
{
    int              ret = 0;
    size_t           size;
    humidity_data_t *pdata = (humidity_data_t *)buf;

    if (buf == NULL) {
        return -1;
    }

    size = sizeof(humidity_data_t);
    if (len < size) {
        return -1;
    }

    ret = drv_shtc1_update_data(&shtc1_ctx);
    if (ret != 0) {
        return -1;
    }

    pdata->h = g_data_new.humidity / 100;

    pdata->timestamp = aos_now_ms();

    return (int)size;
}

static int drv_humi_sensirion_shtc1_write(const void *buf, size_t len)
{
    (void)buf;
    (void)len;
    return 0;
}

static int drv_humi_sensirion_shtc1_ioctl(int cmd, unsigned long arg)
{
    switch (cmd) {
        case SENSOR_IOCTL_GET_INFO: {
            /* fill the dev info here */
            dev_sensor_info_t *info = (dev_sensor_info_t *)arg;
            info->model             = "SHTC1";
            info->unit              = permillage;
        } break;
        default:
            return -1;
    }

    LOG("%s %s successfully \n", SENSOR_STR, __func__);
    return 0;
}

int drv_temp_sensirion_shtc1_init(void)
{
    int          ret = 0;
    sensor_obj_t sensor_temp;

    memset(&sensor_temp, 0, sizeof(sensor_temp));

    if (!g_init_bitwise) {

        ret = drv_temp_humi_sensirion_shtc1_validate_id(
          &shtc1_ctx, SHTC1_CHIP_ID_VAL, SHTC1_ID_REG_MASK);
        if (unlikely(ret)) {
            return -1;
        }
    }

    if (!(g_init_bitwise & (1 << FLAG_INIT_TEMP))) {
        /* fill the sensor_temp obj parameters here */
        sensor_temp.tag        = TAG_DEV_TEMP;
        sensor_temp.path       = dev_temp_path;
        sensor_temp.io_port    = I2C_PORT;
        sensor_temp.open       = drv_temp_sensirion_shtc1_open;
        sensor_temp.close      = drv_temp_sensirion_shtc1_close;
        sensor_temp.read       = drv_temp_sensirion_shtc1_read;
        sensor_temp.write      = drv_temp_sensirion_shtc1_write;
        sensor_temp.ioctl      = drv_temp_sensirion_shtc1_ioctl;
        sensor_temp.irq_handle = drv_temp_sensirion_shtc1_irq_handle;

        ret = sensor_create_obj(&sensor_temp);
        if (unlikely(ret)) {
            return -1;
        }

        g_init_bitwise |= 1 << FLAG_INIT_TEMP;
    }

    LOG("%s %s successfully \n", SENSOR_STR, __func__);
    return 0;
}

int drv_humi_sensirion_shtc1_init(void)
{
    int          ret = 0;
    sensor_obj_t sensor_humi;

    memset(&sensor_humi, 0, sizeof(sensor_humi));

    if (!g_init_bitwise) {

        ret = drv_temp_humi_sensirion_shtc1_validate_id(
          &shtc1_ctx, SHTC1_CHIP_ID_VAL, SHTC1_ID_REG_MASK);
        if (unlikely(ret)) {
            return -1;
        }
    }

    if (!(g_init_bitwise & (1 << FLAG_INIT_HUMI))) {
        /* fill the sensor_humi obj parameters here */
        sensor_humi.tag        = TAG_DEV_HUMI;
        sensor_humi.path       = dev_humi_path;
        sensor_humi.io_port    = I2C_PORT;
        sensor_humi.open       = drv_humi_sensirion_shtc1_open;
        sensor_humi.close      = drv_humi_sensirion_shtc1_close;
        sensor_humi.read       = drv_humi_sensirion_shtc1_read;
        sensor_humi.write      = drv_humi_sensirion_shtc1_write;
        sensor_humi.ioctl      = drv_humi_sensirion_shtc1_ioctl;
        sensor_humi.irq_handle = drv_humi_sensirion_shtc1_irq_handle;

        ret = sensor_create_obj(&sensor_humi);
        if (unlikely(ret)) {
            return -1;
        }

        g_init_bitwise |= 1 << FLAG_INIT_HUMI;
    }

    LOG("%s %s successfully \n", SENSOR_STR, __func__);
    return 0;
}
SENSOR_DRV_ADD(drv_humi_sensirion_shtc1_init);
SENSOR_DRV_ADD(drv_temp_sensirion_shtc1_init);
