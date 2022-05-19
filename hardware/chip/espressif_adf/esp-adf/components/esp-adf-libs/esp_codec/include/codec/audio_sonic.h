// Copyright 2018 Espressif Systems (Shanghai) PTE LTD
// All rights reserved.

#ifndef _AUDIO_SONIC_H_
#define _AUDIO_SONIC_H_

#include "esp_err.h"
#include "audio_element.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
* @brief      Information on audio file and configuration parameters required by sonic to process the file.
*/
typedef struct {
    int samplerate;                  /*!< Audio file sample rate (in Hz)*/
    int channel;                     /*!< Number of audio file channels (Mono=1, Dual=2) */
    int resample_linear_interpolate; /*!< Flag of using simple linear interpolation. 1 indicates using simple linear interpolation. 0 indicates not using simple linear interpolation.*/
    float pitch;                     /*!< Scale factor of pitch of audio file. If the value of 'pitch' is 0.3, the pitch of audio file processed by sonic islower than the original. If the value of 'pitch' is 1.3, the pitch of audio file processed by sonic is 30% higher than the original.*/
    float speed;                     /*!< Scale factor of speed of audio file. If the value of 'speed' is 0.3, the speed of audio file processed by sonic is 70% slower than the original. If the value of 'speed' is 1.3, the speed of audio file processed by sonic is 30% faster than the original.*/
} sonic_info_t;

/**
* @brief      Sonic configuration
*/
typedef struct {
    sonic_info_t sonic_info; /*!< Information of sonic*/
    int out_rb_size;         /*!< Size of output ring buffer */
    int task_stack;          /*!< Task stack size */
    int task_core;           /*!< Task running in core */
    int task_prio;           /*!< Task priority */
    bool stack_in_ext;       /*!< Try to allocate stack in external memory */
} sonic_cfg_t;

#define SONIC_SET_VALUE_FOR_INITIALIZATION (1.0f)
#define SONIC_TASK_STACK                   (4 * 1024)
#define SONIC_TASK_CORE                    (0)
#define SONIC_TASK_PRIO                    (5)
#define SONIC_RINGBUFFER_SIZE              (8 * 1024)

#define DEFAULT_SONIC_CONFIG()                             \
    {                                                      \
        .sonic_info = {                                    \
            .samplerate = 44100,                           \
            .channel = 1,                                  \
            .resample_linear_interpolate = 0,              \
            .pitch = SONIC_SET_VALUE_FOR_INITIALIZATION,   \
            .speed = SONIC_SET_VALUE_FOR_INITIALIZATION,   \
        },                                                 \
        .out_rb_size = SONIC_RINGBUFFER_SIZE,              \
        .task_stack = SONIC_TASK_STACK,                    \
        .task_core = SONIC_TASK_CORE,                      \
        .task_prio = SONIC_TASK_PRIO,                      \
        .stack_in_ext = true,                              \
    }

/**
* @brief      Sets the audio sample rate and the number of channels to be processed by the sonic.
*
* @param      self       Audio element handle
* @param      rate       The sample rate of stream data
* @param      ch         The number channels of stream data
*
* @return
*             ESP_OK
*             ESP_FAIL
*/
esp_err_t sonic_set_info(audio_element_handle_t self, int rate, int ch);

/**
* @brief      Sets the audio pitch and speed to be processed by the sonic.
*
* @param      self       Audio element handle
* @param      pitch      Scale factor of pitch of audio file. 0 means the original pitch. The range is [0.2 4.0].
* @param      speed      Scale factor of speed of audio file. 0 means the original speed. The range is [0.1 8.0].
*
* @return
*             ESP_OK
*             ESP_FAIL
*/
esp_err_t sonic_set_pitch_and_speed_info(audio_element_handle_t self, float pitch, float speed);

/**
* @brief      Creates an Audio Element handle for sonic
*
* @param      config      The sonic configuration
*
* @return     The sonic audio element handle
*/
audio_element_handle_t sonic_init(sonic_cfg_t *config);

#ifdef __cplusplus
}
#endif

#endif
