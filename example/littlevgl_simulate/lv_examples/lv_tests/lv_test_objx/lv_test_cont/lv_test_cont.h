/**
 * @file lv_test_cont.h
 *
 */

#ifndef LV_TEST_CONT_H
#define LV_TEST_CONT_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "lvgl/lvgl.h"

#if USE_LV_CONT != 0

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
 * Create containers to test their  basic functionalities
 */
void lv_test_cont_1(void);

/**
 * Test nested style inheritance on padding update
 */
void lv_test_cont_2(void);

/**********************
 *      MACROS
 **********************/

#endif /*USE_LV_CONT*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_TEST_CONT_H*/
