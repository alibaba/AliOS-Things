/*
 * ESPRESSIF MIT License
 *
 * Copyright (c) 2019 <ESPRESSIF SYSTEMS (SHANGHAI) CO., LTD>
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
#include "esp_peripherals.h"
#include "periph_sdcard.h"
#include "board.h"
#include "playlist.h"
#include "sdcard_list.h"
#include "dram_list.h"
#include "flash_list.h"
#include "partition_list.h"
#include "unity.h"
#include "sdcard_scan.h"

static const char *TAG = "TEST_PLAYLIST";

/**
 * Usage case test
 */
static esp_err_t initialize_sdcard(esp_periph_set_handle_t *set_in)
{
    // Initialize peripherals management
    esp_err_t ret = ESP_OK;
    esp_periph_config_t periph_cfg = DEFAULT_ESP_PERIPH_SET_CONFIG();
    esp_periph_set_handle_t set = esp_periph_set_init(&periph_cfg);
    TEST_ASSERT_NOT_NULL(set);
    *set_in = set;
    ret = audio_board_sdcard_init(set, SD_MODE_1_LINE);
    return ret;
}

TEST_CASE("Create a playlist handle, add playlists to the handle and use it", "[playlist]")
{
    esp_periph_set_handle_t set;
    TEST_ASSERT_FALSE(initialize_sdcard(&set));

    ESP_LOGI(TAG, "Create a handle to manage playlists");
    playlist_handle_t handle = playlist_create();
    TEST_ASSERT_NOT_NULL(handle);

    ESP_LOGI(TAG, "Create specific playlist");
    playlist_operator_handle_t sdcard_handle = NULL;
    TEST_ASSERT_FALSE(sdcard_list_create(&sdcard_handle));

    playlist_operator_handle_t flash_handle = NULL;
    TEST_ASSERT_FALSE(flash_list_create(&flash_handle));

    playlist_operator_handle_t partition_handle = NULL;
    TEST_ASSERT_FALSE(partition_list_create(&partition_handle));

    playlist_operator_handle_t dram_handle = NULL;
    TEST_ASSERT_FALSE(dram_list_create(&dram_handle));


    ESP_LOGI(TAG, "add a dram playlist and save a url to it");
    TEST_ASSERT_FALSE(playlist_add(handle, dram_handle, 0));
    TEST_ASSERT_FALSE(playlist_save(handle, "save a url to dram playlist"));
    TEST_ASSERT_FALSE(playlist_show(handle));

    ESP_LOGI(TAG, "add a sdcard playlist and checkout to the playlist and then save a url to it");
    TEST_ASSERT_FALSE(playlist_add(handle, sdcard_handle, 1));
    TEST_ASSERT_FALSE(playlist_checkout_by_id(handle, 1));
    TEST_ASSERT_FALSE(playlist_save(handle, "save a url to sdcard playlist"));
    TEST_ASSERT_FALSE(playlist_show(handle));

    ESP_LOGI(TAG, "add a nvs flash playlist and checkout to the playlist and then save a url to it");
    TEST_ASSERT_FALSE(playlist_add(handle, flash_handle, 2));
    TEST_ASSERT_FALSE(playlist_checkout_by_id(handle, 2));
    TEST_ASSERT_FALSE(playlist_save(handle, "save a url to flash playlist"));
    TEST_ASSERT_FALSE(playlist_show(handle));

    ESP_LOGI(TAG, "add a partition playlist and checkout to the playlist and then save a url to it");
    TEST_ASSERT_FALSE(playlist_add(handle, partition_handle, 3));
    TEST_ASSERT_FALSE(playlist_checkout_by_id(handle, 3));
    TEST_ASSERT_FALSE(playlist_save(handle, "save a url to partition list"));
    TEST_ASSERT_FALSE(playlist_show(handle));

    ESP_LOGI(TAG, "get type and list id of current playlist");
    TEST_ASSERT_NOT_EQUAL(-1, playlist_get_current_list_type(handle));
    TEST_ASSERT_NOT_EQUAL(-1, playlist_get_current_list_id(handle));

    ESP_LOGI(TAG, "basic operation of playlist, get next, previous, current URL in current playlist");

    char *url = NULL;
    TEST_ASSERT_FALSE(playlist_checkout_by_id(handle, 0));
    TEST_ASSERT_FALSE(playlist_get_current_list_url(handle, &url));
    ESP_LOGI(TAG, "current URL: %s", url);
    TEST_ASSERT_FALSE(playlist_next(handle, 2, &url));
    ESP_LOGI(TAG, "operation: next, step = 2, url: %s", url);
    TEST_ASSERT_FALSE(playlist_prev(handle, 1, &url));
    ESP_LOGI(TAG, "operation: prev, step = 1, url: %s", url);

    TEST_ASSERT_FALSE(playlist_checkout_by_id(handle, 1));
    TEST_ASSERT_FALSE(playlist_get_current_list_url(handle, &url));
    ESP_LOGI(TAG, "current URL: %s", url);
    TEST_ASSERT_FALSE(playlist_next(handle, 3, &url));
    ESP_LOGI(TAG, "operation: next, step = 3, url: %s", url);
    TEST_ASSERT_FALSE(playlist_prev(handle, 2, &url));
    ESP_LOGI(TAG, "operation: prev, step = 2, url: %s", url);

    TEST_ASSERT_FALSE(playlist_checkout_by_id(handle, 2));
    TEST_ASSERT_FALSE(playlist_get_current_list_url(handle, &url));
    ESP_LOGI(TAG, "current URL: %s", url);
    TEST_ASSERT_FALSE(playlist_next(handle, 1, &url));
    ESP_LOGI(TAG, "operation: next, step = 1, url: %s", url);
    TEST_ASSERT_FALSE(playlist_prev(handle, 1, &url));
    ESP_LOGI(TAG, "operation: prev, step = 1, url: %s", url);

    TEST_ASSERT_FALSE(playlist_checkout_by_id(handle, 3));
    TEST_ASSERT_FALSE(playlist_get_current_list_url(handle, &url));
    ESP_LOGI(TAG, "current URL: %s", url);
    TEST_ASSERT_FALSE(playlist_next(handle, 2, &url));
    ESP_LOGI(TAG, "operation: next, step = 2, url: %s", url);
    TEST_ASSERT_FALSE(playlist_prev(handle, 1, &url));
    ESP_LOGI(TAG, "operation: prev, step = 1, url: %s", url);

    ESP_LOGI(TAG, "destroy all playlists");
    TEST_ASSERT_FALSE(playlist_destroy(handle));
    handle = NULL;

    TEST_ASSERT_FALSE(esp_periph_set_destroy(set));
}

