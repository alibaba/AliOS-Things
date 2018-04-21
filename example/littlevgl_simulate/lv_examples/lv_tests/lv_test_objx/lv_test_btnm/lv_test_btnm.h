/**
 * @file lv_test_btnm.h
 *
 */

#ifndef LV_TEST_BTNM_H
#define LV_TEST_BTNM_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "lvgl/lvgl.h"

#if USE_LV_BTNM != 0

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
 * Create button matrixes to test their functionalities
 */
void lv_test_btnm_1(void);

/**********************
 *      MACROS
 **********************/

#endif /*USE_LV_BTNM*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_TEST_BTNM_H*/
