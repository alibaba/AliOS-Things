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

#include <string.h>
#include "audio_error.h"
#include "audio_mem.h"
#include "flash_list.h"
#include "nvs_flash.h"

#define DEFAULT_NVS_NAME_SPACE    "NVS"
#define NVS_NAME_SPACE_MAX_LENGTH  16
#define NVS_FLASH_URL_MAX_LENGTH   2048

static const char *TAG = "FLASH_LIST";

/**
 * @brief Nvs flash list management unit
 */
typedef struct flash_list {
    char *name_space;            /*!< nvs name space */
    nvs_handle url_nvs_handle;   /*!< nvs handle */
    uint16_t url_num;            /*!< number of URLs */
    int16_t cur_url_id;          /*!< current URL id */
    char *cur_url;               /*!< point to current URL */
} flash_list_t;

esp_err_t flash_list_get_operation(playlist_operation_t *operation);

static esp_err_t flash_list_choose_id(flash_list_t *playlist, int id, char **url_buff)
{
    esp_err_t ret = ESP_OK;

    if (playlist->cur_url) {
        audio_free(playlist->cur_url);
    }

    playlist->cur_url = (char *)audio_calloc(1, NVS_FLASH_URL_MAX_LENGTH);
    AUDIO_NULL_CHECK(TAG, playlist->cur_url, {
        ESP_LOGE(TAG, "Allocate memory failed!");
        return ESP_FAIL;
    });

    size_t len = NVS_FLASH_URL_MAX_LENGTH;
    ret = nvs_get_str(playlist->url_nvs_handle, (const char *)&id, playlist->cur_url, &len);

    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Flash list choose url id failed");
        return ESP_FAIL;
    }

    *url_buff = playlist->cur_url;
    playlist->cur_url_id = id;
    return ESP_OK;
}

esp_err_t flash_list_create(playlist_operator_handle_t *handle)
{
    AUDIO_NULL_CHECK(TAG, handle, return ESP_FAIL);
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES) {
        // NVS partition was truncated and needs to be erased
        // Retry nvs_flash_init
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }

    static int list_id;
    playlist_operator_handle_t flash_handle = (playlist_operator_handle_t )audio_calloc(1, sizeof(playlist_operator_t));
    AUDIO_NULL_CHECK(TAG, flash_handle, return ESP_FAIL);

    flash_list_t *flash_list = (flash_list_t *)audio_calloc(1, sizeof(flash_list_t));
    AUDIO_NULL_CHECK(TAG, flash_list, {
        audio_free(flash_handle);
        return ESP_FAIL;
    });

    flash_handle->playlist = flash_list;
    flash_handle->get_operation = flash_list_get_operation;

    flash_list->name_space = audio_calloc(1, NVS_NAME_SPACE_MAX_LENGTH);
    AUDIO_NULL_CHECK(TAG, flash_list, {
        audio_free(flash_handle);
        audio_free(flash_list);
        return ESP_FAIL;
    });
    sprintf(flash_list->name_space, "%s%d", DEFAULT_NVS_NAME_SPACE, list_id++);
    ret |= nvs_open(flash_list->name_space, NVS_READWRITE, &flash_list->url_nvs_handle);

    if (ret != ESP_OK) {
        audio_free(flash_handle);
        audio_free(flash_list->name_space);
        audio_free(flash_list);
        ESP_LOGE(TAG, "Flash playlist open failed, please check");
    }

    *handle = flash_handle;
    return ESP_OK;
}

esp_err_t flash_list_save(playlist_operator_handle_t handle, const char *url)
{
    esp_err_t ret = ESP_OK;
    AUDIO_NULL_CHECK(TAG, handle, return ESP_FAIL);
    AUDIO_NULL_CHECK(TAG, url, return ESP_FAIL);
    if (strlen(url) >= NVS_FLASH_URL_MAX_LENGTH - 1) {
        ESP_LOGE(TAG, "The URL is too long to save, please change the NVS_FLASH_URL_MAX_LENGTH and retry");
        return ESP_FAIL;
    }
    flash_list_t *playlist = handle->playlist;
    AUDIO_NULL_CHECK(TAG, playlist, return ESP_FAIL);

    ret |= nvs_set_str(playlist->url_nvs_handle, (const char *)&playlist->url_num, url);
    ret |= nvs_commit(playlist->url_nvs_handle);

    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Flash save url failed, URL: %s, ret: %d", url, ret);
        return ESP_FAIL;
    }

    playlist->url_num ++;
    return ESP_OK;
}