TEST_CASE("Create different playlists and use choose function", "[playlist]")
{
    esp_periph_set_handle_t set;
    TEST_ASSERT_FALSE(initialize_sdcard(&set));

    ESP_LOGI(TAG, "Create a handle to manage playlists");
    playlist_handle_t handle = playlist_create();
    TEST_ASSERT_NOT_NULL(handle);

    ESP_LOGI(TAG, "Create specific playlist");
    playlist_operator_handle_t sdcard_handle = NULL;
    TEST_ASSERT_FALSE(sdcard_list_create(&sdcard_handle));

    playlist_operator_handle_t flash_handle = NULL;
    TEST_ASSERT_FALSE(flash_list_create(&flash_handle));

    playlist_operator_handle_t partition_handle = NULL;
    TEST_ASSERT_FALSE(partition_list_create(&partition_handle));

    playlist_operator_handle_t dram_handle = NULL;
    TEST_ASSERT_FALSE(dram_list_create(&dram_handle));

    ESP_LOGI(TAG, "add a dram playlist and save urls to it");
    TEST_ASSERT_FALSE(playlist_add(handle, dram_handle, 0));
    TEST_ASSERT_FALSE(playlist_save(handle, "save url to dram playlist ID0"));
    TEST_ASSERT_FALSE(playlist_save(handle, "save url to dram playlist ID1"));
    TEST_ASSERT_FALSE(playlist_save(handle, "save url to dram playlist ID2"));
    TEST_ASSERT_FALSE(playlist_save(handle, "save url to dram playlist ID3"));
    TEST_ASSERT_FALSE(playlist_show(handle));

    ESP_LOGI(TAG, "add a sdcard playlist and checkout to the playlist and then save urls to it");
    TEST_ASSERT_FALSE(playlist_add(handle, sdcard_handle, 1));
    TEST_ASSERT_FALSE(playlist_checkout_by_id(handle, 1));
    TEST_ASSERT_FALSE(playlist_save(handle, "save url to sdcard playlist ID0"));
    TEST_ASSERT_FALSE(playlist_save(handle, "save url to sdcard playlist ID1"));
    TEST_ASSERT_FALSE(playlist_save(handle, "save url to sdcard playlist ID2"));
    TEST_ASSERT_FALSE(playlist_save(handle, "save url to sdcard playlist ID3"));
    TEST_ASSERT_FALSE(playlist_show(handle));

    ESP_LOGI(TAG, "add a nvs flash playlist and checkout to the playlist and then save urls to it");
    TEST_ASSERT_FALSE(playlist_add(handle, flash_handle, 2));
    TEST_ASSERT_FALSE(playlist_checkout_by_id(handle, 2));
    TEST_ASSERT_FALSE(playlist_save(handle, "save url to flash playlist ID0"));
    TEST_ASSERT_FALSE(playlist_save(handle, "save url to flash playlist ID1"));
    TEST_ASSERT_FALSE(playlist_save(handle, "save url to flash playlist ID2"));
    TEST_ASSERT_FALSE(playlist_save(handle, "save url to flash playlist ID3"));
    TEST_ASSERT_FALSE(playlist_show(handle));

    ESP_LOGI(TAG, "add a partition playlist and checkout to the playlist and then save urls to it");
    TEST_ASSERT_FALSE(playlist_add(handle, partition_handle, 3));
    TEST_ASSERT_FALSE(playlist_checkout_by_id(handle, 3));
    TEST_ASSERT_FALSE(playlist_save(handle, "save url to partition playlist ID0"));
    TEST_ASSERT_FALSE(playlist_save(handle, "save url to partition playlist ID1"));
    TEST_ASSERT_FALSE(playlist_save(handle, "save url to partition playlist ID2"));
    TEST_ASSERT_FALSE(playlist_save(handle, "save url to partition playlist ID3"));
    TEST_ASSERT_FALSE(playlist_show(handle));

    char *url = NULL;
    ESP_LOGI(TAG, "Checkout to dram playlist and choose id in the list");
    TEST_ASSERT_FALSE(playlist_checkout_by_id(handle, 0));
    for (int i = 0; i < playlist_get_current_list_url_num(handle); i++) {
        playlist_choose(handle, i, &url);
        ESP_LOGW(TAG, "Choose ID: %d, URL: %s", i, url);
    }

    ESP_LOGI(TAG, "Checkout to sdcard playlist and choose id in the list");
    TEST_ASSERT_FALSE(playlist_checkout_by_id(handle, 1));
    for (int i = 0; i < playlist_get_current_list_url_num(handle); i++) {
        playlist_choose(handle, i, &url);
        ESP_LOGW(TAG, "Choose sdcard ID: %d, URL: %s", i, url);
    }

    ESP_LOGI(TAG, "Checkout to flash playlist and choose id in the list");
    TEST_ASSERT_FALSE(playlist_checkout_by_id(handle, 2));
    for (int i = 0; i < playlist_get_current_list_url_num(handle); i++) {
        playlist_choose(handle, i, &url);
        ESP_LOGW(TAG, "Choose sdcard ID: %d, URL: %s", i, url);
    }

    ESP_LOGI(TAG, "Checkout to partition playlist and choose id in the list");
    TEST_ASSERT_FALSE(playlist_checkout_by_id(handle, 3));
    for (int i = 0; i < playlist_get_current_list_url_num(handle); i++) {
        playlist_choose(handle, i, &url);
        ESP_LOGW(TAG, "Choose sdcard ID: %d, URL: %s", i, url);
    }

    ESP_LOGI(TAG, "Test finish, destroy all playlists");
    TEST_ASSERT_FALSE(playlist_destroy(handle));
    handle = NULL;

    TEST_ASSERT_FALSE(esp_periph_set_destroy(set));
}

