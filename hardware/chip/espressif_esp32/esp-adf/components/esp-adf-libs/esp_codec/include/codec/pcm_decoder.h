/*
 * ESPRESSIF MIT License
 *
 * Copyright (c) 2020 <ESPRESSIF SYSTEMS (SHANGHAI) CO., LTD.>
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

#ifndef _PCM_DECODER_H_
#define _PCM_DECODER_H_

#include "esp_err.h"
#include "audio_element.h"

#ifdef __cplusplus
extern "C" {
#endif

#define PCM_DECODER_TASK_STACK          (3 * 1024)
#define PCM_DECODER_TASK_CORE           (0)
#define PCM_DECODER_TASK_PRIO           (5)
#define PCM_DECODER_RINGBUFFER_SIZE     (8 * 1024)

#define DEFAULT_PCM_DECODER_CONFIG() {                  \
    .out_rb_size        = PCM_DECODER_RINGBUFFER_SIZE,  \
    .task_stack         = PCM_DECODER_TASK_STACK,       \
    .task_core          = PCM_DECODER_TASK_CORE,        \
    .task_prio          = PCM_DECODER_TASK_PRIO,        \
    .stack_in_ext       = true,                         \
    .rate               = 44100,                        \
    .bits               = 16,                           \
    .channels           = 2,                            \
}

/**
 * @note The sample rate and channle of PCM should be specific by user with `audio_element_setinfo`.
 */

/**
 * @brief      PCM Decoder configurations
 */
typedef struct {
    int                     out_rb_size;    /*!< Size of output ringbuffer */
    int                     task_stack;     /*!< Task stack size */
    int                     task_core;      /*!< Task running in core (0 or 1) */
    int                     task_prio;      /*!< Task priority (based on freeRTOS priority) */
    bool                    stack_in_ext;   /*!< Try to allocate stack in external memory */
    int                     rate;           /*!< Sample rates in Hz */
    int                     bits;           /*!< Bit wide */
    int                     channels;       /*!< Number of audio channel, mono is 1, stereo is 2 */
} pcm_decoder_cfg_t;

/**
 * @brief      Create an Audio Element handle to decode incoming PCM data
 *
 * @param      config  The configuration
 *
 * @return     The audio element handle
 */
audio_element_handle_t pcm_decoder_init(pcm_decoder_cfg_t *config);


#ifdef __cplusplus
}
#endif

#endif
