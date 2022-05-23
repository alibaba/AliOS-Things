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

#include <stdlib.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/event_groups.h"

#include "esp_log.h"
#include "audio_element.h"
#include "audio_mem.h"
#include "audio_mutex.h"
#include "audio_error.h"
#include "audio_thread.h"

static const char *TAG = "AUDIO_ELEMENT";
#define DEFAULT_MAX_WAIT_TIME       (2000/portTICK_RATE_MS)

/**
 *  I/O Element Abstract
 */
typedef struct io_callback {
    stream_func                 cb;
    void                        *ctx;
} io_callback_t;

/**
 *  Audio Callback Abstract
 */
typedef struct audio_callback {
    event_cb_func               cb;
    void                        *ctx;
} audio_callback_t;

typedef struct audio_multi_rb {
    ringbuf_handle_t            *rb;
    int                         max_rb_num;
} audio_multi_rb_t;

typedef enum {
    IO_TYPE_RB = 1, /* I/O through ringbuffer */
    IO_TYPE_CB,     /* I/O through callback */
} io_type_t;

typedef enum {
    EVENTS_TYPE_Q = 1,  /* Events through MessageQueue */
    EVENTS_TYPE_CB,     /* Events through Callback function */
} events_type_t;

struct audio_element {
    /* Functions/RingBuffers */
    el_io_func                  open;
    ctrl_func                   seek;
    process_func                process;
    el_io_func                  close;
    el_io_func                  destroy;
    io_type_t                   read_type;
    union {
        ringbuf_handle_t        input_rb;
        io_callback_t           read_cb;
    } in;
    io_type_t                   write_type;
    union {
        ringbuf_handle_t        output_rb;
        io_callback_t           write_cb;
    } out;

    audio_multi_rb_t            multi_in;
    audio_multi_rb_t            multi_out;

    /* Properties */
    volatile bool               is_open;
    audio_element_state_t       state;

    events_type_t               events_type;
    audio_event_iface_handle_t  iface_event;
    audio_callback_t            callback_event;

    int                         buf_size;
    char                        *buf;

    char                        *tag;
    int                         task_stack;
    int                         task_prio;
    int                         task_core;
    xSemaphoreHandle            lock;
    audio_element_info_t        info;
    audio_element_info_t        *report_info;

    bool                        stack_in_ext;
    audio_thread_t              audio_thread;

    /* PrivateData */
    void                        *data;
    EventGroupHandle_t          state_event;
    int                         input_wait_time;
    int                         output_wait_time;
    int                         out_buf_size_expect;
    int                         out_rb_size;
    volatile bool               is_running;
    volatile bool               task_run;
    volatile bool               stopping;
};

const static int STOPPED_BIT = BIT0;
const static int STARTED_BIT = BIT1;
const static int BUFFER_REACH_LEVEL_BIT = BIT2;
const static int TASK_CREATED_BIT = BIT3;
const static int TASK_DESTROYED_BIT = BIT4;
const static int PAUSED_BIT = BIT5;
const static int RESUMED_BIT = BIT6;

static esp_err_t audio_element_on_cmd_error(audio_element_handle_t el);
static esp_err_t audio_element_on_cmd_stop(audio_element_handle_t el);

static esp_err_t audio_element_force_set_state(audio_element_handle_t el, audio_element_state_t new_state)
{
    el->state = new_state;
    return ESP_OK;
}

static esp_err_t audio_element_cmd_send(audio_element_handle_t el, audio_element_msg_cmd_t cmd)
{
    audio_event_iface_msg_t msg = {
        .source = el,
        .source_type = AUDIO_ELEMENT_TYPE_ELEMENT,
        .cmd = cmd,
    };
    ESP_LOGV(TAG, "[%s]evt internal cmd = %d", el->tag, msg.cmd);
    return audio_event_iface_cmd(el->iface_event, &msg);
}

static esp_err_t audio_element_msg_sendout(audio_element_handle_t el, audio_event_iface_msg_t *msg)
{
    msg->source = el;
    msg->source_type = AUDIO_ELEMENT_TYPE_ELEMENT;
    if (el->events_type == EVENTS_TYPE_CB && el->callback_event.cb) {
        return el->callback_event.cb(el, msg, el->callback_event.ctx);
    }
    return audio_event_iface_sendout(el->iface_event, msg);
}

esp_err_t audio_element_process_init(audio_element_handle_t el)
{
    if (el->open == NULL) {
        el->is_open = true;
        xEventGroupSetBits(el->state_event, STARTED_BIT);
        return ESP_OK;
    }
    el->is_open = true;
    audio_element_force_set_state(el, AEL_STATE_INITIALIZING);
    esp_err_t ret = el->open(el);
    if (ret == ESP_OK) {
        ESP_LOGD(TAG, "[%s] el opened", el->tag);
        audio_element_force_set_state(el, AEL_STATE_RUNNING);
        audio_element_report_status(el, AEL_STATUS_STATE_RUNNING);
        xEventGroupSetBits(el->state_event, STARTED_BIT);
        return ESP_OK;
    } else if (ret == AEL_IO_DONE) {
        ESP_LOGW(TAG, "[%s] OPEN AEL_IO_DONE", el->tag);
        audio_element_force_set_state(el, AEL_STATE_RUNNING);
        audio_element_report_status(el, AEL_STATUS_STATE_RUNNING);
        return ESP_OK;
    } else if (ret == AEL_IO_ABORT) {
        ESP_LOGW(TAG, "[%s] AEL_IO_ABORT, %d", el->tag, ret);
        audio_element_on_cmd_stop(el);
    } else {
        ESP_LOGE(TAG, "[%s] AEL_STATUS_ERROR_OPEN,%d", el->tag, ret);
        audio_element_force_set_state(el, AEL_STATE_ERROR);
        audio_element_report_status(el, AEL_STATUS_ERROR_OPEN);
        audio_element_on_cmd_error(el);
    }
    return ESP_FAIL;
}

esp_err_t audio_element_process_deinit(audio_element_handle_t el)
{
    if (el->is_open && el->close) {
        ESP_LOGV(TAG, "[%s] will be closed, line %d", el->tag, __LINE__);
        el->close(el);
    }
    el->is_open = false;
    return ESP_OK;
}

static esp_err_t audio_element_on_cmd_error(audio_element_handle_t el)
{
    if (el->state != AEL_STATE_STOPPED) {
        ESP_LOGW(TAG, "[%s] audio_element_on_cmd_error,%d", el->tag, el->state);
        audio_element_process_deinit(el);
        el->state = AEL_STATE_ERROR;
        audio_event_iface_set_cmd_waiting_timeout(el->iface_event, portMAX_DELAY);
        el->is_running = false;
        xEventGroupSetBits(el->state_event, STOPPED_BIT);
    }
    return ESP_OK;
}

