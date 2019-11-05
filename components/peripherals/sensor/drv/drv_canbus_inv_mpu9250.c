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
#include "sensor_hal.h"


#define LSM6DSL_ACC_MUL 1000
#define MPU9250_GYRO_SENSITIVITY_2000DPS  70000

static int32_t cur_acc_factor = 61;
static int32_t cur_gyro_factor = MPU9250_GYRO_SENSITIVITY_2000DPS;

extern void CAN1_dataReceive(void* rxDataBuffer, uint8_t dataLen);


static int drv_canbus_acc_inv_mpu9250_open(void)
{
    return 0;
}


static int drv_canbus_acc_inv_mpu9250_read(void *buf, size_t len)
{
    size_t size;
    short rxAccData[6] = {0};
    accel_data_t *accel = (accel_data_t *)buf;
    if(buf == NULL){
        return -1;
    }

    size = sizeof(accel_data_t);
    if(len < size){
        return -1;
    }

    CAN1_dataReceive(rxAccData, 6);
    accel->data[DATA_AXIS_X] = rxAccData[0];
    accel->data[DATA_AXIS_Y] = rxAccData[1];
    accel->data[DATA_AXIS_Z] = rxAccData[2];

    if(cur_acc_factor != 0){
        accel->data[DATA_AXIS_X] = (accel->data[DATA_AXIS_X] * cur_acc_factor)/LSM6DSL_ACC_MUL;
        accel->data[DATA_AXIS_Y] = (accel->data[DATA_AXIS_Y] * cur_acc_factor)/LSM6DSL_ACC_MUL;
        accel->data[DATA_AXIS_Z] = (accel->data[DATA_AXIS_Z] * cur_acc_factor)/LSM6DSL_ACC_MUL;
    }

    accel->timestamp = aos_now_ms();

    return (int)size;
}


int drv_canbus_acc_inv_mpu9250_init(void){
    int ret = 0;
    sensor_obj_t sensor;
    memset(&sensor, 0, sizeof(sensor));
    /* fill the sensor obj parameters here */
    sensor.io_port    = CAN_PORT;
    sensor.tag        = TAG_DEV_ACC;
    sensor.path       = dev_acc_path;
    sensor.open       = drv_canbus_acc_inv_mpu9250_open;
    sensor.close      = NULL;
    sensor.read       = drv_canbus_acc_inv_mpu9250_read;
    sensor.write      = NULL;
    sensor.ioctl      = NULL;
    sensor.irq_handle = NULL;

    ret = sensor_create_obj(&sensor);
    if(unlikely(ret)){
        return -1;
    }

    LOG("%s %s successfully \n", SENSOR_STR, __func__);
    return 0;
}



//gyro
static int drv_canbus_gyro_inv_mpu9250_open(void)
{
    return 0;
}


static int drv_canbus_gyro_inv_mpu9250_read(void *buf, size_t len)
{
    size_t size;
    short rxGyroData[6] = {0};
    gyro_data_t *gyro = (gyro_data_t *)buf;
    if(buf == NULL){
        return -1;
    }

    size = sizeof(gyro_data_t);
    if(len < size){
        return -1;
    }

    CAN1_dataReceive(rxGyroData, 6);
    gyro->data[DATA_AXIS_X] = rxGyroData[3];
    gyro->data[DATA_AXIS_Y] = rxGyroData[4];
    gyro->data[DATA_AXIS_Z] = rxGyroData[5];

    if(cur_gyro_factor != 0){
         gyro->data[DATA_AXIS_X] = (gyro->data[DATA_AXIS_X] * cur_gyro_factor);
         gyro->data[DATA_AXIS_Y] = (gyro->data[DATA_AXIS_Y] * cur_gyro_factor);
         gyro->data[DATA_AXIS_Z] = (gyro->data[DATA_AXIS_Z] * cur_gyro_factor);
     }

    gyro->timestamp = aos_now_ms();

    return (int)size;
}


int drv_canbus_gyro_inv_mpu9250_init(void){
    int ret = 0;
    sensor_obj_t sensor;
    memset(&sensor, 0, sizeof(sensor));
    /* fill the sensor obj parameters here */
    sensor.io_port    = CAN_PORT;
    sensor.tag        = TAG_DEV_GYRO;
    sensor.path       = dev_gyro_path;
    sensor.open       = drv_canbus_gyro_inv_mpu9250_open;
    sensor.close      = NULL;
    sensor.read       = drv_canbus_gyro_inv_mpu9250_read;
    sensor.write      = NULL;
    sensor.ioctl      = NULL;
    sensor.irq_handle = NULL;

    ret = sensor_create_obj(&sensor);
    if(unlikely(ret)){
        return -1;
    }

    LOG("%s %s successfully \n", SENSOR_STR, __func__);
    return 0;
}

SENSOR_DRV_ADD(drv_canbus_acc_inv_mpu9250_init);
SENSOR_DRV_ADD(drv_canbus_gyro_inv_mpu9250_init);


