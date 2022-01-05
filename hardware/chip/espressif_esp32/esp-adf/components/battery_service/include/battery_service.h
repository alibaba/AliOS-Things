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
#ifndef __BATTERY_SERVICE_H__
#define __BATTERY_SERVICE_H__

#include "periph_service.h"
#include "voltage_monitor.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Battery service event
 */
typedef enum {
    BAT_SERV_EVENT_UNKNOWN,

    /* Battery monitor's events */
    BAT_SERV_EVENT_VOL_REPORT = 1,
    BAT_SERV_EVENT_BAT_FULL,
    BAT_SERV_EVENT_BAT_LOW,

    /* Charger monitor's events */
    BAT_SERV_EVENT_CHARGING_BEGIN = 100,
    BAT_SERV_EVENT_CHARGING_STOP,
} battery_service_event_t;

/**
 * @brief Battery service configure
 */
typedef struct {
    /* Service Basic */
    int task_stack;           /*!< >0 Service task stack; =0 with out task created */
    int task_prio;            /*!< Service task priority (based on freeRTOS priority) */
    int task_core;            /*!< Service task running in core (0 or 1) */
    bool extern_stack;        /*!< Task stack allocate on the extern ram */
    periph_service_cb evt_cb; /*!< Service callback function */
    void *cb_ctx;             /*!< Callback context */

    /* Battery monitor */
    vol_monitor_handle_t vol_monitor;

    /* Charger monitor */
    void *charger_monitor; /*!< Not supported yet */
} battery_service_config_t;

#define BATTERY_SERVICE_DEFAULT_CONFIG() \
    {                                    \
        .task_stack = 3 * 1024,          \
        .task_prio = 6,                  \
        .task_core = 0,                  \
        .evt_cb = NULL,                  \
        .cb_ctx = NULL,                  \
        .extern_stack = true,            \
        .vol_monitor = NULL,             \
    }

/**
  * @brief     Create the battery service instance
  *
  * @param     config  configuration of the battery service
  *
  * @return
  *    - NULL:  Failed
  *    - Others: Success
  */
periph_service_handle_t battery_service_create(battery_service_config_t *config);

/**
  * @brief     Start or stop the battery voltage report
  *
  * @param[in]   handle     pointer to 'periph_service_handle_t' structure
  * @param[in]  on_off      'true' to start, 'false' to stop
  *
  * @return
  *    - ESP_OK: Success
  *    - ESP_ERR_INVALID_ARG： handle is NULL
  */
esp_err_t battery_service_vol_report_switch(periph_service_handle_t handle, bool on_off);

/**
  * @brief     Set voltage report freqency
  *
  * @param[in]   handle     pointer to 'periph_service_handle_t' structure
  * @param[in]   freq       voltage report freqency
  *
  * @return
  *    - ESP_OK: Success
  *    - ESP_ERR_INVALID_ARG： handle is NULL
  */
esp_err_t battery_service_set_vol_report_freq(periph_service_handle_t handle, int freq);

#ifdef __cplusplus
}
#endif

#endif /* __BATTERY_SERVICE_H__ */