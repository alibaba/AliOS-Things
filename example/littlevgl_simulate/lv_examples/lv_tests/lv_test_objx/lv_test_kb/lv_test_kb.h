/**
 * @file lv_test_kb.h
 *
 */

#ifndef LV_TEST_KB_H
#define LV_TEST_KB_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "lvgl/lvgl.h"

#if USE_LV_KB != 0

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
 * Create a default object and test the basic functions
 */
void lv_test_kb_1(void);

/**
 * Create a styles keyboard
 */
void lv_test_kb_2(void);

/**********************
 *      MACROS
 **********************/

#endif /*USE_LV_KB*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_TEST_KB_H*/
