/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#ifndef __SYSTEM_ARMCM_H__
#define __SYSTEM_ARMCM_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "stdbool.h"

extern uint32_t SystemCoreClock;     /*!< System Clock Frequency (Core Clock)  */

void SystemInit (void);

// #ifdef UNALIGNED_ACCESS

// __STATIC_FORCEINLINE bool get_unaligned_access_status(void) { return true; }

// __STATIC_FORCEINLINE bool config_unaligned_access(bool enable) { return true; }

// #else

bool get_unaligned_access_status(void);

bool config_unaligned_access(bool enable);

// #endif

uint32_t get_cpu_id(void);

#ifdef __cplusplus
}
#endif

#endif

