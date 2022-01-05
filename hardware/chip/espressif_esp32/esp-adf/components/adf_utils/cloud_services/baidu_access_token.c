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
#include <stdlib.h>
#include "esp_http_client.h"
#include "json_utils.h"
#include "esp_log.h"
#include "audio_error.h"

#define BAIDU_URI_LENGTH (200)
#define BAIDU_AUTH_ENDPOINT "https://openapi.baidu.com/oauth/2.0/token?grant_type=client_credentials"

static const char *TAG = "BAIDU_AUTH";

char *baidu_get_access_token(const char *access_key, const char *access_secret)
{
    char *token = NULL;
    char *url = calloc(1, BAIDU_URI_LENGTH);

    AUDIO_MEM_CHECK(TAG, url, return NULL);

    snprintf(url, BAIDU_URI_LENGTH, BAIDU_AUTH_ENDPOINT"&client_id=%s&client_secret=%s", access_key, access_secret);

    esp_http_client_config_t config = {
        .url = url,
    };
    esp_http_client_handle_t http_client = esp_http_client_init(&config);
    AUDIO_MEM_CHECK(TAG, http_client, return NULL);

    if (esp_http_client_open(http_client, 0) != ESP_OK) {
        ESP_LOGE(TAG, "Error open http request to baidu auth server");
        goto _exit;
    }
    esp_http_client_fetch_headers(http_client);
    int max_len = 2 * 1024;
    char *data = malloc(max_len);

    AUDIO_MEM_CHECK(TAG, data, goto _exit);

    int read_index = 0, total_len = 0;
    while (1) {
        int read_len = esp_http_client_read(http_client, data + read_index, max_len - read_index);
        if (read_len <= 0) {
            break;
        }
        read_index += read_len;
        total_len += read_len;
        data[read_index] = 0;
    }
    if (total_len <= 0) {
        ESP_LOGE(TAG, "Invalid length of the response");
        free(data);
        goto _exit;
    }
    // Remove unexpect characters
    ESP_LOGD(TAG, "Data=%s", data);
    token = json_get_token_value(data, "access_token");
    free(data);
    if (token) {
        ESP_LOGI(TAG, "Access token=%s", token);
    }
_exit:
    free(url);
    esp_http_client_close(http_client);
    esp_http_client_cleanup(http_client);
    return token;
}
