/*
 * ESPRESSIF MIT License
 *
 * Copyright (c) 2019 <ESPRESSIF SYSTEMS (SHANGHAI) CO., LTD>
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

#ifndef __AUDIO_SERVICE_H__
#define __AUDIO_SERVICE_H__

#include "audio_error.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Audio service state
 */
typedef enum {
    SERVICE_STATE_UNKNOWN,
    SERVICE_STATE_IDLE,
    SERVICE_STATE_CONNECTING,
    SERVICE_STATE_CONNECTED,
    SERVICE_STATE_RUNNING,
    SERVICE_STATE_STOPPED,
} service_state_t;

/**
 * @brief Audio service event informations
 */
typedef struct {
    int         type;       /*!< Type of event */
    void       *source;     /*!< Event source */
    void       *data;       /*!< Event data */
    int         len;        /*!< Length of data */
} service_event_t;

typedef struct audio_service_impl *audio_service_handle_t;
typedef esp_err_t (*service_ctrl)(audio_service_handle_t handle);
typedef esp_err_t (*service_callback)(audio_service_handle_t handle, service_event_t *evt, void *ctx);

/**
 * @brief Audio service configurations
 */
typedef struct {
    int                 task_stack;             /*!< >0 Service task stack; =0 with out task created */
    int                 task_prio;              /*!< Service task priority (based on freeRTOS priority) */
    int                 task_core;              /*!< Service task running in core (0 or 1) */
    TaskFunction_t      task_func;              /*!< A pointer to TaskFunction_t for service task function */
    service_ctrl        service_start;          /*!< Start function */
    service_ctrl        service_stop;           /*!< Stop function */
    service_ctrl        service_connect;        /*!< Connect function */
    service_ctrl        service_disconnect;     /*!< Disconnect function */
    service_ctrl        service_destroy;        /*!< Destroy function */
    const char          *service_name;          /*!< Name of audio service */
    void                *user_data;             /*!< User context */
} audio_service_config_t;

/**
 * brief      Create audio service instance
 *
 * @param[in] config   Configuration of the audio service instance
 *
 * @return
 *     - NULL, Fail
 *     - Others, Success
 */
audio_service_handle_t audio_service_create(audio_service_config_t *config);

/**
 * brief      Destroy audio service instance
 *
 * @param[in] handle   The audio service instance
 *
 * @return
 *     - ESP_OK
 *     - ESP_FAIL
 *     - ESP_ERR_INVALID_ARG
 */
esp_err_t audio_service_destroy(audio_service_handle_t handle);

/**
 * brief      Start the specific audio service
 *
 * @param[in] handle   The audio service instance
 *
 * @return
 *     - ESP_OK
 *     - ESP_FAIL
 *     - ESP_ERR_INVALID_ARG
 */
esp_err_t audio_service_start(audio_service_handle_t handle);

/**
 * brief      Stop the specific audio service
 *
 * @param[in] handle   The audio service instance
 *
 * @return
 *     - ESP_OK
 *     - ESP_FAIL
 *     - ESP_ERR_INVALID_ARG
 */
esp_err_t audio_service_stop(audio_service_handle_t handle);

/**
 * brief      Set the specific audio service callback function.
 *
 * @param[in] handle   The audio service instance
 * @param[in] cb       A pointer to service_callback
 * @param[in] ctx      A pointer to user context
 *
 * @return
 *     - ESP_OK
 *     - ESP_FAIL
 *     - ESP_ERR_INVALID_ARG
 */
esp_err_t audio_service_set_callback(audio_service_handle_t handle, service_callback cb, void *ctx);

/**
 * brief      Called audio service by configurations
 *
 * @param[in] handle   The audio service instance
 * @param[in] evt      A pointer to service_event_t
 *
 * @return
 *     - ESP_OK
 *     - ESP_FAIL
 *     - ESP_ERR_INVALID_ARG
 */
esp_err_t audio_service_callback(audio_service_handle_t handle, service_event_t *evt);

/**
 * brief      Connect the specific audio service
 *
 * @param[in] handle   The audio service instance
 *
 * @return
 *     - ESP_OK
 *     - ESP_FAIL
 *     - ESP_ERR_INVALID_ARG
 */
esp_err_t audio_service_connect(audio_service_handle_t handle);

/**
 * brief      Disconnect the specific audio service
 *
 * @param[in] handle   The audio service instance
 *
 * @return
 *     - ESP_OK
 *     - ESP_FAIL
 *     - ESP_ERR_INVALID_ARG
 */
esp_err_t audio_service_disconnect(audio_service_handle_t handle);

/**
 * brief      Set user data to specific audio service instance
 *
 * @param[in] handle   The audio service instance
 * @param[in] data     A pointer to user data
 *
 * @return
 *     - ESP_OK
 *     - ESP_FAIL
 *     - ESP_ERR_INVALID_ARG
 */
esp_err_t audio_service_set_data(audio_service_handle_t handle, void *data);

/**
 * brief      Get user data by specific audio service instance
 *
 * @param[in] handle   The audio service instance
 *
 * @return A pointer to user data
 */
void *audio_service_get_data(audio_service_handle_t handle);

#ifdef __cplusplus
}
#endif

#endif // End of __AUDIO_SERVICE_H__
