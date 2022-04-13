/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */

#ifndef _IO_DAC_H_
#define _IO_DAC_H_

#include <stdbool.h>

#define IOC_DAC_BASE 'D'
#define IOC_DAC_START IOC_DAC_BASE + 1 // set timer parameters
#define IOC_DAC_STOP IOC_DAC_BASE + 2 // get timer parameters
#define IOC_DAC_SET_VALUE IOC_DAC_BASE + 3 // start/stop timer
#define IOC_DAC_GET_VALUE IOC_DAC_BASE + 4 // auto reload timer or not

#define CHANNEL_ID_BIT 8
#define CHANNEL_ID_MASK 0x000000FF
#define DAC_VALUE_MASK 0xFFFFFF00

#define MAKE_DAC_ARG(c, v) ((c & CHANNEL_ID_MASK) | (v << CHANNEL_ID_BIT))
#define GET_DAC_CH(x) (x & CHANNEL_ID_MASK)
#define GET_DAC_VAL(x) ((x & DAC_VALUE_MASK) >> CHANNEL_ID_BIT)

#endif //_IO_DAC_H_
