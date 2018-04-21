/**
 * @file lv_test_roller.h
 *
 */

#ifndef LV_TEST_ROLLER_H
#define LV_TEST_ROLLER_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "lvgl/lvgl.h"

#if USE_LV_ROLLER != 0

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
 * Create rollers to test their functionalities
 */
void lv_test_roller_1(void);

/**********************
 *      MACROS
 **********************/

#endif /*USE_LV_ROLLER*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_TEST_ROLLER_H*/
