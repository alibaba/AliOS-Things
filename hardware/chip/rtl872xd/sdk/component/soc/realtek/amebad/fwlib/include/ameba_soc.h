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
#include "rtl8721d.h"
#include "rtl8721d_simulation.h"
#include "strproc.h"
#include "rtl8721dlp_km4.h"
#include "rtl8721d_captouch.h"

#ifndef CONFIG_BUILD_ROM
/* ram headers */
#include "platform_opts.h"
#include "rtl8721d_ota.h"

#include "freertos_pmu.h"
#include "FreeRTOS.h"

#ifdef PLATFORM_FREERTOS
#include "rtl8721d_freertos_pmu.h"
#include "freertos_pmu.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#endif
#endif

#endif //_AMEBA_SOC_H_
