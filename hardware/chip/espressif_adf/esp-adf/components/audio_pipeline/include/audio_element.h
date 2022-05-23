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

#ifndef _AUDIO_ELEMENT_H_
#define _AUDIO_ELEMENT_H_

#include "esp_err.h"
#include "audio_event_iface.h"
#include "ringbuf.h"
#include "audio_common.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    AEL_IO_OK           = ESP_OK,
    AEL_IO_FAIL         = ESP_FAIL,
    AEL_IO_DONE         = -2,
    AEL_IO_ABORT        = -3,
    AEL_IO_TIMEOUT      = -4,
    AEL_PROCESS_FAIL    = -5,
} audio_element_err_t;

/**
 * @brief Audio element state
 */
typedef enum {
    AEL_STATE_NONE          = 0,
    AEL_STATE_INIT          = 1,
    AEL_STATE_INITIALIZING  = 2,
    AEL_STATE_RUNNING       = 3,
    AEL_STATE_PAUSED        = 4,
    AEL_STATE_STOPPED       = 5,
    AEL_STATE_FINISHED      = 6,
    AEL_STATE_ERROR         = 7
} audio_element_state_t;

/**
 * Audio element action command, process on dispatcher
 */
typedef enum {
    AEL_MSG_CMD_NONE                = 0,
    // AEL_MSG_CMD_ERROR               = 1,
    AEL_MSG_CMD_FINISH              = 2,
    AEL_MSG_CMD_STOP                = 3,
    AEL_MSG_CMD_PAUSE               = 4,
    AEL_MSG_CMD_RESUME              = 5,
    AEL_MSG_CMD_DESTROY             = 6,
    // AEL_MSG_CMD_CHANGE_STATE        = 7,
    AEL_MSG_CMD_REPORT_STATUS       = 8,
    AEL_MSG_CMD_REPORT_MUSIC_INFO   = 9,
    AEL_MSG_CMD_REPORT_CODEC_FMT    = 10,
    AEL_MSG_CMD_REPORT_POSITION     = 11,
} audio_element_msg_cmd_t;

/**
 * Audio element status report
 */
typedef enum {
    AEL_STATUS_NONE                     = 0,
    AEL_STATUS_ERROR_OPEN               = 1,
    AEL_STATUS_ERROR_INPUT              = 2,
    AEL_STATUS_ERROR_PROCESS            = 3,
    AEL_STATUS_ERROR_OUTPUT             = 4,
    AEL_STATUS_ERROR_CLOSE              = 5,
    AEL_STATUS_ERROR_TIMEOUT            = 6,
    AEL_STATUS_ERROR_UNKNOWN            = 7,
    AEL_STATUS_INPUT_DONE               = 8,
    AEL_STATUS_INPUT_BUFFERING          = 9,
    AEL_STATUS_OUTPUT_DONE              = 10,
    AEL_STATUS_OUTPUT_BUFFERING         = 11,
    AEL_STATUS_STATE_RUNNING            = 12,
    AEL_STATUS_STATE_PAUSED             = 13,
    AEL_STATUS_STATE_STOPPED            = 14,
    AEL_STATUS_STATE_FINISHED           = 15,
    AEL_STATUS_MOUNTED                  = 16,
    AEL_STATUS_UNMOUNTED                = 17,
} audio_element_status_t;

typedef struct audio_element *audio_element_handle_t;

/**
 * @brief Audio Element user reserved data
 */
typedef struct {
    int user_data_0;     /*!< user data 0 */
    int user_data_1;     /*!< user data 1 */
    int user_data_2;     /*!< user data 2 */
    int user_data_3;     /*!< user data 3 */
    int user_data_4;     /*!< user data 4 */
} audio_element_reserve_data_t;

/**
 * @brief Audio Element informations
 */
typedef struct {
    int sample_rates;                           /*!< Sample rates in Hz */
    int channels;                               /*!< Number of audio channel, mono is 1, stereo is 2 */
    int bits;                                   /*!< Bit wide (8, 16, 24, 32 bits) */
    int bps;                                    /*!< Bit per second */
    int64_t byte_pos;                           /*!< The current position (in bytes) being processed for an element */
    int64_t total_bytes;                        /*!< The total bytes for an element */
    int duration;                               /*!< The duration for an element (optional) */
    char *uri;                                  /*!< URI (optional) */
    esp_codec_type_t codec_fmt;                 /*!< Music format (optional) */
    audio_element_reserve_data_t reserve_data;  /*!< This value is reserved for user use (optional) */
} audio_element_info_t;

