// Copyright 2018 Espressif Systems (Shanghai) PTE LTD
// All rights reserved.

#ifndef _WAV_ENCODER_H_
#define _WAV_ENCODER_H_

#include "esp_err.h"
#include "audio_element.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief      WAV Encoder configurations
 */
typedef struct {
    int                     out_rb_size;    /*!< Size of output ringbuffer */
    int                     task_stack;     /*!< Task stack size */
    int                     task_core;      /*!< Task running in core (0 or 1) */
    int                     task_prio;      /*!< Task priority (based on freeRTOS priority) */
    bool                    stack_in_ext;   /*!< Try to allocate stack in external memory */
} wav_encoder_cfg_t;

#define WAV_ENCODER_TASK_STACK          (3 * 1024)
#define WAV_ENCODER_TASK_CORE           (0)
#define WAV_ENCODER_TASK_PRIO           (5)
#define WAV_ENCODER_RINGBUFFER_SIZE     (8 * 1024)

#define DEFAULT_WAV_ENCODER_CONFIG() {\
    .out_rb_size        = WAV_ENCODER_RINGBUFFER_SIZE,\
    .task_stack         = WAV_ENCODER_TASK_STACK,\
    .task_core          = WAV_ENCODER_TASK_CORE,\
    .task_prio          = WAV_ENCODER_TASK_PRIO,\
    .stack_in_ext       = true,\
}

/**
 * @brief      Create a handle to an Audio Element to encode incoming data using WAV format
 *
 * @param      config  The configuration
 *
 * @return     The audio element handle
 */
audio_element_handle_t wav_encoder_init(wav_encoder_cfg_t *config);


#ifdef __cplusplus
}
#endif

#endif
