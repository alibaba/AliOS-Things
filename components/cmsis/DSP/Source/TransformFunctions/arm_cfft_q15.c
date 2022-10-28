/* ----------------------------------------------------------------------
 * Project:      CMSIS DSP Library
 * Title:        arm_cfft_q15.c
 * Description:  Combined Radix Decimation in Q15 Frequency CFFT processing function
 *
 * $Date:        18. March 2019
 * $Revision:    V1.6.0
 *
 * Target Processor: Cortex-M cores
 * -------------------------------------------------------------------- */
/*
 * Copyright (C) 2010-2019 ARM Limited or its affiliates. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "arm_math.h"

extern void arm_radix4_butterfly_q15(
        q15_t * pSrc,
        uint32_t fftLen,
  const q15_t * pCoef,
        uint32_t twidCoefModifier);

extern void arm_radix4_butterfly_inverse_q15(
        q15_t * pSrc,
        uint32_t fftLen,
  const q15_t * pCoef,
        uint32_t twidCoefModifier);

extern void arm_bitreversal_16(
        uint16_t * pSrc,
  const uint16_t bitRevLen,
  const uint16_t * pBitRevTable);

void arm_cfft_radix4by2_q15(
        q15_t * pSrc,
        uint32_t fftLen,
  const q15_t * pCoef);

void arm_cfft_radix4by2_inverse_q15(
        q15_t * pSrc,
        uint32_t fftLen,
  const q15_t * pCoef);

/**
  @ingroup groupTransforms
 */

/**
  @addtogroup ComplexFFT
  @{
 */

/**
  @brief         Processing function for Q15 complex FFT.
  @param[in]     S               points to an instance of Q15 CFFT structure
  @param[in,out] p1              points to the complex data buffer of size <code>2*fftLen</code>. Processing occurs in-place
  @param[in]     ifftFlag       flag that selects transform direction
                   - value = 0: forward transform
                   - value = 1: inverse transform
  @param[in]     bitReverseFlag flag that enables / disables bit reversal of output
                   - value = 0: disables bit reversal of output
                   - value = 1: enables bit reversal of output
  @return        none
 */

void arm_cfft_q15(
  const arm_cfft_instance_q15 * S,
        q15_t * p1,
        uint8_t ifftFlag,
        uint8_t bitReverseFlag)
{
  uint32_t L = S->fftLen;

  if (ifftFlag == 1U)
  {
     switch (L)
     {
     case 16:
     case 64:
     case 256:
     case 1024:
     case 4096:
       arm_radix4_butterfly_inverse_q15 ( p1, L, (q15_t*)S->pTwiddle, 1 );
       break;

     case 32:
     case 128:
     case 512:
     case 2048:
       arm_cfft_radix4by2_inverse_q15 ( p1, L, S->pTwiddle );
       break;
     }
  }
  else
  {
     switch (L)
     {
     case 16:
     case 64:
     case 256:
     case 1024:
     case 4096:
       arm_radix4_butterfly_q15  ( p1, L, (q15_t*)S->pTwiddle, 1 );
       break;

     case 32:
     case 128:
     case 512:
     case 2048:
       arm_cfft_radix4by2_q15  ( p1, L, S->pTwiddle );
       break;
     }
  }

  if ( bitReverseFlag )
    arm_bitreversal_16 ((uint16_t*) p1, S->bitRevLength, S->pBitRevTable);
}

/**
  @} end of ComplexFFT group
 */

void arm_cfft_radix4by2_q15(
        q15_t * pSrc,
        uint32_t fftLen,
  const q15_t * pCoef)
{
        uint32_t i;
        uint32_t n2;
        q15_t p0, p1, p2, p3;
#if defined (ARM_MATH_DSP)
        q31_t T, S, R;
        q31_t coeff, out1, out2;
  const q15_t *pC = pCoef;
        q15_t *pSi = pSrc;
        q15_t *pSl = pSrc + fftLen;
#else
        uint32_t l;
        q15_t xt, yt, cosVal, sinVal;
#endif

  n2 = fftLen >> 1U;

#if defined (ARM_MATH_DSP)

  for (i = n2; i > 0; i--)
  {
      coeff = read_q15x2_ia ((q15_t **) &pC);

      T = read_q15x2 (pSi);
      T = __SHADD16(T, 0); /* this is just a SIMD arithmetic shift right by 1 */

      S = read_q15x2 (pSl);
      S = __SHADD16(S, 0); /* this is just a SIMD arithmetic shift right by 1 */

      R = __QSUB16(T, S);

      write_q15x2_ia (&pSi, __SHADD16(T, S));

#ifndef ARM_MATH_BIG_ENDIAN
      out1 = __SMUAD(coeff, R) >> 16U;
      out2 = __SMUSDX(coeff, R);
#else
      out1 = __SMUSDX(R, coeff) >> 16U;
      out2 = __SMUAD(coeff, R);
#endif /* #ifndef ARM_MATH_BIG_ENDIAN */

      write_q15x2_ia (&pSl, (q31_t) ((out2) & 0xFFFF0000) | (out1 & 0x0000FFFF));
  }

#else /* #if defined (ARM_MATH_DSP) */

  for (i = 0; i < n2; i++)
  {
     cosVal = pCoef[2 * i];
     sinVal = pCoef[2 * i + 1];

     l = i + n2;

     xt =           (pSrc[2 * i] >> 1U) - (pSrc[2 * l] >> 1U);
     pSrc[2 * i] = ((pSrc[2 * i] >> 1U) + (pSrc[2 * l] >> 1U)) >> 1U;

     yt =               (pSrc[2 * i + 1] >> 1U) - (pSrc[2 * l + 1] >> 1U);
     pSrc[2 * i + 1] = ((pSrc[2 * l + 1] >> 1U) + (pSrc[2 * i + 1] >> 1U)) >> 1U;

     pSrc[2 * l]     = (((int16_t) (((q31_t) xt * cosVal) >> 16U)) +
                        ((int16_t) (((q31_t) yt * sinVal) >> 16U))  );

     pSrc[2 * l + 1] = (((int16_t) (((q31_t) yt * cosVal) >> 16U)) -
                        ((int16_t) (((q31_t) xt * sinVal) >> 16U))   );
  }

#endif /* #if defined (ARM_MATH_DSP) */

  /* first col */
  arm_radix4_butterfly_q15( pSrc,          n2, (q15_t*)pCoef, 2U);

  /* second col */
  arm_radix4_butterfly_q15( pSrc + fftLen, n2, (q15_t*)pCoef, 2U);

  n2 = fftLen >> 1U;
  for (i = 0; i < n2; i++)
  {
     p0 = pSrc[4 * i + 0];
     p1 = pSrc[4 * i + 1];
     p2 = pSrc[4 * i + 2];
     p3 = pSrc[4 * i + 3];

     p0 <<= 1U;
     p1 <<= 1U;
     p2 <<= 1U;
     p3 <<= 1U;

     pSrc[4 * i + 0] = p0;
     pSrc[4 * i + 1] = p1;
     pSrc[4 * i + 2] = p2;
     pSrc[4 * i + 3] = p3;
  }

}

