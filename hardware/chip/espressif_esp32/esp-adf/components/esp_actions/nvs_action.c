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

#include "audio_error.h"
#include "audio_mem.h"
#include "esp_action_def.h"
#include "esp_audio.h"
#include "esp_log.h"

#include "nvs_action.h"

static const char *TAG = "NVS_ACTION";

esp_err_t nvs_action_open(void *instance, action_arg_t *arg, action_result_t *result)
{
    AUDIO_MEM_CHECK(TAG, arg, return ESP_FAIL);
    AUDIO_MEM_CHECK(TAG, result, return ESP_FAIL);

    nvs_action_open_args_t *open_arg = (nvs_action_open_args_t *)arg->data;
    result->data = audio_calloc(1, sizeof(nvs_handle));
    result->len = sizeof(nvs_handle);
    return nvs_open(open_arg->name, open_arg->open_mode, result->data);
}

esp_err_t nvs_action_open_from_partion(void *instance, action_arg_t *arg, action_result_t *result)
{
    AUDIO_MEM_CHECK(TAG, arg, return ESP_FAIL);
    AUDIO_MEM_CHECK(TAG, result, return ESP_FAIL);

    nvs_action_open_partition_args_t *open_arg = (nvs_action_open_partition_args_t *)arg->data;
    result->data = audio_calloc(1, sizeof(nvs_handle));
    result->len = sizeof(nvs_handle);
    return nvs_open_from_partition(open_arg->partition, open_arg->name, open_arg->open_mode, result->data);
}

esp_err_t nvs_action_set(void *instance, action_arg_t *arg, action_result_t *result)
{
    AUDIO_MEM_CHECK(TAG, instance, return ESP_FAIL);
    AUDIO_MEM_CHECK(TAG, arg, return ESP_FAIL);
    AUDIO_MEM_CHECK(TAG, result, return ESP_FAIL);

    nvs_action_set_args_t *set_arg = (nvs_action_set_args_t *)arg->data;
    esp_err_t err = ESP_FAIL;

    switch (set_arg->type) {
        case NVS_TYPE_U8:
            err = nvs_set_u8((nvs_handle)instance, set_arg->key, set_arg->value.u8);
            break;
        case NVS_TYPE_I8:
            err = nvs_set_i8((nvs_handle)instance, set_arg->key, set_arg->value.i8);
            break;
        case NVS_TYPE_U16:
            err = nvs_set_u16((nvs_handle)instance, set_arg->key, set_arg->value.u16);
            break;
        case NVS_TYPE_I16:
            err = nvs_set_i16((nvs_handle)instance, set_arg->key, set_arg->value.i16);
            break;
        case NVS_TYPE_U32:
            err = nvs_set_u32((nvs_handle)instance, set_arg->key, set_arg->value.u32);
            break;
        case NVS_TYPE_I32:
            err = nvs_set_i32((nvs_handle)instance, set_arg->key, set_arg->value.i32);
            break;
        case NVS_TYPE_U64:
            err = nvs_set_u64((nvs_handle)instance, set_arg->key, set_arg->value.u64);
            break;
        case NVS_TYPE_I64:
            err = nvs_set_i64((nvs_handle)instance, set_arg->key, set_arg->value.i64);
            break;
        case NVS_TYPE_STR:
            err = nvs_set_str((nvs_handle)instance, set_arg->key, set_arg->value.string);
            break;
        case NVS_TYPE_BLOB:
            err = nvs_set_blob((nvs_handle)instance, set_arg->key, set_arg->value.blob, set_arg->len);
            break;
        default:
            break;
    }

    return err;
}

