/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#ifndef __SPEECH_MICS_CALIBRATION_H__
#define __SPEECH_MICS_CALIBRATION_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// #define SPEECH_MICS_CALIBRATION_SET_MODE            (0)
// #define SPEECH_MICS_CALIBRATION_GET_MODE            (1)

#define SPEECH_MICS_CALIBRATION_GET_COEF_LEN        (2)
#define SPEECH_MICS_CALIBRATION_GET_COEF            (3)
#define SPEECH_MICS_CALIBRATION_SET_COEF            (4)

#define SPEECH_MICS_CALIBRATION_GET_PS_MSE          (5)

#define SPEECH_MICS_CALIBRATION_GET_MICOUT_LEN      (10)
#define SPEECH_MICS_CALIBRATION_GET_MIC1OUT         (11)
#define SPEECH_MICS_CALIBRATION_GET_MIC2OUT         (12)
#define SPEECH_MICS_CALIBRATION_GET_MIC3OUT         (13)
#define SPEECH_MICS_CALIBRATION_GET_MIC4OUT         (14)
#define SPEECH_MICS_CALIBRATION_GET_MIC5OUT         (15)
#define SPEECH_MICS_CALIBRATION_GET_MIC6OUT         (16)

// mode
#define SPEECH_MICS_CALIBRATION_MODE_CALIBRATION    (0)
#define SPEECH_MICS_CALIBRATION_MODE_VERIFY         (1)
#define SPEECH_MICS_CALIBRATION_MODE_NULL           (2)

// Return status
#define SPEECH_MICS_CALIBRATION_RET_READY           (0)
#define SPEECH_MICS_CALIBRATION_RET_BUSY            (1)
#define SPEECH_MICS_CALIBRATION_RET_OK              (2)


typedef struct {
    int32_t enable;
    // int32_t mode;
    int32_t setup_time;
    int32_t calibration_time;
    int32_t verify_time;
} SPEECH_MICS_CALIBRATION_CFG_T;

typedef void *SPEECH_MICS_CALIBRATION_STATE_T;

// Creat a instance from speech_mics_calibration module/class
// Common value include: sample rate, frame size and so on.
SPEECH_MICS_CALIBRATION_STATE_T *speech_mics_calibration_create(SPEECH_MICS_CALIBRATION_CFG_T *cfg, int frame_size, int sample_rate);

// Destory a speech mics_calibration instance
int speech_mics_calibration_destroy(SPEECH_MICS_CALIBRATION_STATE_T *inst);

// Just use modify instance configure
int speech_mics_calibration_set_config(SPEECH_MICS_CALIBRATION_STATE_T *inst, SPEECH_MICS_CALIBRATION_CFG_T *cfg);

// Enable some function
int speech_mics_calibration_ctl(SPEECH_MICS_CALIBRATION_STATE_T *inst, int ctl, void *ptr);

// Process speech stream
int speech_mics_calibration_process(SPEECH_MICS_CALIBRATION_STATE_T *inst, int16_t *pcm_mic1, int16_t *pcm_mic2, int32_t pcm_len);

// Test: verify calibration effect
int speech_mics_calibration_test_verify(SPEECH_MICS_CALIBRATION_STATE_T *inst, int16_t *pcm_mic1, int16_t *pcm_mic2, int32_t pcm_len);

// Debug speech_mics_calibration instance
int speech_mics_calibration_dump(SPEECH_MICS_CALIBRATION_STATE_T *inst);

#ifdef __cplusplus
}
#endif

#endif
