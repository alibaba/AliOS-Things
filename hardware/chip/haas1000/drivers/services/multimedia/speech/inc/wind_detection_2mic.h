/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#ifndef __WIND_DETECTION_H__
#define __WIND_DETECTION_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    int32_t     bypass;
} WindDetection2MicConfig;

struct WindDetection2MicState_;

typedef struct WindDetection2MicState_ WindDetection2MicState;

WindDetection2MicState *WindDetection2Mic_create(int sample_rate, int frame_size, const WindDetection2MicConfig *cfg);

int32_t WindDetection2Mic_destroy(WindDetection2MicState *st);

int32_t WindDetection2Mic_set_config(WindDetection2MicState *st, const WindDetection2MicConfig *cfg);

float WindDetection2Mic_process(WindDetection2MicState *st, short *mic1, short *mic2, uint32_t frame_len);

#ifdef __cplusplus
}
#endif

#endif
