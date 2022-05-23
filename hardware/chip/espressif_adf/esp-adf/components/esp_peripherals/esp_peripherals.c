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

#include <string.h>
#include "sys/queue.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "audio_event_iface.h"
#include "audio_mutex.h"
#include "esp_peripherals.h"
#include "audio_thread.h"
#include "audio_mem.h"

static const char *TAG = "ESP_PERIPH";

#define DEFAULT_ESP_PERIPH_WAIT_TICK       (10/portTICK_RATE_MS)

struct esp_periph {
    char                       *tag;
    bool                        disabled;
    esp_periph_id_t             periph_id;
    esp_periph_func             init;
    esp_periph_run_func         run;
    esp_periph_func             destroy;
    esp_periph_state_t          state;
    void                       *source;
    void                       *periph_data;
    esp_periph_event_t         *on_evt;
    TimerHandle_t               timer;
    STAILQ_ENTRY(esp_periph)    entries;
};

typedef struct esp_periph_sets {
    EventGroupHandle_t                              state_event_bits;
    xSemaphoreHandle                                lock;
    int                                             task_stack;
    int                                             task_prio;
    int                                             task_core;
    audio_thread_t                                  audio_thread;
    bool                                            ext_stack;
    bool                                            run;
    esp_periph_event_t                              event_handle;
    STAILQ_HEAD(esp_periph_list_item, esp_periph)   periph_list;
} esp_periph_set_t;

static const int STARTED_BIT = BIT0;
static const int STOPPED_BIT = BIT1;

static esp_err_t esp_periph_wait_for_stop(esp_periph_set_handle_t periph_set_handle, TickType_t ticks_to_wait);

static esp_err_t process_peripheral_event(audio_event_iface_msg_t *msg, void *context)
{
    esp_periph_handle_t periph_evt = (esp_periph_handle_t) msg->source;
    esp_periph_handle_t periph;
    esp_periph_set_t *sets = context;
    STAILQ_FOREACH(periph, &sets->periph_list, entries) {
        if (periph->periph_id == periph_evt->periph_id
            && periph_evt->state == PERIPH_STATE_RUNNING
            && periph_evt->run
            && !periph_evt->disabled) {
            return periph_evt->run(periph_evt, msg);
        }
    }
    return ESP_OK;
}

static void esp_periph_task(void *pv)
{
    esp_periph_handle_t periph;
    esp_periph_set_handle_t periph_set_handle = (esp_periph_set_handle_t)pv;
    ESP_LOGD(TAG, "esp_periph_task is running, handle:%p", periph_set_handle);
    xEventGroupSetBits(periph_set_handle->state_event_bits, STARTED_BIT);
    xEventGroupClearBits(periph_set_handle->state_event_bits, STOPPED_BIT);

    while (periph_set_handle->run) {
        mutex_lock(periph_set_handle->lock);
        STAILQ_FOREACH(periph, &periph_set_handle->periph_list, entries) {
            if (periph->disabled) {
                continue;
            }
            if (periph->state == PERIPH_STATE_INIT && periph->init) {
                ESP_LOGD(TAG, "PERIPH[%s]->init", periph->tag);
                if (periph->init(periph) == ESP_OK) {
                    periph->state = PERIPH_STATE_RUNNING;
                } else {
                    periph->state = PERIPH_STATE_ERROR;
                }
            }
        }
        mutex_unlock(periph_set_handle->lock);
        audio_event_iface_waiting_cmd_msg(esp_periph_set_get_event_iface(periph_set_handle));
    }
    STAILQ_FOREACH(periph, &periph_set_handle->periph_list, entries) {
        esp_periph_stop_timer(periph);
        if (periph->destroy) {
            periph->destroy(periph);
        }
    }
    xEventGroupClearBits(periph_set_handle->state_event_bits, STARTED_BIT);
    xEventGroupSetBits(periph_set_handle->state_event_bits, STOPPED_BIT);

    vTaskDelete(NULL);
}

