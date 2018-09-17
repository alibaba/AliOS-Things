/**
 * @file symon.h
 *
 */

#ifndef SYSMON_H
#define SYSMON_H

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
 * Initialize the system monitor
 */
void sysmon_create(void);

/**********************
 *      MACROS
 **********************/

#endif /*USE_LV_SYSMON*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* SYSMON_H */
