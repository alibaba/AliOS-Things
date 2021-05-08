/*
 * Copyright (C) 2017-2019 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     drv_codec.h
 * @brief    head file for codec
 * @version  V1.0
 * @date     12. April 2019
 * @model    codec
 ******************************************************************************/
#ifndef _DRV_CODEC_H_
#define _DRV_CODEC_H_

#include <drv/common.h>
#include "stdint.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    CODEC_SAMPLE_RATE_8000              = 8000,
    CODEC_SAMPLE_RATE_11025             = 11025,
    CODEC_SAMPLE_RATE_12000             = 12000,
    CODEC_SAMPLE_RATE_16000             = 16000,
    CODEC_SAMPLE_RATE_22050             = 22050,
    CODEC_SAMPLE_RATE_24000             = 24000,
    CODEC_SAMPLE_RATE_32000             = 32000,
    CODEC_SAMPLE_RATE_44100             = 44100,
    CODEC_SAMPLE_RATE_48000             = 48000,
    CODEC_SAMPLE_RATE_96000             = 96000,
} codec_sample_rate_t;

/****** CODEC Event *****/
typedef enum {
    CODEC_EVENT_PERIOD_READ_COMPLETE        = 0,  ///< a peroid data read complete
    CODEC_EVENT_PERIOD_WRITE_COMPLETE       = 1,  ///< a peroid data write complete
    CODEC_EVENT_WRITE_BUFFER_EMPTY          = 2,  ///< fifo is empty
    CODEC_EVENT_READ_BUFFER_FULL            = 3,  ///< fifo is full
    CODEC_EVENT_VAD_TRIGGER                 = 4,  ///< vad is trigger 
    CODEC_EVENT_TRANSFER_ERROR              = 5,  ///< transfer error
} codec_event_t;

typedef enum {
    CODEC_MODE_RUN                  = 0,   ///< Running mode
    CODEC_MODE_SLEEP,                      ///< Sleep mode
    CODEC_MODE_SHUTDOWN                    ///< Shutdown mode
} codec_lpm_state_t;

typedef void (*codec_event_cb_t)(int idx, codec_event_t event, void *arg);

typedef void *codec_handle_t;

typedef struct {
    codec_sample_rate_t sample_rate;
    uint32_t channel_num;       ///< record channel num
    uint32_t bit_width;
} codec_input_config_t;

typedef struct {
    codec_sample_rate_t sample_rate;
    uint32_t bit_width;
    uint32_t mono_mode_en;
} codec_output_config_t;

typedef struct {
    int32_t codec_idx;
    int32_t ch_idx;        ///< input channle number
    codec_event_cb_t cb;
    void *cb_arg;
    uint8_t *buf;
    uint32_t buf_size;     ///< Used to cache incoming data
    uint32_t period;       ///< when a peroid data is reached,the callback function is called
    uint32_t vad_threshold;
    void *priv;
} codec_input_t;

typedef struct {
    int32_t codec_idx;
    int32_t ch_idx;
    codec_event_cb_t cb;
    void *cb_arg;
    uint8_t *buf;           ///< Used to cache incoming data
    uint32_t buf_size;
    uint32_t period;        ///< when a peroid data is reached,the callback function is called
    void *priv;
} codec_output_t;

/**
  \brief  Init the CODEC according to the specified
  \param[in]   idx codec index
  \return 0 for success, negative for error code
*/
int32_t csi_codec_init(uint32_t idx);

/**
  \brief  Uninit the CODEC
  \param[in]   idx codec index
  \return none
*/
void csi_codec_uninit(uint32_t idx);
void csi_codec_lpm(uint32_t idx, codec_lpm_state_t state);

/**
  \brief  Control codec power
  \param[in]   idx codec index
  \param[in]   state power ctrl status
  \return 0 for success, negative for error code
*/
int32_t csi_codec_power_control(int32_t idx, csi_power_stat_e state);

/**
  \brief  Open the codec input channel
  \param[in]   handle: codec input handle
  \return 0 for success, negative for error code
*/
int32_t csi_codec_input_open(codec_input_t *handle);

/**
  \brief  Config the codec input channel
  \param[in]   handle: codec input handle
  \param[in]   config: codec input config param
  \return 0 for success, negative for error code
*/
int32_t csi_codec_input_config(codec_input_t *handle, codec_input_config_t *config);

/**
  \brief  Close codec input channel
  \param[in]   handle: codec input handle
  \return 0 for success, negative for error code
*/
int32_t csi_codec_input_close(codec_input_t *handle);

