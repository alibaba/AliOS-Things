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

#define BMI088_ACC_I2C_ADDR_LOW                    (0x18)
#define BMI088_ACC_I2C_ADDR_HIGH                   (0x19)
#define BMI088_ACC_I2C_ADDR_TRANS(n)               ((n)<<1)
#define BMI088_ACC_I2C_ADDR                         BMI088_ACC_I2C_ADDR_TRANS(BMI088_ACC_I2C_ADDR_LOW)

#define BMI088_ACC_CHIP_ID_ADDR                      UINT8_C(0X00)
#define BMI088_ACC_CHIP_ID_VALUE                    (0x1E)

#define BMI088_ACC_DEFAULT_ODR                      (100)

#define BMI088_GYRO_I2C_ADDR1                       (0x68 << 1)
#define BMI088_GYRO_I2C_ADDR2                       (0x69 << 1)

#define BMI088_GYRO_CHIP_ID_ADDR                    (0x00)
#define BMI088_GYRO_CHIP_ID_VALUE                   (0x0F)

#define BMI088_GYRO_DEFAULT_ODR                      (200)
i2c_dev_t bmi088_acc_ctx = {
    .port = 3,
    .config.address_width = 8,
    .config.freq = 400000,
    .config.dev_addr = BMI088_ACC_I2C_ADDR,
};

i2c_dev_t bmi088_gyro_ctx = {
    .port                 = 1,
    .config.address_width = 8,
    .config.freq          = 400000,
    .config.dev_addr      = BMI088_GYRO_I2C_ADDR1,
};

extern int drv_acc_bosch_bma4xy_validate_id(i2c_dev_t* drv);
extern int drv_acc_bosch_bma4xy_soft_reset(i2c_dev_t* drv);
extern int drv_acc_bosch_bma4xy_set_range(i2c_dev_t* drv, uint32_t range);
extern int drv_acc_bosch_bma4xy_set_odr(i2c_dev_t* drv, uint32_t hz);
extern int drv_acc_bosch_bma4xy_set_power_mode(i2c_dev_t* drv, dev_power_mode_e mode);
extern int drv_acc_bosch_bma4xy_open(void);
extern int drv_acc_bosch_bma4xy_close(void);
extern int drv_acc_bosch_bma4xy_read(void *buf, size_t len);
extern void drv_acc_bosch_bma4xy_irq_handle(void);


extern int drv_gyro_bosch_bmg160_validate_id(i2c_dev_t* drv, uint8_t id_value);
extern int drv_gyro_bosch_bmg160_set_power_mode(i2c_dev_t* drv, dev_power_mode_e mode);
extern int drv_gyro_bosch_bmg160_set_odr(i2c_dev_t* drv, uint32_t odr);
extern int drv_gyro_bosch_bmg160_set_range(i2c_dev_t* drv, uint32_t range);
extern void drv_gyro_bosch_bmg160_irq_handle(void);
extern int drv_gyro_bosch_bmg160_open(void);
extern int drv_gyro_bosch_bmg160_close(void);
extern int drv_gyro_bosch_bmg160_read(void *buf, size_t len);


