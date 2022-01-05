/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

#include "aos_hal_pwm.h"
#include "driver/ledc.h"

bool pwm_inited = false;
int chan_gpio[LEDC_CHANNEL_MAX];

// Params for PW operation
// 5khz
#define PWFREQ (5000)
// High speed mode
#if CONFIG_IDF_TARGET_ESP32
#define PWMODE (LEDC_HIGH_SPEED_MODE)
#else
#define PWMODE (LEDC_LOW_SPEED_MODE)
#endif
// 10-bit resolution (compatible with esp8266 PWM)
#define PWRES (LEDC_TIMER_10_BIT)
// Timer 1
#define PWTIMER (LEDC_TIMER_1)

ledc_timer_config_t timer_cfg = {
    .duty_resolution = PWRES,
    .freq_hz = PWFREQ,
    .speed_mode = PWMODE,
    .timer_num = PWTIMER
};

int32_t aos_hal_pwm_init(aos_hal_pwm_dev_t *pwm)
{
    // init need set params
    pwm->active = 0;
    pwm->channel = -1;

    // init esp32 hw pwm
    if (!pwm_inited) {
        // Initial condition: no channels assigned
        for (int x = 0; x < LEDC_CHANNEL_MAX; ++x) {
            chan_gpio[x] = -1;
        }
        // Init with default timer params
        ledc_timer_config(&timer_cfg);
        pwm_inited = true;
    }

    int channel;
    int avail = -1;

    // Find a free PWM channel, also spot if our pin is
    //  already mentioned.
    for (channel = 0; channel < LEDC_CHANNEL_MAX; ++channel) {
        if (chan_gpio[channel] == pwm->port) {
            break;
        }
        if ((avail == -1) && (chan_gpio[channel] == -1)) {
            avail = channel;
        }
    }
    if (channel >= LEDC_CHANNEL_MAX) {
        if (avail == -1) {
            printf("out of PWM channels\r\n");
        }
        channel = avail;
    }
    pwm->channel = channel;
    pwm->active = 1;

    // New PWM assignment
    if (chan_gpio[channel] == -1) {
        ledc_channel_config_t cfg = {
            .channel = channel,
            .duty = (1 << timer_cfg.duty_resolution) / 2,
            .gpio_num = pwm->port,
            .intr_type = LEDC_INTR_DISABLE,
            .speed_mode = PWMODE,
            .timer_sel = PWTIMER,
        };
        if (ledc_channel_config(&cfg) != ESP_OK) {
            printf("PWM not supported on pin %d\r\n", pwm->port);
        }
        chan_gpio[channel] = pwm->port;
    }

    return 0;
}

int32_t aos_hal_pwm_start(aos_hal_pwm_dev_t *pwm)
{
    esp_err_t ret = 0;
    ret = ledc_update_duty(PWMODE, pwm->channel);
    return ret;
}

int32_t aos_hal_pwm_stop(aos_hal_pwm_dev_t *pwm)
{
    esp_err_t ret = 0;

    int channel = pwm->channel;
    // Valid channel?
    if ((channel >= 0) && (channel < LEDC_CHANNEL_MAX)) {
        // Mark it unused, and tell the hardware to stop routing
        ret = ledc_stop(PWMODE, channel, 0);
    }
    return ret;
}

int32_t aos_hal_pwm_para_chg(aos_hal_pwm_dev_t *pwm, aos_hal_pwm_config_t para)
{
    esp_err_t ret = 0;
    // set freq frist
    if (para.freq != timer_cfg.freq_hz) {
        int ores = timer_cfg.duty_resolution;
        int oval = timer_cfg.freq_hz;

        // Find the highest bit resolution for the requested frequency
        if (para.freq <= 0) {
            para.freq = 1;
        }
        unsigned int res = 0;
        for (unsigned int i = LEDC_APB_CLK_HZ / para.freq; i > 1; i >>= 1) {
            ++res;
        }
        if (res == 0) {
            res = 1;
        } else if (res > PWRES) {
            // Limit resolution to PWRES to match units of our duty
            res = PWRES;
        }

        // Configure the new resolution and frequency
        timer_cfg.duty_resolution = res;
        timer_cfg.freq_hz = para.freq;
        if (ledc_timer_config(&timer_cfg) != ESP_OK) {
            timer_cfg.duty_resolution = ores;
            timer_cfg.freq_hz = oval;
            return -1;
        }
    }

    // set duty
    // calc max duty value
    int max_duty = 0;
    max_duty = (1 << timer_cfg.duty_resolution);
    // calc customer duty value
    int duty = 0;
    duty = (int)(para.duty_cycle * max_duty / 100);
    // duty protect
    duty &= ((1 << PWRES) - 1);
    duty >>= PWRES - timer_cfg.duty_resolution;

    ret = ledc_set_duty(PWMODE, pwm->channel, duty);

    return ret;
}

int32_t aos_hal_pwm_finalize(aos_hal_pwm_dev_t *pwm)
{
    esp_err_t ret = 0;
    int channel = pwm->channel;
    // Valid channel?
    if ((channel >= 0) && (channel < LEDC_CHANNEL_MAX)) {
        // Mark it unused, and tell the hardware to stop routing
        chan_gpio[channel] = -1;
        ret = ledc_stop(PWMODE, channel, 0);
        pwm->active = 0;
        pwm->channel = -1;
        gpio_matrix_out(pwm->port, SIG_GPIO_OUT_IDX, false, false);
    }
    return ret;
}



