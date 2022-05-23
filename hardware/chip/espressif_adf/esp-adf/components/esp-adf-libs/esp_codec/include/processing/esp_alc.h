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

#ifndef _ESP_AUDIO_ALC_H_
#define _ESP_AUDIO_ALC_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief      ALC Configuration
 */
typedef struct {
    unsigned char *in_buf;      /*!< Input buffer */
    int volume;                 /*!< Volume of input stream. The volume can be set.*/
    int channel;                /*!< Number of channels for input stream */
    void *handle;               /*!< The handle for setting volume */
} volume_set_t;

/**
 * @brief      Initialise volume setup with ALC
 *
 * @return     Handle for setting volume
 */
void *alc_volume_setup_open();

/**
 * @brief      Stop and cleanup the handle for setting volume with ALC
 *
 * @param      handle     the handle for setting volume
 */
void alc_volume_setup_close(void *handle);

/**
 * @brief      Process data buffer by setting volume with ALC
 *
 * @param      buffer     input and output buffer
 * @param      bytes      size of `buffer`. unit: bytes
 * @param      channels   the number of channels for input stream
 * @param      handle     the handle for setting volume
 * @param      volume     the volume to be set to
 *
 * @return     -1         the number of channels for input stream is not 1 or 2
 *              0         success
 */
int alc_volume_setup_process(void *buffer, unsigned int bytes, int channels, void *handle, int volume);

#ifdef __cplusplus
}
#endif

#endif
