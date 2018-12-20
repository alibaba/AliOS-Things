/* ----------------------------------------------------------------------      
* Copyright (C) 2010-2014 ARM Limited. All rights reserved. 
*      
* $Date:        19. March 2015
* $Revision: 	V.1.4.5
*      
* Project:      CMSIS DSP Library 
* Title:	    arm_cmplx_mat_mult_q15.c      
*      
* Description:	 Q15 complex matrix multiplication.      
*      
* Target Processor:          Cortex-M4/Cortex-M3/Cortex-M0
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
 * @ingroup groupMatrix      
 */

/**      
 * @addtogroup CmplxMatrixMult      
 * @{      
 */


/**      
 * @brief Q15 Complex matrix multiplication      
 * @param[in]       *pSrcA points to the first input complex matrix structure      
 * @param[in]       *pSrcB points to the second input complex matrix structure      
 * @param[out]      *pDst points to output complex matrix structure      
 * @param[in]		*pScratch points to the array for storing intermediate results     
 * @return     		The function returns either      
 * <code>ARM_MATH_SIZE_MISMATCH</code> or <code>ARM_MATH_SUCCESS</code> based on the outcome of size checking.         
 *  
 * \par Conditions for optimum performance  
 *  Input, output and state buffers should be aligned by 32-bit  
 *  
 * \par Restrictions  
 *  If the silicon does not support unaligned memory access enable the macro UNALIGNED_SUPPORT_DISABLE  
 *	In this case input, output, scratch buffers should be aligned by 32-bit  
 *  
 * @details      
 * <b>Scaling and Overflow Behavior:</b>      
 *      
 * \par      
 * The function is implemented using a 64-bit internal accumulator. The inputs to the      
 * multiplications are in 1.15 format and multiplications yield a 2.30 result.      
 * The 2.30 intermediate      
 * results are accumulated in a 64-bit accumulator in 34.30 format. This approach      
 * provides 33 guard bits and there is no risk of overflow. The 34.30 result is then      
 * truncated to 34.15 format by discarding the low 15 bits and then saturated to      
 * 1.15 format.      
 *      
 * \par      
 * Refer to <code>arm_mat_mult_fast_q15()</code> for a faster but less precise version of this function.      
 *      
 */




