
/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */

#ifndef _CH8305_H_
#define _CH8305_H_
#include "aos/hal/i2c.h"
#include <math.h>
#include <stdbool.h>

extern void cht8305_init();
extern void cht8305_getTempHumidity(float *humidity, float *temperature);
extern void cht8305_deinit(void);
#endif
