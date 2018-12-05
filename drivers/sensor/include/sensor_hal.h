/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef SENSOR_HAL_H
#define SENSOR_HAL_H

#include "sensor.h"

#define SENSOR_DRV_ADD(func) \
int __sensor_##func##_func__(void){ \
    return func(); \
}

typedef int (*SENSOR_INIT_FUN)(void);

extern int sensor_hal_init(void);
extern int sensor_create_obj(sensor_obj_t *sensor);

#ifdef UDATA_MODBUS
extern int find_ModbusSensors(char *path);
extern int modbus_sensor_read(void *buffer, size_t length, int index);
extern void modbus_sensor_drv_init();
#endif


#endif /* SENSOR_HAL_H */
