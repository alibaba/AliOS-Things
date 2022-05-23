// Copyright 2018 Espressif Systems (Shanghai) PTE LTD
// All rights reserved.

#ifndef _RSP_FILTER_H_
#define _RSP_FILTER_H_

#include "esp_err.h"
#include "audio_element.h"
#include "esp_resample.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief      Resample Filter Configuration
 */
typedef struct {
    int src_rate;              /*!< The sampling rate of the source PCM file (in Hz)*/
    int src_ch;                /*!< The number of channel(s) of the source PCM file (Mono=1, Dual=2) */
    int dest_rate;             /*!< The sampling rate of the destination PCM file (in Hz) */
    int dest_ch;               /*!< The number of channel(s) of the destination PCM file (Mono=1, Dual=2) */
    int sample_bits;           /*!< The bit width of the PCM file. Currently, the only supported bit width is 16 bits. */
    esp_resample_mode_t mode;  /*!< The resampling mode (the encoding mode or the decoding mode). For decoding mode, input PCM length is constant; for encoding mode, output PCM length is constant. */
    int max_indata_bytes;      /*!< The maximum buffer size of the input PCM (in bytes) */
    int out_len_bytes;         /*!< The buffer length of the output stream data. This parameter must be configured in encoding mode. */
    esp_resample_type_t type;  /*!< The resampling type (Automatic, Upsampling and Downsampling) */
    int complexity;            /*!< Indicates the complexity of the resampling. This parameter is only valid when a FIR filter is used. Range: 0~5; O indicates the lowest complexity, which means the accuracy is the lowest and the speed is the fastest; Meanwhile, 5 indicates the highest complexity, which means the accuracy is the highest and the speed is the slowest.If user set `complexity` less than 0,  `complexity` can be set 0. If user set `complexity` more than 5,  `complexity` can be set 5. */
    int down_ch_idx;           /*!< Indicates the channel that is selected (the right channel or the left channel). This parameter is only valid when the complexity parameter is set to 0 and the number of channel(s) of the input file has changed from dual to mono. */
    esp_rsp_prefer_type_t prefer_flag; /*!< The select flag about lesser CPU usage or lower INRAM usage, refer to esp_resample.h */
    int out_rb_size;           /*!< Output ringbuffer size*/
    int task_stack;            /*!< Task stack size */
    int task_core;             /*!< Task running on core */
    int task_prio;             /*!< Task priority */
    bool stack_in_ext;         /*!< Try to allocate stack in external memory */
} rsp_filter_cfg_t;

#define RSP_FILTER_BUFFER_BYTE              (512)
#define RSP_FILTER_TASK_STACK               (4 * 1024)
#define RSP_FILTER_TASK_CORE                (0)
#define RSP_FILTER_TASK_PRIO                (5)
#define RSP_FILTER_RINGBUFFER_SIZE          (2 * 1024)

#define DEFAULT_RESAMPLE_FILTER_CONFIG() {          \
        .src_rate = 44100,                          \
        .src_ch = 2,                                \
        .dest_rate = 48000,                         \
        .dest_ch = 2,                               \
        .sample_bits = 16,                          \
        .mode = RESAMPLE_DECODE_MODE,               \
        .max_indata_bytes = RSP_FILTER_BUFFER_BYTE, \
        .out_len_bytes = RSP_FILTER_BUFFER_BYTE,    \
        .type = ESP_RESAMPLE_TYPE_AUTO,             \
        .complexity = 2,                            \
        .down_ch_idx = 0,                           \
        .prefer_flag = ESP_RSP_PREFER_TYPE_SPEED,   \
        .out_rb_size = RSP_FILTER_RINGBUFFER_SIZE,  \
        .task_stack = RSP_FILTER_TASK_STACK,        \
        .task_core = RSP_FILTER_TASK_CORE,          \
        .task_prio = RSP_FILTER_TASK_PRIO,          \
        .stack_in_ext = true,                       \
    }

/**
 * @brief      Set the source audio sample rate and the number of channels to be processed by the resample.
 *
 * @param      self       Audio element handle
 * @param      src_rate   The sample rate of stream data
 * @param      src_ch     The number channels of stream data
 *
 * @return
 *             ESP_OK
 *             ESP_FAIL
 */
esp_err_t rsp_filter_set_src_info(audio_element_handle_t self, int src_rate, int src_ch);

/**
 * @brief      Create an Audio Element handle to resample incoming data.
 *
 * Depending on configuration, there are upsampling, downsampling, as well as
 * converting data between mono and dual.
 *
 *   - If the esp_resample_mode_t is `RESAMPLE_DECODE_MODE`, `src_rate` and `src_ch` will be fetched from `audio_element_getinfo`.
 *   - If the esp_resample_mode_t is `RESAMPLE_ENCODE_MODE`, `src_rate`, `src_ch`, `dest_rate` and `dest_ch` must be configured.
 *
 * @param      config     The configuration
 *
 * @return     The audio element handler
 */
audio_element_handle_t rsp_filter_init(rsp_filter_cfg_t *config);

#ifdef __cplusplus
}
#endif

#endif