void arm_cfft_radix4by2_inverse_q15(
        q15_t * pSrc,
        uint32_t fftLen,
  const q15_t * pCoef)
{
        uint32_t i;
        uint32_t n2;
        q15_t p0, p1, p2, p3;
#if defined (ARM_MATH_DSP)
        q31_t T, S, R;
        q31_t coeff, out1, out2;
  const q15_t *pC = pCoef;
        q15_t *pSi = pSrc;
        q15_t *pSl = pSrc + fftLen;
#else
        uint32_t l;
        q15_t xt, yt, cosVal, sinVal;
#endif

  n2 = fftLen >> 1U;

#if defined (ARM_MATH_DSP)

  for (i = n2; i > 0; i--)
  {
     coeff = read_q15x2_ia ((q15_t **) &pC);

     T = read_q15x2 (pSi);
     T = __SHADD16(T, 0); /* this is just a SIMD arithmetic shift right by 1 */

     S = read_q15x2 (pSl);
     S = __SHADD16(S, 0); /* this is just a SIMD arithmetic shift right by 1 */

     R = __QSUB16(T, S);

     write_q15x2_ia (&pSi, __SHADD16(T, S));

#ifndef ARM_MATH_BIG_ENDIAN
     out1 = __SMUSD(coeff, R) >> 16U;
     out2 = __SMUADX(coeff, R);
#else
     out1 = __SMUADX(R, coeff) >> 16U;
     out2 = __SMUSD(__QSUB(0, coeff), R);
#endif /* #ifndef ARM_MATH_BIG_ENDIAN */

     write_q15x2_ia (&pSl, (q31_t) ((out2) & 0xFFFF0000) | (out1 & 0x0000FFFF));
  }

#else /* #if defined (ARM_MATH_DSP) */

  for (i = 0; i < n2; i++)
  {
     cosVal = pCoef[2 * i];
     sinVal = pCoef[2 * i + 1];

     l = i + n2;

     xt =           (pSrc[2 * i] >> 1U) - (pSrc[2 * l] >> 1U);
     pSrc[2 * i] = ((pSrc[2 * i] >> 1U) + (pSrc[2 * l] >> 1U)) >> 1U;

     yt =               (pSrc[2 * i + 1] >> 1U) - (pSrc[2 * l + 1] >> 1U);
     pSrc[2 * i + 1] = ((pSrc[2 * l + 1] >> 1U) + (pSrc[2 * i + 1] >> 1U)) >> 1U;

     pSrc[2 * l]      = (((int16_t) (((q31_t) xt * cosVal) >> 16U)) -
                         ((int16_t) (((q31_t) yt * sinVal) >> 16U))  );

     pSrc[2 * l + 1] = (((int16_t) (((q31_t) yt * cosVal) >> 16U)) +
                        ((int16_t) (((q31_t) xt * sinVal) >> 16U))  );
  }

#endif /* #if defined (ARM_MATH_DSP) */

  /* first col */
  arm_radix4_butterfly_inverse_q15( pSrc,          n2, (q15_t*)pCoef, 2U);

  /* second col */
  arm_radix4_butterfly_inverse_q15( pSrc + fftLen, n2, (q15_t*)pCoef, 2U);

  n2 = fftLen >> 1U;
  for (i = 0; i < n2; i++)
  {
     p0 = pSrc[4 * i + 0];
     p1 = pSrc[4 * i + 1];
     p2 = pSrc[4 * i + 2];
     p3 = pSrc[4 * i + 3];

     p0 <<= 1U;
     p1 <<= 1U;
     p2 <<= 1U;
     p3 <<= 1U;

     pSrc[4 * i + 0] = p0;
     pSrc[4 * i + 1] = p1;
     pSrc[4 * i + 2] = p2;
     pSrc[4 * i + 3] = p3;
  }
}
