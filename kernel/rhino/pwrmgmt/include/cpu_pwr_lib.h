/* cpu_pwr_lib.c - CPU power manager common library */

/*
 * Copyright (C) 2018 Alibaba Group Holding Limited
 */

/*
modification history
--------------------
14jan2018, init vesion
*/

#ifndef K_CPU_PWR_LIB_H
#define K_CPU_PWR_LIB_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <k_api.h>
#include "pwr_state.h"

#ifndef container_of
#define container_of(ptr, type, member) \
    ((type *)((char *)(ptr)-offsetof(type, member)))
#endif /* container_of */

    extern void cpu_pwrmgmt_init(void);
    extern void cpu_pwr_down(void);
    extern void cpu_pwr_up(void);

    extern void (*_func_cpu_tickless_up)(void);
    extern void (*_func_cpu_tickless_down)(void);

#ifdef __cplusplus
}
#endif

#endif /* K_CPU_PWR_LIB_H */
