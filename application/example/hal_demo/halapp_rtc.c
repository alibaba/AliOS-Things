/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#include <stdio.h>
#include <stdlib.h>

#include <aos/kernel.h>
#include "aos/hal/rtc.h"
#include "board.h"

#if (AOS_HAL_RTC_ENABLED > 0)

static rtc_dev_t rtc0;

void hal_rtc_app_init(void)
{
    int ret = -1;

    rtc_config_t rtc_cfg;
    rtc_time_t   time_buf;

    /* rtc port set */
    rtc0.port = PORT_RTC_0;

    /* set to DEC format */
    rtc0.config.format = HAL_RTC_FORMAT_DEC;

    /* init rtc0 with the given settings */
    ret = hal_rtc_init(&rtc0);
    if (ret != 0) {
        printf("rtc init error !\n");
        return;
    }

    time_buf.sec     = 0;
    time_buf.min     = 31;
    time_buf.hr      = 21;
    time_buf.weekday = 3;
    time_buf.date    = 26;
    time_buf.month   = 2;
    time_buf.year    = 2020;

    /* set rtc0 time to 2020/2/26,21:31:00 */
    ret = hal_rtc_set_time(&rtc0, &time_buf);
    if (ret != 0) {
        printf("rtc0 set time error !\n");
        return;
    }
    printf("set time is:2020-2-26 21:31:00\r\n");
}

void hal_rtc_app_gettime(void)
{
    int        ret = -1;
    rtc_time_t time_buf;

    /* get current time */
    ret = hal_rtc_get_time(&rtc0, &time_buf);
    if (ret != 0) {
        printf("get time error !\n");
        return;
    }

    printf("current time is: %4d-%02d-%02d %02d:%02d:%02d\r\n", time_buf.year, time_buf.month,
           time_buf.date, time_buf.hr, time_buf.min, time_buf.sec);
}

#endif

