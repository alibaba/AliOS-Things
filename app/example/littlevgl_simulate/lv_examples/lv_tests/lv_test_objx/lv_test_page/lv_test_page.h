/**
 * @file lv_test_page.h
 *
 */

#ifndef LV_TEST_PAGE_H
#define LV_TEST_PAGE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../../../lv_ex_conf.h"   
#include "lvgl/lvgl.h"

#if USE_LV_PAGE && USE_LV_TESTS

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
 * Create pages to test their basic functionalities
 */
void lv_test_page_1(void);

/**
 * Test styling, scrollbar modes, layout and action
 */
void lv_test_page_2(void);

/**********************
 *      MACROS
 **********************/

#endif /*USE_LV_PAGE && USE_LV_TESTS*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_TEST_PAGE_H*/
