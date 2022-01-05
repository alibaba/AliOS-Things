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
#include "audio_element.h"
#include "esp_log.h"
#include "esp_err.h"

static const char *TAG = "AUDIO_ELEMENT_TEST";

static esp_err_t _el_open(audio_element_handle_t self)
{
    ESP_LOGI(TAG, "_el_open");
    return ESP_OK;
}

static int _el_read(audio_element_handle_t self, char *buffer, int len, TickType_t ticks_to_wait)
{
    ESP_LOGI(TAG, "_el_read");
    return len;
}

static int _el_process(audio_element_handle_t self, char *in_buffer, int in_len, char *out_buffer, int out_len)
{
    ESP_LOGI(TAG, "_el_process");
    vTaskDelay(100/portTICK_RATE_MS);
    return in_len;
}

static int _el_write(audio_element_handle_t self, char *buffer, int len, TickType_t ticks_to_wait)
{
    ESP_LOGI(TAG, "_el_write");
    return len;
}

static esp_err_t _el_close(audio_element_handle_t self)
{
    ESP_LOGI(TAG, "_el_close");
    return ESP_OK;
}

TEST_CASE("audio_element", "esp-adf")
{
    esp_log_level_set("*", ESP_LOG_INFO);
    esp_log_level_set("AUDIO_ELEMENT", ESP_LOG_VERBOSE);

    ESP_LOGI(TAG, "[✓] audio_element_init element");
    audio_element_handle_t el;
    audio_element_cfg_t cfg = DEFAULT_AUDIO_ELEMENT_CONFIG();

    cfg.open = _el_open;
    cfg.read = _el_read;
    cfg.process = _el_process;
    cfg.write = _el_write;
    cfg.close = _el_close;
    el = audio_element_init(&cfg);


    TEST_ASSERT_NOT_NULL(el);
    TEST_ASSERT_EQUAL(ESP_OK, audio_element_run(el));
    TEST_ASSERT_EQUAL(ESP_OK, audio_element_resume(el, 0, 0));
    vTaskDelay(2000/portTICK_RATE_MS);
    TEST_ASSERT_EQUAL(ESP_OK, audio_element_pause(el));
    vTaskDelay(2000/portTICK_RATE_MS);
    TEST_ASSERT_EQUAL(ESP_OK, audio_element_resume(el, 0, 0));
    vTaskDelay(2000/portTICK_RATE_MS);

    TEST_ASSERT_EQUAL(ESP_OK, audio_element_stop(el));
    TEST_ASSERT_EQUAL(ESP_OK, audio_element_deinit(el));
}

TEST_CASE("audio_element_input_rb", "esp-adf")
{
    esp_log_level_set("*", ESP_LOG_INFO);
    esp_log_level_set("AUDIO_ELEMENT", ESP_LOG_VERBOSE);

    ESP_LOGI(TAG, "[✓] audio_element_init element");
    RingbufHandle_t input_rb;
    audio_element_handle_t el;
    audio_element_cfg_t cfg = DEFAULT_AUDIO_ELEMENT_CONFIG();

    cfg.open = _el_open;
    // cfg.read = _el_read;
    cfg.process = _el_process;
    cfg.write = _el_write;
    cfg.close = _el_close;
    el = audio_element_init(&cfg);
    input_rb = xRingbufferCreate(4096, RINGBUF_TYPE_NOSPLIT);
    audio_element_set_input_ringbuf(el, input_rb);

    TEST_ASSERT_NOT_NULL(el);
    TEST_ASSERT_EQUAL(ESP_OK, audio_element_run(el));
    TEST_ASSERT_EQUAL(ESP_OK, audio_element_resume(el, 0, 0));
    vTaskDelay(2000/portTICK_RATE_MS);
    TEST_ASSERT_EQUAL(ESP_OK, audio_element_pause(el));
    vTaskDelay(2000/portTICK_RATE_MS);
    TEST_ASSERT_EQUAL(ESP_OK, audio_element_resume(el, 0, 0));
    vTaskDelay(2000/portTICK_RATE_MS);

    TEST_ASSERT_EQUAL(ESP_OK, audio_element_stop(el));
    TEST_ASSERT_EQUAL(ESP_OK, audio_element_deinit(el));
    vRingbufferDelete(input_rb);
}

