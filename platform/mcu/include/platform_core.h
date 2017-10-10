/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef __PLATFORM_CORE_h__
#define __PLATFORM_CORE_h__

#include "common.h"

/**
 * Returns the current CPU cycle count.
 *
 * This function is used to accurately calculate sub-systick timing.
 */
extern uint32_t platform_get_cycle_count( void );

/**
 * Returns TRUE if the CPU is currently running in interrupt context
 *
 */
extern mico_bool_t platform_is_in_interrupt_context( void );

#endif // __PLATFORM_CORE_h__

