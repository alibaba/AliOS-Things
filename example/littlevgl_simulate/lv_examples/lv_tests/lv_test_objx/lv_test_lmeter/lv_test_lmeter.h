/**
 * @file lv_test_lmeter.h
 *
 */

#ifndef LV_TEST_LMETER_H
#define LV_TEST_LMETER_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "lvgl/lvgl.h"

#if USE_LV_LMETER != 0

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
 * Create line meters to test their functionalities
 */
void lv_test_lmeter_1(void);

/**********************
 *      MACROS
 **********************/

#endif /*USE_LV_LMETER*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_TEST_LMETER_H*/
