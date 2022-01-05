// Copyright 2018 Espressif Systems (Shanghai) PTE LTD
// All rights reserved.

#ifndef _OGG_DECODER_H_
#define _OGG_DECODER_H_

#include "esp_err.h"
#include "audio_element.h"
#include "audio_common.h"

#ifdef __cplusplus
extern "C" {
#endif

#define OGG_DECODER_TASK_STACK_SIZE     (10 * 1024)
#define OGG_DECODER_TASK_CORE           (0)
#define OGG_DECODER_TASK_PRIO           (5)
#define OGG_DECODER_RINGBUFFER_SIZE     (2 * 1024)

#define DEFAULT_OGG_DECODER_CONFIG() {                  \
    .out_rb_size        = OGG_DECODER_RINGBUFFER_SIZE,  \
    .task_stack         = OGG_DECODER_TASK_STACK_SIZE,  \
    .task_core          = OGG_DECODER_TASK_CORE,        \
    .task_prio          = OGG_DECODER_TASK_PRIO,        \
    .stack_in_ext       = true,                         \
}

/**
 * @brief      OGG Decoder configuration
 */
typedef struct {
    int                     out_rb_size;    /*!< Size of output ringbuffer */
    int                     task_stack;     /*!< Task stack size */
    int                     task_core;      /*!< CPU core number (0 or 1) where decoder task in running */
    int                     task_prio;      /*!< Task priority (based on freeRTOS priority) */
    bool                    stack_in_ext;   /*!< Try to allocate stack in external memory */
} ogg_decoder_cfg_t;

/**
 * @brief      Create an Audio Element handle to decode incoming OGG data
 *
 * @param      config  The configuration
 *
 * @return     The audio element handle
 */
audio_element_handle_t ogg_decoder_init(ogg_decoder_cfg_t *config);

#ifdef __cplusplus
}
#endif

#endif
