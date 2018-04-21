/**
 * @file lv_test_chart.h
 *
 */

#ifndef LV_TEST_CHART_H
#define LV_TEST_CHART_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "lvgl/lvgl.h"

#if USE_LV_CHART != 0

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
 * Create charts to test their functionalities
 */
void lv_test_chart_1(void);

/**********************
 *      MACROS
 **********************/

#endif /*USE_LV_CHART*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_TEST_CHART_H*/
