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
#include "audio_error.h"
#include "audio_mem.h"
#include "nvs_flash.h"
#include "wifi_ssid_manager.h"

#include "esp_delegate.h"
#include "nvs_action.h"

#define WIFI_CONF_NVS_NAMESPACE  "WIFI_CONF_NVS"
#define WIFI_INFO_NVS_NAMESPACE  "WIFI_INFO_NVS"

#define SSID_MANAGER_CONF_KEY    "MANAGER_CONF"

#define WIFI_STORE_KEY           "KEY"
#define KEY_BUFF_SIZE            16

#define WIFI_SSID_MAX_LENGTH     33
#define WIFI_PWD_MAX_LENGTH      65

static const char *TAG = "WIFI_SSID_MANAGER";

/**
 * @breif Configuration of ssid manager
 */
typedef struct {
    uint8_t max_ssid_num;    /*!< The max number of ssid to be stored */
    uint8_t exsit_ssid_num;  /*!< The number of existed ssid */
    uint8_t latest_ssid;     /*!< Latest stored ssid */
} nvs_ssid_conf_t;

/**
 * @brief Information for every ssid saved in flash
 */
typedef struct {
    bool choosen;                      /*!< Judge wether this ssid have been choosen to connect */
    int cnt;                           /*!< An indicator to measure the storage time */
    char ssid[WIFI_SSID_MAX_LENGTH];   /*!< SSID to be saved */
    char pwd[WIFI_PWD_MAX_LENGTH];     /*!< Password to be saved */
} nvs_stored_info_t;

/**
 * @breif Management unit of wifi ssid
 */
struct wifi_ssid_manager {
    esp_dispatcher_handle_t dispatcher; /*!< dispatcher handle to run the nvs actions */
    nvs_handle conf_nvs;      /*!< Nvs handle to save the ssid manager configuration */
    nvs_handle info_nvs;      /*!< Nvs handle to save the wifi information */
};

static char *get_key_by_id(uint8_t id)
{
    char *buff = audio_calloc(1, KEY_BUFF_SIZE);
    AUDIO_NULL_CHECK(TAG, buff, return NULL);
    sprintf(buff, "%s%d", WIFI_STORE_KEY, id);
    return buff;
}

static esp_err_t nvs_ssid_list_conf_save(wifi_ssid_manager_handle_t handle, nvs_ssid_conf_t *conf)
{
    esp_err_t ret = ESP_OK;
    nvs_action_set_args_t set_blob = {
        .key = SSID_MANAGER_CONF_KEY,
        .type = NVS_TYPE_BLOB,
        .value.blob = conf,
        .len = sizeof(nvs_ssid_conf_t),
    };
    action_arg_t set_blob_arg = {
        .data = &set_blob,
        .len = sizeof(nvs_action_set_args_t),
    };
    action_result_t result = { 0 };
    ret = esp_dispatcher_execute_with_func(handle->dispatcher, nvs_action_set, (void *)handle->conf_nvs, &set_blob_arg, &result);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Fail to save configuration to nvs flash: %d", ret);
    }
    memset(&result, 0x00, sizeof(action_result_t));
    ret = esp_dispatcher_execute_with_func(handle->dispatcher, nvs_action_commit, (void *)handle->conf_nvs, NULL, &result);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Fail to commit changes to nvs flash: %d", ret);
    }
    return ret;
}

static esp_err_t nvs_ssid_list_conf_get(wifi_ssid_manager_handle_t handle, nvs_ssid_conf_t *conf)
{
    esp_err_t ret = ESP_OK;
    nvs_action_get_args_t get_blob = {
        .key = SSID_MANAGER_CONF_KEY,
        .type = NVS_TYPE_BLOB,
        .wanted_size = sizeof(nvs_ssid_conf_t),
    };
    action_arg_t get_blob_arg = {
        .data = &get_blob,
        .len = sizeof(nvs_action_get_args_t),
    };
    action_result_t result = { 0 };
    ret = esp_dispatcher_execute_with_func(handle->dispatcher, nvs_action_get, (void *)handle->conf_nvs, &get_blob_arg, &result);
    if (ret == ESP_OK) {
        memcpy(conf, result.data, sizeof(nvs_ssid_conf_t));
        free(result.data);
        result.data = NULL;
    } else {
        ESP_LOGE(TAG, "Fail to get configuration from nvs flash");
    }
    return ret;
}

