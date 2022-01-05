/*
 * ESPRESSIF MIT License
 *
 * Copyright (c) 2019 <ESPRESSIF SYSTEMS (SHANGHAI) CO., LTD.>
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

#ifndef _AUDI_FORGE_WRITER_H_
#define _AUDI_FORGE_WRITER_H_

#include "audio_error.h"
#include "esp_alc.h"
#include "esp_downmix.h"
#include "esp_equalizer.h"
#include "esp_resample.h"
#include "esp_sonic.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 *
 * The module is multifunctional audio forge which contains  resample, downmix, ALC, equalizer and sonic. Choose a combination of several function by `component_select`.
 *
 *  resample: Change sample rate or number of channels for source stream.
 *  downmix: Down-mix different source streams or stereo source stream.
 *  ALC: Change volume of source file.
 *  equalizer: Modify a frequency response to compensate for distortion.
 *  sonic: Change the speed and pitch of source file.
 *
*/

extern int audio_forge_set_gain_value[];  /*!< the paramenter is for equalizer */

/**
 * @brief      Information of source files
 */
typedef struct {
        int samplerate;          /*!< Sample rate */
        int channel;             /*!< Number of channel(s)*/
        int bit_num;             /*!< Only 16-bit PCM audio is supported */
} audio_forge_src_info_t;

/**
 * @brief      Information of downmix processing
 */
typedef struct {
        float gain[2];          /*!< The gain processed by downmix. Refer to `downmix_input_info_t` */
        int transit_time;       /*!< The transit time processed by downmix. Refer to `downmix_input_info_t` */
} audio_forge_downmix_t;

/**
 * @brief      The select of audio forge component.
 *               eg. To use resample and downmix together, please enter AUDIO_FORGE_SELECT_RESAMPLE | AUDIO_FORGE_SELECT_DOWNMIX.
 */

typedef enum {
    AUDIO_FORGE_SELECT_RESAMPLE  = 0x01,     /*!< Resample selected */
    AUDIO_FORGE_SELECT_DOWNMIX   = 0x02,     /*!< Downmix selected */
    AUDIO_FORGE_SELECT_ALC       = 0x04,     /*!< ALC selected */
    AUDIO_FORGE_SELECT_EQUALIZER = 0x08,     /*!< Equalizer selected */
    AUDIO_FORGE_SELECT_SONIC     = 0x10,     /*!< Sonic selected */
} audio_forge_select_t;

/**
 * @brief    Closed mode of the audio forge
 */
typedef enum {
    AUDIO_FORGE_STOP_MODE_AUTO = 1,       /*!< When all audios are closed, the element will free resource. */
    AUDIO_FORGE_STOP_MODE_MANUAL = 2,     /*!< First use `audio_set_stop_mode` for setting stop mode to not AUDIO_FORGE_STOP_MODE_MANUAL. Then call `audio_forge_close` to free resource */
    AUDIO_FORGE_STOP_MODE_INVALID = 0,
} audio_forge_stop_mode_t;

/**
 * @brief      Information of audio forge
 */
typedef struct {
    int dest_samplerate;                                        /*!< The sample rate of destination files. If resample is selected, this value must be set */
    int dest_channel;                                           /*!< The channel of destination files, If resample and downmix are selected, the value must be set  */
    int rsp_cplx;                                               /*!< complexity level of resample. refer to `esp_resample.h`*/
    int alc_volume;                                             /*!< The volume processed by ALC */
    float sonic_pitch;                                          /*!< The pitch processed by sonic */
    float sonic_speed;                                          /*!< The speed processed by sonic */
    int *equalizer_gain;                                        /*!< The gain processed by equalizer */
    audio_forge_select_t component_select;                      /*!< The select of audio forge component */
    int max_sample;                                             /*!< Number of sample per audio forge processing */
    int source_num;                                             /*!< Number of source streams */
    audio_forge_stop_mode_t stop_mode;
} audio_forge_info_t;

/**
 * @brief      audio forge configurations
 */
