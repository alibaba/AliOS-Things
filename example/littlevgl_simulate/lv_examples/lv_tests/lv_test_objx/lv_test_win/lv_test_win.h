/**
 * @file lv_test_win.h
 *
 */

#ifndef LV_TEST_WIN_H
#define LV_TEST_WIN_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../../../lv_ex_conf.h"   
#include "lvgl/lvgl.h"

#if USE_LV_WIN && USE_LV_TESTS

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
 * Create windows to test their functionalities
 */
void lv_test_win_1(void);

/**********************
 *      MACROS
 **********************/

#endif /*USE_LV_WIN && USE_LV_TESTS*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_TEST_WIN_H*/
