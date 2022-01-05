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

#include "esp_log.h"
#include "driver/sdmmc_host.h"
#include "driver/sdmmc_defs.h"
#include "driver/gpio.h"
#include "sdcard.h"
#include "periph_sdcard.h"
#include "audio_mem.h"

static const char *TAG = "PERIPH_SDCARD";

#define SDCARD_CHECK_TIMEOUT_MS (20)

#define VALIDATE_SDCARD(periph, ret) if (!(periph && esp_periph_get_id(periph) == PERIPH_ID_SDCARD)) { \
    ESP_LOGE(TAG, "Invalid SDCARD periph, at line %d", __LINE__);\
    return ret;\
}

#define tick_get periph_tick_get


static esp_err_t periph_sdcard_mount(esp_periph_handle_t periph);
static esp_err_t periph_sdcard_unmount(esp_periph_handle_t periph);


typedef struct {
    char *root;
    int card_detect_pin;
    bool is_mounted;
    long long last_detect_time;
    periph_sdcard_mode_t sd_mode;
} periph_sdcard_t;


static void IRAM_ATTR sdcard_gpio_intr_handler(void *param)
{
    esp_periph_handle_t periph = (esp_periph_handle_t)param;
    periph_sdcard_t *sdcard = esp_periph_get_data(periph);

    if (sdcard_is_exist() && !sdcard->is_mounted) {
        esp_periph_send_cmd_from_isr(periph, SDCARD_STATUS_CARD_DETECT_CHANGE, NULL, 0);
    } else if (!sdcard_is_exist() && sdcard->is_mounted) {
        esp_periph_send_cmd_from_isr(periph, SDCARD_STATUS_CARD_DETECT_CHANGE, NULL, 0);
    }
}

static esp_err_t _sdcard_run(esp_periph_handle_t self, audio_event_iface_msg_t *msg)
{
    if (msg->cmd != SDCARD_STATUS_CARD_DETECT_CHANGE) {
        return ESP_OK;
    }
    periph_sdcard_t *sdcard = esp_periph_get_data(self);
    if (sdcard_is_exist() && !sdcard->is_mounted) {
        periph_sdcard_mount(self);
    } else if (!sdcard_is_exist() && sdcard->is_mounted) {
        periph_sdcard_unmount(self);
    }
    return ESP_OK;
}

static void sdcard_timer_handler(xTimerHandle tmr)
{
    esp_periph_handle_t periph = (esp_periph_handle_t) pvTimerGetTimerID(tmr);
    esp_periph_send_cmd(periph, SDCARD_STATUS_CARD_DETECT_CHANGE, NULL, 0);
}

static esp_err_t _sdcard_init(esp_periph_handle_t self)
{
    periph_sdcard_t *sdcard = esp_periph_get_data(self);
    esp_err_t ret = sdcard_init(sdcard->card_detect_pin, sdcard_gpio_intr_handler, self);
    if (sdcard_is_exist()) {
        ret |= periph_sdcard_mount(self);
    } else {
        ESP_LOGE(TAG, "no sdcard detect");
    }
    esp_periph_start_timer(self, 1000 / portTICK_RATE_MS, sdcard_timer_handler);
    return ESP_OK;
}

static esp_err_t _sdcard_destroy(esp_periph_handle_t self)
{
    VALIDATE_SDCARD(self, ESP_FAIL);
    esp_err_t ret = ESP_OK;

    ret |= sdcard_unmount();
    ret |= sdcard_destroy();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "stop sdcard error!");
    }
    periph_sdcard_t *sdcard = esp_periph_get_data(self);
    audio_free(sdcard->root);
    audio_free(sdcard);
    return ret;
}

esp_err_t periph_sdcard_mount(esp_periph_handle_t periph)
{
    VALIDATE_SDCARD(periph, ESP_FAIL);

    periph_sdcard_t *sdcard = esp_periph_get_data(periph);

    int ret = sdcard_mount(sdcard->root, sdcard->sd_mode);
    if (ret == ESP_OK) {
        ESP_LOGD(TAG, "Mount SDCARD success");
        sdcard->is_mounted = true;
        return esp_periph_send_event(periph, SDCARD_STATUS_MOUNTED, NULL, 0);
    } else if (ret == ESP_ERR_INVALID_STATE) {
        ESP_LOGD(TAG, "periph sdcard handle already mounted!");
        return ESP_OK;
    } else {
        esp_periph_send_event(periph, SDCARD_STATUS_MOUNT_ERROR, NULL, 0);
        sdcard->is_mounted = false;
        ESP_LOGE(TAG, "mount sdcard error!");
        return ESP_FAIL;
    }
}

esp_err_t periph_sdcard_unmount(esp_periph_handle_t periph)
{
    VALIDATE_SDCARD(periph, ESP_FAIL);
    periph_sdcard_t *sdcard = esp_periph_get_data(periph);
    int ret = sdcard_unmount();
    if (ret == ESP_OK) {
        ESP_LOGD(TAG, "UnMount SDCARD success");
        sdcard->is_mounted = false;
        return esp_periph_send_event(periph, SDCARD_STATUS_UNMOUNTED, NULL, 0);
    } else {
        esp_periph_send_event(periph, SDCARD_STATUS_UNMOUNT_ERROR, NULL, 0);
        ESP_LOGE(TAG, "unmount sdcard error!");
        sdcard->is_mounted = false;
        return ESP_FAIL;
    }
    return ESP_OK;
}

esp_periph_handle_t periph_sdcard_init(periph_sdcard_cfg_t *sdcard_cfg)
{
    esp_periph_handle_t periph = esp_periph_create(PERIPH_ID_SDCARD, "periph_sdcard");
    AUDIO_MEM_CHECK(TAG, periph, return NULL);

    periph_sdcard_t *sdcard = audio_calloc(1, sizeof(periph_sdcard_t));
    AUDIO_MEM_CHECK(TAG, sdcard, {
        audio_free(periph);
        return NULL;
    });

    if (sdcard_cfg->root) {
        sdcard->root = audio_strdup(sdcard_cfg->root);
    } else {
        sdcard->root = audio_strdup("/sdcard");
    }
    AUDIO_MEM_CHECK(TAG, sdcard->root, {
        audio_free(sdcard);
        audio_free(periph);
        return NULL;
    });

    sdcard->card_detect_pin = sdcard_cfg->card_detect_pin;
    sdcard->sd_mode = sdcard_cfg->mode;
    esp_periph_set_data(periph, sdcard);
    esp_periph_set_function(periph, _sdcard_init, _sdcard_run, _sdcard_destroy);
    return periph;
}

bool periph_sdcard_is_mounted(esp_periph_handle_t periph)
{
    VALIDATE_SDCARD(periph, ESP_FAIL);
    periph_sdcard_t *sdcard = esp_periph_get_data(periph);
    return sdcard->is_mounted;
}
