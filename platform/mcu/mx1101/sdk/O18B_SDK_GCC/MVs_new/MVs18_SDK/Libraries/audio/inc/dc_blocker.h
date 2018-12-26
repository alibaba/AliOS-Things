/**
 *************************************************************************************
 * @file	dc_blocker.h
 * @brief	DC blocker
 *
 * @author	ZHAO Ying (Alfred)
 * @version	V1.0.0
 *
 * &copy; Shanghai Mountain View Silicon Technology Co.,Ltd. All rights reserved.
 *
 *************************************************************************************
 */

#ifndef _DC_BLOCKER_H
#define _DC_BLOCKER_H

#include <stdint.h>

#define MAX_CH 2
//#define MAX_N 192


typedef struct _DCBlocker
{
	int32_t num_channels;
	int32_t x_delay[MAX_CH];
	int32_t y_delay[MAX_CH];	
	int64_t delta[MAX_CH];
} DCBlocker;


#ifdef __cplusplus
extern "C" {
#endif//__cplusplus

/**
 * @brief Initialization for DC blocker
 * @param ct Pointer to the DC blocker context structure. The structure's memory should be allocated by the calling process.
 * @param num_channels Number of channels. 
 * @return error code. 0 means successful, other codes indicate error.
 */
int32_t init_dc_blocker(DCBlocker* ct, int32_t num_channels);


/**
 * @brief Apply DC blocker to a frame of PCM data.
 * @param ct Pointer to a DCBlocker object.
 * @param pcm_in Address of the PCM input. The PCM layout must be the same as in Microsoft WAVE format, i.e. for mono: M0,M1,M2,... ("mono-S"); for stereo: L0,R0,L1,R1,L2,R2,... ("stereo-R+L")
 * @param pcm_out Address of the PCM output. The PCM layout is the same as in Microsoft WAVE format, i.e. for mono: M0,M1,M2,... ("mono-S"); for stereo: L0,R0,L1,R1,L2,R2,... ("stereo-R+L")
 *        pcm_out can be the same as pcm_in. In this case, the PCM signals are changed in-place.
 * @param n Number of PCM samples to process.
 * @return none
 */
void apply_dc_blocker(DCBlocker* ct, int16_t *pcm_in, int16_t *pcm_out, int32_t n);

#ifdef __cplusplus
}
#endif//__cplusplus

#endif // _DC_BLOCKER_H
