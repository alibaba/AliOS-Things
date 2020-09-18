/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#ifndef __SYS_TIME_H__
#define __SYS_TIME_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "plat_types.h"
#include "hal_cmu.h"
#ifndef osCMSIS_FreeRTOS
#include <sys/time.h>
#else
#include "time.h"
#endif

#ifdef __ARM_ARCH_ISA_ARM
int gettimeofday(struct timeval *__restrict __p, void *__restrict __tz);
#endif

int gettimeready(void);

void hal_sys_time_init(s32_t ntp_sec, u32_t frac);

s32_t hal_sys_time_get(void);

char *hal_sys_get_cur_time(void);

time_t hal_sys_get_cur_rawtime(void);

char *hal_sys_format_time(s32_t sec);


#ifdef __cplusplus
}
#endif

#endif