static esp_err_t nvs_wifi_info_save(wifi_ssid_manager_handle_t handle, uint8_t key_id, nvs_stored_info_t *info)
{
    esp_err_t ret = ESP_OK;
    char *key = get_key_by_id(key_id);

    nvs_action_set_args_t set_blob = {
        .key = key,
        .type = NVS_TYPE_BLOB,
        .value.blob = info,
        .len = sizeof(nvs_stored_info_t),
    };
    action_arg_t set_blob_arg = {
        .data = &set_blob,
        .len = sizeof(nvs_action_set_args_t),
    };
    action_result_t result = { 0 };
    ret = esp_dispatcher_execute_with_func(handle->dispatcher, nvs_action_set, (void *)handle->info_nvs, &set_blob_arg, &result);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Fail to save wifi information, key id: %d", key_id);
    }
    memset(&result, 0x00, sizeof(action_result_t));
    ret = esp_dispatcher_execute_with_func(handle->dispatcher, nvs_action_commit, (void *)handle->info_nvs, NULL, &result);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Fail to commit wifi information, key id: %d", key_id);
    }

    audio_free(key);
    return ret;
}

static esp_err_t nvs_wifi_info_get(wifi_ssid_manager_handle_t handle, uint8_t key_id, nvs_stored_info_t *info)
{
    esp_err_t ret = ESP_OK;
    char *key = get_key_by_id(key_id);

    nvs_action_get_args_t get_blob = {
        .key = key,
        .type = NVS_TYPE_BLOB,
        .wanted_size = sizeof(nvs_stored_info_t),
    };
    action_arg_t get_blob_arg = {
        .data = &get_blob,
        .len = sizeof(nvs_action_get_args_t),
    };
    action_result_t result = { 0 };
    ret = esp_dispatcher_execute_with_func(handle->dispatcher, nvs_action_get, (void *)handle->info_nvs, &get_blob_arg, &result);
    if (ret == ESP_OK) {
        memcpy(info, result.data, sizeof(nvs_stored_info_t));
        free(result.data);
        result.data = NULL;
    } else {
        ESP_LOGE(TAG, "Fail to get info from nvs flash");
    }
    audio_free(key);

    return ret;
}

static int8_t get_stored_id_by_ssid(wifi_ssid_manager_handle_t handle, uint8_t exsit_ssid_num, const char *ssid)
{
    nvs_stored_info_t info = {0};
    for (int i = 0; i < exsit_ssid_num; i++) {
        memset(&info, 0, sizeof(nvs_stored_info_t));
        nvs_wifi_info_get(handle, i, &info);
        if (strlen(info.ssid) != strlen(ssid)) {
            continue;
        }
        if (strcmp(info.ssid, ssid) == 0) {
            ESP_LOGD(TAG, "Found the same ssid in flash, update it");
            return i;
        }
    }
    return ESP_FAIL;
}

static esp_err_t nvs_set_counter(wifi_ssid_manager_handle_t handle, uint8_t exsit_ssid_num)
{
    esp_err_t ret = ESP_OK;
    nvs_stored_info_t info = {0};
    for (int i = 0; i < exsit_ssid_num; i++) {
        memset(&info, 0, sizeof(nvs_stored_info_t));
        ret |= nvs_wifi_info_get(handle, i, &info);
        info.cnt ++;
        ret |= nvs_wifi_info_save(handle, i, &info);
    }
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Fail to set counter");
        return ESP_FAIL;
    }
    return ESP_OK;
}

static uint8_t nvs_get_write_id(wifi_ssid_manager_handle_t handle, uint8_t exsit_ssid_num)
{
    uint8_t max_cnt = 0, max_cnt_id = 0;
    nvs_stored_info_t info = {0};
    for (int i = 0; i < exsit_ssid_num; i++) {
        memset(&info, 0, sizeof(nvs_stored_info_t));
        nvs_wifi_info_get(handle, i, &info);
        if (info.cnt >= max_cnt) {
            max_cnt = info.cnt;
            max_cnt_id = i;
        }
    }
    return max_cnt_id;
}

