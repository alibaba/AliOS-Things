/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#ifndef __AUDIO_DRC2_H__
#define __AUDIO_DRC2_H__

#include <stdint.h>

#define MAX_LOOK_AHEAD_TIME (10)

typedef struct {
    int knee;
    int look_ahead_time;
    int threshold;
    float makeup_gain;
    int ratio;
    int attack_time;
    int release_time;
} DRC2_CFG_T;

#ifdef __cplusplus
extern "C" {
#endif

void audio_drc2_open(int sample_rate, int sample_bits, int ch_num);
void audio_drc2_close(void);
void audio_drc2_process(uint8_t *buf_p_l, uint8_t *buf_p_r, uint32_t pcm_len);
void audio_drc2_set_cfg(const DRC2_CFG_T *cfg);


#ifdef __cplusplus
}
#endif

#endif
