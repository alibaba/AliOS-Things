/**
 *************************************************************************************
 * @file	echo.h
 * @brief	Echo effect (for mono only)
 *
 * @author	ZHAO Ying (Alfred)
 * @version	v1.2.0
 *
 * &copy; Shanghai Mountain View Silicon Co.,Ltd. All rights reserved.
 *************************************************************************************
 */

#ifndef __ECHO_H__
#define __ECHO_H__

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus

#include <stdint.h>

#define MAX_DELAY 12032	// 48000*0.25 = 12000 samples (maximum 250ms delay @ 48kHz). +32 to avoid overlap of the latest block and the oldest block
#define MAX_DEPTH_VAL  16384//32768 

/** error code for echo effect */
typedef enum _ECHO_ERROR_CODE
{
    ECHO_ERROR_CHANNEL_NUMBER_NOT_SUPPORTED = -256,
    ECHO_ERROR_DELAY_TOO_LARGE,
    ECHO_ERROR_NUM_SAMPLES_NOT_EVEN,
    // No Error
    ECHO_ERROR_OK = 0,					/**< no error */
} ECHO_ERROR_CODE;


/** echo context */
typedef struct _EchoContext
{
	int32_t num_channels;					// number of channels
	//int32_t delay_samples;				// delay in samples
	int32_t p;								// next position for overwriting
	int32_t last_ap;						// last anchor PCM
	int8_t last_ai;							// last anchor index
	uint8_t s[MAX_DELAY / 2];					// compressed filter delays.
	int16_t anchor_pcm[MAX_DELAY / 32];		// anchor PCM samples
	int8_t  anchor_index[MAX_DELAY / 32];		// anchor index
} EchoContext;



/**
 * @brief Initialize echo effect module
 * @param ct Pointer to an EchoContext object.
 * @param num_channels Number of channels. Current function supports mono signals only due to memory constraint.
 * @return error code. ECHO_ERROR_OK means successful, other codes indicate error.
 */
int32_t init_echo(EchoContext* ct, int32_t num_channels);


/**
 * @brief Apply echo effect to a frame of PCM data.
 * @param ct Pointer to a EchoContext object.
 * @param pcm_in Address of the PCM input. The PCM layout must be the same as in Microsoft WAVE format, i.e. for mono: M0, M1, M2, ... ("mono-S"); for stereo: L0, R0, L1, R1, L2, R2, ... ("stereo-R+L")
 * @param pcm_out Address of the PCM output. The PCM layout is the same as in Microsoft WAVE format, i.e. for mono: M0, M1, M2, ... ("mono-S"); for stereo: L0, R0, L1, R1, L2, R2, ... ("stereo-R+L")
 *        pcm_out can be the same as pcm_in. In this case, the PCM signals are changed in-place.
 * @param n Number of PCM samples to process. n must be even.
 * @param attenuation attenuation coefficient. Q1.15 format to represent value in range from 0 to 1. For example, 8192 represents 0.25 as the attenuation coefficient.
 * @param delay_samples Delay in samples. Range: 0 ~ (MAX_DELAY-32).
 * @return error code. ECHO_ERROR_OK means successful, other codes indicate error.
 */
int32_t apply_echo(EchoContext* ct, int16_t* pcm_in, int16_t* pcm_out, int32_t n, int16_t attenuation, int32_t delay_samples);



#ifdef __cplusplus
}
#endif//__cplusplus

#endif//__ECHO_H__