#define AUDIO_ELEMENT_INFO_DEFAULT()    { \
    .sample_rates = 44100,                \
    .channels = 2,                        \
    .bits = 16,                           \
    .bps = 0,                             \
    .byte_pos = 0,                        \
    .total_bytes = 0,                     \
    .duration = 0,                        \
    .uri = NULL,                          \
    .codec_fmt = ESP_CODEC_TYPE_UNKNOW    \
}

typedef esp_err_t (*el_io_func)(audio_element_handle_t self);
typedef audio_element_err_t (*process_func)(audio_element_handle_t self, char *el_buffer, int el_buf_len);
typedef audio_element_err_t (*stream_func)(audio_element_handle_t self, char *buffer, int len, TickType_t ticks_to_wait,
        void *context);
typedef esp_err_t (*event_cb_func)(audio_element_handle_t el, audio_event_iface_msg_t *event, void *ctx);
typedef esp_err_t (*ctrl_func)(audio_element_handle_t self, void *in_data, int in_size, void *out_data, int *out_size);

/**
 * @brief Audio Element configurations.
 *        Each Element at startup will be a self-running task.
 *        These tasks will execute the callback open -> [loop: read -> process -> write] -> close.
 *        These callback functions are provided by the user corresponding to this configuration.
 *
 */
typedef struct {
    el_io_func          open;             /*!< Open callback function */
    ctrl_func           seek;             /*!< Seek callback function */
    process_func        process;          /*!< Process callback function */
    el_io_func          close;            /*!< Close callback function */
    el_io_func          destroy;          /*!< Destroy callback function */
    stream_func         read;             /*!< Read callback function */
    stream_func         write;            /*!< Write callback function */
    int                 buffer_len;       /*!< Buffer length use for an Element */
    int                 task_stack;       /*!< Element task stack */
    int                 task_prio;        /*!< Element task priority (based on freeRTOS priority) */
    int                 task_core;        /*!< Element task running in core (0 or 1) */
    int                 out_rb_size;      /*!< Output ringbuffer size */
    void                *data;            /*!< User context */
    const char          *tag;             /*!< Element tag */
    bool                stack_in_ext;     /*!< Try to allocate stack in external memory */
    int                 multi_in_rb_num;  /*!< The number of multiple input ringbuffer */
    int                 multi_out_rb_num; /*!< The number of multiple output ringbuffer */
} audio_element_cfg_t;

#define DEFAULT_ELEMENT_RINGBUF_SIZE    (8*1024)
#define DEFAULT_ELEMENT_BUFFER_LENGTH   (1024)
#define DEFAULT_ELEMENT_STACK_SIZE      (2*1024)
#define DEFAULT_ELEMENT_TASK_PRIO       (5)
#define DEFAULT_ELEMENT_TASK_CORE       (0)

#define DEFAULT_AUDIO_ELEMENT_CONFIG() {                \
    .buffer_len         = DEFAULT_ELEMENT_BUFFER_LENGTH,\
    .task_stack         = DEFAULT_ELEMENT_STACK_SIZE,   \
    .task_prio          = DEFAULT_ELEMENT_TASK_PRIO,    \
    .task_core          = DEFAULT_ELEMENT_TASK_CORE,    \
    .multi_in_rb_num    = 0,                            \
    .multi_out_rb_num   = 0,                            \
}

/**
 * @brief      Initialize audio element with config.
 *
 * @param      config  The configuration
 *
 * @return
 *     - audio_elemenent handle object
 *     - NULL
 */
audio_element_handle_t audio_element_init(audio_element_cfg_t *config);

/**
 * @brief      Destroy audio element handle object, stop, clear, deletel all.
 *
 * @param[in]  el    The audio element handle
 *
 * @return
 *     - ESP_OK
 *     - ESP_FAIL
 */
esp_err_t audio_element_deinit(audio_element_handle_t el);

/**
 * @brief      Set context data to element handle object.
 *             It can be retrieved by calling `audio_element_getdata`.
 *
 * @param[in]  el    The audio element handle
 * @param      data  The data pointer
 *
 * @return
 *     - ESP_OK
 *     - ESP_FAIL
 */
esp_err_t audio_element_setdata(audio_element_handle_t el, void *data);

/**
 * @brief      Get context data from element handle object.
 *
 * @param[in]  el    The audio element handle
 *
 * @return     data pointer
 */
