// Copyright 2018 Espressif Systems (Shanghai) PTE LTD
// All rights reserved.

#ifndef _EQUALIZER_H_
#define _EQUALIZER_H_

#include "esp_err.h"
#include "audio_element.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief      Equalizer Configuration
 */
typedef struct equalizer_cfg {
    int samplerate;     /*!< Audio sample rate (in Hz)*/
    int channel;        /*!< Number of audio channels (Mono=1, Dual=2) */
    int *set_gain;      /*!< Equalizer gain */
    int out_rb_size;    /*!< Size of output ring buffer */
    int task_stack;     /*!< Task stack size */
    int task_core;      /*!< Task running in core...*/
    int task_prio;      /*!< Task priority*/
    bool stack_in_ext;  /*!< Try to allocate stack in external memory */
} equalizer_cfg_t;

#define EQUALIZER_TASK_STACK       (4 * 1024)
#define EQUALIZER_TASK_CORE        (0)
#define EQUALIZER_TASK_PRIO        (5)
#define EQUALIZER_RINGBUFFER_SIZE  (8 * 1024)

/**
* @note  `set_value_gain` is defined in c file.
*         values is {-13, -13, -13, -13, -13, -13, -13, -13, -13, -13, -13, -13, -13, -13, -13, -13, -13, -13, -13, -13};
*/
extern int set_value_gain[];

#define DEFAULT_EQUALIZER_CONFIG() {                    \
        .samplerate     = 48000,                        \
        .channel        = 1,                            \
        .set_gain       = set_value_gain,               \
        .out_rb_size    = EQUALIZER_RINGBUFFER_SIZE,    \
        .task_stack     = EQUALIZER_TASK_STACK,         \
        .task_core      = EQUALIZER_TASK_CORE,          \
        .task_prio      = EQUALIZER_TASK_PRIO,          \
        .stack_in_ext   = true,                         \
    }

/**
 * @brief      Set the audio sample rate and the number of channels to be processed by the equalizer.
 *
 * @param      self       Audio element handle
 * @param      rate       Audio sample rate
 * @param      ch         Audio channel
 *
 * @return
 *             ESP_OK
 *             ESP_FAIL
 */
esp_err_t equalizer_set_info(audio_element_handle_t self, int rate, int ch);

/**
 * @brief      Set the audio gain to be processed by the equalizer.
 *
 * @param      self                      Audio element handle
 * @param      index                     the position of center frequencies of equalizer
 * @param      value_gain                the value of audio gain which in `index`
 * @param      is_channels_gain_equal    if Number of audio channel is equal 2, the value of audio gains which two channels are equal by checking `is_channels_gain_equal`. if `is_channels_gain_equal` is `true`,it means equal,otherwise unequal.
 *
 * @return
 *             ESP_OK
 *             ESP_FAIL
 */
esp_err_t equalizer_set_gain_info(audio_element_handle_t self, int index, int value_gain, bool is_channels_gain_equal);

/**
 * @brief      Create an Audio Element handle that equalizes incoming data.
 *
 * @param      config  The configuration
 *
 * @return     The created audio element handle
 */
audio_element_handle_t equalizer_init(equalizer_cfg_t *config);

#ifdef __cplusplus
}
#endif

#endif