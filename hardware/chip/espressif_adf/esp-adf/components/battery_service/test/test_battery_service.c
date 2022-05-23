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

#include <string.h>

#include "esp_log.h"
#include "audio_mem.h"
#include "board.h"
#include "audio_hal.h"
#include "unity.h"
#include "test_utils.h"
#include "driver/adc.h"
#include "esp_adc_cal.h"
#include "battery_service.h"

static const char *TAG = "battery_test";

TEST_CASE("create without configuration", "[battery_service]")
{
    esp_log_level_set("BATTERY_SERVICE", ESP_LOG_NONE);
    AUDIO_MEM_SHOW("MEMORY BEFORE");
    int cnt = 10000;
    while (cnt--) {
        periph_service_handle_t battery_service = battery_service_create(NULL);
        TEST_ASSERT_NULL(battery_service);
    }
    AUDIO_MEM_SHOW("MEMORY AFTER");
}

TEST_CASE("create with configuration all zero", "[battery_service]")
{
    battery_service_config_t battery_config = { 0 };
    AUDIO_MEM_SHOW("MEMORY BEFORE");
    int cnt = 10000;
    while (cnt--) {
        periph_service_handle_t battery_service = battery_service_create(&battery_config);
        TEST_ASSERT_NULL(battery_service);
    }
    AUDIO_MEM_SHOW("MEMORY AFTER");
}

TEST_CASE("create with configration default", "[battery_service]")
{
    battery_service_config_t battery_config = BATTERY_SERVICE_DEFAULT_CONFIG();

    esp_log_level_set("BATTERY_SERVICE", ESP_LOG_NONE);
    AUDIO_MEM_SHOW("MEMORY BEFORE");
    int cnt = 10000;
    while (cnt--) {
        periph_service_handle_t battery_service = battery_service_create(&battery_config);
        TEST_ASSERT_NOT_NULL(battery_service);
        TEST_ASSERT_EQUAL(ESP_OK, periph_service_destroy(battery_service));
        vTaskDelay(1);
    }
    AUDIO_MEM_SHOW("MEMORY AFTER");
}

typedef struct {
    int unit;  /*!< ADC unit, see `adc_unit_t` */
    int chan;  /*!< ADC channel, see `adc_channel_t` */
    int width; /*!< ADC width, see `adc_channel_t` */
    int atten; /*!< ADC atten, see `adc_atten_t` */
    int v_ref; /*!< default vref` */
} vol_adc_param_t;

static bool adc_init(void *user_data)
{
    vol_adc_param_t *adc_cfg = (vol_adc_param_t *)user_data;
    adc1_config_width(adc_cfg->width);
    adc1_config_channel_atten(adc_cfg->chan, adc_cfg->atten);
    return true;
}

static bool adc_deinit(void *user_data)
{
    return true;
}

static int vol_read(void *user_data)
{
#define ADC_SAMPLES_NUM (10)

    vol_adc_param_t *adc_cfg = (vol_adc_param_t *)user_data;

    uint32_t data[ADC_SAMPLES_NUM] = { 0 };
    uint32_t sum = 0;
    int tmp = 0;

#if CONFIG_IDF_TARGET_ESP32
    esp_adc_cal_characteristics_t characteristics;
    esp_adc_cal_characterize(adc_cfg->unit, adc_cfg->atten, adc_cfg->width, adc_cfg->v_ref, &characteristics);
    for (int i = 0; i < ADC_SAMPLES_NUM; ++i) {
        esp_adc_cal_get_voltage(adc_cfg->chan, &characteristics, &data[i]);
    }
#elif CONFIG_IDF_TARGET_ESP32S2
    for (int i = 0; i < ADC_SAMPLES_NUM; i++) {
        data[i] = adc1_get_raw((adc1_channel_t)channel);
    }
#endif

    for (int j = 0; j < ADC_SAMPLES_NUM - 1; j++) {
        for (int i = 0; i < ADC_SAMPLES_NUM - j - 1; i++) {
            if (data[i] > data[i + 1]) {
                tmp = data[i];
                data[i] = data[i + 1];
                data[i + 1] = tmp;
            }
        }
    }
    for (int num = 1; num < ADC_SAMPLES_NUM - 1; num++)
        sum += data[num];
    return (sum / (ADC_SAMPLES_NUM - 2));
}


