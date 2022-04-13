/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#ifndef __UVOICE_RESAMPLER_H__
#define __UVOICE_RESAMPLER_H__


int uvoice_resampler_process(void *resampler, uint8_t *input, int input_len, uint8_t **output, int *output_len);
int uvoice_resampler_update(void *resampler, int src_rate, int dst_rate, int channels, int sbits);
int uvoice_resampler_create(void **resampler, int src_rate, int dst_rate, int channels, int sbits);
int uvoice_resampler_release(void *resampler);

#endif /* __UVOICE_RESAMPLER_H__ */
