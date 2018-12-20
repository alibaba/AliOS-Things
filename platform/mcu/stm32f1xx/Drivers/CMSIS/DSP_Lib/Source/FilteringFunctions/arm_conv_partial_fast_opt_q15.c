/* ----------------------------------------------------------------------    
* Copyright (C) 2010-2014 ARM Limited. All rights reserved.    
*    
* $Date:        19. March 2015
* $Revision: 	V.1.4.5
*    
* Project: 	    CMSIS DSP Library    
* Title:		arm_conv_partial_fast_opt_q15.c    
*    
* Description:	Fast Q15 Partial convolution.    
*    
* Target Processor: Cortex-M4/Cortex-M3
*  
* Redistribution and use in source and binary forms, with or without 
* modification, are permitted provided that the following conditions
* are met:
*   - Redistributions of source code must retain the above copyright
*     notice, this list of conditions and the following disclaimer.
*   - Redistributions in binary form must reproduce the above copyright
*     notice, this list of conditions and the following disclaimer in
*     the documentation and/or other materials provided with the 
*     distribution.
*   - Neither the name of ARM LIMITED nor the names of its contributors
*     may be used to endorse or promote products derived from this
*     software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
* "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
* LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
* FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE 
* COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
* BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
* CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
* LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
* ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.     
* -------------------------------------------------------------------- */

#include "arm_math.h"

/**    
 * @ingroup groupFilters    
 */

/**    
 * @addtogroup PartialConv    
 * @{    
 */

/**    
 * @brief Partial convolution of Q15 sequences (fast version) for Cortex-M3 and Cortex-M4.    
 * @param[in]       *pSrcA points to the first input sequence.    
 * @param[in]       srcALen length of the first input sequence.    
 * @param[in]       *pSrcB points to the second input sequence.    
 * @param[in]       srcBLen length of the second input sequence.    
 * @param[out]      *pDst points to the location where the output result is written.    
 * @param[in]       firstIndex is the first output sample to start with.    
 * @param[in]       numPoints is the number of output points to be computed.    
 * @param[in]       *pScratch1 points to scratch buffer of size max(srcALen, srcBLen) + 2*min(srcALen, srcBLen) - 2.   
 * @param[in]       *pScratch2 points to scratch buffer of size min(srcALen, srcBLen).   
 * @return Returns either ARM_MATH_SUCCESS if the function completed correctly or ARM_MATH_ARGUMENT_ERROR if the requested subset is not in the range [0 srcALen+srcBLen-2].    
 *    
 * See <code>arm_conv_partial_q15()</code> for a slower implementation of this function which uses a 64-bit accumulator to avoid wrap around distortion.    
 *    
 * \par Restrictions    
 *  If the silicon does not support unaligned memory access enable the macro UNALIGNED_SUPPORT_DISABLE    
 *	In this case input, output, scratch1 and scratch2 buffers should be aligned by 32-bit    
 *     
 */

#ifndef UNALIGNED_SUPPORT_DISABLE