esp_err_t flash_list_show(playlist_operator_handle_t handle)
{
    esp_err_t ret = ESP_OK;
    AUDIO_NULL_CHECK(TAG, handle, return ESP_FAIL);
    flash_list_t *playlist = handle->playlist;
    AUDIO_NULL_CHECK(TAG, playlist, return ESP_FAIL);

    size_t len = NVS_FLASH_URL_MAX_LENGTH;
    char *out_str = audio_calloc(1, NVS_FLASH_URL_MAX_LENGTH);
    for (int i = 0; i < playlist->url_num; i++) {
        memset(out_str, 0, NVS_FLASH_URL_MAX_LENGTH);
        len = NVS_FLASH_URL_MAX_LENGTH;
        ret = nvs_get_str(playlist->url_nvs_handle, (const char *)&i, out_str, &len);
        ESP_LOGI(TAG, "ID:%d 	URL: %s", i, out_str);
    }
    audio_free(out_str);
    return ret;
}

esp_err_t flash_list_next(playlist_operator_handle_t handle, int step, char **url_buff)
{
    AUDIO_NULL_CHECK(TAG, handle, return ESP_FAIL);
    AUDIO_NULL_CHECK(TAG, url_buff, return ESP_FAIL);
    flash_list_t *playlist = handle->playlist;
    AUDIO_NULL_CHECK(TAG, playlist, return ESP_FAIL);

    if (playlist->url_num == 0) {
        ESP_LOGE(TAG, "No url, please save urls to playlist first");
        return ESP_FAIL;
    }
    if (step < 0) {
        ESP_LOGE(TAG, "Steps should be larger than 0");
        return ESP_FAIL;
    }

    int id = 0, total_step = 0;
    total_step = playlist->cur_url_id + step;
    id = total_step;
    if (total_step >= playlist->url_num) {
        id = total_step % playlist->url_num;
    }
    return flash_list_choose_id(playlist, id, url_buff);
}

esp_err_t flash_list_prev(playlist_operator_handle_t handle, int step, char **url_buff)
{
    AUDIO_NULL_CHECK(TAG, handle, return ESP_FAIL);
    AUDIO_NULL_CHECK(TAG, url_buff, return ESP_FAIL);
    flash_list_t *playlist = handle->playlist;
    AUDIO_NULL_CHECK(TAG, playlist, return ESP_FAIL);

    if (playlist->url_num == 0) {
        ESP_LOGE(TAG, "No url, please save urls to playlist first");
        return ESP_FAIL;
    }
    if (step < 0) {
        ESP_LOGE(TAG, "Steps should be larger than 0");
        return ESP_FAIL;
    }
    int id = 0, total_step = 0;
    total_step = playlist->cur_url_id - step;
    id = total_step;
    if (total_step < 0) {
        if (total_step % playlist->url_num == 0) {
            id = 0;
        } else {
            id = playlist->url_num + total_step % playlist->url_num;
        }
    }
    return flash_list_choose_id(playlist, id, url_buff);
}

esp_err_t flash_list_current(playlist_operator_handle_t handle, char **url_buff)
{
    AUDIO_NULL_CHECK(TAG, handle, return ESP_FAIL);
    flash_list_t *playlist = handle->playlist;
    AUDIO_NULL_CHECK(TAG, playlist, return ESP_FAIL);

    if (NULL == playlist || NULL == url_buff) {
        ESP_LOGE(TAG, "Invalid parameters, line: %d, func:%s", __LINE__, __FUNCTION__);
        return ESP_FAIL;
    }
    if (playlist->url_num == 0) {
        ESP_LOGE(TAG, "No url, please save urls to playlist first");
        return ESP_FAIL;
    }

    return flash_list_choose_id(playlist, playlist->cur_url_id, url_buff);
}

