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

#define USE_AOS

#define USE_GLOBAL
#define USE_INTEGER
#define USE_MATH
#define USE_JSON
#define USE_STRING
#define USE_ARRAY



#define USE_ALINK
#define USE_PROCESS
#define USE_GPIO
#define USE_TIMERS
#define USE_UART

#define TRACE_JSE_INFO
#define DUMP_SYMBL_USAGE

// 根据实现情况配置GPIO
#define GPIO_COUNT 5

// GPIO id号, 与硬件对应
static const int gpio_id[GPIO_COUNT]= {1,2,3,4,5};


/*
aos make xxx@linuxhost
linuxhost 不支持gpio
undefined reference to `hal_gpio_init'
undefined reference to `hal_gpio_output_high'
undefined reference to `hal_gpio_output_low'
undefined reference to `hal_gpio_input_get'
*/

#undef USE_GPIO


#endif /* BOARD_H_ */