void *audio_element_getdata(audio_element_handle_t el);

/**
 * @brief      Set elemenet tag name, or clear if tag = NULL.
 *
 * @param[in]  el    The audio element handle
 * @param[in]  tag   The tag name pointer
 *
 * @return
 *     - ESP_OK
 *     - ESP_FAIL
 */
esp_err_t audio_element_set_tag(audio_element_handle_t el, const char *tag);

/**
 * @brief      Get element tag name.
 *
 * @param[in]  el    The audio element handle
 *
 * @return     Element tag name pointer
 */
char *audio_element_get_tag(audio_element_handle_t el);

/**
 * @brief      Set audio element infomation.
 *
 * @param[in]  el    The audio element handle
 * @param      info  The information pointer
 *
 * @return
 *     - ESP_OK
 *     - ESP_FAIL
 */
esp_err_t audio_element_setinfo(audio_element_handle_t el, audio_element_info_t *info);

/**
 * @brief      Get audio element infomation.
 *
 * @param[in]  el    The audio element handle
 * @param      info  The information pointer
 *
 * @return
 *     - ESP_OK
 *     - ESP_FAIL
 */
esp_err_t audio_element_getinfo(audio_element_handle_t el, audio_element_info_t *info);

/**
 * @brief      Set audio element URI.
 *
 * @param[in]  el    The audio element handle
 * @param[in]  uri   The uri pointer
 *
 * @return
 *     - ESP_OK
 *     - ESP_FAIL
 */
esp_err_t audio_element_set_uri(audio_element_handle_t el, const char *uri);

/**
 * @brief      Get audio element URI.
 *
 * @param[in]  el    The audio element handle
 *
 * @return     URI pointer
 */
char *audio_element_get_uri(audio_element_handle_t el);

/**
 * @brief      Start Audio Element.
 *             With this function, audio_element will start as freeRTOS task,
 *             and put the task into 'PAUSED' state.
 *             Note: Element does not actually start when this function returns
 *
 * @param[in]  el    The audio element handle
 *
 * @return
 *     - ESP_OK
 *     - ESP_FAIL
 */
esp_err_t audio_element_run(audio_element_handle_t el);

/**
 * @brief      Terminate Audio Element.
 *             With this function, audio_element will exit the task function.
 *             Note: this API only sends request. It does not actually terminate immediately when this function returns.
 *
 * @param[in]  el    The audio element handle
 *
 * @return
 *     - ESP_OK
 *     - ESP_FAIL
 */
esp_err_t audio_element_terminate(audio_element_handle_t el);

/**
 * @brief      Terminate Audio Element with specific ticks for timeout.
 *             With this function, audio_element will exit the task function.
 *             Note: this API only sends request. It does not actually terminate immediately when this function returns.
 *
 * @param[in]  el               The audio element handle
 * @param[in]  ticks_to_wait    The maximum amount of time to blocking
 * @return
 *     - ESP_OK
 *     - ESP_FAIL
 */
esp_err_t audio_element_terminate_with_ticks(audio_element_handle_t el, TickType_t ticks_to_wait);

/**
 * @brief      Request stop of the Audio Element.
 *             After receiving the stop request, the element will ignore the actions being performed
 *             (read/write, wait for the ringbuffer ...) and close the task, reset the state variables.
 *             Note: this API only sends requests, Element does not actually stop when this function returns
 *
 * @param[in]  el    The audio element handle
 *
 * @return
 *     - ESP_OK
 *     - ESP_FAIL
 */
esp_err_t audio_element_stop(audio_element_handle_t el);

/**
 * @brief      After the `audio_element_stop` function is called, the Element task will perform some abort procedures.
 *             This function will be blocked (Time is DEFAULT_MAX_WAIT_TIME) until Element Task has done and exit.
 *
 * @param[in]  el    The audio element handle
 *
 * @return
 *     - ESP_OK, Success
 *     - ESP_FAIL, The state is not AEL_STATE_RUNNING
 *     - ESP_ERR_TIMEOUT, Timeout
 */
esp_err_t audio_element_wait_for_stop(audio_element_handle_t el);

/**
 * @brief      After the `audio_element_stop` function is called, the Element task will perform some abort procedures.
 *             The maximum amount of time should block waiting for Element task has stopped.
 *
 * @param[in]  el               The audio element handle
 * @param[in]  ticks_to_wait    The maximum amount of time to wait for stop
 *
 * @return
 *     - ESP_OK, Success
 *     - ESP_FAIL, The state is not AEL_STATE_RUNNING
 *     - ESP_ERR_TIMEOUT, Timeout
 */
