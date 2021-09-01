#ifndef __SPEECH_2MIC_NS6_H__
#define __SPEECH_2MIC_NS6_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    int32_t     bypass;
    int32_t     wnr_enable;
	int32_t     denoise_dB;
} Speech2MicNs6Config;

struct Speech2MicNs6State_;
typedef struct Speech2MicNs6State_ Speech2MicNs6State;

Speech2MicNs6State *speech_2mic_ns6_create(int sample_rate, int frame_size, const Speech2MicNs6Config *cfg);
int32_t speech_2mic_ns6_destroy(Speech2MicNs6State *st);
int32_t speech_2mic_ns6_process(Speech2MicNs6State *st, short *channel1,short *channel2, int32_t pcm_len);

float speech_2mic_ns6_get_required_mips(Speech2MicNs6State *st);

#ifdef __cplusplus
}
#endif

#endif