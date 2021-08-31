/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#ifndef __APP_FACTORY_BT_H__
#define __APP_FACTORY__BTH__

#include "app_utils.h"

typedef struct {
    uint16_t *buf;
    uint32_t len;
    uint32_t cuur_buf_pos;
}audio_test_pcmpatten_t;

int app_factorymode_audioloop(bool on, enum APP_SYSFREQ_FREQ_T freq);

int app_factorymode_output_pcmpatten(audio_test_pcmpatten_t *pcmpatten, uint8_t *buf, uint32_t len);

int app_factorymode_mic_cancellation_run(void * mic_st, signed short *inbuf, int sample);

void *app_factorymode_mic_cancellation_init(void* (* alloc_ext)(int));

#endif
