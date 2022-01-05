/*
 * ESPRESSIF MIT License
 *
 * Copyright (c) 2020 <ESPRESSIF SYSTEMS (SHANGHAI) CO., LTD.>
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

#ifndef _AUTO_WAV_DEC_H_
#define _AUTO_WAV_DEC_H_

#include "audio_type_def.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief      Open an Audio Element type data
 *
 * @param      el   The audio element handle
 *
 * @return
 *             ESP_OK
 *             ESP_FAIL
 */
esp_err_t wav_decoder_open(audio_element_handle_t el);

/**
 * @brief      Close an Audio Element type data
 *
 * @param      el   The audio element handle
 *
 * @return
 *             ESP_OK
 *             ESP_FAIL
 */
esp_err_t wav_decoder_close(audio_element_handle_t el);

/**
 * @brief      Do music data to decode
 *
 * @param      el   The audio element handle
 *
 * @return
 *             ESP_OK
 *             ESP_FAIL
 */
esp_codec_err_t wav_decoder_process(audio_element_handle_t el);

/**
 * @brief      Seek postion of music data to decode
 *
 * @param      self         The audio element handle
 * @param      in_data      The parameter required by callback function format. Not used by this function.
 * @param      in_size      The parameter required by callback function format. Not used by this function.
 * @param      out_data     The position of music data
 * @param      out_size     the size of `outdata`
 *
 * @return
 *             ESP_OK
 *             ESP_FAIL
 */
esp_err_t wav_decoder_get_pos(audio_element_handle_t self, void *in_data, int in_size, void *out_data, int *out_size);

#ifdef __cplusplus
}
#endif

#endif
