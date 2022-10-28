/* ----------------------------------------------------------------------
 * Project:      CMSIS DSP Library
 * Title:        arm_fir_q15.c
 * Description:  Q15 FIR filter processing function
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
  @ingroup groupFilters
 */

/**
  @addtogroup FIR
  @{
 */

/**
  @brief         Processing function for the Q15 FIR filter.
  @param[in]     S          points to an instance of the Q15 FIR filter structure
  @param[in]     pSrc       points to the block of input data
  @param[out]    pDst       points to the block of output data
  @param[in]     blockSize  number of samples to process
  @return        none

  @par           Scaling and Overflow Behavior
                   The function is implemented using a 64-bit internal accumulator.
                   Both coefficients and state variables are represented in 1.15 format and multiplications yield a 2.30 result.
                   The 2.30 intermediate results are accumulated in a 64-bit accumulator in 34.30 format.
                   There is no risk of internal overflow with this approach and the full precision of intermediate multiplications is preserved.
                   After all additions have been performed, the accumulator is truncated to 34.15 format by discarding low 15 bits.
                   Lastly, the accumulator is saturated to yield a result in 1.15 format.

  @remark
                   Refer to \ref arm_fir_fast_q15() for a faster but less precise implementation of this function.
 */

void arm_fir_q15(
  const arm_fir_instance_q15 * S,
  const q15_t * pSrc,
        q15_t * pDst,
        uint32_t blockSize)
{
        q15_t *pState = S->pState;                     /* State pointer */
  const q15_t *pCoeffs = S->pCoeffs;                   /* Coefficient pointer */
        q15_t *pStateCurnt;                            /* Points to the current sample of the state */
        q15_t *px;                                     /* Temporary pointer for state buffer */
  const q15_t *pb;                                     /* Temporary pointer for coefficient buffer */
        q63_t acc0;                                    /* Accumulators */
        uint32_t numTaps = S->numTaps;                 /* Number of filter coefficients in the filter */
        uint32_t tapCnt, blkCnt;                       /* Loop counters */

#if defined (ARM_MATH_LOOPUNROLL)
        q63_t acc1, acc2, acc3;                        /* Accumulators */
        q31_t x0, x1, x2, c0;                          /* Temporary variables to hold state and coefficient values */
#endif

  /* S->pState points to state array which contains previous frame (numTaps - 1) samples */
  /* pStateCurnt points to the location where the new input data should be written */
  pStateCurnt = &(S->pState[(numTaps - 1U)]);

#if defined (ARM_MATH_LOOPUNROLL)

  /* Loop unrolling: Compute 4 output values simultaneously.
   * The variables acc0 ... acc3 hold output values that are being computed:
   *
   *    acc0 =  b[numTaps-1] * x[n-numTaps-1] + b[numTaps-2] * x[n-numTaps-2] + b[numTaps-3] * x[n-numTaps-3] +...+ b[0] * x[0]
   *    acc1 =  b[numTaps-1] * x[n-numTaps]   + b[numTaps-2] * x[n-numTaps-1] + b[numTaps-3] * x[n-numTaps-2] +...+ b[0] * x[1]
   *    acc2 =  b[numTaps-1] * x[n-numTaps+1] + b[numTaps-2] * x[n-numTaps]   + b[numTaps-3] * x[n-numTaps-1] +...+ b[0] * x[2]
   *    acc3 =  b[numTaps-1] * x[n-numTaps+2] + b[numTaps-2] * x[n-numTaps+1] + b[numTaps-3] * x[n-numTaps]   +...+ b[0] * x[3]
   */
  blkCnt = blockSize >> 2U;

  while (blkCnt > 0U)
  {
    /* Copy 4 new input samples into the state buffer. */
    *pStateCurnt++ = *pSrc++;
    *pStateCurnt++ = *pSrc++;
    *pStateCurnt++ = *pSrc++;
    *pStateCurnt++ = *pSrc++;

    /* Set all accumulators to zero */
    acc0 = 0;
    acc1 = 0;
    acc2 = 0;
    acc3 = 0;

    /* Typecast q15_t pointer to q31_t pointer for state reading in q31_t */
    px = pState;

    /* Typecast q15_t pointer to q31_t pointer for coefficient reading in q31_t */
    pb = pCoeffs;

    /* Read the first two samples from the state buffer:  x[n-N], x[n-N-1] */
    x0 = read_q15x2_ia (&px);

    /* Read the third and forth samples from the state buffer: x[n-N-2], x[n-N-3] */
    x2 = read_q15x2_ia (&px);

    /* Loop over the number of taps.  Unroll by a factor of 4.
       Repeat until we've computed numTaps-(numTaps%4) coefficients. */
    tapCnt = numTaps >> 2U;

    while (tapCnt > 0U)
    {
      /* Read the first two coefficients using SIMD:  b[N] and b[N-1] coefficients */
      c0 = read_q15x2_ia ((q15_t **) &pb);

      /* acc0 +=  b[N] * x[n-N] + b[N-1] * x[n-N-1] */
      acc0 = __SMLALD(x0, c0, acc0);

      /* acc2 +=  b[N] * x[n-N-2] + b[N-1] * x[n-N-3] */
      acc2 = __SMLALD(x2, c0, acc2);

      /* pack  x[n-N-1] and x[n-N-2] */
#ifndef ARM_MATH_BIG_ENDIAN
      x1 = __PKHBT(x2, x0, 0);
#else
      x1 = __PKHBT(x0, x2, 0);
#endif

      /* Read state x[n-N-4], x[n-N-5] */
      x0 = read_q15x2_ia (&px);

      /* acc1 +=  b[N] * x[n-N-1] + b[N-1] * x[n-N-2] */
      acc1 = __SMLALDX(x1, c0, acc1);

      /* pack  x[n-N-3] and x[n-N-4] */
#ifndef ARM_MATH_BIG_ENDIAN
      x1 = __PKHBT(x0, x2, 0);
#else
      x1 = __PKHBT(x2, x0, 0);
#endif

      /* acc3 +=  b[N] * x[n-N-3] + b[N-1] * x[n-N-4] */
      acc3 = __SMLALDX(x1, c0, acc3);

      /* Read coefficients b[N-2], b[N-3] */
      c0 = read_q15x2_ia ((q15_t **) &pb);

      /* acc0 +=  b[N-2] * x[n-N-2] + b[N-3] * x[n-N-3] */
      acc0 = __SMLALD(x2, c0, acc0);

      /* Read state x[n-N-6], x[n-N-7] with offset */
      x2 = read_q15x2_ia (&px);

      /* acc2 +=  b[N-2] * x[n-N-4] + b[N-3] * x[n-N-5] */
      acc2 = __SMLALD(x0, c0, acc2);

      /* acc1 +=  b[N-2] * x[n-N-3] + b[N-3] * x[n-N-4] */
      acc1 = __SMLALDX(x1, c0, acc1);

      /* pack  x[n-N-5] and x[n-N-6] */
#ifndef ARM_MATH_BIG_ENDIAN
      x1 = __PKHBT(x2, x0, 0);
#else
      x1 = __PKHBT(x0, x2, 0);
#endif

      /* acc3 +=  b[N-2] * x[n-N-5] + b[N-3] * x[n-N-6] */
      acc3 = __SMLALDX(x1, c0, acc3);

      /* Decrement tap count */
      tapCnt--;
    }

    /* If the filter length is not a multiple of 4, compute the remaining filter taps.
       This is always be 2 taps since the filter length is even. */
    if ((numTaps & 0x3U) != 0U)
    {
      /* Read last two coefficients */
      c0 = read_q15x2_ia ((q15_t **) &pb);

      /* Perform the multiply-accumulates */
      acc0 = __SMLALD(x0, c0, acc0);
      acc2 = __SMLALD(x2, c0, acc2);

      /* pack state variables */
#ifndef ARM_MATH_BIG_ENDIAN
      x1 = __PKHBT(x2, x0, 0);
#else
      x1 = __PKHBT(x0, x2, 0);
#endif

      /* Read last state variables */
      x0 = read_q15x2 (px);

      /* Perform the multiply-accumulates */
      acc1 = __SMLALDX(x1, c0, acc1);

      /* pack state variables */
#ifndef ARM_MATH_BIG_ENDIAN
      x1 = __PKHBT(x0, x2, 0);
#else
      x1 = __PKHBT(x2, x0, 0);
#endif

      /* Perform the multiply-accumulates */
      acc3 = __SMLALDX(x1, c0, acc3);
    }

    /* The results in the 4 accumulators are in 2.30 format. Convert to 1.15 with saturation.
       Then store the 4 outputs in the destination buffer. */
#ifndef ARM_MATH_BIG_ENDIAN
    write_q15x2_ia (&pDst, __PKHBT(__SSAT((acc0 >> 15), 16), __SSAT((acc1 >> 15), 16), 16));
    write_q15x2_ia (&pDst, __PKHBT(__SSAT((acc2 >> 15), 16), __SSAT((acc3 >> 15), 16), 16));
#else
    write_q15x2_ia (&pDst, __PKHBT(__SSAT((acc1 >> 15), 16), __SSAT((acc0 >> 15), 16), 16));
    write_q15x2_ia (&pDst, __PKHBT(__SSAT((acc3 >> 15), 16), __SSAT((acc2 >> 15), 16), 16));
#endif /* #ifndef ARM_MATH_BIG_ENDIAN */

    /* Advance the state pointer by 4 to process the next group of 4 samples */
    pState = pState + 4U;

    /* Decrement loop counter */
    blkCnt--;
  }

  /* Loop unrolling: Compute remaining output samples */
  blkCnt = blockSize % 0x4U;

#else

  /* Initialize blkCnt with number of taps */
  blkCnt = blockSize;

#endif /* #if defined (ARM_MATH_LOOPUNROLL) */

  while (blkCnt > 0U)
  {
    /* Copy two samples into state buffer */
    *pStateCurnt++ = *pSrc++;

    /* Set the accumulator to zero */
    acc0 = 0;

    /* Use SIMD to hold states and coefficients */
    px = pState;
    pb = pCoeffs;

    tapCnt = numTaps >> 1U;

    do
    {
      acc0 += (q31_t) *px++ * *pb++;
	  acc0 += (q31_t) *px++ * *pb++;

      tapCnt--;
    }
    while (tapCnt > 0U);

    /* The result is in 2.30 format. Convert to 1.15 with saturation.
       Then store the output in the destination buffer. */
    *pDst++ = (q15_t) (__SSAT((acc0 >> 15), 16));

    /* Advance state pointer by 1 for the next sample */
    pState = pState + 1U;

    /* Decrement loop counter */
    blkCnt--;
  }

  /* Processing is complete.
     Now copy the last numTaps - 1 samples to the start of the state buffer.
     This prepares the state buffer for the next function call. */

  /* Points to the start of the state buffer */
  pStateCurnt = S->pState;

#if defined (ARM_MATH_LOOPUNROLL)

  /* Loop unrolling: Compute 4 taps at a time */
  tapCnt = (numTaps - 1U) >> 2U;

  /* Copy data */
  while (tapCnt > 0U)
  {
    *pStateCurnt++ = *pState++;
    *pStateCurnt++ = *pState++;
    *pStateCurnt++ = *pState++;
    *pStateCurnt++ = *pState++;

    /* Decrement loop counter */
    tapCnt--;
  }

  /* Calculate remaining number of copies */
  tapCnt = (numTaps - 1U) % 0x4U;

#else

  /* Initialize tapCnt with number of taps */
  tapCnt = (numTaps - 1U);

#endif /* #if defined (ARM_MATH_LOOPUNROLL) */

  /* Copy remaining data */
  while (tapCnt > 0U)
  {
    *pStateCurnt++ = *pState++;

    /* Decrement loop counter */
    tapCnt--;
  }

}

/**
  @} end of FIR group
 */
