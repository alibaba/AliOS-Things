/**
 * @file lv_test_mbox.h
 *
 */

#ifndef LV_TEST_MBOX_H
#define LV_TEST_MBOX_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "lvgl/lvgl.h"

#if USE_LV_MBOX != 0

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
 * Create message boxes to test their functionalities
 */
void lv_test_mbox_1(void);

/**********************
 *      MACROS
 **********************/

#endif /*USE_LV_MBOX*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_TEST_MBOX_H*/
