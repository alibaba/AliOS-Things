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
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

#include "sys/queue.h"
#include "esp_log.h"
#include "audio_element.h"
#include "audio_pipeline.h"
#include "audio_event_iface.h"
#include "audio_mem.h"
#include "audio_mutex.h"
#include "ringbuf.h"
#include "audio_error.h"

static const char *TAG = "AUDIO_PIPELINE";

#define PIPELINE_DEBUG(x) debug_pipeline_lists(x, __LINE__, __func__)

typedef struct ringbuf_item {
    STAILQ_ENTRY(ringbuf_item)  next;
    ringbuf_handle_t            rb;
    audio_element_handle_t      host_el;
    bool                        linked;
    bool                        kept_ctx;
} ringbuf_item_t;

typedef STAILQ_HEAD(ringbuf_list, ringbuf_item) ringbuf_list_t;

typedef struct audio_element_item {
    STAILQ_ENTRY(audio_element_item) next;
    audio_element_handle_t           el;
    bool                             linked;
    bool                             kept_ctx;
    audio_element_status_t           el_state;
} audio_element_item_t;

typedef STAILQ_HEAD(audio_element_list, audio_element_item) audio_element_list_t;

struct audio_pipeline {
    audio_element_list_t        el_list;
    ringbuf_list_t              rb_list;
    audio_element_state_t       state;
    xSemaphoreHandle            lock;
    bool                        linked;
    audio_event_iface_handle_t  listener;
};

static audio_element_item_t *audio_pipeline_get_el_item_by_tag(audio_pipeline_handle_t pipeline, const char *tag)
{
    audio_element_item_t *item;
    STAILQ_FOREACH(item, &pipeline->el_list, next) {
        char *el_tag = audio_element_get_tag(item->el);
        if (el_tag && strcasecmp(el_tag, tag) == 0) {
            return item;
        }
    }
    return NULL;
}

static audio_element_item_t *audio_pipeline_get_el_item_by_handle(audio_pipeline_handle_t pipeline, audio_element_handle_t el)
{
    audio_element_item_t *item;
    STAILQ_FOREACH(item, &pipeline->el_list, next) {
        if (item->el == el) {
            return item;
        }
    }
    return NULL;
}

esp_err_t audio_pipeline_change_state(audio_pipeline_handle_t pipeline, audio_element_state_t new_state)
{
    pipeline->state = new_state;
    return ESP_OK;
}

static void audio_pipeline_register_element(audio_pipeline_handle_t pipeline, audio_element_handle_t el)
{
    if (audio_pipeline_get_el_item_by_handle(pipeline, el) != NULL) {
        ESP_LOGW(TAG, "%d, %s already exist in pipeline", __LINE__, audio_element_get_tag(el));
        return;
    }
    audio_element_item_t *el_item = audio_calloc(1, sizeof(audio_element_item_t));
    AUDIO_MEM_CHECK(TAG, el_item, return);
    el_item->el = el;
    el_item->linked = true;
    STAILQ_INSERT_TAIL(&pipeline->el_list, el_item, next);
}

static void audio_pipeline_unregister_element(audio_pipeline_handle_t pipeline, audio_element_handle_t el)
{
    audio_element_item_t *el_item, *tmp;
    STAILQ_FOREACH_SAFE(el_item, &pipeline->el_list, next, tmp) {
        if (el_item->el == el) {
            STAILQ_REMOVE(&pipeline->el_list, el_item, audio_element_item, next);
            audio_free(el_item);
        }
    }
}

static void add_rb_to_audio_pipeline(audio_pipeline_handle_t pipeline, ringbuf_handle_t rb, audio_element_handle_t host_el)
{
    ringbuf_item_t *rb_item = (ringbuf_item_t *)audio_calloc(1, sizeof(ringbuf_item_t));
    AUDIO_MEM_CHECK(TAG, rb_item, return);
    rb_item->rb = rb;
    rb_item->linked = true;
    rb_item->kept_ctx = false;
    rb_item->host_el = host_el;
    STAILQ_INSERT_TAIL(&pipeline->rb_list, rb_item, next);
}

static void debug_pipeline_lists(audio_pipeline_handle_t pipeline, int line, const char *func)
{
    audio_element_item_t *el_item, *el_tmp;
    ringbuf_item_t *rb_item, *tmp;
    ESP_LOGD(TAG, "FUNC:%s, LINE:%d", func, line);
    STAILQ_FOREACH_SAFE(el_item, &pipeline->el_list, next, el_tmp) {
        ESP_LOGD(TAG, "el-list: linked:%d, kept:%d, el:%p, %16s, in_rb:%p, out_rb:%p",
                 el_item->linked, el_item->kept_ctx,
                 el_item->el, audio_element_get_tag(el_item->el),
                 audio_element_get_input_ringbuf(el_item->el),
                 audio_element_get_output_ringbuf(el_item->el));
    }
    STAILQ_FOREACH_SAFE(rb_item, &pipeline->rb_list, next, tmp) {
        ESP_LOGD(TAG, "rb-list: linked:%d, kept:%d, rb:%p, host_el:%p, %16s", rb_item->linked, rb_item->kept_ctx,
                 rb_item->rb, rb_item->host_el,
                 rb_item->host_el != NULL ? audio_element_get_tag(rb_item->host_el) : "NULL");
    }
}

