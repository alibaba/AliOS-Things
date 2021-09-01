/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#ifndef __VOICEACTIVITYDETECTION_H__
#define __VOICEACTIVITYDETECTION_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    float     snrthd;
	float     energythd;
} VADConfig;

struct VADState_;

typedef struct VADState_ VADState;

VADState *VAD_process_state_init(int32_t sample_rate, int32_t frame_size, const VADConfig *cfg);

int32_t VAD_set_config(VADState *st, const VADConfig *cfg);

short VAD_process_run(VADState *st, short *in);

int32_t VAD_destroy(VADState *st);

#ifdef __cplusplus
}
#endif

#endif