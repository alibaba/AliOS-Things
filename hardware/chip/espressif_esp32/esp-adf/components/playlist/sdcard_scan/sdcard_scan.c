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

#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include "esp_log.h"
#include "audio_error.h"
#include "audio_mem.h"
#include "sdcard_scan.h"

#define SDCARD_FILE_PREV_NAME           "file:/"
#define SDCARD_SCAN_URL_MAX_LENGTH      (1024 * 2)

static const char *TAG = "SDCARD_SCAN";

static void scan_dir(sdcard_scan_cb_t save_url_cb, const char *path, int cur_depth, int depth, const char *file_extension[] , int filter_num, void *user_data)
{
    if (cur_depth > depth) {
        ESP_LOGD(TAG, "scan depth = %d, exit", cur_depth);
        return;
    }
    char *file_url = audio_calloc(1, SDCARD_SCAN_URL_MAX_LENGTH);
    AUDIO_NULL_CHECK(TAG, file_url, return);

    DIR *dir = opendir(path);
    if (dir == NULL) {
        ESP_LOGE(TAG, "Open [%s] directory failed", path);
        audio_free(file_url);
        return;
    }

    struct dirent *file_info = NULL;
    while (NULL != (file_info = readdir(dir))) {
        if ((strlen(file_info->d_name) + strlen(path)) > (SDCARD_SCAN_URL_MAX_LENGTH - strlen(SDCARD_FILE_PREV_NAME))) {
            ESP_LOGE(TAG, "The file name is too long, invalid url");
            continue;
        }
        if (file_info->d_name[0] == '.') {
            continue;
        }

        if (file_info->d_type == DT_DIR) {
            if (file_info->d_name[0] == '_' && file_info->d_name[1] == '_') {
                continue;
            }
            memset(file_url, 0, SDCARD_SCAN_URL_MAX_LENGTH);
            sprintf(file_url, "%s/%s", path, file_info->d_name);
            scan_dir(save_url_cb, file_url, cur_depth + 1, depth, file_extension, filter_num, user_data);
        } else {
            memset(file_url, 0, SDCARD_SCAN_URL_MAX_LENGTH);
            sprintf(file_url, "%s%s/%s", SDCARD_FILE_PREV_NAME, path, file_info->d_name);
            if (NULL == file_extension) {
                save_url_cb(user_data, file_url);
                continue;
            }
            char *detect = strrchr(file_info->d_name, '.');
            if (NULL == detect) {
                continue;
            }
            detect ++;
            for (int i = 0; i < filter_num; i++) {
                if (strcasecmp(detect, file_extension[i]) == 0) {
                    save_url_cb(user_data, file_url);
                    break;
                }
            }
        }
    }
    audio_free(file_url);
    closedir(dir);
}

esp_err_t sdcard_scan(sdcard_scan_cb_t cb, const char *path, int depth, const char *file_extension[], int filter_num, void *user_data)
{
    AUDIO_NULL_CHECK(TAG, cb, return ESP_FAIL);
    AUDIO_NULL_CHECK(TAG, path, return ESP_FAIL);
    AUDIO_NULL_CHECK(TAG, file_extension, return ESP_FAIL);
    if (depth < 0 || filter_num < 0) {
        ESP_LOGE(TAG, "Invalid parameters, please check");
        return ESP_FAIL;
    }

    scan_dir(cb, path, 0, depth, file_extension, filter_num, user_data);
    return ESP_OK;
}