static esp_err_t audio_element_on_cmd_stop(audio_element_handle_t el)
{
    if ((el->state != AEL_STATE_FINISHED) && (el->state != AEL_STATE_STOPPED)) {
        audio_element_process_deinit(el);
        el->state = AEL_STATE_STOPPED;
        audio_event_iface_set_cmd_waiting_timeout(el->iface_event, portMAX_DELAY);
        audio_element_report_status(el, AEL_STATUS_STATE_STOPPED);
        el->is_running = false;
        el->stopping = false;
        ESP_LOGD(TAG, "[%s] audio_element_on_cmd_stop", el->tag);
        xEventGroupSetBits(el->state_event, STOPPED_BIT);
    } else {
        // Change element state to AEL_STATE_STOPPED, even if AEL_STATE_ERROR or AEL_STATE_FINISHED
        // Except AEL_STATE_STOPPED and is not running
        ESP_LOGD(TAG, "[%s] audio_element_on_cmd_stop, state:%d", el->tag, el->state);
        if ((el->is_running == false) && (el->state == AEL_STATE_STOPPED)) {
            el->stopping = false;
            return ESP_OK;
        }
        el->state = AEL_STATE_STOPPED;
        el->is_running = false;
        el->stopping = false;
        audio_element_report_status(el, AEL_STATUS_STATE_STOPPED);
        xEventGroupSetBits(el->state_event, STOPPED_BIT);
    }
    return ESP_OK;
}

static esp_err_t audio_element_on_cmd_finish(audio_element_handle_t el)
{
    if ((el->state == AEL_STATE_ERROR)
        || (el->state == AEL_STATE_STOPPED)) {
        ESP_LOGD(TAG, "[%s] audio_element_on_cmd_finish, state:%d", el->tag, el->state);
        return ESP_OK;
    }
    audio_element_process_deinit(el);
    el->state = AEL_STATE_FINISHED;
    audio_event_iface_set_cmd_waiting_timeout(el->iface_event, portMAX_DELAY);
    audio_element_report_status(el, AEL_STATUS_STATE_FINISHED);
    el->is_running = false;
    xEventGroupSetBits(el->state_event, STOPPED_BIT);
    ESP_LOGD(TAG, "[%s] audio_element_on_cmd_finish", el->tag);
    return ESP_OK;
}

static esp_err_t audio_element_on_cmd_resume(audio_element_handle_t el)
{
    if (el->state == AEL_STATE_RUNNING) {
        el->is_running = true;
        xEventGroupSetBits(el->state_event, RESUMED_BIT);
        return ESP_OK;
    }
    if (el->state != AEL_STATE_INIT && el->state != AEL_STATE_RUNNING && el->state != AEL_STATE_PAUSED) {
        audio_element_reset_output_ringbuf(el);
    }
    el->is_running = true;
    xEventGroupSetBits(el->state_event, RESUMED_BIT);
    if (audio_element_process_init(el) != ESP_OK) {
        audio_element_abort_output_ringbuf(el);
        audio_element_abort_input_ringbuf(el);
        el->is_running = false;
        return ESP_FAIL;
    }
    audio_event_iface_set_cmd_waiting_timeout(el->iface_event, 0);
    xEventGroupClearBits(el->state_event, STOPPED_BIT);
    return ESP_OK;
}

static esp_err_t audio_element_on_cmd(audio_event_iface_msg_t *msg, void *context)
{
    audio_element_handle_t el = (audio_element_handle_t)context;

    if (msg->source_type != AUDIO_ELEMENT_TYPE_ELEMENT) {
        ESP_LOGE(TAG, "[%s] Invalid event type, this event should be ELEMENT type", el->tag);
        return ESP_FAIL;
    }
    esp_err_t ret = ESP_OK;
    //process an event
    switch (msg->cmd) {
        case AEL_MSG_CMD_FINISH:
            ESP_LOGD(TAG, "[%s] AEL_MSG_CMD_FINISH, state:%d", el->tag, el->state);
            ret = audio_element_on_cmd_finish(el);
            break;
        case AEL_MSG_CMD_STOP:
            ESP_LOGD(TAG, "[%s] AEL_MSG_CMD_STOP, state:%d", el->tag, el->state);
            ret = audio_element_on_cmd_stop(el);
            break;
        case AEL_MSG_CMD_PAUSE:
            el->state = AEL_STATE_PAUSED;
            audio_element_process_deinit(el);
            audio_event_iface_set_cmd_waiting_timeout(el->iface_event, portMAX_DELAY);
            audio_element_report_status(el, AEL_STATUS_STATE_PAUSED);
            el->is_running = false;
            ESP_LOGI(TAG, "[%s] AEL_MSG_CMD_PAUSE", el->tag);
            xEventGroupSetBits(el->state_event, PAUSED_BIT);
            break;
        case AEL_MSG_CMD_RESUME:
            ESP_LOGI(TAG, "[%s] AEL_MSG_CMD_RESUME,state:%d", el->tag, el->state);
            ret = audio_element_on_cmd_resume(el);
            break;
        case AEL_MSG_CMD_DESTROY:
            el->is_running = false;
            ESP_LOGD(TAG, "[%s] AEL_MSG_CMD_DESTROY", el->tag);
            ret = AEL_IO_ABORT;
    }
    return ret;
}

static esp_err_t audio_element_process_running(audio_element_handle_t el)
{
    int process_len = -1;
    if (el->state < AEL_STATE_RUNNING || !el->is_running) {
        return ESP_ERR_INVALID_STATE;
    }
    process_len = el->process(el, el->buf, el->buf_size);
    if (process_len <= 0) {
        switch (process_len) {
            case AEL_IO_ABORT:
                ESP_LOGD(TAG, "[%s] ERROR_PROCESS, AEL_IO_ABORT", el->tag);
                audio_element_on_cmd_stop(el);
                break;
            case AEL_IO_DONE:
            case AEL_IO_OK:
                // Re-open if reset_state function called
                if (audio_element_get_state(el) == AEL_STATE_INIT) {
                    return audio_element_on_cmd_resume(el);
                }
                audio_element_set_ringbuf_done(el);
                audio_element_on_cmd_finish(el);
                break;
            case AEL_IO_FAIL:
                ESP_LOGE(TAG, "[%s] ERROR_PROCESS, AEL_IO_FAIL", el->tag);
                audio_element_report_status(el, AEL_STATUS_ERROR_PROCESS);
                audio_element_on_cmd_error(el);
                break;
            case AEL_IO_TIMEOUT:
                ESP_LOGD(TAG, "[%s] ERROR_PROCESS, AEL_IO_TIMEOUT", el->tag);
                break;
            case AEL_PROCESS_FAIL:
                ESP_LOGE(TAG, "[%s] ERROR_PROCESS, AEL_PROCESS_FAIL", el->tag);
                audio_element_report_status(el, AEL_STATUS_ERROR_PROCESS);
                audio_element_on_cmd_error(el);
                break;
            default:
                ESP_LOGW(TAG, "[%s] Process return error,ret:%d", el->tag, process_len);
                break;
        }
    }
    return ESP_OK;
}

