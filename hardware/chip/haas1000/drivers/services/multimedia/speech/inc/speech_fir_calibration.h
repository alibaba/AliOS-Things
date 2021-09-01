#ifndef SPEECH_FIR_CALIBRATION_H
#define SPEECH_FIR_CALIBRATION_H

#include "fftfilt.h"

// i.e. only support 4 channels
#define SPEECH_FIR_CALIB_MAX_NUM (3)

typedef struct
{
    float *filter;
    int filter_length;
} CalibChannelConfig;

typedef struct
{
    int bypass;
    int mic_num;
    float delay;    // fraction delay for main mic
    CalibChannelConfig calib[SPEECH_FIR_CALIB_MAX_NUM];
} SpeechFirCalibConfig;

typedef struct SpeechFirCalibState_ SpeechFirCalibState;

#define CONSTRUCT_FUNC_NAME_A(p, c, m)          p ## _ ## c ## _ ## m
#define CONSTRUCT_FUNC_NAME(p, c, m)            CONSTRUCT_FUNC_NAME_A(p, c, m)

#ifndef FIR_CALIB_IMPL
#if defined(VQE_SIMULATE)
#define FIR_CALIB_IMPL fir
#else
#define FIR_CALIB_IMPL hwfir
#endif
#endif

#define speech_fir_calib_init CONSTRUCT_FUNC_NAME(speech, FIR_CALIB_IMPL, calib_init)
#define speech_fir_calib_destroy CONSTRUCT_FUNC_NAME(speech, FIR_CALIB_IMPL, calib_destroy)
#define speech_fir_calib_process CONSTRUCT_FUNC_NAME(speech, FIR_CALIB_IMPL, calib_process)

#ifdef __cplusplus
extern "C" {
#endif

SpeechFirCalibState *speech_fir_calib_init(int32_t sample_rate, int32_t frame_size, const SpeechFirCalibConfig *config);

void speech_fir_calib_destroy(SpeechFirCalibState *st);

void speech_fir_calib_process(SpeechFirCalibState *st, int16_t *buf, int32_t frame_size);

#ifdef __cplusplus
}
#endif

#endif