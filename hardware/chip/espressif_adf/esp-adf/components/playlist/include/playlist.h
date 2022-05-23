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

#ifndef _PLAY_LIST_H_
#define _PLAY_LIST_H_

#include "esp_log.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Type of playlist
 */
typedef enum {
    PLAYLIST_UNKNOWN = -1,  /*!< Unknown type */
    PLAYLIST_SDCARD,        /*!< Playlist in sdcard */
    PLAYLIST_FLASH,         /*!< Playlist in nvs */
    PLAYLIST_DRAM,          /*!< Playlist in ram */
    PLAYLIST_PARTITION      /*!< Playlist in partition */
} playlist_type_t;

/**
 * @brief All types of Playlists' operation
 */
typedef struct {
    esp_err_t (*show)    (void *playlist);                              /*!< Show all the URLs in playlist */
    esp_err_t (*save)    (void *playlist, const char *url);             /*!< Save URLs to playlist */
    esp_err_t (*next)    (void *playlist, int step, char **url_buff);   /*!< Get next URL in playlist */
    esp_err_t (*prev)    (void *playlist, int step, char **url_buff);   /*!< Get previous URL in playlist */
    esp_err_t (*reset)   (void *playlist);                              /*!< Reset the playlist */
    esp_err_t (*choose)  (void *playlist, int url_id, char **url_buff); /*!< Get url by url id */
    esp_err_t (*current) (void *playlist, char **url_buff);             /*!< Get current URL in playlist */
    esp_err_t (*destroy) (void *playlist);                              /*!< Destroy playlist */
    bool      (*exist)   (void *playlist, const char *url);             /*!< Judge whether the url exists */
    int   (*get_url_num) (void *playlist);                              /*!< Get number of URLS in current playlist */
    int   (*get_url_id)  (void *playlist);                              /*!< Get current url id in playlist */
    playlist_type_t      type;                                          /*!< Type of playlist */
    esp_err_t (*remove_by_url)(void *playlist, const char *url);        /*!< Remove the corresponding url */
    esp_err_t (*remove_by_id)(void *playlist, uint16_t url_id);         /*!< Remove url by id */

} playlist_operation_t;

/**
 * @brief Information of playlist manager node
 */
typedef struct {
    void                           *playlist;                    /*!< Specific playlist's pointer */
    esp_err_t (*get_operation)(playlist_operation_t *operation); /*!< Function pointer to get playlists' handle */
} playlist_operator_t;

typedef playlist_operator_t *playlist_operator_handle_t;
typedef struct playlist_handle *playlist_handle_t;

/**
 * @brief Create a playlist manager handle
 *
 * @return  playlist handle  success
 *          NULL             failed
 */
playlist_handle_t playlist_create(void);

/**
 * @brief Create a playlist manager and add playlist handle to it
 *
 * @note  The partition playlist can only be added once, or it will be overwrited by the newest partiiton playlist
 * @note  Different lists must use different IDs, because even if they are in different handles,
 *        list_id is the only indicator that distinguishes them.
 *
 * @param handle      Playlist manager handle
 * @param list_handle The playlist handle to be added
 * @param list_id     The playlist id to be registered
 *
 * @return ESP_OK   success
 *         ESP_FAIL failed
 */
esp_err_t playlist_add(playlist_handle_t handle, playlist_operator_handle_t list_handle, uint8_t list_id);

/**
 * @brief Playlist checkout by list id
 *
 * @param handle  Playlist handle
 * @param id      Specified list id
 *
 * @return ESP_OK   success
 *         ESP_FAIL failed
 */
esp_err_t playlist_checkout_by_id(playlist_handle_t handle, uint8_t id);

/**
 * @brief Get number of playlists in the handle
 *
 * @param handle  Playlist handle
 *
 * @return success Number of playlists in handle
 *         failed  -1
 */
int playlist_get_list_num(playlist_handle_t handle);

/**
 * @brief Get current playlist type
 *
 * @param handle  Playlist handle
 *
 * @return success Type of current playlist
 *         failed  -1
 */