esp_periph_set_handle_t esp_periph_set_init(esp_periph_config_t *config)
{
    esp_periph_set_t *periph_sets = NULL;
    int _err_step = 1;
    bool _success =
        (
            (periph_sets                   = audio_calloc(1, sizeof(esp_periph_set_t))) && _err_step ++ &&
            (periph_sets->state_event_bits = xEventGroupCreate())                  && _err_step ++ &&
            (periph_sets->lock             = mutex_create())                       && _err_step ++
        );

    AUDIO_MEM_CHECK(TAG, _success, {
        goto _periph_init_failed;
    });

    STAILQ_INIT(&periph_sets->periph_list);

    //TODO: Should we uninstall gpio isr service??
    //TODO: Because gpio need for sdcard and gpio, then install isr here

    gpio_install_isr_service(ESP_INTR_FLAG_LEVEL1);

    periph_sets->run = false;
    xEventGroupClearBits(periph_sets->state_event_bits, STARTED_BIT);
    xEventGroupSetBits(periph_sets->state_event_bits, STOPPED_BIT);
    periph_sets->task_stack = config->task_stack;
    periph_sets->task_prio = config->task_prio;
    periph_sets->task_core = config->task_core;
    periph_sets->ext_stack = config->extern_stack;

    audio_event_iface_cfg_t event_cfg = AUDIO_EVENT_IFACE_DEFAULT_CFG();
    event_cfg.queue_set_size = 0;
    event_cfg.context = periph_sets;
    event_cfg.on_cmd = process_peripheral_event;
    periph_sets->event_handle.iface = audio_event_iface_init(&event_cfg);

    AUDIO_MEM_CHECK(TAG, periph_sets->event_handle.iface, goto _periph_init_failed);
    audio_event_iface_set_cmd_waiting_timeout(periph_sets->event_handle.iface, DEFAULT_ESP_PERIPH_WAIT_TICK);
    return periph_sets;

_periph_init_failed:
    if (periph_sets) {
        mutex_destroy(periph_sets->lock);
        vEventGroupDelete(periph_sets->state_event_bits);

        if (periph_sets->event_handle.iface) {
            audio_event_iface_destroy(periph_sets->event_handle.iface);
        }

        audio_free(periph_sets);
        periph_sets = NULL;
    }
    return NULL;
}

esp_err_t esp_periph_set_destroy(esp_periph_set_handle_t periph_set_handle)
{
    if (periph_set_handle == NULL) {
        AUDIO_ERROR(TAG, "Peripherals have not been initialized");
        return ESP_FAIL;
    }
    periph_set_handle->run = false;
    esp_periph_wait_for_stop(periph_set_handle, portMAX_DELAY);
    esp_periph_handle_t item, tmp;
    STAILQ_FOREACH_SAFE(item, &periph_set_handle->periph_list, entries, tmp) {
        STAILQ_REMOVE(&periph_set_handle->periph_list, item, esp_periph, entries);
        audio_free(item->tag);
        audio_free(item);
    }
    mutex_destroy(periph_set_handle->lock);
    vEventGroupDelete(periph_set_handle->state_event_bits);

    gpio_uninstall_isr_service();
    audio_event_iface_destroy(periph_set_handle->event_handle.iface);
    audio_free(periph_set_handle);
    periph_set_handle = NULL;
    return ESP_OK;
}

esp_err_t esp_periph_set_stop_all(esp_periph_set_handle_t periph_set_handle)
{
    if (periph_set_handle == NULL) {
        AUDIO_ERROR(TAG, "Peripherals have not been initialized");
        return ESP_FAIL;
    }
    esp_periph_handle_t periph;
    STAILQ_FOREACH(periph, &periph_set_handle->periph_list, entries) {
        periph->disabled = true;
    }
    return ESP_OK;
}

esp_periph_handle_t esp_periph_set_get_by_id(esp_periph_set_handle_t periph_set_handle, int periph_id)
{
    esp_periph_handle_t periph;
    if (periph_set_handle == NULL) {
        AUDIO_ERROR(TAG, "Peripherals have not been initialized");
        return NULL;
    }
    mutex_lock(periph_set_handle->lock);
    STAILQ_FOREACH(periph, &periph_set_handle->periph_list, entries) {
        if (periph->periph_id == periph_id) {
            mutex_unlock(periph_set_handle->lock);
            return periph;
        }
    }
    ESP_LOGD(TAG, "Periph id %d not found", periph_id);
    mutex_unlock(periph_set_handle->lock);
    return NULL;
}