arm_status arm_conv_partial_fast_opt_q15(
  q15_t * pSrcA,
  uint32_t srcALen,
  q15_t * pSrcB,
  uint32_t srcBLen,
  q15_t * pDst,
  uint32_t firstIndex,
  uint32_t numPoints,
  q15_t * pScratch1,
  q15_t * pScratch2)
{

  q15_t *pOut = pDst;                            /* output pointer */
  q15_t *pScr1 = pScratch1;                      /* Temporary pointer for scratch1 */
  q15_t *pScr2 = pScratch2;                      /* Temporary pointer for scratch1 */
  q31_t acc0, acc1, acc2, acc3;                  /* Accumulator */
  q31_t x1, x2, x3;                              /* Temporary variables to hold state and coefficient values */
  q31_t y1, y2;                                  /* State variables */
  q15_t *pIn1;                                   /* inputA pointer */
  q15_t *pIn2;                                   /* inputB pointer */
  q15_t *px;                                     /* Intermediate inputA pointer  */
  q15_t *py;                                     /* Intermediate inputB pointer  */
  uint32_t j, k, blkCnt;                         /* loop counter */
  arm_status status;

  uint32_t tapCnt;                               /* loop count */

  /* Check for range of output samples to be calculated */
  if((firstIndex + numPoints) > ((srcALen + (srcBLen - 1u))))
  {
    /* Set status as ARM_MATH_ARGUMENT_ERROR */
    status = ARM_MATH_ARGUMENT_ERROR;
  }
  else
  {

    /* The algorithm implementation is based on the lengths of the inputs. */
    /* srcB is always made to slide across srcA. */
    /* So srcBLen is always considered as shorter or equal to srcALen */
    if(srcALen >= srcBLen)
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

    /* Temporary pointer for scratch2 */
    py = pScratch2;

    /* pointer to take end of scratch2 buffer */
    pScr2 = pScratch2 + srcBLen - 1;

    /* points to smaller length sequence */
    px = pIn2;

    /* Apply loop unrolling and do 4 Copies simultaneously. */
    k = srcBLen >> 2u;

    /* First part of the processing with loop unrolling copies 4 data points at a time.       
     ** a second loop below copies for the remaining 1 to 3 samples. */

    /* Copy smaller length input sequence in reverse order into second scratch buffer */
    while(k > 0u)
    {
      /* copy second buffer in reversal manner */
      *pScr2-- = *px++;
      *pScr2-- = *px++;
      *pScr2-- = *px++;
      *pScr2-- = *px++;

      /* Decrement the loop counter */
      k--;
    }

    /* If the count is not a multiple of 4, copy remaining samples here.       
     ** No loop unrolling is used. */
    k = srcBLen % 0x4u;

    while(k > 0u)
    {
      /* copy second buffer in reversal manner for remaining samples */
      *pScr2-- = *px++;

      /* Decrement the loop counter */
      k--;
    }

    /* Initialze temporary scratch pointer */
    pScr1 = pScratch1;

    /* Assuming scratch1 buffer is aligned by 32-bit */
    /* Fill (srcBLen - 1u) zeros in scratch buffer */
    arm_fill_q15(0, pScr1, (srcBLen - 1u));

    /* Update temporary scratch pointer */
    pScr1 += (srcBLen - 1u);

    /* Copy bigger length sequence(srcALen) samples in scratch1 buffer */

    /* Copy (srcALen) samples in scratch buffer */
    arm_copy_q15(pIn1, pScr1, srcALen);

    /* Update pointers */
    pScr1 += srcALen;

    /* Fill (srcBLen - 1u) zeros at end of scratch buffer */
    arm_fill_q15(0, pScr1, (srcBLen - 1u));

    /* Update pointer */
    pScr1 += (srcBLen - 1u);

    /* Initialization of pIn2 pointer */
    pIn2 = py;

    pScratch1 += firstIndex;

    pOut = pDst + firstIndex;

    /* First part of the processing with loop unrolling process 4 data points at a time.       
     ** a second loop below process for the remaining 1 to 3 samples. */

    /* Actual convolution process starts here */
    blkCnt = (numPoints) >> 2;

    while(blkCnt > 0)
    {
      /* Initialze temporary scratch pointer as scratch1 */
      pScr1 = pScratch1;

      /* Clear Accumlators */
      acc0 = 0;
      acc1 = 0;
      acc2 = 0;
      acc3 = 0;

      /* Read two samples from scratch1 buffer */
      x1 = *__SIMD32(pScr1)++;

      /* Read next two samples from scratch1 buffer */
      x2 = *__SIMD32(pScr1)++;

      tapCnt = (srcBLen) >> 2u;

      while(tapCnt > 0u)
      {

        /* Read four samples from smaller buffer */
        y1 = _SIMD32_OFFSET(pIn2);
        y2 = _SIMD32_OFFSET(pIn2 + 2u);

        /* multiply and accumlate */
        acc0 = __SMLAD(x1, y1, acc0);
        acc2 = __SMLAD(x2, y1, acc2);

        /* pack input data */
#ifndef ARM_MATH_BIG_ENDIAN
        x3 = __PKHBT(x2, x1, 0);
#else
        x3 = __PKHBT(x1, x2, 0);
#endif

        /* multiply and accumlate */
        acc1 = __SMLADX(x3, y1, acc1);

        /* Read next two samples from scratch1 buffer */
        x1 = _SIMD32_OFFSET(pScr1);

        /* multiply and accumlate */
        acc0 = __SMLAD(x2, y2, acc0);

        acc2 = __SMLAD(x1, y2, acc2);

        /* pack input data */
#ifndef ARM_MATH_BIG_ENDIAN
        x3 = __PKHBT(x1, x2, 0);
#else
        x3 = __PKHBT(x2, x1, 0);
#endif

        acc3 = __SMLADX(x3, y1, acc3);
        acc1 = __SMLADX(x3, y2, acc1);

        x2 = _SIMD32_OFFSET(pScr1 + 2u);

#ifndef ARM_MATH_BIG_ENDIAN
        x3 = __PKHBT(x2, x1, 0);
#else
        x3 = __PKHBT(x1, x2, 0);
#endif

        acc3 = __SMLADX(x3, y2, acc3);

        /* update scratch pointers */
        pIn2 += 4u;
        pScr1 += 4u;


        /* Decrement the loop counter */
        tapCnt--;
      }

      /* Update scratch pointer for remaining samples of smaller length sequence */
      pScr1 -= 4u;

      /* apply same above for remaining samples of smaller length sequence */
      tapCnt = (srcBLen) & 3u;

      while(tapCnt > 0u)
      {

        /* accumlate the results */
        acc0 += (*pScr1++ * *pIn2);
        acc1 += (*pScr1++ * *pIn2);
        acc2 += (*pScr1++ * *pIn2);
        acc3 += (*pScr1++ * *pIn2++);

        pScr1 -= 3u;

        /* Decrement the loop counter */
        tapCnt--;
      }

      blkCnt--;


      /* Store the results in the accumulators in the destination buffer. */

#ifndef  ARM_MATH_BIG_ENDIAN

      *__SIMD32(pOut)++ =
        __PKHBT(__SSAT((acc0 >> 15), 16), __SSAT((acc1 >> 15), 16), 16);
      *__SIMD32(pOut)++ =
        __PKHBT(__SSAT((acc2 >> 15), 16), __SSAT((acc3 >> 15), 16), 16);

#else

      *__SIMD32(pOut)++ =
        __PKHBT(__SSAT((acc1 >> 15), 16), __SSAT((acc0 >> 15), 16), 16);
      *__SIMD32(pOut)++ =
        __PKHBT(__SSAT((acc3 >> 15), 16), __SSAT((acc2 >> 15), 16), 16);

#endif /*      #ifndef  ARM_MATH_BIG_ENDIAN    */

      /* Initialization of inputB pointer */
      pIn2 = py;

      pScratch1 += 4u;

    }


    blkCnt = numPoints & 0x3;

    /* Calculate convolution for remaining samples of Bigger length sequence */
    while(blkCnt > 0)
    {
      /* Initialze temporary scratch pointer as scratch1 */
      pScr1 = pScratch1;

      /* Clear Accumlators */
      acc0 = 0;

      tapCnt = (srcBLen) >> 1u;

      while(tapCnt > 0u)
      {

        /* Read next two samples from scratch1 buffer */
        x1 = *__SIMD32(pScr1)++;

        /* Read two samples from smaller buffer */
        y1 = *__SIMD32(pIn2)++;

        acc0 = __SMLAD(x1, y1, acc0);

        /* Decrement the loop counter */
        tapCnt--;
      }

      tapCnt = (srcBLen) & 1u;

      /* apply same above for remaining samples of smaller length sequence */
      while(tapCnt > 0u)
      {

        /* accumlate the results */
        acc0 += (*pScr1++ * *pIn2++);

        /* Decrement the loop counter */
        tapCnt--;
      }

      blkCnt--;

      /* The result is in 2.30 format.  Convert to 1.15 with saturation.       
       ** Then store the output in the destination buffer. */
      *pOut++ = (q15_t) (__SSAT((acc0 >> 15), 16));

      /* Initialization of inputB pointer */
      pIn2 = py;

      pScratch1 += 1u;

    }
    /* set status as ARM_MATH_SUCCESS */
    status = ARM_MATH_SUCCESS;
  }
  /* Return to application */
  return (status);
}

