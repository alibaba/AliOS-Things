// Copyright (c) 2019 <ESPRESSIF SYSTEMS (SHANGHAI) CO., LTD.>
// All rights reserved.

#ifndef _DOWNMIX_H_
#define _DOWNMIX_H_

#include "esp_err.h"
#include "audio_element.h"
#include "esp_downmix.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
* @brief      Downmix configuration
*/
typedef struct {
    esp_downmix_info_t downmix_info; /*!< Downmix information */
    int max_sample;                  /*!< The number of samples per downmix processing */
    int out_rb_size;                 /*!< Size of ring buffer */
    int task_stack;                  /*!< Size of task stack */
    int task_core;                   /*!< Task running in core... */
    int task_prio;                   /*!< Task priority (based on the FreeRTOS priority) */
    bool stack_in_ext;               /*!< Try to allocate stack in external memory */
} downmix_cfg_t;

#define DOWNMIX_TASK_STACK (8 * 1024)
#define DOWNMIX_TASK_CORE (0)
#define DOWNMIX_TASK_PRIO (5)
#define DOWNMIX_RINGBUFFER_SIZE (8 * 1024)
#define DM_BUF_SIZE (256)

#define DEFAULT_DOWNMIX_CONFIG()                                      \
    {                                                                 \
        .downmix_info = {                                             \
            .out_ctx = ESP_DOWNMIX_OUT_CTX_LEFT_RIGHT,                \
            .mode = ESP_DOWNMIX_WORK_MODE_BYPASS,                     \
            .output_type = ESP_DOWNMIX_OUTPUT_TYPE_ONE_CHANNEL,       \
            .source_num = SOURCE_NUM_MAX,                             \
        },                                                            \
        .max_sample = DM_BUF_SIZE,                                    \
        .out_rb_size = DOWNMIX_RINGBUFFER_SIZE,                       \
        .task_stack = DOWNMIX_TASK_STACK,                             \
        .task_core = DOWNMIX_TASK_CORE,                               \
        .task_prio = DOWNMIX_TASK_PRIO,                               \
        .stack_in_ext = true,                                         \
    }

/**
* @brief      Sets the downmix timeout.
*
* @param      self               audio element handle
* @param      ticks_to_wait      input ringbuffer timeout
* @param      index              The index of multi input ringbuffer.
*/
void downmix_set_input_rb_timeout(audio_element_handle_t self, int ticks_to_wait, int index);

/**
* @brief      Sets the downmix input ringbuffer. refer to `ringbuf.h`
*
* @param      self      audio element handle
* @param      rb        handle of ringbuffer
* @param      index     The index of multi input ringbuffer.
*/
void downmix_set_input_rb(audio_element_handle_t self, ringbuf_handle_t rb, int index);

/**
* @brief      Passes number of channels for output stream. Only supported mono and dual.
*
* @param      self         audio element handle
* @param      output_type  down-mixer output type.
*
* @return
*             ESP_OK
*             ESP_FAIL
*/
esp_err_t downmix_set_output_type(audio_element_handle_t self, esp_downmix_output_type_t output_type);

/**
* @brief      Sets BYPASS, ON or OFF status of down-mixer.
*
* @param      self       audio element handle
* @param      mode       down-mixer work mode.
*
* @return
*             ESP_OK
*             ESP_FAIL
*/
esp_err_t downmix_set_work_mode(audio_element_handle_t self, esp_downmix_work_mode_t mode);

/**
* @brief      Passes content of per channel output stream by down-mixer.
*
* @param      self       audio element handle
* @param      out_ctx    content of output stream.
*
* @return
*             ESP_OK
*             ESP_FAIL
*/
esp_err_t downmix_set_out_ctx_info(audio_element_handle_t self, esp_downmix_out_ctx_type_t out_ctx);

/**
* @brief      Sets the sample rate and the number of channels of input stream to be processed.
*
* @param      self      audio element handle
* @param      rate      sample rate of the input stream
* @param      ch        number of channel(s) of the input stream
* @param      index     The index of input stream. The index must be in [0, SOURCE_NUM_MAX - 1] range.
*
* @return
*             ESP_OK
*             ESP_FAIL
*/
esp_err_t downmix_set_source_stream_info(audio_element_handle_t self, int rate, int ch, int index);

/**
* @brief      Sets the audio gain to be processed.
*
* @param      self               audio element handle
* @param      gain               the reset value of `gain`. The `gain` is an array of two elements.
* @param      index              The index of input stream. The index must be in [0, SOURCE_NUM_MAX - 1] range.
* @return
*             ESP_OK
*             ESP_FAIL
*/
esp_err_t downmix_set_gain_info(audio_element_handle_t self, float *gain, int index);

/**
* @brief      Sets the audio `transit_time` to be processed.
*
* @param      self                  audio element handle
* @param      transit_time          the reset value of `transit_time`
* @param      index                 The index of input stream. The index must be in [0, SOURCE_NUM_MAX - 1] range
*
* @return
*             ESP_OK
*             ESP_FAIL
*/
esp_err_t downmix_set_transit_time_info(audio_element_handle_t self, int transit_time, int index);

/**
* @brief      Initializes information of the source streams for downmixing.
*
* @param      self          audio element handle
* @param      source_num    The information array of source streams
*
* @return
*             ESP_OK
*             ESP_FAIL
*/
esp_err_t source_info_init(audio_element_handle_t self, esp_downmix_input_info_t *source_num);

/**
* @brief      Initializes the Audio Element handle for downmixing.
*
* @param      config  the configuration
*
* @return     The initialized Audio Element handle
*/
audio_element_handle_t downmix_init(downmix_cfg_t *config);

#ifdef __cplusplus
}
#endif

#endif
