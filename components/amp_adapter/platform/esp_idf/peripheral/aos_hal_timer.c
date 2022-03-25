/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

#include "esp_idf_version.h"
#include "aos_hal_timer_internal.h"
#include "driver/timer.h"
#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(4, 1, 1)
#include "hal/timer_ll.h"
#define HAVE_TIMER_LL (1)
#endif

#define TIMER_INTR_SEL TIMER_INTR_LEVEL
#define TIMER_DIVIDER 8

// TIMER_BASE_CLK is normally 80MHz. TIMER_DIVIDER ought to divide this exactly
#define TIMER_SCALE (TIMER_BASE_CLK / TIMER_DIVIDER)
#define TIMER_FLAGS 0

void aos_hal_timer_isr(void *tim)
{
    aos_hal_timer_dev_t *tim_isr = tim;
    // calc group & index
    uint8_t group = ((tim_isr->port) >> 1) & 1;
    uint8_t index = (tim_isr->port) & 1;

    timg_dev_t *device = group ? &(TIMERG1) : &(TIMERG0);

    #if HAVE_TIMER_LL

    #if CONFIG_IDF_TARGET_ESP32 && ESP_IDF_VERSION < ESP_IDF_VERSION_VAL(5, 0, 0)
    device->hw_timer[index].update = 1;
    #else
    #if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(4, 4, 0)
    #if CONFIG_IDF_TARGET_ESP32S3
    device->hw_timer[index].update.tn_update = 1;
    #else
    device->hw_timer[index].update.tx_update = 1;
    #endif
    #else
    device->hw_timer[index].update.update = 1;
    #endif
    #endif
    timer_ll_clear_intr_status(device, index);
    #if ESP_IDF_VERSION < ESP_IDF_VERSION_VAL(5, 0, 0)
    timer_ll_set_alarm_enable(device, index, tim_isr->config.reload_mode);
    #else
    timer_ll_set_alarm_value(device, index, tim_isr->config.reload_mode);
    #endif

    #else

    device->hw_timer[index].update = 1;
    if (index) {
        device->int_clr_timers.t1 = 1;
    } else {
        device->int_clr_timers.t0 = 1;
    }
    device->hw_timer[index].config.alarm_en = tim_isr->config.reload_mode;

    #endif

    tim_isr->config.cb(tim_isr->config.arg);
}

int32_t aos_hal_timer_init_internel(aos_hal_timer_dev_t *tim)
{
    esp_err_t ret = 0;
    uint64_t set_period = 0;
    timer_config_t config;
    config.alarm_en = TIMER_ALARM_EN;
    config.auto_reload = tim->config.reload_mode;
    config.counter_dir = TIMER_COUNT_UP;
    config.divider = TIMER_DIVIDER;
    config.intr_type = TIMER_INTR_LEVEL;
    config.counter_en = TIMER_PAUSE;

    // calc group & index
    uint8_t group = ((tim->port) >> 1) & 1;
    uint8_t index = (tim->port) & 1;

    // calc period time
    set_period =  (tim->config.period) / 1000UL;
    set_period =  (((uint64_t)(set_period)) * TIMER_SCALE) / 1000UL;

    ret = timer_init(group, index, &config);
    ret = timer_set_counter_value(group, index, 0x00000000);
    ret = timer_set_alarm_value(group, index, set_period);
    ret = timer_enable_intr(group, index);
    ret = timer_isr_register(group, index, aos_hal_timer_isr, (void *)tim, TIMER_FLAGS, (timer_isr_handle_t *)&tim->config.handle);

    return ret;
}

int32_t aos_hal_timer_init(aos_hal_timer_dev_t *tim)
{
    esp_err_t ret = 0;
    return ret;
}

int32_t aos_hal_timer_start(aos_hal_timer_dev_t *tim)
{
    esp_err_t ret = 0;
    // calc group & index
    uint8_t group = ((tim->port) >> 1) & 1;
    uint8_t index = (tim->port) & 1;

    // reset timer
    aos_hal_timer_stop(tim);
    ret = aos_hal_timer_init_internel(tim);
    // start timer
    ret = timer_start(group, index);
    return ret;
}

void aos_hal_timer_stop(aos_hal_timer_dev_t *tim)
{
    // calc group & index
    uint8_t group = ((tim->port) >> 1) & 1;
    uint8_t index = (tim->port) & 1;

    if (tim->config.handle) {
        timer_pause(group, index);
        timer_disable_intr(group, index);
        esp_intr_free(tim->config.handle);
        tim->config.handle = NULL;
    }
}

int32_t aos_hal_timer_reload(aos_hal_timer_dev_t *tim)
{
    esp_err_t ret = 0;

    aos_hal_timer_stop(tim);
    ret = aos_hal_timer_init_internel(tim);
    ret = aos_hal_timer_start(tim);

    return ret;
}

int32_t aos_hal_timer_para_chg(aos_hal_timer_dev_t *tim, aos_hal_timer_config_t para)
{
    esp_err_t ret = 0;

    aos_hal_timer_stop(tim);
    // change tim config params
    tim->config.arg = para.arg;
    tim->config.period = para.period;
    tim->config.reload_mode = para.reload_mode;
    // reset timer
    ret = aos_hal_timer_init_internel(tim);
    ret = aos_hal_timer_start(tim);

    return ret;
}

int32_t aos_hal_timer_finalize(aos_hal_timer_dev_t *tim)
{
    int32_t ret = 0;
    return ret;
}