static esp_err_t nvs_reset_choosen_flag(wifi_ssid_manager_handle_t handle, nvs_ssid_conf_t *conf)
{
    esp_err_t ret = ESP_OK;
    nvs_stored_info_t info = {0};
    for (int i = 0; i < conf->exsit_ssid_num; i++) {
        ret |= nvs_wifi_info_get(handle, i, &info);
        info.choosen = false;
        ret |= nvs_wifi_info_save(handle, i, &info);
    }
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Fail to reset choosen flag");
    }
    return ret;
}

static esp_err_t wifi_ssid_manager_init_nvs(void *instance, action_arg_t *arg, action_result_t *result)
{
    wifi_ssid_manager_handle_t mng_handle = (wifi_ssid_manager_handle_t)instance;

    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    if (ret != ESP_OK) {
        return ret;
    }
    ret = nvs_open(WIFI_CONF_NVS_NAMESPACE, NVS_READWRITE, &mng_handle->conf_nvs);
    if (ret != ESP_OK) {
        return ret;
    }
    ret = nvs_open(WIFI_INFO_NVS_NAMESPACE, NVS_READWRITE, &mng_handle->info_nvs);
    if (ret != ESP_OK) {
        nvs_close(mng_handle->conf_nvs);
        return ret;
    }
    return ret;
}

wifi_ssid_manager_handle_t wifi_ssid_manager_create(uint8_t max_ssid_num)
{
    wifi_ssid_manager_handle_t mng_handle = audio_calloc(1, sizeof(struct wifi_ssid_manager));
    AUDIO_NULL_CHECK(TAG, mng_handle, return NULL);

    mng_handle->dispatcher = esp_dispatcher_get_delegate_handle();
    AUDIO_NULL_CHECK(TAG, mng_handle, {
        free(mng_handle);
        return NULL;
    });
    action_result_t init_result = { 0 };
    if (esp_dispatcher_execute_with_func(mng_handle->dispatcher, wifi_ssid_manager_init_nvs, (void *)mng_handle, NULL, &init_result) != ESP_OK) {
        esp_dispatcher_destroy(mng_handle->dispatcher);
        free(mng_handle);
        return NULL;
    }

    nvs_ssid_conf_t conf = {0};
    esp_err_t ret = nvs_ssid_list_conf_get(mng_handle, &conf);
    if (ret == ESP_ERR_NVS_NOT_FOUND) {
        conf.max_ssid_num = max_ssid_num;
        nvs_ssid_list_conf_save(mng_handle, &conf);
    }

    nvs_reset_choosen_flag(mng_handle, &conf);
    return mng_handle;
}

esp_err_t wifi_ssid_manager_save(wifi_ssid_manager_handle_t handle, const char *ssid, const char *pwd)
{
    esp_err_t ret = ESP_OK;
    if (strlen(ssid) >= WIFI_SSID_MAX_LENGTH || strlen(pwd) >= WIFI_PWD_MAX_LENGTH) {
        ESP_LOGE(TAG, "The length of wifi ssid or password is too long");
        return ESP_FAIL;
    }
    AUDIO_NULL_CHECK(TAG, handle, return ESP_FAIL);
    nvs_ssid_conf_t conf = {0};
    nvs_ssid_list_conf_get(handle, &conf);

    int8_t stored_id = get_stored_id_by_ssid(handle, conf.exsit_ssid_num, ssid);
    uint8_t key_id = 0;

    if (stored_id < 0) { // The ssid was not saved in flash
        if (conf.exsit_ssid_num < conf.max_ssid_num) {
            key_id = conf.exsit_ssid_num;
            conf.latest_ssid = conf.exsit_ssid_num;
            conf.exsit_ssid_num++;
        } else {
            key_id = nvs_get_write_id(handle, conf.exsit_ssid_num);
            conf.latest_ssid = key_id;
        }
    } else {
        key_id = stored_id;
        conf.latest_ssid = stored_id;
    }
    nvs_stored_info_t info = {
        .cnt = 0,
        .choosen = false,
    };
    memcpy(info.ssid, ssid, strlen(ssid));
    memcpy(info.pwd, pwd, strlen(pwd));
    ret |= nvs_wifi_info_save(handle, key_id, &info);
    ret |= nvs_set_counter(handle, conf.exsit_ssid_num);
    ret |= nvs_ssid_list_conf_save(handle, &conf);
    ret |= nvs_reset_choosen_flag(handle, &conf);

    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Fail to save url to nvs, ret = 0x%x", ret);
        return ESP_FAIL;
    }
    return ESP_OK;
}

