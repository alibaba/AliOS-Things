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
#include "hal/sensor.h"

#define LSM6DSL_ACC_MUL 1000
#define MPU9250_GYRO_SENSITIVITY_2000DPS  70000

static cur_acc_factor = 61;
static cur_gyro_factor = MPU9250_GYRO_SENSITIVITY_2000DPS;


extern short acc_can[3];
extern short gyro_can[3];
// extern float euler_can[3];

// static int drv_acc_st_lsm6dsl_close(void)
// {
//     int ret = 0;
//     ret  = drv_acc_st_lsm6dsl_set_power_mode(&lsm6dsl_ctx, DEV_POWER_OFF);
//     if(unlikely(ret)){
//         return -1;
//     }
//     return 0;
// }

static int drv_acc_inv_mpu9250_open(void)
{ 
    return 0;
}


static int drv_acc_inv_mpu9250_read(void *buf, size_t len)
{
    int ret = 0;
    size_t size;
    uint8_t reg[6];
    accel_data_t *accel = (accel_data_t *)buf;
    if(buf == NULL){
        return -1;
    }

    size = sizeof(accel_data_t);
    if(len < size){
        return -1;
    }

  
    accel->data[DATA_AXIS_X] = acc_can[0];
    accel->data[DATA_AXIS_Y] = acc_can[1];
    accel->data[DATA_AXIS_Z] = acc_can[2];

    if(cur_acc_factor != 0){
        accel->data[DATA_AXIS_X] = (accel->data[DATA_AXIS_X] * cur_acc_factor)/LSM6DSL_ACC_MUL;
        accel->data[DATA_AXIS_Y] = (accel->data[DATA_AXIS_Y] * cur_acc_factor)/LSM6DSL_ACC_MUL;
        accel->data[DATA_AXIS_Z] = (accel->data[DATA_AXIS_Z] * cur_acc_factor)/LSM6DSL_ACC_MUL;
    }
    
    accel->timestamp = aos_now_ms();

    return (int)size;
}


int drv_acc_inv_mpu9250_init(void){
    int ret = 0;
    sensor_obj_t sensor;
    /* fill the sensor obj parameters here */
    sensor.io_port    = CAN_PORT;
    sensor.tag        = TAG_DEV_ACC;
    sensor.path       = dev_acc_path;
    sensor.open       = drv_acc_inv_mpu9250_open;
    sensor.close      = NULL;
    sensor.read       = drv_acc_inv_mpu9250_read;
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
static int drv_gyro_inv_mpu9250_open(void)
{ 
    return 0;
}


static int drv_gyro_inv_mpu9250_read(void *buf, size_t len)
{
    int ret = 0;
    size_t size;
    uint8_t reg[6];
    gyro_data_t *gyro = (gyro_data_t *)buf;
    if(buf == NULL){
        return -1;
    }

    size = sizeof(gyro_data_t);
    if(len < size){
        return -1;
    }

  
    gyro->data[DATA_AXIS_X] = gyro_can[0];
    gyro->data[DATA_AXIS_Y] = gyro_can[1];
    gyro->data[DATA_AXIS_Z] = gyro_can[2];

    if(cur_gyro_factor != 0){
         gyro->data[DATA_AXIS_X] = (gyro->data[DATA_AXIS_X] * cur_gyro_factor);
         gyro->data[DATA_AXIS_Y] = (gyro->data[DATA_AXIS_Y] * cur_gyro_factor);
         gyro->data[DATA_AXIS_Z] = (gyro->data[DATA_AXIS_Z] * cur_gyro_factor);
     }
    
    gyro->timestamp = aos_now_ms();

    return (int)size;
}


int drv_gyro_inv_mpu9250_init(void){
    int ret = 0;
    sensor_obj_t sensor;
    /* fill the sensor obj parameters here */
    sensor.io_port    = CAN_PORT;
    sensor.tag        = TAG_DEV_GYRO;
    sensor.path       = dev_gyro_path;
    sensor.open       = drv_gyro_inv_mpu9250_open;
    sensor.close      = NULL;
    sensor.read       = drv_gyro_inv_mpu9250_read;
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