TEST_CASE("Add same type to a handle use different list id", "[playlist]")
{
    esp_periph_set_handle_t set;
    TEST_ASSERT_FALSE(initialize_sdcard(&set));

    ESP_LOGI(TAG, "Create a handle to manage playlists");
    playlist_handle_t handle = playlist_create();
    TEST_ASSERT_NOT_NULL(handle);

    ESP_LOGI(TAG, "Create specific playlist");
    playlist_operator_handle_t sdcard_handle = NULL;
    TEST_ASSERT_FALSE(sdcard_list_create(&sdcard_handle));

    playlist_operator_handle_t sdcard_handle1 = NULL;
    TEST_ASSERT_FALSE(sdcard_list_create(&sdcard_handle1));

    playlist_operator_handle_t flash_handle = NULL;
    TEST_ASSERT_FALSE(flash_list_create(&flash_handle));

    playlist_operator_handle_t flash_handle1 = NULL;
    TEST_ASSERT_FALSE(flash_list_create(&flash_handle1));

    playlist_operator_handle_t partition_handle = NULL;
    TEST_ASSERT_FALSE(partition_list_create(&partition_handle));

    playlist_operator_handle_t partition_handle1 = NULL;
    TEST_ASSERT_FALSE(partition_list_create(&partition_handle1));

    playlist_operator_handle_t dram_handle = NULL;
    TEST_ASSERT_FALSE(dram_list_create(&dram_handle));

    playlist_operator_handle_t dram_handle1 = NULL;
    TEST_ASSERT_FALSE(dram_list_create(&dram_handle1));

    ESP_LOGI(TAG, "add two sdcard playlists to handle");
    TEST_ASSERT_FALSE(playlist_add(handle, sdcard_handle, 0));
    TEST_ASSERT_FALSE(playlist_checkout_by_id(handle, 0));
    TEST_ASSERT_FALSE(playlist_save(handle, "save a url to sdcard playlist, id: 0"));
    TEST_ASSERT_FALSE(playlist_show(handle));

    TEST_ASSERT_FALSE(playlist_add(handle, sdcard_handle1, 1));
    TEST_ASSERT_FALSE(playlist_checkout_by_id(handle, 1));
    TEST_ASSERT_FALSE(playlist_save(handle, "save a url to sdcard playlist, id: 1"));
    TEST_ASSERT_FALSE(playlist_show(handle));

    ESP_LOGI(TAG, "add two dram playlists to handle");
    TEST_ASSERT_FALSE(playlist_add(handle, dram_handle, 2));
    TEST_ASSERT_FALSE(playlist_checkout_by_id(handle, 2));
    TEST_ASSERT_FALSE(playlist_save(handle, "save a url to dram playlist, id: 2"));
    TEST_ASSERT_FALSE(playlist_show(handle));

    TEST_ASSERT_FALSE(playlist_add(handle, dram_handle1, 3));
    TEST_ASSERT_FALSE(playlist_checkout_by_id(handle, 3));
    TEST_ASSERT_FALSE(playlist_save(handle, "save a url to dram playlist, id: 3"));
    TEST_ASSERT_FALSE(playlist_show(handle));

    ESP_LOGI(TAG, "add two nvs flash playlists to handle");
    TEST_ASSERT_FALSE(playlist_add(handle, flash_handle, 4));
    TEST_ASSERT_FALSE(playlist_checkout_by_id(handle, 4));
    TEST_ASSERT_FALSE(playlist_save(handle, "save a url to flash playlist, id: 4"));
    TEST_ASSERT_FALSE(playlist_show(handle));

    TEST_ASSERT_FALSE(playlist_add(handle, flash_handle1, 5));
    TEST_ASSERT_FALSE(playlist_checkout_by_id(handle, 5));
    TEST_ASSERT_FALSE(playlist_save(handle, "save a url to flash playlist, id: 5"));
    TEST_ASSERT_FALSE(playlist_show(handle));

    ESP_LOGI(TAG, "add a partition playlist to handle");
    // The partition playlist can only be added once, or it will be overwrited by the newest partition playlist
    TEST_ASSERT_FALSE(playlist_add(handle, partition_handle, 6));
    TEST_ASSERT_FALSE(playlist_checkout_by_id(handle, 6));
    TEST_ASSERT_FALSE(playlist_save(handle, "save a url to partition list, id: 6"));
    TEST_ASSERT_FALSE(playlist_show(handle));

    TEST_ASSERT_FALSE(playlist_destroy(handle));
    handle = NULL;

    TEST_ASSERT_FALSE(esp_periph_set_destroy(set));
}

TEST_CASE("Create multiple handles and add playlists to them", "[playlist]")
{
    esp_periph_set_handle_t set;
    TEST_ASSERT_FALSE(initialize_sdcard(&set));

    ESP_LOGI(TAG, "Create a handle to manage playlists, called handle0");
    playlist_handle_t handle = playlist_create();
    TEST_ASSERT_NOT_NULL(handle);

    ESP_LOGI(TAG, "add a sdcard playlist to the handle 0");
    playlist_operator_handle_t sdcard_handle = NULL;
    TEST_ASSERT_FALSE(sdcard_list_create(&sdcard_handle));
    TEST_ASSERT_FALSE(playlist_add(handle, sdcard_handle, 0));
    TEST_ASSERT_FALSE(playlist_save(handle, "This a sdcard playlist in handle 0"));
    TEST_ASSERT_FALSE(playlist_show(handle));

    ESP_LOGI(TAG, "add a dram playlist to the handle 0");
    playlist_operator_handle_t dram_handle = NULL;
    TEST_ASSERT_FALSE(dram_list_create(&dram_handle));
    TEST_ASSERT_FALSE(playlist_add(handle, dram_handle, 1));
    TEST_ASSERT_FALSE(playlist_checkout_by_id(handle, 1));
    TEST_ASSERT_FALSE(playlist_save(handle, "This a dram playlist in handle 0"));
    TEST_ASSERT_FALSE(playlist_show(handle));

    ESP_LOGI(TAG, "add a nvs flash playlist to the handle 0");
    playlist_operator_handle_t flash_handle = NULL;
    TEST_ASSERT_FALSE(flash_list_create(&flash_handle));
    TEST_ASSERT_FALSE(playlist_add(handle, flash_handle, 2));
    TEST_ASSERT_FALSE(playlist_checkout_by_id(handle, 2));
    TEST_ASSERT_FALSE(playlist_save(handle, "This a flash playlist in handle 0"));
    TEST_ASSERT_FALSE(playlist_show(handle));

    ESP_LOGI(TAG, "add a partition playlist to the handle 0");
    playlist_operator_handle_t partition_handle = NULL;
    TEST_ASSERT_FALSE(partition_list_create(&partition_handle));
    TEST_ASSERT_FALSE(playlist_add(handle, partition_handle, 3));
    TEST_ASSERT_FALSE(playlist_checkout_by_id(handle, 3));
    TEST_ASSERT_FALSE(playlist_save(handle, "This a partition playlist in handle 0"));
    TEST_ASSERT_FALSE(playlist_show(handle));

    ESP_LOGI(TAG, "create a new handle, called handle 1");
    playlist_handle_t handle1 = playlist_create();
    TEST_ASSERT_NOT_NULL(handle1);

    ESP_LOGI(TAG, "add a sdcard playlist to the handle 1");
    playlist_operator_handle_t sdcard_handle1 = NULL;
    TEST_ASSERT_FALSE(sdcard_list_create(&sdcard_handle1));
    TEST_ASSERT_FALSE(playlist_add(handle1, sdcard_handle1, 0));
    TEST_ASSERT_FALSE(playlist_save(handle1, "This a sdcard playlist in handle 1"));
    TEST_ASSERT_FALSE(playlist_show(handle1));

    ESP_LOGI(TAG, "add a dram playlist to the handle 1");
    playlist_operator_handle_t dram_handle1 = NULL;
    TEST_ASSERT_FALSE(dram_list_create(&dram_handle1));
    TEST_ASSERT_FALSE(playlist_add(handle1, dram_handle1, 1));
    TEST_ASSERT_FALSE(playlist_checkout_by_id(handle1, 1));
    TEST_ASSERT_FALSE(playlist_save(handle1, "This a dram playlist in handle 1"));
    TEST_ASSERT_FALSE(playlist_show(handle1));

    ESP_LOGI(TAG, "add a nvs flash playlist to the handle 1");
    playlist_operator_handle_t flash_handle1 = NULL;
    TEST_ASSERT_FALSE(flash_list_create(&flash_handle1));
    TEST_ASSERT_FALSE(playlist_add(handle1, flash_handle1, 2));
    TEST_ASSERT_FALSE(playlist_checkout_by_id(handle1, 2));
    TEST_ASSERT_FALSE(playlist_save(handle1, "This a flash playlist in handle 1"));
    TEST_ASSERT_FALSE(playlist_show(handle1));

    // partition playlist can only use once, or it will be rewritten

    TEST_ASSERT_FALSE(playlist_destroy(handle));
    handle = NULL;
    TEST_ASSERT_FALSE(playlist_destroy(handle1));
    handle1 = NULL;

    TEST_ASSERT_FALSE(esp_periph_set_destroy(set));
}

