/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef HAL_RTC_KSYSCALL_H
#define HAL_RTC_KSYSCALL_H

int32_t sys_hal_rtc_init_stub(void *arg);

int32_t sys_hal_rtc_get_time_stub(void *arg);

int32_t sys_hal_rtc_set_time_stub(void *arg);

int32_t sys_hal_rtc_finalize_stub(void *arg);

#endif /* HAL_RTC_KSYSCALL_H */

