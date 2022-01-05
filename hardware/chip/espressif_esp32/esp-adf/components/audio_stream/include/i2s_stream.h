/*
 * ESPRESSIF MIT License
 *
 * Copyright (c) 2018 <ESPRESSIF SYSTEMS (SHANGHAI) PTE LTD>
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

#ifndef _I2S_STREAM_WRITER_H_
#define _I2S_STREAM_WRITER_H_

#include "driver/i2s.h"
#include "audio_common.h"
#include "audio_error.h"
#include "audio_idf_version.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief      I2S Stream configurations
 *             Default value will be used if any entry is zero
 */
typedef struct {
    audio_stream_type_t     type;               /*!< Type of stream */
    i2s_config_t            i2s_config;         /*!< I2S driver configurations */
    i2s_port_t              i2s_port;           /*!< I2S driver hardware port */
    bool                    use_alc;            /*!< It is a flag for ALC. If use ALC, the value is true. Or the value is false */
    int                     volume;             /*!< The volume of audio input data will be set. */
    int                     out_rb_size;        /*!< Size of output ringbuffer */
    int                     task_stack;         /*!< Task stack size */
    int                     task_core;          /*!< Task running in core (0 or 1) */
    int                     task_prio;          /*!< Task priority (based on freeRTOS priority) */
    bool                    stack_in_ext;       /*!< Try to allocate stack in external memory */
    int                     multi_out_num;      /*!< The number of multiple output */
    bool                    uninstall_drv;      /*!< whether uninstall the i2s driver when stream destroyed*/
} i2s_stream_cfg_t;

#define I2S_STREAM_TASK_STACK           (3072+512)
#define I2S_STREAM_BUF_SIZE             (2048)
#define I2S_STREAM_TASK_PRIO            (23)
#define I2S_STREAM_TASK_CORE            (0)
#define I2S_STREAM_RINGBUFFER_SIZE      (8 * 1024)

#if (ESP_IDF_VERSION < ESP_IDF_VERSION_VAL(4, 3, 0))
#define I2S_STREAM_CFG_DEFAULT() {                                              \
    .type = AUDIO_STREAM_WRITER,                                                \
    .i2s_config = {                                                             \
        .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX | I2S_MODE_RX),      \
        .sample_rate = 44100,                                                   \
        .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,                           \
        .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,                           \
        .communication_format = I2S_COMM_FORMAT_I2S,                            \
        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL2 | ESP_INTR_FLAG_IRAM,          \
        .dma_buf_count = 3,                                                     \
        .dma_buf_len = 300,                                                     \
        .use_apll = true,                                                       \
        .tx_desc_auto_clear = true,                                             \
        .fixed_mclk = 0                                                         \
    },                                                                          \
    .i2s_port = I2S_NUM_0,                                                      \
    .use_alc = false,                                                           \
    .volume = 0,                                                                \
    .out_rb_size = I2S_STREAM_RINGBUFFER_SIZE,                                  \
    .task_stack = I2S_STREAM_TASK_STACK,                                        \
    .task_core = I2S_STREAM_TASK_CORE,                                          \
    .task_prio = I2S_STREAM_TASK_PRIO,                                          \
    .stack_in_ext = false,                                                      \
    .multi_out_num = 0,                                                         \
    .uninstall_drv = true,                                                      \
}

