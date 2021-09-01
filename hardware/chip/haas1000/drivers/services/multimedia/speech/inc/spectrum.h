/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#ifndef SPECTRUM_H
#define SPECTRUM_H

#include <stdint.h>

#define MAX_FREQ_NUM (10)

typedef struct
{
    int freq_num;
    int freq_list[MAX_FREQ_NUM];
} SpectrumConfig;

struct SpectrumState_;

typedef struct SpectrumState_ SpectrumState;

#ifdef __cplusplus
extern "C" {
#endif

SpectrumState *spectrum_init(int sample_rate, int frame_size, const SpectrumConfig *config);

void spectrum_destroy(SpectrumState *st);

/* Update input buffer, must be called every block */
void spectrum_analysis(SpectrumState *st, const float *x);

/* Calculate spectrum, should be called when needed */
void spectrum_process(SpectrumState *st, float *spectrum, int spectrum_num);

#ifdef __cplusplus
}
#endif

#endif
