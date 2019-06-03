/*
 * Copyright (C) 2018 Alibaba Group Holding Limited
 */

#ifndef CPU_PWR_LIB_H
#define CPU_PWR_LIB_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <k_api.h>

#include "pwrmgmt_default_config.h"
#include "cpu_pwr_state.h"

#ifndef container_of
#define container_of(ptr, type, member) \
        ((type *)((char *)(ptr) - offsetof(type, member)))
#endif /* container_of */

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
typedef enum cpu_idle_mode {
    CPU_IDLE_MODE_RUN      = 0,
    CPU_IDLE_MODE_SLEEP    = 1,
    CPU_IDLE_MODE_TICKLESS = 2
} CPU_IDLE_MODE;

int  cpu_pwrmgmt_init(void);
void cpu_pwr_down(void);
void cpu_pwr_up(void);

pwr_status_t  cpu_pwr_idle_mode_set(CPU_IDLE_MODE mode);
CPU_IDLE_MODE cpu_pwr_idle_mode_get(void);

pwr_status_t cpu_pwr_suspend(void);
pwr_status_t cpu_pwr_resume(void);
int          cpu_pwr_is_suspend(void);

extern void (*_func_cpu_tickless_up)(void);
extern void (*_func_cpu_tickless_down)(void);

#ifdef __cplusplus
}
#endif

#endif /* CPU_PWR_LIB_H */