typedef struct {
    audio_forge_info_t      audio_forge;        /*!< Information of audio forge */
    int                     out_rb_size;        /*!< Size of output ringbuffer */
    int                     task_stack;         /*!< Task stack size */
    int                     task_core;          /*!< Task running in core (0 or 1) */
    int                     task_prio;          /*!< Task priority (based on freeRTOS priority) */
    bool                    stack_in_ext;       /*!< Try to allocate stack in external memory */
} audio_forge_cfg_t;

#define AUDIO_FORGE_TASK_STACK           (4096)
#define AUDIO_FORGE_BUF_SIZE             (2048)
#define AUDIO_FORGE_TASK_PRIO            (5)
#define AUDIO_FORGE_TASK_CORE            (1)
#define AUDIO_FORGE_RINGBUFFER_SIZE      (8 * 1024)
#define AUDIO_FORGE_SAMPLE_SIZE          (256)


#define AUDIO_FORGE_CFG_DEFAULT() {                                        \
    .audio_forge = {                                                       \
        .dest_samplerate = 48000,                                          \
        .dest_channel = 2,                                                 \
        .rsp_cplx = 0,                                                     \
        .alc_volume = 0,                                                   \
        .sonic_pitch = 1.0,                                                \
        .sonic_speed = 1.0,                                                \
        .equalizer_gain = NULL,                      \
        .component_select = AUDIO_FORGE_SELECT_RESAMPLE,                   \
        .max_sample = AUDIO_FORGE_SAMPLE_SIZE,                             \
        .source_num = 2,                                                   \
        .stop_mode = AUDIO_FORGE_STOP_MODE_AUTO,                           \
    },                                                                     \
    .task_prio = AUDIO_FORGE_TASK_PRIO,                                    \
    .task_core = AUDIO_FORGE_TASK_CORE,                                    \
    .task_stack = AUDIO_FORGE_TASK_STACK,                                  \
    .out_rb_size = AUDIO_FORGE_RINGBUFFER_SIZE,                            \
    .stack_in_ext = true,                                                  \
}

/**
* @brief      Switch on the multi-audio downmix.
*
* @param      self          audio element handle
*/
void audio_forge_downmix_start(audio_element_handle_t self);

/**
* @brief      Switch off the multi-audio downmix.
*
* @param      self          audio element handle
*/
void audio_forge_downmix_stop(audio_element_handle_t self);

/**
* @brief      Set audio forge stop mode.
*
* @param      self          audio element handle
*/
void audio_forge_set_stop_mode(audio_element_handle_t self, audio_forge_stop_mode_t stop_mode);

/**
* @brief      Set the audio forge timeout.
*
* @param      self          audio element handle
* @param      tick_to_wait  input ringbuffer timeout
* @param      index         the index of multi input ringbuffer. Refer to `ringbuf.h`
*/
void audio_forge_downmix_set_input_rb_timeout(audio_element_handle_t self, int tick_to_wait, int index);

/**
* @brief      Set the audio forge input ringbuffer.
*
* @param      self          audio element handle
* @param      rb            handle of ringbuffer
* @param      index         the index of multi input ringbuffer. Refer to `ringbuf.h`
*/
void audio_forge_downmix_set_input_rb(audio_element_handle_t self, ringbuf_handle_t rb, int index);

/**
* @brief      Set the audio `transform_time` to be processed by the downmix.
*
* @param      self          audio element handle
* @param      transit_time  reset value of transform_time defined in audio_forge_downmix_t. The `transit_time` is an array of two elements.
* @param      index         the index of audio streams, start from `0`.
*
* @return
*             ESP_OK
*             ESP_FAIL
*/
esp_err_t audio_forge_downmix_set_transit_time(audio_element_handle_t self, int transit_time, int index);

/**
* @brief      Set the audio gain to be processed by the downmix.
*
* @param      self          audio element handle
* @param      gain          the reset value of `gain` which in `audio_forge_downmix_t`. The `gain` is an array of four elements.
* @param      index         the index of audio streams, start from `0`.
*
* @return
*             ESP_OK
*             ESP_FAIL
*/
esp_err_t audio_forge_downmix_set_gain(audio_element_handle_t self, int *gain, int index);

