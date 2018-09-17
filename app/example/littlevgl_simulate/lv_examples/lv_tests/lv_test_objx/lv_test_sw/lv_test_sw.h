/**
 * @file lv_test_sw.h
 *
 */

#ifndef LV_TEST_SW_H
#define LV_TEST_SW_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../../../lv_ex_conf.h"   
#include "lvgl/lvgl.h"

#if USE_LV_SW && USE_LV_TESTS

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
 * Create switches to test their functionalities
 */
void lv_test_sw_1(void);

/**********************
 *      MACROS
 **********************/

#endif /*USE_LV_SW && USE_LV_TESTS*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_TEST_SW_H*/
