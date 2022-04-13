/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include "amp_defines.h"
#include "amp_hal_rtc.h"
#include "ql_rtc.h"

#define MOD_STR "RTC"

int32_t amp_hal_rtc_init(rtc_dev_t *rtc)
{
    return 0;
}

int32_t amp_hal_rtc_get_time(rtc_dev_t *rtc, rtc_time_t *time)
{
    int32_t ret = -1;
    ql_rtc_time_t tm = {0};

    ret = ql_rtc_get_time(&tm);
    if (ret != 0)
    {
        amp_error(MOD_STR, "rtc time get failed! \n");
        return ret;
    }

    time->sec = tm.tm_sec;
    time->min = tm.tm_min;
    time->hr = tm.tm_hour;
    time->date = tm.tm_mday;
    time->month = tm.tm_mon;
    time->year = tm.tm_year - 1970;
    time->weekday = tm.tm_wday;

    return 0;
}

int32_t amp_hal_rtc_set_time(rtc_dev_t *rtc, const rtc_time_t *time)
{
    int32_t ret = -1;
    ql_rtc_time_t tm = {0};

    tm.tm_sec = time->sec;
    tm.tm_min = time->min;
    tm.tm_hour = time->hr;
    tm.tm_mday = time->date;
    tm.tm_mon = time->month;
    tm.tm_year = time->year + 1970;
    tm.tm_wday = time->weekday;

    ret = ql_rtc_set_time(&tm);
    if (ret != 0)
    {
        amp_error(MOD_STR, "rtc time set failed! \n");
        return ret;
    }

    return ret;
}

int32_t amp_hal_rtc_finalize(rtc_dev_t *rtc)
{
    return 0;
}