TEST_CASE("voltage monitor configure test", "[battery_service]")
{
    vol_adc_param_t adc_cfg = {
        .unit = ADC_UNIT_1,
        .chan = ADC_CHANNEL_1,
        .width = ADC_WIDTH_BIT_12,
        .atten = ADC_ATTEN_11db,
        .v_ref = 1100,
    };

    vol_monitor_param_t vol_monitor_cfg = {
        .init = adc_init,
        .deinit = adc_deinit,
        .vol_get = vol_read,
        .read_freq = 2,
        .report_freq = 2,
        .vol_full_threshold = 0,
        .vol_low_threshold = 0,
    };
    periph_service_handle_t battery_service = NULL;

    battery_service_config_t battery_config = BATTERY_SERVICE_DEFAULT_CONFIG();
    vol_monitor_handle_t vol_monitor = NULL;

    vol_monitor_cfg.init = NULL;
    TEST_ASSERT_NULL(vol_monitor_create(&vol_monitor_cfg));
    vol_monitor_cfg.init = adc_init;
    printf("\r\n");

    vol_monitor_cfg.deinit = NULL;
    TEST_ASSERT_NULL(vol_monitor_create(&vol_monitor_cfg));
    vol_monitor_cfg.deinit = adc_deinit;
    printf("\r\n");

    vol_monitor_cfg.vol_get = NULL;
    TEST_ASSERT_NULL(vol_monitor_create(&vol_monitor_cfg));
    vol_monitor_cfg.vol_get = vol_read;
    printf("\r\n");

    vol_monitor_cfg.user_data = audio_calloc(1, sizeof(vol_adc_param_t));
    AUDIO_MEM_CHECK(TAG, vol_monitor_cfg.user_data, return);
    memcpy(vol_monitor_cfg.user_data, &adc_cfg, sizeof(vol_adc_param_t));

    // read_freq
    vol_monitor_cfg.read_freq = -1;
    TEST_ASSERT_NULL(vol_monitor_create(&vol_monitor_cfg));
    printf("\r\n");

    vol_monitor_cfg.read_freq = 0;
    TEST_ASSERT_NULL(vol_monitor_create(&vol_monitor_cfg));
    printf("\r\n");

    vol_monitor_cfg.read_freq = 100;
    TEST_ASSERT_NOT_NULL((vol_monitor = vol_monitor_create(&vol_monitor_cfg)));
    TEST_ASSERT_EQUAL(ESP_OK, vol_monitor_destroy(vol_monitor));
    vol_monitor = NULL;
    printf("\r\n");

    // report_freq
    vol_monitor_cfg.report_freq = -1;
    TEST_ASSERT_NULL(vol_monitor_create(&vol_monitor_cfg));
    printf("\r\n");

    vol_monitor_cfg.report_freq = 0;
    TEST_ASSERT_NOT_NULL((vol_monitor = vol_monitor_create(&vol_monitor_cfg)));
    TEST_ASSERT_EQUAL(ESP_OK, vol_monitor_destroy(vol_monitor));
    vol_monitor = NULL;
    printf("\r\n");

    vol_monitor_cfg.report_freq = 100;
    TEST_ASSERT_NOT_NULL((vol_monitor = vol_monitor_create(&vol_monitor_cfg)));
    TEST_ASSERT_EQUAL(ESP_OK, vol_monitor_destroy(vol_monitor));
    vol_monitor = NULL;
    printf("\r\n");

    // full
    vol_monitor_cfg.vol_full_threshold = -1;
    TEST_ASSERT_NULL(vol_monitor_create(&vol_monitor_cfg));
    printf("\r\n");

    vol_monitor_cfg.vol_full_threshold = 0;
    TEST_ASSERT_NOT_NULL((vol_monitor = vol_monitor_create(&vol_monitor_cfg)));
    TEST_ASSERT_EQUAL(ESP_OK, vol_monitor_destroy(vol_monitor));
    vol_monitor = NULL;
    printf("\r\n");

    vol_monitor_cfg.vol_full_threshold = 100;
    TEST_ASSERT_NOT_NULL((vol_monitor = vol_monitor_create(&vol_monitor_cfg)));
    TEST_ASSERT_EQUAL(ESP_OK, vol_monitor_destroy(vol_monitor));
    vol_monitor = NULL;
    printf("\r\n");

    // low
    vol_monitor_cfg.vol_low_threshold = -1;
    TEST_ASSERT_NULL(vol_monitor_create(&vol_monitor_cfg));
    printf("\r\n");

    vol_monitor_cfg.vol_low_threshold = 0;
    TEST_ASSERT_NOT_NULL((vol_monitor = vol_monitor_create(&vol_monitor_cfg)));
    TEST_ASSERT_EQUAL(ESP_OK, vol_monitor_destroy(vol_monitor));
    printf("\r\n");

    vol_monitor_cfg.vol_low_threshold = 50;
    TEST_ASSERT_NOT_NULL((vol_monitor = vol_monitor_create(&vol_monitor_cfg)));
    TEST_ASSERT_EQUAL(ESP_OK, vol_monitor_destroy(vol_monitor));
    vol_monitor = NULL;
    printf("\r\n");

    vol_monitor_cfg.vol_low_threshold = 100;
    TEST_ASSERT_NULL(vol_monitor_create(&vol_monitor_cfg));
    printf("\r\n");

    vol_monitor_cfg.vol_low_threshold = 200;
    TEST_ASSERT_NULL(vol_monitor_create(&vol_monitor_cfg));
    printf("\r\n");

    free(vol_monitor_cfg.user_data);
}

