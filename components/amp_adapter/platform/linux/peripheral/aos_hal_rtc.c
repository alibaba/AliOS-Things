/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include "aos_hal_rtc.h"

rtc_time_t time_save;

int32_t aos_hal_rtc_init(rtc_dev_t *rtc)
{
    time_save.year = 97;
    time_save.month = 10;
    time_save.date = 3;
    time_save.hr = 10;
    time_save.min = 20;
    time_save.sec = 45;

    return 0;
}

int32_t aos_hal_rtc_get_time(rtc_dev_t *rtc, rtc_time_t *time)
{
    time->year = time_save.year;
    time->month = time_save.month;
    time->date = time_save.date;
    time->hr = time_save.hr;
    time->min = time_save.min;
    time->sec = time_save.sec;
    return 0;
}

int32_t aos_hal_rtc_set_time(rtc_dev_t *rtc, const rtc_time_t *time)
{
    time_save.year = time->year;
    time_save.month = time->month;
    time_save.date = time->date;
    time_save.hr = time->hr;
    time_save.min = time->min;
    time_save.sec = time->sec;

    return 0;
}

int32_t aos_hal_rtc_finalize(rtc_dev_t *rtc)
{
    time_save.year = 0;
    time_save.month = 0;
    time_save.date = 0;
    time_save.hr = 0;
    time_save.min = 0;
    time_save.sec = 0;
    
     return 0;
}
