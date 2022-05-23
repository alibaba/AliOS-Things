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

#ifndef __PERIPH_SERVICE_H__
#define __PERIPH_SERVICE_H__

#include "audio_error.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Peripheral service state
 */
typedef enum {
    PERIPH_SERVICE_STATE_UNKNOWN,
    PERIPH_SERVICE_STATE_IDLE,
    PERIPH_SERVICE_STATE_RUNNING,
    PERIPH_SERVICE_STATE_STOPPED,
} periph_service_state_t;

/**
 * @brief Peripheral service event informations
 */
typedef struct {
    int         type;       /*!< Type of event */
    void       *source;     /*!< Event source */
    void       *data;       /*!< Event data */
    int         len;        /*!< Length of data */
} periph_service_event_t;

typedef struct periph_service_impl *periph_service_handle_t;
typedef esp_err_t (*periph_service_ctrl)(periph_service_handle_t handle);
typedef esp_err_t (*periph_service_io)(void *ioctl_handle, int cmd, int value);
typedef esp_err_t (*periph_service_cb)(periph_service_handle_t handle, periph_service_event_t *evt, void *ctx);

/**
 * @brief Peripheral service configurations
 */
typedef struct {
    int                         task_stack;             /*!< >0 Service task stack; =0 with out task created */
    int                         task_prio;              /*!< Service task priority (based on freeRTOS priority) */
    int                         task_core;              /*!< Service task running in core (0 or 1) */
    TaskFunction_t              task_func;              /*!< Service task function */
    bool                        extern_stack;           /*!< Task stack allocate on the extern ram */
    periph_service_ctrl         service_start;          /*!< Start function */
    periph_service_ctrl         service_stop;           /*!< Stop function */
    periph_service_ctrl         service_destroy;        /*!< Destroy function */
    periph_service_io           service_ioctl;          /*!< In out control function */
    char                        *service_name;          /*!< Name of peripheral service */
    void                        *user_data;             /*!< User data */
} periph_service_config_t;

/**
 * brief      Create peripheral service instance
 *
 * @param[in] config   Configuration of the peripheral service instance
 *
 * @return
 *     - NULL, Fail
 *     - Others, Success
 */
periph_service_handle_t periph_service_create(periph_service_config_t *config);

/**
 * brief      Destroy peripheral service instance
 *
 * @param[in] handle   The peripheral service instance
 *
 * @return
 *     - ESP_OK
 *     - ESP_FAIL
 *     - ESP_ERR_INVALID_ARG
 */
esp_err_t periph_service_destroy(periph_service_handle_t handle);

/**
 * brief      Start the specific peripheral service
 *
 * @param[in] handle   The peripheral service instance
 *
 * @return
 *     - ESP_OK
 *     - ESP_FAIL
 *     - ESP_ERR_INVALID_ARG
 */
esp_err_t periph_service_start(periph_service_handle_t handle);

/**
 * brief      Stop the specific peripheral service
 *
 * @param[in] handle   The peripheral service instance
 *
 * @return
 *     - ESP_OK
 *     - ESP_FAIL
 *     - ESP_ERR_INVALID_ARG
 */
esp_err_t periph_service_stop(periph_service_handle_t handle);

/**
 * brief      Set the specific peripheral service callback function
 *
 * @param[in] handle   The peripheral service instance
 * @param[in] cb       A pointer to service_callback
 * @param[in] ctx      A pointer to user context
 *
 * @return
 *     - ESP_OK
 *     - ESP_FAIL
 *     - ESP_ERR_INVALID_ARG
 */
esp_err_t periph_service_set_callback(periph_service_handle_t handle, periph_service_cb cb, void *ctx);

/**
 * brief      Called peripheral service by configurations
 *
 * @param[in] handle   The peripheral service instance
 * @param[in] evt      A pointer to periph_service_event_t
 *
 * @return
 *     - ESP_OK
 *     - ESP_FAIL
 *     - ESP_ERR_INVALID_ARG
 */
esp_err_t periph_service_callback(periph_service_handle_t handle, periph_service_event_t *evt);

/**
 * brief      Set user data to specific peripheral service instance
 *
 * @param[in] handle   The peripheral service instance
 * @param[in] data     A pointer to user data
 *
 * @return
 *     - ESP_OK
 *     - ESP_FAIL
 *     - ESP_ERR_INVALID_ARG
 */
esp_err_t periph_service_set_data(periph_service_handle_t handle, void *data);

/**
 * brief      Get user data by specific peripheral service instance
 *
 * @param[in] handle   The peripheral service instance
 *
 * @return A pointer to user data
 */
void *periph_service_get_data(periph_service_handle_t handle);

/**
 * brief      In/out control by peripheral service instance
 *
 * @param[in] handle            The peripheral service instance
 * @param[in] ioctl_handle      Sub-instance handle
 * @param[in] cmd               Command of value
 * @param[in] value             Data of the command
 *
 * @return
 *     - ESP_OK
 *     - ESP_FAIL
 *     - ESP_ERR_INVALID_ARG
 */
esp_err_t periph_service_ioctl(periph_service_handle_t handle, void *ioctl_handle, int cmd, int value);

#ifdef __cplusplus
}
#endif

#endif // End of __PERIPH_SERVICE_H__
