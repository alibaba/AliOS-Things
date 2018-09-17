/**
 * @file lv_test_gauge.h
 *
 */

#ifndef LV_TEST_GAUGE_H
#define LV_TEST_GAUGE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../../../lv_ex_conf.h"   
#include "lvgl/lvgl.h"

#if USE_LV_GAUGE && USE_LV_TESTS

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
 * Create gauges to test their functionalities
 */
void lv_test_gauge_1(void);
/**********************
 *      MACROS
 **********************/

#endif /*USE_LV_GAUGE*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*USE_LV_GAUGE && USE_LV_TESTS*/
