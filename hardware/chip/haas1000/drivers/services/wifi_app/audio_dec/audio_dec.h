/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#ifndef _AUDIO_DEC_
#define _AUDIO_DEC_
#ifdef __cplusplus
extern "C"
{
#endif

#include "plat_types.h"

    typedef enum
    {
        AUDIO_DEC_FORMAT_INVALID = -1,
        AUDIO_DEC_FORMAT_MP3 = 0,
        AUDIO_DEC_FORMAT_AAC_ADTS,
        AUDIO_DEC_FORMAT_AAC_M4A,
        AUDIO_DEC_FORMAT_OPUS,
    } audio_dec_format_t;

    typedef struct
    {
        uint32_t sample_rate;
        uint32_t channels_num;
        uint32_t bitrate;
        uint32_t sample_bit;
    } audio_dec_pcm_arg_t;

    typedef int (*audio_dec_pcm_handler_cb)(uint8_t *pcm, uint32_t pcm_size, audio_dec_pcm_arg_t *pcm_arg, void *user_arg);
    typedef int (*audio_dec_finish_cb)(void);

    int audio_dec_get_decoder_available_input_data_size(void * audio_dec_handle, audio_dec_format_t format);
    void* audio_dec_open(audio_dec_format_t format);
    int audio_dec_write(void * audio_dec_handle, audio_dec_format_t format, uint8_t *data, uint32_t input_size, audio_dec_pcm_handler_cb cb, void * cb_arg);
    int audio_dec_close(void * audio_dec_handle, audio_dec_format_t format, audio_dec_finish_cb cb);
    int audio_dec_check_pcm_arg_changed(audio_dec_pcm_arg_t *old_pcm_arg, audio_dec_pcm_arg_t *new_pcm_arg);

#ifdef __cplusplus
}
#endif

#endif /* _AUDIO_DEC_ */