playlist_type_t playlist_get_current_list_type(playlist_handle_t handle);

/**
 * @brief Get current playlist id
 *
 * @param handle  Playlist handle
 *
 * @return success Current playlist id
 *         failed  -1
 */
int playlist_get_current_list_id(playlist_handle_t handle);

/**
 * @brief Get current URL in current playlist
 *
 * @param      handle        Playlist handle
 * @param[out] url_buff      A second rank pointer to get a address of URL
 *
 * @return ESP_OK   success
 *         ESP_FAIL failed
 */
esp_err_t playlist_get_current_list_url(playlist_handle_t handle, char **url_buff);

/**
 * @brief Get number of URLs in current playlist
 *
 * @param handle       Playlist handle
 *
 * @return Number of URLS in current playlsit
 */
int playlist_get_current_list_url_num(playlist_handle_t handle);

/**
 * @brief Get current url id in current playlist
 *
 * @param handle       Playlist handle
 *
 * @return Current url's id in current playlsit
 */
int playlist_get_current_list_url_id(playlist_handle_t handle);

/**
 * @brief Save a URL to the current playlist
 *
 * @param handle  Playlist handle
 * @param url     The URL to be saved ot sdcard
 *
 * @return ESP_OK   success
 *         ESP_FAIL failed
 */
esp_err_t playlist_save(playlist_handle_t handle, const char *url);

/**
 * @brief Next URl in current playlist
 *
 * @param      handle        Playlist handle
 * @param      step          Next steps from current position
 * @param[out] url_buff      A second rank pointer to get a address of URL
 *
 * @return ESP_OK   success
 *         ESP_FAIL failed
 */
esp_err_t playlist_next(playlist_handle_t handle, int step, char **url_buff);

/**
 * @brief Previous URL in current playlist
 *
 * @param      handle        Playlist handle
 * @param      step          Previous steps from current position
 * @param[out] url_buff      A second rank pointer to get a address of URL
 *
 * @return ESP_OK   success
 *         ESP_FAIL failed
 */
esp_err_t playlist_prev(playlist_handle_t handle, int step, char **url_buff);

/**
 * @brief Choose a url by url id
 *
 * @param      handle          Playlist handle
 * @param      url_id          The id of url in current list
 * @param[out] url_buff        A second rank pointer to get a address of URL
 *
 * @return ESP_OK    success
 * @return ESP_FAIL  failed
 */
esp_err_t playlist_choose(playlist_handle_t handle, int url_id, char **url_buff);

/**
 * @brief Show URLs in current playlist
 *
 * @param handle  Playlist handle
 *
 * @return ESP_OK   success
 *         ESP_FAIL failed
 */
esp_err_t playlist_show(playlist_handle_t handle);

/**
 * @brief Reset current playlist
 *
 * @param handle   Playlist handle
 *
 * @return ESP_OK   success
 *         ESP_FAIL failed
 */
esp_err_t playlist_reset(playlist_handle_t handle);

/**
 * @brief Remove corresponding url
 *
 * @param handle   Playlist handle
 * @param url      The url to be removed
 *
 * @return ESP_OK   success
 *         ESP_FAIL failed
 */
esp_err_t playlist_remove_by_url(playlist_handle_t handle, const char *url);

/**
 * @brief Remove url by url id
 *
 * @param handle   Playlist handle
 * @param url_id   The id of url to be removed
 *
 * @return ESP_OK   success
 *         ESP_FAIL failed
 */
esp_err_t playlist_remove_by_url_id(playlist_handle_t handle, uint16_t url_id);

/**
 * @brief Judge whether the url exists in current playlist
 *
 * @param handle   Playlist handle
 * @param url      The url to be checked
 *
 * @return true    existence
 *         false   Non-existent
 */
bool playlist_exist(playlist_handle_t handle, const char *url);

/**
 * @brief Destroy all playlists in the handle
 *
 * @param handle   Playlist handle
 *
 * @return ESP_OK   success
 *         ESP_FAIL failed
 */
esp_err_t playlist_destroy(playlist_handle_t handle);

#ifdef __cplusplus
}
#endif

#endif
