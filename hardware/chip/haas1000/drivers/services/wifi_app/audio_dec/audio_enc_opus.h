/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
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