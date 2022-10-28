/* ----------------------------------------------------------------------
 * Project:      CMSIS DSP Library
 * Title:        arm_conv_partial_q31.c
 * Description:  Partial convolution of Q31 sequences
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
  @addtogroup PartialConv
  @{
 */

/**
  @brief         Partial convolution of Q31 sequences.
  @param[in]     pSrcA      points to the first input sequence
  @param[in]     srcALen    length of the first input sequence
  @param[in]     pSrcB      points to the second input sequence
  @param[in]     srcBLen    length of the second input sequence
  @param[out]    pDst       points to the location where the output result is written
  @param[in]     firstIndex is the first output sample to start with
  @param[in]     numPoints  is the number of output points to be computed
  @return        execution status
                   - \ref ARM_MATH_SUCCESS        : Operation successful
                   - \ref ARM_MATH_ARGUMENT_ERROR : requested subset is not in the range [0 srcALen+srcBLen-2]

  @remark
                   Refer to \ref arm_conv_partial_fast_q31() for a faster but less precise implementation of this function.
 */

arm_status arm_conv_partial_q31(
  const q31_t * pSrcA,
        uint32_t srcALen,
  const q31_t * pSrcB,
        uint32_t srcBLen,
        q31_t * pDst,
        uint32_t firstIndex,
        uint32_t numPoints)
{

#if (1)
//#if !defined(ARM_MATH_CM0_FAMILY)

  const q31_t *pIn1;                                   /* InputA pointer */
  const q31_t *pIn2;                                   /* InputB pointer */
        q31_t *pOut = pDst;                            /* Output pointer */
  const q31_t *px;                                     /* Intermediate inputA pointer */
  const q31_t *py;                                     /* Intermediate inputB pointer */
  const q31_t *pSrc1, *pSrc2;                          /* Intermediate pointers */
        q63_t sum;                                     /* Accumulator */
        uint32_t j, k, count, blkCnt, check;
        int32_t blockSize1, blockSize2, blockSize3;    /* Loop counters */
        arm_status status;                             /* Status of Partial convolution */

#if defined (ARM_MATH_LOOPUNROLL)
        q63_t acc0, acc1, acc2;                        /* Accumulator */
        q31_t x0, x1, x2, c0;                          /* Temporary variables */
#endif

  /* Check for range of output samples to be calculated */
  if ((firstIndex + numPoints) > ((srcALen + (srcBLen - 1U))))
  {
    /* Set status as ARM_MATH_ARGUMENT_ERROR */
    status = ARM_MATH_ARGUMENT_ERROR;
  }
  else
  {
    /* The algorithm implementation is based on the lengths of the inputs. */
    /* srcB is always made to slide across srcA. */
    /* So srcBLen is always considered as shorter or equal to srcALen */
    if (srcALen >= srcBLen)
    {
      /* Initialization of inputA pointer */
      pIn1 = pSrcA;

      /* Initialization of inputB pointer */
      pIn2 = pSrcB;
    }
    else
    {
      /* Initialization of inputA pointer */
      pIn1 = pSrcB;

      /* Initialization of inputB pointer */
      pIn2 = pSrcA;

      /* srcBLen is always considered as shorter or equal to srcALen */
      j = srcBLen;
      srcBLen = srcALen;
      srcALen = j;
    }

    /* Conditions to check which loopCounter holds
     * the first and last indices of the output samples to be calculated. */
    check = firstIndex + numPoints;
    blockSize3 = ((int32_t)check > (int32_t)srcALen) ? (int32_t)check - (int32_t)srcALen : 0;
    blockSize3 = ((int32_t)firstIndex > (int32_t)srcALen - 1) ? blockSize3 - (int32_t)firstIndex + (int32_t)srcALen : blockSize3;
    blockSize1 = ((int32_t) srcBLen - 1) - (int32_t) firstIndex;
    blockSize1 = (blockSize1 > 0) ? ((check > (srcBLen - 1U)) ? blockSize1 : (int32_t) numPoints) : 0;
    blockSize2 = (int32_t) check - ((blockSize3 + blockSize1) + (int32_t) firstIndex);
    blockSize2 = (blockSize2 > 0) ? blockSize2 : 0;

    /* conv(x,y) at n = x[n] * y[0] + x[n-1] * y[1] + x[n-2] * y[2] + ...+ x[n-N+1] * y[N -1] */
    /* The function is internally
     * divided into three stages according to the number of multiplications that has to be
     * taken place between inputA samples and inputB samples. In the first stage of the
     * algorithm, the multiplications increase by one for every iteration.
     * In the second stage of the algorithm, srcBLen number of multiplications are done.
     * In the third stage of the algorithm, the multiplications decrease by one
     * for every iteration. */

    /* Set the output pointer to point to the firstIndex
     * of the output sample to be calculated. */
    pOut = pDst + firstIndex;

    /* --------------------------
     * Initializations of stage1
     * -------------------------*/

    /* sum = x[0] * y[0]
     * sum = x[0] * y[1] + x[1] * y[0]
     * ....
     * sum = x[0] * y[srcBlen - 1] + x[1] * y[srcBlen - 2] +...+ x[srcBLen - 1] * y[0]
     */

    /* In this stage the MAC operations are increased by 1 for every iteration.
       The count variable holds the number of MAC operations performed.
       Since the partial convolution starts from firstIndex
       Number of Macs to be performed is firstIndex + 1 */
    count = 1U + firstIndex;

    /* Working pointer of inputA */
    px = pIn1;

    /* Working pointer of inputB */
    pSrc2 = pIn2 + firstIndex;
    py = pSrc2;

    /* ------------------------
     * Stage1 process
     * ----------------------*/

    /* The first stage starts here */
    while (blockSize1 > 0U)
    {
      /* Accumulator is made zero for every iteration */
      sum = 0;

#if defined (ARM_MATH_LOOPUNROLL)

      /* Loop unrolling: Compute 4 outputs at a time */
      k = count >> 2U;

      while (k > 0U)
      {
        /* x[0] * y[srcBLen - 1] */
        sum += (q63_t) *px++ * (*py--);

        /* x[1] * y[srcBLen - 2] */
        sum += (q63_t) *px++ * (*py--);

        /* x[2] * y[srcBLen - 3] */
        sum += (q63_t) *px++ * (*py--);

        /* x[3] * y[srcBLen - 4] */
        sum += (q63_t) *px++ * (*py--);

        /* Decrement loop counter */
        k--;
      }

      /* Loop unrolling: Compute remaining outputs */
      k = count % 0x4U;

#else

      /* Initialize k with number of samples */
      k = count;

#endif /* #if defined (ARM_MATH_LOOPUNROLL) */

      while (k > 0U)
      {
        /* Perform the multiply-accumulate */
        sum += (q63_t) *px++ * (*py--);

        /* Decrement loop counter */
        k--;
      }

      /* Store the result in the accumulator in the destination buffer. */
      *pOut++ = (q31_t) (sum >> 31);

      /* Update the inputA and inputB pointers for next MAC calculation */
      py = ++pSrc2;
      px = pIn1;

      /* Increment MAC count */
      count++;

      /* Decrement loop counter */
      blockSize1--;
    }

    /* --------------------------
     * Initializations of stage2
     * ------------------------*/

    /* sum = x[0] * y[srcBLen-1] + x[1] * y[srcBLen-2] +...+ x[srcBLen-1] * y[0]
     * sum = x[1] * y[srcBLen-1] + x[2] * y[srcBLen-2] +...+ x[srcBLen] * y[0]
     * ....
     * sum = x[srcALen-srcBLen-2] * y[srcBLen-1] + x[srcALen] * y[srcBLen-2] +...+ x[srcALen-1] * y[0]
     */

    /* Working pointer of inputA */
    if ((int32_t)firstIndex - (int32_t)srcBLen + 1 > 0)
    {
      pSrc1 = pIn1 + firstIndex - srcBLen + 1;
    }
    else
    {
      pSrc1 = pIn1;
    }
    px = pSrc1;

    /* Working pointer of inputB */
    pSrc2 = pIn2 + (srcBLen - 1U);
    py = pSrc2;

    /* count is index by which the pointer pIn1 to be incremented */
    count = 0U;

    /* -------------------
     * Stage2 process
     * ------------------*/

    /* Stage2 depends on srcBLen as in this stage srcBLen number of MACS are performed.
     * So, to loop unroll over blockSize2,
     * srcBLen should be greater than or equal to 4 */
    if (srcBLen >= 4U)
    {
#if defined (ARM_MATH_LOOPUNROLL)

      /* Loop unroll over blkCnt */
      blkCnt = blockSize2 / 3;

      while (blkCnt > 0U)
      {
        /* Set all accumulators to zero */
        acc0 = 0;
        acc1 = 0;
        acc2 = 0;

        /* read x[0], x[1] samples */
        x0 = *px++;
        x1 = *px++;

        /* Apply loop unrolling and compute 3 MACs simultaneously. */
        k = srcBLen / 3;

        /* First part of the processing with loop unrolling.  Compute 3 MACs at a time.
         ** a second loop below computes MACs for the remaining 1 to 2 samples. */
        do
        {
          /* Read y[srcBLen - 1] sample */
          c0 = *(py);

          /* Read x[2] sample */
          x2 = *(px);

          /* Perform the multiply-accumulate */
          /* acc0 +=  x[0] * y[srcBLen - 1] */
          acc0 += (q63_t) x0 * c0;
          /* acc1 +=  x[1] * y[srcBLen - 1] */
          acc1 += (q63_t) x1 * c0;
          /* acc2 +=  x[2] * y[srcBLen - 1] */
          acc2 += (q63_t) x2 * c0;

          /* Read y[srcBLen - 2] sample */
          c0 = *(py - 1U);

          /* Read x[3] sample */
          x0 = *(px + 1U);

          /* Perform the multiply-accumulate */
          /* acc0 +=  x[1] * y[srcBLen - 2] */
          acc0 += (q63_t) x1 * c0;
          /* acc1 +=  x[2] * y[srcBLen - 2] */
          acc1 += (q63_t) x2 * c0;
          /* acc2 +=  x[3] * y[srcBLen - 2] */
          acc2 += (q63_t) x0 * c0;

          /* Read y[srcBLen - 3] sample */
          c0 = *(py - 2U);

          /* Read x[4] sample */
          x1 = *(px + 2U);

          /* Perform the multiply-accumulate */
          /* acc0 +=  x[2] * y[srcBLen - 3] */
          acc0 += (q63_t) x2 * c0;
          /* acc1 +=  x[3] * y[srcBLen - 2] */
          acc1 += (q63_t) x0 * c0;
          /* acc2 +=  x[4] * y[srcBLen - 2] */
          acc2 += (q63_t) x1 * c0;


          px += 3U;

          py -= 3U;

        } while (--k);

        /* If the srcBLen is not a multiple of 3, compute any remaining MACs here.
         ** No loop unrolling is used. */
        k = srcBLen - (3 * (srcBLen / 3));

        while (k > 0U)
        {
          /* Read y[srcBLen - 5] sample */
          c0 = *py--;
          /* Read x[7] sample */
          x2 = *px++;

          /* Perform the multiply-accumulates */
          /* acc0 +=  x[4] * y[srcBLen - 5] */
          acc0 += (q63_t) x0 * c0;
          /* acc1 +=  x[5] * y[srcBLen - 5] */
          acc1 += (q63_t) x1 * c0;
          /* acc2 +=  x[6] * y[srcBLen - 5] */
          acc2 += (q63_t) x2 * c0;

          /* Reuse the present samples for the next MAC */
          x0 = x1;
          x1 = x2;

          /* Decrement the loop counter */
          k--;
        }

        /* Store the result in the accumulator in the destination buffer. */
        *pOut++ = (q31_t) (acc0 >> 31);
        *pOut++ = (q31_t) (acc1 >> 31);
        *pOut++ = (q31_t) (acc2 >> 31);

        /* Increment the pointer pIn1 index, count by 3 */
        count += 3U;

        /* Update the inputA and inputB pointers for next MAC calculation */
        px = pSrc1 + count;
        py = pSrc2;

        /* Decrement loop counter */
        blkCnt--;
      }

      /* Loop unrolling: Compute remaining outputs */
      blkCnt = blockSize2 - 3 * (blockSize2 / 3);

#else

      /* Initialize blkCnt with number of samples */
      blkCnt = blockSize2;

#endif /* #if defined (ARM_MATH_LOOPUNROLL) */

      while (blkCnt > 0U)
      {
        /* Accumulator is made zero for every iteration */
        sum = 0;

#if defined (ARM_MATH_LOOPUNROLL)

        /* Loop unrolling: Compute 4 outputs at a time */
        k = srcBLen >> 2U;

        while (k > 0U)
        {
          /* Perform the multiply-accumulates */
          sum += (q63_t) *px++ * (*py--);
          sum += (q63_t) *px++ * (*py--);
          sum += (q63_t) *px++ * (*py--);
          sum += (q63_t) *px++ * (*py--);

          /* Decrement loop counter */
          k--;
        }

        /* Loop unrolling: Compute remaining outputs */
        k = srcBLen % 0x4U;

#else

        /* Initialize blkCnt with number of samples */
        k = srcBLen;

#endif /* #if defined (ARM_MATH_LOOPUNROLL) */

        while (k > 0U)
        {
          /* Perform the multiply-accumulate */
          sum += (q63_t) *px++ * *py--;

          /* Decrement loop counter */
          k--;
        }

        /* Store the result in the accumulator in the destination buffer. */
        *pOut++ = (q31_t) (sum >> 31);

        /* Increment MAC count */
        count++;

        /* Update the inputA and inputB pointers for next MAC calculation */
        px = pSrc1 + count;
        py = pSrc2;

        /* Decrement loop counter */
        blkCnt--;
      }
    }
    else
    {
      /* If the srcBLen is not a multiple of 4,
       * the blockSize2 loop cannot be unrolled by 4 */
      blkCnt = (uint32_t) blockSize2;

      while (blkCnt > 0U)
      {
        /* Accumulator is made zero for every iteration */
        sum = 0;

        /* srcBLen number of MACS should be performed */
        k = srcBLen;

        while (k > 0U)
        {
          /* Perform the multiply-accumulate */
          sum += (q63_t) *px++ * *py--;

          /* Decrement loop counter */
          k--;
        }

        /* Store the result in the accumulator in the destination buffer. */
        *pOut++ = (q31_t) (sum >> 31);

        /* Increment the MAC count */
        count++;

        /* Update the inputA and inputB pointers for next MAC calculation */
        px = pSrc1 + count;
        py = pSrc2;

        /* Decrement the loop counter */
        blkCnt--;
      }
    }


    /* --------------------------
     * Initializations of stage3
     * -------------------------*/

    /* sum += x[srcALen-srcBLen+1] * y[srcBLen-1] + x[srcALen-srcBLen+2] * y[srcBLen-2] +...+ x[srcALen-1] * y[1]
     * sum += x[srcALen-srcBLen+2] * y[srcBLen-1] + x[srcALen-srcBLen+3] * y[srcBLen-2] +...+ x[srcALen-1] * y[2]
     * ....
     * sum +=  x[srcALen-2] * y[srcBLen-1] + x[srcALen-1] * y[srcBLen-2]
     * sum +=  x[srcALen-1] * y[srcBLen-1]
     */

    /* In this stage the MAC operations are decreased by 1 for every iteration.
       The blockSize3 variable holds the number of MAC operations performed */
    count = srcBLen - 1U;

    /* Working pointer of inputA */
    pSrc1 = (pIn1 + srcALen) - (srcBLen - 1U);
    px = pSrc1;

    /* Working pointer of inputB */
    pSrc2 = pIn2 + (srcBLen - 1U);
    py = pSrc2;

    /* -------------------
     * Stage3 process
     * ------------------*/

    while (blockSize3 > 0U)
    {
      /* Accumulator is made zero for every iteration */
      sum = 0;

#if defined (ARM_MATH_LOOPUNROLL)

      /* Loop unrolling: Compute 4 outputs at a time */
      k = count >> 2U;

      while (k > 0U)
      {
        /* sum += x[srcALen - srcBLen + 1] * y[srcBLen - 1] */
        sum += (q63_t) *px++ * *py--;

        /* sum += x[srcALen - srcBLen + 2] * y[srcBLen - 2] */
        sum += (q63_t) *px++ * *py--;

        /* sum += x[srcALen - srcBLen + 3] * y[srcBLen - 3] */
        sum += (q63_t) *px++ * *py--;

        /* sum += x[srcALen - srcBLen + 4] * y[srcBLen - 4] */
        sum += (q63_t) *px++ * *py--;

        /* Decrement loop counter */
        k--;
      }

      /* Loop unrolling: Compute remaining outputs */
      k = count % 0x4U;

#else

      /* Initialize blkCnt with number of samples */
      k = count;

#endif /* #if defined (ARM_MATH_LOOPUNROLL) */

      while (k > 0U)
      {
        /* Perform the multiply-accumulate */
        /* sum +=  x[srcALen-1] * y[srcBLen-1] */
        sum += (q63_t) *px++ * *py--;

        /* Decrement loop counter */
        k--;
      }

      /* Store the result in the accumulator in the destination buffer. */
      *pOut++ = (q31_t) (sum >> 31);

      /* Update the inputA and inputB pointers for next MAC calculation */
      px = ++pSrc1;
      py = pSrc2;

      /* Decrement MAC count */
      count--;

      /* Decrement the loop counter */
      blockSize3--;
    }

    /* Set status as ARM_MATH_SUCCESS */
    status = ARM_MATH_SUCCESS;
  }

  /* Return to application */
  return (status);

#else
/* alternate version for CM0_FAMILY */

  const q31_t *pIn1 = pSrcA;                           /* InputA pointer */
  const q31_t *pIn2 = pSrcB;                           /* InputB pointer */
        q63_t sum;                                     /* Accumulator */
        uint32_t i, j;                                 /* Loop counters */
        arm_status status;                             /* Status of Partial convolution */

  /* Check for range of output samples to be calculated */
  if ((firstIndex + numPoints) > ((srcALen + (srcBLen - 1U))))
  {
    /* Set status as ARM_MATH_ARGUMENT_ERROR */
    status = ARM_MATH_ARGUMENT_ERROR;
  }
  else
  {
    /* Loop to calculate convolution for output length number of values */
    for (i = firstIndex; i <= (firstIndex + numPoints - 1); i++)
    {
      /* Initialize sum with zero to carry on MAC operations */
      sum = 0;

      /* Loop to perform MAC operations according to convolution equation */
      for (j = 0U; j <= i; j++)
      {
        /* Check the array limitations */
        if (((i - j) < srcBLen) && (j < srcALen))
        {
          /* z[i] += x[i-j] * y[j] */
          sum += ((q63_t) pIn1[j] * pIn2[i - j]);
        }
      }

      /* Store the output in the destination buffer */
      pDst[i] = (q31_t) (sum >> 31U);
    }

    /* Set status as ARM_MATH_SUCCESS */
    status = ARM_MATH_SUCCESS;
  }

  /* Return to application */
  return (status);

#endif /* #if !defined(ARM_MATH_CM0_FAMILY) */

}

/**
  @} end of PartialConv group
 */
