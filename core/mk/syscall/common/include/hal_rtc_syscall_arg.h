/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef HAL_RTC_SYSCALL_ARG_H
#define HAL_RTC_SYSCALL_ARG_H

#include "aos/hal/rtc.h"

typedef struct {
    rtc_dev_t *rtc;
} hal_rtc_init_syscall_arg_t;

typedef struct {
    rtc_dev_t  *rtc;
    rtc_time_t *time;
} hal_rtc_get_time_syscall_arg_t;

typedef struct {
    rtc_dev_t        *rtc;
    const rtc_time_t *time;
}hal_rtc_set_time_syscall_arg_t;

typedef struct {
    rtc_dev_t *rtc;
}hal_rtc_finalize_syscall_arg_t;

#endif /* HAL_RTC_SYSCALL_ARG_H */

