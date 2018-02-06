/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */



#ifndef BOARD_H_
#define BOARD_H_

#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include "aos/aos.h"

//#define USE_MK3060

#define USE_AOS

//#define OPTIMIZE_SIZE

#define USE_GLOBAL
#define USE_INTEGER
#define USE_MATH
#define USE_JSON
#define USE_STRING
#define USE_ARRAY


//#define USE_ALINK
#define USE_PROCESS
//#define USE_GPIO
#define USE_TIMERS
#define USE_UART

#define TRACE_JSE_INFO
#define DUMP_SYMBL_USAGE

// 根据实现情况配置GPIO或与HAL对应

/*
platform\mcu\beken\hal\gpio.c
static const uint8_t gpio_map[] =
{
    [1] = 30,
    [2] = 29,
    [3] = 17,
    [4] = 16,
    [5] = 14,
    [6] = 15,
    [7] = 0,
    [8] = 1,
    [11] = 20,
    [12] = 4,
    [17] = 23,
    [18] = 22,
    [19] = 11,
    [20] = 10,
    [21] = 21,
};

GPIO20 GPIO4 已经验证

*/
#define GPIO_COUNT 22

// GPIO id号, 与硬件对应
static const int gpio_id[GPIO_COUNT]= {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21};

//#define USE_ADC
#undef USE_ADC

#endif /* BOARD_H_ */



