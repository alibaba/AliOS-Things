// Copyright 2018 Espressif Systems (Shanghai) PTE LTD
// All rights reserved.

#ifndef _FLAC_DECODER_H_
#define _FLAC_DECODER_H_

#include "esp_err.h"
#include "audio_element.h"
#include "audio_common.h"

#ifdef __cplusplus
extern "C" {
#endif

#define FLAC_DECODER_TASK_STACK_SIZE     (10 * 1024)
#define FLAC_DECODER_TASK_CORE           (0)
#define FLAC_DECODER_TASK_PRIO           (5)
#define FLAC_DECODER_RINGBUFFER_SIZE     (2 * 1024)

#define DEFAULT_FLAC_DECODER_CONFIG() {                 \
    .out_rb_size        = FLAC_DECODER_RINGBUFFER_SIZE, \
    .task_stack         = FLAC_DECODER_TASK_STACK_SIZE, \
    .task_core          = FLAC_DECODER_TASK_CORE,       \
    .task_prio          = FLAC_DECODER_TASK_PRIO,       \
    .stack_in_ext       = true,                         \
}

/**
 * @brief      FLAC Decoder configuration
 */
typedef struct {
    int                     out_rb_size;    /*!< Size of output ringbuffer */
    int                     task_stack;     /*!< Task stack size */
    int                     task_core;      /*!< CPU core number (0 or 1) where decoder task in running */
    int                     task_prio;      /*!< Task priority (based on freeRTOS priority) */
    bool                    stack_in_ext;   /*!< Try to allocate stack in external memory */
} flac_decoder_cfg_t;

/**
 * @brief      Create an Audio Element handle to decode incoming FLAC data
 *
 * @param      config  The configuration
 *
 * @return     The audio element handle
 */
audio_element_handle_t flac_decoder_init(flac_decoder_cfg_t *config);

#ifdef __cplusplus
}
#endif

#endif
