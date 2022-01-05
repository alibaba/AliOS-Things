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

#ifndef __COREDUMP_UPLOAD_H__
#define __COREDUMP_UPLOAD_H__

#include <stdbool.h>
#include "esp_err.h"
#include "periph_service.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief coredump service configuration parameters
 */
typedef struct {
    int task_stack;           /*!< >0 Service task stack; =0 with out task created */
    int task_prio;            /*!< Service task priority (based on freeRTOS priority) */
    int task_core;            /*!< Service task running in core (0 or 1) */
    periph_service_cb evt_cb; /*!< Service callback function */
    void *cb_ctx;             /*!< Callback context */
    bool (*do_post)(char *url, uint8_t *data, size_t len); /*!< POST interface, users can override this to customize the http client.
                                                                if left NULL, the service will use the default one */
} coredump_upload_service_config_t;

#define COREDUMP_UPLOAD_SERVICE_DEFAULT_CONFIG() \
    {                                \
        .task_stack = 4096,          \
        .task_prio = 5,              \
        .task_core = 1,              \
        .evt_cb = NULL,              \
        .cb_ctx = NULL,              \
        .do_post = NULL,             \
    }

/**
 * @brief      This function will check the reset code and determine whether to upload the coredump
 * @return
 *     - true: last reboot is a abnormal reset.
 *     - false
 */
bool coredump_need_upload();

/**
 * @brief      Upload the core dump image to the url.
 *             This function will block the current task until the upload process finished.
 *
 * @param[in]  handle   the 'periph_service_handle_t'
 * @param[in]  url      server addr
 *
 * @return
 *     - ESP_OK
 *     - ESP_FAIL
 */
esp_err_t coredump_upload(periph_service_handle_t handle, char *url);

/**
 * @brief     Create the core dump upload service instance
 *
 * @param     config  configuration of the OTA service
 *
 * @return
 *    - NULL:  Failed
 *    - Others: Success
 */
periph_service_handle_t coredump_upload_service_create(coredump_upload_service_config_t *config);

#ifdef __cplusplus
}
#endif

#endif /* __COREDUMP_UPLOAD_H__ */
