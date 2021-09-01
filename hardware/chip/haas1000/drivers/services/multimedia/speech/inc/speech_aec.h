/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#ifndef __SPEECH_AEC_H__
#define __SPEECH_AEC_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define SPEECH_AEC_GET_LIB_ST          (0)

/*
60
16383
4
*/
typedef struct {
    int32_t     bypass;
    int32_t     delay;
    int32_t     leak_estimate;
    int32_t     leak_estimate_shift;
} SpeechAecConfig;

struct SpeechAecState_;

typedef struct SpeechAecState_ SpeechAecState;

// Creat a instance from speech_aec module/class
// Common value include: sample rate, frame size and so on.
SpeechAecState *speech_aec_create(int32_t sample_rate, int32_t frame_size, const SpeechAecConfig *cfg);

// Destory a speech aec instance
int32_t speech_aec_destroy(SpeechAecState *st);

// Just use modify instance configure
int32_t speech_aec_set_config(SpeechAecState *st, const SpeechAecConfig *cfg);

// Get/set some value or enable/disable some function
int32_t speech_aec_ctl(SpeechAecState *st, int32_t ctl, void *ptr);

// Process speech stream
int32_t speech_aec_process(SpeechAecState *st, int16_t *pcm_in, int16_t *pcm_ref, int32_t pcm_len, int16_t *pcm_out);

// Debug speech_aec instance
int32_t speech_aec_dump(SpeechAecState *st);

#ifdef __cplusplus
}
#endif

#endif
