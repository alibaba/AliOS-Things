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
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>

#include "audio_error.h"
#include "audio_mem.h"
#include "sdcard_list.h"

#define SDCARD_DEFAULT_DIR_NAME         "/sdcard/__playlist"
#define SDCARD_DEFAULT_URL_FILE_NAME    "/sdcard/__playlist/_playlist_url"
#define SDCARD_DEFAULT_OFFSET_FILE_NAME "/sdcard/__playlist/_offset"

#define SDCARD_URL_FILE_NAME_LENGTH     (strlen(SDCARD_DEFAULT_URL_FILE_NAME) + 10)
#define SDCARD_OFFSET_FILE_NAME_LENGTH  (strlen(SDCARD_DEFAULT_OFFSET_FILE_NAME) + 10)

#define SDCARD_LIST_URL_MAX_LENGTH      (1024 * 2)

#define CHECK_ERROR(TAG, para, action)  {\
    if ((para) == false) {\
       ESP_LOGE(TAG, "unexpect action in sdcard list, line: %d", __LINE__);\
       action;\
    }\
}

static const char *TAG = "PLAYLIST_SDCARD";

/**
 * @brief Sdcard list management unit
 */
typedef struct sdcard_list {
    char *save_file_name;                /*!< Name of file to save URLs */
    char *offset_file_name;              /*!< Name of file to save offset */
    FILE *save_file;                     /*!< File to save urls */
    FILE *offset_file;                   /*!< File to save offset of urls */
    char *cur_url;                       /*!< Point to current URL */
    uint16_t url_num;                    /*!< Number of URLs */
    uint16_t cur_url_id;                 /*!< Current url ID */
    uint32_t total_size_save_file;       /*!< Size of file to save URLs */
    uint32_t total_size_offset_file;     /*!< Size of file to save offset */
} sdcard_list_t;

esp_err_t sdcard_list_get_operation(playlist_operation_t *operation);

static esp_err_t save_url_to_sdcard(sdcard_list_t *playlist, const char *path)
{
    if (playlist->save_file == NULL || playlist->offset_file == NULL) {
        ESP_LOGE(TAG, "The file to save playlist failed to open");
        return ESP_FAIL;
    }
    uint16_t len = strlen(path);
    CHECK_ERROR(TAG, ((fseek(playlist->offset_file, playlist->total_size_offset_file, SEEK_SET)) == 0), return ESP_FAIL);
    CHECK_ERROR(TAG, ((fseek(playlist->save_file, playlist->total_size_save_file, SEEK_SET)) == 0), return ESP_FAIL);
    CHECK_ERROR(TAG, (fwrite(path, 1, len, playlist->save_file) == len), return ESP_FAIL);
    CHECK_ERROR(TAG, (fwrite(&playlist->total_size_save_file, 1, sizeof(uint32_t), playlist->offset_file) == sizeof(uint32_t)), return ESP_FAIL);
    CHECK_ERROR(TAG, (fwrite(&len, 1, sizeof(uint16_t), playlist->offset_file)) == sizeof(uint16_t), return ESP_FAIL);
    CHECK_ERROR(TAG, (fsync(fileno(playlist->save_file)) == 0), return ESP_FAIL);
    CHECK_ERROR(TAG, (fsync(fileno(playlist->offset_file)) == 0), return ESP_FAIL);
    playlist->total_size_save_file += len;
    playlist->total_size_offset_file += (sizeof(uint16_t) + sizeof(uint32_t));
    playlist->url_num++;

    return ESP_OK;
}

static esp_err_t sdcard_list_open(sdcard_list_t *playlist, uint8_t list_id)
{
    playlist->save_file_name = audio_calloc(1, SDCARD_URL_FILE_NAME_LENGTH);
    AUDIO_NULL_CHECK(TAG, playlist->save_file_name, return ESP_FAIL);
    playlist->offset_file_name = audio_calloc(1, SDCARD_OFFSET_FILE_NAME_LENGTH);
    AUDIO_NULL_CHECK(TAG, playlist->offset_file_name, {
        audio_free(playlist->save_file_name);
        return ESP_FAIL;
    });

    sprintf(playlist->save_file_name, "%s%d", SDCARD_DEFAULT_URL_FILE_NAME, list_id);
    sprintf(playlist->offset_file_name, "%s%d", SDCARD_DEFAULT_OFFSET_FILE_NAME, list_id);

    mkdir(SDCARD_DEFAULT_DIR_NAME, 0777);

    playlist->save_file = fopen(playlist->save_file_name, "w+");
    playlist->offset_file = fopen(playlist->offset_file_name, "w+");

    if (playlist->save_file == NULL || NULL == playlist->offset_file) {
        ESP_LOGE(TAG, "open file error, line: %d, have you mounted sdcard, set the long file name and UTF-8 encoding configuration ?", __LINE__);
        audio_free(playlist->save_file_name);
        audio_free(playlist->offset_file_name);
        if (playlist->save_file) {
            fclose(playlist->save_file);
        }
        if (playlist->offset_file) {
            fclose(playlist->offset_file);
        }
        return ESP_FAIL;
    }
    return ESP_OK;
}

