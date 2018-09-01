/**
 * @file SSD1963.h
 * 
 */

#ifndef SSD1963_H
#define SSD1963_H

/*********************
 *      INCLUDES
 *********************/
#include "lv_drv_conf.h"
#if USE_SSD1963 != 0

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
void ssd1963_init(void);
void ssd1963_flush(int32_t x1, int32_t y1, int32_t x2, int32_t y2, const lv_color_t * color_p);
void ssd1963_fill(int32_t x1, int32_t y1, int32_t x2, int32_t y2, lv_color_t  color);
void ssd1963_map(int32_t x1, int32_t y1, int32_t x2, int32_t y2, const lv_color_t * color_p);

/**********************
 *      MACROS
 **********************/

#endif

#endif
