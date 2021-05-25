/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "aos/kernel.h"
#include "aos/hal/rtc.h"
#include "rtc_test.h"

#define RTC_DELAY_MS 2000
#define RTC_TOLERANCE_RANGE 0.01f

static rtc_time_t rtc_time_r;

static rtc_time_t rtc_time = {.sec = 45, .min = 30, .hr = 15, .weekday = 5, .date = 1, .month = 3, .year = 19};

static rtc_dev_t  rtc_dev;

void hal_rtc_test(void)
{
    int ret = -1;

    printf("*********** rtc test start ! ***********\n");

    memset(&rtc_time_r, 0, sizeof(rtc_time_t));

    rtc_dev.port = PORT_RTC_TEST;
    rtc_dev.config.format = HAL_RTC_FORMAT_DEC;

    printf("step1: set rtc date to\n");
    printf("        year: %d month: %d date: %d weekday: %d hr: %d min: %d sec: %d\n",
           rtc_time.year, rtc_time.month, rtc_time.date, rtc_time.weekday,
           rtc_time.hr, rtc_time.min, rtc_time.sec);

    ret = hal_rtc_init(&rtc_dev);
    if (ret != 0) {
        printf("hal_rtc_init error !\n");
        return;
    }

    ret = hal_rtc_set_time(&rtc_dev, &rtc_time);
    if (ret != 0) {
        printf("hal_rtc_set_time error !\n");
        return;
    }

    printf("step2: sleep %d ms !\n", RTC_DELAY_MS);
    aos_msleep(RTC_DELAY_MS * (1 + RTC_TOLERANCE_RANGE));

    ret = hal_rtc_get_time(&rtc_dev, &rtc_time_r);
    if (ret != 0) {
        printf("hal_rtc_get_time error !\n");
        return;
    }

    printf("step3: read rtc, current date is\n");
    printf("        year: %d month: %d date: %d weekday: %d hr: %d min: %d sec: %d\n",
           rtc_time_r.year, rtc_time_r.month, rtc_time_r.date, rtc_time_r.weekday,
           rtc_time_r.hr, rtc_time_r.min, rtc_time_r.sec);

    if ((rtc_time_r.sec != rtc_time.sec + RTC_DELAY_MS / 1000)
        ||(rtc_time_r.min != rtc_time.min)
        ||(rtc_time_r.hr != rtc_time.hr)
        ||(rtc_time_r.weekday != rtc_time.weekday)
        ||(rtc_time_r.date != rtc_time.date)
        ||(rtc_time_r.month != rtc_time.month)
        ||(rtc_time_r.year != rtc_time.year)) {
        printf("rtc value error !\n");
        return;
    }

    ret = hal_rtc_finalize(&rtc_dev);
    if (ret != 0) {
        printf("hal_rtc_finalize error !\n");
        return;
    }

    printf("rtc test result: succeed !\n");

    printf("*********** rtc test end ! ***********\n");
}