TEST_CASE("Create playlists and use them without playlist manager handle", "[playlist]")
{
    ESP_LOGI(TAG, "Initialize sdcard");
    esp_periph_set_handle_t set;
    TEST_ASSERT_FALSE(initialize_sdcard(&set));

    ESP_LOGI(TAG, "Create a sdcard playlist and use it");
    playlist_operator_handle_t sdcard_handle = NULL;
    TEST_ASSERT_FALSE(sdcard_list_create(&sdcard_handle));

    TEST_ASSERT_FALSE(sdcard_list_save(sdcard_handle, "sdcard playlist test url 0"));
    TEST_ASSERT_FALSE(sdcard_list_save(sdcard_handle, "sdcard playlist test url 1"));
    TEST_ASSERT_FALSE(sdcard_list_save(sdcard_handle, "sdcard playlist test url 2"));
    TEST_ASSERT_FALSE(sdcard_list_save(sdcard_handle, "sdcard playlist test url 3"));
    TEST_ASSERT_FALSE(sdcard_list_show(sdcard_handle));

    char *url = NULL;
    ESP_LOGI(TAG, "current url number: %d", sdcard_list_get_url_num(sdcard_handle));
    TEST_ASSERT_FALSE(sdcard_list_next(sdcard_handle, 1, &url));
    ESP_LOGI(TAG, "operation: get next url, step: 1, url: %s", url);
    TEST_ASSERT_FALSE( sdcard_list_next(sdcard_handle, 2, &url));
    ESP_LOGI(TAG, "operation: get next url, step: 2, url: %s", url);
    TEST_ASSERT_FALSE(sdcard_list_prev(sdcard_handle, 50, &url));
    ESP_LOGI(TAG, "operation: get previous url, step: 50, url: %s", url);
    TEST_ASSERT_FALSE(sdcard_list_current(sdcard_handle, &url));
    ESP_LOGI(TAG, "operation: get current url, url: %s", url);
    TEST_ASSERT_FALSE(sdcard_list_destroy(sdcard_handle));
    sdcard_handle = NULL;

    ESP_LOGI(TAG, "Create a flash playlist and use it");
    playlist_operator_handle_t flash_handle = NULL;
    TEST_ASSERT_FALSE(flash_list_create(&flash_handle));

    TEST_ASSERT_FALSE(flash_list_save(flash_handle, "flash playlist test url 0"));
    TEST_ASSERT_FALSE(flash_list_save(flash_handle, "flash playlist test url 1"));
    TEST_ASSERT_FALSE(flash_list_save(flash_handle, "flash playlist test url 2"));
    TEST_ASSERT_FALSE(flash_list_save(flash_handle, "flash playlist test url 3"));
    TEST_ASSERT_FALSE(flash_list_show(flash_handle));

    ESP_LOGI(TAG, "current url number: %d", flash_list_get_url_num(flash_handle));
    TEST_ASSERT_FALSE(flash_list_next(flash_handle, 2, &url));
    ESP_LOGI(TAG, "operation: get next url, step: 2, url: %s", url);
    TEST_ASSERT_FALSE(flash_list_next(flash_handle, 3, &url));
    ESP_LOGI(TAG, "operation: get next url, step: 3, url: %s", url);
    TEST_ASSERT_FALSE(flash_list_prev(flash_handle, 1, &url));
    ESP_LOGI(TAG, "operation: get previous url, step: 1, url: %s", url);
    TEST_ASSERT_FALSE(flash_list_current(flash_handle, &url));
    ESP_LOGI(TAG, "operation: get current url, url: %s", url);
    TEST_ASSERT_FALSE(flash_list_destroy(flash_handle));
    flash_handle = NULL;

    ESP_LOGI(TAG, "Create a partition playlist and use it");
    playlist_operator_handle_t partition_handle = NULL;
    TEST_ASSERT_FALSE(partition_list_create(&partition_handle));

    TEST_ASSERT_FALSE(partition_list_save(partition_handle, "partition playlist test url 0"));
    TEST_ASSERT_FALSE(partition_list_save(partition_handle, "partition playlist test url 1"));
    TEST_ASSERT_FALSE(partition_list_save(partition_handle, "partition playlist test url 2"));
    TEST_ASSERT_FALSE(partition_list_save(partition_handle, "partition playlist test url 3"));
    TEST_ASSERT_FALSE(partition_list_show(partition_handle));

    ESP_LOGI(TAG, "current url number: %d", partition_list_get_url_num(partition_handle));
    TEST_ASSERT_FALSE(partition_list_next(partition_handle, 5, &url));
    ESP_LOGI(TAG, "operation: get next url, step: 5, url: %s", url);
    TEST_ASSERT_FALSE(partition_list_next(partition_handle, 10, &url));
    ESP_LOGI(TAG, "operation: get next url, step: 10, url: %s", url);
    TEST_ASSERT_FALSE(partition_list_prev(partition_handle, 23, &url));
    ESP_LOGI(TAG, "operation: get previous url, step: 23, url: %s", url);
    TEST_ASSERT_FALSE(partition_list_current(partition_handle, &url));
    ESP_LOGI(TAG, "operation: get current url, url: %s", url);
    TEST_ASSERT_FALSE(partition_list_destroy(partition_handle));
    partition_handle = NULL;

    ESP_LOGI(TAG, "Create a dram playlist and use it");
    playlist_operator_handle_t dram_handle = NULL;
    TEST_ASSERT_FALSE(dram_list_create(&dram_handle));

    TEST_ASSERT_FALSE(dram_list_save(dram_handle, "dram playlist test url 0"));
    TEST_ASSERT_FALSE(dram_list_save(dram_handle, "dram playlist test url 1"));
    TEST_ASSERT_FALSE(dram_list_save(dram_handle, "dram playlist test url 2"));
    TEST_ASSERT_FALSE(dram_list_save(dram_handle, "dram playlist test url 3"));
    TEST_ASSERT_FALSE(dram_list_show(dram_handle));

    ESP_LOGI(TAG, "current url number: %d", dram_list_get_url_num(dram_handle));
    TEST_ASSERT_FALSE(dram_list_next(dram_handle, 21, &url));
    ESP_LOGI(TAG, "operation: get next url, step: 21, url: %s", url);
    TEST_ASSERT_FALSE(dram_list_next(dram_handle, 12, &url));
    ESP_LOGI(TAG, "operation: get next url, step: 12, url: %s", url);
    TEST_ASSERT_FALSE(dram_list_prev(dram_handle, 2, &url));
    ESP_LOGI(TAG, "operation: get previous url, step: 2, url: %s", url);
    TEST_ASSERT_FALSE(dram_list_current(dram_handle, &url));
    ESP_LOGI(TAG, "operation: get current url, url: %s", url);
    TEST_ASSERT_FALSE(dram_list_destroy(dram_handle));
    dram_handle = NULL;

    TEST_ASSERT_FALSE(esp_periph_set_destroy(set));
}


