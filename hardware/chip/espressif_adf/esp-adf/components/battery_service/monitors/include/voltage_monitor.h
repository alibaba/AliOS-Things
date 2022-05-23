/*
 * ESPRESSIF MIT License
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

#ifndef __VOL_MONITOR_H__
#define __VOL_MONITOR_H__

#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Battery adc configure
 */
typedef enum {
    VOL_MONITOR_EVENT_FREQ_REPORT,
    VOL_MONITOR_EVENT_BAT_FULL,
    VOL_MONITOR_EVENT_BAT_LOW,
} vol_monitor_event_t;

/**
 * @brief Battery adc configure
 */
typedef struct {
    bool (*init)(void *);   /*!< Voltage read init */
    bool (*deinit)(void *); /*!< Voltage read deinit */
    int (*vol_get)(void *); /*!< Voltage read interface */
    void *user_data;        /*!< Parameters for callbacks */
    int read_freq;          /*!< Voltage read frequency, unit: s*/
    int report_freq;        /*!< Voltage report frequency, voltage will be report with a interval calculate by （`read_freq` * `report_freq`） */
    int vol_full_threshold; /*!< Voltage threshold to report, unit: mV */
    int vol_low_threshold;  /*!< Voltage threshold to report, unit: mV */
} vol_monitor_param_t;

/**
 * @brief voltage monitor handle
 */
typedef void *vol_monitor_handle_t;

/**
 * @brief callback define
 */
typedef void (*vol_monitor_event_cb)(int msg_id, void *data, void *user_ctx);

/**
  * @brief     Create the voltage monitor instance
  *
  * @param[in]  config      pointer to 'vol_monitor_param_t' structure
  *
  * @return
  *    - NULL:  Failed
  *    - Others: Success
  */
vol_monitor_handle_t vol_monitor_create(vol_monitor_param_t *config);

/**
  * @brief     Destroy the voltage monitor
  *
  * @param[in]  handle      pointer to 'vol_monitor_handle_t' structure
  *
  * @return
  *    - ESP_OK:  Success
  *    - Others: Failed
  */
esp_err_t vol_monitor_destroy(vol_monitor_handle_t handle);

/**
  * @brief     Set the event callback
  *
  * @param[in]  handle      pointer to 'vol_monitor_handle_t' structure
  * @param[in]  event_cb    callback used to handle the events of voltage monitor
  * @param[in]  user_ctx    user's data
  *
  * @return
  *    - ESP_OK:  Success
  *    - Others: Failed
  */
esp_err_t vol_monitor_set_event_cb(vol_monitor_handle_t handle, vol_monitor_event_cb event_cb, void *user_ctx);

/**
  * @brief     Start the voltage report with the configured frequency
  *
  * @param[in]  handle      pointer to 'vol_monitor_handle_t' structure
  *
  * @return
  *    - ESP_OK:  Success
  *    - Others: Failed
  */
esp_err_t vol_monitor_start_freq_report(vol_monitor_handle_t handle);

/**
  * @brief     Stop the voltage frequency report
  *
  * @param[in]  handle      pointer to 'vol_monitor_handle_t' structure
  *
  * @return
  *    - ESP_OK:  Success
  *    - Others: Failed
  */
esp_err_t vol_monitor_stop_freq_report(vol_monitor_handle_t handle);

/**
  * @brief     Set the voltage report frequency
  *
  * @param[in]  handle      pointer to 'vol_monitor_handle_t' structure
  * @param[in]  freq        voltage report freqency
  *
  * @return
  *    - ESP_OK:  Success
  *    - Others: Failed
  */
esp_err_t vol_monitor_set_report_freq(vol_monitor_handle_t handle, int freq);

#ifdef __cplusplus
}
#endif

#endif /* __BATTERY_SERVICE_H__ */