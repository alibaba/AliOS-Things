// Copyright 2018 Espressif Systems (Shanghai) PTE LTD
// All rights reserved.

#ifndef _AUDIO_DEF_H_
#define _AUDIO_DEF_H_

#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ESP_ERR_AUDIO_BASE                  (0x80000 + 0x1000) /*!< Starting number of ESP audio error codes */

typedef enum {
    ESP_ERR_AUDIO_NO_ERROR                  = ESP_OK,
    ESP_ERR_AUDIO_FAIL                      = ESP_FAIL,

    /* esp audio errors */
    ESP_ERR_AUDIO_NO_INPUT_STREAM           = ESP_ERR_AUDIO_BASE + 1,  // 0x0x81001
    ESP_ERR_AUDIO_NO_OUTPUT_STREAM          = ESP_ERR_AUDIO_BASE + 2,
    ESP_ERR_AUDIO_NO_CODEC                  = ESP_ERR_AUDIO_BASE + 3,
    ESP_ERR_AUDIO_HAL_FAIL                  = ESP_ERR_AUDIO_BASE + 4,
    ESP_ERR_AUDIO_MEMORY_LACK               = ESP_ERR_AUDIO_BASE + 5,
    ESP_ERR_AUDIO_INVALID_URI               = ESP_ERR_AUDIO_BASE + 6,
    ESP_ERR_AUDIO_INVALID_PATH              = ESP_ERR_AUDIO_BASE + 7,
    ESP_ERR_AUDIO_INVALID_PARAMETER         = ESP_ERR_AUDIO_BASE + 8,
    ESP_ERR_AUDIO_NOT_READY                 = ESP_ERR_AUDIO_BASE + 9,
    ESP_ERR_AUDIO_NOT_SUPPORT               = ESP_ERR_AUDIO_BASE + 10,
    ESP_ERR_AUDIO_TIMEOUT                   = ESP_ERR_AUDIO_BASE + 11,
    ESP_ERR_AUDIO_ALREADY_EXISTS            = ESP_ERR_AUDIO_BASE + 12,
    ESP_ERR_AUDIO_LINK_FAIL                 = ESP_ERR_AUDIO_BASE + 13,
    ESP_ERR_AUDIO_UNKNOWN                   = ESP_ERR_AUDIO_BASE + 14,
    ESP_ERR_AUDIO_OUT_OF_RANGE              = ESP_ERR_AUDIO_BASE + 15,
    ESP_ERR_AUDIO_STOP_BY_USER              = ESP_ERR_AUDIO_BASE + 16,

    ESP_ERR_AUDIO_OPEN                      = ESP_ERR_AUDIO_BASE + 0x100,// 0x81100
    ESP_ERR_AUDIO_INPUT                     = ESP_ERR_AUDIO_BASE + 0x101,
    ESP_ERR_AUDIO_PROCESS                   = ESP_ERR_AUDIO_BASE + 0x102,
    ESP_ERR_AUDIO_OUTPUT                    = ESP_ERR_AUDIO_BASE + 0x103,
    ESP_ERR_AUDIO_CLOSE                     = ESP_ERR_AUDIO_BASE + 0x104,
} audio_err_t;

typedef enum {
    AUDIO_STATUS_UNKNOWN    = 0,
    AUDIO_STATUS_RUNNING    = 1,
    AUDIO_STATUS_PAUSED     = 2,
    AUDIO_STATUS_STOPPED    = 3,
    AUDIO_STATUS_FINISHED   = 4,
    AUDIO_STATUS_ERROR      = 5,
} esp_audio_status_t;

typedef enum {
    TERMINATION_TYPE_NOW    = 0,    /*!<  Audio operation will be terminated immediately */
    TERMINATION_TYPE_DONE   = 1,    /*!<  Audio operation will be stopped when finished */
    TERMINATION_TYPE_MAX,
} audio_termination_type_t;

typedef enum {
    ESP_AUDIO_PREFER_MEM     = 0,
    ESP_AUDIO_PREFER_SPEED   = 1,
} esp_audio_prefer_t;

typedef enum {
    MEDIA_SRC_TYPE_NULL          = 0,
    MEDIA_SRC_TYPE_MUSIC_BASE    = 0x100,
    MEDIA_SRC_TYPE_MUSIC_SD      = MEDIA_SRC_TYPE_MUSIC_BASE + 1,
    MEDIA_SRC_TYPE_MUSIC_HTTP    = MEDIA_SRC_TYPE_MUSIC_BASE + 2,
    MEDIA_SRC_TYPE_MUSIC_FLASH   = MEDIA_SRC_TYPE_MUSIC_BASE + 3,
    MEDIA_SRC_TYPE_MUSIC_A2DP    = MEDIA_SRC_TYPE_MUSIC_BASE + 4,
    MEDIA_SRC_TYPE_MUSIC_DLNA    = MEDIA_SRC_TYPE_MUSIC_BASE + 5,
    MEDIA_SRC_TYPE_MUSIC_RAW     = MEDIA_SRC_TYPE_MUSIC_BASE + 6,
    MEDIA_SRC_TYPE_MUSIC_MAX     = 0x1FF,

    MEDIA_SRC_TYPE_TONE_BASE     = 0x200,
    MEDIA_SRC_TYPE_TONE_SD       = MEDIA_SRC_TYPE_TONE_BASE + 1,
    MEDIA_SRC_TYPE_TONE_HTTP     = MEDIA_SRC_TYPE_TONE_BASE + 2,
    MEDIA_SRC_TYPE_TONE_FLASH    = MEDIA_SRC_TYPE_TONE_BASE + 3,
    MEDIA_SRC_TYPE_TONE_MAX      = 0x2FF,

    MEDIA_SRC_TYPE_RESERVE_BASE  = 0x800,
    MEDIA_SRC_TYPE_RESERVE_MAX   = 0xFFF,
} media_source_type_t;

/**
 * @brief esp_audio status information parameters
 */
typedef struct {
    esp_audio_status_t      status;         /*!< Status of esp_audio */
    audio_err_t             err_msg;        /*!< Status is `AUDIO_STATUS_ERROR`, err_msg will be setup */
    media_source_type_t     media_src;      /*!< Media source type*/
} esp_audio_state_t;

typedef void (*esp_audio_event_callback)(esp_audio_state_t *audio, void *ctx);
typedef esp_err_t (*audio_volume_set)(void *hd, int vol);
typedef esp_err_t (*audio_volume_get)(void *hd, int *vol);

#ifdef __cplusplus
}
#endif

#endif