audio_element_handle_t audio_pipeline_get_el_by_tag(audio_pipeline_handle_t pipeline, const char *tag)
{
    if (tag == NULL || pipeline == NULL) {
        ESP_LOGE(TAG, "Invalid parameters, tag:%p, p:%p", tag, pipeline);
        return NULL;
    }
    audio_element_item_t *item;
    STAILQ_FOREACH(item, &pipeline->el_list, next) {
        char *el_tag = audio_element_get_tag(item->el);
        ESP_LOGD(TAG, "Get_el_by_tag, el:%p, kept:%d, linked:%d el-tag:%16s, in_tag:%s",
                 item->el, item->kept_ctx, item->linked, item->el != NULL ? audio_element_get_tag(item->el) : "NULL", tag);
        if (item->kept_ctx) {
            continue;
        }
        if (el_tag && strcasecmp(el_tag, tag) == 0) {
            return item->el;
        }
    }
    return NULL;
}

audio_element_handle_t audio_pipeline_get_el_once(audio_pipeline_handle_t pipeline, const audio_element_handle_t start_el, const char *tag)
{
    if (tag == NULL || pipeline == NULL) {
        ESP_LOGE(TAG, "Invalid parameters, tag:%p, p:%p", tag, pipeline);
        return NULL;
    }
    audio_element_item_t *item;
    bool start = false;
    STAILQ_FOREACH(item, &pipeline->el_list, next) {
        if (start == false) {
            if (item->el == start_el) {
                start = true;
            }
        } else {
            char *el_tag = audio_element_get_tag(item->el);
            ESP_LOGD(TAG, "Get_el_by_el, el:%p, kept:%d, linked:%d el-tag:%16s, in_tag:%s",
                     item->el, item->kept_ctx, item->linked, item->el != NULL ? audio_element_get_tag(item->el) : "NULL", tag);
            if (item->kept_ctx) {
                continue;
            }
            if (el_tag && strcasecmp(el_tag, tag) == 0) {
                return item->el;
            }
        }
    }
    return NULL;
}


esp_err_t audio_pipeline_set_listener(audio_pipeline_handle_t pipeline, audio_event_iface_handle_t listener)
{
    audio_element_item_t *el_item;
    if (pipeline->listener) {
        audio_pipeline_remove_listener(pipeline);
    }
    STAILQ_FOREACH(el_item, &pipeline->el_list, next) {
        if (el_item->linked == false) {
            continue;
        }
        if (audio_element_msg_set_listener(el_item->el, listener) != ESP_OK) {
            ESP_LOGE(TAG, "Error register event with: %s", (char *)audio_element_get_tag(el_item->el));
            return ESP_FAIL;
        }
    }
    pipeline->listener = listener;
    return ESP_OK;
}

esp_err_t audio_pipeline_remove_listener(audio_pipeline_handle_t pipeline)
{
    audio_element_item_t *el_item;
    if (pipeline->listener == NULL) {
        ESP_LOGW(TAG, "There are no listener registered");
        return ESP_FAIL;
    }
    STAILQ_FOREACH(el_item, &pipeline->el_list, next) {
        if (el_item->linked == false) {
            continue;
        }
        if (audio_element_msg_remove_listener(el_item->el, pipeline->listener) != ESP_OK) {
            ESP_LOGE(TAG, "Error unregister event with: %s", audio_element_get_tag(el_item->el));
            return ESP_FAIL;
        }
    }
    pipeline->listener = NULL;
    return ESP_OK;
}

audio_pipeline_handle_t audio_pipeline_init(audio_pipeline_cfg_t *config)
{
    audio_pipeline_handle_t pipeline;
    bool _success =
        (
            (pipeline       = audio_calloc(1, sizeof(struct audio_pipeline)))   &&
            (pipeline->lock = mutex_create())
        );

    AUDIO_MEM_CHECK(TAG, _success, return NULL);
    STAILQ_INIT(&pipeline->el_list);
    STAILQ_INIT(&pipeline->rb_list);

    pipeline->state = AEL_STATE_INIT;
    return pipeline;
}

esp_err_t audio_pipeline_deinit(audio_pipeline_handle_t pipeline)
{
    audio_pipeline_terminate(pipeline);
    audio_pipeline_unlink(pipeline);
    audio_element_item_t *el_item, *tmp;
    STAILQ_FOREACH_SAFE(el_item, &pipeline->el_list, next, tmp) {
        ESP_LOGD(TAG, "[%16s]-[%p]element instance has been deleted", audio_element_get_tag(el_item->el), el_item->el);
        audio_element_deinit(el_item->el);
        audio_pipeline_unregister(pipeline, el_item->el);
    }
    mutex_destroy(pipeline->lock);
    audio_free(pipeline);
    return ESP_OK;
}

esp_err_t audio_pipeline_register(audio_pipeline_handle_t pipeline, audio_element_handle_t el, const char *name)
{
    audio_pipeline_unregister(pipeline, el);
    if (name) {
        audio_element_set_tag(el, name);
    }
    audio_element_item_t *el_item = audio_calloc(1, sizeof(audio_element_item_t));

    AUDIO_MEM_CHECK(TAG, el_item, return ESP_ERR_NO_MEM);
    el_item->el = el;
    el_item->linked = false;
    STAILQ_INSERT_TAIL(&pipeline->el_list, el_item, next);
    return ESP_OK;
}

