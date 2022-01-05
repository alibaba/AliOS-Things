// Copyright 2018 Espressif Systems (Shanghai) PTE LTD
// All rights reserved.

/**
 * @brief     ESP_Decoder is a decoder library Espressif develops which could automatically
 *            recognize audio formats based on the input audio streams.
 */

#ifndef _ESP_DECODER_H_
#define _ESP_DECODER_H_

#include "esp_err.h"
#include "audio_element.h"
#include "audio_common.h"
#include "auto_wav_dec.h"
#include "auto_aac_dec.h"
#include "auto_mp3_dec.h"
#include "auto_amr_dec.h"
#include "auto_flac_dec.h"
#include "auto_ogg_dec.h"
#include "auto_opus_dec.h"
#include "auto_pcm_dec.h"
#include "audio_type_def.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define ESP_DECODER_TASK_STACK_SIZE (3 * 1024)
#define ESP_DECODER_TASK_CORE (0)
#define ESP_DECODER_TASK_PRIO (5)
#define ESP_DECODER_RINGBUFFER_SIZE (10 * 1024)

/**
 * @brief      To provide audio stream element
 */
typedef struct{
    esp_err_t (*decoder_open)(audio_element_handle_t el);                         /*!< Open an Audio Element type data */
    esp_codec_err_t (*decoder_process)(audio_element_handle_t el);                /*!< Audio data to decoding */
    esp_err_t (*decoder_close)(audio_element_handle_t el);                        /*!< Close an Audio Element type data */
    esp_err_t (*decoder_seek)(audio_element_handle_t self, void *in_data, int in_size, void *out_data, int *out_size);  /*!< Seek Audio data in `timePos` to decode */
    esp_codec_type_t decoder_type;                                                /*!< Type of Audio file */
} audio_decoder_t;

/**
 * @brief      The audio formats list
 */
typedef struct decoder_node{
    audio_decoder_t audio_decoder;                                   /*!< Audio stream element */
    struct decoder_node *next;                                       /*!< The audio formats list */
} audio_decoder_node_t;

/**
 * @brief      Auto audio decoder configuration
 */
typedef struct{
    int out_rb_size;                                                /*!< Size of output ringbuffer */
    int task_stack;                                                 /*!< Task stack size */
    int task_core;                                                  /*!< CPU core number (0 or 1) where decoder task in running */
    int task_prio;                                                  /*!< Task priority (based on freeRTOS priority) */
    bool stack_in_ext;                                              /*!< Try to allocate stack in external memory */
} esp_decoder_cfg_t;

#define DEFAULT_ESP_WAV_DECODER_CONFIG()          \
    {                                             \
        .decoder_open = wav_decoder_open,         \
        .decoder_process = wav_decoder_process,   \
        .decoder_close = wav_decoder_close,       \
        .decoder_seek = wav_decoder_get_pos,      \
        .decoder_type = ESP_CODEC_TYPE_WAV,       \
    }
#define DEFAULT_ESP_MP3_DECODER_CONFIG()          \
    {                                             \
        .decoder_open = mp3_decoder_open,         \
        .decoder_process = mp3_decoder_process,   \
        .decoder_close = mp3_decoder_close,       \
        .decoder_seek = mp3_decoder_get_pos,      \
        .decoder_type = ESP_CODEC_TYPE_MP3,       \
    }
#define DEFAULT_ESP_AMRNB_DECODER_CONFIG()        \
    {                                             \
        .decoder_open = amr_decoder_open,         \
        .decoder_process = amr_decoder_process,   \
        .decoder_close = amr_decoder_close,       \
        .decoder_seek = amr_decoder_get_pos,      \
        .decoder_type = ESP_CODEC_TYPE_AMRNB,     \
    }
#define DEFAULT_ESP_AMRWB_DECODER_CONFIG()        \
    {                                             \
        .decoder_open = amr_decoder_open,         \
        .decoder_process = amr_decoder_process,   \
        .decoder_close = amr_decoder_close,       \
        .decoder_seek = amr_decoder_get_pos,      \
        .decoder_type = ESP_CODEC_TYPE_AMRWB,     \
    }
