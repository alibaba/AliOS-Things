/**
 * @file fbdev.h
 *
 */

#ifndef FBDEV_H
#define FBDEV_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "lv_drv_conf.h"
#if USE_FBDEV

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
void fbdev_init(void);
void fbdev_fill(int32_t x1, int32_t y1, int32_t x2, int32_t y2, lv_color_t color);
void fbdev_map(int32_t x1, int32_t y1, int32_t x2, int32_t y2, const lv_color_t * color_p);

/**********************
 *      MACROS
 **********************/

#endif  /*USE_FBDEV*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*FBDEV_H*/