static void scan_sdcard_cb(void *user_data, char *url)
{
    playlist_handle_t handle = (playlist_handle_t)user_data;
    playlist_save(handle, url);
}

TEST_CASE("Create a sdcard playlist and scan sdcard then save the specified url to the sdcard playlist", "[playlist]")
{
    esp_periph_set_handle_t set;
    TEST_ASSERT_FALSE(initialize_sdcard(&set));

    playlist_handle_t handle = playlist_create();
    TEST_ASSERT_NOT_NULL(handle);

    ESP_LOGI(TAG, "create a sdcard playlist");
    playlist_operator_handle_t sdcard_handle = NULL;
    TEST_ASSERT_FALSE(sdcard_list_create(&sdcard_handle));
    TEST_ASSERT_FALSE(playlist_add(handle, sdcard_handle, 0));

    TEST_ASSERT_FALSE(sdcard_scan(scan_sdcard_cb, "/sdcard", 0, (const char *[]) {"mp3", "wav", "aac"}, 3, handle));
    TEST_ASSERT_FALSE(playlist_show(handle));

    TEST_ASSERT_FALSE(playlist_destroy(handle));
    handle = NULL;

    TEST_ASSERT_FALSE(esp_periph_set_destroy(set));
}


/**
 * Abnormal operation and stress test
 */

TEST_CASE("Create a manager handle and use it without adding playlist", "[playlist]")
{
    playlist_handle_t handle = playlist_create();
    TEST_ASSERT_NOT_NULL(handle);

    char *url = NULL;
    TEST_ASSERT_TRUE(playlist_next(handle, 1, &url));
    TEST_ASSERT_TRUE(playlist_prev(handle, 1, &url));
    TEST_ASSERT_TRUE(playlist_get_current_list_url(handle, &url));
    TEST_ASSERT_TRUE(playlist_show(handle));
    TEST_ASSERT_TRUE(playlist_checkout_by_id(handle, 0));
    TEST_ASSERT_TRUE(playlist_get_current_list_id(handle));
    TEST_ASSERT_TRUE(playlist_get_current_list_type(handle));

    TEST_ASSERT_FALSE(playlist_destroy(handle));
    handle = NULL;
}

TEST_CASE("Create a playlist handle and add a sdcard playlist without initializing sdcard", "[playlist]")
{
    // The sdcard hasn't been initialized, there's bound to be a failure here.
    playlist_operator_handle_t sdcard_handle = NULL;
    TEST_ASSERT_TRUE(sdcard_list_create(&sdcard_handle));
}

TEST_CASE("Create and destroy a playlist manager", "[playlist]")
{
    playlist_handle_t handle = playlist_create();
    TEST_ASSERT_NOT_NULL(handle);
    TEST_ASSERT_FALSE(playlist_destroy(handle));
}