#define BATTERY_FREQ_REPORT (BIT0)
#define BATTERY_FULL_REPORT (BIT1)
#define BATTERY_LOW_REPORT  (BIT2)

EventGroupHandle_t sync_events;

static esp_err_t battery_service_cb(periph_service_handle_t handle, periph_service_event_t *evt, void *ctx)
{
    if (evt->type == BAT_SERV_EVENT_VOL_REPORT) {
        ESP_LOGI(TAG, "GOT VOL REPORT");
        xEventGroupSetBits(sync_events, BATTERY_FREQ_REPORT);
    } else if (evt->type == BAT_SERV_EVENT_BAT_FULL) {
        ESP_LOGI(TAG, "GOT FULL REPORT");
        xEventGroupSetBits(sync_events, BATTERY_FULL_REPORT);
    } else if (evt->type == BAT_SERV_EVENT_BAT_LOW) {
        ESP_LOGI(TAG, "GOT LOW REPORT");
        xEventGroupSetBits(sync_events, BATTERY_LOW_REPORT);
    } else {
        ESP_LOGE(TAG, "error message");
    }
    return ESP_OK;
}

TEST_CASE("voltage monitor freq report", "[battery_service]")
{
    sync_events = xEventGroupCreate();
    TEST_ASSERT_NOT_NULL(sync_events);

    vol_adc_param_t adc_cfg = {
        .unit = ADC_UNIT_1,
        .chan = ADC_CHANNEL_1,
        .width = ADC_WIDTH_BIT_12,
        .atten = ADC_ATTEN_11db,
        .v_ref = 1100,
    };

    vol_monitor_param_t vol_monitor_cfg = {
        .init = adc_init,
        .deinit = adc_deinit,
        .vol_get = vol_read,
        .read_freq = 1,
        .report_freq = 1,
        .vol_full_threshold = 0,
        .vol_low_threshold = 0,
    };
    vol_monitor_cfg.user_data = audio_calloc(1, sizeof(vol_adc_param_t));
    AUDIO_MEM_CHECK(TAG, vol_monitor_cfg.user_data, return);
    memcpy(vol_monitor_cfg.user_data, &adc_cfg, sizeof(vol_adc_param_t));

    battery_service_config_t battery_config = BATTERY_SERVICE_DEFAULT_CONFIG();
    vol_monitor_handle_t vol_monitor = vol_monitor_create(&vol_monitor_cfg);
    TEST_ASSERT_NOT_NULL(vol_monitor);
    battery_config.vol_monitor = vol_monitor;
    battery_config.evt_cb = battery_service_cb;

    periph_service_handle_t battery_service = battery_service_create(&battery_config);
    TEST_ASSERT_NOT_NULL(battery_service);
    TEST_ASSERT_EQUAL(ESP_OK, periph_service_start(battery_service));
    TEST_ASSERT_EQUAL(ESP_OK, battery_service_vol_report_switch(battery_service, true));
    EventBits_t bits = xEventGroupWaitBits(sync_events, BATTERY_FREQ_REPORT, true, true, pdMS_TO_TICKS(4000));
    TEST_ASSERT_EQUAL(BATTERY_FREQ_REPORT, bits & BATTERY_FREQ_REPORT);

    TEST_ASSERT_EQUAL(ESP_OK, battery_service_set_vol_report_freq(battery_service, 2));
    xEventGroupClearBits(sync_events, BATTERY_FREQ_REPORT);
    bits = xEventGroupWaitBits(sync_events, BATTERY_FREQ_REPORT, true, true, pdMS_TO_TICKS(4000));
    TEST_ASSERT_EQUAL(BATTERY_FREQ_REPORT, bits & BATTERY_FREQ_REPORT);

    TEST_ASSERT_EQUAL(ESP_OK, battery_service_vol_report_switch(battery_service, false));
    bits = xEventGroupWaitBits(sync_events, BATTERY_FREQ_REPORT, true, true, pdMS_TO_TICKS(4000));
    TEST_ASSERT_NOT_EQUAL(BATTERY_FREQ_REPORT, bits & BATTERY_FREQ_REPORT);

    TEST_ASSERT_EQUAL(ESP_OK, periph_service_destroy(battery_service));
    TEST_ASSERT_EQUAL(ESP_OK, vol_monitor_destroy(vol_monitor));
    vol_monitor = NULL;

    vol_monitor_cfg.report_freq = 0;
    vol_monitor = vol_monitor_create(&vol_monitor_cfg);
    TEST_ASSERT_NOT_NULL(vol_monitor);
    battery_config.vol_monitor = vol_monitor;
    battery_service = battery_service_create(&battery_config);
    TEST_ASSERT_NOT_NULL(battery_service);
    TEST_ASSERT_EQUAL(ESP_OK, periph_service_start(battery_service));
    TEST_ASSERT_EQUAL(ESP_OK, battery_service_vol_report_switch(battery_service, true));
    bits = xEventGroupWaitBits(sync_events, BATTERY_FREQ_REPORT, true, true, pdMS_TO_TICKS(4000));
    TEST_ASSERT_EQUAL(0x00000000, bits & BATTERY_FREQ_REPORT);
    TEST_ASSERT_EQUAL(ESP_OK, periph_service_destroy(battery_service));
    TEST_ASSERT_EQUAL(ESP_OK, vol_monitor_destroy(vol_monitor));
    vol_monitor = NULL;

    vEventGroupDelete(sync_events);
    free(vol_monitor_cfg.user_data);
}