#define DEFAULT_ESP_AAC_DECODER_CONFIG()          \
    {                                             \
        .decoder_open = aac_decoder_open,         \
        .decoder_process = aac_decoder_process,   \
        .decoder_close = aac_decoder_close,       \
        .decoder_seek = aac_decoder_get_pos,      \
        .decoder_type = ESP_CODEC_TYPE_AAC,       \
    }
#define DEFAULT_ESP_M4A_DECODER_CONFIG()          \
    {                                             \
        .decoder_open = aac_decoder_open,         \
        .decoder_process = aac_decoder_process,   \
        .decoder_close = aac_decoder_close,       \
        .decoder_seek = aac_decoder_get_pos,      \
        .decoder_type = ESP_CODEC_TYPE_M4A,       \
    }
#define DEFAULT_ESP_TS_DECODER_CONFIG()           \
    {                                             \
        .decoder_open = aac_decoder_open,         \
        .decoder_process = aac_decoder_process,   \
        .decoder_close = aac_decoder_close,       \
        .decoder_seek = aac_decoder_get_pos,      \
        .decoder_type = ESP_CODEC_TYPE_TSAAC,     \
    }
#define DEFAULT_ESP_OGG_DECODER_CONFIG()          \
    {                                             \
        .decoder_open = ogg_decoder_open,         \
        .decoder_process = ogg_decoder_process,   \
        .decoder_close = ogg_decoder_close,       \
        .decoder_seek = ogg_decoder_get_pos,      \
        .decoder_type = ESP_CODEC_TYPE_OGG,    \
    }
#define DEFAULT_ESP_OPUS_DECODER_CONFIG()          \
    {                                              \
        .decoder_open = opus_decoder_open,         \
        .decoder_process = opus_decoder_process,   \
        .decoder_close = opus_decoder_close,       \
        .decoder_seek = opus_decoder_get_pos,      \
        .decoder_type = ESP_CODEC_TYPE_OPUS,       \
    }
#define DEFAULT_ESP_FLAC_DECODER_CONFIG()          \
    {                                              \
        .decoder_open = flac_decoder_open,         \
        .decoder_process = flac_decoder_process,   \
        .decoder_close = flac_decoder_close,       \
        .decoder_seek = flac_decoder_get_pos,      \
        .decoder_type = ESP_CODEC_TYPE_FLAC,    \
    }

#define DEFAULT_ESP_PCM_DECODER_CONFIG()           \
    {                                              \
        .decoder_open = pcm_decoder_open,          \
        .decoder_process = pcm_decoder_process,    \
        .decoder_close = pcm_decoder_close,        \
        .decoder_seek = pcm_decoder_get_pos,       \
        .decoder_type = ESP_CODEC_TYPE_PCM,        \
    }

#define DEFAULT_ESP_DECODER_CONFIG()                \
    {                                               \
        .out_rb_size = ESP_DECODER_RINGBUFFER_SIZE, \
        .task_stack = ESP_DECODER_TASK_STACK_SIZE,  \
        .task_core = ESP_DECODER_TASK_CORE,         \
        .task_prio = ESP_DECODER_TASK_PRIO,         \
        .stack_in_ext = true,                       \
    }

/**
* @brief      Create an Audio Element handler to decode incoming audio data.
*             - The handler is created to automatically recognize audio formats.
*             - The creation of the handler is dependent on the array of user’s audio decoder list.
*
* @param      config              The configuration
* @param      audio_decoder_list  The audio formats list
* @param      list_size           The number of audio formats
*
* @return     The audio element handle
*/
audio_element_handle_t esp_decoder_init(esp_decoder_cfg_t *config, audio_decoder_t *audio_decoder_list, int list_size);

#ifdef __cplusplus
}
#endif

#endif
