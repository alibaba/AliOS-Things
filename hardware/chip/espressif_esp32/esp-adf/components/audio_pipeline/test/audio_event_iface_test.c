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
#include "audio_event_iface.h"
#include "esp_log.h"
#include "esp_err.h"
#include "audio_common.h"

static const char *TAG = "ESP_EMITTER_TEST";

static esp_err_t event_on_cmd(audio_event_iface_msg_t *msg, void *context)
{
    ESP_LOGI(TAG, "receive evt msg cmd = %d, source addr = %x, type = %d", msg->cmd, (int)msg->source, msg->source_type);
    if (msg->cmd == 9) {
        return ESP_FAIL;
    }
    return ESP_OK;
}

TEST_CASE("audio_event_iface", "esp-adf")
{
    ESP_LOGI(TAG, "[✓] audio_event_iface_init evt1");
    audio_event_iface_handle_t evt1;
    audio_event_iface_cfg_t cfg = AUDIO_EVENT_IFACE_DEFAULT_CFG();
    cfg.on_cmd = event_on_cmd;
    cfg.queue_size = 10;
    cfg.context = &evt1;
    evt1 = audio_event_iface_init(&cfg);
    TEST_ASSERT_NOT_NULL(evt1);

    audio_event_iface_msg_t msg;
    int i;
    ESP_LOGI(TAG, "[✓] dispatch 10 msg to evt1");
    for (i = 0; i < 10; i++) {
        msg.cmd = i;
        TEST_ASSERT_EQUAL(ESP_OK, audio_event_iface_cmd(evt1, &msg));
    }
    msg.cmd = 10;
    TEST_ASSERT_EQUAL(ESP_FAIL, audio_event_iface_cmd(evt1, &msg));

    ESP_LOGI(TAG, "[✓] listening 10 event have dispatched fron evt1");
    while (audio_event_iface_waiting_cmd_msg(evt1) == ESP_OK);

    ESP_LOGI(TAG, "[✓] audio_event_iface_init evt2");
    audio_event_iface_handle_t evt2;
    cfg.context = &evt2;
    cfg.type = AUDIO_ELEMENT_TYPE_PIPELINE;
    evt2 = audio_event_iface_init(&cfg);
    TEST_ASSERT_NOT_NULL(evt2);

    ESP_LOGI(TAG, "[✓] audio_event_iface_init evt3");
    audio_event_iface_handle_t evt3;
    cfg.context = &evt3;
    cfg.type = AUDIO_ELEMENT_TYPE_PLAYER;
    evt3 = audio_event_iface_init(&cfg);
    TEST_ASSERT_NOT_NULL(evt3);

    ESP_LOGI(TAG, "[✓] listen evt2 from evt1");
    TEST_ASSERT_EQUAL(ESP_OK, audio_event_iface_set_listener(evt2, evt1));

    ESP_LOGI(TAG, "[✓] listen evt3 from evt1");
    TEST_ASSERT_EQUAL(ESP_OK, audio_event_iface_set_listener(evt3, evt1));

    ESP_LOGI(TAG, "[✓] dispatch 2 msg to evt1");
    for (i = 0; i < 2; i++) {
        msg.source = evt1;
        msg.cmd = i;
        TEST_ASSERT_EQUAL(ESP_OK, audio_event_iface_cmd(evt1, &msg));
    }

    ESP_LOGI(TAG, "[✓] sendout 3 msg from evt2 to evt1");
    for (i = 2; i < 5; i++) {
        msg.source = evt2;
        msg.cmd = i;
        TEST_ASSERT_EQUAL(ESP_OK, audio_event_iface_sendout(evt2, &msg));
    }

    ESP_LOGI(TAG, "[✓] sendout 5 msg from evt3 to evt1");
    for (i = 5; i < 10; i++) {
        msg.source = evt3;
        msg.cmd = i;
        TEST_ASSERT_EQUAL(ESP_OK, audio_event_iface_sendout(evt3, &msg));
    }

    ESP_LOGI(TAG, "[✓] listening 10 event have dispatched from evt1, evt2 and evt3");
    while (audio_event_iface_waiting_cmd_msg(evt1) == ESP_OK);

    ESP_LOGI(TAG, "[✓] remove listener from evt2");
    TEST_ASSERT_EQUAL(ESP_OK, audio_event_iface_remove_listener(evt2, evt1));

    ESP_LOGI(TAG, "[✓] remove listener from evt3");
    TEST_ASSERT_EQUAL(ESP_OK, audio_event_iface_remove_listener(evt3, evt1));

    ESP_LOGI(TAG, "[✓] sendout 5 msg from evt2 to evt1");
    for (i = 1; i < 5; i++) {
        msg.source = evt2;
        msg.cmd = i;
        TEST_ASSERT_EQUAL(ESP_OK, audio_event_iface_sendout(evt2, &msg));
    }

    ESP_LOGI(TAG, "[✓] dispatch 5 msg to evt1");
    for (i = 5; i < 10; i++) {
        msg.source = evt1;
        msg.cmd = i;
        TEST_ASSERT_EQUAL(ESP_OK, audio_event_iface_cmd(evt1, &msg));
    }

    ESP_LOGI(TAG, "[✓] listening 10 event have dispatched from evt1 and evt2");
    while (audio_event_iface_listen(evt1) == ESP_OK);

    audio_event_iface_destroy(evt1);
    audio_event_iface_destroy(evt2);
    audio_event_iface_destroy(evt3);
}
