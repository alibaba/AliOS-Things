/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */
#ifndef _AMEBA_SOC_H_
#define _AMEBA_SOC_H_

/* rom headers */
#include "rtl8710b.h"

#ifndef CONFIG_BUILD_ROM
/* ram headers */
#include "platform_opts.h"
#include "rtl_consol_ram.h"
#include "rtl8710b_ota.h"

#include "freertos_pmu.h"
#include "rtl8710b_freertos_pmu.h"

#if PLATFORM_FREERTOS

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#endif
#endif

#endif //_AMEBA_SOC_H_
