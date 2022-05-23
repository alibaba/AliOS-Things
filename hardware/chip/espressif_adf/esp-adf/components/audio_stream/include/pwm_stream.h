/*
 *  * ESPRESSIF MIT License
 *
 * Copyright (c) 2020 <ESPRESSIF SYSTEMS (SHANGHAI) CO., LTD>
 *
 * Permission is hereby granted for use on all ESPRESSIF SYSTEMS products, in which case,
 * it is free of charge, to any person obtaining a copy of this software and associated
 * documentation files (the "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished
 * to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#ifndef _PWM_STREAM_H_
#define _PWM_STREAM_H_

#include "driver/ledc.h"
#include "driver/timer.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief      PWM audio configurations
 *           
 */
typedef struct
{
    timer_group_t       tg_num;               /*!< timer group number (0 - 1) */
    timer_idx_t         timer_num;            /*!< timer number  (0 - 1) */
    int                 gpio_num_left;        /*!< the LEDC output gpio_num, Left channel */
    int                 gpio_num_right;       /*!< the LEDC output gpio_num, Right channel */
    ledc_channel_t      ledc_channel_left;    /*!< LEDC channel (0 - 7), Corresponding to left channel*/
    ledc_channel_t      ledc_channel_right;   /*!< LEDC channel (0 - 7), Corresponding to right channel*/
    ledc_timer_t        ledc_timer_sel;       /*!< Select the timer source of channel (0 - 3) */
    ledc_timer_bit_t    duty_resolution;      /*!< ledc pwm bits */
    uint32_t            data_len;             /*!< ringbuffer size */

} audio_pwm_config_t;

/**
 * @brief      PWM Stream configurations
 *             Default value will be used if any entry is zero
 */
typedef struct {
    audio_stream_type_t     type;               /*!< Type of stream */
    audio_pwm_config_t      pwm_config;         /*!<  driver configurations */
    int                     out_rb_size;        /*!< Size of output ringbuffer */
    int                     task_stack;         /*!< Task stack size */
    int                     task_core;          /*!< Task running in core (0 or 1) */
    int                     task_prio;          /*!< Task priority (based on freeRTOS priority) */
    int                     buffer_len;         /*!< pwm_stream buffer length */
    bool                    ext_stack;          /*!< Allocate stack on extern ram */
} pwm_stream_cfg_t;

#define PWM_STREAM_GPIO_NUM_LEFT  GPIO_NUM_18
#define PWM_STREAM_GPIO_NUM_RIGHT GPIO_NUM_17

#define PWM_STREAM_TASK_STACK           (3072+512)
#define PWM_STREAM_BUF_SIZE             (2048)
#define PWM_STREAM_TASK_PRIO            (23)
#define PWM_STREAM_TASK_CORE            (0)
#define PWM_STREAM_RINGBUFFER_SIZE      (0)
#define PWM_CONFIG_RINGBUFFER_SIZE      (1024*8)

#define PWM_STREAM_CFG_DEFAULT() {                    \
    .type = AUDIO_STREAM_WRITER,                      \
    .pwm_config = {                                   \
        .tg_num = TIMER_GROUP_0,                      \
        .timer_num = TIMER_0,                         \
        .gpio_num_left = PWM_STREAM_GPIO_NUM_LEFT,    \
        .gpio_num_right = PWM_STREAM_GPIO_NUM_RIGHT,  \
        .ledc_channel_left = LEDC_CHANNEL_0,          \
        .ledc_channel_right = LEDC_CHANNEL_1,         \
        .ledc_timer_sel = LEDC_TIMER_0,               \
        .duty_resolution = LEDC_TIMER_8_BIT,          \
        .data_len = PWM_CONFIG_RINGBUFFER_SIZE,       \
    },                                                \
    .out_rb_size = PWM_STREAM_RINGBUFFER_SIZE,        \
    .task_stack = PWM_STREAM_TASK_STACK,              \
    .task_core = PWM_STREAM_TASK_CORE,                \
    .task_prio = PWM_STREAM_TASK_PRIO,                \
    .buffer_len =  PWM_STREAM_BUF_SIZE,               \
    .ext_stack = false,                               \
}

/**
 * @brief      Initialize PWM stream
 *
 * @param      config   The PWM Stream configuration
 *
 * @return     The audio element handle
 */
audio_element_handle_t pwm_stream_init(pwm_stream_cfg_t *config);

/**
 * @brief      Setup clock for PWM Stream, this function is only used with handle created by `pwm_stream_init`
 *
 * @param[in]  pwm_stream   The pwm element handle
 * @param[in]  rate  Clock rate (in Hz)
 * @param[in]  bits  Audio bit width (16, 32)
 * @param[in]  ch    Number of Audio channels (1: Mono, 2: Stereo)
 *
 * @return
 *     - ESP_OK
 *     - ESP_FAIL
 */
esp_err_t pwm_stream_set_clk(audio_element_handle_t pwm_stream, int rate, int bits, int ch);

#ifdef __cplusplus
}
#endif

#endif
