#ifndef __WNR_H__
#define __WNR_H__

#include "iirfilt.h"

typedef struct
{
    int32_t bypass;
    int lpf_cutoff;
    int hpf_cutoff;
    float power_ratio_thr; // ratio of the power spectrum of the lower frequencies over the total power spectrum for all frequencies
    float power_thr;       // normalized power of the low frequencies
} WnrConfig;

struct WnrState_;

typedef struct WnrState_ WnrState;

#ifdef __cplusplus
extern "C" {
#endif

WnrState* wnr_create(int32_t sample_rate, int32_t frame_size, const WnrConfig *config);

void wnr_destroy(WnrState *st);

void wnr_process(WnrState *st, int16_t *buf, int frameSize);

#ifdef __cplusplus
}
#endif

#endif