audio_element_err_t audio_element_input(audio_element_handle_t el, char *buffer, int wanted_size)
{
    int in_len = 0;
    if (el->read_type == IO_TYPE_CB) {
        if (el->in.read_cb.cb == NULL) {
            ESP_LOGE(TAG, "[%s] Read IO Type callback but callback not set", el->tag);
            return ESP_FAIL;
        }
        in_len = el->in.read_cb.cb(el, buffer, wanted_size, el->input_wait_time,
                                   el->in.read_cb.ctx);
    } else if (el->read_type == IO_TYPE_RB) {
        if (el->in.input_rb == NULL) {
            ESP_LOGE(TAG, "[%s] Read IO type ringbuf but ringbuf not set", el->tag);
            return ESP_FAIL;
        }
        in_len = rb_read(el->in.input_rb, buffer, wanted_size, el->input_wait_time);
    } else {
        ESP_LOGE(TAG, "[%s] Invalid read IO type", el->tag);
        return ESP_FAIL;
    }
    if (in_len <= 0) {
        switch (in_len) {
            case AEL_IO_ABORT:
                ESP_LOGW(TAG, "IN-[%s] AEL_IO_ABORT", el->tag);
                break;
            case AEL_IO_DONE:
            case AEL_IO_OK:
                ESP_LOGI(TAG, "IN-[%s] AEL_IO_DONE,%d", el->tag, in_len);
                break;
            case AEL_IO_FAIL:
                ESP_LOGE(TAG, "IN-[%s] AEL_STATUS_ERROR_INPUT", el->tag);
                audio_element_report_status(el, AEL_STATUS_ERROR_INPUT);
                break;
            case AEL_IO_TIMEOUT:
                // ESP_LOGD(TAG, "IN-[%s] AEL_IO_TIMEOUT", el->tag);
                break;
            default:
                ESP_LOGE(TAG, "IN-[%s] Input return not support,ret:%d", el->tag, in_len);
                break;
        }
    }
    return in_len;
}

audio_element_err_t audio_element_output(audio_element_handle_t el, char *buffer, int write_size)
{
    int output_len = 0;
    if (el->write_type == IO_TYPE_CB) {
        if (el->out.write_cb.cb && write_size) {
            output_len = el->out.write_cb.cb(el, buffer, write_size, el->output_wait_time,
                                             el->out.write_cb.ctx);
        }
    } else if (el->write_type == IO_TYPE_RB) {
        if (el->out.output_rb && write_size) {
            output_len = rb_write(el->out.output_rb, buffer, write_size, el->output_wait_time);
            if ((rb_bytes_filled(el->out.output_rb) > el->out_buf_size_expect) || (output_len < 0)) {
                xEventGroupSetBits(el->state_event, BUFFER_REACH_LEVEL_BIT);
            }
        }
    }
    if (output_len <= 0) {
        switch (output_len) {
            case AEL_IO_ABORT:
                ESP_LOGW(TAG, "OUT-[%s] AEL_IO_ABORT", el->tag);
                break;
            case AEL_IO_DONE:
            case AEL_IO_OK:
                ESP_LOGI(TAG, "OUT-[%s] AEL_IO_DONE,%d", el->tag, output_len);
                break;
            case AEL_IO_FAIL:
                ESP_LOGE(TAG, "OUT-[%s] AEL_STATUS_ERROR_OUTPUT", el->tag);
                audio_element_report_status(el, AEL_STATUS_ERROR_OUTPUT);
                break;
            case AEL_IO_TIMEOUT:
                ESP_LOGW(TAG, "OUT-[%s] AEL_IO_TIMEOUT", el->tag);
                break;
            default:
                ESP_LOGE(TAG, "OUT-[%s] Output return not support,ret:%d", el->tag, output_len);
                break;
        }
    }
    return output_len;
}
void audio_element_task(void *pv)
{
    audio_element_handle_t el = (audio_element_handle_t)pv;
    el->task_run = true;
    xEventGroupSetBits(el->state_event, TASK_CREATED_BIT);
    audio_element_force_set_state(el, AEL_STATE_INIT);
    audio_event_iface_set_cmd_waiting_timeout(el->iface_event, portMAX_DELAY);
    if (el->buf_size > 0) {
        el->buf = audio_calloc(1, el->buf_size);
        AUDIO_MEM_CHECK(TAG, el->buf, {
            el->task_run = false;
            ESP_LOGE(TAG, "[%s] Error malloc element buffer", el->tag);
        });
    }
    xEventGroupClearBits(el->state_event, STOPPED_BIT);
    esp_err_t ret = ESP_OK;
    while (el->task_run) {
        if ((ret = audio_event_iface_waiting_cmd_msg(el->iface_event)) != ESP_OK) {
            xEventGroupSetBits(el->state_event, STOPPED_BIT);
            /*
             * Do not exit task when audio_element_process_init failure to
             * make call audio_element_deinit safety.
            */
            if (ret == AEL_IO_ABORT) {
                break;
            }
        }
        if (audio_element_process_running(el) != ESP_OK) {
            // continue;
        }
    }

    if (el->is_open && el->close) {
        ESP_LOGD(TAG, "[%s-%p] el closed", el->tag, el);
        el->close(el);
        audio_element_force_set_state(el, AEL_STATE_STOPPED);
    }
    el->is_open = false;
    audio_free(el->buf);
    el->buf = NULL;
    el->stopping = false;
    el->task_run = false;
    ESP_LOGD(TAG, "[%s-%p] el task deleted,%d", el->tag, el, uxTaskGetStackHighWaterMark(NULL));
    xEventGroupSetBits(el->state_event, STOPPED_BIT);
    xEventGroupSetBits(el->state_event, RESUMED_BIT);
    xEventGroupSetBits(el->state_event, TASK_DESTROYED_BIT);
    audio_thread_delete_task(&el->audio_thread);
}

esp_err_t audio_element_reset_state(audio_element_handle_t el)
{
    return audio_element_force_set_state(el, AEL_STATE_INIT);
}

audio_element_state_t audio_element_get_state(audio_element_handle_t el)
{
    if (el) {
        return el->state;
    }
    return ESP_FAIL;
}

QueueHandle_t audio_element_get_event_queue(audio_element_handle_t el)
{
    if (!el) {
        return NULL;
    }
    return audio_event_iface_get_queue_handle(el->iface_event);
}

esp_err_t audio_element_setdata(audio_element_handle_t el, void *data)
{
    el->data = data;
    return ESP_OK;
}

void *audio_element_getdata(audio_element_handle_t el)
{
    return el->data;
}

esp_err_t audio_element_set_tag(audio_element_handle_t el, const char *tag)
{
    if (el->tag) {
        audio_free(el->tag);
        el->tag = NULL;
    }

    if (tag) {
        el->tag = audio_strdup(tag);
        AUDIO_MEM_CHECK(TAG, el->tag, {
            return ESP_ERR_NO_MEM;
        });
    }
    return ESP_OK;
}

