/**
 * @file lv_test_img.h
 *
 */

#ifndef LV_TEST_IMG_H
#define LV_TEST_IMG_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "lvgl/lvgl.h"

#if USE_LV_IMG != 0

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
 * Create images to test their functionalities
 */
void lv_test_img_1(void);

/**********************
 *      MACROS
 **********************/

#endif /*USE_LV_IMG*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_TEST_IMG_H*/
