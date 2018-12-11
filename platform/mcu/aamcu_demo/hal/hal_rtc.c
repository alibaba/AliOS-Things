/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include <stdio.h>
#include <stdlib.h>
#include "hal/hal.h"

#ifdef HAL_RTC_MODULE_ENABLED

int32_t hal_rtc_init(rtc_dev_t *rtc)
{
}

int32_t hal_rtc_get_time(rtc_dev_t *rtc, rtc_time_t *time)
{
}

int32_t hal_rtc_set_time(rtc_dev_t *rtc, const rtc_time_t *time)
{
}

int32_t hal_rtc_finalize(rtc_dev_t *rtc)
{
#endif