esp_err_t audio_pipeline_unregister(audio_pipeline_handle_t pipeline, audio_element_handle_t el)
{
    audio_element_item_t *el_item, *tmp;
    STAILQ_FOREACH_SAFE(el_item, &pipeline->el_list, next, tmp) {
        if (el_item->el == el) {
            STAILQ_REMOVE(&pipeline->el_list, el_item, audio_element_item, next);
            audio_free(el_item);
            return ESP_OK;
        }
    }
    return ESP_FAIL;
}

esp_err_t audio_pipeline_resume(audio_pipeline_handle_t pipeline)
{
    audio_element_item_t *el_item;
    bool wait_first_el = true;
    esp_err_t ret = ESP_OK;
    STAILQ_FOREACH(el_item, &pipeline->el_list, next) {
        ESP_LOGD(TAG, "resume,linked:%d, state:%d,[%s-%p]", el_item->linked,
                 audio_element_get_state(el_item->el), audio_element_get_tag(el_item->el), el_item->el);
        if (false == el_item->linked) {
            continue;
        }
        if (wait_first_el) {
            ret |= audio_element_resume(el_item->el, 0, 2000 / portTICK_RATE_MS);
            wait_first_el = false;
        } else {
            ret |= audio_element_resume(el_item->el, 0, 2000 / portTICK_RATE_MS);
        }
    }
    audio_pipeline_change_state(pipeline, AEL_STATE_RUNNING);
    return ret;
}

esp_err_t audio_pipeline_pause(audio_pipeline_handle_t pipeline)
{
    audio_element_item_t *el_item;
    STAILQ_FOREACH(el_item, &pipeline->el_list, next) {
        if (false == el_item->linked) {
            continue;
        }
        ESP_LOGD(TAG, "pause [%s]  %p", audio_element_get_tag(el_item->el), el_item->el);
        audio_element_pause(el_item->el);
    }

    return ESP_OK;
}

esp_err_t audio_pipeline_run(audio_pipeline_handle_t pipeline)
{
    audio_element_item_t *el_item;
    if (pipeline->state != AEL_STATE_INIT) {
        ESP_LOGW(TAG, "Pipeline already started, state:%d", pipeline->state);
        return ESP_OK;
    }
    STAILQ_FOREACH(el_item, &pipeline->el_list, next) {
        ESP_LOGD(TAG, "start el[%16s], linked:%d, state:%d,[%p], ", audio_element_get_tag(el_item->el), el_item->linked,  audio_element_get_state(el_item->el), el_item->el);
        if (el_item->linked
            && ((AEL_STATE_INIT == audio_element_get_state(el_item->el))
                || (AEL_STATE_STOPPED == audio_element_get_state(el_item->el))
                || (AEL_STATE_FINISHED == audio_element_get_state(el_item->el))
                || (AEL_STATE_ERROR == audio_element_get_state(el_item->el)))) {
            audio_element_run(el_item->el);
        }
    }
    AUDIO_MEM_SHOW(TAG);

    if (ESP_FAIL == audio_pipeline_resume(pipeline)) {
        ESP_LOGE(TAG, "audio_pipeline_resume failed");
        audio_pipeline_change_state(pipeline, AEL_STATE_ERROR);
        audio_pipeline_terminate(pipeline);
        return ESP_FAIL;
    } else {
        audio_pipeline_change_state(pipeline, AEL_STATE_RUNNING);
    }

    ESP_LOGI(TAG, "Pipeline started");
    return ESP_OK;
}

esp_err_t audio_pipeline_terminate(audio_pipeline_handle_t pipeline)
{
    audio_element_item_t *el_item;
    ESP_LOGD(TAG, "Destroy audio_pipeline elements");
    STAILQ_FOREACH(el_item, &pipeline->el_list, next) {
        if (el_item->linked) {
            audio_element_terminate(el_item->el);
        }
    }
    return ESP_OK;
}

esp_err_t audio_pipeline_terminate_with_ticks(audio_pipeline_handle_t pipeline, TickType_t ticks_to_wait)
{
    audio_element_item_t *el_item;
    esp_err_t ret = ESP_OK;
    ESP_LOGD(TAG, "Destroy audio_pipeline elements with ticks[%d]", ticks_to_wait);
    STAILQ_FOREACH(el_item, &pipeline->el_list, next) {
        if (el_item->linked) {
            ret |= audio_element_terminate_with_ticks(el_item->el, ticks_to_wait);
        }
    }
    return ret;
}

esp_err_t audio_pipeline_stop(audio_pipeline_handle_t pipeline)
{
    audio_element_item_t *el_item;
    ESP_LOGD(TAG, "audio_element_stop");
    if (pipeline->state != AEL_STATE_RUNNING) {
        ESP_LOGW(TAG, "Without stop, st:%d", pipeline->state);
        return ESP_FAIL;
    }
    STAILQ_FOREACH(el_item, &pipeline->el_list, next) {
        if (el_item->linked) {
            audio_element_stop(el_item->el);
        }
    }
    return ESP_OK;
}

static inline esp_err_t __audio_pipeline_wait_stop(audio_pipeline_handle_t pipeline, TickType_t ticks_to_wait)
{
    audio_element_item_t *el_item;
    esp_err_t ret = ESP_OK;
    STAILQ_FOREACH(el_item, &pipeline->el_list, next) {
        if (el_item->linked) {
            esp_err_t res = audio_element_wait_for_stop_ms(el_item->el, ticks_to_wait);
            if (res == ESP_ERR_TIMEOUT) {
                ESP_LOGW(TAG, "Wait stop timeout, el:%p, tag:%s",
                         el_item->el, audio_element_get_tag(el_item->el) == NULL ? "NULL" : audio_element_get_tag(el_item->el));
            } else {
                audio_element_reset_state(el_item->el);
            }
            ret |= res;
        }
    }
    audio_pipeline_change_state(pipeline, AEL_STATE_INIT);
    return ret;
}

