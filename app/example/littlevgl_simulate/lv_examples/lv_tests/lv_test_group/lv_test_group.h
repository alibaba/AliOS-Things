/**
 * @file lv_test_group.h
 *
 */

#ifndef LV_TEST_GROUP_H
#define LV_TEST_GROUP_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../../lv_ex_conf.h"  
#include "lvgl/lvgl.h" 
#if USE_LV_GROUP && USE_LV_TESTS    
    
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
 * Create base groups to test their functionalities
 */
void lv_test_group_1(void);

/**********************
 *      MACROS
 **********************/

#endif /* USE_LV_GROUP && USE_LV_TESTS */

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_TEST_BAR_H*/
