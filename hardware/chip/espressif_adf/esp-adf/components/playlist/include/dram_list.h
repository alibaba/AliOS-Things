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

#ifndef _DRAM_LIST_H_
#define _DRAM_LIST_H_

#include "playlist.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Create a playlist in dram
 *
 * @param[out] handle  The playlist handle from application layer
 *
 * @return ESP_OK   success
 *         ESP_FAIL failed
 */
esp_err_t dram_list_create(playlist_operator_handle_t *handle);

/**
 * @brief Save URL to dram playlist
 *
 * @param handle     Playlist handle
 * @param url        URL to be saved
 *
 * @return ESP_OK    success
 *         ESP_FAIL  failed
 */
esp_err_t dram_list_save(playlist_operator_handle_t handle, const char *url);

/**
 * @brief The following URLs in dram playlist
 *
 * @param      handle         Playlist handle
 * @param      step           The offset of URL from current URL
 * @param[out] url_buff       A second rank pointer to get a address of URL
 *
 * @return ESP_OK    success
 *         ESP_FAIL  failed
 */
esp_err_t dram_list_next(playlist_operator_handle_t handle, int step, char **url_buff);

/**
 * @brief The previous URLs in dram playlist
 *
 * @param      handle         Playlist handle
 * @param      step           The offset of URL from current URL
 * @param[out] url_buff       A second rank pointer to get a address of URL
 *
 * @return ESP_OK    success
 *         ESP_FAIL  failed
 */
esp_err_t dram_list_prev(playlist_operator_handle_t handle, int step, char **url_buff);

/**
 * @brief Judge whether the url exists in dram playlist
 *
 * @param handle   Playlist handle
 * @param url      The url to be checked
 *
 * @return true    existence
 *         false   Non-existent
 */
bool dram_list_exist(playlist_operator_handle_t handle, const char *url);

/**
 * @brief Reset dram playlist
 *
 * @param handle   Playlist handle
 *
 * @return ESP_OK   success
 *         ESP_FAIL failed
 */
esp_err_t dram_list_reset(playlist_operator_handle_t handle);

/**
 * @brief The current URL in current playlist
 *
 * @param      handle         Playlist handle
 * @param[out] url_buff       A second rank pointer to get a address of URL
 *
 * @return ESP_OK    success
 *         ESP_FAIL  failed
 */
esp_err_t dram_list_current(playlist_operator_handle_t handle, char **url_buff);

/**
 * @brief Choose a url by url id
 *
 * @param      handle         Playlist handle
 * @param      url_id         The id of url in dram list
 * @param[out] url_buff       A second rank pointer to get a address of URL
 *
 * @return ESP_OK    success
 *         ESP_FAIL  failed
 */
esp_err_t dram_list_choose(playlist_operator_handle_t handle, int url_id, char **url_buff);

/**
 * @brief Get URLs number in the dram playlist
 *
 * @param handle        Playlist handle
 *
 * @return URLs number in dram playlist
 *         ESP_FAIL     Fail to get number of urls
 */
int dram_list_get_url_num(playlist_operator_handle_t handle);

/**
 * @brief Get current url id in the dram playlist
 *
 * @param handle        Playlist handle
 *
 * @return Current url id in dram playlist
 *         ESP_FAIL     Fail to get url id
 */
int dram_list_get_url_id(playlist_operator_handle_t handle);

/**
 * @brief Show all the URLs in the dram playlist
 *
 * @param handle     Playlist handle
 *
 * @return ESP_OK    success
 *         ESP_FAIL  failed
 */
esp_err_t dram_list_show(playlist_operator_handle_t handle);

/**
 * @brief Remove corrsponding url in dram list
 *
 * @param handle   Playlist handle
 * @param url      The url to be removed
 *
 * @return ESP_OK    success
 *         ESP_FAIL  failed
 */
esp_err_t dram_list_remove_by_url(playlist_operator_handle_t handle, const char *url);

/**
 * @brief Remove url by id
 *
 * @param handle   Playlist handle
 * @param url_id   The url id to be removed
 *
 * @return ESP_OK    success
 *         ESP_FAIL  failed
 */
esp_err_t dram_list_remove_by_url_id(playlist_operator_handle_t handle, uint16_t url_id);

/**
 * @brief Destroy the dram playlist
 *
 * @param handle     Playlist handle
 *
 * @return ESP_OK    success
 *         ESP_FAIL  failed
 */
esp_err_t dram_list_destroy(playlist_operator_handle_t handle);

#ifdef __cplusplus
}
#endif

#endif