TEST_CASE("Create a playlist and then call function without save URLs", "[playlist]")
{
    char *url = NULL;
    esp_periph_set_handle_t set;
    TEST_ASSERT_FALSE(initialize_sdcard(&set));

    playlist_operator_handle_t sdcard_handle = NULL;
    TEST_ASSERT_FALSE(sdcard_list_create(&sdcard_handle));
    TEST_ASSERT_TRUE(sdcard_list_next(sdcard_handle, 1, &url));
    TEST_ASSERT_TRUE(sdcard_list_prev(sdcard_handle, 1, &url));
    TEST_ASSERT_TRUE(sdcard_list_current(sdcard_handle, &url));
    TEST_ASSERT_FALSE(sdcard_list_destroy(sdcard_handle));
    sdcard_handle = NULL;

    playlist_operator_handle_t flash_handle = NULL;
    TEST_ASSERT_FALSE(flash_list_create(&flash_handle));
    TEST_ASSERT_TRUE(flash_list_next(flash_handle, 1, &url));
    TEST_ASSERT_TRUE(flash_list_prev(flash_handle, 1, &url));
    TEST_ASSERT_TRUE(flash_list_current(flash_handle, &url));
    TEST_ASSERT_FALSE(flash_list_destroy(flash_handle));
    flash_handle = NULL;

    playlist_operator_handle_t partition_handle = NULL;
    TEST_ASSERT_FALSE(partition_list_create(&partition_handle));
    TEST_ASSERT_TRUE(partition_list_next(partition_handle, 1, &url));
    TEST_ASSERT_TRUE(partition_list_prev(partition_handle, 1, &url));
    TEST_ASSERT_TRUE(partition_list_current(partition_handle, &url));
    TEST_ASSERT_FALSE(partition_list_destroy(partition_handle));
    partition_handle = NULL;

    playlist_operator_handle_t dram_handle = NULL;
    TEST_ASSERT_FALSE(dram_list_create(&dram_handle));
    TEST_ASSERT_TRUE(dram_list_next(dram_handle, 1, &url));
    TEST_ASSERT_TRUE(dram_list_prev(dram_handle, 1, &url));
    TEST_ASSERT_TRUE(dram_list_current(dram_handle, &url));
    TEST_ASSERT_FALSE(dram_list_destroy(dram_handle));
    dram_handle = NULL;

    TEST_ASSERT_FALSE(esp_periph_set_destroy(set));
}

TEST_CASE("Register a existed ID to playlist manager", "[playlist]")
{
    esp_periph_set_handle_t set;
    TEST_ASSERT_FALSE(initialize_sdcard(&set));

    playlist_handle_t handle = playlist_create();
    TEST_ASSERT_NOT_NULL(handle);

    playlist_operator_handle_t sdcard_handle = NULL;
    TEST_ASSERT_FALSE(sdcard_list_create(&sdcard_handle));
    TEST_ASSERT_FALSE(playlist_add(handle, sdcard_handle, 0));

    // because of ID 0 had been registered, so there must be a failure
    playlist_operator_handle_t flash_handle = NULL;
    TEST_ASSERT_FALSE(flash_list_create(&flash_handle));
    TEST_ASSERT_TRUE(playlist_add(handle, flash_handle, 0));

    TEST_ASSERT_FALSE(flash_list_destroy(flash_handle));
    TEST_ASSERT_FALSE(playlist_destroy(handle));

    TEST_ASSERT_FALSE(esp_periph_set_destroy(set));
}

TEST_CASE("Checkout to a playlist id that is non-existed", "[playlist]")
{
    playlist_handle_t handle = playlist_create();
    TEST_ASSERT_NOT_NULL(handle);

    playlist_operator_handle_t flash_handle = NULL;
    TEST_ASSERT_FALSE(flash_list_create(&flash_handle));
    TEST_ASSERT_FALSE(playlist_add(handle, flash_handle, 0));

    ESP_LOGW(TAG, "Because of playlist ID 1 hadn't been registered, so there must be a failure");
    TEST_ASSERT_TRUE(playlist_checkout_by_id(handle, 1));

    TEST_ASSERT_FALSE(playlist_destroy(handle));
}

TEST_CASE("Get number of urls and current id in playlist", "[playlist]")
{
    esp_periph_set_handle_t set;
    TEST_ASSERT_FALSE(initialize_sdcard(&set));

    ESP_LOGI(TAG, "Create a handle to manage playlists");
    playlist_handle_t handle = playlist_create();
    TEST_ASSERT_NOT_NULL(handle);

    ESP_LOGI(TAG, "Create specific playlist");
    playlist_operator_handle_t sdcard_handle = NULL;
    TEST_ASSERT_FALSE(sdcard_list_create(&sdcard_handle));
    playlist_operator_handle_t flash_handle = NULL;
    TEST_ASSERT_FALSE(flash_list_create(&flash_handle));
    playlist_operator_handle_t partition_handle = NULL;
    TEST_ASSERT_FALSE(partition_list_create(&partition_handle));
    playlist_operator_handle_t dram_handle = NULL;
    TEST_ASSERT_FALSE(dram_list_create(&dram_handle));

    ESP_LOGI(TAG, "add a dram playlist and save a url to it");
    TEST_ASSERT_FALSE(playlist_add(handle, dram_handle, 0));
    TEST_ASSERT_FALSE(playlist_save(handle, "save a url to dram playlist0"));
    TEST_ASSERT_FALSE(playlist_save(handle, "save a url to dram playlist1"));
    TEST_ASSERT_FALSE(playlist_save(handle, "save a url to dram playlist2"));
    TEST_ASSERT_FALSE(playlist_show(handle));

    ESP_LOGI(TAG, "add a sdcard playlist and checkout to the playlist and then save a url to it");
    TEST_ASSERT_FALSE(playlist_add(handle, sdcard_handle, 1));
    TEST_ASSERT_FALSE(playlist_checkout_by_id(handle, 1));
    TEST_ASSERT_FALSE(playlist_save(handle, "save a url to sdcard playlist0"));
    TEST_ASSERT_FALSE(playlist_save(handle, "save a url to sdcard playlist1"));
    TEST_ASSERT_FALSE(playlist_save(handle, "save a url to sdcard playlist2"));
    TEST_ASSERT_FALSE(playlist_show(handle));

    ESP_LOGI(TAG, "add a nvs flash playlist and checkout to the playlist and then save a url to it");
    TEST_ASSERT_FALSE(playlist_add(handle, flash_handle, 2));
    TEST_ASSERT_FALSE(playlist_checkout_by_id(handle, 2));
    TEST_ASSERT_FALSE(playlist_save(handle, "save a url to flash playlist0"));
    TEST_ASSERT_FALSE(playlist_save(handle, "save a url to flash playlist1"));
    TEST_ASSERT_FALSE(playlist_save(handle, "save a url to flash playlist2"));
    TEST_ASSERT_FALSE(playlist_show(handle));

    ESP_LOGI(TAG, "add a partition playlist and checkout to the playlist and then save a url to it");
    TEST_ASSERT_FALSE(playlist_add(handle, partition_handle, 3));
    TEST_ASSERT_FALSE(playlist_checkout_by_id(handle, 3));
    TEST_ASSERT_FALSE(playlist_save(handle, "save a url to partition playlist0"));
    TEST_ASSERT_FALSE(playlist_save(handle, "save a url to partition playlist1"));
    TEST_ASSERT_FALSE(playlist_save(handle, "save a url to partition playlist2"));
    TEST_ASSERT_FALSE(playlist_show(handle));

    ESP_LOGI(TAG, "get type and list id of current playlist");
    TEST_ASSERT_NOT_EQUAL(-1, playlist_get_current_list_type(handle));
    TEST_ASSERT_NOT_EQUAL(-1, playlist_get_current_list_id(handle));

    TEST_ASSERT_FALSE(playlist_checkout_by_id(handle, 0));
    TEST_ASSERT_EQUAL(3, playlist_get_current_list_url_num(handle));
    char *buff = NULL;
    for (int i = 0; i < 2; i++) {
        TEST_ASSERT_EQUAL(i, playlist_get_current_list_url_id(handle));
        TEST_ASSERT_FALSE(playlist_next(handle, 1, &buff));
    }

    TEST_ASSERT_FALSE(playlist_checkout_by_id(handle, 1));
    TEST_ASSERT_EQUAL(3, playlist_get_current_list_url_num(handle));
    for (int i = 0; i < 2; i++) {
        TEST_ASSERT_EQUAL(i, playlist_get_current_list_url_id(handle));
        TEST_ASSERT_FALSE(playlist_next(handle, 1, &buff));
    }

    TEST_ASSERT_FALSE(playlist_checkout_by_id(handle, 2));
    TEST_ASSERT_EQUAL(3, playlist_get_current_list_url_num(handle));
    for (int i = 0; i < 2; i++) {
        TEST_ASSERT_EQUAL(i, playlist_get_current_list_url_id(handle));
        TEST_ASSERT_FALSE(playlist_next(handle, 1, &buff));
    }

    TEST_ASSERT_FALSE(playlist_checkout_by_id(handle, 3));
    TEST_ASSERT_EQUAL(3, playlist_get_current_list_url_num(handle));
    for (int i = 0; i < 2; i++) {
        TEST_ASSERT_EQUAL(i, playlist_get_current_list_url_id(handle));
        TEST_ASSERT_FALSE(playlist_next(handle, 1, &buff));
    }

    ESP_LOGI(TAG, "destroy all playlists");
    TEST_ASSERT_FALSE(playlist_destroy(handle));
    handle = NULL;

    TEST_ASSERT_FALSE(esp_periph_set_destroy(set));
}