char *audio_element_get_tag(audio_element_handle_t el)
{
    return el->tag;
}

esp_err_t audio_element_set_uri(audio_element_handle_t el, const char *uri)
{
    mutex_lock(el->lock);
    if (el->info.uri) {
        audio_free(el->info.uri);
        el->info.uri = NULL;
    }

    if (uri) {
        el->info.uri = audio_strdup(uri);
        AUDIO_MEM_CHECK(TAG, el->info.uri, {
            mutex_unlock(el->lock);
            return ESP_ERR_NO_MEM;
        });
    }
    mutex_unlock(el->lock);
    return ESP_OK;
}

char *audio_element_get_uri(audio_element_handle_t el)
{
    mutex_lock(el->lock);
    char *uri = el->info.uri;
    mutex_unlock(el->lock);
    return uri;
}

esp_err_t audio_element_set_event_callback(audio_element_handle_t el, event_cb_func cb_func, void *ctx)
{
    el->events_type = EVENTS_TYPE_CB;
    el->callback_event.cb = cb_func;
    el->callback_event.ctx = ctx;
    return ESP_OK;
}

esp_err_t audio_element_msg_set_listener(audio_element_handle_t el, audio_event_iface_handle_t listener)
{
    return audio_event_iface_set_listener(el->iface_event, listener);
}

esp_err_t audio_element_msg_remove_listener(audio_element_handle_t el, audio_event_iface_handle_t listener)
{
    return audio_event_iface_remove_listener(listener, el->iface_event);
}

esp_err_t audio_element_setinfo(audio_element_handle_t el, audio_element_info_t *info)
{
    if (info && el) {
        //FIXME: We will got reset if lock mutex here
        mutex_lock(el->lock);
        memcpy(&el->info, info, sizeof(audio_element_info_t));
        mutex_unlock(el->lock);
        return ESP_OK;
    }
    return ESP_FAIL;
}

esp_err_t audio_element_getinfo(audio_element_handle_t el, audio_element_info_t *info)
{
    if (info && el) {
        mutex_lock(el->lock);
        memcpy(info, &el->info, sizeof(audio_element_info_t));
        mutex_unlock(el->lock);
        return ESP_OK;
    }
    return ESP_FAIL;
}

esp_err_t audio_element_report_info(audio_element_handle_t el)
{
    if (el) {
        audio_event_iface_msg_t msg = { 0 };
        msg.cmd = AEL_MSG_CMD_REPORT_MUSIC_INFO;
        msg.data = NULL;
        ESP_LOGD(TAG, "REPORT_INFO,[%s]evt out cmd:%d,", el->tag, msg.cmd);
        audio_element_msg_sendout(el, &msg);
        return ESP_OK;
    }
    return ESP_FAIL;
}

esp_err_t audio_element_report_codec_fmt(audio_element_handle_t el)
{
    if (el) {
        audio_event_iface_msg_t msg = { 0 };
        msg.cmd = AEL_MSG_CMD_REPORT_CODEC_FMT;
        msg.data = NULL;
        ESP_LOGD(TAG, "REPORT_FMT,[%s]evt out cmd:%d,", el->tag, msg.cmd);
        audio_element_msg_sendout(el, &msg);
        return ESP_OK;
    }
    return ESP_FAIL;
}

esp_err_t audio_element_report_status(audio_element_handle_t el, audio_element_status_t status)
{
    if (el) {
        audio_event_iface_msg_t msg = { 0 };
        msg.cmd = AEL_MSG_CMD_REPORT_STATUS;
        msg.data = (void *)status;
        msg.data_len = sizeof(status);
        ESP_LOGD(TAG, "REPORT_STATUS,[%s]evt out cmd = %d,status:%d", el->tag, msg.cmd, status);
        return audio_element_msg_sendout(el, &msg);
    }
    return ESP_FAIL;
}

esp_err_t audio_element_report_pos(audio_element_handle_t el)
{
    if (el) {
        audio_event_iface_msg_t msg = { 0 };
        msg.cmd = AEL_MSG_CMD_REPORT_POSITION;
        if (el->report_info == NULL) {
            el->report_info = audio_calloc(1, sizeof(audio_element_info_t));
            AUDIO_MEM_CHECK(TAG, el->report_info, return ESP_ERR_NO_MEM);
        }

        audio_element_getinfo(el, el->report_info);
        msg.data = el->report_info;
        msg.data_len = sizeof(audio_element_info_t);
        ESP_LOGD(TAG, "REPORT_POS,[%s]evt out cmd:%d,", el->tag, msg.cmd);
        audio_element_msg_sendout(el, &msg);
        return ESP_OK;
    }
    return ESP_FAIL;
}

esp_err_t audio_element_finish_state(audio_element_handle_t el)
{
    if (el->task_stack <= 0) {
        el->state = AEL_STATE_FINISHED;
        audio_element_report_status(el, AEL_STATUS_STATE_FINISHED);
        el->is_running = false;
        xEventGroupSetBits(el->state_event, STOPPED_BIT);
        return ESP_OK;
    }
    return ESP_FAIL;
}

esp_err_t audio_element_change_cmd(audio_element_handle_t el, audio_element_msg_cmd_t cmd)
{
    AUDIO_NULL_CHECK(TAG, el, return ESP_ERR_INVALID_ARG);
    return audio_element_cmd_send(el, cmd);
}

esp_err_t audio_element_reset_input_ringbuf(audio_element_handle_t el)
{
    if (el->read_type != IO_TYPE_RB) {
        return ESP_FAIL;
    }
    int ret = ESP_OK;
    if (el->in.input_rb) {
        ret |= rb_reset(el->in.input_rb);
        for (int i = 0; i < el->multi_in.max_rb_num; ++i) {
            if (el->multi_in.rb[i]) {
                ret |= rb_reset(el->multi_in.rb[i]);
            }
        }
    }
    return ret;
}

esp_err_t audio_element_reset_output_ringbuf(audio_element_handle_t el)
{
    if (el->write_type != IO_TYPE_RB) {
        return ESP_FAIL;
    }
    int ret = ESP_OK;
    if (el->out.output_rb) {
        ret |= rb_reset(el->out.output_rb);
        for (int i = 0; i < el->multi_out.max_rb_num; ++i) {
            if (el->multi_out.rb[i]) {
                ret |= rb_reset(el->multi_out.rb[i]);
            }
        }
    }
    return ESP_OK;
}

esp_err_t audio_element_abort_input_ringbuf(audio_element_handle_t el)
{
    if (el->read_type != IO_TYPE_RB) {
        return ESP_FAIL;
    }
    int ret = ESP_OK;
    if (el->in.input_rb) {
        ret |= rb_abort(el->in.input_rb);
        for (int i = 0; i < el->multi_in.max_rb_num; ++i) {
            if (el->multi_in.rb[i]) {
                ret |= rb_abort(el->multi_in.rb[i]);
            }
        }
    }
    return ESP_OK;
}

