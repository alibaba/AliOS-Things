/**
 *************************************************************************************
 * @file	three_d.h
 * @brief	3D audio effect 
 *
 * @author	ZHAO Ying (Alfred)
 * @version	v3.0.0
 *
 * &copy; Shanghai Mountain View Silicon Co.,Ltd. All rights reserved.
 *************************************************************************************
 */

#ifndef __THREE_D_H__
#define __THREE_D_H__

#include <stdint.h>

#define MAX_CH 2
//#define MAX_N 192


/** error code for 3D */
typedef enum _THREE_D_ERROR_CODE
{
    THREE_D_ERROR_SAMPLE_RATE_NOT_SUPPORTED = -256,
	THREE_D_ERROR_NUMBER_OF_CHANNELS_NOT_SUPPORTED,
    THREE_D_ERROR_NON_POSITIVE_NUMBER_OF_SAMPLES,
	// No Error
	THREE_D_ERROR_OK = 0,					/**< no error              */
} THREE_D_ERROR_CODE;


/** 3D context */
typedef struct _ThreeDContext
{
	int32_t num_channels;
	int32_t sample_rate;
	int32_t sample_rate_index;
	int32_t intensity;
	int32_t pre_gain;
	int32_t post_gain;
	int32_t bass_preservation;
	int32_t mid_boost;

	int16_t sd[4];	// delayed samples
	int32_t s0[4];	// filter delays.
	int32_t s1[4];	// filter delays.
	int32_t sp[4];	// filter delays.
	
} ThreeDContext;


#ifdef __cplusplus
extern "C" {
#endif//__cplusplus


/**
 * @brief Initialize 3D audio effect module
 * @param td Pointer to a ThreeDContext object.
 * @param num_channels number of channels 
 * @param sample_rate Sample rate.
 * @param bass_preservation Flag to turn on/off preservation of bass (low frequencies). Once turned on, low frequencies are exempt from 3D processing. 1:on; 0:off.
 * @param mid_boost Flag to turn on/off boost of middle frequencies. Once turned on, middle frequencies are boosted. 1:on; 0:off.
 * @return error code. THREE_D_ERROR_OK means successful, other codes indicate error.
 */
int32_t init_3d(ThreeDContext *td, int32_t num_channels, int32_t sample_rate, int32_t bass_preservation, int32_t mid_boost);


/**
 * @brief Apply 3D audio effect to a frame of PCM data.
 * @param td Pointer to a ThreeDContext object.
 * @param pcm_in Address of the PCM input. The PCM layout must be the same as in Microsoft WAVE format for both mono and stereo cases.
 * @param pcm_out Address of the PCM output. The PCM layout is the same as in Microsoft WAVE format for both mono and stereo cases.
 *        pcm_out can be the same as pcm_in. In this case, the PCM is changed in-place.
 * @param n Number of PCM samples to process.
 * @param intensity 3D intensity. range: 0 ~ 100
 * @param pre_gain Pregain applied before 3D processing. Q2.15 format to represent value in range [0.5, 2]. For example, 16384 represents x0.5 (-6dB), 23198 represents x0.707 (-3dB), 32768 represents x1.0 (0dB),  46286 represents x1.413 (+3dB), 65536 represents x2 (+6dB)
 * @param post_gain Post gain applied after 3D processing. Q2.15 format to represent value in range [0.5, 2]. For example, 16384 represents x0.5 (-6dB), 32768 represents x1.0 (0dB),  46286 represents x1.413 (+3dB), 65536 represents x2 (+6dB)
 * @return error code. THREE_D_ERROR_OK means successful, other codes indicate error.
 */
int32_t apply_3d(ThreeDContext *td, int16_t *pcm_in, int16_t *pcm_out, int32_t n, int32_t intensity, int32_t pre_gain, int32_t post_gain);


#ifdef __cplusplus
}
#endif//__cplusplus

#endif//__THREE_D_H__