/**
 * @brief     Setup volume of stream by the ALC
 *
 * @param     self           audio element handle
 * @param     volume         the volume of stream will be set.
 *
 * @return
 *            ESP_OK
 *            ESP_FAIL
 */
esp_err_t audio_forge_alc_set_volume(audio_element_handle_t self, int volume);

/**
 * @brief      Get volume of stream
 *
 * @param      self          audio element handle
 * @param      volume        the volume of stream
 *
 * @return
 *             ESP_OK
 *             ESP_FAIL
 */
esp_err_t audio_forge_alc_get_volume(audio_element_handle_t self, int *volume);

/**
 * @brief      Set the audio gain to be processed by the equalizer.
 *
 * @param      self          audio element handle
 * @param      eq_gain       the value of audio gain which in `index`
 * @param      band_index    the position of center frequencies of equalizer
 *
 * @return
 *             ESP_OK
 *             ESP_FAIL
 */
esp_err_t audio_forge_eq_set_gain(audio_element_handle_t self, int eq_gain, int band_index);

/**
 * @brief      Set the component select.
 *
 * @param      self                 audio element handle
 * @param      component_select     The select of audio forge component. eg. To use resample and downmix together, please enter AUDIO_FORGE_SELECT_RESAMPLE | AUDIO_FORGE_SELECT_DOWNMIX.
 *
 * @return
 *             ESP_OK
 *             ESP_FAIL
 */
esp_err_t audio_forge_sonic_set_component_select(audio_element_handle_t self, int component_select);

/**
* @brief      Set the audio pitch to be processed by the sonic.
*
* @param      self          audio element handle
* @param      sonic_pitch   scale factor of pitch of audio stream. 0 means the original pitch. The range is [0.2 4.0].
*
* @return
*             ESP_OK
*             ESP_FAIL
*/
esp_err_t audio_forge_sonic_set_pitch(audio_element_handle_t self, float sonic_pitch);

/**
* @brief      Get the audio speed to be processed by the sonic.
*
* @param      self          audio element handle
* @param      sonic_speed   scale factor of speed of audio stream. 1.0 means the original speed. The range is [0.25 2.0].
*
* @return
*             ESP_OK
*             ESP_FAIL
*/
esp_err_t audio_forge_sonic_get_speed(audio_element_handle_t self, float *sonic_speed);

/**
* @brief      Set the audio speed to be processed by the sonic.
*
* @param      self          audio element handle
* @param      sonic_speed   scale factor of speed of audio stream. 0 means the original speed. The range is [0.25 2.0].
*
* @return
*             ESP_OK
*             ESP_FAIL
*/
esp_err_t audio_forge_sonic_set_speed(audio_element_handle_t self, float sonic_speed);

/**
* @brief      Set the sample rate and the number of channels of input file to be processed .
*
* @param      self          audio element handle
* @param      samplerate    sample rate of the source audio stream
* @param      channel       number of channel(s) of the audio stream
* @param      index         the index of audio streams, start from `0`.
*
* @return
*             ESP_OK
*             ESP_FAIL
*/
esp_err_t audio_forge_set_src_info(audio_element_handle_t self, audio_forge_src_info_t source_num, int index);

/**
* @brief      Initialize information of the source files for audio forge.
*
* @param      self          audio element handle
* @param      source_num    the information array of source files
* @param      downmix_info  it is used in downmix. Refer to `audio_forge_downmix_t`
*
* @return     ESP_OK
*             ESP_FAIL
*/
esp_err_t audio_forge_source_info_init(audio_element_handle_t self, audio_forge_src_info_t *source_num, audio_forge_downmix_t *downmix_info);

/**
 * @brief      Create a handle to an audio forge to processing the audio stream
 *
 * @param      config       the configuration
 *
 * @return     the Audio Element handle
 */
audio_element_handle_t audio_forge_init(audio_forge_cfg_t *config);

#ifdef __cplusplus
}
#endif

#endif
