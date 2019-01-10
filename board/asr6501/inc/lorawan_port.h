/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */


/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __LORAWAN_PORT_H__
#define __LORAWAN_PORT_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include <stdio.h>
#include <k_api.h>
#include "port.h"
#include "lorawan.h"
#include "utilities.h"


typedef uint32_t time_ms_t;
typedef uint32_t time_tick_t;


extern hal_lrwan_dev_chg_mode_t aos_lrwan_chg_mode;
extern hal_lrwan_time_itf_t aos_lrwan_time_itf;
extern hal_manufactory_itf_t aos_mft_itf;
extern hal_lrwan_radio_ctrl_t aos_lrwan_radio_ctrl;
extern hal_lrwan_sys_t          aos_lrwan_sys;


#ifdef __cplusplus
}
#endif

#endif /* lorawan_port.h */
