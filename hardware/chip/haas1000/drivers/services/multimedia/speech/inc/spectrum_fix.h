/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#ifndef SPECTRUM_FIX_H
#define SPECTRUM_FIX_H

#include <stdint.h>

#define MAX_FREQ_NUM (10)

typedef struct
{
    int freq_num;
    int freq_list[MAX_FREQ_NUM];
} SpectrumFixConfig;

struct SpectrumFixState_;

typedef struct SpectrumFixState_ SpectrumFixState;

#ifdef __cplusplus
extern "C" {
#endif

SpectrumFixState *spectrum_fix_init(int sample_rate, int frame_size, const SpectrumFixConfig *config);

void spectrum_fix_destroy(SpectrumFixState *st);

/* Update input buffer, must be called every block */
void spectrum_fix_analysis(SpectrumFixState *st, const int16_t *x);

/* Calculate spectrum, should be called when needed */
void spectrum_fix_process(SpectrumFixState *st, int16_t *spectrum, int spectrum_num);

#ifdef __cplusplus
}
#endif

#endif