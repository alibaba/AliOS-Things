/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#define CONFIG_LOGMACRO_DETAILS
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "be_jse_module.h"
#include "be_port_hal.h"

#define RTC_YEAR "year"
#define RTC_MONTH "month"
#define RTC_DAY "day"
#define RTC_HOUR "hour"
#define RTC_MINUTE "minute"
#define RTC_SECOND "second"

#define RTC_TIME_FORMAT                                                 \
    "{\"year\":\"%d\",\"month\":\"%d\",\"day\":\"%d\",\"hour\":\"%d\"," \
    "\"minute\":\"%d\",\"second\":\"%d\"}"

static rtc_dev_t g_rtcdev;
static be_jse_symbol_t *rtc_open(void) {
    int8_t ret = -1;
    be_jse_handle_function(0, NULL, NULL, NULL, NULL);
    ret = hal_rtc_init(&g_rtcdev);
    if (0 != ret) {
        be_error("rtc", "hal_rtc_init fail!\n");
    }
    return new_int_symbol(ret);
}

static be_jse_symbol_t *rtc_close(void) {
    int8_t ret = -1;
    be_jse_handle_function(0, NULL, NULL, NULL, NULL);
    ret = hal_rtc_finalize(&g_rtcdev);
    if (0 != ret) {
        be_error("rtc", "hal_rtc_finalize fail!\n");
    }
    return new_int_symbol(ret);
}

static be_jse_symbol_t *rtc_get_time(void) {
    int8_t ret  = -1;
    uint8_t len = 0;
    rtc_time_t rtcTime;
    char buff[128] = {0x00};

    be_jse_handle_function(0, NULL, NULL, NULL, NULL);
    ret = hal_rtc_get_time(&g_rtcdev, &rtcTime);
    if (0 != ret) {
        be_error("rtc", "hal_rtc_get_time fail!\n");
    }
    len = snprintf(buff, 128, RTC_TIME_FORMAT, rtcTime.year, rtcTime.month,
                   rtcTime.date, rtcTime.hr, rtcTime.min, rtcTime.sec);
    buff[len] = 0x00;

    return new_json_symbol(buff, len);
}

static be_jse_symbol_t *rtc_set_time(void) {
    int8_t ret = -1;
    rtc_time_t rtcTime;
    be_jse_symbol_t *arg0              = NULL;
    be_jse_symbol_t *temp_symbol       = NULL;
    be_jse_symbol_t *temp_value_symbol = NULL;

    be_jse_handle_function(0, &arg0, NULL, NULL, NULL);
    if (!arg0 || !symbol_is_object(arg0)) goto out;
    ret = hal_rtc_get_time(&g_rtcdev, &rtcTime);
    if (ret < 0) {
        be_error("rtc", "hal_rtc_get_time fail!\n");
        goto out;
    }
    temp_symbol =
        lookup_named_child_symbol(get_symbol_node_id(arg0), RTC_YEAR, false);
    if (temp_symbol && (temp_value_symbol = get_symbol_value(temp_symbol))) {
        if (symbol_is_int(temp_value_symbol))
            rtcTime.year = get_symbol_int(temp_value_symbol);
    }
    symbol_unlock(temp_value_symbol);
    symbol_unlock(temp_symbol);
    temp_symbol =
        lookup_named_child_symbol(get_symbol_node_id(arg0), RTC_MONTH, false);
    if (temp_symbol && (temp_value_symbol = get_symbol_value(temp_symbol))) {
        if (symbol_is_int(temp_value_symbol))
            rtcTime.month = get_symbol_int(temp_value_symbol);
    }
    symbol_unlock(temp_value_symbol);
    symbol_unlock(temp_symbol);
    temp_symbol =
        lookup_named_child_symbol(get_symbol_node_id(arg0), RTC_DAY, false);
    if (temp_symbol && (temp_value_symbol = get_symbol_value(temp_symbol))) {
        if (symbol_is_int(temp_value_symbol))
            rtcTime.date = get_symbol_int(temp_value_symbol);
    }
    symbol_unlock(temp_value_symbol);
    symbol_unlock(temp_symbol);
    temp_symbol =
        lookup_named_child_symbol(get_symbol_node_id(arg0), RTC_HOUR, false);
    if (temp_symbol && (temp_value_symbol = get_symbol_value(temp_symbol))) {
        if (symbol_is_int(temp_value_symbol))
            rtcTime.hr = get_symbol_int(temp_value_symbol);
    }
    symbol_unlock(temp_value_symbol);
    symbol_unlock(temp_symbol);
    temp_symbol =
        lookup_named_child_symbol(get_symbol_node_id(arg0), RTC_MINUTE, false);
    if (temp_symbol && (temp_value_symbol = get_symbol_value(temp_symbol))) {
        if (symbol_is_int(temp_value_symbol))
            rtcTime.min = get_symbol_int(temp_value_symbol);
    }
    symbol_unlock(temp_value_symbol);
    symbol_unlock(temp_symbol);
    temp_symbol =
        lookup_named_child_symbol(get_symbol_node_id(arg0), RTC_SECOND, false);
    if (temp_symbol && (temp_value_symbol = get_symbol_value(temp_symbol))) {
        if (symbol_is_int(temp_value_symbol))
            rtcTime.sec = get_symbol_int(temp_value_symbol);
    }
    symbol_unlock(temp_value_symbol);
    symbol_unlock(temp_symbol);
    ret = hal_rtc_set_time(&g_rtcdev, &rtcTime);
out:
    symbol_unlock(arg0);
    return new_int_symbol(ret);
}

static be_jse_symbol_t *rtc_module_handle_cb(be_jse_vm_ctx_t *execInfo,
                                             be_jse_symbol_t *var,
                                             const char *name) {
    if (0 == strcmp(name, "open")) {
        return rtc_open();
    }
    if (0 == strcmp(name, "getTime")) {
        return rtc_get_time();
    }
    if (0 == strcmp(name, "setTime")) {
        return rtc_set_time();
    }
    if (0 == strcmp(name, "close")) {
        return rtc_close();
    }
    return (BE_JSE_FUNC_UNHANDLED);
}

void module_rtc_register(void) {
    be_jse_module_load("RTC", rtc_module_handle_cb);
}
