/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#ifndef __SINGLE_MIC_NN_DENOISE_H__
#define __SINGLE_MIC_NN_DENOISE_H__

#ifdef __cplusplus
extern "C" {
#endif


struct NNDenoiseState_;

typedef struct NNDenoiseState_ NNDenoiseState;



NNDenoiseState * single_mic_NN_denoise_create(int32_t sr, int32_t frame_size,int32_t post_coef);
void single_mic_NN_denoise_destroy(NNDenoiseState * st);
void single_mic_NN_denoise_run(NNDenoiseState * st,int16_t* input_buf);
















// NNDenoiseState *speech_ns2_create(int32_t sample_rate, int32_t frame_size, const SpeechNs2Config *cfg);

// int32_t speech_ns2_destroy(NNDenoiseState *st);

// int32_t speech_ns2_set_config(SpeechNs2State *st, const SpeechNs2Config *cfg);

// int32_t speech_ns2_process(SpeechNs2State *st, int16_t *pcm_buf, int32_t pcm_len);

// int32_t speech_ns2_set_echo_state(SpeechNs2State *st, void *echo_state);

// int32_t speech_ns2_set_echo_suppress(SpeechNs2State *st, int32_t echo_suppress);

#ifdef __cplusplus
}
#endif

#endif
