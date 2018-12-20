/* ----------------------------------------------------------------------    
* Copyright (C) 2010-2014 ARM Limited. All rights reserved.    
*    
* $Date:        19. October 2015
* $Revision: 	V.1.4.5 a
*    
* Project: 	    CMSIS DSP Library    
* Title:	    arm_biquad_cascade_df1_fast_q31.c    
*    
* Description:	Processing function for the    
*				Q31 Fast Biquad cascade DirectFormI(DF1) filter.    
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
 * @addtogroup BiquadCascadeDF1    
 * @{    
 */

/**    
 * @details    
 *    
 * @param[in]  *S        points to an instance of the Q31 Biquad cascade structure.    
 * @param[in]  *pSrc     points to the block of input data.    
 * @param[out] *pDst     points to the block of output data.    
 * @param[in]  blockSize number of samples to process per call.    
 * @return 	   none.    
 *    
 * <b>Scaling and Overflow Behavior:</b>    
 * \par    
 * This function is optimized for speed at the expense of fixed-point precision and overflow protection.    
 * The result of each 1.31 x 1.31 multiplication is truncated to 2.30 format.    
 * These intermediate results are added to a 2.30 accumulator.    
 * Finally, the accumulator is saturated and converted to a 1.31 result.    
 * The fast version has the same overflow behavior as the standard version and provides less precision since it discards the low 32 bits of each multiplication result.    
 * In order to avoid overflows completely the input signal must be scaled down by two bits and lie in the range [-0.25 +0.25). Use the intialization function    
 * arm_biquad_cascade_df1_init_q31() to initialize filter structure.    
 *    
 * \par    
 * Refer to the function <code>arm_biquad_cascade_df1_q31()</code> for a slower implementation of this function which uses 64-bit accumulation to provide higher precision.  Both the slow and the fast versions use the same instance structure.    
 * Use the function <code>arm_biquad_cascade_df1_init_q31()</code> to initialize the filter structure.    
 */

