/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include "amp_hal_rtc.h"
#include "time.h"
#include "drv_rtc.h"
#include "osi_log.h"

#define CFW_TIME_ZONE_SECOND (15 * 60)

int32_t amp_hal_rtc_init(rtc_dev_t *rtc)
{
    return 0;
}

int32_t amp_hal_rtc_get_time(rtc_dev_t *rtc, rtc_time_t *time)
{
    int ret = -1;
    if (time == NULL)
    {
        OSI_PRINTFI("param time is NULL!\n");
        return ret;
    }
    int timeoffset = 32 * CFW_TIME_ZONE_SECOND;
    time_t lt = osiEpochSecond() + timeoffset;
    struct tm tm;
    gmtime_r(&lt, &tm);
    time->year = (tm.tm_year + 1900) % 100;
    time->month = tm.tm_mon + 1;
    time->date = tm.tm_mday;
    time->hr = tm.tm_hour;
    time->min = tm.tm_min;
    time->sec = tm.tm_sec;
    time->weekday = (time->date+1+2*time->month+3*(time->month+1)/5+time->year+time->year/4-time->year/100+time->year/400) % 7;
    ret = 0;
    return ret;
}

int32_t amp_hal_rtc_set_time(rtc_dev_t *rtc, const rtc_time_t *time)
{
    int ret = -1;
    if (time == NULL)
    {
        OSI_PRINTFI("param time is NULL!\n");    
        return ret;
    }
    int setalarmtimezone = 32;
    struct tm tm;
    memset(&tm, 0x00, sizeof(tm));
    tm.tm_year = time->year;
    tm.tm_mon = time->month - 1;
    tm.tm_mday = time->date;
    tm.tm_hour = time->hr;
    tm.tm_min = time->min;
    tm.tm_sec = time->sec;
    tm.tm_isdst = 0;
    if (tm.tm_year > 100)
    {
        if (tm.tm_year < 1970 || tm.tm_year >= 2070)
        {
            return ret;
        }
        tm.tm_year -= 1900;
    }
    else
    {
        // 3GPP doesn't define how to interprete 2 digitals of year
        // So, [0-70) -> [2000-2070), [70-99] -> [1970-1999]
        if (tm.tm_year < 70)
        {
            tm.tm_year += (2000 - 1900);
        }
    }
    struct tm ntm = tm;
    time_t t = mktime(&ntm);
    if (t == -1)
    {
        return ret;
    }
    if (tm.tm_year != ntm.tm_year ||
        tm.tm_mon != ntm.tm_mon ||
        tm.tm_mday != ntm.tm_mday ||
        tm.tm_hour != ntm.tm_hour ||
        tm.tm_min != ntm.tm_min ||
        tm.tm_sec != ntm.tm_sec)
    {
        return ret;
    }
    osiSetEpochTime((mktime(&tm) - setalarmtimezone * CFW_TIME_ZONE_SECOND) * 1000LL);
    drvRtcUpdateTime();
    atCfgAutoSave();
    ret  = 0;
    return ret;
}

int32_t amp_hal_rtc_finalize(rtc_dev_t *rtc)
{
    return 0;
}