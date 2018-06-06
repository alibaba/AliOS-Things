/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

/*
 * be_jse_feature.h
 *
 * JavaScript feature 相关宏定义
 * 与对应硬件board相关
 *
 */

#ifndef BE_JSE_FEATURE_H_
#define BE_JSE_FEATURE_H_

//#ifdef LINUX

#define USE_GLOBAL
#define USE_INTEGER
#define USE_MATH
#define USE_JSON
#define USE_STRING
#define USE_ARRAY

//#endif

#define USE_ALINK
#define USE_PROCESS
#define USE_GPIO

#define USE_MODULES
//#define BE_JSE_SILENT

#define DUMP_SYMBL_USAGE
#define TRACE_JSE_INFO


/*
aos make xxx@linuxhost
linuxhost 不支持gpio
undefined reference to `hal_gpio_init'
undefined reference to `hal_gpio_output_high'
undefined reference to `hal_gpio_output_low'
undefined reference to `hal_gpio_input_get'
*/

#ifdef CSP_LINUXHOST

#undef USE_GPIO

#endif

#undef USE_PROCESS
#undef USE_GPIO

#endif /* BE_JSE_FEATURE_H_ */