audio_event_iface_handle_t esp_periph_set_get_event_iface(esp_periph_set_handle_t periph_set_handle)
{
    return periph_set_handle->event_handle.iface;
}

esp_err_t esp_periph_set_register_callback(esp_periph_set_handle_t periph_set_handle, esp_periph_event_handle_t cb, void *user_context)
{
    if (periph_set_handle == NULL) {
        return ESP_FAIL;
    } else {
        periph_set_handle->event_handle.cb = cb;
        periph_set_handle->event_handle.user_ctx = user_context;
        return ESP_OK;
    }
}

QueueHandle_t esp_periph_set_get_queue(esp_periph_set_handle_t periph_set_handle)
{
    return audio_event_iface_get_queue_handle(periph_set_handle->event_handle.iface);
}

esp_err_t esp_periph_wait_for_stop(esp_periph_set_handle_t periph_set_handle, TickType_t ticks_to_wait)
{
    EventGroupHandle_t ev_bits = periph_set_handle->state_event_bits;

    EventBits_t uxBits = xEventGroupWaitBits(ev_bits, STOPPED_BIT, false, true, ticks_to_wait);
    if (uxBits & STOPPED_BIT) {
        return ESP_OK;
    }
    return ESP_FAIL;
}

esp_err_t esp_periph_set_list_init(esp_periph_set_handle_t periph_set)
{
    esp_periph_handle_t periph;
    STAILQ_FOREACH(periph, &periph_set->periph_list, entries) {
        if (periph->init) {
            periph->init(periph);
        }
    }
    return ESP_OK;
}

esp_err_t esp_periph_set_list_run(esp_periph_set_handle_t periph_set, audio_event_iface_msg_t msg)
{
    esp_periph_handle_t periph;
    STAILQ_FOREACH(periph, &periph_set->periph_list, entries) {
        if (periph->run) {
            periph->run(periph, &msg);
        }
    }
    return ESP_OK;
}

esp_err_t esp_periph_set_list_destroy(esp_periph_set_handle_t periph_set)
{
    esp_periph_handle_t periph;
    STAILQ_FOREACH(periph, &periph_set->periph_list, entries) {
        if (periph->destroy) {
            periph->destroy(periph);
        }
    }
    return ESP_OK;
}

esp_periph_handle_t esp_periph_create(int periph_id, const char *tag)
{
    esp_periph_handle_t new_entry = audio_calloc(1, sizeof(struct esp_periph));

    AUDIO_MEM_CHECK(TAG, new_entry, return NULL);
    if (tag) {
        new_entry->tag = audio_strdup(tag);
    } else {
        new_entry->tag = audio_strdup("periph");
    }
    AUDIO_MEM_CHECK(TAG, new_entry->tag, {
        audio_free(new_entry);
        return NULL;
    })
    new_entry->state = PERIPH_STATE_INIT;
    new_entry->periph_id = periph_id;
    return new_entry;
}

esp_err_t esp_periph_set_function(esp_periph_handle_t periph,
                                  esp_periph_func init,
                                  esp_periph_run_func run,
                                  esp_periph_func destroy)
{
    periph->init = init;
    periph->run = run;
    periph->destroy = destroy;
    return ESP_OK;
}

esp_err_t esp_periph_start(esp_periph_set_handle_t periph_set_handle, esp_periph_handle_t periph)
{
    if (periph_set_handle == NULL) {
        AUDIO_ERROR(TAG, "Peripherals have not been initialized");
        return ESP_FAIL;
    }
    if (esp_periph_set_get_by_id(periph_set_handle, periph->periph_id) != NULL) {
        ESP_LOGI(TAG, "This peripheral has been added");
        periph->disabled = false;
    } else {
        esp_periph_register_on_events(periph, &periph_set_handle->event_handle);
        STAILQ_INSERT_TAIL(&periph_set_handle->periph_list, periph, entries);
    }
    if (periph_set_handle->run == false && periph_set_handle->task_stack > 0) {
        periph_set_handle->run = true;
        if (audio_thread_create(&periph_set_handle->audio_thread,
                                 "esp_periph",
                                 esp_periph_task,
                                 periph_set_handle,
                                 periph_set_handle->task_stack,
                                 periph_set_handle->task_prio,
                                 periph_set_handle->ext_stack,
                                 periph_set_handle->task_core) != ESP_OK) {
            ESP_LOGE(TAG, "Create [%s] task failed", periph->tag);
            return ESP_FAIL;
        }
    }
    return ESP_OK;
}

