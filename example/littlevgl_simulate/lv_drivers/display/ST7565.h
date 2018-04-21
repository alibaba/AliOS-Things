/**
 * @file ST7565.h
 * 
 */

#ifndef ST7565_H
#define ST7565_H

/*********************
 *      INCLUDES
 *********************/
#include "lv_drv_conf.h"
#if USE_ST7565 != 0

#include <stdint.h>
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
void st7565_init(void);
void st7565_flush(int32_t x1, int32_t y1, int32_t x2, int32_t y2, lv_color_t * color_p);
void st7565_fill(int32_t x1, int32_t y1, int32_t x2, int32_t y2, lv_color_t color);
void st7565_map(int32_t x1, int32_t y1, int32_t x2, int32_t y2, lv_color_t * color_p);

/**********************
 *      MACROS
 **********************/

#endif

#endif