esp_err_t wifi_ssid_manager_get_latest_config(wifi_ssid_manager_handle_t handle, wifi_config_t *config)
{
    esp_err_t ret = ESP_OK;
    AUDIO_NULL_CHECK(TAG, handle, return ESP_FAIL);
    AUDIO_NULL_CHECK(TAG, config, return ESP_FAIL);
    nvs_ssid_conf_t conf = {0};
    nvs_stored_info_t info = {0};

    ret |= nvs_ssid_list_conf_get(handle, &conf);
    if (conf.exsit_ssid_num <= 0) {
        ESP_LOGW(TAG, "There is no ssid stored in flash, please save ssids to flash first");
        return ESP_FAIL;
    }
    ret |= nvs_wifi_info_get(handle, conf.latest_ssid, &info);
    memset(config->sta.ssid, 0, sizeof(config->sta.ssid));
    memset(config->sta.password, 0, sizeof(config->sta.password));
    memcpy(config->sta.ssid, info.ssid, strlen(info.ssid));
    memcpy(config->sta.password, info.pwd, strlen(info.pwd));
    if (info.choosen == false) {
        info.choosen = true;
        ret |= nvs_wifi_info_save(handle, conf.latest_ssid, &info);
    }
    return ret;
}

esp_err_t wifi_ssid_manager_get_best_config(wifi_ssid_manager_handle_t handle, wifi_config_t *config)
{
    esp_err_t ret = ESP_OK;
    AUDIO_NULL_CHECK(TAG, handle, return ESP_FAIL);
    AUDIO_NULL_CHECK(TAG, config, return ESP_FAIL);
    uint16_t ap_num = 0;
    nvs_ssid_conf_t conf = {0};
    nvs_stored_info_t info = {0};
    wifi_scan_config_t scan_conf = {0};
    scan_conf.show_hidden = true;
    ret |= esp_wifi_scan_start(&scan_conf, true);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Fail to scan ap around the device, ret = %d", ret);
        return ESP_FAIL;
    }
    esp_wifi_scan_get_ap_num(&ap_num);

    ret |= nvs_ssid_list_conf_get(handle, &conf);
    if (ap_num > 0) {
        wifi_ap_record_t *ap_record = audio_calloc(1, ap_num * sizeof(wifi_ap_record_t));
        AUDIO_NULL_CHECK(TAG, ap_record, return ESP_FAIL);

        int8_t stored_id = -1;
        esp_wifi_scan_get_ap_records(&ap_num, ap_record);
        int max_rssi = 0, max_rssi_stroed_id = -1;
        for (int i = 0; i < ap_num; i++) {
            stored_id = get_stored_id_by_ssid(handle, conf.exsit_ssid_num, (const char *)ap_record[i].ssid);
            if (stored_id >= 0) {
                nvs_wifi_info_get(handle, stored_id, &info);
                if (info.choosen) {
                    continue;
                }
                if (max_rssi == 0) {
                    max_rssi = ap_record[i].rssi;
                }
                if (ap_record[i].rssi >= max_rssi) {
                    max_rssi = ap_record[i].rssi;
                    max_rssi_stroed_id = stored_id;
                }
            }
        }
        audio_free(ap_record);
        if (max_rssi_stroed_id >= 0) {
            conf.latest_ssid = max_rssi_stroed_id;
            nvs_stored_info_t info = {0};
            ret |= nvs_wifi_info_get(handle, max_rssi_stroed_id, &info);
            info.choosen = true;
            memset(config->sta.ssid, 0, sizeof(config->sta.ssid));
            memset(config->sta.password, 0, sizeof(config->sta.password));
            memcpy(config->sta.ssid, info.ssid, strlen(info.ssid));
            memcpy(config->sta.password, info.pwd, strlen(info.pwd));
            ret |= nvs_wifi_info_save(handle, max_rssi_stroed_id, &info);
            ret |= nvs_ssid_list_conf_save(handle, &conf);
            return ret;
        } else {
            ESP_LOGE(TAG, "There is no accessable wifi info stored in flash");
            return ESP_FAIL;
        }
    } else {
        ESP_LOGW(TAG, "There is no ap around the device, ap_num = %d", ap_num);
        return ESP_FAIL;
    }
}

