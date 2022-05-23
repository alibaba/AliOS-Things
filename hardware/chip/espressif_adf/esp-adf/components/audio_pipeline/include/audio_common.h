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

#ifndef _AUDIO_COMMON_H_
#define _AUDIO_COMMON_H_


#ifdef __cplusplus
extern "C" {
#endif

#include "audio_type_def.h"

#define ELEMENT_SUB_TYPE_OFFSET 16

typedef enum {
    AUDIO_ELEMENT_TYPE_UNKNOW = 0x01<<ELEMENT_SUB_TYPE_OFFSET,
    AUDIO_ELEMENT_TYPE_ELEMENT= 0x01<<(ELEMENT_SUB_TYPE_OFFSET+1),
    AUDIO_ELEMENT_TYPE_PLAYER = 0x01<<(ELEMENT_SUB_TYPE_OFFSET+2),
    AUDIO_ELEMENT_TYPE_SERVICE = 0x01<<(ELEMENT_SUB_TYPE_OFFSET+3),
    AUDIO_ELEMENT_TYPE_PERIPH = 0x01<<(ELEMENT_SUB_TYPE_OFFSET+4),
} audio_element_type_t;

typedef enum {
    AUDIO_STREAM_NONE = 0,
    AUDIO_STREAM_READER,
    AUDIO_STREAM_WRITER
} audio_stream_type_t;

typedef enum {
    AUDIO_CODEC_TYPE_NONE = 0,
    AUDIO_CODEC_TYPE_DECODER,
    AUDIO_CODEC_TYPE_ENCODER
} audio_codec_type_t;

#define mem_assert(x)

#ifdef __cplusplus
}
#endif

#endif
