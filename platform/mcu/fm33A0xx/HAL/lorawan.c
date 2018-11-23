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
    void (*stop_alarm)(void);
    void (*set_alarm)(uint32_t timeout);
    void (*set_uc_wakeup_time)(void);
    void (*set_timeout)(TimerEvent_t *obj);
    TimerTime_t (*compute_elapsed_time)(TimerTime_t time);
    TimerTime_t (*get_current_time)(void );
    void (*set_timer_val)(TimerEvent_t *obj, uint32_t value);
    
} hal_lrwan_time_itf_t;

/* the struct is for control of radio */
typedef struct 
{
    void (*radio_reset)(void);
    void (*radio_reset_cfg_input)(void);
    void (*radio_rw_en)(void);  
    void (*radio_rw_dis)(void);  
    uint16_t (*radio_rw)(uint16_t tx_data);  
} hal_lrwan_radio_ctrl_t;

#ifdef __cplusplus
}
#endif

#endif /* HAL_LRWAN_H */

