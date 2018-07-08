/**
 * @file lv_tutorial_animation.h
 *
 */

#ifndef LV_TUTORIAL_ANIMATION_H
#define LV_TUTORIAL_ANIMATION_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../../lv_ex_conf.h"
#include "lvgl/lvgl.h"

#if USE_LV_TUTORIALS && USE_LV_ANIMATION

    
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
 * Crate some objects an animate them
 */
void lv_tutorial_animations(void);

/**********************
 *      MACROS
 **********************/

#endif /*USE_LV_TUTORIALS*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_TUTORIAL_ANTMATION_H*/
