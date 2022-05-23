// Copyright 2018 Espressif Systems (Shanghai) PTE LTD
// All rights reserved.

#ifndef _AMRWB_ENCODER_H_
#define _AMRWB_ENCODER_H_

#include "esp_err.h"
#include "audio_element.h"
#include "audio_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief      Enum of AMRWB Encoder bitrate choose
 */
typedef enum {
    AMRWB_ENC_BITRATE_MDNONE      = -1,   /*!< Invalid mode */
    AMRWB_ENC_BITRATE_MD66        = 0,    /*!< 6.60kbps   */
    AMRWB_ENC_BITRATE_MD885       = 1,    /*!< 8.85kbps   */
    AMRWB_ENC_BITRATE_MD1265      = 2,    /*!< 12.65kbps  */
    AMRWB_ENC_BITRATE_MD1425      = 3,    /*!< 14.25kbps  */
    AMRWB_ENC_BITRATE_MD1585      = 4,    /*!< 15.85bps   */
    AMRWB_ENC_BITRATE_MD1825      = 5,    /*!< 18.25bps   */
    AMRWB_ENC_BITRATE_MD1985      = 6,    /*!< 19.85kbps  */
    AMRWB_ENC_BITRATE_MD2305      = 7,    /*!< 23.05kbps  */
    AMRWB_ENC_BITRATE_MD2385      = 8,    /*!< 23.85kbps> */
    AMRWB_ENC_BITRATE_N_MODES     = 9,    /*!< Invalid mode */    
} amrwb_encoder_bitrate_t;

/**
 * @brief     AMRWB Encoder configurations
 */
typedef struct {
    int                     out_rb_size;             /*!< Size of output ringbuffer */
    int                     task_stack;              /*!< Task stack size */
    int                     task_core;               /*!< Task running in core (0 or 1) */
    int                     task_prio;               /*!< Task priority (based on freeRTOS priority) */
    amrwb_encoder_bitrate_t bitrate_mode;            /*!< AMRWB Encoder bitrate choose */
    bool                    contain_amrwb_header;    /*!< Choose to contain amrwb header in amrwb encoder whether or not (true or false, true means choose to contain amrwb header) */
    bool                    stack_in_ext;            /*!< Try to allocate stack in external memory */
} amrwb_encoder_cfg_t;

#define AMRWB_ENCODER_TASK_STACK          (15 * 1024)
#define AMRWB_ENCODER_TASK_CORE           (0)
#define AMRWB_ENCODER_TASK_PRIO           (5)
#define AMRWB_ENCODER_RINGBUFFER_SIZE     (2 * 1024)
#define DEFAULT_AMRWB_ENCODER_CONFIG() {                         \
    .out_rb_size             = AMRWB_ENCODER_RINGBUFFER_SIZE,    \
    .task_stack              = AMRWB_ENCODER_TASK_STACK,         \
    .task_core               = AMRWB_ENCODER_TASK_CORE,          \
    .task_prio               = AMRWB_ENCODER_TASK_PRIO,          \
    .bitrate_mode            = AMRWB_ENC_BITRATE_MD885,          \
    .contain_amrwb_header    = false,                            \
    .stack_in_ext            = true,                             \
}

/**
 * @brief      Set AMRWB encoder bitrate
 *
 * @param      self          Audio element handle
 *
 * @param      bitrate_mode  Bitrate choose, value from amrwb_encoder_bitrate_t
 *
 * @return     
 *             ESP_OK
 *             ESP_FAIL
 */
esp_err_t amrwb_encoder_set_bitrate(audio_element_handle_t self, amrwb_encoder_bitrate_t bitrate_mode);

/**
 * @brief      Create an Audio Element handle to encode incoming amrwb data
 *
 * @param      config  The configuration
 *
 * @return     The audio element handle
 */
audio_element_handle_t amrwb_encoder_init(amrwb_encoder_cfg_t *config);

#ifdef __cplusplus
}
#endif

#endif