esp_err_t audio_element_wait_for_stop_ms(audio_element_handle_t el, TickType_t ticks_to_wait);

/**
 * @brief      Request audio Element enter 'PAUSE' state.
 *             In this state, the task will wait for any event
 *
 * @param[in]  el    The audio element handle
 *
 * @return
 *     - ESP_OK
 *     - ESP_FAIL
 */
esp_err_t audio_element_pause(audio_element_handle_t el);

/**
 * @brief      Request audio Element enter 'RUNNING' state.
 *             In this state, the task listens to events and invokes the callback functions.
 *             At the same time it will wait until the size/total_size of the output ringbuffer is greater than or equal to `wait_for_rb_threshold`.
 *             If the timeout period has been exceeded and ringbuffer output has not yet reached `wait_for_rb_threshold` then the function will return.
 *
 * @param[in]  el                     The audio element handle
 * @param[in]  wait_for_rb_threshold  The wait for rb threshold (0 .. 1)
 * @param[in]  timeout                The timeout
 *
 * @return
 *     - ESP_OK
 *     - ESP_FAIL
 */
esp_err_t audio_element_resume(audio_element_handle_t el, float wait_for_rb_threshold, TickType_t timeout);

/**
 * @brief      This function will add a `listener` to listen to all events from audio element `el`.
 *             Any event from el->external_event will be send to the `listener`.
 *
 * @param      el           The audio element handle
 * @param      listener     The event will be listen to
 *
 * @return
 *     - ESP_OK
 *     - ESP_FAIL
 */
esp_err_t audio_element_msg_set_listener(audio_element_handle_t el, audio_event_iface_handle_t listener);

/**
 * @brief      This function will add a `callback` to be called from audio element `el`.
 *             Any event to caller will cause to call callback function.
 *
 * @param      el           The audio element handle
 * @param      cb_func      The callback function
 * @param      ctx          Caller context
 *
 * @return
 *     - ESP_OK
 *     - ESP_FAIL
 */
esp_err_t audio_element_set_event_callback(audio_element_handle_t el, event_cb_func cb_func, void *ctx);

/**
 * @brief      Remove listener out of el.
 *             No new events will be sent to the listener.
 *
 * @param[in]  el        The audio element handle
 * @param      listener  The listener
 *
 * @return
 *     - ESP_OK
 *     - ESP_FAIL
 */
esp_err_t audio_element_msg_remove_listener(audio_element_handle_t el, audio_event_iface_handle_t listener);

/**
 * @brief      Set Element input ringbuffer
 *
 * @param[in]  el    The audio element handle
 * @param[in]  rb    The ringbuffer handle
 *
 * @return
 *     - ESP_OK
 *     - ESP_FAIL
 */
esp_err_t audio_element_set_input_ringbuf(audio_element_handle_t el, ringbuf_handle_t rb);

/**
 * @brief      Get Element input ringbuffer.
 *
 * @param[in]  el    The audio element handle
 *
 * @return     ringbuf_handle_t
 */
ringbuf_handle_t audio_element_get_input_ringbuf(audio_element_handle_t el);

/**
 * @brief      Set Element output ringbuffer.
 *
 * @param[in]  el    The audio element handle
 * @param[in]  rb    The ringbuffer handle
 *
 * @return
 *     - ESP_OK
 *     - ESP_FAIL
 */
esp_err_t audio_element_set_output_ringbuf(audio_element_handle_t el, ringbuf_handle_t rb);

/**
 * @brief      Get Element output ringbuffer.
 *
 * @param[in]  el    The audio element handle
 *
 * @return     ringbuf_handle_t
 */
ringbuf_handle_t audio_element_get_output_ringbuf(audio_element_handle_t el);

/**
 * @brief      Get current Element state.
 *
 * @param[in]  el    The audio element handle
 *
 * @return     audio_element_state_t
 */
audio_element_state_t audio_element_get_state(audio_element_handle_t el);

/**
 * @brief      If the element is requesting data from the input ringbuffer, this function forces it to abort.
 *
 * @param[in]  el    The audio element handle
 *
 * @return
 *     - ESP_OK
 *     - ESP_FAIL
 */
esp_err_t audio_element_abort_input_ringbuf(audio_element_handle_t el);

