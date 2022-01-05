// Copyright 2018 Espressif Systems (Shanghai) PTE LTD
// All rights reserved.

#ifndef _AUDIO_TYPE_DEF_H_
#define _AUDIO_TYPE_DEF_H_

#ifdef __cplusplus
extern "C"
{
#endif

#define ESP_AUDIO_SAMPLERATE_8K (8000)
#define ESP_AUDIO_SAMPLERATE_11K (11025)
#define ESP_AUDIO_SAMPLERATE_16K (16000)
#define ESP_AUDIO_SAMPLERATE_22K (22050)
#define ESP_AUDIO_SAMPLERATE_32K (32000)
#define ESP_AUDIO_SAMPLERATE_44K (44100)
#define ESP_AUDIO_SAMPLERATE_48K (48000)

#define ESP_AUDIO_BIT_8 (8)
#define ESP_AUDIO_BIT_16 (16)
#define ESP_AUDIO_BIT_24 (24)
#define ESP_AUDIO_BIT_32 (32)

#define ESP_AUDIO_CHANNEL_MONO (1)
#define ESP_AUDIO_CHANNEL_DUAL (2)

#define ESP_AUDIO_U32_AT(ptr) (((uint32_t)(ptr)[0] << 24) | ((uint32_t)(ptr)[1] << 16) | ((uint32_t)(ptr)[2] << 8) | ((uint32_t)(ptr)[3]))
#define ESP_AUDIO_U64_AT(ptr) (((uint64_t)(ptr)[0] << 56) | ((uint64_t)(ptr)[1] << 48) | ((uint64_t)(ptr)[2] << 40) | ((uint64_t)(ptr)[3] << 32) | ((uint64_t)(ptr)[4] << 24) | ((uint64_t)(ptr)[5] << 16) | ((uint64_t)(ptr)[6] << 8) | ((uint64_t)(ptr)[7]))

typedef enum
{
    ESP_CODEC_TYPE_UNKNOW        = 0,
    ESP_CODEC_TYPE_RAW           = 1,
    ESP_CODEC_TYPE_WAV           = 2,
    ESP_CODEC_TYPE_MP3           = 3,
    ESP_CODEC_TYPE_AAC           = 4,
    ESP_CODEC_TYPE_OPUS          = 5,
    ESP_CODEC_TYPE_M4A           = 6,
    ESP_CODEC_TYPE_MP4           = 7,
    ESP_CODEC_TYPE_FLAC          = 8,
    ESP_CODEC_TYPE_OGG           = 9,
    ESP_CODEC_TYPE_TSAAC         = 10,
    ESP_CODEC_TYPE_AMRNB         = 11,
    ESP_CODEC_TYPE_AMRWB         = 12,  
    ESP_CODEC_TYPE_PCM           = 13,
    ESP_AUDIO_TYPE_M3U8          = 14,
    ESP_AUDIO_TYPE_PLS           = 15,  
    ESP_CODEC_TYPE_UNSUPPORT     = 16,
} esp_codec_type_t;

typedef enum
{
    ESP_DECODER_WORK_MODE_MANUAL = 0,
    ESP_DECODER_WORK_MODE_AUTO   = 1,
} esp_decoder_work_mode_t;

/**
 * @brief the enum value from `ESP_CODEC_ERR_OK` to `ESP_CODEC_ERR_TIMEOUT` rely on `audio_element_err_t` which in `audio_element.h`.
 */
typedef enum
{   
    ESP_CODEC_ERR_CONTINUE       = 1,
    ESP_CODEC_ERR_OK             = 0,
    ESP_CODEC_ERR_FAIL           = -1,
    ESP_CODEC_ERR_DONE           = -2,
    ESP_CODEC_ERR_ABORT          = -3,
    ESP_CODEC_ERR_TIMEOUT        = -4,
    ESP_CODEC_ERR_UNSYNC         = -5,
    ESP_CODEC_ERR_UNSUPPORT      = -6,
    ESP_CODEC_ERR_PARSE          = -7,
    ESP_CODEC_ERR_INFO           = -8,
    ESP_CODEC_ERR_INPUT          = -9,
    ESP_CODEC_ERR_NO_MEM         = -10,
} esp_codec_err_t;

/**
 * @brief      Gain codec extension. need to sync by `esp_codec_type_t`.
 *              eg. esp_codec_type_t codec_type = ESP_CODEC_TYPE_MP3;
 *                  char* ext = get_codec_ext(codec_type);
 *                  the value of ext is `mp3`.
 *
 * @param      codec_type   the value of `esp_codec_type_t`
 *
 * @return     codec extension
 */
const char * get_codec_ext(esp_codec_type_t codec_type);

#ifdef __cplusplus
}
#endif

#endif
