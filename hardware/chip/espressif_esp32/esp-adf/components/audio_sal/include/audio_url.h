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

#ifndef __AUDIO_URL_H__
#define __AUDIO_URL_H__

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Allocate new memory, encoding given string to percent-encoding and return the pointer
 *
 * @note Returned pointer should be freed by user.
 *       ALPHA(0x41-0x5A and 0x61-0x7A), DIGIT (0x30-0x39) and "~!@#$&*()=:/,;?+'-_." are reserved.
 *
 * @param  str   String to be encode
 *
 * @return
 *     - valid pointer on success
 *     - NULL when any errors
 */
char *audio_url_encode(const char *str);

/**
 * @brief Allocate new memory, decoding given percent-encoding string and return the pointer
 *
 * @note Returned pointer should be freed by user
 *
 * @param str  String to be decode
 *
 * @return
 *     - valid pointer on success
 *     - NULL when any errors
 */
char *audio_url_decode(const char *str);


#ifdef __cplusplus
}
#endif

#endif
