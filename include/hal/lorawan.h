/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef HAL_LRWAN_H
#define HAL_LRWAN_H

#ifdef __cplusplus
extern "C" {
#endif

/*INCLUDES*******************************************************************
 *                          SYSTEM HEADER FILES          
 *END***********************************************************************/
#include <stdint.h>
#include "base.h"
#include "timeServer.h"

/*MACROS*********************************************************************
 *                          DATATYPE DEFINITIONS                    
 *END***********************************************************************/
/* the struct is for changing the device working mode */
typedef struct 
{
    void (*enter_stop_mode)(void);
    void (*exit_stop_mode)(void);
    void (*enter_sleep_mode)(void);  
} hal_lrwan_dev_chg_mode_t;

/* LoRaWan time and timer interface */
typedef struct 
{
    void (*delay_ms)(uint32_t delay);
    uint32_t (*set_timer_context)(void);
    uint32_t (*get_timer_context)(void);
    uint32_t (*get_timer_elapsed_time)(void);
    uint32_t (*ms2tick)(TimerTime_t timeMicroSec);
    TimerTime_t (*tick2ms)(uint32_t tick);
    uint32_t (*get_min_timeout)(void);
    uint32_t (*get_timer_val)(void);
    void (*stop_alarm)(void);
    void (*set_alarm)(uint32_t timeout);
    void (*set_uc_wakeup_time)(void);
} hal_lrwan_time_itf_t;

#ifdef __cplusplus
}
#endif

#endif /* HAL_LRWAN_H */

