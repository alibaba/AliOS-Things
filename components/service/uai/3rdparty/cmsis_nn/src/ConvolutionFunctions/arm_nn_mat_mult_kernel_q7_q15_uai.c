/*
 * Copyright (C) 2010-2018 Arm Limited or its affiliates. All rights reserved.
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

/* ----------------------------------------------------------------------
 * Project:      CMSIS NN Library
 * Title:        arm_nn_mat_mult_kernel_q7_q15.c
 * Description:  Matrix-multiplication function for convolution
 *
 * $Date:        17. January 2018
 * $Revision:    V.1.0.0
 *
 * Target Processor:  Cortex-M cores
 * -------------------------------------------------------------------- */

#include "arm_math.h"
#include "arm_nnfunctions.h"

  /**
   * @brief Matrix-multiplication function for convolution
   * @param[in]       pA          pointer to operand A
   * @param[in]       pInBuffer   pointer to operand B, always conssists of 2 vectors
   * @param[in]       ch_im_out   numRow of A
   * @param[in]       numCol_A    numCol of A
   * @param[in,out]   pOut        pointer to output
   * @return     The function returns the incremented output pointer
   *
   * @details
   *
   * This function does the matrix multiplication with weight matrix
   * and 2 columns from im2col.
   */

q7_t     *arm_nn_mat_mult_kernel_q7_q15_uai(const q7_t * pA,
                                            const q15_t * pInBuffer,
                                            const uint16_t ch_im_out,
                                            const uint16_t numCol_A,
                                            const q7_t * bias,
                                            const int32_t *kernel_scale,
                                            const int32_t *bias_scale,
                                            const int32_t act_scale,
                                            const int8_t shift,
                                            q7_t * pOut)
{
#if defined (ARM_MATH_DSP)
    /* set up the second output pointers */
    q31_t     *pOut2 = pOut + ch_im_out;

    uint16_t  rowCnt = ch_im_out >> 1;
    uint16_t i_ch_out = 0;

    /* this loop over rows in A */
    while (rowCnt)
    {
        /* setup pointers for B */
        const q15_t *pB = pInBuffer;
        const q15_t *pB2 = pB + numCol_A;

        /* align the second pointer for A */
        const q7_t *pA2 = pA + numCol_A;

        /* init the sum with bias */
        q31_t     sum  = 0;
        q31_t     sum2 = 0;
        q31_t     sum3 = 0;
        q31_t     sum4 = 0;
        q63_t sum_temp = 0;

        uint16_t  colCnt = numCol_A >> 2;
        /* accumulate over the vector */
        while (colCnt)
        {
            q31_t     inA11, inA12, inA21, inA22;
            q31_t     inB1 = *__SIMD32(pB)++;
            q31_t     inB2 = *__SIMD32(pB2)++;

            pA = (q7_t *) read_and_pad((void *)pA, &inA11, &inA12);
            pA2 = (q7_t *) read_and_pad((void *)pA2, &inA21, &inA22);

            sum = __SMLAD(inA11, inB1, sum);
            sum2 = __SMLAD(inA11, inB2, sum2);
            sum3 = __SMLAD(inA21, inB1, sum3);
            sum4 = __SMLAD(inA21, inB2, sum4);

            inB1 = *__SIMD32(pB)++;
            inB2 = *__SIMD32(pB2)++;

            sum = __SMLAD(inA12, inB1, sum);
            sum2 = __SMLAD(inA12, inB2, sum2);
            sum3 = __SMLAD(inA22, inB1, sum3);
            sum4 = __SMLAD(inA22, inB2, sum4);

            colCnt--;
        }                       /* while over colCnt */
        colCnt = numCol_A & 0x3;
        while (colCnt)
        {
            q7_t      inA1 = *pA++;
            q15_t     inB1 = *pB++;
            q7_t      inA2 = *pA2++;
            q15_t     inB2 = *pB2++;

            sum += inA1 * inB1;
            sum2 += inA1 * inB2;
            sum3 += inA2 * inB1;
            sum4 += inA2 * inB2;
            colCnt--;
        }                       /* while over colCnt */
        sum_temp = sum * kernel_scale[i_ch_out] + bias[i_ch_out] * bias_scale[i_ch_out];
        *pOut++  = (q7_t)__SSAT((sum_temp >> shift) / act_scale, 8);

        sum_temp = sum3 * kernel_scale[i_ch_out + 1] + bias[i_ch_out + 1] * bias_scale[i_ch_out + 1];
        *pOut++  = (q7_t)__SSAT((sum_temp >> shift) / act_scale, 8);

        sum_temp = sum2 * kernel_scale[i_ch_out] + bias[i_ch_out] * bias_scale[i_ch_out];
        *pOut2++ = (q7_t)__SSAT((sum_temp >> shift) / act_scale, 8);

        sum_temp = sum4 * kernel_scale[i_ch_out + 1] + bias[i_ch_out + 1] * bias_scale[i_ch_out + 1];
        *pOut2++ = (q7_t)__SSAT((sum_temp >> shift) / act_scale, 8);

        i_ch_out += 2;
        /* skip the row computed with A2 */
        pA += numCol_A;
        rowCnt--;
    }                           /* for over ch_im_out */

    /* compute left-over row if any */
    if (ch_im_out & 0x1)
    {
        /* setup pointers for B */
        const q15_t *pB = pInBuffer;
        const q15_t *pB2 = pB + numCol_A;

        /* load the bias */
        q31_t     sum  = 0;
        q31_t     sum2 = 0;
        q63_t sum_temp = 0;

        uint16_t  colCnt = numCol_A >> 2;
        while (colCnt)
        {
            q31_t     inA11, inA12;
            q31_t     inB1 = *__SIMD32(pB)++;
            q31_t     inB2 = *__SIMD32(pB2)++;

            pA = (q7_t *) read_and_pad((void *)pA, &inA11, &inA12);

            sum = __SMLAD(inA11, inB1, sum);
            sum2 = __SMLAD(inA11, inB2, sum2);

            inB1 = *__SIMD32(pB)++;
            inB2 = *__SIMD32(pB2)++;
            sum = __SMLAD(inA12, inB1, sum);
            sum2 = __SMLAD(inA12, inB2, sum2);

            colCnt--;
        }
        colCnt = numCol_A & 0x3;
        while (colCnt)
        {
            q7_t      inA1 = *pA++;
            q15_t     inB1 = *pB++;
            q15_t     inB2 = *pB2++;

            sum += inA1 * inB1;
            sum2 += inA1 * inB2;
            colCnt--;
        }
        sum_temp = sum * kernel_scale[i_ch_out] + bias[i_ch_out] * bias_scale[i_ch_out];
        *pOut++ = (q7_t)__SSAT((sum_temp >> shift) / act_scale, 8);
        sum_temp = sum2 * kernel_scale[i_ch_out + 1] + bias[i_ch_out + 1] * bias_scale[i_ch_out + 1];
        *pOut2++ = (q7_t)__SSAT((sum_temp >> shift) / act_scale, 8);
    }

    pOut += ch_im_out;

    /* return the new output pointer with offset */
    return pOut;
#else
    /* To be completed */
    return NULL;
#endif                          /* ARM_MATH_DSP */

}
