/**
 * @file lv_tutorial_antialiasing.h
 *
 */

#ifndef LV_TUTORIAL_ANTIALIASING_H
#define LV_TUTORIAL_ANTIALIASING_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../../lv_ex_conf.h"
#if USE_LV_TUTORIALS
    
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
 * Create object to see how they change from the anti aliasing
 * Modify LV_ANTIALIAS and LV_FONT_ANTIALIAS to see what is changing
 */
void lv_tutorial_antialiasing(void);

/**********************
 *      MACROS
 **********************/

#endif /*USE_LV_TUTORIALS*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_TUTORIAL_ANTIALIASING_H*/
