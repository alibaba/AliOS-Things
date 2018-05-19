/**
 * @file lv_test_led.h
 *
 */

#ifndef LV_TEST_LED_H
#define LV_TEST_LED_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "lvgl/lvgl.h"

#if USE_LV_LED != 0

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
 * Create LEDs to test their functionalities
 */
void lv_test_led_1(void);
/**********************
 *      MACROS
 **********************/

#endif /*USE_LV_LED*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_TEST_LED_H*/
