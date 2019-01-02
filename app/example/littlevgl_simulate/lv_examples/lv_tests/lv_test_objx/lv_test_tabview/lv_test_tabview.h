/**
 * @file lv_test_tabview.h
 *
 */

#ifndef LV_TEST_TABVIEW_H
#define LV_TEST_TABVIEW_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../../../lv_ex_conf.h"   
#include "lvgl/lvgl.h"

#if USE_LV_TABVIEW && USE_LV_TESTS

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
 * Create tab views to test their functionalities
 */
void lv_test_tabview_1(void);

/**********************
 *      MACROS
 **********************/

#endif /*USE_LV_TABVIEW && USE_LV_TESTS*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_TEST_TABVIEW_H*/