/**
  \brief  Read the record data
  \param[in]   handle: codec input handle
  \param[in]   buf: used to store read data
  \param[in]   length: read data length
  \return read data len, if the data is empty return 0;
*/
uint32_t csi_codec_input_read(codec_input_t *handle, uint8_t *buf, uint32_t length);

/**
  \brief  Get buffer free space
  \param[in]   handle: codec input handle
  \return buffer free space (bytes)
*/
uint32_t csi_codec_input_buf_avail(codec_input_t *handle);

/**
  \brief  Reset the buf, discard all data in the cache
  \param[in]   handle: codec input handle
  \return 0 for success, negative for error code
*/
int32_t csi_codec_input_buf_reset(codec_input_t *handle);

/**
  \brief  Start the input channel.
  \param[in]   handle: codec input handle
  \return 0 for success, negative for error code
*/
int32_t csi_codec_input_start(codec_input_t *handle);

/**
  \brief  Stop the input channel and reset the input cache.
  \param[in]   handle: codec input handle
  \return 0 for success, negative for error code
*/
int32_t csi_codec_input_stop(codec_input_t *handle);

/**
  \brief  Pause the input channel, channel will be pause receive data.
  \param[in]   handle: codec input handle
  \return 0 for success, negative for error code
*/
int32_t csi_codec_input_pause(codec_input_t *handle);

/**
  \brief  Resume the input channel
  \param[in]   handle: codec input handle
  \return 0 for success, negative for error code
*/
int32_t csi_codec_input_resume(codec_input_t *handle);

/**
  \brief  Set input digital gain
  \param[in]   handle: codec input handle
  \param[in]   val: gain value
  \return 0 for success, negative for error code
*/
int32_t csi_codec_input_set_digital_gain(codec_input_t *handle, int32_t val);

/**
  \brief  Set input analog gain
  \param[in]   handle: codec input handle
  \param[in]   val: gain value
  \return 0 for success, negative for error code
*/
int32_t csi_codec_input_set_analog_gain(codec_input_t *handle, int32_t val);

/**
  \brief  Get input digital gain
  \param[in]   handle: codec input handle
  \param[out]  val: pointer, that used save gain val
  \return 0 for success, negative for error code
*/
int32_t csi_codec_input_get_digital_gain(codec_input_t *handle, int32_t *val);

/**
  \brief  Get input analog gain
  \param[in]   handle: codec input handle
  \param[out]  val: pointer, that used save gain val
  \return 0 for success, negative for error code
*/
int32_t csi_codec_input_get_analog_gain(codec_input_t *handle, int32_t *val);

/**
  \brief  Set input mixer gain
  \param[in]   handle: codec input handle
  \param[in]  val: gain val
  \return 0 for success, negative for error code
*/
int32_t csi_codec_input_set_mixer_gain(codec_input_t *handle, int32_t val);

/**
  \brief  Get input mixer gain
  \param[in]   handle: codec input handle
  \param[out]  val: pointer, that used save gain val
  \return 0 for success, negative for error code
*/
int32_t csi_codec_input_get_mixer_gain(codec_input_t *handle, int32_t *val);

/**
  \brief  Contol input mute
  \param[in]  handle: codec input handle
  \param[in]  en: 1 enable mute, 0 disable meute
  \return 0 for success, negative for error code
*/
int32_t csi_codec_input_mute(codec_input_t *handle, int en);

/**
  \brief  Open the codec output channel
  \param[in]   handle: codec output handle
  \return 0 for success, negative for error code
*/
int32_t csi_codec_output_open(codec_output_t *handle);

/**
  \brief  Close the codec output channel
  \param[in]   handle: codec output handle
  \return 0 for success, negative for error code
*/
int32_t csi_codec_output_close(codec_output_t *handle);

/**
  \brief  Close the codec output channel
  \param[in]   handle: codec output handle
  \param[in]   config: channel config param
  \return 0 for success, negative for error code
*/
int32_t csi_codec_output_config(codec_output_t *handle, codec_output_config_t *config);

/**
  \brief  Close the codec output channel
  \param[in]   handle: codec output handle
  \param[in]   buf: write data
  \param[in]   lenght: write data length
  \return The number of data written to the cache
*/
uint32_t csi_codec_output_write(codec_output_t *handle, uint8_t *buf, uint32_t length);

/**
  \brief  Get buffer free space
  \param[in]  handle: codec output handle
  \return buffer free space (bytes)
*/
uint32_t csi_codec_output_buf_avail(codec_output_t *handle);

/**
  \brief  Reset the buf, discard all data in the cache
  \param[in]   handle: codec input handle
  \return 0 for success, negative for error code
*/
int32_t csi_codec_output_buf_reset(codec_output_t *handle);

/**
  \brief  Start the output channel.
  \param[in]   handle: codec output handle
  \return 0 for success, negative for error code
*/
int32_t csi_codec_output_start(codec_output_t *handle);

