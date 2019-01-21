/*
 * Copyright (C) 2019 X-Cite SA (http://www.x-cite.io)
 * Written by Lemmer El Assal (lemmer@x-cite.io)
*/

#include "sps30.h"
#include "sensor.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <aos/aos.h>
#include <vfs_conf.h>
#include <vfs_err.h>
#include <vfs_register.h>
#include <hal/base.h>
#include "common.h"
#include "sensor_drv_api.h"
#include "sensor_hal.h"


#ifndef SPS30_I2C_PORT
	#define SPS30_I2C_PORT 1
#endif

#ifndef SPS30_DATA_READ_MIN_INTERVAL
	#define SPS30_DATA_READ_MIN_INTERVAL 10000
#endif

#ifndef SPS30_I2C_FREQ
    #define SPS30_I2C_FREQ 10000
#endif

#ifndef SPS30_MAX_TRIES
    #define SPS30_MAX_TRIES 10
#endif

#ifndef REDTEXT
#define REDTEXT   "\x1B[31m"
#endif
#ifndef GREENTEXT
#define GREENTEXT   "\x1B[32m"
#endif
#ifndef RESETTEXT
#define RESETTEXT "\x1B[0m"
#endif

#ifndef TRACE_COLOR
#define TRACE_COLOR(color, type, fmt, ...)  \
    do { \
        HAL_Printf("%s%s|%03d :: [%s] ", color, __func__, __LINE__, type); \
        HAL_Printf(fmt, ##__VA_ARGS__); \
        HAL_Printf("%s" RESETTEXT, "\r\n"); \
    } while(0)
#endif

#ifndef TRACE_ERROR
#define TRACE_ERROR(fmt, ...) TRACE_COLOR(REDTEXT, "ERROR", fmt, ##__VA_ARGS__)
#endif

#ifndef TRACE_INFO
#define TRACE_INFO(fmt, ...) TRACE_COLOR(GREENTEXT, "INFO", fmt, ##__VA_ARGS__)
#endif


static struct sps30_measurement g_sps30_data_new;
sensor_obj_t sensor_pm;


i2c_dev_t sps30_ctx = {
    .port = SPS30_I2C_PORT,
	.config.freq = SPS30_I2C_FREQ,
	.config.address_width = I2C_MEM_ADDR_SIZE_8BIT,
    .config.dev_addr = SPS_I2C_ADDRESS,
}; 

void sensirion_i2c_init();
s8 sensirion_i2c_read(u8 address, u8* data, u16 count);
s8 sensirion_i2c_write(u8 address, const u8* data, u16 count);
void sensirion_sleep_usec(u32 useconds);


static int drv_sps30_update_data()
{
    static uint32_t prev_update_tick = 0;
    uint32_t now_tick = 0;
    int ret = 0;
    int tries = SPS30_MAX_TRIES;

    now_tick = aos_now_ms();
    if ((now_tick - prev_update_tick >= SPS30_DATA_READ_MIN_INTERVAL) || (prev_update_tick == 0)) {
		ret = sps30_start_measurement();
        if(ret) {
            TRACE_ERROR("[SPS30] Start measurement failed (returned %d)", ret);
            return ret;
        }
        // SPS30 should spin up here and requires some time to be ready.
        aos_msleep(2000);

        uint16_t dataready = 0;
        do {
            do {
                ret = sps30_read_data_ready(&dataready);
                if(ret) {
                    TRACE_ERROR("[SPS30] Data ready failed (returned %d)", ret);
                    return ret;
                }
                if((dataready & 1) == 0)
                    aos_msleep(1000); // delay by 1 s before retrying.
            } while((dataready & 1) == 0);

            if(ret == 0) {
                ret = sps30_read_measurement(&g_sps30_data_new);
                if (unlikely(ret != 0)) {
                    TRACE_ERROR("[SPS30] Read measurement failed (returned %d)", ret);
                    aos_msleep(1000); // delay by 1 s before retrying.
                    tries--;
                } else {
                    ret = sps30_stop_measurement();
                    if (unlikely(ret != 0)) {
                        TRACE_ERROR("[SPS30] Stop measurement failed (returned %d)", ret);
                        aos_msleep(1000); // delay by 1 s before retrying.
                        tries--;
                    }
                }
            } else
                tries--;
        } while(tries && ret);
        
        
        if(ret == 0)
            prev_update_tick = now_tick;
        return ret;
    }
    return 0;
}


static void drv_pm_sensirion_sps30_irq_handle(void)
{
    /* no handle so far */
}

static int drv_pm_sensirion_sps30_open(void)
{
    LOG("%s %s successfully \n", SENSOR_STR, __func__);

    return 0;
}

static int drv_pm_sensirion_sps30_close(void)
{
    LOG("%s %s successfully \n", SENSOR_STR, __func__);

    return 0;
}

static int drv_pm_sensirion_sps30_read(void *buf, size_t len)
{
    int ret = 0;
    const size_t size = sizeof(pm_data_t);
    pm_data_t *pdata = (pm_data_t*)buf;

    if ((buf == NULL) || (len < size)){
        return -1;
    }
    
    ret = drv_sps30_update_data();
    if(ret)
        return ret;

    pdata->mc_1p0 = g_sps30_data_new.mc_1p0;
    pdata->mc_2p5 = g_sps30_data_new.mc_2p5;
    pdata->mc_4p0 = g_sps30_data_new.mc_4p0;
    pdata->mc_10p0 = g_sps30_data_new.mc_10p0;
    pdata->nc_0p5 = g_sps30_data_new.nc_0p5;
    pdata->nc_1p0 = g_sps30_data_new.nc_1p0;
    pdata->nc_2p5 = g_sps30_data_new.nc_2p5;
    pdata->nc_4p0 = g_sps30_data_new.nc_4p0;
    pdata->nc_10p0 = g_sps30_data_new.nc_10p0;
    pdata->typical_particle_size = g_sps30_data_new.typical_particle_size;
    pdata->timestamp = aos_now_ms();

    return (int)size;
}

static int drv_pm_sensirion_sps30_write(const void *buf, size_t len)
{
    (void)buf;
    (void)len;
    return 0;
}


static int drv_pm_sensirion_sps30_ioctl(int cmd, unsigned long arg)
{
    switch (cmd) {
        case SENSOR_IOCTL_GET_INFO:
            {
                /* fill the dev info here */
                dev_sensor_info_t *info = (dev_sensor_info_t *)arg;
                info->model = "SPS30";
                info->vendor = DEV_SENSOR_VENDOR_SENSIRION;
            }
            break;
        default:
            return -1;
    }

    LOG("%s %s successfully \n", SENSOR_STR, __func__);
    return 0;
}

int32_t drv_sps30_init() {
    uint32_t ret = 0;
    
    sensirion_i2c_init();

    char serial[SPS_MAX_SERIAL_LEN];
    ret = sps30_get_serial(serial);
    if(!ret)
        TRACE_INFO("[SPS30] Serial: %s", serial);
    else
        TRACE_ERROR("[SPS30] Get serial failed (returned %d).", ret);

	memset(&sensor_pm, 0, sizeof(sensor_pm));
	/* fill the sensor_pm obj parameters here */
	sensor_pm.tag        = TAG_DEV_PM;
	sensor_pm.path       = dev_pm_path;
	sensor_pm.io_port    = SPS30_I2C_PORT;
	sensor_pm.open       = drv_pm_sensirion_sps30_open;
	sensor_pm.close      = drv_pm_sensirion_sps30_close;
	sensor_pm.read       = drv_pm_sensirion_sps30_read;
	sensor_pm.write      = drv_pm_sensirion_sps30_write;
	sensor_pm.ioctl      = drv_pm_sensirion_sps30_ioctl;
	sensor_pm.irq_handle = drv_pm_sensirion_sps30_irq_handle;
	ret = sensor_create_obj(&sensor_pm);
	if (unlikely(ret)) {
		return -1;
	}

    return ret;
}

/**
 * Initialize all hard- and software components that are needed for the I2C
 * communication.
 */

void sensirion_i2c_init()
{
    hal_i2c_init(&sps30_ctx);
}

/**
 * Execute one read transaction on the I2C bus, reading a given number of bytes.
 * If the device does not acknowledge the read command, an error shall be
 * returned.
 *
 * @param address 7-bit I2C address to read from
 * @param data    pointer to the buffer where the data is to be stored
 * @param count   number of bytes to read from I2C and store in the buffer
 * @returns 0 on success, error code otherwise
 */
s8 sensirion_i2c_read(u8 address, u8* data, u16 count)
{
    return hal_i2c_master_recv(&sps30_ctx, address, data, count, 1000);
}

/**
 * Execute one write transaction on the I2C bus, sending a given number of bytes.
 * The bytes in the supplied buffer must be sent to the given address. If the
 * slave device does not acknowledge any of the bytes, an error shall be
 * returned.
 *
 * @param address 7-bit I2C address to write to
 * @param data    pointer to the buffer containing the data to write
 * @param count   number of bytes to read from the buffer and send over I2C
 * @returns 0 on success, error code otherwise
 */
s8 sensirion_i2c_write(u8 address, const u8* data, u16 count)
{
    return hal_i2c_master_send(&sps30_ctx, address, data, count, 1000);
}

/**
 * Sleep for a given number of microseconds. The function should delay the
 * execution for at least the given time, but may also sleep longer.
 *
 * Despite the unit, a <10 millisecond precision is sufficient.
 *
 * @param useconds the sleep time in microseconds
 */
void sensirion_sleep_usec(u32 useconds) {
    int mseconds = (int) useconds / 1000;
    if(mseconds == 0)
        aos_msleep(1);
    else
        aos_msleep(mseconds);
}