TEST_CASE("Judge whether a url exists in playlist and reset playlist", "[playlist]")
{
    esp_periph_set_handle_t set;
    TEST_ASSERT_FALSE(initialize_sdcard(&set));

    ESP_LOGI(TAG, "Create a handle to manage playlists");
    playlist_handle_t handle = playlist_create();
    TEST_ASSERT_NOT_NULL(handle);

    ESP_LOGI(TAG, "Create specific playlist");
    playlist_operator_handle_t sdcard_handle = NULL;
    TEST_ASSERT_FALSE(sdcard_list_create(&sdcard_handle));
    playlist_operator_handle_t flash_handle = NULL;
    TEST_ASSERT_FALSE(flash_list_create(&flash_handle));
    playlist_operator_handle_t partition_handle = NULL;
    TEST_ASSERT_FALSE(partition_list_create(&partition_handle));
    playlist_operator_handle_t dram_handle = NULL;
    TEST_ASSERT_FALSE(dram_list_create(&dram_handle));

    ESP_LOGI(TAG, "add a dram playlist and save a url to it");
    TEST_ASSERT_FALSE(playlist_add(handle, dram_handle, 0));
    TEST_ASSERT_FALSE(playlist_save(handle, "save a url to dram playlist0"));
    TEST_ASSERT_FALSE(playlist_show(handle));

    ESP_LOGI(TAG, "add a sdcard playlist and checkout to the playlist and then save a url to it");
    TEST_ASSERT_FALSE(playlist_add(handle, sdcard_handle, 1));
    TEST_ASSERT_FALSE(playlist_checkout_by_id(handle, 1));
    TEST_ASSERT_FALSE(playlist_save(handle, "save a url to sdcard playlist0"));
    TEST_ASSERT_FALSE(playlist_show(handle));

    ESP_LOGI(TAG, "add a nvs flash playlist and checkout to the playlist and then save a url to it");
    TEST_ASSERT_FALSE(playlist_add(handle, flash_handle, 2));
    TEST_ASSERT_FALSE(playlist_checkout_by_id(handle, 2));
    TEST_ASSERT_FALSE(playlist_save(handle, "save a url to flash playlist0"));
    TEST_ASSERT_FALSE(playlist_show(handle));

    ESP_LOGI(TAG, "add a partition playlist and checkout to the playlist and then save a url to it");
    TEST_ASSERT_FALSE(playlist_add(handle, partition_handle, 3));
    TEST_ASSERT_FALSE(playlist_checkout_by_id(handle, 3));
    TEST_ASSERT_FALSE(playlist_save(handle, "save a url to partition playlist0"));
    TEST_ASSERT_FALSE(playlist_show(handle));

    TEST_ASSERT_FALSE(playlist_checkout_by_id(handle, 0));
    TEST_ASSERT_TRUE(playlist_exist(handle, "save a url to dram playlist0"));
    TEST_ASSERT_FALSE(playlist_reset(handle));
    TEST_ASSERT_EQUAL(0, playlist_get_current_list_url_num(handle));
    TEST_ASSERT_FALSE(playlist_save(handle, "save a url to dram playlist0"));
    TEST_ASSERT_FALSE(playlist_save(handle, "save a url to dram playlist1"));
    TEST_ASSERT_FALSE(playlist_save(handle, "save a url to dram playlist2"));
    TEST_ASSERT_FALSE(playlist_show(handle));


    TEST_ASSERT_FALSE(playlist_checkout_by_id(handle, 1));
    TEST_ASSERT_TRUE(playlist_exist(handle, "save a url to sdcard playlist0"));
    TEST_ASSERT_FALSE(playlist_reset(handle));
    TEST_ASSERT_EQUAL(0, playlist_get_current_list_url_num(handle));
    TEST_ASSERT_FALSE(playlist_save(handle, "save a url to sdcard playlist0"));
    TEST_ASSERT_FALSE(playlist_save(handle, "save a url to sdcard playlist1"));
    TEST_ASSERT_FALSE(playlist_save(handle, "save a url to sdcard playlist2"));
    TEST_ASSERT_FALSE(playlist_show(handle));

    TEST_ASSERT_FALSE(playlist_checkout_by_id(handle, 2));
    TEST_ASSERT_TRUE(playlist_exist(handle, "save a url to flash playlist0"));
    TEST_ASSERT_FALSE(playlist_reset(handle));
    TEST_ASSERT_EQUAL(0, playlist_get_current_list_url_num(handle));
    TEST_ASSERT_FALSE(playlist_save(handle, "save a url to flash playlist0"));
    TEST_ASSERT_FALSE(playlist_save(handle, "save a url to flash playlist1"));
    TEST_ASSERT_FALSE(playlist_save(handle, "save a url to flash playlist2"));
    TEST_ASSERT_FALSE(playlist_show(handle));

    TEST_ASSERT_FALSE(playlist_checkout_by_id(handle, 3));
    TEST_ASSERT_TRUE(playlist_exist(handle, "save a url to partition playlist0"));
    TEST_ASSERT_FALSE(playlist_reset(handle));
    TEST_ASSERT_EQUAL(0, playlist_get_current_list_url_num(handle));
    TEST_ASSERT_FALSE(playlist_save(handle, "save a url to partition playlist0"));
    TEST_ASSERT_FALSE(playlist_save(handle, "save a url to partition playlist1"));
    TEST_ASSERT_FALSE(playlist_save(handle, "save a url to partition playlist2"));
    TEST_ASSERT_FALSE(playlist_show(handle));

    ESP_LOGI(TAG, "destroy all playlists");
    TEST_ASSERT_FALSE(playlist_destroy(handle));
    handle = NULL;

    TEST_ASSERT_FALSE(esp_periph_set_destroy(set));
}

