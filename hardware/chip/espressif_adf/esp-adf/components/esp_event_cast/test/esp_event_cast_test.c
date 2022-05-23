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
#include "unity.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "esp_event_cast.h"
#include "esp_log.h"
#include "audio_mem.h"

static const char *TAG = "EVT_CAST_TEST";
#define TEST_QUEUE_NUMBER 50

static void task_send(void *pv)
{
    esp_event_cast_handle_t broadcast = (esp_event_cast_handle_t)pv;
    int buf[3] = {0};
    int i = 0x33;
    while (1) {
        vTaskDelay(5000 / portTICK_RATE_MS);
        memset(&buf, i++, sizeof(buf));
        esp_event_cast_broadcasting(broadcast, &buf);
        printf("%s, i = %x, count=%x\r\n", __func__, i, esp_event_cast_get_count(broadcast));
        AUDIO_MEM_SHOW(TAG);
    }
    vTaskDelete(NULL);
}

static void task_recv0(void *pv)
{
    esp_event_cast_handle_t broadcast = (esp_event_cast_handle_t)pv;
    xQueueHandle que = xQueueCreate(3, 12);

    int buf[3] = {0};
    while (buf[0] != (500)) {
        esp_event_cast_register(broadcast, que);
        xQueueReceive(que, & buf, portMAX_DELAY);
        esp_event_cast_unregister(broadcast, que);
        printf("[%8s], que = %p, data=%d, %d, %d\r\n", __func__, que, buf[0], buf[1], buf[2]);
    }
    vQueueDelete(que);
    vTaskDelete(NULL);
}

static void task_recv1(void *pv)
{
    esp_event_cast_handle_t broadcast = (esp_event_cast_handle_t)pv;
    xQueueHandle que = xQueueCreate(3, 12);
    esp_event_cast_register(broadcast, que);
    int buf[3] = {0};
    while (buf[0] != (50)) {
        xQueueReceive(que, & buf, portMAX_DELAY);
        printf("[%8s], que = %p, data=%d, %d, %d\r\n", __func__, que, buf[0], buf[1], buf[2]);
    }
    esp_event_cast_unregister(broadcast, que);
    vQueueDelete(que);
    vTaskDelete(NULL);
}

static void task_recv2(void *pv)
{
    esp_event_cast_handle_t broadcast = (esp_event_cast_handle_t)pv;
    xQueueHandle que = xQueueCreate(3, 12);
    esp_event_cast_register(broadcast, que);
    int buf[3] = {0};
    while (buf[0] != (50)) {
        xQueueReceive(que, & buf, portMAX_DELAY);
        printf("[%8s], que = %p, data=%d, %d, %d\r\n", __func__, que, buf[0], buf[1], buf[2]);
    }
    esp_event_cast_unregister(broadcast, que);
    vQueueDelete(que);
    vTaskDelete(NULL);
}

static void task_recv3(void *pv)
{
    esp_event_cast_handle_t broadcast = (esp_event_cast_handle_t)pv;
    xQueueHandle que = xQueueCreate(3, 12);
    esp_event_cast_register(broadcast, que);
    int buf[3] = {0};
    while (buf[0] != (50)) {
        xQueueReceive(que, & buf, portMAX_DELAY);
        printf("[%8s], que = %p, data=%d, %d, %d\r\n", __func__, que, buf[0], buf[1], buf[2]);
    }
    esp_event_cast_unregister(broadcast, que);
    vQueueDelete(que);
    vTaskDelete(NULL);
}

static void task_recv4(void *pv)
{
    esp_event_cast_handle_t broadcast = (esp_event_cast_handle_t)pv;
    xQueueHandle que = xQueueCreate(3, 12);
    esp_event_cast_register(broadcast, que);
    int buf[3] = {0};
    while (buf[0] != (50)) {
        xQueueReceive(que, &buf, portMAX_DELAY);
        printf("[%8s], que = %p, data=%d, %d, %d\r\n", __func__, que, buf[0], buf[1], buf[2]);
    }
    esp_event_cast_unregister(broadcast, que);
    vQueueDelete(que);
    vTaskDelete(NULL);
}

