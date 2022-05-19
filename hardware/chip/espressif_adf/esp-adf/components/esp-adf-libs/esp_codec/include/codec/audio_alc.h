/*
 * ESPRESSIF MIT License
 *
 * Copyright (c) 2019 <ESPRESSIF SYSTEMS (SHANGHAI) CO., LTD.>
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

#ifndef _AUDIO_ALC_H_
#define _AUDIO_ALC_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief      ALC Configuration
 */
typedef struct {
    int volume;                         /*!< The volume to be set to; The default is 0. 0 means the original valume of input stream. If the value is -64, the volume of input stream is silent. */
    int channel;                        /*!< The number of channels for input stream */
    int out_rb_size;                    /*!< Size of output ringbuffer */
    int task_stack;                     /*!< Task stack size */
    int task_core;                      /*!< Task running in core (0 or 1) */
    int task_prio;                      /*!< Task priority (based on freeRTOS priority) */
    bool stack_in_ext;                  /*!< Try to allocate stack in external memory */
} alc_volume_setup_cfg_t;

#define ALC_VOLUME_SET_TASK_STACK       (4 * 1024)
#define ALC_VOLUME_SET_TASK_CORE        (0)
#define ALC_VOLUME_SET_TASK_PRIO        (5)
#define ALC_VOLUME_SET_RINGBUFFER_SIZE  (8 * 1024)

#define DEFAULT_ALC_VOLUME_SETUP_CONFIG() {                                         \
        .volume = 0,                                                                \
        .channel = 1,                                                               \
        .out_rb_size = ALC_VOLUME_SET_RINGBUFFER_SIZE,                              \
        .task_stack = ALC_VOLUME_SET_TASK_STACK,                                    \
        .task_core = ALC_VOLUME_SET_TASK_CORE,                                      \
        .task_prio = ALC_VOLUME_SET_TASK_PRIO,                                      \
        .stack_in_ext = true,                                                       \
    }

/**
 * @brief      Set the number of channels of input audio stream
 *
 * @param      self       Audio element handle
 * @param      channels   the number of channels for input stream
 */
void alc_volume_setup_set_channel(audio_element_handle_t self, int channel);

/**
 * @brief      Set the volume of input audio stream with ALC
 *
 * @param      self       Audio element handle
 * @param      volume     the set volume of input audio stream
 */
void alc_volume_setup_set_volume(audio_element_handle_t self, int volume);

/**
 * @brief      Get the volume of input audio stream
 *
 * @param      self       Audio element handle
 *
 * @return     the volume of input audio stream
 */
int alc_volume_setup_get_volume(audio_element_handle_t self);

/**
 * @brief      Create an Audio Element handle to set volume of input audio stream with ALC
 *
 * @param      config     the configuration
 * @return     the audio element handler
 */
audio_element_handle_t alc_volume_setup_init(alc_volume_setup_cfg_t *config);

#ifdef __cplusplus
}
#endif

#endif