void arm_biquad_cascade_df1_fast_q31(
  const arm_biquad_casd_df1_inst_q31 * S,
  q31_t * pSrc,
  q31_t * pDst,
  uint32_t blockSize)
{
  q31_t acc = 0;                                 /*  accumulator                   */
  q31_t Xn1, Xn2, Yn1, Yn2;                      /*  Filter state variables        */
  q31_t b0, b1, b2, a1, a2;                      /*  Filter coefficients           */
  q31_t *pIn = pSrc;                             /*  input pointer initialization  */
  q31_t *pOut = pDst;                            /*  output pointer initialization */
  q31_t *pState = S->pState;                     /*  pState pointer initialization */
  q31_t *pCoeffs = S->pCoeffs;                   /*  coeff pointer initialization  */
  q31_t Xn;                                      /*  temporary input               */
  int32_t shift = (int32_t) S->postShift + 1;    /*  Shift to be applied to the output */
  uint32_t sample, stage = S->numStages;         /*  loop counters                     */


  do
  {
    /* Reading the coefficients */
    b0 = *pCoeffs++;
    b1 = *pCoeffs++;
    b2 = *pCoeffs++;
    a1 = *pCoeffs++;
    a2 = *pCoeffs++;

    /* Reading the state values */
    Xn1 = pState[0];
    Xn2 = pState[1];
    Yn1 = pState[2];
    Yn2 = pState[3];

    /* Apply loop unrolling and compute 4 output values simultaneously. */
    /*      The variables acc ... acc3 hold output values that are being computed:       
     *       
     *    acc =  b0 * x[n] + b1 * x[n-1] + b2 * x[n-2] + a1 * y[n-1] + a2 * y[n-2]       
     */

    sample = blockSize >> 2u;

    /* First part of the processing with loop unrolling.  Compute 4 outputs at a time.       
     ** a second loop below computes the remaining 1 to 3 samples. */
    while(sample > 0u)
    {
      /* Read the input */
      Xn = *pIn;

      /* acc =  b0 * x[n] + b1 * x[n-1] + b2 * x[n-2] + a1 * y[n-1] + a2 * y[n-2] */
      /* acc =  b0 * x[n] */
      /*acc = (q31_t) (((q63_t) b1 * Xn1) >> 32);*/
      mult_32x32_keep32_R(acc, b1, Xn1);
      /* acc +=  b1 * x[n-1] */
      /*acc = (q31_t) ((((q63_t) acc << 32) + ((q63_t) b0 * (Xn))) >> 32);*/
      multAcc_32x32_keep32_R(acc, b0, Xn);
      /* acc +=  b[2] * x[n-2] */
      /*acc = (q31_t) ((((q63_t) acc << 32) + ((q63_t) b2 * (Xn2))) >> 32);*/
      multAcc_32x32_keep32_R(acc, b2, Xn2);
      /* acc +=  a1 * y[n-1] */
      /*acc = (q31_t) ((((q63_t) acc << 32) + ((q63_t) a1 * (Yn1))) >> 32);*/
      multAcc_32x32_keep32_R(acc, a1, Yn1);
      /* acc +=  a2 * y[n-2] */
      /*acc = (q31_t) ((((q63_t) acc << 32) + ((q63_t) a2 * (Yn2))) >> 32);*/
      multAcc_32x32_keep32_R(acc, a2, Yn2);

      /* The result is converted to 1.31 , Yn2 variable is reused */
      Yn2 = acc << shift;

      /* Read the second input */
      Xn2 = *(pIn + 1u);

      /* Store the output in the destination buffer. */
      *pOut = Yn2;

      /* acc =  b0 * x[n] + b1 * x[n-1] + b2 * x[n-2] + a1 * y[n-1] + a2 * y[n-2] */
      /* acc =  b0 * x[n] */
      /*acc = (q31_t) (((q63_t) b0 * (Xn2)) >> 32);*/
      mult_32x32_keep32_R(acc, b0, Xn2);
      /* acc +=  b1 * x[n-1] */
      /*acc = (q31_t) ((((q63_t) acc << 32) + ((q63_t) b1 * (Xn))) >> 32);*/
      multAcc_32x32_keep32_R(acc, b1, Xn);
      /* acc +=  b[2] * x[n-2] */
      /*acc = (q31_t) ((((q63_t) acc << 32) + ((q63_t) b2 * (Xn1))) >> 32);*/
      multAcc_32x32_keep32_R(acc, b2, Xn1);
      /* acc +=  a1 * y[n-1] */
      /*acc = (q31_t) ((((q63_t) acc << 32) + ((q63_t) a1 * (Yn2))) >> 32);*/
      multAcc_32x32_keep32_R(acc, a1, Yn2);
      /* acc +=  a2 * y[n-2] */
      /*acc = (q31_t) ((((q63_t) acc << 32) + ((q63_t) a2 * (Yn1))) >> 32);*/
      multAcc_32x32_keep32_R(acc, a2, Yn1);

      /* The result is converted to 1.31, Yn1 variable is reused  */
      Yn1 = acc << shift;

      /* Read the third input  */
      Xn1 = *(pIn + 2u);

      /* Store the output in the destination buffer. */
      *(pOut + 1u) = Yn1;

      /* acc =  b0 * x[n] + b1 * x[n-1] + b2 * x[n-2] + a1 * y[n-1] + a2 * y[n-2] */
      /* acc =  b0 * x[n] */
      /*acc = (q31_t) (((q63_t) b0 * (Xn1)) >> 32);*/
      mult_32x32_keep32_R(acc, b0, Xn1);
      /* acc +=  b1 * x[n-1] */
      /*acc = (q31_t) ((((q63_t) acc << 32) + ((q63_t) b1 * (Xn2))) >> 32);*/
      multAcc_32x32_keep32_R(acc, b1, Xn2);
      /* acc +=  b[2] * x[n-2] */
      /*acc = (q31_t) ((((q63_t) acc << 32) + ((q63_t) b2 * (Xn))) >> 32);*/
      multAcc_32x32_keep32_R(acc, b2, Xn);
      /* acc +=  a1 * y[n-1] */
      /*acc = (q31_t) ((((q63_t) acc << 32) + ((q63_t) a1 * (Yn1))) >> 32);*/
      multAcc_32x32_keep32_R(acc, a1, Yn1);
      /* acc +=  a2 * y[n-2] */
      /*acc = (q31_t) ((((q63_t) acc << 32) + ((q63_t) a2 * (Yn2))) >> 32);*/
      multAcc_32x32_keep32_R(acc, a2, Yn2);

      /* The result is converted to 1.31, Yn2 variable is reused  */
      Yn2 = acc << shift;

      /* Read the forth input */
      Xn = *(pIn + 3u);

      /* Store the output in the destination buffer. */
      *(pOut + 2u) = Yn2;
      pIn += 4u;

      /* acc =  b0 * x[n] + b1 * x[n-1] + b2 * x[n-2] + a1 * y[n-1] + a2 * y[n-2] */
      /* acc =  b0 * x[n] */
      /*acc = (q31_t) (((q63_t) b0 * (Xn)) >> 32);*/
      mult_32x32_keep32_R(acc, b0, Xn);
      /* acc +=  b1 * x[n-1] */
      /*acc = (q31_t) ((((q63_t) acc << 32) + ((q63_t) b1 * (Xn1))) >> 32);*/
      multAcc_32x32_keep32_R(acc, b1, Xn1);
      /* acc +=  b[2] * x[n-2] */
      /*acc = (q31_t) ((((q63_t) acc << 32) + ((q63_t) b2 * (Xn2))) >> 32);*/
      multAcc_32x32_keep32_R(acc, b2, Xn2);
      /* acc +=  a1 * y[n-1] */
      /*acc = (q31_t) ((((q63_t) acc << 32) + ((q63_t) a1 * (Yn2))) >> 32);*/
      multAcc_32x32_keep32_R(acc, a1, Yn2);
      /* acc +=  a2 * y[n-2] */
      /*acc = (q31_t) ((((q63_t) acc << 32) + ((q63_t) a2 * (Yn1))) >> 32);*/
      multAcc_32x32_keep32_R(acc, a2, Yn1);

      /* Every time after the output is computed state should be updated. */
      /* The states should be updated as:  */
      /* Xn2 = Xn1    */
      Xn2 = Xn1;

      /* The result is converted to 1.31, Yn1 variable is reused  */
      Yn1 = acc << shift;

      /* Xn1 = Xn     */
      Xn1 = Xn;

      /* Store the output in the destination buffer. */
      *(pOut + 3u) = Yn1;
      pOut += 4u;

      /* decrement the loop counter */
      sample--;
    }

    /* If the blockSize is not a multiple of 4, compute any remaining output samples here.       
     ** No loop unrolling is used. */
    sample = (blockSize & 0x3u);

   while(sample > 0u)
   {
      /* Read the input */
      Xn = *pIn++;

      /* acc =  b0 * x[n] + b1 * x[n-1] + b2 * x[n-2] + a1 * y[n-1] + a2 * y[n-2] */
      /* acc =  b0 * x[n] */
      /*acc = (q31_t) (((q63_t) b0 * (Xn)) >> 32);*/
      mult_32x32_keep32_R(acc, b0, Xn);
      /* acc +=  b1 * x[n-1] */
      /*acc = (q31_t) ((((q63_t) acc << 32) + ((q63_t) b1 * (Xn1))) >> 32);*/
      multAcc_32x32_keep32_R(acc, b1, Xn1);
      /* acc +=  b[2] * x[n-2] */
      /*acc = (q31_t) ((((q63_t) acc << 32) + ((q63_t) b2 * (Xn2))) >> 32);*/
      multAcc_32x32_keep32_R(acc, b2, Xn2);
      /* acc +=  a1 * y[n-1] */
      /*acc = (q31_t) ((((q63_t) acc << 32) + ((q63_t) a1 * (Yn1))) >> 32);*/
      multAcc_32x32_keep32_R(acc, a1, Yn1);
      /* acc +=  a2 * y[n-2] */
      /*acc = (q31_t) ((((q63_t) acc << 32) + ((q63_t) a2 * (Yn2))) >> 32);*/
      multAcc_32x32_keep32_R(acc, a2, Yn2);

      /* The result is converted to 1.31  */
      acc = acc << shift;

      /* Every time after the output is computed state should be updated. */
      /* The states should be updated as:  */
      /* Xn2 = Xn1    */
      /* Xn1 = Xn     */
      /* Yn2 = Yn1    */
      /* Yn1 = acc    */
      Xn2 = Xn1;
      Xn1 = Xn;
      Yn2 = Yn1;
      Yn1 = acc;

      /* Store the output in the destination buffer. */
      *pOut++ = acc;

      /* decrement the loop counter */
      sample--;
   }

    /*  The first stage goes from the input buffer to the output buffer. */
    /*  Subsequent stages occur in-place in the output buffer */
    pIn = pDst;

    /* Reset to destination pointer */
    pOut = pDst;

    /*  Store the updated state variables back into the pState array */
    *pState++ = Xn1;
    *pState++ = Xn2;
    *pState++ = Yn1;
    *pState++ = Yn2;

  } while(--stage);
}

/**    
  * @} end of BiquadCascadeDF1 group    
  */