esp_err_t audio_pipeline_wait_for_stop(audio_pipeline_handle_t pipeline)
{
    if (pipeline->state != AEL_STATE_RUNNING) {
        ESP_LOGW(TAG, "Without wait stop, st:%d", pipeline->state);
        return ESP_FAIL;
    }

    ESP_LOGD(TAG, "%s - IN", __func__);
    esp_err_t ret = __audio_pipeline_wait_stop(pipeline, portMAX_DELAY);
    ESP_LOGD(TAG, "%s - OUT", __func__);
    return ret;
}

esp_err_t audio_pipeline_wait_for_stop_with_ticks(audio_pipeline_handle_t pipeline, TickType_t ticks_to_wait)
{
    if (pipeline->state != AEL_STATE_RUNNING) {
        ESP_LOGW(TAG, "Without wait stop, st:%d", pipeline->state);
        return ESP_FAIL;
    }

    ESP_LOGD(TAG, "%s - IN", __func__);
    esp_err_t ret = __audio_pipeline_wait_stop(pipeline, ticks_to_wait);
    ESP_LOGD(TAG, "%s - OUT", __func__);
    return ret;
}

static esp_err_t _pipeline_rb_linked(audio_pipeline_handle_t pipeline, audio_element_handle_t el, bool first, bool last)
{
    static ringbuf_handle_t rb;
    ringbuf_item_t *rb_item;
    if (last) {
        audio_element_set_input_ringbuf(el, rb);
    } else {
        if (!first) {
            audio_element_set_input_ringbuf(el, rb);
        }
        bool _success = (
                            (rb_item = audio_calloc(1, sizeof(ringbuf_item_t))) &&
                            (rb = rb_create(audio_element_get_output_ringbuf_size(el), 1))
                        );

        AUDIO_MEM_CHECK(TAG, _success, {
            audio_free(rb_item);
            return ESP_ERR_NO_MEM;
        });

        rb_item->rb = rb;
        rb_item->linked = true;
        rb_item->kept_ctx = false;
        rb_item->host_el = el;
        STAILQ_INSERT_TAIL(&pipeline->rb_list, rb_item, next);
        audio_element_set_output_ringbuf(el, rb);
        ESP_LOGI(TAG, "link el->rb, el:%p, tag:%s, rb:%p", el, audio_element_get_tag(el) == NULL ? "NULL" : audio_element_get_tag(el), rb);
    }
    return ESP_OK;
}

esp_err_t audio_pipeline_link(audio_pipeline_handle_t pipeline, const char *link_tag[], int link_num)
{
    esp_err_t ret = ESP_OK;
    bool first = false, last = false;
    if (pipeline->linked) {
        audio_pipeline_unlink(pipeline);
    }
    for (int i = 0; i < link_num; i++) {
        audio_element_item_t *item = audio_pipeline_get_el_item_by_tag(pipeline, link_tag[i]);
        if (item == NULL) {
            ESP_LOGE(TAG, "There is 1 link_tag invalid: %s", link_tag[i]);
            return ESP_FAIL;
        }
        item->linked = true;
        item->kept_ctx = false;
        audio_element_handle_t el = item->el;
        first = (i == 0);
        last = (i == link_num - 1);
        ret = _pipeline_rb_linked(pipeline, el, first, last);
        if (ret != ESP_OK) {
            return ret;
        }
    }
    pipeline->linked = true;
    PIPELINE_DEBUG(pipeline);
    return ESP_OK;
}

esp_err_t audio_pipeline_unlink(audio_pipeline_handle_t pipeline)
{
    audio_element_item_t *el_item;
    ringbuf_item_t *rb_item, *tmp;
    if (!pipeline->linked) {
        return ESP_OK;
    }
    audio_pipeline_remove_listener(pipeline);
    STAILQ_FOREACH(el_item, &pipeline->el_list, next) {
        if (el_item->linked) {
            el_item->linked = false;
            el_item->kept_ctx = false;
            audio_element_set_output_ringbuf(el_item->el, NULL);
            audio_element_set_input_ringbuf(el_item->el, NULL);
            ESP_LOGD(TAG, "audio_pipeline_unlink, %p, %s", el_item->el, audio_element_get_tag(el_item->el));
        }
    }
    STAILQ_FOREACH_SAFE(rb_item, &pipeline->rb_list, next, tmp) {
        ESP_LOGD(TAG, "audio_pipeline_unlink, RB:%p,host_el:%p", rb_item->rb, rb_item->host_el);
        STAILQ_REMOVE(&pipeline->rb_list, rb_item, ringbuf_item, next);
        if (rb_item->host_el) {
            audio_element_set_output_ringbuf(rb_item->host_el, NULL);
            audio_element_set_input_ringbuf(rb_item->host_el, NULL);
        }
        rb_destroy(rb_item->rb);
        rb_item->linked = false;
        rb_item->kept_ctx = false;
        rb_item->host_el = NULL;
        audio_free(rb_item);
    }
    ESP_LOGI(TAG, "audio_pipeline_unlinked");
    STAILQ_INIT(&pipeline->rb_list);
    pipeline->linked = false;
    return ESP_OK;
}

