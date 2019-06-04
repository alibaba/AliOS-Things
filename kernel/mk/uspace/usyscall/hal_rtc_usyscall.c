/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "k_config.h"

#if (RHINO_CONFIG_HAL_RTC_SYSCALL > 0)

#include <stdint.h>
#include "syscall.h"
#include "hal_rtc_syscall_arg.h"
#include "syscall_no.h"

int32_t hal_rtc_init(rtc_dev_t *rtc)
{
    hal_rtc_init_syscall_arg_t _arg;

    _arg.rtc = rtc;

    return (int32_t)SYSCALL(SYS_HAL_RTC_INIT, (void*)&_arg);
}

int32_t hal_rtc_get_time(rtc_dev_t *rtc, rtc_time_t *time)
{
    hal_rtc_get_time_syscall_arg_t _arg;

    _arg.rtc  = rtc;
    _arg.time = time;

    return (int32_t)SYSCALL(SYS_HAL_RTC_GET_TIME, (void*)&_arg);
}

int32_t hal_rtc_set_time(rtc_dev_t *rtc, const rtc_time_t *time)
{
    hal_rtc_set_time_syscall_arg_t _arg;

    _arg.rtc  = rtc;
    _arg.time = time;

    return (int32_t)SYSCALL(SYS_HAL_RTC_SET_TIME, (void*)&_arg);
}

int32_t hal_rtc_finalize(rtc_dev_t *rtc)
{
    hal_rtc_finalize_syscall_arg_t _arg;

    _arg.rtc = rtc;

    return (int32_t)SYSCALL(SYS_HAL_RTC_FINALIZE, (void*)&_arg);
}

#endif /* RHINO_CONFIG_HAL_RTC_SYSCALL */