esp_err_t audio_element_abort_output_ringbuf(audio_element_handle_t el)
{
    if (el->write_type != IO_TYPE_RB) {
        return ESP_FAIL;
    }
    int ret = ESP_OK;
    if (el->out.output_rb) {
        ret |= rb_abort(el->out.output_rb);
        for (int i = 0; i < el->multi_out.max_rb_num; ++i) {
            if (el->multi_out.rb[i]) {
                ret |= rb_abort(el->multi_out.rb[i]);
            }
        }
    }
    return ESP_OK;
}

esp_err_t audio_element_set_ringbuf_done(audio_element_handle_t el)
{
    int ret = ESP_OK;
    if (NULL == el) {
        return ESP_FAIL;
    }
    if (el->out.output_rb && el->write_type == IO_TYPE_RB) {
        ret |= rb_done_write(el->out.output_rb);
        for (int i = 0; i < el->multi_out.max_rb_num; ++i) {
            if (el->multi_out.rb[i]) {
                ret |= rb_done_write(el->multi_out.rb[i]);
            }
        }
    }
    return ret;
}

esp_err_t audio_element_set_input_ringbuf(audio_element_handle_t el, ringbuf_handle_t rb)
{
    if (rb) {
        el->in.input_rb = rb;
        el->read_type = IO_TYPE_RB;
    } else if (el->read_type == IO_TYPE_RB) {
        el->in.input_rb = rb;
    }
    return ESP_OK;
}

ringbuf_handle_t audio_element_get_input_ringbuf(audio_element_handle_t el)
{
    if (el->read_type == IO_TYPE_RB) {
        return el->in.input_rb;
    } else {
        return NULL;
    }
}

esp_err_t audio_element_set_output_ringbuf(audio_element_handle_t el, ringbuf_handle_t rb)
{
    if (rb) {
        el->out.output_rb = rb;
        el->write_type = IO_TYPE_RB;
    } else if (el->write_type == IO_TYPE_RB) {
        el->out.output_rb = rb;
    }
    return ESP_OK;
}

ringbuf_handle_t audio_element_get_output_ringbuf(audio_element_handle_t el)
{
    if (el->write_type == IO_TYPE_RB) {
        return el->out.output_rb;
    } else {
        return NULL;
    }
}

esp_err_t audio_element_set_input_timeout(audio_element_handle_t el, TickType_t timeout)
{
    if (el) {
        el->input_wait_time = timeout;
        return ESP_OK;
    }
    return ESP_FAIL;
}

esp_err_t audio_element_set_output_timeout(audio_element_handle_t el, TickType_t timeout)
{
    if (el) {
        el->output_wait_time = timeout;
        return ESP_OK;
    }
    return ESP_FAIL;
}

int audio_element_get_output_ringbuf_size(audio_element_handle_t el)
{
    if (el) {
        return el->out_rb_size;
    }
    return 0;
}

esp_err_t audio_element_set_output_ringbuf_size(audio_element_handle_t el, int rb_size)
{
    if (el) {
        el->out_rb_size = rb_size;
        return ESP_OK;
    }
    return ESP_FAIL;
}

esp_err_t audio_element_set_read_cb(audio_element_handle_t el, stream_func fn, void *context)
{
    if (el) {
        el->in.read_cb.cb = fn;
        el->in.read_cb.ctx = context;
        el->read_type = IO_TYPE_CB;
        return ESP_OK;
    }
    return ESP_FAIL;
}

esp_err_t audio_element_set_write_cb(audio_element_handle_t el, stream_func fn, void *context)
{
    if (el) {
        el->out.write_cb.cb = fn;
        el->out.write_cb.ctx = context;
        el->write_type = IO_TYPE_CB;
        return ESP_OK;
    }
    return ESP_FAIL;
}

stream_func audio_element_get_write_cb(audio_element_handle_t el)
{
    if (el && el->write_type == IO_TYPE_CB) {
        return el->out.write_cb.cb;
    }
    ESP_LOGE(TAG, "Fail to get write callback");
    return NULL;
}

stream_func audio_element_get_read_cb(audio_element_handle_t el)
{
    if (el && el->read_type == IO_TYPE_CB) {
        return el->in.read_cb.cb;
    }
    ESP_LOGE(TAG, "Fail to get read callback");
    return NULL;
}

esp_err_t audio_element_wait_for_stop(audio_element_handle_t el)
{
    if (el->is_running == false) {
        ESP_LOGD(TAG, "[%s] Element already stopped, return without waiting", el->tag);
        return ESP_FAIL;
    }
    EventBits_t uxBits = xEventGroupWaitBits(el->state_event, STOPPED_BIT, false, true, DEFAULT_MAX_WAIT_TIME);
    esp_err_t ret = ESP_ERR_TIMEOUT;
    if (uxBits & STOPPED_BIT) {
        ret = ESP_OK;
    }
    return ret;
}

esp_err_t audio_element_wait_for_buffer(audio_element_handle_t el, int size_expect, TickType_t timeout)
{
    int ret = ESP_FAIL;
    el->out_buf_size_expect = size_expect;
    if (el->out.output_rb) {
        xEventGroupClearBits(el->state_event, BUFFER_REACH_LEVEL_BIT);
        EventBits_t uxBits = xEventGroupWaitBits(el->state_event, BUFFER_REACH_LEVEL_BIT, false, true, timeout);
        if ((uxBits & BUFFER_REACH_LEVEL_BIT) != 0) {
            ret = ESP_OK;
        } else {
            ret = ESP_FAIL;
        }
    }
    return ret;
}

