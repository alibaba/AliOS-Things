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
#include "lvgl/lvgl.h"

#if USE_LV_GAUGE != 0

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

#endif /*LV_TEST_GAUGE_H*/
