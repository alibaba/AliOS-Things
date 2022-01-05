// Copyright 2015-2019 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

// The HAL layer for Touch Sensor (common part)

#include "hal/touch_sensor_hal.h"
#include "hal/touch_sensor_types.h"

void touch_hal_init(void)
{
    touch_ll_intr_disable(TOUCH_PAD_INTR_MASK_ALL);
    touch_ll_clear_channel_mask(SOC_TOUCH_SENSOR_BIT_MASK_MAX);
    touch_ll_clear_trigger_status_mask();
    touch_ll_set_meas_times(TOUCH_PAD_MEASURE_CYCLE_DEFAULT);
    touch_ll_set_sleep_time(TOUCH_PAD_SLEEP_CYCLE_DEFAULT);
    touch_ll_set_voltage_high(TOUCH_PAD_HIGH_VOLTAGE_THRESHOLD);
    touch_ll_set_voltage_low(TOUCH_PAD_LOW_VOLTAGE_THRESHOLD);
    touch_ll_set_voltage_attenuation(TOUCH_PAD_ATTEN_VOLTAGE_THRESHOLD);
    touch_ll_set_idle_channel_connect(TOUCH_PAD_IDLE_CH_CONNECT_DEFAULT);
    /* Clear touch channels to initialize the channel value (benchmark, raw_data).
     * Note: Should call it after enable clock gate. */
    touch_ll_clkgate(true);  // Enable clock gate for touch sensor.
    touch_ll_reset_benchmark(TOUCH_PAD_MAX);
    touch_ll_sleep_reset_benchmark();
}

void touch_hal_deinit(void)
{
    touch_ll_reset_benchmark(TOUCH_PAD_MAX);
    touch_ll_sleep_reset_benchmark();
    touch_ll_stop_fsm();
    touch_ll_clkgate(false);
    touch_ll_clear_channel_mask(SOC_TOUCH_SENSOR_BIT_MASK_MAX);
    touch_ll_clear_trigger_status_mask();
    touch_ll_intr_disable(TOUCH_PAD_INTR_MASK_ALL);
    touch_ll_timeout_disable();
    touch_ll_waterproof_disable();
    touch_ll_denoise_disable();
    touch_pad_t prox_pad[SOC_TOUCH_PROXIMITY_CHANNEL_NUM] = {[0 ... (SOC_TOUCH_PROXIMITY_CHANNEL_NUM - 1)] = 0};
    touch_ll_proximity_set_channel_num((const touch_pad_t *)prox_pad);
    touch_ll_sleep_set_channel_num(0);
    touch_ll_sleep_disable_approach();
    touch_ll_reset();   // Reset the touch sensor FSM.
}

void touch_hal_filter_set_config(const touch_filter_config_t *filter_info)
{
    touch_ll_filter_set_filter_mode(filter_info->mode);
    touch_ll_filter_set_debounce(filter_info->debounce_cnt);
    touch_ll_filter_set_noise_thres(filter_info->noise_thr);
    touch_ll_filter_set_jitter_step(filter_info->jitter_step);
    touch_ll_filter_set_smooth_mode(filter_info->smh_lvl);
}

void touch_hal_filter_get_config(touch_filter_config_t *filter_info)
{
    touch_ll_filter_get_filter_mode(&filter_info->mode);
    touch_ll_filter_get_debounce(&filter_info->debounce_cnt);
    touch_ll_filter_get_noise_thres(&filter_info->noise_thr);
    touch_ll_filter_get_jitter_step(&filter_info->jitter_step);
    touch_ll_filter_get_smooth_mode(&filter_info->smh_lvl);
}

void touch_hal_denoise_set_config(const touch_pad_denoise_t *denoise)
{
    touch_ll_denoise_set_cap_level(denoise->cap_level);
    touch_ll_denoise_set_grade(denoise->grade);
}

void touch_hal_denoise_get_config(touch_pad_denoise_t *denoise)
{
    touch_ll_denoise_get_cap_level(&denoise->cap_level);
    touch_ll_denoise_get_grade(&denoise->grade);
}

void touch_hal_denoise_enable(void)
{
    touch_ll_clear_channel_mask(1U << SOC_TOUCH_DENOISE_CHANNEL);
    touch_ll_denoise_enable();
}

void touch_hal_waterproof_set_config(const touch_pad_waterproof_t *waterproof)
{
    touch_ll_waterproof_set_guard_pad(waterproof->guard_ring_pad);
    touch_ll_waterproof_set_sheild_driver(waterproof->shield_driver);
}

void touch_hal_waterproof_get_config(touch_pad_waterproof_t *waterproof)
{
    touch_ll_waterproof_get_guard_pad(&waterproof->guard_ring_pad);
    touch_ll_waterproof_get_sheild_driver(&waterproof->shield_driver);
}

void touch_hal_waterproof_enable(void)
{
    touch_ll_clear_channel_mask(1U << SOC_TOUCH_SHIELD_CHANNEL);
    touch_ll_waterproof_enable();
}

bool touch_hal_enable_proximity(touch_pad_t touch_num, bool enabled)
{
    int i = 0;
    touch_pad_t ch_num[SOC_TOUCH_PROXIMITY_CHANNEL_NUM] = {0};
    touch_ll_proximity_get_channel_num(ch_num);
    if (enabled) {
        for (i = 0; i < SOC_TOUCH_PROXIMITY_CHANNEL_NUM; i++) {
            if (ch_num[i] == TOUCH_PAD_NUM0 || ch_num[i] >= TOUCH_PAD_MAX || ch_num[i] == touch_num) {
                ch_num[i] = touch_num;
                break;
            }
        }
        if (i == SOC_TOUCH_PROXIMITY_CHANNEL_NUM) {
            return false;
        }
    } else {
        for (i = 0; i < SOC_TOUCH_PROXIMITY_CHANNEL_NUM; i++) {
            if (ch_num[i] == touch_num) {
                ch_num[i] = TOUCH_PAD_NUM0;
                break;
            }
        }
    }
    touch_ll_proximity_set_channel_num(ch_num);
    return true;
}

void touch_hal_sleep_channel_enable(touch_pad_t pad_num, bool enable)
{
    if (enable) {
        touch_ll_sleep_set_channel_num(pad_num);
        touch_ll_sleep_set_threshold(SOC_TOUCH_PAD_THRESHOLD_MAX);
        /* Default change touch dbias to self-dbias to save power.
        Measuring the sleep pad threshold after `sleep_channel_set_config`. */
        touch_ll_sleep_low_power(true);
        touch_ll_sleep_reset_benchmark();
    } else {
        touch_ll_sleep_set_channel_num(TOUCH_PAD_NUM0);
    }
}

void touch_hal_sleep_channel_get_config(touch_pad_sleep_channel_t *slp_config)
{
    touch_ll_sleep_get_channel_num(&slp_config->touch_num);
    slp_config->en_proximity = touch_ll_sleep_get_approach_status();
}