static esp_err_t sdcard_list_close(sdcard_list_t *playlist)
{
    fclose(playlist->offset_file);
    fclose(playlist->save_file);
    playlist->offset_file = NULL;
    playlist->save_file = NULL;
    return ESP_OK;
}

static esp_err_t sdcard_list_choose_id(sdcard_list_t *playlist, int id, char **url_buff)
{
    uint32_t pos = 0;
    uint16_t  size = 0;
    int offset = id * (sizeof(uint32_t) + sizeof(uint16_t));

    CHECK_ERROR(TAG, ((fseek(playlist->offset_file, offset, SEEK_SET)) == 0), return ESP_FAIL);
    CHECK_ERROR(TAG, ((fread(&pos, 1, sizeof(uint32_t), playlist->offset_file)) == sizeof(uint32_t)), return ESP_FAIL);
    CHECK_ERROR(TAG, ((fread(&size, 1, sizeof(uint16_t), playlist->offset_file)) == sizeof(uint16_t)), return ESP_FAIL);

    if (playlist->cur_url) {
        audio_free(playlist->cur_url);
    }
    playlist->cur_url = (char *)audio_calloc(1, size + 1);
    AUDIO_NULL_CHECK(TAG, playlist->cur_url, {
        ESP_LOGE(TAG, "Fail to allocate memory for url");
        fclose(playlist->offset_file);
        fclose(playlist->save_file);
        return ESP_FAIL;
    });

    CHECK_ERROR(TAG, ((fseek(playlist->save_file, pos, SEEK_SET)) == 0), return ESP_FAIL);
    CHECK_ERROR(TAG, ((fread(playlist->cur_url, 1, size, playlist->save_file)) == size), return ESP_FAIL);

    playlist->cur_url[size] = 0;
    playlist->cur_url_id = id;
    *url_buff = playlist->cur_url;

    return ESP_OK;
}

esp_err_t sdcard_list_create(playlist_operator_handle_t *handle)
{
    esp_err_t ret = ESP_OK;
    AUDIO_NULL_CHECK(TAG, handle, return ESP_FAIL);

    playlist_operator_handle_t sdcard_handle = (playlist_operator_handle_t )audio_calloc(1, sizeof(playlist_operator_t));
    AUDIO_NULL_CHECK(TAG, sdcard_handle, return ESP_FAIL);

    sdcard_list_t *sdcard_list = (sdcard_list_t *)audio_calloc(1, sizeof(sdcard_list_t));
    AUDIO_NULL_CHECK(TAG, sdcard_list, {
        audio_free(sdcard_handle);
        return ESP_FAIL;
    });

    sdcard_handle->playlist = sdcard_list;
    sdcard_handle->get_operation = sdcard_list_get_operation;

    static int list_id;
    ret |= sdcard_list_open(sdcard_list, list_id++);
    if (ret != ESP_OK) {
        audio_free(sdcard_handle);
        audio_free(sdcard_list);
        return ESP_FAIL;
    }

    *handle = sdcard_handle;
    return ESP_OK;
}

esp_err_t sdcard_list_show(playlist_operator_handle_t handle)
{
    AUDIO_NULL_CHECK(TAG, handle, return ESP_FAIL);
    sdcard_list_t *playlist = handle->playlist;
    AUDIO_NULL_CHECK(TAG, playlist, return ESP_FAIL);

    uint32_t pos = 0;
    uint16_t  size = 0;
    char *url = audio_calloc(1, SDCARD_LIST_URL_MAX_LENGTH);

    CHECK_ERROR(TAG, (fseek(playlist->save_file, 0, SEEK_SET) == 0), return ESP_FAIL);
    CHECK_ERROR(TAG, (fseek(playlist->offset_file, 0, SEEK_SET) == 0), return ESP_FAIL);

    ESP_LOGI(TAG, "ID   URL");
    for (int i = 0; i < playlist->url_num; i++) {
        memset(url, 0, SDCARD_LIST_URL_MAX_LENGTH);
        CHECK_ERROR(TAG, (fread(&pos, 1, sizeof(uint32_t), playlist->offset_file) == sizeof(uint32_t)), return ESP_FAIL);
        CHECK_ERROR(TAG, (fread(&size, 1, sizeof(uint16_t), playlist->offset_file) == sizeof(uint16_t)), return ESP_FAIL);
        CHECK_ERROR(TAG, (fseek(playlist->save_file, pos, SEEK_SET) == 0), return ESP_FAIL);
        CHECK_ERROR(TAG, (fread(url, 1, size, playlist->save_file) == size), return ESP_FAIL);
        ESP_LOGI(TAG, "%d   %s", i, url);
    }

    audio_free(url);
    return ESP_OK;
}