TEST_CASE("create and destroy", "[esp_event_cast]")
{
    ESP_LOGI(TAG, "[✓] setup Wi-Fi and SDcard");
    AUDIO_MEM_SHOW(TAG);
    esp_event_cast_handle_t broadcast = NULL;
    for (int i = 0; i < TEST_QUEUE_NUMBER; ++i) {
        broadcast = esp_event_cast_create();
        esp_event_cast_destroy(broadcast);
    }
    AUDIO_MEM_SHOW(TAG);

    xQueueHandle que_handle[TEST_QUEUE_NUMBER];
    for (int i = 0; i < TEST_QUEUE_NUMBER; ++i) {
        que_handle[i] = xQueueCreate(3, 12);
    }
    for (int i = 0; i < 20; ++i) {
        broadcast = esp_event_cast_create();
        for (int i = 0; i < TEST_QUEUE_NUMBER; ++i) {
            esp_event_cast_register(broadcast, que_handle[i]);
        }
        ESP_LOGI(TAG, "count is %d", esp_event_cast_get_count(broadcast));
        esp_event_cast_destroy(broadcast);
    }
    for (int i = 0; i < TEST_QUEUE_NUMBER; ++i) {
        vQueueDelete(que_handle[i]);
        que_handle[i] = NULL;
    }
    AUDIO_MEM_SHOW(TAG);
}

TEST_CASE("register and unregister", "[esp_event_cast]")
{
    ESP_LOGI(TAG, "[✓] setup Wi-Fi and SDcard");
    AUDIO_MEM_SHOW(TAG);
    esp_event_cast_handle_t broadcast = NULL;
    xQueueHandle que_handle[TEST_QUEUE_NUMBER];
    for (int i = 0; i < TEST_QUEUE_NUMBER; ++i) {
        que_handle[i] = xQueueCreate(3, 12);
    }
    for (int i = 0; i < 20; ++i) {
        broadcast = esp_event_cast_create();
        for (int i = 0; i < TEST_QUEUE_NUMBER; ++i) {
            esp_event_cast_register(broadcast, que_handle[i]);
        }
        ESP_LOGI(TAG, "count is %d", esp_event_cast_get_count(broadcast));
        for (int i = 0; i < TEST_QUEUE_NUMBER / 2; ++i) {
            esp_event_cast_unregister(broadcast, que_handle[i]);
        }
        ESP_LOGI(TAG, "count is %d", esp_event_cast_get_count(broadcast));
        esp_event_cast_destroy(broadcast);
    }
    for (int i = 0; i < TEST_QUEUE_NUMBER; ++i) {
        vQueueDelete(que_handle[i]);
        que_handle[i] = NULL;
    }
    AUDIO_MEM_SHOW(TAG);
}

TEST_CASE("broadcasting from task and receiver", "[esp_event_cast]")
{
    ESP_LOGI(TAG, "[✓] setup Wi-Fi and SDcard");
    AUDIO_MEM_SHOW(TAG);
    esp_event_cast_handle_t broadcast = esp_event_cast_create();
    xTaskCreate(task_recv1,
                "task_recv1",
                2048,
                broadcast,
                1,
                NULL);
    xTaskCreate(task_recv2,
                "task_recv2",
                2048,
                broadcast,
                3,
                NULL);
    xTaskCreate(task_recv3,
                "task_recv3",
                2048,
                broadcast,
                23,
                NULL);
    xTaskCreate(task_recv4,
                "task_recv4",
                2048,
                broadcast,
                10,
                NULL);
    AUDIO_MEM_SHOW(TAG);

    int buf[3] = {0};
    for (int i = 20; i <= 50; ++i) {
        vTaskDelay(1000 / portTICK_RATE_MS);
        buf[0] = i;
        buf[1] = i;
        buf[2] = i;
        esp_event_cast_broadcasting(broadcast, &buf);
        printf("[Broadcast data], i = %d, count=%x\r\n", i, esp_event_cast_get_count(broadcast));
        AUDIO_MEM_SHOW(TAG);
    }
    vTaskDelay(3000 / portTICK_PERIOD_MS);
    AUDIO_MEM_SHOW(TAG);
    esp_event_cast_destroy(broadcast);
}


TEST_CASE("Received data in real time", "[esp_event_cast]")
{
    ESP_LOGI(TAG, "[✓] setup Wi-Fi and SDcard");
    AUDIO_MEM_SHOW(TAG);
    esp_event_cast_handle_t broadcast = esp_event_cast_create();
    xTaskCreate(task_recv0,
                "task_recv0",
                2048,
                broadcast,
                15,
                NULL);

    int buf[3] = {0};
    for (int i = 1; i <= 500; ++i) {
        // vTaskDelay(1000 / portTICK_RATE_MS);
        buf[0] = i;
        buf[1] = i;
        buf[2] = i;
        esp_event_cast_broadcasting(broadcast, &buf);
        printf("[Broadcast data], i = %d, count=%x\r\n", i, esp_event_cast_get_count(broadcast));
        AUDIO_MEM_SHOW(TAG);
    }
    vTaskDelay(3000 / portTICK_PERIOD_MS);
    AUDIO_MEM_SHOW(TAG);
    esp_event_cast_destroy(broadcast);
}