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
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/queue.h"
#include "freertos/task.h"

#include "esp_core_dump.h"
#include "esp_http_client.h"
#include "esp_log.h"
#include "esp_spi_flash.h"
#include "esp_system.h"

#include "audio_error.h"
#include "audio_mem.h"
#include "coredump_upload_service.h"

typedef struct {
    xQueueHandle cmd_q;
    EventGroupHandle_t sync_evt;
    bool (*do_post)(char *url, uint8_t *data, size_t len);
} coredump_upload_t;

typedef struct {
    enum {
        MSG_UPLOAD,
        MSG_DESTROY,
    } msg_id;
    void *pdata;
} coredump_msg_t;

#define COREDUMP_UPLOAD_SUCCESS (BIT0)
#define COREDUMP_UPLOAD_FAIL    (BIT1)
#define COREDUMP_UPLOAD_DESTROY (BIT2)

static char *TAG = "COREDUMP_UPLOAD";

static bool coredump_read(uint8_t **des, size_t *len)
{
    size_t addr = 0;

    if (esp_core_dump_image_get(&addr, len) != ESP_OK) {
        ESP_LOGW(TAG, "No dump info to upload");
        return false;
    }
    if (*len == 0) {
        return false;
    }
    *des = audio_calloc(1, *len);
    AUDIO_MEM_CHECK(TAG, *des, return false);
    if (spi_flash_read(addr, *des, *len) != ESP_OK) {
        ESP_LOGE(TAG, "Core dump read ERROR");
        free(*des);
        *des = NULL;
        *len = 0;
        return false;
    }

    return true;
}

static bool coredump_do_http_post(char *url, uint8_t *data, size_t len)
{
    esp_http_client_config_t config = {
        .url = url,
        .method = HTTP_METHOD_POST,
    };
    esp_http_client_handle_t http_client = esp_http_client_init(&config);
    esp_http_client_set_header(http_client, "Content-Type", "application/octet-stream");
    esp_http_client_set_post_field(http_client, (const char *)data, (int)len);
    int response = 0;
    if (esp_http_client_perform(http_client) == ESP_OK) {
        response = esp_http_client_get_status_code(http_client);
        ESP_LOGI(TAG, "HTTP GET Status = %d, content_length = %d", response, esp_http_client_get_content_length(http_client));
    } else {
        ESP_LOGE(TAG, "Post failed");
    }
    esp_http_client_cleanup(http_client);

    return response == 200 ? true : false;
}

static bool coredump_upload_partition(coredump_upload_t *uploader, char *url)
{
    uint8_t *buf = NULL;
    size_t len = 0;
    bool ret = false;
    if (coredump_read(&buf, &len) == true) {
        if (uploader->do_post(url, buf, len) == true) {
            ESP_LOGI(TAG, "core dump upload success");
            ret = true;
        } else {
            ESP_LOGE(TAG, "core dump upload failed");
        }
    }
    if (buf != NULL) {
        free(buf);
    }
    return ret;
}

static void coredump_upload_task(void *args)
{
    coredump_upload_t *uploader = periph_service_get_data((periph_service_handle_t)args);
    bool task_run = true;
    coredump_msg_t msg = { 0 };

    while (task_run) {
        if (xQueueReceive(uploader->cmd_q, &msg, portMAX_DELAY)) {
            switch (msg.msg_id) {
                case MSG_UPLOAD: {
                    bool result = coredump_upload_partition(uploader, msg.pdata);
                    xEventGroupSetBits(uploader->sync_evt, result == true ? COREDUMP_UPLOAD_SUCCESS : COREDUMP_UPLOAD_FAIL);
                    break;
                }
                case MSG_DESTROY:
                    task_run = false;
                    break;
                default:
                    break;
            }
            if (msg.pdata) {
                free(msg.pdata);
            }
        }
    }
    xEventGroupSetBits(uploader->sync_evt, COREDUMP_UPLOAD_DESTROY);
    vTaskDelete(NULL);
}

