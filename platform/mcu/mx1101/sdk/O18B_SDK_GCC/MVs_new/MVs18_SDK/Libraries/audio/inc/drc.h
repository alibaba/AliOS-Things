/**
 *************************************************************************************
 * @file	drc.h
 * @brief	DRC (Dynamic Range Control) fixed-point implementation
 *
 * @author	ZHAO Ying (Alfred)
 * @version	v1.3.7
 *
 * &copy; Shanghai Mountain View Silicon Co.,Ltd. All rights reserved.
 *************************************************************************************
 */

#ifndef __DRC_H__
#define __DRC_H__

#include <stdint.h>

#define MAX_CH 2
//#define MAX_N 192
#define FLOAT float
#define INTERNAL_64BIT

/** error code for DRC*/
typedef enum _DRC_ERROR_CODE
{
    DRC_ERROR_SAMPLE_RATE_NOT_SUPPORTED = -256,
    DRC_ERROR_NUM_CHANNELS_NOT_SUPPORTED,
	DRC_ERROR_THRESHOLD_OUT_OF_RANGE,
	DRC_ERROR_RATIO_OUT_OF_RANGE,

	// No Error
	DRC_ERROR_OK = 0,					/**< no error              */
} DRC_ERROR_CODE;


/** DRC context */
typedef struct _DRCContext
{
	int32_t num_channels;		// number of channels
	int32_t sample_rate;		// sample rate
	int32_t fc;					// cut-off frequency for 2-bands mode

	int32_t threshold;			// threshold in dB
	int32_t knee;				// knee width in dB (0: hard knee; other values: soft knee)
	//int32_t ratio;				// compression ratio
	//int32_t attack_tc;			// attack time constant in millisecond
	//int32_t release_tc;			// release time constant in millisecond

	int32_t maxdb;
	int32_t slope;
	int32_t alpha_attack;
	int32_t alpha_release;
#ifdef INTERNAL_64BIT
	int64_t state[MAX_CH];
#else
	int32_t state[MAX_CH];
#endif
	
	int32_t bl[2];
	int32_t bh[2];
	int32_t a[2];
	
	int32_t d[MAX_CH][14];		// filter delays of x & y

} DRCContext;


#ifdef __cplusplus
extern "C" {
#endif//__cplusplus


/**
 * @brief Initialize Dynamic Range Control (DRC) module
 * @param ct Pointer to a DRCContext object.
 * @param num_channels Number of channels. 
 * @param sample_rate Sample rate.
 * @param threshold A compressor reduces the level of an audio signal if its amplitude exceeds a certain threshold. Set the threshold in dB. Allowable range: [-90,0]
 * @param knee Width of knee in dB. Set it 0 for fixed-point implementation.
 * @param ratio The amount of gain reduction is determined by ratio: a ratio of 4:1 means that if input level is 4 dB over the threshold, the output signal level will be 1 dB over the threshold. The gain (level) has been reduced by 3 dB.
 * @param attack_tc Amount of time (in millisecond) it will take for the gain to decrease to a set level.
 * @param release_tc Amount of time (in millisecond) it will take for the gain to restore to 0dB.
 * @param fc Crossover frequency in Hz to seperate the full band into 2 subbands. Set 0 to apply DRC for the full band. Set other values (20 ~ 20000) to apply DRC for the subband lower than the crossover frequency only.
 * @return error code. DRC_ERROR_OK means successful, other codes indicate error.
 */
int32_t init_drc(DRCContext *ct, int32_t num_channels, int32_t sample_rate, int32_t threshold, int32_t knee, int32_t ratio, int32_t attack_tc, int32_t release_tc, int32_t fc);

/**
 * @brief Apply Dynamic Range Control (DRC) to a frame of PCM data.
 * @param ct Pointer to a DRCContext object.
 * @param pcm_in Address of the PCM input. The PCM layout must be the same as in Microsoft WAVE format, i.e. for mono: M0,M1,M2,... ("mono-S"); for stereo: L0,R0,L1,R1,L2,R2,... ("stereo-R+L")
 * @param pcm_out Address of the PCM output. The PCM layout is the same as in Microsoft WAVE format, i.e. for mono: M0,M1,M2,... ("mono-S"); for stereo: L0,R0,L1,R1,L2,R2,... ("stereo-R+L")
 *        pcm_out can be the same as pcm_in. In this case, the PCM signals are changed in-place.
 * @param n Number of PCM samples to process.
 * @return error code. DRC_ERROR_OK means successful, other codes indicate error.
 */
int32_t apply_drc(DRCContext *ct, int16_t *pcm_in, int16_t *pcm_out, int32_t n);


#ifdef __cplusplus
}
#endif //__cplusplus

#endif //__DRC_H__