#define I2S_STREAM_INTERNAL_DAC_CFG_DEFAULT() {                                 \
    .type = AUDIO_STREAM_WRITER,                                                \
    .i2s_config = {                                                             \
        .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_DAC_BUILT_IN | I2S_MODE_TX),\
        .sample_rate = 44100,                                                   \
        .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,                           \
        .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,                           \
        .communication_format = I2S_COMM_FORMAT_I2S_MSB,                        \
        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL2,                               \
        .dma_buf_count = 3,                                                     \
        .dma_buf_len = 300,                                                     \
        .use_apll = false,                                                      \
        .tx_desc_auto_clear = true,                                             \
        .fixed_mclk = 0                                                         \
    },                                                                          \
    .i2s_port = I2S_NUM_0,                                                      \
    .use_alc = false,                                                           \
    .volume = 0,                                                                \
    .out_rb_size = I2S_STREAM_RINGBUFFER_SIZE,                                  \
    .task_stack = I2S_STREAM_TASK_STACK,                                        \
    .task_core = I2S_STREAM_TASK_CORE,                                          \
    .task_prio = I2S_STREAM_TASK_PRIO,                                          \
    .stack_in_ext = false,                                                      \
    .multi_out_num = 0,                                                         \
    .uninstall_drv = false,                                                     \
}

#define I2S_STREAM_TX_PDM_CFG_DEFAULT() {                                       \
    .type = AUDIO_STREAM_WRITER,                                                \
    .i2s_config = {                                                             \
        .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_PDM | I2S_MODE_TX),     \
        .sample_rate = 44100,                                                   \
        .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,                           \
        .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,                           \
        .communication_format = I2S_COMM_FORMAT_I2S_MSB,                        \
        .dma_buf_count = 3,                                                     \
        .dma_buf_len = 300,                                                     \
        .use_apll = true,                                                       \
        .tx_desc_auto_clear = true,                                             \
        .fixed_mclk = 0                                                         \
    },                                                                          \
    .i2s_port = I2S_NUM_0,                                                      \
    .use_alc = false,                                                           \
    .volume = 0,                                                                \
    .out_rb_size = I2S_STREAM_RINGBUFFER_SIZE,                                  \
    .task_stack = I2S_STREAM_TASK_STACK,                                        \
    .task_core = I2S_STREAM_TASK_CORE,                                          \
    .task_prio = I2S_STREAM_TASK_PRIO,                                          \
    .stack_in_ext = false,                                                      \
    .multi_out_num = 0,                                                         \
    .uninstall_drv = false,                                                     \
}
#else
#define I2S_STREAM_CFG_DEFAULT() {                                              \
    .type = AUDIO_STREAM_WRITER,                                                \
    .i2s_config = {                                                             \
        .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX | I2S_MODE_RX),      \
        .sample_rate = 44100,                                                   \
        .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,                           \
        .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,                           \
        .communication_format = I2S_COMM_FORMAT_STAND_I2S,                      \
        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL2 | ESP_INTR_FLAG_IRAM,          \
        .dma_buf_count = 3,                                                     \
        .dma_buf_len = 300,                                                     \
        .use_apll = true,                                                       \
        .tx_desc_auto_clear = true,                                             \
        .fixed_mclk = 0                                                         \
    },                                                                          \
    .i2s_port = I2S_NUM_0,                                                      \
    .use_alc = false,                                                           \
    .volume = 0,                                                                \
    .out_rb_size = I2S_STREAM_RINGBUFFER_SIZE,                                  \
    .task_stack = I2S_STREAM_TASK_STACK,                                        \
    .task_core = I2S_STREAM_TASK_CORE,                                          \
    .task_prio = I2S_STREAM_TASK_PRIO,                                          \
    .stack_in_ext = false,                                                      \
    .multi_out_num = 0,                                                         \
    .uninstall_drv = true,                                                      \
}

