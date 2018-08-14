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
#include "../../../../lv_ex_conf.h"   
#include "lvgl/lvgl.h"

#if USE_LV_LIST && USE_LV_TESTS

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

#endif /*USE_LV_LIST && USE_LV_TESTS*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_TEST_LIST_H*/