#else

arm_status arm_conv_partial_fast_opt_q15(
  q15_t * pSrcA,
  uint32_t srcALen,
  q15_t * pSrcB,
  uint32_t srcBLen,
  q15_t * pDst,
  uint32_t firstIndex,
  uint32_t numPoints,
  q15_t * pScratch1,
  q15_t * pScratch2)
{

  q15_t *pOut = pDst;                            /* output pointer */
  q15_t *pScr1 = pScratch1;                      /* Temporary pointer for scratch1 */
  q15_t *pScr2 = pScratch2;                      /* Temporary pointer for scratch1 */
  q31_t acc0, acc1, acc2, acc3;                  /* Accumulator */
  q15_t *pIn1;                                   /* inputA pointer */
  q15_t *pIn2;                                   /* inputB pointer */
  q15_t *px;                                     /* Intermediate inputA pointer  */
  q15_t *py;                                     /* Intermediate inputB pointer  */
  uint32_t j, k, blkCnt;                         /* loop counter */
  arm_status status;                             /* Status variable */
  uint32_t tapCnt;                               /* loop count */
  q15_t x10, x11, x20, x21;                      /* Temporary variables to hold srcA buffer */
  q15_t y10, y11;                                /* Temporary variables to hold srcB buffer */


  /* Check for range of output samples to be calculated */
  if((firstIndex + numPoints) > ((srcALen + (srcBLen - 1u))))
  {
    /* Set status as ARM_MATH_ARGUMENT_ERROR */
    status = ARM_MATH_ARGUMENT_ERROR;
  }
  else
  {

    /* The algorithm implementation is based on the lengths of the inputs. */
    /* srcB is always made to slide across srcA. */
    /* So srcBLen is always considered as shorter or equal to srcALen */
    if(srcALen >= srcBLen)
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

    /* Temporary pointer for scratch2 */
    py = pScratch2;

    /* pointer to take end of scratch2 buffer */
    pScr2 = pScratch2 + srcBLen - 1;

    /* points to smaller length sequence */
    px = pIn2;

    /* Apply loop unrolling and do 4 Copies simultaneously. */
    k = srcBLen >> 2u;

    /* First part of the processing with loop unrolling copies 4 data points at a time.       
     ** a second loop below copies for the remaining 1 to 3 samples. */
    while(k > 0u)
    {
      /* copy second buffer in reversal manner */
      *pScr2-- = *px++;
      *pScr2-- = *px++;
      *pScr2-- = *px++;
      *pScr2-- = *px++;

      /* Decrement the loop counter */
      k--;
    }

    /* If the count is not a multiple of 4, copy remaining samples here.       
     ** No loop unrolling is used. */
    k = srcBLen % 0x4u;

    while(k > 0u)
    {
      /* copy second buffer in reversal manner for remaining samples */
      *pScr2-- = *px++;

      /* Decrement the loop counter */
      k--;
    }

    /* Initialze temporary scratch pointer */
    pScr1 = pScratch1;

    /* Fill (srcBLen - 1u) zeros in scratch buffer */
    arm_fill_q15(0, pScr1, (srcBLen - 1u));

    /* Update temporary scratch pointer */
    pScr1 += (srcBLen - 1u);

    /* Copy bigger length sequence(srcALen) samples in scratch1 buffer */


    /* Apply loop unrolling and do 4 Copies simultaneously. */
    k = srcALen >> 2u;

    /* First part of the processing with loop unrolling copies 4 data points at a time.       
     ** a second loop below copies for the remaining 1 to 3 samples. */
    while(k > 0u)
    {
      /* copy second buffer in reversal manner */
      *pScr1++ = *pIn1++;
      *pScr1++ = *pIn1++;
      *pScr1++ = *pIn1++;
      *pScr1++ = *pIn1++;

      /* Decrement the loop counter */
      k--;
    }

    /* If the count is not a multiple of 4, copy remaining samples here.       
     ** No loop unrolling is used. */
    k = srcALen % 0x4u;

    while(k > 0u)
    {
      /* copy second buffer in reversal manner for remaining samples */
      *pScr1++ = *pIn1++;

      /* Decrement the loop counter */
      k--;
    }


    /* Apply loop unrolling and do 4 Copies simultaneously. */
    k = (srcBLen - 1u) >> 2u;

    /* First part of the processing with loop unrolling copies 4 data points at a time.       
     ** a second loop below copies for the remaining 1 to 3 samples. */
    while(k > 0u)
    {
      /* copy second buffer in reversal manner */
      *pScr1++ = 0;
      *pScr1++ = 0;
      *pScr1++ = 0;
      *pScr1++ = 0;

      /* Decrement the loop counter */
      k--;
    }

    /* If the count is not a multiple of 4, copy remaining samples here.       
     ** No loop unrolling is used. */
    k = (srcBLen - 1u) % 0x4u;

    while(k > 0u)
    {
      /* copy second buffer in reversal manner for remaining samples */
      *pScr1++ = 0;

      /* Decrement the loop counter */
      k--;
    }


    /* Initialization of pIn2 pointer */
    pIn2 = py;

    pScratch1 += firstIndex;

    pOut = pDst + firstIndex;

    /* Actual convolution process starts here */
    blkCnt = (numPoints) >> 2;

    while(blkCnt > 0)
    {
      /* Initialze temporary scratch pointer as scratch1 */
      pScr1 = pScratch1;

      /* Clear Accumlators */
      acc0 = 0;
      acc1 = 0;
      acc2 = 0;
      acc3 = 0;

      /* Read two samples from scratch1 buffer */
      x10 = *pScr1++;
      x11 = *pScr1++;

      /* Read next two samples from scratch1 buffer */
      x20 = *pScr1++;
      x21 = *pScr1++;

      tapCnt = (srcBLen) >> 2u;

      while(tapCnt > 0u)
      {

        /* Read two samples from smaller buffer */
        y10 = *pIn2;
        y11 = *(pIn2 + 1u);

        /* multiply and accumlate */
        acc0 += (q31_t) x10 *y10;
        acc0 += (q31_t) x11 *y11;
        acc2 += (q31_t) x20 *y10;
        acc2 += (q31_t) x21 *y11;

        /* multiply and accumlate */
        acc1 += (q31_t) x11 *y10;
        acc1 += (q31_t) x20 *y11;

        /* Read next two samples from scratch1 buffer */
        x10 = *pScr1;
        x11 = *(pScr1 + 1u);

        /* multiply and accumlate */
        acc3 += (q31_t) x21 *y10;
        acc3 += (q31_t) x10 *y11;

        /* Read next two samples from scratch2 buffer */
        y10 = *(pIn2 + 2u);
        y11 = *(pIn2 + 3u);

        /* multiply and accumlate */
        acc0 += (q31_t) x20 *y10;
        acc0 += (q31_t) x21 *y11;
        acc2 += (q31_t) x10 *y10;
        acc2 += (q31_t) x11 *y11;
        acc1 += (q31_t) x21 *y10;
        acc1 += (q31_t) x10 *y11;

        /* Read next two samples from scratch1 buffer */
        x20 = *(pScr1 + 2);
        x21 = *(pScr1 + 3);

        /* multiply and accumlate */
        acc3 += (q31_t) x11 *y10;
        acc3 += (q31_t) x20 *y11;

        /* update scratch pointers */
        pIn2 += 4u;
        pScr1 += 4u;

        /* Decrement the loop counter */
        tapCnt--;
      }

      /* Update scratch pointer for remaining samples of smaller length sequence */
      pScr1 -= 4u;

      /* apply same above for remaining samples of smaller length sequence */
      tapCnt = (srcBLen) & 3u;

      while(tapCnt > 0u)
      {
        /* accumlate the results */
        acc0 += (*pScr1++ * *pIn2);
        acc1 += (*pScr1++ * *pIn2);
        acc2 += (*pScr1++ * *pIn2);
        acc3 += (*pScr1++ * *pIn2++);

        pScr1 -= 3u;

        /* Decrement the loop counter */
        tapCnt--;
      }

      blkCnt--;


      /* Store the results in the accumulators in the destination buffer. */
      *pOut++ = __SSAT((acc0 >> 15), 16);
      *pOut++ = __SSAT((acc1 >> 15), 16);
      *pOut++ = __SSAT((acc2 >> 15), 16);
      *pOut++ = __SSAT((acc3 >> 15), 16);

      /* Initialization of inputB pointer */
      pIn2 = py;

      pScratch1 += 4u;

    }


    blkCnt = numPoints & 0x3;

    /* Calculate convolution for remaining samples of Bigger length sequence */
    while(blkCnt > 0)
    {
      /* Initialze temporary scratch pointer as scratch1 */
      pScr1 = pScratch1;

      /* Clear Accumlators */
      acc0 = 0;

      tapCnt = (srcBLen) >> 1u;

      while(tapCnt > 0u)
      {

        /* Read next two samples from scratch1 buffer */
        x10 = *pScr1++;
        x11 = *pScr1++;

        /* Read two samples from smaller buffer */
        y10 = *pIn2++;
        y11 = *pIn2++;

        /* multiply and accumlate */
        acc0 += (q31_t) x10 *y10;
        acc0 += (q31_t) x11 *y11;

        /* Decrement the loop counter */
        tapCnt--;
      }

      tapCnt = (srcBLen) & 1u;

      /* apply same above for remaining samples of smaller length sequence */
      while(tapCnt > 0u)
      {

        /* accumlate the results */
        acc0 += (*pScr1++ * *pIn2++);

        /* Decrement the loop counter */
        tapCnt--;
      }

      blkCnt--;

      /* Store the result in the accumulator in the destination buffer. */
      *pOut++ = (q15_t) (__SSAT((acc0 >> 15), 16));

      /* Initialization of inputB pointer */
      pIn2 = py;

      pScratch1 += 1u;

    }

    /* set status as ARM_MATH_SUCCESS */
    status = ARM_MATH_SUCCESS;

  }

  /* Return to application */
  return (status);
}

#endif	/*	#ifndef UNALIGNED_SUPPORT_DISABLE	*/

/**    
 * @} end of PartialConv group    
 */
