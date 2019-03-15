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

#include "pwrmgmt_state.h"

#ifndef container_of
#define container_of(ptr, type, member) \
        ((type *)((char *)(ptr) - offsetof(type, member)))
#endif /* container_of */

int  cpu_pwrmgmt_init(void);
void cpu_pwr_down(void);
void cpu_pwr_up(void);

extern void (*_func_cpu_tickless_up)(void);
extern void (*_func_cpu_tickless_down)(void);

#ifdef __cplusplus
}
#endif

#endif /* CPU_PWR_LIB_H */