esp_err_t audio_pipeline_register_more(audio_pipeline_handle_t pipeline, audio_element_handle_t element_1, ...)
{
    va_list args;
    va_start(args, element_1);
    while (element_1) {
        audio_pipeline_register_element(pipeline, element_1);
        element_1 = va_arg(args, audio_element_handle_t);
    }
    va_end(args);
    return ESP_OK;
}

esp_err_t audio_pipeline_unregister_more(audio_pipeline_handle_t pipeline, audio_element_handle_t element_1, ...)
{
    va_list args;
    va_start(args, element_1);
    while (element_1) {
        audio_pipeline_unregister_element(pipeline, element_1);
        element_1 = va_arg(args, audio_element_handle_t);
    }
    va_end(args);
    return ESP_OK;
}

esp_err_t audio_pipeline_link_more(audio_pipeline_handle_t pipeline, audio_element_handle_t element_1, ...)
{
    va_list args;
    esp_err_t ret = ESP_OK;
    int idx = 0;
    bool first = false;
    bool last = false;
    if (pipeline->linked) {
        audio_pipeline_unlink(pipeline);
    }
    va_start(args, element_1);
    while (element_1) {
        audio_element_handle_t el = element_1;
        audio_element_item_t *item = audio_pipeline_get_el_item_by_handle(pipeline, element_1);
        if (item == NULL) {
            ESP_LOGE(TAG, "Can't found element[%p-%s] item", element_1, audio_element_get_tag(element_1));
            return ESP_FAIL;
        }
        item->linked = true;
        item->kept_ctx = false;
        idx ++;
        first = (idx == 1);
        element_1 = va_arg(args, audio_element_handle_t);
        last = (NULL == element_1) ? true : false;
        ret = _pipeline_rb_linked(pipeline, el, first, last);
        if (ret != ESP_OK) {
            return ret;
        }
    }
    pipeline->linked = true;
    va_end(args);
    return ESP_OK;
}

esp_err_t audio_pipeline_link_insert(audio_pipeline_handle_t pipeline, bool first, audio_element_handle_t prev, ringbuf_handle_t conect_rb, audio_element_handle_t next)
{
    if (first) {
        audio_pipeline_register_element(pipeline, prev);
    }
    ESP_LOGD(TAG, "element is prev:%p, rb:%p, next:%p", prev, conect_rb, next);
    audio_pipeline_register_element(pipeline, next);
    audio_element_set_output_ringbuf(prev, conect_rb);
    audio_element_set_input_ringbuf(next, conect_rb);
    add_rb_to_audio_pipeline(pipeline, conect_rb, prev);
    pipeline->linked = true;
    return ESP_OK;
}

esp_err_t audio_pipeline_listen_more(audio_pipeline_handle_t pipeline, audio_element_handle_t element_1, ...)
{
    va_list args;
    va_start(args, element_1);
    while (element_1) {
        audio_element_handle_t el = element_1;
        element_1 = va_arg(args, audio_element_handle_t);
        QueueHandle_t que = audio_element_get_event_queue(el);
        audio_event_iface_msg_t dummy = {0};
        while (1) {
            if (xQueueReceive(que, &dummy, 0) == pdTRUE) {
                ESP_LOGD(TAG, "Listen_more el:%p, que :%p, OK", el, que);
            } else {
                ESP_LOGD(TAG, "Listen_more el:%p, que :%p, FAIL", el, que);
                break;
            }
        }
    }
    va_end(args);
    PIPELINE_DEBUG(pipeline);
    return ESP_OK;
}

