/*
 * ESPRESSIF MIT License
 *
 * Copyright (c) 2019 <ESPRESSIF SYSTEMS (SHANGHAI) CO., LTD>
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

#ifndef _ESP_G711_H
#define _ESP_G711_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief      This function accepts an 16-bit signed integer and encodes it as A-law data
 *             stored in a unsigned char.
 *
 * @param[in]  pcm_val   16-bits pcm
 *
 * @return     a-law pcm
 */
unsigned char esp_g711a_encode(int16_t pcm_val);

/**
 * @brief      Convert an A-law value to 16-bit signed linear PCM
 *
 * @param[in]  pcm_val   a-law pcm
 *
 * @return     16-bits pcm
 */
int16_t esp_g711a_decode(unsigned char pcm_val);

/**
 * @brief      This function accepts a 16-bit signed integer and encodes it as u-law data
 *             stored in a unsigned char.
 *
 * @param[in]  pcm_val   16-bits pcm
 *
 * @return     u-law pcm
 */
unsigned char esp_g711u_encode(int16_t pcm_val);

/**
 * @brief      Convert a u-law value to 16-bit linear PCM
 *
 * @param[in]  pcm_val   u-law pcm
 *
 * @return     16-bits pcm
 */
int16_t esp_g711u_decode(unsigned char pcm_val);


#ifdef __cplusplus
}
#endif


#endif