TEST_CASE("Remove fuction test in dram list", "[playlist]")
{
    esp_periph_set_handle_t set;
    TEST_ASSERT_FALSE(initialize_sdcard(&set));

    ESP_LOGI(TAG, "Create a handle to manage playlists");
    playlist_handle_t handle = playlist_create();
    TEST_ASSERT_NOT_NULL(handle);

    ESP_LOGI(TAG, "Create specific playlist");
    playlist_operator_handle_t sdcard_handle = NULL;
    TEST_ASSERT_FALSE(sdcard_list_create(&sdcard_handle));
    playlist_operator_handle_t flash_handle = NULL;
    TEST_ASSERT_FALSE(flash_list_create(&flash_handle));
    playlist_operator_handle_t partition_handle = NULL;
    TEST_ASSERT_FALSE(partition_list_create(&partition_handle));
    playlist_operator_handle_t dram_handle = NULL;
    TEST_ASSERT_FALSE(dram_list_create(&dram_handle));

    ESP_LOGI(TAG, "add a dram playlist and save a url to it");
    TEST_ASSERT_FALSE(playlist_add(handle, dram_handle, 0));
    TEST_ASSERT_FALSE(playlist_save(handle, "save a url to dram playlist0"));
    TEST_ASSERT_FALSE(playlist_save(handle, "save a url to dram playlist1"));
    TEST_ASSERT_FALSE(playlist_save(handle, "save a url to dram playlist2"));

    ESP_LOGI(TAG, "add a sdcard playlist and checkout to the playlist and then save a url to it");
    TEST_ASSERT_FALSE(playlist_add(handle, sdcard_handle, 1));
    TEST_ASSERT_FALSE(playlist_checkout_by_id(handle, 1));
    TEST_ASSERT_FALSE(playlist_save(handle, "save a url to sdcard playlist0"));
    TEST_ASSERT_FALSE(playlist_show(handle));

    ESP_LOGI(TAG, "add a nvs flash playlist and checkout to the playlist and then save a url to it");
    TEST_ASSERT_FALSE(playlist_add(handle, flash_handle, 2));
    TEST_ASSERT_FALSE(playlist_checkout_by_id(handle, 2));
    TEST_ASSERT_FALSE(playlist_save(handle, "save a url to flash playlist0"));
    TEST_ASSERT_FALSE(playlist_show(handle));

    ESP_LOGI(TAG, "add a partition playlist and checkout to the playlist and then save a url to it");
    TEST_ASSERT_FALSE(playlist_add(handle, partition_handle, 3));
    TEST_ASSERT_FALSE(playlist_checkout_by_id(handle, 3));
    TEST_ASSERT_FALSE(playlist_save(handle, "save a url to partition playlist0"));
    TEST_ASSERT_FALSE(playlist_show(handle));

    ESP_LOGI(TAG, "remove a url");
    TEST_ASSERT_FALSE(playlist_checkout_by_id(handle, 0));
    TEST_ASSERT_FALSE(playlist_remove_by_url(handle, "save a url to dram playlist0"));
    TEST_ASSERT_FALSE(playlist_remove_by_url_id(handle, 1));
    TEST_ASSERT_EQUAL(1, playlist_get_current_list_url_num(handle));
    TEST_ASSERT_FALSE(playlist_show(handle));

    ESP_LOGI(TAG, "add urls again");
    TEST_ASSERT_FALSE(playlist_save(handle, "save url after remove"));
    TEST_ASSERT_EQUAL(2, playlist_get_current_list_url_num(handle));
    TEST_ASSERT_FALSE(playlist_show(handle));

// We only support remove function in dram list, so the remove fuction will return 
// ESP_ERR_NOT_SUPPORTED when checkout to other playlists.
    TEST_ASSERT_FALSE(playlist_checkout_by_id(handle, 1));
    TEST_ASSERT_EQUAL(ESP_ERR_NOT_SUPPORTED, playlist_remove_by_url(handle, "save a url to sdcard playlist0"));

    TEST_ASSERT_FALSE(playlist_checkout_by_id(handle, 2));
    TEST_ASSERT_EQUAL(ESP_ERR_NOT_SUPPORTED, playlist_remove_by_url(handle, "save a url to flash playlist0"));

    TEST_ASSERT_FALSE(playlist_checkout_by_id(handle, 3));
    TEST_ASSERT_EQUAL(ESP_ERR_NOT_SUPPORTED, playlist_remove_by_url(handle, "save a url to partition playlist0"));

    ESP_LOGI(TAG, "destroy all playlists");
    TEST_ASSERT_FALSE(playlist_destroy(handle));
    handle = NULL;

    TEST_ASSERT_FALSE(esp_periph_set_destroy(set));
}
