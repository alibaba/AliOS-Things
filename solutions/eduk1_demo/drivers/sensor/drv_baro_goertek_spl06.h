/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#include <stdio.h>

#ifndef SENSOR_SPI06_H
#define SENSOR_SPI06_H

#ifdef __cplusplus
extern "C" {
#endif
typedef struct {
    uint8_t id;
    double  Ctemp;
    double  Ftemp;
    double  pressure;
    double  altitude;
} spl06_data_t;

void spl06_init(void);
void spl06_getdata(spl06_data_t *p);

#ifdef __cplusplus
}
#endif

#endif
