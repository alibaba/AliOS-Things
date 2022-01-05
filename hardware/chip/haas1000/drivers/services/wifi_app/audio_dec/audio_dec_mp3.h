/*
 * Copyright (c) 2021 Bestechnic (Shanghai) Co., Ltd. All rights reserved.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef _AUDIO_DEC_MP3_H_
#define _AUDIO_DEC_MP3_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "audio_dec.h"

#define AUDIO_DEC_MP3_FIFO_SIZE (1024*2)

    void *audio_mp3_dec_open(void);
    int audio_mp3_dec_write(void *mp3_dec_handle, uint8_t *data, uint32_t input_size, audio_dec_pcm_handler_cb cb, void *cb_arg);
    int audio_mp3_dec_close(void * mp3_dec_handle);
    uint32_t audio_mp3_dec_get_decoder_available_input_data_size(void *mp3_dec_handle);

#ifdef __cplusplus
}
#endif

#endif /* _AUDIO_DEC_MP3_H_ */
