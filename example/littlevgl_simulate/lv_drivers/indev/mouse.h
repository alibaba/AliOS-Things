/**
 * @file mouse.h
 *
 */

#ifndef MOUSE_H
#define MOUSE_H

/*********************
 *      INCLUDES
 *********************/

#include "lv_drv_conf.h"

#if USE_MOUSE
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

/**
 * Initialize the mouse
 */
void mouse_init(void);
/**
 * Get the current position and state of the mouse
 * @param data store the mouse data here
 * @return false: because the points are not buffered, so no more data to be read
 */
bool mouse_read(lv_indev_data_t * data);


/**********************
 *      MACROS
 **********************/

#endif

#endif