esp_err_t audio_pipeline_check_items_state(audio_pipeline_handle_t pipeline, audio_element_handle_t el, audio_element_status_t status)
{
    audio_element_item_t *item;
    int el_cnt = 0;
    int el_sta_cnt = 0;
    audio_element_item_t *it = audio_pipeline_get_el_item_by_handle(pipeline, el);
    it->el_state =  status;
    STAILQ_FOREACH(item, &pipeline->el_list, next) {
        if (false == item->linked) {
            continue;
        }
        el_cnt ++;
        ESP_LOGV(TAG, "pipeline state check, pl:%p, el:%p, tag:%16s, state:%d, status:%d", pipeline, item->el,
                 audio_element_get_tag(item->el), item->el_state, status);
        int st = audio_element_get_state(item->el);
        if ((st == AEL_STATE_STOPPED)
            || (st == AEL_STATE_FINISHED)
            || (st == AEL_STATE_ERROR)) {
            ESP_LOGV(TAG, "Element rewrite, tag:%16s, el-state:%d, it->el_state:%d, wanted:%d",
                     audio_element_get_tag(item->el), st, it->el_state, status);
            if (st == AEL_STATE_ERROR) {
                item->el_state = AEL_STATUS_ERROR_PROCESS;
            } else {
                item->el_state = st + AEL_STATUS_INPUT_BUFFERING;
            }
        }
        if (item->el_state == AEL_STATUS_NONE) {
            continue;
        }
        if (status == item->el_state) {
            el_sta_cnt++;
        } else if ((status == AEL_STATUS_STATE_RUNNING)) {
            if ((item->el_state == AEL_STATUS_STATE_FINISHED)
                || ((item->el_state > AEL_STATUS_NONE) && (item->el_state < AEL_STATUS_INPUT_DONE))) {
                el_sta_cnt++;
                ESP_LOGW(TAG, "Check AEL RUNNING, pl:%p, el:%p, tag:%16s, state:%d, wanted:%d", pipeline, item->el,
                         audio_element_get_tag(item->el), item->el_state, status);
            }
        } else if (status == AEL_STATUS_STATE_PAUSED) {
            if ((item->el_state == AEL_STATUS_STATE_FINISHED)
                || ((item->el_state > AEL_STATUS_NONE) && (item->el_state < AEL_STATUS_INPUT_DONE))) {
                el_sta_cnt++;
                ESP_LOGW(TAG, "Check AEL PAUSED, pl:%p, el:%p, tag:%16s, state:%d, wanted:%d", pipeline, item->el,
                         audio_element_get_tag(item->el), item->el_state, status);
            }
        } else if (status == AEL_STATUS_STATE_STOPPED) {
            if ((item->el_state == AEL_STATUS_STATE_FINISHED)
                || ((item->el_state > AEL_STATUS_NONE) && (item->el_state < AEL_STATUS_INPUT_DONE))) {
                el_sta_cnt++;
                ESP_LOGW(TAG, "Check AEL STOPPED, pl:%p, el:%p, tag:%16s, state:%d, wanted:%d", pipeline, item->el,
                         audio_element_get_tag(item->el), item->el_state, status);
            }
        } else if (status == AEL_STATUS_STATE_FINISHED) {
            if ((item->el_state == AEL_STATUS_STATE_STOPPED)
                || ((item->el_state > AEL_STATUS_NONE) && (item->el_state < AEL_STATUS_INPUT_DONE))) {
                el_sta_cnt++;
                ESP_LOGW(TAG, "Check AEL FINISHED, pl:%p, el:%p, tag:%16s, state:%d, wanted:%d", pipeline, item->el,
                         audio_element_get_tag(item->el), item->el_state, status);
            }
        } else {
            // TODO nothing
        }
    }
    if (el_cnt && (el_sta_cnt == el_cnt)) {
        return ESP_OK;
    } else {
        return ESP_FAIL;
    }
}

esp_err_t audio_pipeline_reset_items_state(audio_pipeline_handle_t pipeline)
{
    audio_element_item_t *el_item;
    ESP_LOGD(TAG, "audio_pipeline_reset_items_state");
    STAILQ_FOREACH(el_item, &pipeline->el_list, next) {
        if (el_item->linked) {
            el_item->el_state = AEL_STATUS_NONE;
        }
    }
    return ESP_OK;
}

esp_err_t audio_pipeline_reset_ringbuffer(audio_pipeline_handle_t pipeline)
{
    audio_element_item_t *el_item;
    STAILQ_FOREACH(el_item, &pipeline->el_list, next) {
        if (el_item->linked) {
            audio_element_reset_input_ringbuf(el_item->el);
            audio_element_reset_output_ringbuf(el_item->el);
        }
    }
    return ESP_OK;
}

esp_err_t audio_pipeline_reset_elements(audio_pipeline_handle_t pipeline)
{
    audio_element_item_t *el_item;
    STAILQ_FOREACH(el_item, &pipeline->el_list, next) {
        if (el_item->linked) {
            audio_element_reset_state(el_item->el);
        }
    }
    return ESP_OK;
}

esp_err_t audio_pipeline_reset_kept_state(audio_pipeline_handle_t pipeline, audio_element_handle_t el)
{
    audio_element_item_t *el_item;
    STAILQ_FOREACH(el_item, &pipeline->el_list, next) {
        if (el_item->el == el) {
            el_item->linked = false;
            el_item->kept_ctx = false;
            break;
        }
    }
    ringbuf_handle_t rb = audio_element_get_output_ringbuf(el);
    audio_element_set_output_ringbuf(el, NULL);
    ringbuf_item_t *rb_item, *tmp;
    STAILQ_FOREACH_SAFE(rb_item, &pipeline->rb_list, next, tmp) {
        if (rb_item->rb == rb) {
            rb_item->linked = false;
            rb_item->kept_ctx = false;
            rb_item->host_el = NULL;
            ESP_LOGW(TAG, "kept_reset, rb:%p", rb);
            break;
        }
    }
    pipeline->linked = false;
    return ESP_OK;
}

