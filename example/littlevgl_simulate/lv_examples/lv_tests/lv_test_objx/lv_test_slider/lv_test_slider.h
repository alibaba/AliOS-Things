/**
 * @file lv_test_slider.h
 *
 */

#ifndef LV_TEST_SLIDER_H
#define LV_TEST_SLIDER_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "lvgl/lvgl.h"

#if USE_LV_SLIDER != 0

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
 * Create sliders to test their functionalities
 */
void lv_test_slider_1(void);

/**********************
 *      MACROS
 **********************/

#endif /*USE_LV_SLIDER*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_TEST_SLIDER_H*/