esp_err_t nvs_action_get(void *instance, action_arg_t *arg, action_result_t *result)
{
    AUDIO_MEM_CHECK(TAG, instance, return ESP_FAIL);
    AUDIO_MEM_CHECK(TAG, arg, return ESP_FAIL);
    AUDIO_MEM_CHECK(TAG, result, return ESP_FAIL);

    nvs_action_get_args_t *get_arg = (nvs_action_get_args_t *)arg->data;
    esp_err_t err = ESP_FAIL;

    switch (get_arg->type) {
        case NVS_TYPE_U8: {
            result->data = audio_calloc(1, sizeof(uint8_t));
            result->len = sizeof(uint8_t);
            err = nvs_get_u8((nvs_handle)instance, get_arg->key, result->data);
            break;
        }
        case NVS_TYPE_I8: {
            result->data = audio_calloc(1, sizeof(int8_t));
            result->len = sizeof(int8_t);
            err = nvs_get_i8((nvs_handle)instance, get_arg->key, result->data);
            break;
        }
        case NVS_TYPE_U16: {
            result->data = audio_calloc(1, sizeof(uint16_t));
            result->len = sizeof(uint16_t);
            err = nvs_get_u16((nvs_handle)instance, get_arg->key, result->data);
            break;
        }
        case NVS_TYPE_I16: {
            result->data = audio_calloc(1, sizeof(int16_t));
            result->len = sizeof(int16_t);
            err = nvs_get_i16((nvs_handle)instance, get_arg->key, result->data);
            break;
        }
        case NVS_TYPE_U32: {
            result->data = audio_calloc(1, sizeof(uint32_t));
            result->len = sizeof(uint32_t);
            err = nvs_get_u32((nvs_handle)instance, get_arg->key, result->data);
            break;
        }
        case NVS_TYPE_I32: {
            result->data = audio_calloc(1, sizeof(int32_t));
            result->len = sizeof(int32_t);
            err = nvs_get_i32((nvs_handle)instance, get_arg->key, result->data);
            break;
        }
        case NVS_TYPE_U64: {
            result->data = audio_calloc(1, sizeof(uint64_t));
            result->len = sizeof(uint64_t);
            err = nvs_get_u64((nvs_handle)instance, get_arg->key, result->data);
            break;
        }
        case NVS_TYPE_I64: {
            result->data = audio_calloc(1, sizeof(int64_t));
            result->len = sizeof(int64_t);
            err = nvs_get_i64((nvs_handle)instance, get_arg->key, result->data);
            break;
        }
        case NVS_TYPE_STR: {
            result->data = audio_calloc(1, get_arg->wanted_size + 1);
            result->len = get_arg->wanted_size + 1;
            err = nvs_get_str((nvs_handle)instance, get_arg->key, result->data, (size_t *)&result->len);
            break;
        }
        case NVS_TYPE_BLOB: {
            result->data = audio_calloc(1, get_arg->wanted_size + 1);
            result->len = get_arg->wanted_size + 1;
            err = nvs_get_blob((nvs_handle)instance, get_arg->key, result->data, (size_t *)&result->len);
            break;
        }
        default:
            break;
    }

    return err;
}

esp_err_t nvs_action_erase_key(void *instance, action_arg_t *arg, action_result_t *result)
{
    AUDIO_MEM_CHECK(TAG, instance, return ESP_FAIL);
    AUDIO_MEM_CHECK(TAG, arg, return ESP_FAIL);

    return nvs_erase_key((nvs_handle)instance, arg->data);
}

esp_err_t nvs_action_erase_all(void *instance, action_arg_t *arg, action_result_t *result)
{
    AUDIO_MEM_CHECK(TAG, instance, return ESP_FAIL);

    return nvs_erase_all((nvs_handle)instance);
}

esp_err_t nvs_action_commit(void *instance, action_arg_t *arg, action_result_t *result)
{
    AUDIO_MEM_CHECK(TAG, instance, return ESP_FAIL);

    return nvs_commit((nvs_handle)instance);
}

esp_err_t nvs_action_close(void *instance, action_arg_t *arg, action_result_t *result)
{
    AUDIO_MEM_CHECK(TAG, instance, return ESP_FAIL);

    nvs_close((nvs_handle)instance);
    return ESP_OK;
}

esp_err_t nvs_action_get_stats(void *instance, action_arg_t *arg, action_result_t *result)
{
    AUDIO_MEM_CHECK(TAG, arg, return ESP_FAIL);
    AUDIO_MEM_CHECK(TAG, result, return ESP_FAIL);

    result->data = audio_calloc(1, sizeof(action_result_t));
    result->len = sizeof(action_result_t);

    return nvs_get_stats(arg->data, result->data);
}

esp_err_t nvs_action_get_used_entry_count(void *instance, action_arg_t *arg, action_result_t *result)
{
    AUDIO_MEM_CHECK(TAG, instance, return ESP_FAIL);
    AUDIO_MEM_CHECK(TAG, result, return ESP_FAIL);
    result->data = audio_calloc(1, sizeof(size_t));
    result->len = sizeof(size_t);
    return nvs_get_used_entry_count((nvs_handle)instance, (size_t *)&result->data);
}
