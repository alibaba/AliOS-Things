#ifndef __SPEECH_FF_2MIC_NS2_H__
#define __SPEECH_FF_2MIC_NS2_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif
struct SpeechFF2MicNs2State_;
typedef struct SpeechFF2MicNs2State_ SpeechFF2MicNs2State;
SpeechFF2MicNs2State *speech_ff_2mic_ns2_create(int sample_rate, int frame_size);
int32_t speech_ff_2mic_ns2_destroy(SpeechFF2MicNs2State *st);
void speech_ff_2mic_ns2_process(SpeechFF2MicNs2State *st, short *inF, short *inR);
#ifdef __cplusplus
}
#endif

#endif