#define I2S_STREAM_INTERNAL_DAC_CFG_DEFAULT() {                                 \
    .type = AUDIO_STREAM_WRITER,                                                \
    .i2s_config = {                                                             \
        .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_DAC_BUILT_IN | I2S_MODE_TX),\
        .sample_rate = 44100,                                                   \
        .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,                           \
        .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,                           \
        .communication_format = I2S_COMM_FORMAT_STAND_MSB,                        \
        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL2,                               \
        .dma_buf_count = 3,                                                     \
        .dma_buf_len = 300,                                                     \
        .use_apll = false,                                                      \
        .tx_desc_auto_clear = true,                                             \
        .fixed_mclk = 0                                                         \
    },                                                                          \
    .i2s_port = I2S_NUM_0,                                                      \
    .use_alc = false,                                                           \
    .volume = 0,                                                                \
    .out_rb_size = I2S_STREAM_RINGBUFFER_SIZE,                                  \
    .task_stack = I2S_STREAM_TASK_STACK,                                        \
    .task_core = I2S_STREAM_TASK_CORE,                                          \
    .task_prio = I2S_STREAM_TASK_PRIO,                                          \
    .stack_in_ext = false,                                                      \
    .multi_out_num = 0,                                                         \
    .uninstall_drv = false,                                                     \
}

#define I2S_STREAM_TX_PDM_CFG_DEFAULT() {                                       \
    .type = AUDIO_STREAM_WRITER,                                                \
    .i2s_config = {                                                             \
        .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_PDM | I2S_MODE_TX),     \
        .sample_rate = 44100,                                                   \
        .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,                           \
        .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,                           \
        .communication_format = I2S_COMM_FORMAT_STAND_MSB,                      \
        .dma_buf_count = 3,                                                     \
        .dma_buf_len = 300,                                                     \
        .use_apll = true,                                                       \
        .tx_desc_auto_clear = true,                                             \
        .fixed_mclk = 0                                                         \
    },                                                                          \
    .i2s_port = I2S_NUM_0,                                                      \
    .use_alc = false,                                                           \
    .volume = 0,                                                                \
    .out_rb_size = I2S_STREAM_RINGBUFFER_SIZE,                                  \
    .task_stack = I2S_STREAM_TASK_STACK,                                        \
    .task_core = I2S_STREAM_TASK_CORE,                                          \
    .task_prio = I2S_STREAM_TASK_PRIO,                                          \
    .stack_in_ext = false,                                                      \
    .multi_out_num = 0,                                                         \
    .uninstall_drv = false,                                                     \
}
#endif

/**
 * @brief      Create a handle to an Audio Element to stream data from I2S to another Element
 *             or get data from other elements sent to I2S, depending on the configuration of stream type
 *             is AUDIO_STREAM_READER or AUDIO_STREAM_WRITER.
 * @note       If I2S stream is enabled with built-in DAC mode, please don't use I2S_NUM_1. The built-in
 *             DAC functions are only supported on I2S0 for the current ESP32 chip.
 * @param      config  The configuration
 *
 * @return     The Audio Element handle
 */
audio_element_handle_t i2s_stream_init(i2s_stream_cfg_t *config);

/**
 * @brief      Setup clock for I2S Stream, this function is only used with handle created by `i2s_stream_init`
 *
 * @param[in]  i2s_stream   The i2s element handle
 * @param[in]  rate  Clock rate (in Hz)
 * @param[in]  bits  Audio bit width (8, 16, 24, 32)
 * @param[in]  ch    Number of Audio channels (1: Mono, 2: Stereo)
 *
 * @return
 *     - ESP_OK
 *     - ESP_FAIL
 */
esp_err_t i2s_stream_set_clk(audio_element_handle_t i2s_stream, int rate, int bits, int ch);

/**
 * @brief      Setup volume of stream by using ALC
 *
 * @param[in]  i2s_stream   The i2s element handle
 * @param[in]  volume       The volume of stream will be set.
 *
 * @return
 *     - ESP_OK
 *     - ESP_FAIL
 */
esp_err_t i2s_alc_volume_set(audio_element_handle_t i2s_stream, int volume);

/**
 * @brief      Get volume of stream
 *
 * @param[in]  i2s_stream   The i2s element handle
 * @param[in]  volume       The volume of stream
 *
 * @return
 *     - ESP_OK
 *     - ESP_FAIL
 */
esp_err_t i2s_alc_volume_get(audio_element_handle_t i2s_stream, int *volume);

#ifdef __cplusplus
}
#endif

#endif
