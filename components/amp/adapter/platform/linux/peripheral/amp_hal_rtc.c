/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include "amp_hal_rtc.h"

int32_t amp_hal_rtc_init(rtc_dev_t *rtc)
{
    return 0;
}

int32_t amp_hal_rtc_get_time(rtc_dev_t *rtc, rtc_time_t *time)
{
    time->year = 120;
    time->month = 5;
    time->date = 21;
    time->hr = 17;
    time->min = 45;
    time->sec = 55;
    return 0;
}

int32_t amp_hal_rtc_set_time(rtc_dev_t *rtc, const rtc_time_t *time)
{
    return 0;
}

int32_t amp_hal_rtc_finalize(rtc_dev_t *rtc)
{
    return 0;
}