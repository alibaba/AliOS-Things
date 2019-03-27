/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */


/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __LORAWAN_PORT_H__
#define __LORAWAN_PORT_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "hal/lorawan.h"
#include "utilities.h"

/* NOTICE:
   "TIMER_CMD_START" conflict in "k_timer.h" and "efm32g_timer.h",
   so "K_TIMER_H" is defined for exclude "k_timer.h" in "k_api.h" */
#define K_TIMER_H
/* rhino header files */
#include "k_api.h"
#undef  K_TIMER_H

extern hal_lrwan_dev_chg_mode_t aos_lrwan_chg_mode;
extern hal_lrwan_sys_t          aos_lrwan_sys;
extern hal_manufactory_itf_t    aos_mft_itf;

#ifdef __cplusplus
}
#endif

#endif /* lorawan_port.h */
