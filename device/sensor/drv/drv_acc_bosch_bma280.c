/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 *
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <aos/aos.h>
#include <vfs_conf.h>
#include <vfs_err.h>
#include <vfs_register.h>
#include <hal/base.h>
#include "common.h"
#include "sensor.h"
#include "sensor_drv_api.h"
#include "sensor_hal.h"


#define BMA280_I2C_ADDR1                    (0x18)
#define BMA280_I2C_ADDR2                    (0x19)
#define BMA280_I2C_ADDR3                    (0x10)
#define BMA280_I2C_ADDR4                    (0x11)
#define BMA280_I2C_ADDR_TRANS(n) ((n) << 1)
#define BMA280_I2C_ADDR                     BMA280_I2C_ADDR_TRANS(BMA280_I2C_ADDR1)

#define BMA280_CHIP_ID_ADDR                 (0x00)
#define BMA280_CHIP_ID_VALUE                (0xFB)
#define BMA280_DEFAULT_ODR                  (100)

i2c_dev_t bma280_ctx                             = {
    .port                 = 3,
    .config.address_width = 8,
    .config.freq          = 400000,
    .config.dev_addr      = BMA280_I2C_ADDR,
};
extern int drv_acc_bosch_bma253_validate_id(i2c_dev_t *drv, uint8_t id_value);
extern int drv_acc_bosch_bma253_set_odr(i2c_dev_t *drv, uint32_t odr);
extern int drv_acc_bosch_bma253_set_range(i2c_dev_t *drv, uint32_t range);
extern int drv_acc_bosch_bma253_set_power_mode(i2c_dev_t *      drv, dev_power_mode_e mode);
extern int drv_acc_bosch_bma253_workmode_set(work_mode_e mode);
extern int drv_acc_bosch_bma253_soft_reset(i2c_dev_t *drv);
extern int drv_acc_bosch_bma253_soft_reset(i2c_dev_t *drv);
extern int drv_acc_bosch_bma253_open(void);
extern int drv_acc_bosch_bma253_close(void);
extern int drv_acc_bosch_bma253_read(void *buf, size_t len);
extern void drv_acc_bosch_bma253_irq_handle(void);

static int drv_acc_bosch_bma280_ioctl(int cmd, unsigned long arg)
{
    int ret = 0;

    switch (cmd) {
        case SENSOR_IOCTL_ODR_SET: {
            ret = drv_acc_bosch_bma253_set_odr(&bma280_ctx, arg);
            if (unlikely(ret)) {
                return -1;
            }
        } break;
        case SENSOR_IOCTL_RANGE_SET: {
            ret = drv_acc_bosch_bma253_set_range(&bma280_ctx, arg);
            if (unlikely(ret)) {
                return -1;
            }
        } break;
        case SENSOR_IOCTL_SET_POWER: {
            ret = drv_acc_bosch_bma253_set_power_mode(&bma280_ctx, arg);
            if (unlikely(ret)) {
                return -1;
            }
        } break;
        case SENSOR_IOCTL_GET_INFO: {
            /* fill the dev info here */
            dev_sensor_info_t *info = (dev_sensor_info_t *)arg;
            info->model             = "BMA280";
            info->range_max         = 16;
            info->range_min         = 2;
            info->unit              = mg;
        } break;

        default:
            break;
    }

    return 0;
}

int drv_acc_bosch_bma280_init(void)
{
    int          ret = 0;
    sensor_obj_t sensor;

    memset(&sensor, 0, sizeof(sensor));

    /* fill the sensor obj parameters here */
    sensor.io_port     = I2C_PORT;
    sensor.tag         = TAG_DEV_ACC;
    sensor.path        = dev_acc_path;
    sensor.open        = drv_acc_bosch_bma253_open;
    sensor.close       = drv_acc_bosch_bma253_close;
    sensor.read        = drv_acc_bosch_bma253_read;
    sensor.write       = NULL;
    sensor.ioctl       = drv_acc_bosch_bma280_ioctl;
    sensor.irq_handle  = drv_acc_bosch_bma253_irq_handle;
    sensor.mode        = DEV_POLLING;
    sensor.data_len = sizeof(accel_data_t);

    ret = drv_acc_bosch_bma253_workmode_set(sensor.mode);
    if (unlikely(ret)) {
        return -1;
    }


    ret = sensor_create_obj(&sensor);
    if (unlikely(ret)) {
        return -1;
    }

    ret = drv_acc_bosch_bma253_validate_id(&bma280_ctx, BMA280_CHIP_ID_VALUE);
    if (unlikely(ret)) {
        return -1;
    }

    ret = drv_acc_bosch_bma253_soft_reset(&bma280_ctx);
    if (unlikely(ret)) {
        return -1;
    }

    aos_msleep(5);
    ret = drv_acc_bosch_bma253_set_range(&bma280_ctx, ACC_RANGE_8G);
    if (unlikely(ret)) {
        return -1;
    }

    // set odr is 100hz, and will update
    ret = drv_acc_bosch_bma253_set_odr(&bma280_ctx, BMA280_DEFAULT_ODR);
    if (unlikely(ret)) {
        return -1;
    }
    ret = drv_acc_bosch_bma253_set_power_mode(&bma280_ctx, DEV_SLEEP);
    if (unlikely(ret)) {
        return -1;
    }

    /* update the phy sensor info to sensor hal */
    LOG("%s %s successfully \n", SENSOR_STR, __func__);
    return 0;
}
