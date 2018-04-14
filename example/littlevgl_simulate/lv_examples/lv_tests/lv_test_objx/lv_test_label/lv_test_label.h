/**
 * @file lv_test_label.h
 *
 */

#ifndef LV_TEST_LABEL_H
#define LV_TEST_LABEL_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "lvgl/lvgl.h"

#if USE_LV_LABEL != 0

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
 * Create labels with dynamic, static and array texts
 */
void lv_test_label_1(void);

/**
 * Test label long modes
 */
void lv_test_label_2(void);

/**
 * Test text insert and cut
 */
void lv_test_label_3(void);

/**********************
 *      MACROS
 **********************/

#endif /*USE_LV_LABEL*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_TEST_LABEL_H*/