audio_element_handle_t audio_element_init(audio_element_cfg_t *config)
{
    audio_element_handle_t el = audio_calloc(1, sizeof(struct audio_element));

    AUDIO_MEM_CHECK(TAG, el, {
        return NULL;
    });

    audio_event_iface_cfg_t evt_cfg = AUDIO_EVENT_IFACE_DEFAULT_CFG();
    evt_cfg.on_cmd = audio_element_on_cmd;
    evt_cfg.context = el;
    evt_cfg.queue_set_size = 0; // Element have no queue_set by default.
    evt_cfg.external_queue_size = 5;
    evt_cfg.internal_queue_size = 5;
    bool _success =
        (
            ((config->tag ? audio_element_set_tag(el, config->tag) : audio_element_set_tag(el, "unknown")) == ESP_OK) &&
            (el->lock           = mutex_create())                   &&
            (el->iface_event    = audio_event_iface_init(&evt_cfg)) &&
            (el->state_event    = xEventGroupCreate())
        );

    AUDIO_MEM_CHECK(TAG, _success, goto _element_init_failed);

    el->open = config->open;
    el->process = config->process;
    el->close = config->close;
    el->destroy = config->destroy;
    el->seek = config->seek;
    el->multi_in.max_rb_num = config->multi_in_rb_num;
    el->multi_out.max_rb_num = config->multi_out_rb_num;
    if (el->multi_in.max_rb_num > 0) {
        el->multi_in.rb = (ringbuf_handle_t *)audio_calloc(el->multi_in.max_rb_num, sizeof(ringbuf_handle_t));
        AUDIO_MEM_CHECK(TAG, el->multi_in.rb, goto _element_init_failed);
    }
    if (el->multi_out.max_rb_num > 0) {
        el->multi_out.rb = (ringbuf_handle_t *)audio_calloc(el->multi_out.max_rb_num, sizeof(ringbuf_handle_t));
        AUDIO_MEM_CHECK(TAG, el->multi_out.rb, goto _element_init_failed);
    }

    if (config->task_stack > 0) {
        el->task_stack = config->task_stack;
        el->stack_in_ext = config->stack_in_ext;
    }
    if (config->task_prio) {
        el->task_prio = config->task_prio;
    } else {
        el->task_prio = DEFAULT_ELEMENT_TASK_PRIO;
    }
    if (config->task_core) {
        el->task_core = config->task_core;
    } else {
        el->task_core = DEFAULT_ELEMENT_TASK_CORE;
    }
    if (config->out_rb_size > 0) {
        el->out_rb_size = config->out_rb_size;
    } else {
        el->out_rb_size = DEFAULT_ELEMENT_RINGBUF_SIZE;
    }
    el->data = config ->data;

    el->state = AEL_STATE_INIT;
    el->buf_size = config->buffer_len;

    audio_element_info_t info = AUDIO_ELEMENT_INFO_DEFAULT();
    audio_element_setinfo(el, &info);
    audio_element_set_input_timeout(el, portMAX_DELAY);
    audio_element_set_output_timeout(el, portMAX_DELAY);

    if (config->read != NULL) {
        el->read_type = IO_TYPE_CB;
        el->in.read_cb.cb = config->read;
    } else {
        el->read_type = IO_TYPE_RB;
    }

    if (config->write != NULL) {
        el->write_type = IO_TYPE_CB;
        el->out.write_cb.cb = config->write;
    } else {
        el->write_type = IO_TYPE_RB;
    }

    el->events_type = EVENTS_TYPE_Q;
    return el;
_element_init_failed:
    audio_element_set_uri(el, NULL);
    if (el->lock) {
        mutex_destroy(el->lock);
    }
    if (el->state_event) {
        vEventGroupDelete(el->state_event);
    }
    if (el->iface_event) {
        audio_event_iface_destroy(el->iface_event);
    }
    if (el->tag) {
        audio_element_set_tag(el, NULL);
    }
    if (el->multi_in.rb) {
        audio_free(el->multi_in.rb);
        el->multi_in.rb = NULL;
    }
    if (el->multi_out.rb) {
        audio_free(el->multi_out.rb);
        el->multi_out.rb = NULL;
    }
    audio_free(el);
    return NULL;
}

esp_err_t audio_element_deinit(audio_element_handle_t el)
{
    audio_element_stop(el);
    audio_element_wait_for_stop(el);
    audio_element_terminate(el);
    vEventGroupDelete(el->state_event);

    audio_event_iface_destroy(el->iface_event);
    if (el->destroy) {
        el->destroy(el);
    }
    audio_element_set_tag(el, NULL);
    audio_element_set_uri(el, NULL);
    if (el->multi_in.rb) {
        audio_free(el->multi_in.rb);
        el->multi_in.rb = NULL;
    }
    if (el->multi_out.rb) {
        audio_free(el->multi_out.rb);
        el->multi_out.rb = NULL;
    }
    if (el->report_info) {
        audio_free(el->report_info);
    }
    if (el->audio_thread) {
        audio_thread_cleanup(&el->audio_thread);
    }
    mutex_destroy(el->lock);
    el->lock = NULL;
    audio_free(el);
    return ESP_OK;
}

esp_err_t audio_element_run(audio_element_handle_t el)
{
    char task_name[32];
    esp_err_t ret = ESP_FAIL;
    if (el->task_run) {
        ESP_LOGD(TAG, "[%s-%p] Element already created", el->tag, el);
        return ESP_OK;
    }
    ESP_LOGV(TAG, "[%s] Element starting...", el->tag);
    snprintf(task_name, 32, "el-%s", el->tag);
    audio_event_iface_discard(el->iface_event);
    xEventGroupClearBits(el->state_event, TASK_CREATED_BIT);
    if (el->task_stack > 0) {
        ret = audio_thread_create(&el->audio_thread, el->tag, audio_element_task, el, el->task_stack,
                                  el->task_prio, el->stack_in_ext, el->task_core);
        if (ret == ESP_FAIL) {
            audio_element_force_set_state(el, AEL_STATE_ERROR);
            audio_element_report_status(el, AEL_STATUS_ERROR_OPEN);
            ESP_LOGE(TAG, "[%s] audio_thread_create failed", el->tag);
            return ESP_FAIL;
        }
        EventBits_t uxBits = xEventGroupWaitBits(el->state_event, TASK_CREATED_BIT, false, true, DEFAULT_MAX_WAIT_TIME);

        if (uxBits & TASK_CREATED_BIT) {
            ret = ESP_OK;
        }
    } else {
        el->task_run = true;
        el->is_running = true;
        audio_element_force_set_state(el, AEL_STATE_RUNNING);
        audio_element_report_status(el, AEL_STATUS_STATE_RUNNING);
        ret = ESP_OK;
    }
    ESP_LOGI(TAG, "[%s-%p] Element task created", el->tag, el);
    return ret;
}

static inline esp_err_t __audio_element_term(audio_element_handle_t el, TickType_t ticks_to_wait)
{
    xEventGroupClearBits(el->state_event, TASK_DESTROYED_BIT);
    if (audio_element_cmd_send(el, AEL_MSG_CMD_DESTROY) != ESP_OK) {
        ESP_LOGE(TAG, "[%s] Send destroy command failed", el->tag);
        return ESP_FAIL;
    }
    EventBits_t uxBits = xEventGroupWaitBits(el->state_event, TASK_DESTROYED_BIT, false, true, ticks_to_wait);
    esp_err_t ret = ESP_FAIL;
    if (uxBits & TASK_DESTROYED_BIT ) {
        ESP_LOGD(TAG, "[%s-%p] Element task destroyed", el->tag, el);
        ret = ESP_OK;
    } else {
        ESP_LOGW(TAG, "[%s-%p] Element task destroy timeout[%d]", el->tag, el, ticks_to_wait);
    }
    return ret;
}

