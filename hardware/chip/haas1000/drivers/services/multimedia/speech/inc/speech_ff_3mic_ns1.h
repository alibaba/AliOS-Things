#ifndef __SPEECH_FF_3MIC_NS1_H__
#define __SPEECH_FF_3MIC_NS1_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif
struct SpeechFF3MicNs1State_;
typedef struct SpeechFF3MicNs1State_ SpeechFF3MicNs1State;
SpeechFF3MicNs1State *speech_ff_3mic_ns1_create(int sample_rate, int frame_size);
int32_t speech_ff_3mic_ns1_destroy(SpeechFF3MicNs1State *st);
int32_t speech_3mic_ns1_process(SpeechFF3MicNs1State *st, short *pcm_buf, short *ref_buf, int32_t pcm_len, short *out_buf);
#ifdef __cplusplus
}
#endif

#endif