int wifi_ssid_manager_get_ssid_num(wifi_ssid_manager_handle_t handle)
{
    AUDIO_NULL_CHECK(TAG, handle, return ESP_FAIL);
    nvs_ssid_conf_t conf = {0};
    if (nvs_ssid_list_conf_get(handle, &conf) == ESP_OK) {
        return conf.exsit_ssid_num;
    } else {
        return ESP_FAIL;
    }
}

esp_err_t wifi_ssid_manager_list_show(wifi_ssid_manager_handle_t handle)
{
    AUDIO_NULL_CHECK(TAG, handle, return ESP_FAIL);
    esp_err_t ret = ESP_OK;
    nvs_stored_info_t info = {0};
    nvs_ssid_conf_t conf = {0};
    ret |= nvs_ssid_list_conf_get(handle, &conf);
    for (int i = 0; i < conf.exsit_ssid_num; i++) {
        ret |= nvs_wifi_info_get(handle, i, &info);
        ESP_LOGI(TAG, "id = %d, ssid: %s, pwd: %s", i, info.ssid, info.pwd);
    }
    return ret;
}

esp_err_t wifi_ssid_manager_erase_all(wifi_ssid_manager_handle_t handle)
{
    AUDIO_NULL_CHECK(TAG, handle, return ESP_FAIL);
    esp_err_t ret = ESP_OK;
    uint8_t max_ssid_num = 0;
    nvs_ssid_conf_t conf = {0};
    ret |= nvs_ssid_list_conf_get(handle, &conf);
    max_ssid_num = conf.max_ssid_num;
    memset(&conf, 0, sizeof(nvs_ssid_conf_t));
    conf.max_ssid_num = max_ssid_num;
    action_result_t result = { 0 };
    ret |= esp_dispatcher_execute_with_func(handle->dispatcher, nvs_action_erase_all, (void *)handle->info_nvs, NULL, &result);
    ret |= esp_dispatcher_execute_with_func(handle->dispatcher, nvs_action_erase_all, (void *)handle->conf_nvs, NULL, &result);
    ret |= nvs_ssid_list_conf_save(handle, &conf);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Fail to erase nvs flash");
    }
    return ret;
}

esp_err_t wifi_ssid_manager_destroy(wifi_ssid_manager_handle_t handle)
{
    esp_err_t ret = ESP_OK;
    AUDIO_NULL_CHECK(TAG, handle, return ESP_FAIL);
    action_result_t result = { 0 };
    ret |= esp_dispatcher_execute_with_func(handle->dispatcher, nvs_action_erase_all, (void *)handle->info_nvs, NULL, &result);
    ret |= esp_dispatcher_execute_with_func(handle->dispatcher, nvs_action_erase_all, (void *)handle->conf_nvs, NULL, &result);
    esp_dispatcher_execute_with_func(handle->dispatcher, nvs_action_close, (void *)handle->info_nvs, NULL, &result);
    esp_dispatcher_execute_with_func(handle->dispatcher, nvs_action_close, (void *)handle->conf_nvs, NULL, &result);
    esp_dispatcher_destroy(handle->dispatcher);
    audio_free(handle);
    return ret;
}
