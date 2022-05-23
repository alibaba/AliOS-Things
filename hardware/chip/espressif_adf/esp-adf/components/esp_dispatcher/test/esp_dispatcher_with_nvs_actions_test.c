/*
 * ESPRESSIF MIT License
 *
 * Copyright (c) 2020 <ESPRESSIF SYSTEMS (SHANGHAI) PTE LTD>
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

#include "unity.h"

#include "esp_dispatcher.h"
#include "esp_action_def.h"
#include "nvs.h"
#include "nvs_flash.h"
#include "nvs_action.h"

TEST_CASE("esp_dispatcher sync set and read nvs", "esp-adf")
{
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    TEST_ASSERT_EQUAL(ESP_OK, err);

    esp_dispatcher_config_t d_cfg = ESP_DISPATCHER_CONFIG_DEFAULT();
    d_cfg.stack_in_ext = false;
    esp_dispatcher_handle_t dispatcher = esp_dispatcher_create(&d_cfg);
    TEST_ASSERT_NOT_NULL(dispatcher);

    action_result_t result = { 0 };
    nvs_handle nvs_sync_handle = NULL;

    // Open
    printf("Open NVS\n");
    nvs_action_open_args_t open = {
        .name = "action_sync",
        .open_mode = NVS_READWRITE,
    };
    action_arg_t open_arg = {
        .data = &open,
        .len = sizeof(nvs_action_open_args_t),
    };
    memset(&result, 0x00, sizeof(action_result_t));
    TEST_ASSERT_EQUAL(ESP_OK, esp_dispatcher_execute_with_func(dispatcher, nvs_action_open, NULL, &open_arg, &result));
    TEST_ASSERT_EQUAL(ESP_OK, result.err);
    TEST_ASSERT_NOT_NULL(result.data);
    nvs_sync_handle = *(nvs_handle *)result.data;
    free(result.data);

    // Test u8
    printf("Set U8\n");
    nvs_action_set_args_t set_u8 = {
        .key = "u8",
        .type = NVS_TYPE_U8,
        .value.u8 = 0xAB,
        .len = sizeof(uint8_t),
    };
    action_arg_t set_u8_arg = {
        .data = &set_u8,
        .len = sizeof(nvs_action_set_args_t),
    };
    memset(&result, 0x00, sizeof(action_result_t));
    TEST_ASSERT_EQUAL(ESP_OK, esp_dispatcher_execute_with_func(dispatcher, nvs_action_set, (void *)nvs_sync_handle, &set_u8_arg, &result));
    TEST_ASSERT_EQUAL(ESP_OK, result.err);
    memset(&result, 0x00, sizeof(action_result_t));
    TEST_ASSERT_EQUAL(ESP_OK, esp_dispatcher_execute_with_func(dispatcher, nvs_action_commit, (void *)nvs_sync_handle, NULL, &result));
    TEST_ASSERT_EQUAL(ESP_OK, result.err);
    printf("Read U8\n");
    nvs_action_get_args_t get_u8 = {
        .key = "u8",
        .type = NVS_TYPE_U8,
        .wanted_size = sizeof(uint8_t),
    };
    action_arg_t get_u8_arg = {
        .data = &get_u8,
        .len = sizeof(nvs_action_get_args_t),
    };
    memset(&result, 0x00, sizeof(action_result_t));
    TEST_ASSERT_EQUAL(ESP_OK, esp_dispatcher_execute_with_func(dispatcher, nvs_action_get, (void *)nvs_sync_handle, &get_u8_arg, &result));
    TEST_ASSERT_EQUAL(ESP_OK, result.err);
    TEST_ASSERT_NOT_NULL(result.data);
    uint8_t *ret_u8 = (uint8_t *)result.data;
    printf("Read [ 0x%X ] from nvs\n", *ret_u8);
    free(result.data);

    // Test i8
    printf("Set I8\n");
    nvs_action_set_args_t set_i8 = {
        .key = "i8",
        .type = NVS_TYPE_I8,
        .value.i8 = -1,
        .len = sizeof(int8_t),
    };
    action_arg_t set_i8_arg = {
        .data = &set_i8,
        .len = sizeof(nvs_action_set_args_t),
    };
    memset(&result, 0x00, sizeof(action_result_t));
    TEST_ASSERT_EQUAL(ESP_OK, esp_dispatcher_execute_with_func(dispatcher, nvs_action_set, (void *)nvs_sync_handle, &set_i8_arg, &result));
    TEST_ASSERT_EQUAL(ESP_OK, result.err);
    memset(&result, 0x00, sizeof(action_result_t));
    TEST_ASSERT_EQUAL(ESP_OK, esp_dispatcher_execute_with_func(dispatcher, nvs_action_commit, (void *)nvs_sync_handle, NULL, &result));
    TEST_ASSERT_EQUAL(ESP_OK, result.err);
    printf("Read I8\n");
    nvs_action_get_args_t get_i8 = {
        .key = "i8",
        .type = NVS_TYPE_I8,
        .wanted_size = sizeof(int8_t),
    };
    action_arg_t get_i8_arg = {
        .data = &get_i8,
        .len = sizeof(nvs_action_get_args_t),
    };
    memset(&result, 0x00, sizeof(action_result_t));
    TEST_ASSERT_EQUAL(ESP_OK, esp_dispatcher_execute_with_func(dispatcher, nvs_action_get, (void *)nvs_sync_handle, &get_i8_arg, &result));
    TEST_ASSERT_EQUAL(ESP_OK, result.err);
    TEST_ASSERT_NOT_NULL(result.data);
    int8_t *ret_i8 = (int8_t *)result.data;
    printf("Read [ %d ] from nvs\n", *ret_i8);
    free(result.data);

    // Test u16
    printf("Set U16\n");
    nvs_action_set_args_t set_u16 = {
        .key = "u16",
        .type = NVS_TYPE_U16,
        .value.u16 = 0xABCD,
        .len = sizeof(uint16_t),
    };
    action_arg_t set_u16_arg = {
        .data = &set_u16,
        .len = sizeof(nvs_action_set_args_t),
    };
    memset(&result, 0x00, sizeof(action_result_t));
    TEST_ASSERT_EQUAL(ESP_OK, esp_dispatcher_execute_with_func(dispatcher, nvs_action_set, (void *)nvs_sync_handle, &set_u16_arg, &result));
    TEST_ASSERT_EQUAL(ESP_OK, result.err);
    memset(&result, 0x00, sizeof(action_result_t));
    TEST_ASSERT_EQUAL(ESP_OK, esp_dispatcher_execute_with_func(dispatcher, nvs_action_commit, (void *)nvs_sync_handle, NULL, &result));
    TEST_ASSERT_EQUAL(ESP_OK, result.err);
    printf("Read U16\n");
    nvs_action_get_args_t get_u16 = {
        .key = "u16",
        .type = NVS_TYPE_U16,
        .wanted_size = sizeof(uint16_t),
    };
    action_arg_t get_u16_arg = {
        .data = &get_u16,
        .len = sizeof(nvs_action_get_args_t),
    };
    memset(&result, 0x00, sizeof(action_result_t));
    TEST_ASSERT_EQUAL(ESP_OK, esp_dispatcher_execute_with_func(dispatcher, nvs_action_get, (void *)nvs_sync_handle, &get_u16_arg, &result));
    TEST_ASSERT_EQUAL(ESP_OK, result.err);
    TEST_ASSERT_NOT_NULL(result.data);
    uint16_t *ret_u16 = (uint16_t *)result.data;
    printf("Read [ 0x%X ] from nvs\n", *ret_u16);
    free(result.data);

    // Test i16
    printf("Set I16\n");
    nvs_action_set_args_t set_i16 = {
        .key = "i16",
        .type = NVS_TYPE_I16,
        .value.i16 = -2,
        .len = sizeof(int16_t),
    };
    action_arg_t set_i16_arg = {
        .data = &set_i16,
        .len = sizeof(nvs_action_set_args_t),
    };
    memset(&result, 0x00, sizeof(action_result_t));
    TEST_ASSERT_EQUAL(ESP_OK, esp_dispatcher_execute_with_func(dispatcher, nvs_action_set, (void *)nvs_sync_handle, &set_i16_arg, &result));
    TEST_ASSERT_EQUAL(ESP_OK, result.err);
    memset(&result, 0x00, sizeof(action_result_t));
    TEST_ASSERT_EQUAL(ESP_OK, esp_dispatcher_execute_with_func(dispatcher, nvs_action_commit, (void *)nvs_sync_handle, NULL, &result));
    TEST_ASSERT_EQUAL(ESP_OK, result.err);
    printf("Read I16\n");
    nvs_action_get_args_t get_i16 = {
        .key = "i16",
        .type = NVS_TYPE_I16,
        .wanted_size = sizeof(int16_t),
    };
    action_arg_t get_i16_arg = {
        .data = &get_i16,
        .len = sizeof(nvs_action_get_args_t),
    };
    memset(&result, 0x00, sizeof(action_result_t));
    TEST_ASSERT_EQUAL(ESP_OK, esp_dispatcher_execute_with_func(dispatcher, nvs_action_get, (void *)nvs_sync_handle, &get_i16_arg, &result));
    TEST_ASSERT_EQUAL(ESP_OK, result.err);
    TEST_ASSERT_NOT_NULL(result.data);
    int16_t *ret_i16 = (int16_t *)result.data;
    printf("Read [ %d ] from nvs\n", *ret_i16);
    free(result.data);

    // Test u32
    printf("Set U32\n");
    nvs_action_set_args_t set_u32 = {
        .key = "u32",
        .type = NVS_TYPE_U32,
        .value.u32 = 0xABCDABCD,
        .len = sizeof(uint32_t),
    };
    action_arg_t set_u32_arg = {
        .data = &set_u32,
        .len = sizeof(nvs_action_set_args_t),
    };
    memset(&result, 0x00, sizeof(action_result_t));
    TEST_ASSERT_EQUAL(ESP_OK, esp_dispatcher_execute_with_func(dispatcher, nvs_action_set, (void *)nvs_sync_handle, &set_u32_arg, &result));
    TEST_ASSERT_EQUAL(ESP_OK, result.err);
    memset(&result, 0x00, sizeof(action_result_t));
    TEST_ASSERT_EQUAL(ESP_OK, esp_dispatcher_execute_with_func(dispatcher, nvs_action_commit, (void *)nvs_sync_handle, NULL, &result));
    TEST_ASSERT_EQUAL(ESP_OK, result.err);
    printf("Read U32\n");
    nvs_action_get_args_t get_u32 = {
        .key = "u32",
        .type = NVS_TYPE_U32,
        .wanted_size = sizeof(uint32_t),
    };
    action_arg_t get_u32_arg = {
        .data = &get_u32,
        .len = sizeof(nvs_action_get_args_t),
    };
    memset(&result, 0x00, sizeof(action_result_t));
    TEST_ASSERT_EQUAL(ESP_OK, esp_dispatcher_execute_with_func(dispatcher, nvs_action_get, (void *)nvs_sync_handle, &get_u32_arg, &result));
    TEST_ASSERT_EQUAL(ESP_OK, result.err);
    TEST_ASSERT_NOT_NULL(result.data);
    uint32_t *ret_u32 = (uint32_t *)result.data;
    printf("Read [ 0x%X ] from nvs\n", *ret_u32);
    free(result.data);

    // Test i16
    printf("Set I32\n");
    nvs_action_set_args_t set_i32 = {
        .key = "i32",
        .type = NVS_TYPE_I32,
        .value.i32 = -3,
        .len = sizeof(int32_t),
    };
    action_arg_t set_i32_arg = {
        .data = &set_i32,
        .len = sizeof(nvs_action_set_args_t),
    };
    memset(&result, 0x00, sizeof(action_result_t));
    TEST_ASSERT_EQUAL(ESP_OK, esp_dispatcher_execute_with_func(dispatcher, nvs_action_set, (void *)nvs_sync_handle, &set_i32_arg, &result));
    TEST_ASSERT_EQUAL(ESP_OK, result.err);
    memset(&result, 0x00, sizeof(action_result_t));
    TEST_ASSERT_EQUAL(ESP_OK, esp_dispatcher_execute_with_func(dispatcher, nvs_action_commit, (void *)nvs_sync_handle, NULL, &result));
    TEST_ASSERT_EQUAL(ESP_OK, result.err);
    printf("Read I32\n");
    nvs_action_get_args_t get_i32 = {
        .key = "i32",
        .type = NVS_TYPE_I32,
        .wanted_size = sizeof(int32_t),
    };
    action_arg_t get_i32_arg = {
        .data = &get_i32,
        .len = sizeof(nvs_action_get_args_t),
    };
    memset(&result, 0x00, sizeof(action_result_t));
    TEST_ASSERT_EQUAL(ESP_OK, esp_dispatcher_execute_with_func(dispatcher, nvs_action_get, (void *)nvs_sync_handle, &get_i32_arg, &result));
    TEST_ASSERT_EQUAL(ESP_OK, result.err);
    TEST_ASSERT_NOT_NULL(result.data);
    int32_t *ret_i32 = (int32_t *)result.data;
    printf("Read [ %d ] from nvs\n", *ret_i32);
    free(result.data);

    // Test String
    printf("Set string\n");
    nvs_action_set_args_t set_str = {
        .key = "string",
        .type = NVS_TYPE_STR,
        .value.string = "Hello ESP dispatcher",
        .len = strlen("Hello ESP dispatcher"),
    };
    action_arg_t set_str_arg = {
        .data = &set_str,
        .len = sizeof(nvs_action_set_args_t),
    };
    memset(&result, 0x00, sizeof(action_result_t));
    TEST_ASSERT_EQUAL(ESP_OK, esp_dispatcher_execute_with_func(dispatcher, nvs_action_set, (void *)nvs_sync_handle, &set_str_arg, &result));
    TEST_ASSERT_EQUAL(ESP_OK, result.err);
    memset(&result, 0x00, sizeof(action_result_t));
    TEST_ASSERT_EQUAL(ESP_OK, esp_dispatcher_execute_with_func(dispatcher, nvs_action_commit, (void *)nvs_sync_handle, NULL, &result));
    TEST_ASSERT_EQUAL(ESP_OK, result.err);
    printf("Read string\n");
    nvs_action_get_args_t get_str = {
        .key = "string",
        .type = NVS_TYPE_STR,
        .wanted_size = 128,
    };
    action_arg_t get_str_arg = {
        .data = &get_str,
        .len = sizeof(nvs_action_get_args_t),
    };
    memset(&result, 0x00, sizeof(action_result_t));
    TEST_ASSERT_EQUAL(ESP_OK, esp_dispatcher_execute_with_func(dispatcher, nvs_action_get, (void *)nvs_sync_handle, &get_str_arg, &result));
    TEST_ASSERT_EQUAL(ESP_OK, result.err);
    TEST_ASSERT_NOT_NULL(result.data);
    char *ret_str = (char *)result.data;
    printf("Read [ %s ] from nvs\n", ret_str);
    free(result.data);

    // Test Blob
    typedef struct {
        int8_t  i8;
        int16_t i16;
        int32_t i32;
    } data_t;

    data_t dat = {
        .i8 = 0x11,
        .i16 = 0x2222,
        .i32 = 0x33333333,
    };
    printf("Set blob\n");
    nvs_action_set_args_t set_blob = {
        .key = "blob",
        .type = NVS_TYPE_BLOB,
        .value.blob = &dat,
        .len = sizeof(data_t),
    };
    action_arg_t set_blob_arg = {
        .data = &set_blob,
        .len = sizeof(nvs_action_set_args_t),
    };
    memset(&result, 0x00, sizeof(action_result_t));
    TEST_ASSERT_EQUAL(ESP_OK, esp_dispatcher_execute_with_func(dispatcher, nvs_action_set, (void *)nvs_sync_handle, &set_blob_arg, &result));
    TEST_ASSERT_EQUAL(ESP_OK, result.err);
    memset(&result, 0x00, sizeof(action_result_t));
    TEST_ASSERT_EQUAL(ESP_OK, esp_dispatcher_execute_with_func(dispatcher, nvs_action_commit, (void *)nvs_sync_handle, NULL, &result));
    TEST_ASSERT_EQUAL(ESP_OK, result.err);
    printf("Read blob\n");
    nvs_action_get_args_t get_blob = {
        .key = "blob",
        .type = NVS_TYPE_BLOB,
        .wanted_size = sizeof(data_t),
    };
    action_arg_t get_blob_arg = {
        .data = &get_blob,
        .len = sizeof(nvs_action_get_args_t),
    };
    memset(&result, 0x00, sizeof(action_result_t));
    TEST_ASSERT_EQUAL(ESP_OK, esp_dispatcher_execute_with_func(dispatcher, nvs_action_get, (void *)nvs_sync_handle, &get_blob_arg, &result));
    TEST_ASSERT_EQUAL(ESP_OK, result.err);
    TEST_ASSERT_NOT_NULL(result.data);
    data_t *ret_blob = (data_t *)result.data;
    printf("Read [ 0x%X, 0x%X, 0x%X ] from nvs\n", ret_blob->i8, ret_blob->i16, ret_blob->i32);
    free(result.data);

    // Close
    printf("Close NVS\n");
    memset(&result, 0x00, sizeof(action_result_t));
    TEST_ASSERT_EQUAL(ESP_OK, esp_dispatcher_execute_with_func(dispatcher, nvs_action_close, (void *)nvs_sync_handle, NULL, &result));
    TEST_ASSERT_EQUAL(ESP_OK, result.err);

    TEST_ASSERT_EQUAL(ESP_OK, esp_dispatcher_destroy(dispatcher));
    TEST_ASSERT_EQUAL(ESP_OK, nvs_flash_deinit());
}