/**
 * @brief      If the element is waiting to write data to the ringbuffer output, this function forces it to abort.
 *
 * @param[in]  el   The audio element handle
 *
 * @return
 *     - ESP_OK
 *     - ESP_FAIL
 */
esp_err_t audio_element_abort_output_ringbuf(audio_element_handle_t el);

/**
 * @brief      This function will wait until the sizeof the output ringbuffer is greater than or equal to `size_expect`.
 *             If the timeout period has been exceeded and ringbuffer output has not yet reached `size_expect`
 *             then the function will return `ESP_FAIL`
 *
 * @param[in]  el           The audio element handle
 * @param[in]  size_expect  The size expect
 * @param[in]  timeout      The timeout
 *
 * @return
 *     - ESP_OK
 *     - ESP_FAIL
 */
esp_err_t audio_element_wait_for_buffer(audio_element_handle_t el, int size_expect, TickType_t timeout);

/**
 * @brief      Element will sendout event (status) to event by this function.
 *
 * @param[in]  el      The audio element handle
 * @param[in]  status  The status
 *
 * @return
 *     - ESP_OK
 *     - ESP_FAIL
 */
esp_err_t audio_element_report_status(audio_element_handle_t el, audio_element_status_t status);

/**
 * @brief      Element will sendout event (information) to event by this function.
 *
 * @param[in]  el    The audio element handle
 *
 * @return
 *     - ESP_OK
 *     - ESP_FAIL
 */
esp_err_t audio_element_report_info(audio_element_handle_t el);

/**
 * @brief      Element will sendout event (codec format) to event by this function.
 *
 * @param[in]  el    The audio element handle
 *
 * @return
 *     - ESP_OK
 *     - ESP_FAIL
 */
esp_err_t audio_element_report_codec_fmt(audio_element_handle_t el);

/**
 * @brief      Element will sendout event with a duplicate information by this function.
 *
 * @param[in]  el    The audio element handle
 *
 * @return
 *     - ESP_OK
 *     - ESP_FAIL
 *     - ESP_ERR_NO_MEM
 */
esp_err_t audio_element_report_pos(audio_element_handle_t el);

/**
 * @brief      Set input read timeout (default is `portMAX_DELAY`).
 *
 * @param[in]  el       The audio element handle
 * @param[in]  timeout  The timeout
 *
 * @return
 *     - ESP_OK
 *     - ESP_FAIL
 */
esp_err_t audio_element_set_input_timeout(audio_element_handle_t el, TickType_t timeout);

/**
 * @brief      Set output read timeout (default is `portMAX_DELAY`).
 *
 * @param[in]  el       The audio element handle
 * @param[in]  timeout  The timeout
 *
 * @return
 *     - ESP_OK
 *     - ESP_FAIL
 */
esp_err_t audio_element_set_output_timeout(audio_element_handle_t el, TickType_t timeout);

/**
 * @brief      Reset inputbuffer.
 *
 * @param[in]  el    The audio element handle
 *
 * @return
 *     - ESP_OK
 *     - ESP_FAIL
 */
esp_err_t audio_element_reset_input_ringbuf(audio_element_handle_t el);

/**
 * @brief      Set element finish state
 *
 * @param[in]  el    The audio element handle
 *
 * @return
 *     - ESP_OK
 *     - ESP_FAIL
 */
esp_err_t audio_element_finish_state(audio_element_handle_t el);

/**
 * @brief      Change element running state with specific command.
 *
 * @param[in]  el    The audio element handle
 * @param[in]  cmd   Specific command from audio_element_msg_cmd_t
 *
 * @return
 *     - ESP_OK
 *     - ESP_FAIL
 *     - ESP_ERR_INVALID_ARG Element handle is null
 */
esp_err_t audio_element_change_cmd(audio_element_handle_t el, audio_element_msg_cmd_t cmd);

/**
 * @brief      Reset outputbuffer.
 *
 * @param[in]  el    The audio element handle
 *
 * @return
 *     - ESP_OK
 *     - ESP_FAIL
 */
esp_err_t audio_element_reset_output_ringbuf(audio_element_handle_t el);

/**
 * @brief      Call this function to provide Element input data.
 *             Depending on setup using ringbuffer or function callback, Element invokes read ringbuffer, or calls read callback funtion.
 *
 * @param[in]  el            The audio element handle
 * @param      buffer        The buffer pointer
 * @param[in]  wanted_size   The wanted size
 *
 * @return
 *        - > 0 number of bytes produced
 *        - <=0 audio_element_err_t
 */
