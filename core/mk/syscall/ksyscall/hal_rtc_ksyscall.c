/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include "k_config.h"

#if (RHINO_CONFIG_HAL_RTC_SYSCALL > 0)

#include <stdint.h>
#include "aos/hal/rtc.h"
#include "hal_rtc_syscall_arg.h"

int32_t sys_hal_rtc_init_stub(void *arg)
{
    hal_rtc_init_syscall_arg_t *_arg = arg;

    return hal_rtc_init(_arg->rtc);
}
int32_t sys_hal_rtc_get_time_stub(void *arg)
{
    hal_rtc_get_time_syscall_arg_t *_arg = arg;

    return hal_rtc_get_time(_arg->rtc, _arg->time);
}
int32_t sys_hal_rtc_set_time_stub(void *arg)
{
    hal_rtc_set_time_syscall_arg_t *_arg = arg;

    return hal_rtc_set_time(_arg->rtc, _arg->time);
}
int32_t sys_hal_rtc_finalize_stub(void *arg)
{
    hal_rtc_finalize_syscall_arg_t *_arg = arg;

    return hal_rtc_finalize(_arg->rtc);
}

#endif /* RHINO_CONFIG_HAL_RTC_SYSCALL */

