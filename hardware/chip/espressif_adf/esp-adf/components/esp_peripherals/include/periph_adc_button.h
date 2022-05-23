/*
 * ESPRESSIF MIT License
 *
 * Copyright (c) 2018 <ESPRESSIF SYSTEMS (SHANGHAI) PTE LTD>
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

#ifndef _PERIPH_ADC_BUTTON_H_
#define _PERIPH_ADC_BUTTON_H_

#include "driver/adc.h"
#include "adc_button.h"
#include "esp_peripherals.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ADC_BUTTON_STACK_SIZE           2500
#define ADC_BUTTON_TASK_PRIORITY        10
#define ADC_BUTTON_TASK_CORE_ID         0

/**
 * @brief      The configuration of ADC Button
 */
typedef struct {
    adc_arr_t *arr;  /*!< An array with configuration of buttons */
    int arr_size;    /*!< The array size */
    adc_btn_task_cfg_t task_cfg; /*!< Adc button task configuration */
} periph_adc_button_cfg_t;

#define PERIPH_ADC_BUTTON_DEFAULT_CONFIG() {   \
    .task_cfg = {                              \
        .task_stack = ADC_BUTTON_STACK_SIZE,   \
        .task_core  = ADC_BUTTON_TASK_CORE_ID, \
        .task_prio  = ADC_BUTTON_TASK_PRIORITY,\
        .ext_stack  = false                    \
    }                                          \
}

typedef enum {
    PERIPH_ADC_BUTTON_IDLE = 0,
    PERIPH_ADC_BUTTON_PRESSED,
    PERIPH_ADC_BUTTON_RELEASE,
    PERIPH_ADC_BUTTON_LONG_PRESSED,
    PERIPH_ADC_BUTTON_LONG_RELEASE,
} periph_adc_button_event_id_t;

/**
* ESP32 ADC1 channels and GPIO table
*   ADC1_CHANNEL_0 -  GPIO36
*   ADC1_CHANNEL_1 -  GPIO37
*   ADC1_CHANNEL_2 -  GPIO38
*   ADC1_CHANNEL_3 -  GPIO39
*   ADC1_CHANNEL_4 -  GPIO32
*   ADC1_CHANNEL_5 -  GPIO33
*   ADC1_CHANNEL_6 -  GPIO34
*   ADC1_CHANNEL_7 -  GPIO35
*
**/

#define ADC_DEFAULT_ARR() {   \
    .adc_ch = ADC1_CHANNEL_3, \
    .adc_level_step = NULL,   \
    .total_steps = 6,         \
    .press_judge_time = 3000, \
}

/**
 * @brief      Create the button peripheral handle for esp_peripherals.
 *
 * @note       The handle created by this function is automatically destroyed when esp_periph_destroy is called.
 *
 * @param      btn_cfg  The button configuration.
 *
 * @return     The esp peripheral handle.
 */
esp_periph_handle_t periph_adc_button_init(periph_adc_button_cfg_t *btn_cfg);

#ifdef __cplusplus
}
#endif

#endif
