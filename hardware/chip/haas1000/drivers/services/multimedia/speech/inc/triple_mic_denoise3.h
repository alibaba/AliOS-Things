#ifndef TRIPLE_MIC_DENOISE3_H
#define TRIPLE_MIC_DENOISE3_H

#include <stdint.h>
#include "speech_config.h"

typedef struct
{
    int32_t bypass;
    int32_t endfire_enable;
    int32_t broadside_enable;

    // endfire beamforming config
    float delay_taps;
    int32_t freq_smooth_enable;
    int32_t wnr_enable;

    // broadsize beamforming config
} TripleMicDenoise3Config;

struct TripleMicDenoise3State_;

typedef struct TripleMicDenoise3State_ TripleMicDenoise3State;

TripleMicDenoise3State *triple_mic_denoise3_init(int32_t sample_rate, int32_t frame_size, const TripleMicDenoise3Config *cfg);

void triple_mic_denoise3_destroy(TripleMicDenoise3State *st);

// accept 3 channel interleaved pcm stream, ch0 is main mic, ch1 is endfire reference mic, ch2 is broadside reference mic
int32_t triple_mic_denoise3_process(TripleMicDenoise3State *st, speech_data_t *pcm_buf, int32_t pcm_len, speech_data_t *out_buf);

float triple_mic_denoise3_get_required_mips(TripleMicDenoise3State *st);

#endif