audio_element_err_t audio_element_input(audio_element_handle_t el, char *buffer, int wanted_size);

/**
 * @brief      Call this function to sendout Element output data.
 *             Depending on setup using ringbuffer or function callback, Element will invoke write to ringbuffer, or call write callback funtion.
 *
 * @param[in]  el          The audio element handle
 * @param      buffer      The buffer pointer
 * @param[in]  write_size  The write size
 *
 * @return
 *        - > 0 number of bytes written
 *        - <=0 audio_element_err_t
 */
audio_element_err_t audio_element_output(audio_element_handle_t el, char *buffer, int write_size);

/**
 * @brief     This API allows the application to set a read callback for the first audio_element in the pipeline for
 *            allowing the pipeline to interface with other systems. The callback is invoked every time the audio
 *            element requires data to be processed.
 *
 * @param[in]  el        The audio element handle
 * @param[in]  fn        Callback read function. The callback function should return number of bytes read or -1
 *                       in case of error in reading. Note that the callback function may decide to block and
 *                       that may block the entire pipeline.
 * @param[in]  context   An optional context which will be passed to callback function on every invocation
 *
 * @return
 *     - ESP_OK
 *     - ESP_FAIL
 */
esp_err_t audio_element_set_read_cb(audio_element_handle_t el, stream_func fn, void *context);

/**
 * @brief     This API allows the application to set a write callback for the last audio_element in the pipeline for
 *            allowing the pipeline to interface with other systems.
 *            The callback is invoked every time the audio element has a processed data that needs to be passed forward.
 *
 * @param[in]  el        The audio element
 * @param[in]  fn        Callback write function
 *                       The callback function should return number of bytes written or -1 in case of error in writing.
 *                       Note that the callback function may decide to block and that may block the entire pipeline.
 * @param[in]  context   An optional context which will be passed to callback function on every invocation
 *
 * @return
 *     - ESP_OK
 *     - ESP_FAIL
 */
esp_err_t audio_element_set_write_cb(audio_element_handle_t el, stream_func fn, void *context);

/**
 * @brief     Get callback write function that register to the element
 *
 * @param[in]  el        The audio element
 *
 * @return
 *     - Callback write function pointer
 *     - NULL     Failed
 */
stream_func audio_element_get_write_cb(audio_element_handle_t el);

/**
 * @brief     Get callback read function that register to the element
 *
 * @param[in]  el        The audio element
 *
 * @return
 *     - Callback read function pointer
 *     - NULL     Failed
 */
stream_func audio_element_get_read_cb(audio_element_handle_t el);

/**
 * @brief      Get External queue of Emitter.
 *             We can read any event that has been send out of Element from this `QueueHandle_t`.
 *
 * @param[in]  el    The audio element handle
 *
 * @return     QueueHandle_t
 */
QueueHandle_t audio_element_get_event_queue(audio_element_handle_t el);

/**
 * @brief      Set inputbuffer and outputbuffer have finished.
 *
 * @param[in]  el    The audio element handle
 *
 * @return
 *     - ESP_OK
 *     - ESP_FAIL
 */
esp_err_t audio_element_set_ringbuf_done(audio_element_handle_t el);

/**
 * @brief      Enforce 'AEL_STATE_INIT' state.
 *
 * @param[in]  el    The audio element handle
 *
 * @return
 *     - ESP_OK
 *     - ESP_FAIL
 */
esp_err_t audio_element_reset_state(audio_element_handle_t el);

/**
 * @brief      Get Element output ringbuffer size.
 *
 * @param[in]  el    The audio element handle
 *
 * @return
 *     - =0: Parameter NULL
 *     - >0: Size of ringbuffer
 */
int audio_element_get_output_ringbuf_size(audio_element_handle_t el);

/**
 * @brief      Set Element output ringbuffer size.
 *
 * @param[in]  el       The audio element handle
 * @param[in]  rb_size  Size of the ringbuffer
 *
 * @return
 *     - ESP_OK
 *     - ESP_FAIL
 */
esp_err_t audio_element_set_output_ringbuf_size(audio_element_handle_t el, int rb_size);

/**
 * @brief      Call this function to read data from multi input ringbuffer by given index.
 *
 * @param      el            The audio element handle
 * @param      buffer        The buffer pointer
 * @param      wanted_size   The wanted size
 * @param      index         The index of multi input ringbuffer, start from `0`, should be less than `NUMBER_OF_MULTI_RINGBUF`
 * @param      ticks_to_wait Timeout of ringbuffer
 *
 * @return
 *     - ESP_OK
 *     - ESP_ERR_INVALID_ARG
 */
