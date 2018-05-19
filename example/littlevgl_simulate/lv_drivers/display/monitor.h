/**
 * @file monitor.h
 *
 */

#ifndef MONITOR_H
#define MONITOR_H

/*********************
 *      INCLUDES
 *********************/
#include "lv_drv_conf.h"

#if USE_MONITOR != 0
#include "lvgl/lv_misc/lv_color.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/
void monitor_init(void);
void monitor_flush(int32_t x1, int32_t y1, int32_t x2, int32_t y2, const lv_color_t *color_p);
void monitor_fill(int32_t x1, int32_t y1, int32_t x2, int32_t y2, lv_color_t color);
void monitor_map(int32_t x1, int32_t y1, int32_t x2, int32_t y2, const lv_color_t * color_p);

/**********************
 *      MACROS
 **********************/

#endif

#endif