TEST_CASE("voltage monitor battery full message", "[battery_service]")
{
    sync_events = xEventGroupCreate();
    TEST_ASSERT_NOT_NULL(sync_events);

    vol_adc_param_t adc_cfg = {
        .unit = ADC_UNIT_1,
        .chan = ADC_CHANNEL_1,
        .width = ADC_WIDTH_BIT_12,
        .atten = ADC_ATTEN_11db,
        .v_ref = 1100,
    };

    vol_monitor_param_t vol_monitor_cfg = {
        .init = adc_init,
        .deinit = adc_deinit,
        .vol_get = vol_read,
        .read_freq = 1,
        .report_freq = 0,
        .vol_full_threshold = 0,
        .vol_low_threshold = 0,
    };
    vol_monitor_cfg.user_data = audio_calloc(1, sizeof(vol_adc_param_t));
    AUDIO_MEM_CHECK(TAG, vol_monitor_cfg.user_data, return);
    memcpy(vol_monitor_cfg.user_data, &adc_cfg, sizeof(vol_adc_param_t));

    battery_service_config_t battery_config = BATTERY_SERVICE_DEFAULT_CONFIG();
    vol_monitor_handle_t vol_monitor = vol_monitor_create(&vol_monitor_cfg);
    TEST_ASSERT_NOT_NULL(vol_monitor);
    battery_config.vol_monitor = vol_monitor;
    battery_config.evt_cb = battery_service_cb;

    periph_service_handle_t battery_service = battery_service_create(&battery_config);
    TEST_ASSERT_NOT_NULL(battery_service);
    TEST_ASSERT_EQUAL(ESP_OK, periph_service_start(battery_service));
    EventBits_t bits = xEventGroupWaitBits(sync_events, BATTERY_FULL_REPORT, true, true, pdMS_TO_TICKS(4000));
    TEST_ASSERT_NOT_EQUAL(BATTERY_FULL_REPORT, bits & BATTERY_FULL_REPORT);
    TEST_ASSERT_EQUAL(ESP_OK, periph_service_destroy(battery_service));
    TEST_ASSERT_EQUAL(ESP_OK, vol_monitor_destroy(vol_monitor));
    vol_monitor = NULL;

    vol_monitor_cfg.vol_full_threshold = 100;
    vol_monitor = vol_monitor_create(&vol_monitor_cfg);
    TEST_ASSERT_NOT_NULL(vol_monitor);
    battery_config.vol_monitor = vol_monitor;
    battery_service = battery_service_create(&battery_config);
    TEST_ASSERT_NOT_NULL(battery_service);
    TEST_ASSERT_EQUAL(ESP_OK, periph_service_start(battery_service));
    bits = xEventGroupWaitBits(sync_events, BATTERY_FULL_REPORT, true, true, pdMS_TO_TICKS(4000));
    TEST_ASSERT_EQUAL(BATTERY_FULL_REPORT, bits & BATTERY_FULL_REPORT);
    TEST_ASSERT_EQUAL(ESP_OK, periph_service_destroy(battery_service));
    TEST_ASSERT_EQUAL(ESP_OK, vol_monitor_destroy(vol_monitor));
    vol_monitor = NULL;

    vEventGroupDelete(sync_events);
    free(vol_monitor_cfg.user_data);
}