esp_err_t audio_element_multi_input(audio_element_handle_t el, char *buffer, int wanted_size, int index, TickType_t ticks_to_wait);

/**
 * @brief      Call this function write data by multi output ringbuffer.
 *
 * @param[in]  el            The audio element handle
 * @param      buffer        The buffer pointer
 * @param[in]  wanted_size   The wanted size
 * @param      ticks_to_wait Timeout of ringbuffer
 *
 * @return
 *     - ESP_OK
 *     - ESP_FAIL
 */
esp_err_t audio_element_multi_output(audio_element_handle_t el, char *buffer, int wanted_size, TickType_t ticks_to_wait);

/**
 * @brief      Set multi input ringbuffer Element.
 *
 * @param[in]  el    The audio element handle
 * @param[in]  rb    The ringbuffer handle
 * @param[in]  index Index of multi ringbuffer, starts from `0`, should be less than `NUMBER_OF_MULTI_RINGBUF`
 *
 * @return
 *     - ESP_OK
 *     - ESP_FAIL
 */
esp_err_t audio_element_set_multi_input_ringbuf(audio_element_handle_t el, ringbuf_handle_t rb, int index);

/**
 * @brief      Set multi output ringbuffer Element.
 *
 * @param[in]  el    The audio element handle
 * @param[in]  rb    The ringbuffer handle
 * @param[in]  index Index of multi ringbuffer, starts from `0`, should be less than `NUMBER_OF_MULTI_RINGBUF`
 *
 * @return
 *     - ESP_OK
 *     - ESP_ERR_INVALID_ARG
 */
esp_err_t audio_element_set_multi_output_ringbuf(audio_element_handle_t el, ringbuf_handle_t rb, int index);

/**
 * @brief      Get handle of multi input ringbuffer Element by index.
 *
 * @param[in]  el    The audio element handle
 * @param[in]  index Index of multi ringbuffer, starts from `0`, should be less than `NUMBER_OF_MULTI_RINGBUF`
 *
 * @return
 *     - NULL   Error
 *     - Others ringbuf_handle_t
 */
ringbuf_handle_t audio_element_get_multi_input_ringbuf(audio_element_handle_t el, int index);

/**
 * @brief      Get handle of multi output ringbuffer Element by index.
 *
 * @param[in]  el    The audio element handle
 * @param[in]  index Index of multi ringbuffer, starts from `0`, should be less than `NUMBER_OF_MULTI_RINGBUF`
 *
 * @return
 *     - NULL   Error
 *     - Others ringbuf_handle_t
 */
ringbuf_handle_t audio_element_get_multi_output_ringbuf(audio_element_handle_t el, int index);

/**
 * @brief      Provides a way to call element's `open`
 *
 * @param[in]  el    The audio element handle
 *
 * @return
 *     - ESP_OK
 *     - ESP_FAIL
 */
esp_err_t audio_element_process_init(audio_element_handle_t el);

/**
 * @brief      Provides a way to call element's `close`
 *
 * @param[in]  el    The audio element handle
 *
 * @return
 *     - ESP_OK
 *     - ESP_FAIL
 */
esp_err_t audio_element_process_deinit(audio_element_handle_t el);

/**
 * @brief      Call element's `seek`
 *
 * @param[in]  el           The audio element handle
 * @param[in]  in_data      A pointer to in data
 * @param[in]  in_size      The size of the `in_data`
 * @param[out] out_data     A pointer to the out data
 * @param[out] out_size     The size of the `out_data`
 *
 * @return
 *     - ESP_OK
 *     - ESP_FAIL
 *     - ESP_ERR_NOT_SUPPORTED
 */
esp_err_t audio_element_seek(audio_element_handle_t el, void *in_data, int in_size, void *out_data, int *out_size);

/**
 * @brief      Get Element stopping flag
 *
 * @param[in]  el    The audio element handle
 *
 * @return     element's stopping flag
 */
bool audio_element_is_stopping(audio_element_handle_t el);

/**
 * @brief      Update the byte position of element information
 *
 * @param[in]  el    The audio element handle
 * @param[in]  pos   The byte_pos accumulated by this value
 *
 * @return
 *     - ESP_OK
 *     - ESP_FAIL
 */
