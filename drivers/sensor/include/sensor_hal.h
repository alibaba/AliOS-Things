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



#endif /* SENSOR_HAL_H */
