#ifndef SPEECH_IIR_CALIBRATION_H
#define SPEECH_IIR_CALIBRATION_H

#include "speech_eq.h"

// i.e. only support 4 channels
#define SPEECH_IIR_CALIB_MAX_NUM (3)

typedef struct
{
    int bypass;
    int mic_num;
    EqConfig calib[SPEECH_IIR_CALIB_MAX_NUM];
} SpeechIirCalibConfig;

typedef struct SpeechIirCalibState_ SpeechIirCalibState;

SpeechIirCalibState *speech_iir_calib_init(int32_t sample_rate, int32_t frame_size, const SpeechIirCalibConfig *config);

void speech_iir_calib_destroy(SpeechIirCalibState *st);

void speech_iir_calib_process(SpeechIirCalibState *st, int16_t *buf, int32_t frame_size);

#endif