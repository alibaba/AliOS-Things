/**
 *************************************************************************************
 * @file	sra.h
 * @brief	Sample Rate Adjustment (SRA)
 *
 * @author	ZHAO Ying (Alfred)
 * @version	v1.2.0
 *
 * &copy; Shanghai Mountain View Silicon Co.,Ltd. All rights reserved.
 *************************************************************************************
 */

#ifndef __SRA_H__
#define __SRA_H__

#include <stdint.h>

#define MAX_CH 2
#define SRA_BLOCK 128 // suitable for SBC decoder
#define SRA_MAX_CHG 2 // maximum number of samples to increase or decrease for a frame of PCM samples


/** error code for SRA */
typedef enum _SRA_ERROR_CODE
{
    SRA_ERROR_SAMPLE_RATE_CHANGE_TOO_LARGE = -256,
	// No Error
	SRA_ERROR_OK = 0,					/**< no error              */
} SRA_ERROR_CODE;


/** SRA context */
typedef struct _SRAContext
{
	int32_t num_channels;		// number of channels
	int16_t lastPCM[MAX_CH];    // last PCM sample(s)	
	//int32_t ni_all[SRA_MAX_CHG*2][SRA_BLOCK+SRA_MAX_CHG];	// index tables
} SRAContext;


#ifdef __cplusplus
extern "C" {
#endif//__cplusplus


/**
 * @brief Initialize Sample Rate Adjustment (SRA) module
 * @param sra Pointer to a SRAContext object.
 * @param num_channels Number of channels. 
 * @return error code. SRA_ERROR_OK means successful, other codes indicate error.
 */
int32_t init_sra(SRAContext *sra, int32_t num_channels);


/**
 * @brief Apply sample rate adjustment to a frame of PCM data (frame size must be always SRA_BLOCK)
 * @param sra Pointer to a SRAContext object.
 * @param pcm_in Address of the PCM input. The PCM layout must be the same as in Microsoft WAVE format, i.e. for mono: M0,M1,M2,... ("mono-S"); for stereo: L0,R0,L1,R1,L2,R2,... ("stereo-R+L")
 * @param pcm_out Address of the PCM output. The PCM layout is the same as in Microsoft WAVE format, i.e. for mono: M0,M1,M2,... ("mono-S"); for stereo: L0,R0,L1,R1,L2,R2,... ("stereo-R+L")
 *        pcm_out can not be the same as pcm_in, i.e. the output space should not overlap with the input space in memory.
 * @param n_inc_dec Number of PCM samples to increase (positive) or to decrease (negative). Valid range: [-SRA_MAX_CHG, SRA_MAX_CHG]. If set 0, the output is the same as input.
 * @return error code. SRA_ERROR_OK means successful, other codes indicate error.
 * @note This function should be called continuously even there is no need to insert or delete sample(s) in the frame. In this case, set n_inc_dec=0. This is to ensure that the output samples are continuous between frames.
 */
int32_t apply_sra(SRAContext *sra, int16_t *pcm_in, int16_t *pcm_out, int32_t n_inc_dec);


#ifdef __cplusplus
}
#endif//__cplusplus

#endif//__SRA_H__
