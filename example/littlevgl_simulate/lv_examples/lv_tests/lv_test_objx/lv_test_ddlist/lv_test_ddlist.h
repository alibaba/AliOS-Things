/**
 * @file lv_test_ddlist.h
 *
 */

#ifndef LV_TEST_DDLIST_H
#define LV_TEST_DDLIST_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "lvgl/lvgl.h"

#if USE_LV_DDLIST != 0

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
 * Create drop down lists to test their functionalities
 */
void lv_test_ddlist_1(void);

/**********************
 *      MACROS
 **********************/

#endif /*USE_LV_DDLIST*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_TEST_DDLIST_H*/