esp_err_t flash_list_choose(playlist_operator_handle_t handle, int url_id, char **url_buff)
{
    AUDIO_NULL_CHECK(TAG, handle, return ESP_FAIL);
    AUDIO_NULL_CHECK(TAG, url_buff, return ESP_FAIL);
    flash_list_t *playlist = handle->playlist;
    AUDIO_NULL_CHECK(TAG, playlist, return ESP_FAIL);

    if (playlist->url_num == 0) {
        ESP_LOGE(TAG, "No url, please save urls to playlist first");
        return ESP_FAIL;
    }
    if ((url_id < 0) || (url_id >= playlist->url_num)) {
        ESP_LOGE(TAG, "Invalid url id to be choosen");
        return ESP_FAIL;
    }

    return flash_list_choose_id(playlist, url_id, url_buff);
}

esp_err_t flash_list_reset(playlist_operator_handle_t handle)
{
    AUDIO_NULL_CHECK(TAG, handle, return ESP_FAIL);
    flash_list_t *playlist = handle->playlist;
    AUDIO_NULL_CHECK(TAG, playlist, return ESP_FAIL);

    nvs_erase_all(playlist->url_nvs_handle);
    if (playlist->cur_url) {
        audio_free(playlist->cur_url);
        playlist->cur_url = NULL;
    }

    playlist->cur_url_id = 0;
    playlist->url_num = 0;
    return ESP_OK;
}

bool flash_list_exist(playlist_operator_handle_t handle, const char *url)
{
    esp_err_t ret = ESP_OK;
    AUDIO_NULL_CHECK(TAG, handle, return ESP_FAIL);
    flash_list_t *playlist = handle->playlist;
    AUDIO_NULL_CHECK(TAG, playlist, return ESP_FAIL);

    size_t len = NVS_FLASH_URL_MAX_LENGTH;
    char *out_str = audio_calloc(1, NVS_FLASH_URL_MAX_LENGTH);
    for (int i = 0; i < playlist->url_num; i++) {
        memset(out_str, 0, NVS_FLASH_URL_MAX_LENGTH);
        len = NVS_FLASH_URL_MAX_LENGTH;
        ret = nvs_get_str(playlist->url_nvs_handle, (const char *)&i, out_str, &len);
        if (strlen(url) != strlen(out_str)) {
            continue;
        }
        if (strcmp(url, out_str) == 0) {
            audio_free(out_str);
            return true;
        }
    }
    audio_free(out_str);
    return ret;
}

int flash_list_get_url_num(playlist_operator_handle_t handle)
{
    AUDIO_NULL_CHECK(TAG, handle, return ESP_FAIL);
    flash_list_t *playlist = handle->playlist;
    AUDIO_NULL_CHECK(TAG, playlist, return ESP_FAIL);

    return playlist->url_num;
}

int flash_list_get_url_id(playlist_operator_handle_t handle)
{
    AUDIO_NULL_CHECK(TAG, handle, return ESP_FAIL);
    flash_list_t *playlist = handle->playlist;
    AUDIO_NULL_CHECK(TAG, playlist, return ESP_FAIL);
    return playlist->cur_url_id;
}

esp_err_t flash_list_destroy(playlist_operator_handle_t handle)
{
    AUDIO_NULL_CHECK(TAG, handle, return ESP_FAIL);
    flash_list_t *playlist = handle->playlist;
    AUDIO_NULL_CHECK(TAG, playlist, return ESP_FAIL);

    nvs_erase_all(playlist->url_nvs_handle);
    nvs_close(playlist->url_nvs_handle);
    audio_free(playlist->name_space);
    playlist->name_space = NULL;
    if (playlist->cur_url) {
        audio_free(playlist->cur_url);
        playlist->cur_url = NULL;
    }
    audio_free(playlist);
    handle->playlist = NULL;
    audio_free(handle);
    return ESP_OK;
}

esp_err_t flash_list_get_operation(playlist_operation_t *operation)
{
    AUDIO_NULL_CHECK(TAG, operation, return ESP_FAIL);
    operation->show = (void *)flash_list_show;
    operation->save = (void *)flash_list_save;
    operation->next = (void *)flash_list_next;
    operation->prev = (void *)flash_list_prev;
    operation->reset   = (void *)flash_list_reset;
    operation->exist   = (void *)flash_list_exist;
    operation->current = (void *)flash_list_current;
    operation->choose  = (void *)flash_list_choose;
    operation->destroy = (void *)flash_list_destroy;
    operation->get_url_num = (void *)flash_list_get_url_num;
    operation->get_url_id  = (void *)flash_list_get_url_id;
    operation->type = PLAYLIST_FLASH;
    return ESP_OK;
}
