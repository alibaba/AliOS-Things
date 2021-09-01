/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#ifndef __DUAL_MIC_DENOISE_H__
#define __DUAL_MIC_DENOISE_H__

#ifdef __cplusplus
extern "C" {
#endif

#define DUAL_MIC_DENOISE_GET_CALIBRATION_COEF_LEN        (0)
#define DUAL_MIC_DENOISE_GET_CALIBRATION_COEF            (1)
#define DUAL_MIC_DENOISE_SET_CALIBRATION_COEF            (2)

typedef struct {
    int   bypass;
    float alpha_h;
    float alpha_slow;
    float alpha_fast;
    float thegma;
    float thre_corr;
    float thre_filter_diff;
    float cal_left_gain;
    float cal_right_gain;
    int   delay_mono_sample;
    int   wnr_enable;
} DUAL_MIC_DENOISE_CFG_T;

/**
 * @brief      Initialization
 *
 * @param      alloc_ext  The allocate extent RAM
 * @param      cfg        The configuration set parameter
 */
void dual_mic_denoise_init(int32_t sample_rate, int32_t frame_size, const DUAL_MIC_DENOISE_CFG_T *cfg, void* (* alloc_ext)(int));

/**
 * @brief      Run
 *
 * @param      in    input sample buffer
 *                   sample rate: 16k;
 *                   sample bit: 16;
 *                   channel num: 2;
 *                   LRLRLR......: Left: main mic, Right: assist mic
 * @param      out   output sample buffer
 *                   sample rate: 16k;
 *                   sample bit: 16;
 *                   channel num: 1;
 * @param[in]  len   The length is sample number
 */
void dual_mic_denoise_run(short *in, short len, short *out);

/**
 * @brief     Calibrate left and right gain
 *
 * @param      in    Same as dual_mic_denoise_run definition
 * @param[in]  len   Same as dual_mic_denoise_run definition
 * @param[out] cfg   Return calibrate configure parameter
 */
void dual_mic_denoise_Cal_Gain(short *in, short len, DUAL_MIC_DENOISE_CFG_T *cfg);

/**
 * @brief      Oppose initialization
 */
void dual_mic_denoise_deinit(void);

int dual_mic_denoise_ctl(int ctl, void *ptr);

#ifdef __cplusplus
}
#endif

#endif