TEST_CASE("audio_element_output_rb", "esp-adf")
{
    esp_log_level_set("*", ESP_LOG_INFO);
    esp_log_level_set("AUDIO_ELEMENT", ESP_LOG_VERBOSE);

    ESP_LOGI(TAG, "[✓] audio_element_init element");
    RingbufHandle_t output_rb;
    audio_element_handle_t el;
    audio_element_cfg_t cfg = DEFAULT_AUDIO_ELEMENT_CONFIG();

    cfg.open = _el_open;
    cfg.read = _el_read;
    cfg.process = _el_process;
    // cfg.write = _el_write;
    cfg.close = _el_close;
    el = audio_element_init(&cfg);
    output_rb = xRingbufferCreate(4096, RINGBUF_TYPE_NOSPLIT);
    audio_element_set_output_ringbuf(el, output_rb);

    TEST_ASSERT_NOT_NULL(el);
    TEST_ASSERT_EQUAL(ESP_OK, audio_element_run(el));
    TEST_ASSERT_EQUAL(ESP_OK, audio_element_resume(el, 0, 0));
    vTaskDelay(2000/portTICK_RATE_MS);
    TEST_ASSERT_EQUAL(ESP_OK, audio_element_pause(el));
    vTaskDelay(2000/portTICK_RATE_MS);
    TEST_ASSERT_EQUAL(ESP_OK, audio_element_resume(el, 0, 0));
    vTaskDelay(2000/portTICK_RATE_MS);

    TEST_ASSERT_EQUAL(ESP_OK, audio_element_stop(el));
    TEST_ASSERT_EQUAL(ESP_OK, audio_element_deinit(el));
    vRingbufferDelete(output_rb);
}


TEST_CASE("audio_element_input_output_rb", "esp-adf")
{
    esp_log_level_set("*", ESP_LOG_INFO);
    esp_log_level_set("AUDIO_ELEMENT", ESP_LOG_VERBOSE);

    ESP_LOGI(TAG, "[✓] audio_element_init element");
    RingbufHandle_t input_rb, output_rb;
    audio_element_handle_t el;
    audio_element_cfg_t cfg = DEFAULT_AUDIO_ELEMENT_CONFIG();

    cfg.open = _el_open;
    // cfg.read = _el_read;
    cfg.process = _el_process;
    // cfg.write = _el_write;
    cfg.close = _el_close;
    el = audio_element_init(&cfg);
    output_rb = xRingbufferCreate(4096, RINGBUF_TYPE_NOSPLIT);
    input_rb = xRingbufferCreate(4096, RINGBUF_TYPE_NOSPLIT);
    audio_element_set_input_ringbuf(el, input_rb);
    audio_element_set_output_ringbuf(el, output_rb);

    TEST_ASSERT_NOT_NULL(el);
    TEST_ASSERT_EQUAL(ESP_OK, audio_element_run(el));
    TEST_ASSERT_EQUAL(ESP_OK, audio_element_resume(el, 0, 0));
    vTaskDelay(2000/portTICK_RATE_MS);
    TEST_ASSERT_EQUAL(ESP_OK, audio_element_pause(el));
    vTaskDelay(2000/portTICK_RATE_MS);
    TEST_ASSERT_EQUAL(ESP_OK, audio_element_resume(el, 0, 0));
    vTaskDelay(2000/portTICK_RATE_MS);

    TEST_ASSERT_EQUAL(ESP_OK, audio_element_stop(el));
    TEST_ASSERT_EQUAL(ESP_OK, audio_element_deinit(el));
    vRingbufferDelete(input_rb);
    vRingbufferDelete(output_rb);
}
