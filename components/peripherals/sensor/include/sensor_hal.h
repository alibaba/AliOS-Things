/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef SENSOR_HAL_H
#define SENSOR_HAL_H
#include <aos/init.h>
#include <aos/errno.h>
#include <aos/compiler.h>
#include "ulog/ulog.h"
#include "sensor/sensor.h"

#define SENSOR_DRV_ADD(func) \
int __sensor_##func##_func__(void){ \
    return func(); \
}

typedef int (*SENSOR_INIT_FUN)(void);

int sensor_init(void);
int sensor_create_obj(sensor_obj_t *sensor);

#ifdef UDATA_MODBUS
int modbus_init(void);
#endif

#endif /* SENSOR_HAL_H */