esp_err_t audio_element_update_byte_pos(audio_element_handle_t el, int pos);

/**
 * @brief      Set the byte position of element information
 *
 * @param[in]  el    The audio element handle
 * @param[in]  pos   This value is assigned to byte_pos
 *
 * @return
 *     - ESP_OK
 *     - ESP_FAIL
 */
esp_err_t audio_element_set_byte_pos(audio_element_handle_t el, int pos);

/**
 * @brief      Update the total bytes of element information
 *
 * @param[in]  el               The audio element handle
 * @param[in]  total_bytes      The total_bytes accumulated by this value
 *
 * @return
 *     - ESP_OK
 *     - ESP_FAIL
 */
esp_err_t audio_element_update_total_bytes(audio_element_handle_t el, int total_bytes);

/**
 * @brief      Set the total bytes of element information
 *
 * @param[in]  el               The audio element handle
 * @param[in]  total_bytes      This value is assigned to total_bytes
 *
 * @return
 *     - ESP_OK
 *     - ESP_FAIL
 */
esp_err_t audio_element_set_total_bytes(audio_element_handle_t el, int total_bytes);

/**
 * @brief      Set the bps of element information
 *
 * @param[in]  el           The audio element handle
 * @param[in]  bit_rate     This value is assigned to bps
 *
 * @return
 *     - ESP_OK
 *     - ESP_FAIL
 */
esp_err_t audio_element_set_bps(audio_element_handle_t el, int bit_rate);

/**
 * @brief      Set the codec format of element information
 *
 * @param[in]  el       The audio element handle
 * @param[in]  format   This value is assigned to codec_fmt
 *
 * @return
 *     - ESP_OK
 *     - ESP_FAIL
 */
esp_err_t audio_element_set_codec_fmt(audio_element_handle_t el, int format);

/**
 * @brief      Set the sample_rate, channels, bits of element information
 *
 * @param[in]  el             The audio element handle
 * @param[in]  sample_rates   Sample_rates of music information
 * @param[in]  channels       Channels of music information
 * @param[in]  bits           Bits of music information
 *
 * @return
 *     - ESP_OK
 *     - ESP_FAIL
 */
esp_err_t audio_element_set_music_info(audio_element_handle_t el, int sample_rates, int channels, int bits);

/**
 * @brief      Set the duration of element information
 *
 * @param[in]  el           The audio element handle
 * @param[in]  duration     This value is assigned to duration
 *
 * @return
 *     - ESP_OK
 *     - ESP_FAIL
 */
esp_err_t audio_element_set_duration(audio_element_handle_t el, int duration);

/**
 * @brief      Set the user_data_0 of element information
 *
 * @param[in]  el               The audio element handle
 * @param[in]  user_data0       This value is assigned to user_data_0
 *
 * @return
 *     - ESP_OK
 *     - ESP_FAIL
 */
esp_err_t audio_element_set_reserve_user0(audio_element_handle_t el, int user_data0);

/**
 * @brief      Set the user_data_1 of element information
 *
 * @param[in]  el               The audio element handle
 * @param[in]  user_data1       This value is assigned to user_data_1
 *
 * @return
 *     - ESP_OK
 *     - ESP_FAIL
 */
esp_err_t audio_element_set_reserve_user1(audio_element_handle_t el, int user_data1);

/**
 * @brief      Set the user_data_2 of element information
 *
 * @param[in]  el               The audio element handle
 * @param[in]  user_data2       This value is assigned to user_data_2
 *
 * @return
 *     - ESP_OK
 *     - ESP_FAIL
 */
esp_err_t audio_element_set_reserve_user2(audio_element_handle_t el, int user_data2);

/**
 * @brief      Set the user_data_3 of element information
 *
 * @param[in]  el               The audio element handle
 * @param[in]  user_data3       This value is assigned to user_data_3
 *
 * @return
 *     - ESP_OK
 *     - ESP_FAIL
 */
esp_err_t audio_element_set_reserve_user3(audio_element_handle_t el, int user_data3);

/**
 * @brief      Set the user_data_4 of element information
 *
 * @param[in]  el               The audio element handle
 * @param[in]  user_data4       This value is assigned to user_data_4
 *
 * @return
 *     - ESP_OK
 *     - ESP_FAIL
 */
esp_err_t audio_element_set_reserve_user4(audio_element_handle_t el, int user_data4);


#ifdef __cplusplus
}
#endif

#endif
