/**
 * @file demo.h
 *
 */

#ifndef DEMO_H
#define DEMO_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../../lv_ex_conf.h"
#if USE_LV_DEMO
    
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
 * Create a demo application
 */
void demo_create(void);

/**********************
 *      MACROS
 **********************/

#endif /*USE_LV_DEMO*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*DEMO_H*/