/**
  \brief  Stop the output channel and reset the output cache.
  \param[in]   handle: codec output handle
  \return 0 for success, negative for error code
*/
int32_t csi_codec_output_stop(codec_output_t *handle);

/**
  \brief  Pause the ouput channel, channel will be pause send data.
  \param[in]   handle: codec input handle
  \return 0 for success, negative for error code
*/
int32_t csi_codec_output_pause(codec_output_t *handle);

/**
  \brief  Resume the ouput channel.
  \param[in]   handle: codec input handle
  \return 0 for success, negative for error code
*/
int32_t csi_codec_output_resume(codec_output_t *handle);

/**
  \brief  Set output left channel digital gain
  \param[in]   handle: codec output handle
  \param[in]   val: gain value
  \return 0 for success, negative for error code
*/
int32_t csi_codec_output_set_digital_left_gain(codec_output_t *handle, int32_t val);

/**
  \brief  Set output right channel digital gain
  \param[in]   handle: codec output handle
  \param[in]   val: gain value
  \return 0 for success, negative for error code
*/
int32_t csi_codec_output_set_digital_right_gain(codec_output_t *handle, int32_t val);

/**
  \brief  Set output left channel analog gain
  \param[in]   handle: codec output handle
  \param[in]   val: gain value
  \return 0 for success, negative for error code
*/
int32_t csi_codec_output_set_analog_left_gain(codec_output_t *handle, int32_t val);

/**
  \brief  Set output right channel analog gain
  \param[in]   handle: codec output handle
  \param[in]   val: gain value
  \return 0 for success, negative for error code
*/
int32_t csi_codec_output_set_analog_right_gain(codec_output_t *handle, int32_t val);

/**
  \brief  Get output left channel digital gain
  \param[in]   handle: codec output handle
  \param[in]   val: pointer, that used save gain val
  \return 0 for success, negative for error code
*/
int32_t csi_codec_output_get_digital_left_gain(codec_output_t *handle, int32_t *val);

/**
  \brief  Get output right channel digital gain
  \param[in]   handle: codec output handle
  \param[in]   val: pointer, that used save gain val
  \return 0 for success, negative for error code
*/
int32_t csi_codec_output_get_digital_right_gain(codec_output_t *handle, int32_t *val);

/**
  \brief  Get output left channel analog gain
  \param[in]   handle: codec output handle
  \param[in]   val: pointer, that used save gain val
  \return 0 for success, negative for error code
*/
int32_t csi_codec_output_get_analog_left_gain(codec_output_t *handle, int32_t *val);

/**
  \brief  Get output right channel analog gain
  \param[in]   handle: codec output handle
  \param[in]   val: pointer, that used save gain val
  \return 0 for success, negative for error code
*/
int32_t csi_codec_output_get_analog_right_gain(codec_output_t *handle, int32_t *val);

/**
  \brief  Set output left channel mixer gain
  \param[in]   handle: codec output handle
  \param[in]   val: gain value
  \return 0 for success, negative for error code
*/
int32_t csi_codec_output_set_mixer_left_gain(codec_output_t *handle, int32_t val);

/**
  \brief  Set output right channel mixer gain
  \param[in]   handle: codec output handle
  \param[in]   val: gain value
  \return 0 for success, negative for error code
*/
int32_t csi_codec_output_set_mixer_right_gain(codec_output_t *handle, int32_t val);

/**
  \brief  Get output left channel mixer gain
  \param[in]   handle: codec output handle
  \param[in]   val: pointer, that used save gain val
  \return 0 for success, negative for error code
*/
int32_t csi_codec_output_get_mixer_left_gain(codec_output_t *handle, int32_t *val);

/**
  \brief  Get output right channel mixer gain
  \param[in]   handle: codec output handle
  \param[in]   val: pointer, that used save gain val
  \return 0 for success, negative for error code
*/
int32_t csi_codec_output_get_mixer_right_gain(codec_output_t *handle, int32_t *val);

/**
  \brief  Contol output channel mute
  \param[in]  handle: codec output handle
  \param[in]  en: 1 enable mute, 0 disable meute
  \return 0 for success, negative for error code
*/
int32_t csi_codec_output_mute(codec_output_t *handle, int en);

/**
  \brief  Contol codec wakeup
  \param[in]  handle: codec input handle
  \param[in]  en: 1 enable wakeup, 0 disable wakeup
  \return 0 for success, negative for error code
*/
int32_t csi_codec_vad_enable(codec_input_t *handle, int en);

#ifdef __cplusplus
}
#endif

#endif /* _DRV_CODEC_H_  */

