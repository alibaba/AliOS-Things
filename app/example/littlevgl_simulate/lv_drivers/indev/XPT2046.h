/**
 * @file XPT2046.h
 * 
 */

#ifndef XPT2046_H
#define XPT2046_H

/*********************
 *      INCLUDES
 *********************/
#include "lv_drv_conf.h"
#if USE_XPT2046 != 0

#include <stdint.h>
#include <stdbool.h>
#include "lvgl/lv_hal/lv_hal_indev.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/
void xpt2046_init(void);
bool xpt2046_read(lv_indev_data_t * data);

/**********************
 *      MACROS
 **********************/

#endif

#endif
