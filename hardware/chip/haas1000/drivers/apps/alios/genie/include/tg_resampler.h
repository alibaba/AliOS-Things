/**
 * Copyright (C) 2019 Alibaba.inc, All rights reserved.
 *
 * @file:    tgcts_speech.h
 * @brief:
 * @author:  
 * @date:    2019/12/15
 * @version: 1.0
 */

#ifndef _TG_RESAMPLER_H_
#define _TG_RESAMPLER_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef void* tg_resample_handle_t;

#define TG_RESAMPLE_INSTACE_COUNT 3

tg_resample_handle_t tg_resampler_init(int32_t nb_channels, int32_t in_rate, int32_t out_rate, int quality, int *err);
void tg_resampler_destroy(tg_resample_handle_t resample);
int tg_resampler_process_init(tg_resample_handle_t resample, int32_t channel_index, const int16_t *in, int32_t *in_len, int16_t *out, int32_t *out_len);

int tg_resampler_skip_zeros(void);


#ifdef __cplusplus
}
#endif

#endif 

