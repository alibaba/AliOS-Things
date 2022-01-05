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
#ifndef _AUDIO_ENC_OPUS_H_
#define _AUDIO_ENC_OPUS_H_

#ifdef __cplusplus
extern "C"
{
#endif
    typedef enum
    {
        OPUS_ENC_APP_AUDIO,
        OPUS_ENC_APP_VOIP,
        OPUS_ENC_APP_RESTRICTED_LOWDELAY,
    } OPUS_ENC_APP_T;

    int audio_opus_enc_init(uint32_t sample_rate, uint8_t channels_num, uint32_t frame_size_time_ms, OPUS_ENC_APP_T app_type);
    int audio_opus_enc_write(uint8_t *data_in, uint8_t *data_out, int max_output_size);
    int audio_opus_enc_close(void);

    int audio_opus_enc_test(uint8_t *pcm_data, uint32_t pcm_len);
#ifdef __cplusplus
}
#endif

#endif /* _AUDIO_ENC_OPUS_H_ */