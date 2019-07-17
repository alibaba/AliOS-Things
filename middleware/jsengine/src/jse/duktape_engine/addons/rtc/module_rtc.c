/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#define LOG_NDEBUG 0
#include <stdint.h>
#include "be_log.h"
#include "board-mgr/board_mgr.h"
#include "bone_engine_inl.h"
#include "rtc.h"

#define RTC_YEAR "year"
#define RTC_MONTH "month"
#define RTC_DAY "day"
#define RTC_HOUR "hour"
#define RTC_MINUTE "minute"
#define RTC_SECOND "second"

#define RTC_TIME_FORMAT                                                 \
    "{\"year\":\"%d\",\"month\":\"%d\",\"day\":\"%d\",\"hour\":\"%d\"," \
    "\"minute\":\"%d\",\"second\":\"%d\"}"

static rtc_dev_t rtc_dev;

static duk_ret_t native_rtc_open(duk_context *ctx) {
    int ret = hal_rtc_init(&rtc_dev);
    debug("port: %d, format: %d\n", rtc_dev.port, rtc_dev.config.format);
    if (0 != ret) {
        error("hal_rtc_init fail!\n");
    }
    duk_push_int(ctx, ret);
    return 1;
}

static duk_ret_t native_rtc_close(duk_context *ctx) {
    int ret = hal_rtc_finalize(&rtc_dev);
    if (0 != ret) {
        error("hal_rtc_finalize fail!\n");
    }
    duk_push_int(ctx, ret);
    return 1;
}

static duk_ret_t native_rtc_get_time(duk_context *ctx) {
    int8_t ret = -1;
    rtc_time_t rtcTime;

    ret = hal_rtc_get_time(&rtc_dev, &rtcTime);
    if (0 != ret) {
        error("hal_rtc_get_time fail!\n");
        duk_push_null(ctx);
    } else {
        char buf[128] = {0x00};
        snprintf(buf, sizeof(buf), RTC_TIME_FORMAT, rtcTime.year, rtcTime.month,
                 rtcTime.date, rtcTime.hr, rtcTime.min, rtcTime.sec);
        duk_push_string(ctx, buf);
        debug(
            "year: %d, month: %d, day: %d, hour: %d, minute: %d, second: %d\n",
            rtcTime.year, rtcTime.month, rtcTime.date, rtcTime.hr, rtcTime.min,
            rtcTime.sec);
        duk_json_decode(ctx, -1);
    }
    return 1;
}

static duk_ret_t native_rtc_set_time(duk_context *ctx) {
    int8_t ret = -1;
    rtc_time_t rtcTime;

    if (!duk_is_object(ctx, 0)) {
        warn("parameter must be object\n");
        goto out;
    }

    ret = hal_rtc_get_time(&rtc_dev, &rtcTime);
    if (ret < 0) {
        error("hal_rtc_get_time fail!\n");
        goto out;
    }

    duk_get_prop_string(ctx, 0, RTC_YEAR);
    if (duk_is_number(ctx, -1)) rtcTime.year = duk_get_int(ctx, -1);
    duk_pop(ctx);

    duk_get_prop_string(ctx, 0, RTC_MONTH);
    if (duk_is_number(ctx, -1)) rtcTime.month = duk_get_int(ctx, -1);
    duk_pop(ctx);

    duk_get_prop_string(ctx, 0, RTC_DAY);
    if (duk_is_number(ctx, -1)) rtcTime.date = duk_get_int(ctx, -1);
    duk_pop(ctx);

    duk_get_prop_string(ctx, 0, RTC_HOUR);
    if (duk_is_number(ctx, -1)) rtcTime.hr = duk_get_int(ctx, -1);
    duk_pop(ctx);

    duk_get_prop_string(ctx, 0, RTC_MINUTE);
    if (duk_is_number(ctx, -1)) rtcTime.min = duk_get_int(ctx, -1);
    duk_pop(ctx);

    duk_get_prop_string(ctx, 0, RTC_SECOND);
    if (duk_is_number(ctx, -1)) rtcTime.sec = duk_get_int(ctx, -1);
    duk_pop(ctx);

    debug("year: %d, month: %d, day: %d, hour: %d, minute: %d, second: %d\n",
          rtcTime.year, rtcTime.month, rtcTime.date, rtcTime.hr, rtcTime.min,
          rtcTime.sec);
    ret = hal_rtc_set_time(&rtc_dev, &rtcTime);
out:
    duk_push_int(ctx, ret);
    return 1;
}

void module_rtc_register(void) {
    duk_context *ctx = bone_engine_get_context();

    duk_push_object(ctx);

    duk_push_c_function(ctx, native_rtc_open, 0);
    duk_put_prop_string(ctx, -2, "open");

    duk_push_c_function(ctx, native_rtc_close, 0);
    duk_put_prop_string(ctx, -2, "close");

    duk_push_c_function(ctx, native_rtc_get_time, 0);
    duk_put_prop_string(ctx, -2, "getTime");

    duk_push_c_function(ctx, native_rtc_set_time, 1);
    duk_put_prop_string(ctx, -2, "setTime");

    duk_put_global_string(ctx, "RTC");
}
