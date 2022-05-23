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
#include "unity.h"
#include "wifi_ssid_manager.h"
#include "esp_log.h"
#include "periph_wifi.h"

#define MAX_SSID_NUM 5

static const char *TAG = "TEST_WIFI_SSID_MANAGER";

typedef struct {
    const char *ssid;
    const char *pwd;
} ssid_pwd_t;

static const ssid_pwd_t nvs_ssid[MAX_SSID_NUM] = {
    {
        .ssid = "your-ssid-0",
        .pwd  = "your-password-0"
    },
    {
        .ssid = "your-ssid-1",
        .pwd  = "your-password-1"
    },
    {
        .ssid = "your-ssid-2",
        .pwd  = "your-password-2"
    },
    {
        .ssid = "your-ssid-3",
        .pwd  = "your-password-3"
    },
    {
        .ssid = "your-ssid-4",
        .pwd  = "your-password-4"
    }
};

static esp_periph_set_handle_t wifi_setup(void)
{

    esp_periph_config_t periph_cfg = DEFAULT_ESP_PERIPH_SET_CONFIG();
    esp_periph_set_handle_t set = esp_periph_set_init(&periph_cfg);
    TEST_ASSERT_NOT_NULL(set);

    tcpip_adapter_init();

    periph_wifi_cfg_t wifi_cfg = {
        .ssid = "myssid",
        .password = "mypassword",
    };
    esp_periph_handle_t wifi_handle = periph_wifi_init(&wifi_cfg);
    TEST_ASSERT_FALSE(esp_periph_start(set, wifi_handle));
    TEST_ASSERT_FALSE(periph_wifi_wait_for_connected(wifi_handle, portMAX_DELAY));
    return set;
}

TEST_CASE("Create a ssid manager and save different ssids to it", "[WIFI_SSID_MANAGER]")
{
    wifi_ssid_manager_handle_t ssid_manager = wifi_ssid_manager_create(MAX_SSID_NUM);
    TEST_ASSERT_NOT_NULL(ssid_manager);

    for (int i = 0; i < MAX_SSID_NUM; i++) {
        TEST_ASSERT_FALSE(wifi_ssid_manager_save(ssid_manager, nvs_ssid[i].ssid, nvs_ssid[i].pwd));
    }

    TEST_ASSERT_EQUAL(MAX_SSID_NUM, wifi_ssid_manager_get_ssid_num(ssid_manager));
    TEST_ASSERT_FALSE(wifi_ssid_manager_list_show(ssid_manager));
    TEST_ASSERT_FALSE(wifi_ssid_manager_destroy(ssid_manager));
}

TEST_CASE("Save ssid that has been saved in flash", "[WIFI_SSID_MANAGER]")
{
    wifi_ssid_manager_handle_t ssid_manager = wifi_ssid_manager_create(MAX_SSID_NUM);
    TEST_ASSERT_NOT_NULL(ssid_manager);

    TEST_ASSERT_FALSE(wifi_ssid_manager_erase_all(ssid_manager));
    TEST_ASSERT_EQUAL(0, wifi_ssid_manager_get_ssid_num(ssid_manager));

    for (int i = 0; i < 3; i++) {
        TEST_ASSERT_FALSE(wifi_ssid_manager_save(ssid_manager, nvs_ssid[i].ssid, nvs_ssid[i].pwd));
    }
    TEST_ASSERT_EQUAL(3, wifi_ssid_manager_get_ssid_num(ssid_manager));

    ESP_LOGI(TAG, "save the same ssid to flash, the wifi info will be updated but the number of ssids won't increase");
    for (int i = 0; i < 3; i++) {
        TEST_ASSERT_FALSE(wifi_ssid_manager_save(ssid_manager, nvs_ssid[i].ssid, nvs_ssid[i].pwd));
    }

    TEST_ASSERT_EQUAL(3, wifi_ssid_manager_get_ssid_num(ssid_manager));
    TEST_ASSERT_FALSE(wifi_ssid_manager_list_show(ssid_manager));
    TEST_ASSERT_FALSE(wifi_ssid_manager_destroy(ssid_manager));
}

TEST_CASE("Choose the SSID with the best signal", "[WIFI_SSID_MANAGER]")
{
    wifi_ssid_manager_handle_t ssid_manager = wifi_ssid_manager_create(MAX_SSID_NUM);
    TEST_ASSERT_NOT_NULL(ssid_manager);

    TEST_ASSERT_FALSE(wifi_ssid_manager_erase_all(ssid_manager));
    TEST_ASSERT_EQUAL(0, wifi_ssid_manager_get_ssid_num(ssid_manager));

    esp_periph_set_handle_t set = wifi_setup();
    for (int i = 0; i < MAX_SSID_NUM; i++) {
        TEST_ASSERT_FALSE(wifi_ssid_manager_save(ssid_manager, nvs_ssid[i].ssid, nvs_ssid[i].pwd));
    }

    wifi_config_t config = {0};
    if (ESP_OK == wifi_ssid_manager_get_best_config(ssid_manager, &config)) {
        ESP_LOGW(TAG, "get the best configuration, ssid: %s, password: %s", config.sta.ssid, config.sta.password);
    } else {
        ESP_LOGE(TAG, "get best config failed!");
    }

    // The choosen flag will be reset if save a new ssid or call create() function
    TEST_ASSERT_FALSE(wifi_ssid_manager_save(ssid_manager, nvs_ssid[0].ssid, nvs_ssid[0].pwd));

    memset(&config, 0x00, sizeof(wifi_config_t));
    if (ESP_OK == wifi_ssid_manager_get_best_config(ssid_manager, &config)) {
        ESP_LOGW(TAG, "=>get the best configuration, ssid: %s, password: %s", config.sta.ssid, config.sta.password);
    } else {
        ESP_LOGE(TAG, "=>get best config failed!");
    }

    TEST_ASSERT_FALSE(esp_periph_set_stop_all(set));
    TEST_ASSERT_FALSE(esp_periph_set_destroy(set));
    TEST_ASSERT_FALSE(wifi_ssid_manager_destroy(ssid_manager));
}

TEST_CASE("Select the most recently saved SSID ", "[WIFI_SSID_MANAGER]")
{
    wifi_ssid_manager_handle_t ssid_manager = wifi_ssid_manager_create(MAX_SSID_NUM);
    TEST_ASSERT_NOT_NULL(ssid_manager);

    TEST_ASSERT_FALSE(wifi_ssid_manager_erase_all(ssid_manager));
    TEST_ASSERT_EQUAL(0, wifi_ssid_manager_get_ssid_num(ssid_manager));

    for (int i = 0; i < MAX_SSID_NUM; i++) {
        wifi_config_t config = {0};
        TEST_ASSERT_FALSE(wifi_ssid_manager_save(ssid_manager, nvs_ssid[i].ssid, nvs_ssid[i].pwd));
        TEST_ASSERT_FALSE(wifi_ssid_manager_get_latest_config(ssid_manager, &config));
        TEST_ASSERT_EQUAL(0, strcmp((char *)config.sta.ssid, nvs_ssid[i].ssid));
        TEST_ASSERT_EQUAL(0, strcmp((char *)config.sta.password, nvs_ssid[i].pwd));
    }
    TEST_ASSERT_EQUAL(MAX_SSID_NUM, wifi_ssid_manager_get_ssid_num(ssid_manager));
    TEST_ASSERT_FALSE(wifi_ssid_manager_list_show(ssid_manager));
    TEST_ASSERT_FALSE(wifi_ssid_manager_destroy(ssid_manager));
}
