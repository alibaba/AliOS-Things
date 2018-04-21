/**
 * @file lv_test_btn.h
 *
 */

#ifndef LV_TEST_BTN_H
#define LV_TEST_BTN_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "lvgl/lvgl.h"

#if USE_LV_BTN != 0

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
 * Create buttons to test their functionalities
 */
void lv_test_btn_1(void);

/**********************
 *      MACROS
 **********************/

#endif /*USE_LV_BTN*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_TEST_BTN_H*/
