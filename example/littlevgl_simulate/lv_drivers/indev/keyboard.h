/**
 * @file keyboard.h
 *
 */

#ifndef KEYBOARD_H
#define KEYBOARD_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "lv_drv_conf.h"
#if USE_KEYBOARD

#include <stdbool.h>
#include <SDL2/SDL.h>
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
 * Initialize the keyboard
 */
void keyboard_init(void);

/**
 * Get the last pressed or released character from the PC's keyboard
 * @param data store the read data here
 * @return false: because the points are not buffered, so no more data to be read
 */
bool keyboard_read(lv_indev_data_t * data);

/**********************
 *      MACROS
 **********************/

#endif /*USE_KEYBOARD*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*KEYBOARD_H*/
