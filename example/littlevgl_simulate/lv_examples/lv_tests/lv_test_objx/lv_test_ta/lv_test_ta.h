/**
 * @file lv_test_ta.h
 *
 */

#ifndef LV_TEST_TA_H
#define LV_TEST_TA_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../../../lv_ex_conf.h"   
#include "lvgl/lvgl.h"

#if USE_LV_TA && USE_LV_TESTS

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
 * Create text areas to test their basic functionalities
 */
void lv_test_ta_1(void);

/**
 * Test cursor modes
 */
void lv_test_ta_2(void);

/**********************
 *      MACROS
**********************/

#endif /*USE_LV_TA && USE_LV_TESTS*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_TEST_TA_H*/