static int drv_acc_bosch_bmi088_ioctl(int cmd, unsigned long arg)
{
    int ret = 0;

    switch(cmd) {
        case SENSOR_IOCTL_ODR_SET: {
            ret = drv_acc_bosch_bma4xy_set_odr(&bmi088_acc_ctx, arg);
            if(unlikely(ret) != 0) {
                return -1;
            }
        }
        break;
        case SENSOR_IOCTL_RANGE_SET: {
            ret = drv_acc_bosch_bma4xy_set_range(&bmi088_acc_ctx, arg);
            if(unlikely(ret) != 0) {
                return -1;
            }
        }
        break;
        case SENSOR_IOCTL_SET_POWER: {
            ret = drv_acc_bosch_bma4xy_set_power_mode(&bmi088_acc_ctx, arg);
            if(unlikely(ret) != 0) {
                return -1;
            }
        }
        break;
        case SENSOR_IOCTL_GET_INFO: {
            /* fill the dev info here */
            dev_sensor_info_t *info = (dev_sensor_info_t *)arg;
            info->model = "BMI088";
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


int drv_acc_bosch_bmi088_init(void) {
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
    sensor.ioctl      = drv_acc_bosch_bmi088_ioctl;
    sensor.irq_handle = drv_acc_bosch_bma4xy_irq_handle;
    printf("%s %s successfully entered \n", SENSOR_STR, __func__);
    ret = sensor_create_obj(&sensor);
    if(unlikely(ret) != 0) {
        printf("%s %s  sensor_create_obj \n", SENSOR_STR, __func__);
        return -1;
    }

    ret = drv_acc_bosch_bma4xy_validate_id(&bmi088_acc_ctx);
    if(unlikely(ret) != 0) {
        printf("%s %s  drv_acc_bosch_bma4xy_validate_id \n", SENSOR_STR, __func__);
        return -1;
    }

    ret = drv_acc_bosch_bma4xy_soft_reset(&bmi088_acc_ctx);
    if(unlikely(ret) != 0) {
        printf("%s %s  drv_acc_bosch_bma4xy_soft_reset \n", SENSOR_STR, __func__);
        return -1;
    }

    ret = drv_acc_bosch_bma4xy_set_range(&bmi088_acc_ctx, ACC_RANGE_8G);
    if(unlikely(ret) != 0) {
        printf("%s %s  drv_acc_bosch_bma4xy_set_range \n", SENSOR_STR, __func__);
        return -1;
    }

    /* set odr is 100hz, and will update */
    ret = drv_acc_bosch_bma4xy_set_odr(&bmi088_acc_ctx, BMI088_ACC_DEFAULT_ODR);
    if(unlikely(ret) != 0) {
        printf("%s %s  drv_acc_bosch_bma4xy_set_odr \n", SENSOR_STR, __func__);
        return -1;
    }
    ret = drv_acc_bosch_bma4xy_set_power_mode(&bmi088_acc_ctx, DEV_SLEEP);
    if(unlikely(ret) != 0) {
        printf("%s %s  drv_acc_bosch_bma4xy_set_power_mode \n", SENSOR_STR, __func__);
        return -1;
    }

    /* update the phy sensor info to sensor hal */
    printf("%s %s successfully \n", SENSOR_STR, __func__);
    return 0;
}

static int drv_gyro_bosch_bmi088_ioctl(int cmd, unsigned long arg)
{
    int ret = 0;

    switch (cmd) {
        case SENSOR_IOCTL_ODR_SET: {
            ret = drv_gyro_bosch_bmg160_set_odr(&bmi088_gyro_ctx, arg);
            if (unlikely(ret)) {
                return -1;
            }
        } break;
        case SENSOR_IOCTL_RANGE_SET: {
            ret = drv_gyro_bosch_bmg160_set_range(&bmi088_gyro_ctx, arg);
            if (unlikely(ret)) {
                return -1;
            }
        } break;
        case SENSOR_IOCTL_SET_POWER: {
            ret = drv_gyro_bosch_bmg160_set_power_mode(&bmi088_gyro_ctx, arg);
            if (unlikely(ret)) {
                return -1;
            }
        } break;
        case SENSOR_IOCTL_GET_INFO: {
            /* fill the dev info here */
            dev_sensor_info_t *info = (dev_sensor_info_t *)arg;
            info->model             = "BMI088";
            info->range_max         = 2000;
            info->range_min         = 125;
            info->unit              = udps;
        } break;

        default:
            break;
    }

    return 0;
}

int drv_gyro_bosch_bmi088_init(void)
{
    int          ret = 0;
    sensor_obj_t sensor;
    memset(&sensor, 0, sizeof(sensor));

    /* fill the sensor obj parameters here */
    sensor.io_port    = I2C_PORT;
    sensor.tag        = TAG_DEV_GYRO;
    sensor.path       = dev_gyro_path;
    sensor.open       = drv_gyro_bosch_bmg160_open;
    sensor.close      = drv_gyro_bosch_bmg160_close;
    sensor.read       = drv_gyro_bosch_bmg160_read;
    sensor.write      = NULL;
    sensor.ioctl      = drv_gyro_bosch_bmi088_ioctl;
    sensor.irq_handle = drv_gyro_bosch_bmg160_irq_handle;

    ret = sensor_create_obj(&sensor);
    if (unlikely(ret)) {
        return -1;
    }

    ret = drv_gyro_bosch_bmg160_validate_id(&bmi088_gyro_ctx, BMI088_GYRO_CHIP_ID_VALUE);
    if (unlikely(ret)) {
        return -1;
    }

    ret = drv_gyro_bosch_bmg160_set_range(&bmi088_gyro_ctx, GYRO_RANGE_2000DPS);
    if (unlikely(ret)) {
        return -1;
    }

    ret = drv_gyro_bosch_bmg160_set_odr(&bmi088_gyro_ctx, BMI088_GYRO_DEFAULT_ODR);
    if (unlikely(ret)) {
        return -1;
    }

    ret = drv_gyro_bosch_bmg160_set_power_mode(&bmi088_gyro_ctx, DEV_POWER_OFF);
    if (unlikely(ret)) {
        return -1;
    }

    /* update the phy sensor info to sensor hal */
    LOG("%s %s successfully \n", SENSOR_STR, __func__);
    return 0;
}
