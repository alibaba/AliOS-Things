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
#if USE_LV_TESTS    
    
#include "lvgl/lvgl.h"

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
 * Create base objects to test their functionalities
 */
void lv_test_object_1(void);

/**********************
 *      MACROS
 **********************/

#endif /* USE_LV_TESTS */

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_TEST_BAR_H*/