TEST_CASE("voltage monitor battery low message", "[battery_service]")
{
    sync_events = xEventGroupCreate();
    TEST_ASSERT_NOT_NULL(sync_events);

    vol_adc_param_t adc_cfg = {
        .unit = ADC_UNIT_1,
        .chan = ADC_CHANNEL_1,
        .width = ADC_WIDTH_BIT_12,
        .atten = ADC_ATTEN_11db,
        .v_ref = 1100,
    };

    vol_monitor_param_t vol_monitor_cfg = {
        .init = adc_init,
        .deinit = adc_deinit,
        .vol_get = vol_read,
        .read_freq = 1,
        .report_freq = 0,
        .vol_full_threshold = 0,
        .vol_low_threshold = 0,
    };
    vol_monitor_cfg.user_data = audio_calloc(1, sizeof(vol_adc_param_t));
    AUDIO_MEM_CHECK(TAG, vol_monitor_cfg.user_data, return);
    memcpy(vol_monitor_cfg.user_data, &adc_cfg, sizeof(vol_adc_param_t));

    battery_service_config_t battery_config = BATTERY_SERVICE_DEFAULT_CONFIG();
    vol_monitor_handle_t vol_monitor = vol_monitor_create(&vol_monitor_cfg);
    TEST_ASSERT_NOT_NULL(vol_monitor);
    battery_config.vol_monitor = vol_monitor;
    battery_config.evt_cb = battery_service_cb;

    periph_service_handle_t battery_service = battery_service_create(&battery_config);
    TEST_ASSERT_NOT_NULL(battery_service);
    TEST_ASSERT_EQUAL(ESP_OK, periph_service_start(battery_service));
    EventBits_t bits = xEventGroupWaitBits(sync_events, BATTERY_LOW_REPORT, true, true, pdMS_TO_TICKS(4000));
    TEST_ASSERT_NOT_EQUAL(BATTERY_LOW_REPORT, bits & BATTERY_LOW_REPORT);
    TEST_ASSERT_EQUAL(ESP_OK, periph_service_destroy(battery_service));
    TEST_ASSERT_EQUAL(ESP_OK, vol_monitor_destroy(vol_monitor));
    vol_monitor = NULL;

    vol_monitor_cfg.vol_low_threshold = 10000;
    vol_monitor = vol_monitor_create(&vol_monitor_cfg);
    TEST_ASSERT_NOT_NULL(vol_monitor);
    battery_config.vol_monitor = vol_monitor;
    battery_service = battery_service_create(&battery_config);
    TEST_ASSERT_NOT_NULL(battery_service);
    TEST_ASSERT_EQUAL(ESP_OK, periph_service_start(battery_service));
    bits = xEventGroupWaitBits(sync_events, BATTERY_LOW_REPORT, true, true, pdMS_TO_TICKS(4000));
    TEST_ASSERT_EQUAL(BATTERY_LOW_REPORT, bits & BATTERY_LOW_REPORT);
    TEST_ASSERT_EQUAL(ESP_OK, periph_service_destroy(battery_service));
    TEST_ASSERT_EQUAL(ESP_OK, vol_monitor_destroy(vol_monitor));
    vol_monitor = NULL;

    vEventGroupDelete(sync_events);
    free(vol_monitor_cfg.user_data);
}