esp_err_t audio_pipeline_breakup_elements(audio_pipeline_handle_t pipeline, audio_element_handle_t kept_ctx_el)
{
    if (pipeline == NULL) {
        ESP_LOGE(TAG, "%s have invalid args, %p", __func__, pipeline);
        return ESP_ERR_INVALID_ARG;
    }
    audio_pipeline_remove_listener(pipeline);
    audio_element_item_t *el_item, *el_tmp;
    ringbuf_item_t *rb_item, *tmp;
    bool kept = true;
    ESP_LOGD(TAG, "audio_pipeline_breakup_elements IN,%p,%s", kept_ctx_el, kept_ctx_el != NULL ? audio_element_get_tag(kept_ctx_el) : "NULL");
    STAILQ_FOREACH_SAFE(el_item, &pipeline->el_list, next, el_tmp) {
        ESP_LOGD(TAG, "%d, el:%08x, %s, in_rb:%08x, out_rb:%08x, linked:%d, el-kept:%d", __LINE__,
                 (int)el_item->el, audio_element_get_tag(el_item->el),
                 (int)audio_element_get_input_ringbuf(el_item->el),
                 (int)audio_element_get_output_ringbuf(el_item->el),
                 el_item->linked,
                 el_item->kept_ctx);
        if (!el_item->linked) {
            continue;
        }
        if ((!kept) && el_item->el != kept_ctx_el) {
            audio_element_reset_state(el_item->el);
            STAILQ_FOREACH_SAFE(rb_item, &pipeline->rb_list, next, tmp) {
                if (audio_element_get_output_ringbuf(el_item->el) == NULL) {
                    el_item->linked = false;
                    el_item->kept_ctx = false;
                    audio_element_set_input_ringbuf(el_item->el, NULL);
                    ESP_LOGD(TAG, "output ringbuf is null");
                    break;
                } else if (rb_item->rb == audio_element_get_output_ringbuf(el_item->el)) {
                    el_item->linked = false;
                    el_item->kept_ctx = false;
                    rb_item->kept_ctx = false;
                    rb_item->linked = false;
                    rb_item->host_el = NULL;
                    audio_element_set_output_ringbuf(el_item->el, NULL);
                    audio_element_set_input_ringbuf(el_item->el, NULL);
                    ESP_LOGD(TAG, "found output ringbuf, %p", el_item->el);
                    break;
                }
                audio_element_set_output_ringbuf(el_item->el, NULL);
                audio_element_set_input_ringbuf(el_item->el, NULL);
            }
        } else {
            STAILQ_FOREACH_SAFE(rb_item, &pipeline->rb_list, next, tmp) {
                if (rb_item->rb == audio_element_get_output_ringbuf(el_item->el)) {
                    el_item->linked = false;
                    if ((audio_element_get_state(el_item->el) == AEL_STATE_RUNNING)
                        || (audio_element_get_state(el_item->el) == AEL_STATE_PAUSED)
                        /*|| (audio_element_get_state(el_item->el) == AEL_STATE_FINISHED)*/) {
                        el_item->kept_ctx = true;
                        rb_item->kept_ctx = true;
                        ESP_LOGD(TAG, "found kept_ctx_el:%p and ringbuf:%p", el_item->el, rb_item->rb);
                    } else {
                        ESP_LOGW(TAG, "found kept_ctx_el and ringbuf, but not set kept, el:%p, rb:%p", el_item->el, rb_item->rb);
                        audio_element_set_output_ringbuf(el_item->el, NULL);
                    }
                    kept = false;
                    audio_element_set_input_ringbuf(el_item->el, NULL);
                    break;
                }
            }
        }

    }
    // For Debug
    PIPELINE_DEBUG(pipeline);
    STAILQ_FOREACH_SAFE(rb_item, &pipeline->rb_list, next, tmp) {
        if (rb_item->kept_ctx == false) {
            rb_item->linked = false;
            rb_reset(rb_item->rb);
        }
        ESP_LOGD(TAG, "%d, reset rb:%p kept:%d,linked:%d", __LINE__, rb_item->rb, rb_item->kept_ctx, rb_item->linked );
    }
    pipeline->linked = false;
    audio_pipeline_change_state(pipeline, AEL_STATE_INIT);
    return ESP_OK;
}

static esp_err_t audio_pipeline_el_item_link(audio_pipeline_handle_t pipeline,
        audio_element_item_t *src_el_item,
        audio_element_handle_t el, bool first, bool last)
{
    ringbuf_item_t *cur_rb_item = NULL;
    ringbuf_item_t *rb_item, *rb_tmp;
    static ringbuf_handle_t rb;
    // Found the kept ringbuffer if exist
    if (src_el_item->kept_ctx) {
        STAILQ_FOREACH_SAFE(rb_item, &pipeline->rb_list, next, rb_tmp) {
            ESP_LOGD(TAG, "%d, rb:%p host_el:%p kept:%d,linked:%d", __LINE__, rb_item->rb, rb_item->host_el, rb_item->kept_ctx, rb_item->linked);
            if (rb_item->host_el == el) {
                cur_rb_item = rb_item;
                cur_rb_item->linked = true;
                cur_rb_item->kept_ctx = false;
                cur_rb_item->host_el = el;
                src_el_item->kept_ctx = false;
                ESP_LOGD(TAG, "found kept rb:%p kept:%d,linked:%d, el:%p, name:%s", rb_item->rb, rb_item->kept_ctx,
                         rb_item->linked, src_el_item->el, audio_element_get_tag(src_el_item->el));
                break;
            }
        }
    } else {
        STAILQ_FOREACH_SAFE(rb_item, &pipeline->rb_list, next, rb_tmp) {
            ESP_LOGD(TAG, "%d, rb:%p host_el:%p kept:%d,linked:%d", __LINE__, rb_item->rb, rb_item->host_el, rb_item->kept_ctx, rb_item->linked);
            if ((rb_item->linked == false)
                && (rb_item->kept_ctx == false)) {
                if (rb_item->host_el == NULL) {
                    cur_rb_item = rb_item;
                    cur_rb_item->linked = true;
                    cur_rb_item->kept_ctx = false;
                    cur_rb_item->host_el = el;
                    rb_reset(cur_rb_item->rb);
                    ESP_LOGD(TAG, "%d, found not used rb:%p kept:%d,linked:%d", __LINE__, rb_item->rb, rb_item->kept_ctx, rb_item->linked);
                    break;
                }
                rb_item->host_el = NULL;
            }
        }
    }
    if ((last == false) && (cur_rb_item == NULL)) {
        ringbuf_handle_t tmp_rb = NULL;
        bool _success = (
                            (cur_rb_item = audio_calloc(1, sizeof(ringbuf_item_t))) &&
                            (tmp_rb = rb_create(audio_element_get_output_ringbuf_size(el), 1))
                        );

        AUDIO_MEM_CHECK(TAG, _success, {
            audio_free(cur_rb_item);
            return ESP_ERR_NO_MEM;
        });
        cur_rb_item->rb = tmp_rb;
        cur_rb_item->linked = true;
        cur_rb_item->kept_ctx = false;
        cur_rb_item->host_el = el;
        STAILQ_INSERT_TAIL(&pipeline->rb_list, cur_rb_item, next);
        ESP_LOGI(TAG, "create new rb,rb:%p",  cur_rb_item->rb);
    }
    ESP_LOGD(TAG, "%d, el:%p, tag:%s, cur_rb_item:%p, rb:%p, first:%d, last:%d\r\n", __LINE__, el,
             audio_element_get_tag(el), cur_rb_item, cur_rb_item != NULL ? cur_rb_item->rb : NULL, first, last);
    if (last) {
        audio_element_set_input_ringbuf(el, rb);
    } else {
        if (!first) {
            audio_element_set_input_ringbuf(el, rb);
        }
        rb = cur_rb_item->rb;
        audio_element_set_output_ringbuf(el, rb);
    }
    return ESP_OK;
}

