/*
* Copyright (C) 2015-2017 Alibaba Group Holding Limited
*
*
*/

#include <aos/aos.h>
#include <hal/base.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vfs_conf.h>
#include <vfs_err.h>
#include <vfs_register.h>
#include "common.h"
#include "sensor.h"
#include "sensor_drv_api.h"
#include "sensor_hal.h"

#define BMA422_I2C_ADDR_LOW                    (0x18)
#define BMA422_I2C_ADDR_HIGH                   (0x19)
#define BMA422_I2C_ADDR_TRANS(n)               ((n)<<1)
#define BMA422_I2C_ADDR                        BMA422_I2C_ADDR_TRANS(BMA422_I2C_ADDR_LOW)
#define BMA422_DEFAULT_ODR_100HZ               (100)
#define BMA422_CHIP_ID_VALUE                   (0x12)

int drv_acc_bosch_bma4xy_validate_id(i2c_dev_t* drv);
int drv_acc_bosch_bma4xy_soft_reset(i2c_dev_t* drv);
int drv_acc_bosch_bma4xy_set_range(i2c_dev_t* drv, uint32_t range);
int drv_acc_bosch_bma4xy_set_odr(i2c_dev_t* drv, uint32_t hz);
int drv_acc_bosch_bma4xy_set_power_mode(i2c_dev_t* drv, dev_power_mode_e mode);
int drv_acc_bosch_bma4xy_open(void);
int drv_acc_bosch_bma4xy_close(void);
int drv_acc_bosch_bma4xy_read(void *buf, size_t len);
void drv_acc_bosch_bma4xy_irq_handle(void);

i2c_dev_t bma422_ctx = {
    .port = 3,
    .config.address_width = 8,
    .config.freq = 400000,
    .config.dev_addr = BMA422_I2C_ADDR,
};


static int drv_acc_bosch_bma422_ioctl(int cmd, unsigned long arg)
{
    int ret = 0;

    switch(cmd) {
        case SENSOR_IOCTL_ODR_SET: {
            ret = drv_acc_bosch_bma4xy_set_odr(&bma422_ctx, arg);
            if(unlikely(ret)) {
                return -1;
            }
        }
        break;
        case SENSOR_IOCTL_RANGE_SET: {
            ret = drv_acc_bosch_bma4xy_set_range(&bma422_ctx, arg);
            if(unlikely(ret)) {
                return -1;
            }
        }
        break;
        case SENSOR_IOCTL_SET_POWER: {
            ret = drv_acc_bosch_bma4xy_set_power_mode(&bma422_ctx, arg);
            if(unlikely(ret)) {
                return -1;
            }
        }
        break;
        case SENSOR_IOCTL_GET_INFO: {
            /* fill the dev info here */
            dev_sensor_info_t *info = (dev_sensor_info_t *)arg;
            info->model = "BMA422";
            info->range_max = 16;
            info->range_min = 2;
            info->unit = mg;
        }
        break;

        default:
            break;
    }

    return 0;
}


int drv_acc_bosch_bma422_init(void) {
    int ret = 0;
    sensor_obj_t sensor;
    memset(&sensor, 0, sizeof(sensor));

    /* fill the sensor obj parameters here */
    sensor.io_port    = I2C_PORT;
    sensor.tag        = TAG_DEV_ACC;
    sensor.path       = dev_acc_path;
    sensor.open       = drv_acc_bosch_bma4xy_open;
    sensor.close      = drv_acc_bosch_bma4xy_close;
    sensor.read       = drv_acc_bosch_bma4xy_read;
    sensor.write      = NULL;
    sensor.ioctl      = drv_acc_bosch_bma422_ioctl;
    sensor.irq_handle = drv_acc_bosch_bma4xy_irq_handle;
    printf("%s %s successfully entered \n", SENSOR_STR, __func__);
    ret = sensor_create_obj(&sensor);
    if(unlikely(ret)) {
        printf("%s %s  sensor_create_obj \n", SENSOR_STR, __func__);
        return -1;
    }

    ret = drv_acc_bosch_bma4xy_validate_id(&bma422_ctx);
    if(unlikely(ret)) {
        printf("%s %s  drv_acc_bosch_bma4xy_validate_id \n", SENSOR_STR, __func__);
        return -1;
    }

    ret = drv_acc_bosch_bma4xy_soft_reset(&bma422_ctx);
    if(unlikely(ret)) {
        printf("%s %s  drv_acc_bosch_bma4xy_soft_reset \n", SENSOR_STR, __func__);
        return -1;
    }

    ret = drv_acc_bosch_bma4xy_set_range(&bma422_ctx, ACC_RANGE_8G);
    if(unlikely(ret)) {
        printf("%s %s  drv_acc_bosch_bma4xy_set_range \n", SENSOR_STR, __func__);
        return -1;
    }

    /* set odr is 100hz, and will update */
    ret = drv_acc_bosch_bma4xy_set_odr(&bma422_ctx, BMA422_DEFAULT_ODR_100HZ);
    if(unlikely(ret)) {
        printf("%s %s  drv_acc_bosch_bma4xy_set_odr \n", SENSOR_STR, __func__);
        return -1;
    }
    ret = drv_acc_bosch_bma4xy_set_power_mode(&bma422_ctx, DEV_SLEEP);
    if(unlikely(ret)) {
        printf("%s %s  drv_acc_bosch_bma4xy_set_power_mode \n", SENSOR_STR, __func__);
        return -1;
    }

    /* update the phy sensor info to sensor hal */
    printf("%s %s successfully \n", SENSOR_STR, __func__);
    return 0;
}
