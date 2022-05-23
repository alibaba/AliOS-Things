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

#ifndef _AUDIO_EVENT_IFACE_H_
#define _AUDIO_EVENT_IFACE_H_

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Event message
 */
typedef struct {
    int cmd;                /*!< Command id */
    void *data;             /*!< Data pointer */
    int data_len;           /*!< Data length */
    void *source;           /*!< Source event */
    int source_type;        /*!< Source type (To know where it came from) */
    bool need_free_data;    /*!< Need to free data pointer after the event has been processed */
} audio_event_iface_msg_t;

typedef esp_err_t (*on_event_iface_func)(audio_event_iface_msg_t *, void *);

typedef struct audio_event_iface *audio_event_iface_handle_t;

/**
 * Event interface configurations
 */
typedef struct {
    int                 internal_queue_size;        /*!< It's optional, Queue size for event `internal_queue` */
    int                 external_queue_size;        /*!< It's optional, Queue size for event `external_queue` */
    int                 queue_set_size;             /*!< It's optional, QueueSet size for event `queue_set`*/
    on_event_iface_func on_cmd;                     /*!< Function callback for listener when any event arrived */
    void                *context;                   /*!< Context will pass to callback function */
    TickType_t          wait_time;                  /*!< Timeout to check for event queue */
    int                 type;                       /*!< it will pass to audio_event_iface_msg_t source_type (To know where it came from) */
} audio_event_iface_cfg_t;


#define DEFAULT_AUDIO_EVENT_IFACE_SIZE  (5)

#define AUDIO_EVENT_IFACE_DEFAULT_CFG() {                   \
    .internal_queue_size = DEFAULT_AUDIO_EVENT_IFACE_SIZE,  \
    .external_queue_size = DEFAULT_AUDIO_EVENT_IFACE_SIZE,  \
    .queue_set_size = DEFAULT_AUDIO_EVENT_IFACE_SIZE,       \
    .on_cmd = NULL,                                         \
    .context = NULL,                                        \
    .wait_time = portMAX_DELAY,                             \
    .type = 0,                                              \
}

/**
 * @brief      Initialize audio event
 *
 * @param      config  The configurations
 *
 * @return
 *     - ESP_OK
 *     - ESP_FAIL
 */
audio_event_iface_handle_t audio_event_iface_init(audio_event_iface_cfg_t *config);

/**
 * @brief      Cleanup event, it doesn't free evt pointer
 *
 * @param      evt   The event
 *
 * @return
 *     - ESP_OK
 *     - ESP_FAIL
 */
esp_err_t audio_event_iface_destroy(audio_event_iface_handle_t evt);

/**
 * @brief      Add audio event `evt` to the listener, then we can listen `evt` event from `listen`
 *
 * @param      listener     The event can listen another event
 * @param      evt          The event to be added to
 *
 * @return
 *     - ESP_OK
 *     - ESP_FAIL
 */
esp_err_t audio_event_iface_set_listener(audio_event_iface_handle_t evt, audio_event_iface_handle_t listener);

/**
 * @brief      Remove audio event `evt` from the listener
 *
 * @param      listener     The event listener
 * @param      evt          The event to be removed from
 *
 * @return
 *     - ESP_OK
 *     - ESP_FAIL
 */
esp_err_t audio_event_iface_remove_listener(audio_event_iface_handle_t listener, audio_event_iface_handle_t evt);

/**
 * @brief      Set current queue wait time for the event
 *
 * @param      evt        The event
 * @param[in]  wait_time  The wait time
 *
 * @return
 *     - ESP_OK
 *     - ESP_FAIL
 */
esp_err_t audio_event_iface_set_cmd_waiting_timeout(audio_event_iface_handle_t evt, TickType_t wait_time);

/**
 * @brief      Waiting internal queue message
 *
 * @param      evt        The event
 *
 * @return
 *     - ESP_OK
 *     - ESP_FAIL
 */
esp_err_t audio_event_iface_waiting_cmd_msg(audio_event_iface_handle_t evt);

/**
 * @brief      Trigger an event for internal queue with a message
 *
 * @param      evt   The event
 * @param      msg   The message
 *
 * @return
 *     - ESP_OK
 *     - ESP_FAIL
 */
esp_err_t audio_event_iface_cmd(audio_event_iface_handle_t evt, audio_event_iface_msg_t *msg);

/**
 * @brief      It's same with `audio_event_iface_cmd`, but can send a message from ISR
 *
 * @param[in]  evt   The event
 * @param      msg   The message
 *
 * @return
 *     - ESP_OK
 *     - ESP_FAIL
 */
esp_err_t audio_event_iface_cmd_from_isr(audio_event_iface_handle_t evt, audio_event_iface_msg_t *msg);

/**
 * @brief      Trigger and event out with a message
 *
 * @param      evt   The event
 * @param      msg   The message
 *
 * @return
 *     - ESP_OK
 *     - ESP_FAIL
 */
esp_err_t audio_event_iface_sendout(audio_event_iface_handle_t evt, audio_event_iface_msg_t *msg);

/**
 * @brief      Discard all ongoing event message
 *
 * @param      evt   The event
 *
 * @return
 *     - ESP_OK
 *     - ESP_FAIL
 */
esp_err_t audio_event_iface_discard(audio_event_iface_handle_t evt);

/**
 * @brief      Listening and invoke callback function if there are any event are comming
 *
 * @param      evt          The event
 * @param      msg          The message
 * @param      wait_time    The wait time
 *
 * @return
 *     - ESP_OK
 *     - ESP_FAIL
 */
esp_err_t audio_event_iface_listen(audio_event_iface_handle_t evt, audio_event_iface_msg_t *msg, TickType_t wait_time);

/**
 * @brief      Get External queue handle of Emmitter
 *
 * @param[in]  evt   The external queue
 *
 * @return     External QueueHandle_t
 */
QueueHandle_t audio_event_iface_get_queue_handle(audio_event_iface_handle_t evt);

/**
 * @brief      Read the event from all the registered event emitters in the queue set of the interface
 *
 * @param[in]  evt  The event interface
 * @param[out] msg  The pointer to structure in which event is to be received
 * @param[in]  wait_time Timeout for receiving event
 *
 * @return
 *     - ESP_OK     On successful receiving of event
 *     - ESP_FAIL   In case of a timeout or invalid parameter passed
 */
esp_err_t audio_event_iface_read(audio_event_iface_handle_t evt, audio_event_iface_msg_t *msg, TickType_t wait_time);

/**
 * @brief      Get Internal queue handle of Emmitter
 *
 * @param[in]  evt   The Internal queue
 *
 * @return     Internal QueueHandle_t
 */
QueueHandle_t audio_event_iface_get_msg_queue_handle(audio_event_iface_handle_t evt);

/**
 * @brief      Add audio internal event `evt` to the listener, then we can listen `evt` event from `listen`
 *
 * @param      listener     The event can listen another event
 * @param      evt          The event to be added to
 *
 * @return
 *     - ESP_OK
 *     - ESP_FAIL
 */
esp_err_t audio_event_iface_set_msg_listener(audio_event_iface_handle_t evt, audio_event_iface_handle_t listener);
#ifdef __cplusplus
}
#endif

#endif //end of file _AUDIO_EVENT_IFACE_H_