esp_err_t esp_periph_stop(esp_periph_handle_t periph)
{
    if (periph) {
        periph->disabled = true;
        return ESP_OK;
    }
    return ESP_OK;
}

esp_err_t esp_periph_send_cmd(esp_periph_handle_t periph, int cmd, void *data, int data_len)
{
    if (periph->on_evt == NULL) {
        return ESP_FAIL;
    }
    audio_event_iface_msg_t msg;
    msg.cmd = cmd;
    msg.source = periph;
    msg.source_type = periph->periph_id;
    msg.data = (void *)data;
    msg.data_len = data_len;
    return audio_event_iface_cmd(periph->on_evt->iface, &msg);
}

esp_err_t esp_periph_send_cmd_from_isr(esp_periph_handle_t periph, int cmd, void *data, int data_len)
{
    if (periph->on_evt == NULL) {
        return ESP_FAIL;
    }
    audio_event_iface_msg_t msg;
    msg.cmd = cmd;
    msg.source = periph;
    msg.source_type = periph->periph_id;
    msg.data = (void *)data;
    msg.data_len = data_len;
    return audio_event_iface_cmd_from_isr(periph->on_evt->iface, &msg);
}

esp_err_t esp_periph_send_event(esp_periph_handle_t periph, int event_id, void *data, int data_len)
{
    if (periph->on_evt == NULL) {
        return ESP_FAIL;
    }
    audio_event_iface_msg_t msg;
    msg.source_type = periph->periph_id;
    msg.cmd = event_id;
    msg.data = data;
    msg.data_len = data_len;
    msg.need_free_data = false;
    msg.source = periph;

    if (periph->on_evt->cb) {
        periph->on_evt->cb(&msg, periph->on_evt->user_ctx);
    }
    return audio_event_iface_sendout(periph->on_evt->iface, &msg);
}

esp_err_t esp_periph_start_timer(esp_periph_handle_t periph, TickType_t interval_tick, timer_callback callback)
{
    if (periph->timer == NULL) {
        periph->timer = xTimerCreate("periph_itmer", interval_tick, pdTRUE, periph, callback);
        if (xTimerStart(periph->timer, 0) != pdTRUE) {
            AUDIO_ERROR(TAG, "Error to starting timer");
            return ESP_FAIL;
        }
    }
    return ESP_OK;
}

esp_err_t esp_periph_stop_timer(esp_periph_handle_t periph)
{
    if (periph->timer) {
        xTimerStop(periph->timer, portMAX_DELAY);
        xTimerDelete(periph->timer, portMAX_DELAY);
        periph->timer = NULL;
    }
    return ESP_OK;
}

esp_err_t esp_periph_set_data(esp_periph_handle_t periph, void *data)
{
    periph->periph_data = data;
    return ESP_OK;
}

void *esp_periph_get_data(esp_periph_handle_t periph)
{
    return periph->periph_data;
}

esp_periph_state_t esp_periph_get_state(esp_periph_handle_t periph)
{
    return periph->state;
}

esp_periph_id_t esp_periph_get_id(esp_periph_handle_t periph)
{
    return periph->periph_id;
}

esp_err_t esp_periph_set_id(esp_periph_handle_t periph, esp_periph_id_t periph_id)
{
    periph->periph_id = periph_id;
    return ESP_OK;
}

esp_err_t esp_periph_init(esp_periph_handle_t periph)
{
    return periph->init(periph);
}

esp_err_t esp_periph_run(esp_periph_handle_t periph)
{
    return periph->run(periph, NULL);
}

esp_err_t esp_periph_destroy(esp_periph_handle_t periph)
{
    return periph->destroy(periph);
}

esp_err_t esp_periph_register_on_events(esp_periph_handle_t periph, esp_periph_event_t *evts)
{
    periph->on_evt = evts;
    return ESP_OK;
}
