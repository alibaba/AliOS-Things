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

#ifndef _ESP_SSDP_H_
#define _ESP_SSDP_H_

#include "freertos/FreeRTOS.h"
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief SSDP Configuration struct
 */
typedef struct {
    char *udn;              /*!< Unique Device Name */
    char *location;         /*!< The ssdp location for this udn, format as example: http://192.168.8.196/path
                                 or http://${ip}/path which ${ip} will be replaced by device ip address */
    int port;               /*!< SSDP listening on this port, default port will be used if not set */
    int notify_interval_ms; /*!< SSDP notify interval */
    int task_stack;         /*!< Task stack size */
    int task_core;          /*!< Task running in core (0 or 1) */
    int task_prio;          /*!< Task priority (based on freeRTOS priority) */
    bool stack_in_ext;      /*!< Try to allocate stack in external memory */
} ssdp_config_t;

#define DEFAULT_SSDP_UDN                "ESP32_DMR"
#define DEFAULT_SSDP_LOCATION           "http://${ip}/rootDesc.xml"
#define DEFAULT_SSDP_PORT               (1900)
#define DEFAULT_SSDP_TASK_STACK         (4*1024)
#define DEFAULT_SSDP_TASK_CORE          (0)
#define DEFAULT_SSDP_TASK_PRIORITY      (6)
#define DEFAULT_SSDP_NOTIFY_INTERVAL_MS (30000)

#define SSDP_DEFAULT_CONFIG() {\
    .udn = DEFAULT_SSDP_UDN, \
    .location = DEFAULT_SSDP_LOCATION, \
    .port = DEFAULT_SSDP_PORT, \
    .notify_interval_ms = DEFAULT_SSDP_NOTIFY_INTERVAL_MS, \
    .task_stack = DEFAULT_SSDP_TASK_STACK, \
    .task_core = DEFAULT_SSDP_TASK_CORE, \
    .task_prio = DEFAULT_SSDP_TASK_PRIORITY, \
    .stack_in_ext = true, \
}

/**
 * @brief SSDP service description
 */
typedef struct {
    char *uuid;
    char *usn;       //Unique Service Name
    char *location;
} ssdp_service_t;

/**
 * @brief Start SSDP responder service
 *
 * @param config SSDP configuration
 * @param services SSDP Search Target that will be responding
 *
 * @return
 *  - ESP_OK
 *  - ESP_FAIL
 */
esp_err_t ssdp_start(ssdp_config_t *config, const ssdp_service_t services[]);

/**
 * @brief Stop and free all SSDP resources
 */
void ssdp_stop();

/**
 * @brief Send SSDP byebye multicast
 *
 * @return
 *  - ESP_OK
 *  - ESP_FAIL
 */
esp_err_t ssdp_send_byebye();

#ifdef __cplusplus
}
#endif

#endif