arm_status arm_mat_cmplx_mult_q15(
  const arm_matrix_instance_q15 * pSrcA,
  const arm_matrix_instance_q15 * pSrcB,
  arm_matrix_instance_q15 * pDst,
  q15_t * pScratch)
{
  /* accumulator */
  q15_t *pSrcBT = pScratch;                      /* input data matrix pointer for transpose */
  q15_t *pInA = pSrcA->pData;                    /* input data matrix pointer A of Q15 type */
  q15_t *pInB = pSrcB->pData;                    /* input data matrix pointer B of Q15 type */
  q15_t *px;                                     /* Temporary output data matrix pointer */
  uint16_t numRowsA = pSrcA->numRows;            /* number of rows of input matrix A    */
  uint16_t numColsB = pSrcB->numCols;            /* number of columns of input matrix B */
  uint16_t numColsA = pSrcA->numCols;            /* number of columns of input matrix A */
  uint16_t numRowsB = pSrcB->numRows;            /* number of rows of input matrix A    */
  uint16_t col, i = 0u, row = numRowsB, colCnt;  /* loop counters */
  arm_status status;                             /* status of matrix multiplication */
  q63_t sumReal, sumImag;

#ifdef UNALIGNED_SUPPORT_DISABLE
  q15_t in;                                      /* Temporary variable to hold the input value */
  q15_t a, b, c, d;
#else
  q31_t in;                                      /* Temporary variable to hold the input value */
  q31_t prod1, prod2;
  q31_t pSourceA, pSourceB;
#endif

#ifdef ARM_MATH_MATRIX_CHECK
  /* Check for matrix mismatch condition */
  if((pSrcA->numCols != pSrcB->numRows) ||
     (pSrcA->numRows != pDst->numRows) || (pSrcB->numCols != pDst->numCols))
  {
    /* Set status as ARM_MATH_SIZE_MISMATCH */
    status = ARM_MATH_SIZE_MISMATCH;
  }
  else
#endif
  {
    /* Matrix transpose */
    do
    {
      /* Apply loop unrolling and exchange the columns with row elements */
      col = numColsB >> 2;

      /* The pointer px is set to starting address of the column being processed */
      px = pSrcBT + i;

      /* First part of the processing with loop unrolling.  Compute 4 outputs at a time.      
       ** a second loop below computes the remaining 1 to 3 samples. */
      while(col > 0u)
      {
#ifdef UNALIGNED_SUPPORT_DISABLE
        /* Read two elements from the row */
        in = *pInB++;
        *px = in;
        in = *pInB++;
        px[1] = in;

        /* Update the pointer px to point to the next row of the transposed matrix */
        px += numRowsB * 2;

        /* Read two elements from the row */
        in = *pInB++;
        *px = in;
        in = *pInB++;
        px[1] = in;

        /* Update the pointer px to point to the next row of the transposed matrix */
        px += numRowsB * 2;

        /* Read two elements from the row */
        in = *pInB++;
        *px = in;
        in = *pInB++;
        px[1] = in;

        /* Update the pointer px to point to the next row of the transposed matrix */
        px += numRowsB * 2;

        /* Read two elements from the row */
        in = *pInB++;
        *px = in;
        in = *pInB++;
        px[1] = in;

        /* Update the pointer px to point to the next row of the transposed matrix */
        px += numRowsB * 2;

        /* Decrement the column loop counter */
        col--;
      }

      /* If the columns of pSrcB is not a multiple of 4, compute any remaining output samples here.      
       ** No loop unrolling is used. */
      col = numColsB % 0x4u;

      while(col > 0u)
      {
        /* Read two elements from the row */
        in = *pInB++;
        *px = in;
        in = *pInB++;
        px[1] = in;
#else

        /* Read two elements from the row */
        in = *__SIMD32(pInB)++;

        *__SIMD32(px) = in;

        /* Update the pointer px to point to the next row of the transposed matrix */
        px += numRowsB * 2;


        /* Read two elements from the row */
        in = *__SIMD32(pInB)++;

        *__SIMD32(px) = in;

        /* Update the pointer px to point to the next row of the transposed matrix */
        px += numRowsB * 2;

        /* Read two elements from the row */
        in = *__SIMD32(pInB)++;

        *__SIMD32(px) = in;

        /* Update the pointer px to point to the next row of the transposed matrix */
        px += numRowsB * 2;

        /* Read two elements from the row */
        in = *__SIMD32(pInB)++;

        *__SIMD32(px) = in;

        /* Update the pointer px to point to the next row of the transposed matrix */
        px += numRowsB * 2;

        /* Decrement the column loop counter */
        col--;
      }

      /* If the columns of pSrcB is not a multiple of 4, compute any remaining output samples here.      
       ** No loop unrolling is used. */
      col = numColsB % 0x4u;

      while(col > 0u)
      {
        /* Read two elements from the row */
        in = *__SIMD32(pInB)++;

        *__SIMD32(px) = in;
#endif

        /* Update the pointer px to point to the next row of the transposed matrix */
        px += numRowsB * 2;

        /* Decrement the column loop counter */
        col--;
      }

      i = i + 2u;

      /* Decrement the row loop counter */
      row--;

    } while(row > 0u);

    /* Reset the variables for the usage in the following multiplication process */
    row = numRowsA;
    i = 0u;
    px = pDst->pData;

    /* The following loop performs the dot-product of each row in pSrcA with each column in pSrcB */
    /* row loop */
    do
    {
      /* For every row wise process, the column loop counter is to be initiated */
      col = numColsB;

      /* For every row wise process, the pIn2 pointer is set      
       ** to the starting address of the transposed pSrcB data */
      pInB = pSrcBT;

      /* column loop */
      do
      {
        /* Set the variable sum, that acts as accumulator, to zero */
        sumReal = 0;
        sumImag = 0;

        /* Apply loop unrolling and compute 2 MACs simultaneously. */
        colCnt = numColsA >> 1;

        /* Initiate the pointer pIn1 to point to the starting address of the column being processed */
        pInA = pSrcA->pData + i * 2;


        /* matrix multiplication */
        while(colCnt > 0u)
        {
          /* c(m,n) = a(1,1)*b(1,1) + a(1,2) * b(2,1) + .... + a(m,p)*b(p,n) */

#ifdef UNALIGNED_SUPPORT_DISABLE

          /* read real and imag values from pSrcA buffer */
          a = *pInA;
          b = *(pInA + 1u);
          /* read real and imag values from pSrcB buffer */
          c = *pInB;
          d = *(pInB + 1u);

          /* Multiply and Accumlates */
          sumReal += (q31_t) a *c;
          sumImag += (q31_t) a *d;
          sumReal -= (q31_t) b *d;
          sumImag += (q31_t) b *c;

          /* read next real and imag values from pSrcA buffer */
          a = *(pInA + 2u);
          b = *(pInA + 3u);
          /* read next real and imag values from pSrcB buffer */
          c = *(pInB + 2u);
          d = *(pInB + 3u);

          /* update pointer */
          pInA += 4u;

          /* Multiply and Accumlates */
          sumReal += (q31_t) a *c;
          sumImag += (q31_t) a *d;
          sumReal -= (q31_t) b *d;
          sumImag += (q31_t) b *c;
          /* update pointer */
          pInB += 4u;
#else
          /* read real and imag values from pSrcA and pSrcB buffer */
          pSourceA = *__SIMD32(pInA)++;
          pSourceB = *__SIMD32(pInB)++;

          /* Multiply and Accumlates */
#ifdef ARM_MATH_BIG_ENDIAN
          prod1 = -__SMUSD(pSourceA, pSourceB);
#else
          prod1 = __SMUSD(pSourceA, pSourceB);
#endif
          prod2 = __SMUADX(pSourceA, pSourceB);
          sumReal += (q63_t) prod1;
          sumImag += (q63_t) prod2;

          /* read real and imag values from pSrcA and pSrcB buffer */
          pSourceA = *__SIMD32(pInA)++;
          pSourceB = *__SIMD32(pInB)++;

          /* Multiply and Accumlates */
#ifdef ARM_MATH_BIG_ENDIAN
          prod1 = -__SMUSD(pSourceA, pSourceB);
#else
          prod1 = __SMUSD(pSourceA, pSourceB);
#endif
          prod2 = __SMUADX(pSourceA, pSourceB);
          sumReal += (q63_t) prod1;
          sumImag += (q63_t) prod2;

#endif /*      #ifdef UNALIGNED_SUPPORT_DISABLE */

          /* Decrement the loop counter */
          colCnt--;
        }

        /* process odd column samples */
        if((numColsA & 0x1u) > 0u)
        {
          /* c(m,n) = a(1,1)*b(1,1) + a(1,2) * b(2,1) + .... + a(m,p)*b(p,n) */

#ifdef UNALIGNED_SUPPORT_DISABLE

          /* read real and imag values from pSrcA and pSrcB buffer */
          a = *pInA++;
          b = *pInA++;
          c = *pInB++;
          d = *pInB++;

          /* Multiply and Accumlates */
          sumReal += (q31_t) a *c;
          sumImag += (q31_t) a *d;
          sumReal -= (q31_t) b *d;
          sumImag += (q31_t) b *c;

#else
          /* read real and imag values from pSrcA and pSrcB buffer */
          pSourceA = *__SIMD32(pInA)++;
          pSourceB = *__SIMD32(pInB)++;

          /* Multiply and Accumlates */
#ifdef ARM_MATH_BIG_ENDIAN
          prod1 = -__SMUSD(pSourceA, pSourceB);
#else
          prod1 = __SMUSD(pSourceA, pSourceB);
#endif
          prod2 = __SMUADX(pSourceA, pSourceB);
          sumReal += (q63_t) prod1;
          sumImag += (q63_t) prod2;

#endif /*      #ifdef UNALIGNED_SUPPORT_DISABLE */

        }

        /* Saturate and store the result in the destination buffer */

        *px++ = (q15_t) (__SSAT(sumReal >> 15, 16));
        *px++ = (q15_t) (__SSAT(sumImag >> 15, 16));

        /* Decrement the column loop counter */
        col--;

      } while(col > 0u);

      i = i + numColsA;

      /* Decrement the row loop counter */
      row--;

    } while(row > 0u);

    /* set status as ARM_MATH_SUCCESS */
    status = ARM_MATH_SUCCESS;
  }

  /* Return to application */
  return (status);
}

/**      
 * @} end of MatrixMult group      
 */
