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

#ifndef _TTS_STREAM_H_
#define _TTS_STREAM_H_

#include "audio_error.h"
#include "audio_element.h"

#ifdef __cplusplus
extern "C" {
#endif

/* The speech speed speed of synthesized speech */
typedef enum {
    TTS_VOICE_SPEED_0,          /* the slowest speed */
    TTS_VOICE_SPEED_1,
    TTS_VOICE_SPEED_2,
    TTS_VOICE_SPEED_3,
    TTS_VOICE_SPEED_4,
    TTS_VOICE_SPEED_5,          /* the fastest speech */
    TTS_VOICE_SPEED_MAX,
} tts_voice_speed_t;

/**
 * @brief   TTS Stream configurations, if any entry is zero then the configuration will be set to default values
 */
typedef struct {
    audio_stream_type_t     type;               /*!< Stream type */
    int                     buf_sz;             /*!< Audio Element Buffer size */
    int                     out_rb_size;        /*!< Size of output ringbuffer */
    int                     task_stack;         /*!< Task stack size */
    int                     task_core;          /*!< Task running in core (0 or 1) */
    int                     task_prio;          /*!< Task priority (based on freeRTOS priority) */
    bool                    ext_stack;          /*!< Allocate stack on extern ram */
} tts_stream_cfg_t;

#define TTS_STREAM_BUF_SIZE             (4096)
#define TTS_STREAM_TASK_STACK           (3072)
#define TTS_STREAM_TASK_CORE            (0)
#define TTS_STREAM_TASK_PRIO            (4)
#define TTS_STREAM_RINGBUFFER_SIZE      (8 * 1024)

#define TTS_STREAM_CFG_DEFAULT() {                  \
    .type = AUDIO_STREAM_READER,                    \
    .buf_sz = TTS_STREAM_BUF_SIZE,                  \
    .out_rb_size = TTS_STREAM_RINGBUFFER_SIZE,      \
    .task_stack = TTS_STREAM_TASK_STACK,            \
    .task_core = TTS_STREAM_TASK_CORE,              \
    .task_prio = TTS_STREAM_TASK_PRIO,              \
    .ext_stack = false,                             \
}

/**
 * @brief      Create a handle to an Audio Element to stream data from TTS to another Element,
 *             the stream type only support AUDIO_STREAM_READER for now.
 *
 * @param      config  The configuration
 *
 * @return     The Audio Element handle
 */
audio_element_handle_t tts_stream_init(tts_stream_cfg_t *config);

/**
 * @brief      Set tts stream strings.
 *
 * @param[in]  el       The audio element handle
 * @param[in]  string   The string pointer
 *
 * @return
 *     - ESP_OK
 *     - ESP_FAIL
 */
esp_err_t tts_stream_set_strings(audio_element_handle_t el, const char *strings);

/**
 * @brief Setting tts stream voice speed.
 *
 * @param[in] handle    The esp_audio instance
 * @param[in] speed    Speed will be set. 0-5 is legal. 0 is the slowest speed.
 *
 * @return
 *     - ESP_OK
 *     - ESP_FAIL
 */
esp_err_t tts_stream_set_speed(audio_element_handle_t el, tts_voice_speed_t speed);

/**
 * @brief Get tts stream voice speed.
 *
 * @param[in] handle    The esp_audio instance
 * @param[in] speed     Return tts stream Speed will be [0,5]
 *
 * @return
 *     - ESP_OK
 *     - ESP_FAIL
 */
esp_err_t tts_stream_get_speed(audio_element_handle_t el, tts_voice_speed_t *speed);

#ifdef __cplusplus
}
#endif

#endif
