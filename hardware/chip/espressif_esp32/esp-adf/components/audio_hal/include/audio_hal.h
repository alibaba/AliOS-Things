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

#ifndef _AUDIO_HAL_H_
#define _AUDIO_HAL_H_
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"
#include "audio_error.h"

#ifdef __cplusplus
extern "C" {
#endif

#define AUDIO_HAL_VOL_DEFAULT 70

typedef struct audio_hal *audio_hal_handle_t;

/**
 * @brief Select media hal codec mode
 */
typedef enum {
    AUDIO_HAL_CODEC_MODE_ENCODE = 1,  /*!< select adc */
    AUDIO_HAL_CODEC_MODE_DECODE,      /*!< select dac */
    AUDIO_HAL_CODEC_MODE_BOTH,        /*!< select both adc and dac */
    AUDIO_HAL_CODEC_MODE_LINE_IN,     /*!< set adc channel */
} audio_hal_codec_mode_t;

/**
 * @brief Select adc channel for input mic signal
 */
typedef enum {
    AUDIO_HAL_ADC_INPUT_LINE1 = 0x00,  /*!< mic input to adc channel 1 */
    AUDIO_HAL_ADC_INPUT_LINE2,         /*!< mic input to adc channel 2 */
    AUDIO_HAL_ADC_INPUT_ALL,           /*!< mic input to both channels of adc */
    AUDIO_HAL_ADC_INPUT_DIFFERENCE,    /*!< mic input to adc difference channel */
} audio_hal_adc_input_t;

/**
 * @brief Select channel for dac output
 */
typedef enum {
    AUDIO_HAL_DAC_OUTPUT_LINE1 = 0x00,  /*!< dac output signal to channel 1 */
    AUDIO_HAL_DAC_OUTPUT_LINE2,         /*!< dac output signal to channel 2 */
    AUDIO_HAL_DAC_OUTPUT_ALL,           /*!< dac output signal to both channels */
} audio_hal_dac_output_t;

/**
 * @brief Select operating mode i.e. start or stop for audio codec chip
 */
typedef enum {
    AUDIO_HAL_CTRL_STOP  = 0x00,  /*!< set stop mode */
    AUDIO_HAL_CTRL_START = 0x01,  /*!< set start mode */
} audio_hal_ctrl_t;

/**
 * @brief Select I2S interface operating mode i.e. master or slave for audio codec chip
 */
typedef enum {
    AUDIO_HAL_MODE_SLAVE = 0x00,   /*!< set slave mode */
    AUDIO_HAL_MODE_MASTER = 0x01,  /*!< set master mode */
} audio_hal_iface_mode_t;

/**
 * @brief Select I2S interface samples per second
 */
typedef enum {
    AUDIO_HAL_08K_SAMPLES,   /*!< set to  8k samples per second */
    AUDIO_HAL_11K_SAMPLES,   /*!< set to 11.025k samples per second */
    AUDIO_HAL_16K_SAMPLES,   /*!< set to 16k samples in per second */
    AUDIO_HAL_22K_SAMPLES,   /*!< set to 22.050k samples per second */
    AUDIO_HAL_24K_SAMPLES,   /*!< set to 24k samples in per second */
    AUDIO_HAL_32K_SAMPLES,   /*!< set to 32k samples in per second */
    AUDIO_HAL_44K_SAMPLES,   /*!< set to 44.1k samples per second */
    AUDIO_HAL_48K_SAMPLES,   /*!< set to 48k samples per second */
} audio_hal_iface_samples_t;

/**
 * @brief Select I2S interface number of bits per sample
 */
typedef enum {
    AUDIO_HAL_BIT_LENGTH_16BITS = 1,   /*!< set 16 bits per sample */
    AUDIO_HAL_BIT_LENGTH_24BITS,       /*!< set 24 bits per sample */
    AUDIO_HAL_BIT_LENGTH_32BITS,       /*!< set 32 bits per sample */
} audio_hal_iface_bits_t;

/**
 * @brief Select I2S interface format for audio codec chip
 */
typedef enum {
    AUDIO_HAL_I2S_NORMAL = 0,  /*!< set normal I2S format */
    AUDIO_HAL_I2S_LEFT,        /*!< set all left format */
    AUDIO_HAL_I2S_RIGHT,       /*!< set all right format */
    AUDIO_HAL_I2S_DSP,         /*!< set dsp/pcm format */
} audio_hal_iface_format_t;

/**
 * @brief I2s interface configuration for audio codec chip
 */
typedef struct {
    audio_hal_iface_mode_t mode;        /*!< audio codec chip mode */
    audio_hal_iface_format_t fmt;       /*!< I2S interface format */
    audio_hal_iface_samples_t samples;  /*!< I2S interface samples per second */
    audio_hal_iface_bits_t bits;        /*!< i2s interface number of bits per sample */
} audio_hal_codec_i2s_iface_t;

/**
 * @brief Configure media hal for initialization of audio codec chip
 */
typedef struct {
    audio_hal_adc_input_t adc_input;       /*!< set adc channel */
    audio_hal_dac_output_t dac_output;     /*!< set dac channel */
    audio_hal_codec_mode_t codec_mode;     /*!< select codec mode: adc, dac or both */
    audio_hal_codec_i2s_iface_t i2s_iface; /*!< set I2S interface configuration */
} audio_hal_codec_config_t;

/**
 * @brief Configuration of functions and variables used to operate audio codec chip
 */
typedef struct audio_hal {
    esp_err_t (*audio_codec_initialize)(audio_hal_codec_config_t *codec_cfg);                                /*!< initialize codec */
    esp_err_t (*audio_codec_deinitialize)(void);                                                             /*!< deinitialize codec */
    esp_err_t (*audio_codec_ctrl)(audio_hal_codec_mode_t mode, audio_hal_ctrl_t ctrl_state);                 /*!< control codec mode and state */
    esp_err_t (*audio_codec_config_iface)(audio_hal_codec_mode_t mode, audio_hal_codec_i2s_iface_t *iface);  /*!< configure i2s interface */
    esp_err_t (*audio_codec_set_mute) (bool mute);                                                           /*!< set codec mute */
    esp_err_t (*audio_codec_set_volume)(int volume);                                                         /*!< set codec volume */
    esp_err_t (*audio_codec_get_volume)(int *volume);                                                        /*!< get codec volume */
    xSemaphoreHandle audio_hal_lock;                                                                         /*!< semaphore of codec */
    void *handle;                                                                                            /*!< handle of audio codec */
} audio_hal_func_t;


/**
 * @brief Initialize media codec driver
 *
 * @note If selected codec has already been installed, it'll return the audio_hal handle.
 *
 * @param audio_hal_conf Configure structure audio_hal_config_t
 * @param audio_hal_func Structure containing functions used to operate audio the codec chip
 *
 * @return  int, 0--success, others--fail
 */
audio_hal_handle_t audio_hal_init(audio_hal_codec_config_t *audio_hal_conf, audio_hal_func_t *audio_hal_func);

/**
 * @brief Uninitialize media codec driver
 *
 * @param audio_hal reference function pointer for selected audio codec
 *
 * @return  int, 0--success, others--fail
 */
esp_err_t audio_hal_deinit(audio_hal_handle_t audio_hal);

/**
 * @brief Start/stop codec driver
 *
 * @param audio_hal reference function pointer for selected audio codec
 * @param mode select media hal codec mode either encode/decode/or both to start from audio_hal_codec_mode_t
 * @param audio_hal_ctrl select start stop state for specific mode
 *
 * @return     int, 0--success, others--fail
 */
esp_err_t audio_hal_ctrl_codec(audio_hal_handle_t audio_hal, audio_hal_codec_mode_t mode, audio_hal_ctrl_t audio_hal_ctrl);

/**
 * @brief Set codec I2S interface samples rate & bit width and format either I2S or PCM/DSP.
 *
 * @param audio_hal reference function pointer for selected audio codec
 * @param mode select media hal codec mode either encode/decode/or both to start from audio_hal_codec_mode_t
 * @param iface I2S sample rate (ex: 16000, 44100), I2S bit width (16, 24, 32),I2s format (I2S, PCM, DSP).
 *
 * @return
 *     - 0   Success
 *     - -1  Error
 */
esp_err_t audio_hal_codec_iface_config(audio_hal_handle_t audio_hal, audio_hal_codec_mode_t mode, audio_hal_codec_i2s_iface_t *iface);

/**
 * @brief Set voice mute. Enables or disables DAC mute of a codec.
 *        @note `audio_hal_get_volume` will still give a non-zero number in mute state. It will be set to that number when speaker is unmuted.
 *
 * @param audio_hal reference function pointer for selected audio codec
 * @param mute      true/false. If true speaker will be muted and if false speaker will be unmuted.
 *
 * @return     int, 0--success, others--fail
 */
esp_err_t audio_hal_set_mute(audio_hal_handle_t audio_hal, bool mute);

/**
 * @brief Set voice volume.
 *        @note if volume is 0, mute is enabled,range is 0-100.
 *
 * @param audio_hal reference function pointer for selected audio codec
 * @param volume value of volume in percent(%)
 *
 * @return     int, 0--success, others--fail
 */
esp_err_t audio_hal_set_volume(audio_hal_handle_t audio_hal, int volume);

/**
 * @brief get voice volume.
 *        @note if volume is 0, mute is enabled, range is 0-100.
 *
 * @param audio_hal reference function pointer for selected audio codec
 * @param volume value of volume in percent returned(%)
 *
 * @return     int, 0--success, others--fail
 */
esp_err_t audio_hal_get_volume(audio_hal_handle_t audio_hal, int *volume);


#ifdef __cplusplus
}
#endif

#endif //__AUDIO_HAL_H__