bool coredump_need_upload()
{
    bool ret = false;
    esp_reset_reason_t reset_reason = esp_reset_reason();
    ESP_LOGI(TAG, "reset reason is %d", reset_reason);
    switch (reset_reason) {
        case ESP_RST_UNKNOWN:
        case ESP_RST_PANIC:
        case ESP_RST_INT_WDT:
        case ESP_RST_TASK_WDT:
            // case ESP_RST_WDT:
            ret = true;
            break;
        default:
            ret = false;
    }
    return ret;
}

esp_err_t coredump_upload(periph_service_handle_t handle, char *url)
{
    AUDIO_MEM_CHECK(TAG, handle, return ESP_FAIL);
    coredump_upload_t *uploader = periph_service_get_data(handle);
    coredump_msg_t msg = {
        .msg_id = MSG_UPLOAD,
    };
    msg.pdata = audio_calloc(1, strlen(url) + 1);
    AUDIO_MEM_CHECK(TAG, msg.pdata, return ESP_FAIL);
    memcpy(msg.pdata, url, strlen(url));
    xQueueSend(uploader->cmd_q, &msg, portMAX_DELAY);
    EventBits_t bits = xEventGroupWaitBits(uploader->sync_evt, COREDUMP_UPLOAD_FAIL | COREDUMP_UPLOAD_SUCCESS, true, false, portMAX_DELAY);
    return (bits & COREDUMP_UPLOAD_SUCCESS) ? ESP_OK : ESP_FAIL;
}

esp_err_t coredump_upload_deinit(periph_service_handle_t handle)
{
    AUDIO_MEM_CHECK(TAG, handle, return ESP_FAIL);
    coredump_upload_t *uploader = periph_service_get_data(handle);
    coredump_msg_t msg = {
        .msg_id = MSG_DESTROY,
    };
    xQueueSend(uploader->cmd_q, &msg, portMAX_DELAY);
    xEventGroupWaitBits(uploader->sync_evt, COREDUMP_UPLOAD_DESTROY, true, true, portMAX_DELAY);
    if (uploader) {
        if (uploader->cmd_q) {
            vQueueDelete(uploader->cmd_q);
        }
        if (uploader->sync_evt) {
            vEventGroupDelete(uploader->sync_evt);
        }
        free(uploader);
    }
    ESP_LOGW(TAG, "coredump uploader destroyed");
    return ESP_OK;
}

periph_service_handle_t coredump_upload_service_create(coredump_upload_service_config_t *config)
{
    coredump_upload_t *uploader = audio_calloc(1, sizeof(coredump_upload_t));
    AUDIO_MEM_CHECK(TAG, uploader, return NULL);
    uploader->cmd_q = xQueueCreate(2, sizeof(coredump_msg_t));
    AUDIO_MEM_CHECK(TAG, uploader->cmd_q, {
        free(uploader);
        return NULL;
    });
    uploader->sync_evt = xEventGroupCreate();
    AUDIO_MEM_CHECK(TAG, uploader->sync_evt, {
        vQueueDelete(uploader->cmd_q);
        free(uploader);
        return NULL;
    });
    uploader->do_post = config->do_post != NULL ? config->do_post : coredump_do_http_post;
    periph_service_config_t cfg = {
        .task_stack = config->task_stack,
        .task_prio = config->task_prio,
        .task_core = config->task_core,
        .task_func = coredump_upload_task,
        .extern_stack = false, /* Need flash read operation */
        .service_start = NULL,
        .service_stop = NULL,
        .service_ioctl = NULL,
        .service_destroy = coredump_upload_deinit,
        .service_name = "CoreDumpUpload",
        .user_data = (void *)uploader,
    };

    periph_service_handle_t periph_coredump = periph_service_create(&cfg);
    AUDIO_MEM_CHECK(TAG, periph_coredump, {
        vQueueDelete(uploader->cmd_q);
        vEventGroupDelete(uploader->sync_evt);
        free(uploader);
        return NULL;
    });
    periph_service_set_callback(periph_coredump, config->evt_cb, config->cb_ctx);

    return periph_coredump;
}