esp_err_t sdcard_list_next(playlist_operator_handle_t handle, int step, char **url_buff)
{
    AUDIO_NULL_CHECK(TAG, handle, return ESP_FAIL);
    AUDIO_NULL_CHECK(TAG, url_buff, return ESP_FAIL);
    sdcard_list_t *playlist = handle->playlist;
    AUDIO_NULL_CHECK(TAG, playlist, return ESP_FAIL);

    if (playlist->url_num == 0) {
        ESP_LOGE(TAG, "No url, please save urls to playlist first");
        return ESP_FAIL;
    }
    if (step < 0) {
        ESP_LOGE(TAG, "Step should be larger than 0");
        return ESP_FAIL;
    }

    int id = 0, total_step = 0;
    total_step = playlist->cur_url_id + step;
    id = total_step;
    if (total_step >= playlist->url_num) {
        id = total_step % playlist->url_num;
    }

    return sdcard_list_choose_id(playlist, id, url_buff);
}

esp_err_t sdcard_list_prev(playlist_operator_handle_t handle, int step, char **url_buff)
{
    AUDIO_NULL_CHECK(TAG, handle, return ESP_FAIL);
    AUDIO_NULL_CHECK(TAG, url_buff, return ESP_FAIL);
    sdcard_list_t *playlist = handle->playlist;
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

    return sdcard_list_choose_id(playlist, id, url_buff);
}

esp_err_t sdcard_list_current(playlist_operator_handle_t handle, char **url_buff)
{
    AUDIO_NULL_CHECK(TAG, handle, return ESP_FAIL);
    AUDIO_NULL_CHECK(TAG, url_buff, return ESP_FAIL);
    sdcard_list_t *playlist = handle->playlist;
    AUDIO_NULL_CHECK(TAG, playlist, return ESP_FAIL);

    if (playlist->url_num == 0) {
        ESP_LOGE(TAG, "No url, please save urls to playlist first");
        return ESP_FAIL;
    }

    return sdcard_list_choose_id(playlist, playlist->cur_url_id, url_buff);
}

esp_err_t sdcard_list_choose(playlist_operator_handle_t handle, int url_id, char **url_buff)
{
    AUDIO_NULL_CHECK(TAG, handle, return ESP_FAIL);
    AUDIO_NULL_CHECK(TAG, url_buff, return ESP_FAIL);
    sdcard_list_t *playlist = handle->playlist;
    AUDIO_NULL_CHECK(TAG, playlist, return ESP_FAIL);

    if (playlist->url_num == 0) {
        ESP_LOGE(TAG, "No url, please save urls to playlist first");
        return ESP_FAIL;
    }
    if ((url_id < 0) || (url_id >= playlist->url_num)) {
        ESP_LOGE(TAG, "Invalid url id to be choosen");
        return ESP_FAIL;
    }

    return sdcard_list_choose_id(playlist, url_id, url_buff);
}

esp_err_t sdcard_list_save(playlist_operator_handle_t handle, const char *url)
{
    AUDIO_NULL_CHECK(TAG, handle, return ESP_FAIL);
    AUDIO_NULL_CHECK(TAG, url, return ESP_FAIL);
    sdcard_list_t *playlist = handle->playlist;
    AUDIO_NULL_CHECK(TAG, playlist, return ESP_FAIL);

    esp_err_t ret = ESP_OK;
    uint16_t len = strlen(url);
    if (len >= SDCARD_LIST_URL_MAX_LENGTH) {
        ESP_LOGE(TAG, "The url length is greater than MAX LENTGTH, you should change the SDCARD_LIST_URL_MAX_LENGTH value");
        return ESP_FAIL;
    }
    ret = save_url_to_sdcard(playlist, url);
    return ret;
}