esp_err_t audio_pipeline_relink(audio_pipeline_handle_t pipeline, const char *link_tag[], int link_num)
{
    if (pipeline == NULL
        || link_tag == NULL) {
        ESP_LOGE(TAG, "%s have invalid args, %p, %p", __func__, pipeline, link_tag);
        return ESP_ERR_INVALID_ARG;
    }
    if (pipeline->linked) {
        ESP_LOGE(TAG, "%s pipeline is already linked, can't relink", __func__);
        return ESP_FAIL;
    }
    esp_err_t ret = ESP_OK;
    audio_element_item_t *el_item, *el_tmp;
    bool first = false, last = false;
    for (int i = 0; i < link_num; i++) {
        audio_element_item_t *src_el_item = audio_pipeline_get_el_item_by_tag(pipeline, link_tag[i]);
        if (src_el_item == NULL) {
            ESP_LOGE(TAG, "There is link_tag invalid: %s", link_tag[i]);
            ret = ESP_FAIL;
            goto relink_err;
        }
        audio_element_handle_t el = NULL;
        STAILQ_FOREACH_SAFE(el_item, &pipeline->el_list, next, el_tmp) {
            ESP_LOGD(TAG, "%d, linked:%d, kept:%d, el:%s, el:%p, tag:%s, target-el:%p", __LINE__, el_item->linked, el_item->kept_ctx,
                     audio_element_get_tag(el_item->el), el_item->el, link_tag[i], src_el_item->el);
            if (src_el_item->el == el_item->el) {
                el = el_item->el;
                break;
            }
        }
        if (el == NULL) {
            ESP_LOGE(TAG, "Can't find element, wanted_el:%s", link_tag[i]);
            ret = ESP_FAIL;
            goto relink_err;
        }
        src_el_item->linked = true;
        first = (i == 0);
        last = (i == link_num - 1);
        audio_pipeline_el_item_link(pipeline, src_el_item, el, first, last);
    }
    pipeline->linked = true;
    PIPELINE_DEBUG(pipeline);
relink_err:
    return ret;
}

esp_err_t audio_pipeline_relink_more(audio_pipeline_handle_t pipeline, audio_element_handle_t element_1, ...)
{
    AUDIO_NULL_CHECK(TAG, (pipeline || element_1), return ESP_ERR_INVALID_ARG);
    if (pipeline->linked) {
        ESP_LOGE(TAG, "%s pipeline is already linked, can't relink", __func__);
        return ESP_FAIL;
    }
    va_list args;
    audio_element_item_t *el_item, *el_tmp;
    va_start(args, element_1);
    bool first = false, last = false;
    uint16_t idx = 0;
    while (element_1) {
        audio_element_item_t *src_el_item = audio_pipeline_get_el_item_by_handle(pipeline, element_1);
        AUDIO_NULL_CHECK(TAG, src_el_item, return ESP_FAIL);

        audio_element_handle_t el = NULL;
        STAILQ_FOREACH_SAFE(el_item, &pipeline->el_list, next, el_tmp) {
            ESP_LOGD(TAG, "%d, linked:%d, kept:%d, el:%s, el:%p, tag:%s, target-el:%p", __LINE__, el_item->linked, el_item->kept_ctx,
                     audio_element_get_tag(el_item->el), el_item->el, audio_element_get_tag(src_el_item->el), src_el_item->el);
            if (src_el_item->el == el_item->el) {
                el = el_item->el;
                break;
            }
        }
        if (el == NULL) {
            ESP_LOGE(TAG, "Can't find element, wanted_el:%s", audio_element_get_tag(src_el_item->el));
            return ESP_FAIL;
        }
        src_el_item->linked = true;
        idx ++;
        first = (idx == 1);
        element_1 = va_arg(args, audio_element_handle_t);
        last = (NULL == element_1) ? true : false;
        audio_pipeline_el_item_link(pipeline, src_el_item, el, first, last);
    }
    pipeline->linked = true;
    PIPELINE_DEBUG(pipeline);
    va_end(args);
    return ESP_OK;
}