esp_err_t audio_element_terminate(audio_element_handle_t el)
{
    if (!el->task_run) {
        ESP_LOGW(TAG, "[%s] Element has not create when AUDIO_ELEMENT_TERMINATE", el->tag);
        return ESP_OK;
    }
    if (el->task_stack <= 0) {
        el->task_run = false;
        el->is_running = false;
        return ESP_OK;
    }
    return __audio_element_term(el, DEFAULT_MAX_WAIT_TIME);
}

esp_err_t audio_element_terminate_with_ticks(audio_element_handle_t el, TickType_t ticks_to_wait)
{
    if (!el->task_run) {
        ESP_LOGW(TAG, "[%s] Element has not create when AUDIO_ELEMENT_TERMINATE, tick:%d", el->tag, ticks_to_wait);
        return ESP_OK;
    }
    if (el->task_stack <= 0) {
        el->task_run = false;
        el->is_running = false;
        return ESP_OK;
    }
    return __audio_element_term(el, ticks_to_wait);
}

esp_err_t audio_element_pause(audio_element_handle_t el)
{
    if (!el->task_run) {
        ESP_LOGW(TAG, "[%s] Element has not create when AUDIO_ELEMENT_PAUSE", el->tag);
        return ESP_FAIL;
    }
    if ((el->state >= AEL_STATE_PAUSED)) {
        audio_element_force_set_state(el, AEL_STATE_PAUSED);
        ESP_LOGD(TAG, "[%s] Element already paused, state:%d", el->tag, el->state);
        return ESP_OK;
    }
    xEventGroupClearBits(el->state_event, PAUSED_BIT);
    if (el->task_stack <= 0) {
        el->is_running = false;
        audio_element_force_set_state(el, AEL_STATE_PAUSED);
        return ESP_OK;
    }
    if (audio_element_cmd_send(el, AEL_MSG_CMD_PAUSE) != ESP_OK) {
        ESP_LOGE(TAG, "[%s] Element send cmd error when AUDIO_ELEMENT_PAUSE", el->tag);
        return ESP_FAIL;
    }
    EventBits_t uxBits = xEventGroupWaitBits(el->state_event, PAUSED_BIT, false, true, DEFAULT_MAX_WAIT_TIME);
    esp_err_t ret = ESP_FAIL;
    if (uxBits & PAUSED_BIT) {
        ret = ESP_OK;
    }
    return ret;
}

esp_err_t audio_element_resume(audio_element_handle_t el, float wait_for_rb_threshold, TickType_t timeout)
{
    if (!el->task_run) {
        ESP_LOGW(TAG, "[%s] Element has not create when AUDIO_ELEMENT_RESUME", el->tag);
        return ESP_FAIL;
    }
    if (el->state == AEL_STATE_RUNNING) {
        audio_element_report_status(el, AEL_STATUS_STATE_RUNNING);
        ESP_LOGD(TAG, "[%s] RESUME: Element is already running, state:%d, task_run:%d, is_running:%d",
                 el->tag, el->state, el->task_run, el->is_running);
        return ESP_OK;
    }
    if (el->task_stack <= 0) {
        el->is_running = true;
        audio_element_force_set_state(el, AEL_STATE_RUNNING);
        audio_element_report_status(el, AEL_STATUS_STATE_RUNNING);
        return ESP_OK;
    }
    if (el->state == AEL_STATE_ERROR) {
        ESP_LOGE(TAG, "[%s] RESUME: Element error, state:%d", el->tag, el->state);
        return ESP_FAIL;
    }
    if (el->state == AEL_STATE_FINISHED) {
        ESP_LOGI(TAG, "[%s] RESUME: Element has finished, state:%d", el->tag, el->state);
        audio_element_report_status(el, AEL_STATUS_STATE_FINISHED);
        return ESP_OK;
    }
    if (wait_for_rb_threshold > 1 || wait_for_rb_threshold < 0) {
        return ESP_FAIL;
    }
    int ret =  ESP_OK;
    xEventGroupClearBits(el->state_event, RESUMED_BIT);
    if (audio_element_cmd_send(el, AEL_MSG_CMD_RESUME) == ESP_FAIL) {
        ESP_LOGW(TAG, "[%s] Send resume command failed", el->tag);
        return ESP_FAIL;
    }
    EventBits_t uxBits = xEventGroupWaitBits(el->state_event, RESUMED_BIT, false, true, timeout);
    if ((uxBits & RESUMED_BIT) != RESUMED_BIT) {
        ESP_LOGW(TAG, "[%s-%p] RESUME timeout", el->tag, el);
        ret = ESP_FAIL;
    } else {
        if (wait_for_rb_threshold != 0 && el->read_type == IO_TYPE_RB) {
            ret = audio_element_wait_for_buffer(el, rb_get_size(el->in.input_rb) * wait_for_rb_threshold, timeout);
        }
    }
    return ret;
}

esp_err_t audio_element_stop(audio_element_handle_t el)
{
    if (!el->task_run) {
        ESP_LOGD(TAG, "[%s] Element has not create when AUDIO_ELEMENT_STOP", el->tag);
        return ESP_FAIL;
    }
    if (el->is_running == false) {
        xEventGroupSetBits(el->state_event, STOPPED_BIT);
        audio_element_report_status(el, AEL_STATUS_STATE_STOPPED);
        ESP_LOGE(TAG, "[%s] Element already stopped", el->tag);
        return ESP_OK;
    }
    audio_element_abort_output_ringbuf(el);
    audio_element_abort_input_ringbuf(el);
    if (el->state == AEL_STATE_RUNNING) {
        xEventGroupClearBits(el->state_event, STOPPED_BIT);
    }
    if (el->task_stack <= 0) {
        el->is_running = false;
        audio_element_force_set_state(el, AEL_STATE_STOPPED);
        xEventGroupSetBits(el->state_event, STOPPED_BIT);
        audio_element_report_status(el, AEL_STATUS_STATE_STOPPED);
        return ESP_OK;
    }
    if (el->state == AEL_STATE_PAUSED) {
        audio_event_iface_set_cmd_waiting_timeout(el->iface_event, 0);
    }
    if (el->stopping) {
        ESP_LOGD(TAG, "[%s] Stop command has already sent, %d", el->tag, el->stopping);
        return ESP_OK;
    }
    el->stopping = true;
    if (audio_element_cmd_send(el, AEL_MSG_CMD_STOP) != ESP_OK) {
        el->stopping = false;
        ESP_LOGW(TAG, "[%s-%p] Send stop command failed", el->tag, el);
        return ESP_FAIL;
    }
    ESP_LOGD(TAG, "[%s-%p] Send stop command", el->tag, el);
    return ESP_OK;
}

esp_err_t audio_element_wait_for_stop_ms(audio_element_handle_t el, TickType_t ticks_to_wait)
{
    if (el->is_running == false) {
        ESP_LOGD(TAG, "[%s] Element already stopped, return without waiting", el->tag);
        return ESP_FAIL;
    }
    EventBits_t uxBits = xEventGroupWaitBits(el->state_event, STOPPED_BIT, false, true, ticks_to_wait);
    esp_err_t ret = ESP_ERR_TIMEOUT;
    if (uxBits & STOPPED_BIT) {
        ret = ESP_OK;
    }
    return ret;
}

