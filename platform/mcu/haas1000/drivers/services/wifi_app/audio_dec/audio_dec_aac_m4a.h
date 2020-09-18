/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#ifndef _AUDIO_DEC_AAC_M4A_H_
#define _AUDIO_DEC_AAC_M4A_H_

#ifdef __cplusplus
extern "C"
{
#endif

#define AUDIO_DEC_AAC_M4A_FIFO_SIZE (1024)

    int audio_aac_m4a_dec_open(void);
    int audio_aac_m4a_dec_write(uint8_t *data, uint32_t input_size, audio_dec_pcm_handler_cb cb, void * cb_arg);
    int audio_aac_m4a_dec_close(void);
    uint32_t audio_aac_m4a_dec_get_decoder_available_input_data_size(void);
    
    int audio_aac_m4a_dec_prepare_file(uint8_t *data, uint32_t data_len);

#ifdef __cplusplus
}
#endif

#endif /* _AUDIO_DEC_AAC_M4A_H_ */