bool sdcard_list_exist(playlist_operator_handle_t handle, const char *url)
{
    AUDIO_NULL_CHECK(TAG, handle, return ESP_FAIL);
    sdcard_list_t *playlist = handle->playlist;
    AUDIO_NULL_CHECK(TAG, playlist, return ESP_FAIL);

    uint32_t pos = 0;
    uint16_t  size = 0;
    char *url_buff = audio_calloc(1, SDCARD_LIST_URL_MAX_LENGTH);

    CHECK_ERROR(TAG, (fseek(playlist->save_file, 0, SEEK_SET) == 0), return ESP_FAIL);
    CHECK_ERROR(TAG, (fseek(playlist->offset_file, 0, SEEK_SET) == 0), return ESP_FAIL);

    for (int i = 0; i < playlist->url_num; i++) {
        memset(url_buff, 0, SDCARD_LIST_URL_MAX_LENGTH);
        CHECK_ERROR(TAG, (fread(&pos, 1, sizeof(uint32_t), playlist->offset_file) == sizeof(uint32_t)), return ESP_FAIL);
        CHECK_ERROR(TAG, (fread(&size, 1, sizeof(uint16_t), playlist->offset_file) == sizeof(uint16_t)), return ESP_FAIL);
        CHECK_ERROR(TAG, (fseek(playlist->save_file, pos, SEEK_SET) == 0), return ESP_FAIL);
        CHECK_ERROR(TAG, (fread(url_buff, 1, size, playlist->save_file) == size), return ESP_FAIL);
        if (strlen(url_buff) != strlen(url)) {
            continue;
        }
        if (strcmp(url, url_buff) == 0) {
            audio_free(url_buff);
            return true;
        }
    }

    audio_free(url_buff);
    return false;
}

esp_err_t sdcard_list_reset(playlist_operator_handle_t handle)
{
    AUDIO_NULL_CHECK(TAG, handle, return ESP_FAIL);
    sdcard_list_t *playlist = handle->playlist;
    AUDIO_NULL_CHECK(TAG, playlist, return ESP_FAIL);

    /**
     * ftruncate() function is not supported now, it won't affect the operation result.
     *
       CHECK_ERROR(TAG, (ftruncate(playlist->save_file, 0) == ESP_OK), return ESP_FAIL);
       CHECK_ERROR(TAG, (ftruncate(playlist->offset_file, 0) == ESP_OK), return ESP_FAIL);
       CHECK_ERROR(TAG, (fseek(playlist->save_file, 0, SEEK_SET) == 0), return ESP_FAIL);
       CHECK_ERROR(TAG, (fseek(playlist->offset_file, 0, SEEK_SET) == 0), return ESP_FAIL);
    */
    if (playlist->cur_url) {
        audio_free(playlist->cur_url);
        playlist->cur_url = NULL;
    }

    playlist->url_num = 0;
    playlist->cur_url_id = 0;
    playlist->total_size_offset_file = 0;
    playlist->total_size_save_file = 0;
    return ESP_OK;
}

int sdcard_list_get_url_num(playlist_operator_handle_t handle)
{
    AUDIO_NULL_CHECK(TAG, handle, return ESP_FAIL);
    sdcard_list_t *playlist = handle->playlist;
    AUDIO_NULL_CHECK(TAG, playlist, return ESP_FAIL);

    return playlist->url_num;
}

int sdcard_list_get_url_id(playlist_operator_handle_t handle)
{
    AUDIO_NULL_CHECK(TAG, handle, return ESP_FAIL);
    sdcard_list_t *playlist = handle->playlist;
    AUDIO_NULL_CHECK(TAG, playlist, return ESP_FAIL);

    return playlist->cur_url_id;
}

esp_err_t sdcard_list_destroy(playlist_operator_handle_t handle)
{
    AUDIO_NULL_CHECK(TAG, handle, return ESP_FAIL);
    sdcard_list_t *playlist = handle->playlist;
    AUDIO_NULL_CHECK(TAG, playlist, return ESP_FAIL);

    sdcard_list_close(playlist);
    remove(playlist->save_file_name);
    remove(playlist->offset_file_name);
    audio_free(playlist->save_file_name);
    audio_free(playlist->offset_file_name);
    if (playlist->cur_url) {
        audio_free(playlist->cur_url);
        playlist->cur_url = NULL;
    }
    audio_free(playlist);
    handle->playlist = NULL;
    audio_free(handle);
    return ESP_OK;
}

esp_err_t sdcard_list_get_operation(playlist_operation_t *operation)
{
    AUDIO_NULL_CHECK(TAG, operation, return ESP_FAIL);
    operation->show = (void *)sdcard_list_show;
    operation->save = (void *)sdcard_list_save;
    operation->next = (void *)sdcard_list_next;
    operation->prev = (void *)sdcard_list_prev;
    operation->reset   = (void *)sdcard_list_reset;
    operation->exist   = (void *)sdcard_list_exist;
    operation->choose  = (void *)sdcard_list_choose;
    operation->current = (void *)sdcard_list_current;
    operation->destroy = (void *)sdcard_list_destroy;
    operation->get_url_num = (void *)sdcard_list_get_url_num;
    operation->get_url_id  = (void *)sdcard_list_get_url_id;
    operation->type = PLAYLIST_SDCARD;
    return ESP_OK;
}
