// Copyright 2018 Espressif Systems (Shanghai) PTE LTD
// All rights reserved.

#ifndef _AMRNB_ENCODER_H_
#define _AMRNB_ENCODER_H_

#include "esp_err.h"
#include "audio_element.h"
#include "audio_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief      Enum of AMRNB Encoder bitrate choose
 */
typedef enum {
    AMRNB_ENC_BITRATE_UNKNOW  = -1,   /*!< Invalid mode */
    AMRNB_ENC_BITRATE_MR475   = 0,    /* 4.75 kbps */
    AMRNB_ENC_BITRATE_MR515   = 1,    /* 5.15 kbps */
    AMRNB_ENC_BITRATE_MR59    = 2,    /* 5.90 kbps */
    AMRNB_ENC_BITRATE_MR67    = 3,    /* 6.70 kbps */
    AMRNB_ENC_BITRATE_MR74    = 4,    /* 7.40 kbps */
    AMRNB_ENC_BITRATE_MR795   = 5,    /* 7.95 kbps */
    AMRNB_ENC_BITRATE_MR102   = 6,    /* 10.2 kbps */
    AMRNB_ENC_BITRATE_MR122   = 7,    /* 12.2 kbps */
    AMRNB_ENC_BITRATE_MRDTX   = 8,    /* DTX       */
    AMRNB_ENC_BITRATE_N_MODES = 9,    /* Not Used  */
} amrnb_encoder_bitrate_t;

/**
 * @brief      AMRNB Encoder configurations
 */
typedef struct {
    int                       out_rb_size;            /*!< Size of output ringbuffer */
    int                       task_stack;             /*!< Task stack size */
    int                       task_core;              /*!< Task running in core (0 or 1) */
    int                       task_prio;              /*!< Task priority (based on freeRTOS priority) */
    amrnb_encoder_bitrate_t   bitrate_mode;           /*!< AMRNB Encoder bitrate choose */
    bool                      contain_amrnb_header;   /*!< Choose to contain amrnb header in amrnb encoder whether or not (true or false, true means choose to contain amrnb header) */
    bool                      stack_in_ext;           /*!< Try to allocate stack in external memory */
} amrnb_encoder_cfg_t;

#define AMRNB_ENCODER_TASK_STACK          (15 * 1024)
#define AMRNB_ENCODER_TASK_CORE           (0)
#define AMRNB_ENCODER_TASK_PRIO           (5)
#define AMRNB_ENCODER_RINGBUFFER_SIZE     (2 * 1024)
#define DEFAULT_AMRNB_ENCODER_CONFIG() {                       \
    .out_rb_size           = AMRNB_ENCODER_RINGBUFFER_SIZE,    \
    .task_stack            = AMRNB_ENCODER_TASK_STACK,         \
    .task_core             = AMRNB_ENCODER_TASK_CORE,          \
    .task_prio             = AMRNB_ENCODER_TASK_PRIO,          \
    .bitrate_mode          = AMRNB_ENC_BITRATE_MR122,          \
    .contain_amrnb_header  = false,                            \
    .stack_in_ext          = true,                             \
}

/**
 * @brief      Set AMRNB encoder bitrate
 *
 * @param      self          Audio element handle
 *
 * @param      bitrate_mode  Bitrate choose, value from amrnb_encoder_bitrate_t
 *
 * @return     
 *             ESP_OK
 *             ESP_FAIL
 */
esp_err_t amrnb_encoder_set_bitrate(audio_element_handle_t self, amrnb_encoder_bitrate_t bitrate_mode);

/**
 * @brief      Create an Audio Element handle to encode incoming AMRNB data
 *
 * @param      config  The configuration
 *
 * @return     The audio element handle
 */
audio_element_handle_t amrnb_encoder_init(amrnb_encoder_cfg_t *config);

#ifdef __cplusplus
}
#endif

#endif
