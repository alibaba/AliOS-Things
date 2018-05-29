/**
 * @file lv_test_list.h
 *
 */

#ifndef LV_TEST_LIST_H
#define LV_TEST_LIST_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "lvgl/lvgl.h"

#if USE_LV_LIST != 0

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
 * Create lists to test their functionalities
 */
void lv_test_list_1(void);

/**********************
 *      MACROS
 **********************/

#endif /*USE_LV_LIST*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_TEST_LIST_H*/