esp_err_t audio_element_multi_input(audio_element_handle_t el, char *buffer, int wanted_size, int index, TickType_t ticks_to_wait)
{
    esp_err_t ret = ESP_OK;
    if (index >= el->multi_in.max_rb_num) {
        ESP_LOGE(TAG, "The index of ringbuffer is gather than and equal to ringbuffer maximum (%d). line %d", el->multi_in.max_rb_num, __LINE__);
        return ESP_FAIL;
    }
    if (el->multi_in.rb[index]) {
        ret = rb_read(el->multi_in.rb[index], buffer, wanted_size, ticks_to_wait);
    }
    return ret;
}

esp_err_t audio_element_multi_output(audio_element_handle_t el, char *buffer, int wanted_size, TickType_t ticks_to_wait)
{
    esp_err_t ret = ESP_OK;
    for (int i = 0; i < el->multi_out.max_rb_num; ++i) {
        if (el->multi_out.rb[i]) {
            ret |= rb_write(el->multi_out.rb[i], buffer, wanted_size, ticks_to_wait);
        }
    }
    return ret;
}

esp_err_t audio_element_set_multi_input_ringbuf(audio_element_handle_t el, ringbuf_handle_t rb, int index)
{
    if ((index < el->multi_in.max_rb_num) && rb) {
        el->multi_in.rb[index] = rb;
        return ESP_OK;
    }
    return ESP_ERR_INVALID_ARG;
}

esp_err_t audio_element_set_multi_output_ringbuf(audio_element_handle_t el, ringbuf_handle_t rb, int index)
{
    if ((index < el->multi_out.max_rb_num) && rb) {
        el->multi_out.rb[index] = rb;
        return ESP_OK;
    }
    return ESP_ERR_INVALID_ARG;
}

ringbuf_handle_t audio_element_get_multi_input_ringbuf(audio_element_handle_t el, int index)
{
    if (index < el->multi_in.max_rb_num) {
        return el->multi_in.rb[index];
    }
    return NULL;
}

ringbuf_handle_t audio_element_get_multi_output_ringbuf(audio_element_handle_t el, int index)
{
    if (index < el->multi_out.max_rb_num) {
        return el->multi_out.rb[index];
    }
    return NULL;
}

esp_err_t audio_element_seek(audio_element_handle_t el, void *in_data, int in_size, void *out_data, int *out_size)
{
    esp_err_t ret = ESP_OK;
    if (el && el->seek) {
        ret = el->seek(el, in_data, in_size, out_data, out_size);
    } else {
        ret = ESP_ERR_NOT_SUPPORTED;
    }
    return ret;
}

bool audio_element_is_stopping(audio_element_handle_t el)
{
    if (el) {
        return el->stopping;
    }
    return false;
}

esp_err_t audio_element_update_byte_pos(audio_element_handle_t el, int pos)
{
    if (el) {
        mutex_lock(el->lock);
        el->info.byte_pos += pos;
        mutex_unlock(el->lock);
        return ESP_OK;
    }
    return ESP_FAIL;
}

esp_err_t audio_element_set_byte_pos(audio_element_handle_t el, int pos)
{
    if (el) {
        mutex_lock(el->lock);
        el->info.byte_pos = pos;
        mutex_unlock(el->lock);
        return ESP_OK;
    }
    return ESP_FAIL;
}

esp_err_t audio_element_update_total_bytes(audio_element_handle_t el, int total_bytes)
{
    if (el) {
        mutex_lock(el->lock);
        el->info.total_bytes += total_bytes;
        mutex_unlock(el->lock);
        return ESP_OK;
    }
    return ESP_FAIL;
}

esp_err_t audio_element_set_total_bytes(audio_element_handle_t el, int total_bytes)
{
    if (el) {
        mutex_lock(el->lock);
        el->info.total_bytes = total_bytes;
        mutex_unlock(el->lock);
        return ESP_OK;
    }
    return ESP_FAIL;
}

esp_err_t audio_element_set_bps(audio_element_handle_t el, int bit_rate)
{
    if (el) {
        mutex_lock(el->lock);
        el->info.bps = bit_rate;
        mutex_unlock(el->lock);
        return ESP_OK;
    }
    return ESP_FAIL;
}

esp_err_t audio_element_set_codec_fmt(audio_element_handle_t el, int format)
{
    if (el) {
        mutex_lock(el->lock);
        el->info.codec_fmt = format;
        mutex_unlock(el->lock);
        return ESP_OK;
    }
    return ESP_FAIL;
}

esp_err_t audio_element_set_music_info(audio_element_handle_t el, int sample_rates, int channels, int bits)
{
    if (el) {
        mutex_lock(el->lock);
        el->info.sample_rates = sample_rates;
        el->info.channels = channels;
        el->info.bits = bits;
        mutex_unlock(el->lock);
        return ESP_OK;
    }
    return ESP_FAIL;
}

esp_err_t audio_element_set_duration(audio_element_handle_t el, int duration)
{
    if (el) {
        mutex_lock(el->lock);
        el->info.duration = duration;
        mutex_unlock(el->lock);
        return ESP_OK;
    }
    return ESP_FAIL;
}

esp_err_t audio_element_set_reserve_user0(audio_element_handle_t el, int user_data0)
{
    if (el) {
        mutex_lock(el->lock);
        el->info.reserve_data.user_data_0 = user_data0;
        mutex_unlock(el->lock);
        return ESP_OK;
    }
    return ESP_FAIL;
}

esp_err_t audio_element_set_reserve_user1(audio_element_handle_t el, int user_data1)
{
    if (el) {
        mutex_lock(el->lock);
        el->info.reserve_data.user_data_1 = user_data1;
        mutex_unlock(el->lock);
        return ESP_OK;
    }
    return ESP_FAIL;
}

esp_err_t audio_element_set_reserve_user2(audio_element_handle_t el, int user_data2)
{
    if (el) {
        mutex_lock(el->lock);
        el->info.reserve_data.user_data_2 = user_data2;
        mutex_unlock(el->lock);
        return ESP_OK;
    }
    return ESP_FAIL;
}


esp_err_t audio_element_set_reserve_user3(audio_element_handle_t el, int user_data3)
{
    if (el) {
        mutex_lock(el->lock);
        el->info.reserve_data.user_data_3 = user_data3;
        mutex_unlock(el->lock);
        return ESP_OK;
    }
    return ESP_FAIL;
}

esp_err_t audio_element_set_reserve_user4(audio_element_handle_t el, int user_data4)
{
    if (el) {
        mutex_lock(el->lock);
        el->info.reserve_data.user_data_4 = user_data4;
        mutex_unlock(el->lock);
        return ESP_OK;
    }
    return ESP_FAIL;
}
