#ifndef __NDS32_TRANSFORM_MATH_H__
#define __NDS32_TRANSFORM_MATH_H__
/***************************************************************************
 * nds32_transform_math.h                                                  *
 *                                                                         *
 * Copyright (C) 2012-2017 Andes Technology Corporation                    *
 * All rights reserved.                                                    *
 ***************************************************************************/

/**
 * @defgroup transform Transform Functions
 */
#ifdef  __cplusplus
extern "C"
{
#endif

#include "nds32_math_types.h"

// CFFT-Radix2
int32_t nds32_cfft_radix2_f32(float32_t *src, uint32_t m);
int32_t nds32_cifft_radix2_f32(float32_t *src, uint32_t m);
int32_t nds32_cfft_radix2_q15(q15_t *src, uint32_t m);
int32_t nds32_cifft_radix2_q15(q15_t *src, uint32_t m);
int32_t nds32_cfft_radix2_q31(q31_t *src, uint32_t m);
int32_t nds32_cifft_radix2_q31(q31_t *src, uint32_t m);

#define nds32_cfft_rd2_f32 nds32_cfft_radix2_f32
#define nds32_cifft_rd2_f32 nds32_cifft_radix2_f32
#define nds32_cfft_rd2_q15 nds32_cfft_radix2_q15
#define nds32_cifft_rd2_q15 nds32_cifft_radix2_q15
#define nds32_cfft_rd2_q31 nds32_cfft_radix2_q31
#define nds32_cifft_rd2_q31 nds32_cifft_radix2_q31

// CFFT-Radix4
int32_t nds32_cfft_radix4_f32(float32_t *src, uint32_t m);
int32_t nds32_cifft_radix4_f32(float32_t *src, uint32_t m);
int32_t nds32_cfft_radix4_q15(q15_t *src, uint32_t m);
int32_t nds32_cifft_radix4_q15(q15_t *src, uint32_t m);
int32_t nds32_cfft_radix4_q31(q31_t *src, uint32_t m);
int32_t nds32_cifft_radix4_q31(q31_t *src, uint32_t m);

#define nds32_cfft_rd4_f32 nds32_cfft_radix4_f32
#define nds32_cifft_rd4_f32 nds32_cifft_radix4_f32
#define nds32_cfft_rd4_q15 nds32_cfft_radix4_q15
#define nds32_cifft_rd4_q15 nds32_cifft_radix4_q15
#define nds32_cfft_rd4_q31 nds32_cfft_radix4_q31
#define nds32_cifft_rd4_q31 nds32_cifft_radix4_q31

// CFFT
void nds32_cfft_f32(float32_t *src, uint32_t m);
void nds32_cfft_q31(q31_t *src, uint32_t m);
void nds32_cfft_q15(q15_t *src, uint32_t m);

// CIFFT
void nds32_cifft_f32(float32_t *src, uint32_t m);
void nds32_cifft_q31(q31_t *src, uint32_t m);
void nds32_cifft_q15(q15_t *src, uint32_t m);

// Real-FFT
int32_t nds32_rfft_f32(float32_t *src, uint32_t m);
int32_t nds32_rifft_f32(float32_t *src, uint32_t m);
int32_t nds32_rfft_q15(q15_t *src, uint32_t m);
int32_t nds32_rifft_q15(q15_t *src, uint32_t m);
int32_t nds32_rfft_q31(q31_t *src, uint32_t m);
int32_t nds32_rifft_q31(q31_t *src, uint32_t m);

// Real-FFT Fast
int32_t nds32_rfft_fast_f32(float32_t *src, uint32_t m);
int32_t nds32_rifft_fast_f32(float32_t *src, uint32_t m);
int32_t nds32_rfft_fast_q15(q15_t *src, uint32_t m);
int32_t nds32_rifft_fast_q15(q15_t *src, uint32_t m);
int32_t nds32_rfft_fast_q31(q31_t *src, uint32_t m);
int32_t nds32_rifft_fast_q31(q31_t *src, uint32_t m);

// DCT Type II
void nds32_dct_f32(float32_t *src, uint32_t m);
void nds32_dct_q31(q31_t *src, uint32_t m);
void nds32_dct_q15(q15_t *src, uint32_t m);

// IDCT Type II
void nds32_idct_f32(float32_t *src, uint32_t m);
void nds32_idct_q31(q31_t *src, uint32_t m);
void nds32_idct_q15(q15_t *src, uint32_t m);

// DCT Type IV
void nds32_dct4_f32(float32_t *src, uint32_t m);
void nds32_dct4_q15(q15_t *src, uint32_t m);
void nds32_dct4_q31(q31_t *src, uint32_t m);

// IDCT Type IV
void nds32_idct4_f32(float32_t *src, uint32_t m);
void nds32_idct4_q15(q15_t *src, uint32_t m);
void nds32_idct4_q31(q31_t *src, uint32_t m);
#ifdef  __cplusplus
}
#endif
#endif // __NDS32_TRANSFORM_MATH_H__
