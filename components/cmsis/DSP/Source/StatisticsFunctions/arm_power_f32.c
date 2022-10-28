/* ----------------------------------------------------------------------
 * Project:      CMSIS DSP Library
 * Title:        arm_power_f32.c
 * Description:  Sum of the squares of the elements of a floating-point vector
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

/**
  @ingroup groupStats
 */

/**
  @defgroup power Power

  Calculates the sum of the squares of the elements in the input vector.
  The underlying algorithm is used:

  <pre>
      Result = pSrc[0] * pSrc[0] + pSrc[1] * pSrc[1] + pSrc[2] * pSrc[2] + ... + pSrc[blockSize-1] * pSrc[blockSize-1];
  </pre>

  There are separate functions for floating point, Q31, Q15, and Q7 data types.
 */

/**
  @addtogroup power
  @{
 */

/**
  @brief         Sum of the squares of the elements of a floating-point vector.
  @param[in]     pSrc       points to the input vector
  @param[in]     blockSize  number of samples in input vector
  @param[out]    pResult    sum of the squares value returned here
  @return        none
 */
#if defined(ARM_MATH_NEON)
void arm_power_f32(
  const float32_t * pSrc,
  uint32_t blockSize,
  float32_t * pResult)
{
  float32_t sum = 0.0f;                          /* accumulator */
  float32_t in;                                  /* Temporary variable to store input value */
  uint32_t blkCnt;                               /* loop counter */

  float32x4_t sumV = vdupq_n_f32(0.0f);                          /* Temporary result storage */
  float32x2_t sumV2;
  float32x4_t inV;

  blkCnt = blockSize >> 2U;

  /* Compute 4 outputs at a time.
   ** a second loop below computes the remaining 1 to 3 samples. */
  while (blkCnt > 0U)
  {
    /* C = A[0] * A[0] + A[1] * A[1] + A[2] * A[2] + ... + A[blockSize-1] * A[blockSize-1] */
    /* Compute Power and then store the result in a temporary variable, sum. */
    inV = vld1q_f32(pSrc);
    sumV = vmlaq_f32(sumV, inV, inV);
    pSrc += 4;

    /* Decrement the loop counter */
    blkCnt--;
  }
  sumV2 = vpadd_f32(vget_low_f32(sumV),vget_high_f32(sumV));
  sum = sumV2[0] + sumV2[1];

  /* If the blockSize is not a multiple of 4, compute any remaining output samples here.
   ** No loop unrolling is used. */
  blkCnt = blockSize % 0x4U;

  while (blkCnt > 0U)
  {
    /* C = A[0] * A[0] + A[1] * A[1] + A[2] * A[2] + ... + A[blockSize-1] * A[blockSize-1] */
    /* compute power and then store the result in a temporary variable, sum. */
    in = *pSrc++;
    sum += in * in;

    /* Decrement the loop counter */
    blkCnt--;
  }

  /* Store the result to the destination */
  *pResult = sum;
}
#else
void arm_power_f32(
  const float32_t * pSrc,
        uint32_t blockSize,
        float32_t * pResult)
{
        uint32_t blkCnt;                               /* Loop counter */
        float32_t sum = 0.0f;                          /* Temporary result storage */
        float32_t in;                                  /* Temporary variable to store input value */

#if defined (ARM_MATH_LOOPUNROLL)

  /* Loop unrolling: Compute 4 outputs at a time */
  blkCnt = blockSize >> 2U;

  while (blkCnt > 0U)
  {
    /* C = A[0] * A[0] + A[1] * A[1] + ... + A[blockSize-1] * A[blockSize-1] */

    /* Compute Power and store result in a temporary variable, sum. */
    in = *pSrc++;
    sum += in * in;

    in = *pSrc++;
    sum += in * in;

    in = *pSrc++;
    sum += in * in;

    in = *pSrc++;
    sum += in * in;

    /* Decrement loop counter */
    blkCnt--;
  }

  /* Loop unrolling: Compute remaining outputs */
  blkCnt = blockSize % 0x4U;

#else

  /* Initialize blkCnt with number of samples */
  blkCnt = blockSize;

#endif /* #if defined (ARM_MATH_LOOPUNROLL) */

  while (blkCnt > 0U)
  {
    /* C = A[0] * A[0] + A[1] * A[1] + ... + A[blockSize-1] * A[blockSize-1] */

    /* Compute Power and store result in a temporary variable, sum. */
    in = *pSrc++;
    sum += in * in;

    /* Decrement loop counter */
    blkCnt--;
  }

  /* Store result to destination */
  *pResult = sum;
}
#endif /* #if defined(ARM_MATH_NEON) */

/**
  @} end of power group
 */
