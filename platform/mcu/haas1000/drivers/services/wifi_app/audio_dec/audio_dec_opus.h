/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#ifndef _AUDIO_DEC_OPUS_H_
#define _AUDIO_DEC_OPUS_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "audio_dec.h"

    /* call this function befor audio_opus_dec_open to config decoder arg */
    int audio_opus_dec_set_arg(uint32_t sample_rate, uint32_t frame_size_time_ms, uint8_t channles_num);
    void *audio_opus_dec_open(void);
    int audio_opus_dec_write(void *opus_dec_handle, uint8_t *data, uint32_t input_size, audio_dec_pcm_handler_cb cb, void *cb_arg);
    int audio_opus_dec_close(void *opus_dec_handle);
    uint32_t audio_opus_dec_get_decoder_available_input_data_size(void *opus_dec_handle);

#ifdef __cplusplus
}
#endif

#endif /* _AUDIO_DEC_OPUS_H_ */