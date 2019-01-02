/**
 * @file tpcal.h
 *
 */

#ifndef TPCAL_H
#define TPCAL_H

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
 * Create a touch pad calibration screen
 */
void tpcal_create(void);

/**********************
 *      MACROS
 **********************/

#endif /*USE_LV_TPCAL*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*TP_CAL_H*/
