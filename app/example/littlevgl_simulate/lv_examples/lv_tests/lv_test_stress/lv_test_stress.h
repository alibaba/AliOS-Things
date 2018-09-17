/**
 * @file lv_test_object.h
 *
 */

#ifndef LV_TEST_OBJECT_H
#define LV_TEST_OBJECT_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../../lv_ex_conf.h"   
#include "lvgl/lvgl.h"

#if USE_LV_TESTS

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
 * Create and delete a lot of objects and animations.
 */
void lv_test_stress_1(void);

/**********************
 *      MACROS
 **********************/

#endif /*USE_LV_TESTS*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_TEST_BAR_H*/
