/**
 * @file terminal.h
 *
 */

#ifndef TERMINAL_H
#define TERMINAL_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "lvgl/lvgl.h"

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
 * Open a terminal
 * @return pointer to the terminal window
 */
lv_obj_t * terminal_create(void);

/**
 * Add data to the terminal
 * @param txt_in character sting to add to the terminal
 */
void terminal_add(const char * txt_in);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LV_TERMINAL_H */
