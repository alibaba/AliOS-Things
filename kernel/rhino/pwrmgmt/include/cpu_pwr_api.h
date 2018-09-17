/*
 * Copyright (C) 2018 Alibaba Group Holding Limited
 */

/*
modification history
--------------------
14jan2018, init vesion
*/

/*
DESCRIPTION
This file provides APIs for management CPU power.
*/

#ifndef K_CPU_PWR_API_H
#define K_CPU_PWR_API_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <k_api.h>
#include "cpu_pwr_default_config.h"
#include "cpu_pwr_lib.h"
#include "cpu_pwr_hal_lib.h"
#include "pwr_state.h"

    /*
     * CPU power management operates in one of three possible modes:
     *
     * CPU_IDLE_MODE_RUN:
     * In this CPU mode of operation, idle CPU power management is disabled.
     *
     * CPU_IDLE_MODE_SLEEP:
     * the CPUs sleep when idle, but system clock interupts wakes up the CPU
     * at every tick.
     *
     * CPU_IDLE_MODE_TICKLESS: CPUs sleep when idle, potentially for multiple
     * ticks once enter idle mode.
     */
    typedef enum cpu_idle_mode
    {
        CPU_IDLE_MODE_RUN      = 0,
        CPU_IDLE_MODE_SLEEP    = 1,
        CPU_IDLE_MODE_TICKLESS = 2
    } CPU_IDLE_MODE;


    extern pwr_status_t cpu_pwr_c_state_set(cpu_cstate_t target_c_state);
    extern pwr_status_t cpu_pwr_c_state_get(uint32_t      cpu_idx,
                                            cpu_cstate_t *p_cstate);
    extern pwr_status_t cpu_pwr_c_state_capability_get(
      uint32_t cpu_idx, uint32_t *p_support_bitset_c);

    extern pwr_status_t  cpu_pwr_idle_mode_set(CPU_IDLE_MODE mode);
    extern CPU_IDLE_MODE cpu_pwr_idle_mode_get(void);

#if (CPU_PWR_SHOW > 0)
    extern void cpu_pwr_state_show(void);
    extern void cpu_pwr_info_show(void);
#endif

#ifdef __cplusplus
}
#endif

#endif /* K_CPU_PWR_API_H */
