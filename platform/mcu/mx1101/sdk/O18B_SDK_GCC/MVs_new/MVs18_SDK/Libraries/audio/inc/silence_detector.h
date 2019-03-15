/**
 *************************************************************************************
 * @file	silence_detector.h
 * @brief	Silence detector.
 *
 * @author	ZHAO Ying (Alfred)
 * @version	v1.0.0
 *
 * &copy; Shanghai Mountain View Silicon Co.,Ltd. All rights reserved.
 *************************************************************************************
 */

#ifndef __SILENCE_DETECTOR_H__
#define __SILENCE_DETECTOR_H__

#include <stdint.h>

//#define MAX_CH 2
//#define MAX_N 192

/** error code for silence detector */
typedef enum _SILENCE_DETECTOR_ERROR_CODE
{
    SILENCE_DETECTOR_ERROR_SAMPLE_RATE_NOT_SUPPORTED = -256,
	SILENCE_DETECTOR_ERROR_NUMBER_OF_CHANNELS_NOT_SUPPORTED,
	
	// No Error
	SILENCE_DETECTOR_ERROR_OK = 0,					/**< no error              */
} SILENCE_DETECTOR_ERROR_CODE;

/** silence detector context */
typedef struct _SilenceDetectorContext
{
	int32_t sample_rate;
	int32_t num_channels;	
	int32_t state;
	
} SilenceDetectorContext;


#ifdef __cplusplus
extern "C" {
#endif//__cplusplus

/**
 * @brief Initialize silence detector module
 * @param ct Pointer to a SilenceDetectorContext object.
 * @param num_channels number of channels. Both 1 and 2 channels are supported.
 * @param sample_rate sample rate
 * @return error code. SILENCE_DETECTOR_ERROR_OK means successful, other codes indicate error.
 */
int32_t init_silence_detector(SilenceDetectorContext *ct, int32_t num_channels, int32_t sample_rate);


/**
 * @brief Apply silence detector to a frame of PCM data.
 * @param ct Pointer to a SilenceDetectorContext object.
 * @param pcm_in Address of the PCM input. The PCM layout must be the same as in Microsoft WAVE format for both mono and stereo cases.
 * @param n Number of PCM samples to process. 
 * @return smoothed PCM amplitude (>=0 always)
 */
int32_t apply_silence_detector(SilenceDetectorContext *ct, int16_t *pcm_in, int32_t n);


#ifdef __cplusplus
}
#endif//__cplusplus

#endif//__SILENCE_DETECTOR_H__
