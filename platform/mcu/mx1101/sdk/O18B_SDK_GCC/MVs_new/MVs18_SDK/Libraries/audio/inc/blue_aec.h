/**
 *************************************************************************************
 * @file	blue_aec.h
 * @brief	Acoustic Echo Cancellation (AEC) routines for narrow band voice signals
 *
 * @author	ZHAO Ying (Alfred)
 * @version	V4.7.0
 *
 * &copy; Shanghai Mountain View Silicon Technology Co.,Ltd. All rights reserved.
 *
 *************************************************************************************
 */

#ifndef _BLUE_AEC_H
#define _BLUE_AEC_H

//#include "typedefine.h"
#include <stdint.h>


#define BLK_LEN 64				// Block length
#define BLK_BITS 6				// 2^BLK_BITS=BLK_LEN
#define FILTER_L 8				// Filter length = BLK_LEN * FILTER_L
#define FILTER_L_BITS 3			// 2^FILTER_L_BITS = FILTER_L
#define IP_LEN 10				// at least 2+sqrt(64)
//#define IP_LEN BLK_LEN		// at least 2+sqrt(64)
#define W_LEN BLK_LEN
#define DELAY_L FILTER_L*2		// Minimum value = FILTER_L*1. Delay (in samples) = BLK_LEN * DELAY_L. 


typedef struct _BlueAECContext
{
	// FFT related
	int32_t ip[IP_LEN];
	int32_t wfft[W_LEN];

	// matrices
	int32_t wm[FILTER_L][BLK_LEN*2];
	int32_t um[DELAY_L][BLK_LEN*2];
	int32_t uwm[DELAY_L][BLK_LEN*2];

	// vectors
	int32_t uv[BLK_LEN*2];
	int32_t yev[BLK_LEN*2];
	int32_t phiv[BLK_LEN*2];
	int32_t pv[BLK_LEN+1];
	int32_t min_df[BLK_LEN+1];
	int32_t df_bg[BLK_LEN+1];

	int64_t dd[BLK_LEN+1];
	int64_t uu[BLK_LEN+1];
	int64_t ee[BLK_LEN+1];
	int64_t ud[BLK_LEN*2];
	int64_t ed[BLK_LEN*2];

	int16_t cud[BLK_LEN+1];
	int16_t ced[BLK_LEN+1];
	int16_t hed[BLK_LEN+1];		

	int16_t uin_prev[BLK_LEN];
	int16_t din_prev[BLK_LEN];
	int16_t eout1_prev[BLK_LEN];
	int16_t eout2_prev[BLK_LEN];
	
	// scalars
	int32_t curr_blk_idx;
	int32_t delay_blocks;
	int32_t blk_counter;
	int32_t maxw_idx;
	int32_t min_pd;
	int32_t min_df_found;
	int32_t diverge;
	int32_t supp_state;
	int32_t cud_localmin;
	int32_t hlocalmin;
	int32_t hmin;
	int32_t hnewmin;
	int32_t hminctr;
	int32_t ovrd, ovrd_sm;

} BlueAECContext;

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus


/**
 * @brief  Initialization for AEC
 * @param  aecct Pointer to the AEC context structure. The structure's memory should be allocated by the calling process.
 * @return 0 (always success)
 */
int32_t blue_aec_init(BlueAECContext* aecct);


/**
 * @brief  AEC processing routine
 * @param  aecct Pointer to the AEC context structure. The structure's memory should be allocated by the calling process.
 * @param  uin far-end voice block
 * @param  din near-end (local) voice block recorded from microphone
 * @param  eout AEC processed near-end (local) voice block for being sent to the far-end
 * @return none
 */
void blue_aec_run(BlueAECContext* aecct, int16_t uin[BLK_LEN], int16_t din[BLK_LEN], int16_t eout[BLK_LEN]);

#ifdef __cplusplus
}
#endif//__cplusplus

#endif // _BLUE_AEC_H
