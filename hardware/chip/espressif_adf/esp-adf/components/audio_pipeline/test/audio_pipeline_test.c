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

#include <pthread.h>
#include "unity.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "audio_pipeline.h"
#include "esp_log.h"
#include "esp_err.h"

static const char *TAG = "AUDIO_ELEMENT_TEST";

static esp_err_t _el_open(audio_element_handle_t self)
{
    ESP_LOGI(TAG, "[%s] _el_open", audio_element_get_tag(self));
    return ESP_OK;
}

static int _el_read(audio_element_handle_t self, char *buffer, int len, TickType_t ticks_to_wait)
{
    ESP_LOGI(TAG, "[%s] _el_read", audio_element_get_tag(self));
    return len;
}

static int _el_process(audio_element_handle_t self, char *in_buffer, int in_len, char *out_buffer, int out_len)
{
    ESP_LOGI(TAG, "[%s] _el_process, in_len=%d, outlen=%d", audio_element_get_tag(self), in_len, out_len);
    vTaskDelay(100/portTICK_RATE_MS);
    return in_len;
}

static int _el_write(audio_element_handle_t self, char *buffer, int len, TickType_t ticks_to_wait)
{
    ESP_LOGI(TAG, "[%s] _el_write", audio_element_get_tag(self));
    return len;
}

static esp_err_t _el_close(audio_element_handle_t self)
{
    ESP_LOGI(TAG, "[%s] _el_close", audio_element_get_tag(self));
    return ESP_OK;
}

TEST_CASE("audio_pipeline", "esp-adf")
{
    esp_log_level_set("*", ESP_LOG_INFO);
    esp_log_level_set("AUDIO_PIPELINE", ESP_LOG_VERBOSE);
    esp_log_level_set("AUDIO_ELEMENT", ESP_LOG_VERBOSE);
    esp_log_level_set("AUDIO_EVENT", ESP_LOG_VERBOSE);

    ESP_LOGI(TAG, "[✓] init audio_pipeline and audio_element");
    audio_element_handle_t first_el, mid_el, last_el;
    audio_element_cfg_t el_cfg = DEFAULT_AUDIO_ELEMENT_CONFIG();

    el_cfg.open = _el_open;
    el_cfg.read = _el_read;
    el_cfg.process = _el_process;
    el_cfg.close = _el_close;
    first_el = audio_element_init(&el_cfg);
    TEST_ASSERT_NOT_NULL(first_el);

    el_cfg.read = NULL;
    el_cfg.write = NULL;
    mid_el = audio_element_init(&el_cfg);
    TEST_ASSERT_NOT_NULL(mid_el);

    el_cfg.write = _el_write;
    last_el = audio_element_init(&el_cfg);
    TEST_ASSERT_NOT_NULL(last_el);

    audio_pipeline_cfg_t pipeline_cfg = DEFAULT_AUDIO_PIPELINE_CONFIG();
    audio_pipeline_handle_t pipeline = audio_pipeline_init(&pipeline_cfg);
    TEST_ASSERT_NOT_NULL(pipeline);

    
    TEST_ASSERT_EQUAL(ESP_OK, audio_pipeline_register(pipeline, first_el, "first"));
    TEST_ASSERT_EQUAL(ESP_OK, audio_pipeline_register(pipeline, mid_el, "mid"));
    TEST_ASSERT_EQUAL(ESP_OK, audio_pipeline_register(pipeline, last_el, "last"));

    TEST_ASSERT_EQUAL(ESP_OK, audio_pipeline_link(pipeline, (const char *[]){"first", "mid", "last"}, 3));

    TEST_ASSERT_EQUAL(ESP_OK, audio_pipeline_run(pipeline));

    vTaskDelay(10000/portTICK_RATE_MS);
    TEST_ASSERT_EQUAL(ESP_OK, audio_pipeline_stop(pipeline));
    TEST_ASSERT_EQUAL(ESP_OK, audio_pipeline_wait_for_stop(pipeline));
    TEST_ASSERT_EQUAL(ESP_OK, audio_pipeline_unlink(pipeline));
    TEST_ASSERT_EQUAL(ESP_OK, audio_pipeline_run(pipeline));
    vTaskDelay(5000/portTICK_RATE_MS);
    TEST_ASSERT_EQUAL(ESP_OK, audio_pipeline_stop(pipeline));
    TEST_ASSERT_EQUAL(ESP_OK, audio_pipeline_wait_for_stop(pipeline));
    TEST_ASSERT_EQUAL(ESP_OK, audio_pipeline_deinit(pipeline));
    TEST_ASSERT_EQUAL(ESP_OK, audio_element_deinit(first_el));
    TEST_ASSERT_EQUAL(ESP_OK, audio_element_deinit(mid_el));
    TEST_ASSERT_EQUAL(ESP_OK, audio_element_deinit(last_el));

}
