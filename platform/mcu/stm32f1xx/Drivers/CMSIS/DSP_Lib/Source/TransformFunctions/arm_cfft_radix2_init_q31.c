/* ----------------------------------------------------------------------   
* Copyright (C) 2010-2014 ARM Limited. All rights reserved.   
*   
* $Date:        19. March 2015
* $Revision: 	V.1.4.5  
*   
* Project: 	    CMSIS DSP Library   
* Title:	    arm_cfft_radix2_init_q31.c   
*   
* Description:	Radix-2 Decimation in Frequency Fixed-point CFFT & CIFFT Initialization function   
*   
* Target Processor: Cortex-M4/Cortex-M3/Cortex-M0
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
#include "arm_common_tables.h"

/**   
 * @ingroup groupTransforms   
 */

/**   
 * @addtogroup ComplexFFT   
 * @{   
 */


/**    
*    
* @brief  Initialization function for the Q31 CFFT/CIFFT.   
* @deprecated Do not use this function.  It has been superseded by \ref arm_cfft_q31 and will be removed
* @param[in,out] *S             points to an instance of the Q31 CFFT/CIFFT structure.   
* @param[in]     fftLen         length of the FFT.  
* @param[in]     ifftFlag       flag that selects forward (ifftFlag=0) or inverse (ifftFlag=1) transform.  
* @param[in]     bitReverseFlag flag that enables (bitReverseFlag=1) or disables (bitReverseFlag=0) bit reversal of output.  
* @return        The function returns ARM_MATH_SUCCESS if initialization is successful or ARM_MATH_ARGUMENT_ERROR if <code>fftLen</code> is not a supported value.  
*   
* \par Description:  
* \par   
* The parameter <code>ifftFlag</code> controls whether a forward or inverse transform is computed.   
* Set(=1) ifftFlag for calculation of CIFFT otherwise  CFFT is calculated  
* \par   
* The parameter <code>bitReverseFlag</code> controls whether output is in normal order or bit reversed order.   
* Set(=1) bitReverseFlag for output to be in normal order otherwise output is in bit reversed order.   
* \par   
* The parameter <code>fftLen</code>	Specifies length of CFFT/CIFFT process. Supported FFT Lengths are 16, 64, 256, 1024.   
* \par   
* This Function also initializes Twiddle factor table pointer and Bit reversal table pointer.   
*/

arm_status arm_cfft_radix2_init_q31(
  arm_cfft_radix2_instance_q31 * S,
  uint16_t fftLen,
  uint8_t ifftFlag,
  uint8_t bitReverseFlag)
{
  /*  Initialise the default arm status */
  arm_status status = ARM_MATH_SUCCESS;

  /*  Initialise the FFT length */
  S->fftLen = fftLen;

  /*  Initialise the Twiddle coefficient pointer */
  S->pTwiddle = (q31_t *) twiddleCoef_4096_q31;
  /*  Initialise the Flag for selection of CFFT or CIFFT */
  S->ifftFlag = ifftFlag;
  /*  Initialise the Flag for calculation Bit reversal or not */
  S->bitReverseFlag = bitReverseFlag;

  /*  Initializations of Instance structure depending on the FFT length */
  switch (S->fftLen)
  {
    /*  Initializations of structure parameters for 4096 point FFT */
  case 4096u:
    /*  Initialise the twiddle coef modifier value */
    S->twidCoefModifier = 1u;
    /*  Initialise the bit reversal table modifier */
    S->bitRevFactor = 1u;
    /*  Initialise the bit reversal table pointer */
    S->pBitRevTable = (uint16_t *) armBitRevTable;
    break;

    /*  Initializations of structure parameters for 2048 point FFT */
  case 2048u:
    /*  Initialise the twiddle coef modifier value */
    S->twidCoefModifier = 2u;
    /*  Initialise the bit reversal table modifier */
    S->bitRevFactor = 2u;
    /*  Initialise the bit reversal table pointer */
    S->pBitRevTable = (uint16_t *) & armBitRevTable[1];
    break;

    /*  Initializations of structure parameters for 1024 point FFT */
  case 1024u:
    /*  Initialise the twiddle coef modifier value */
    S->twidCoefModifier = 4u;
    /*  Initialise the bit reversal table modifier */
    S->bitRevFactor = 4u;
    /*  Initialise the bit reversal table pointer */
    S->pBitRevTable = (uint16_t *) & armBitRevTable[3];
    break;

    /*  Initializations of structure parameters for 512 point FFT */
  case 512u:
    /*  Initialise the twiddle coef modifier value */
    S->twidCoefModifier = 8u;
    /*  Initialise the bit reversal table modifier */
    S->bitRevFactor = 8u;
    /*  Initialise the bit reversal table pointer */
    S->pBitRevTable = (uint16_t *) & armBitRevTable[7];
    break;

  case 256u:
    /*  Initializations of structure parameters for 256 point FFT */
    S->twidCoefModifier = 16u;
    S->bitRevFactor = 16u;
    S->pBitRevTable = (uint16_t *) & armBitRevTable[15];
    break;

  case 128u:
    /*  Initializations of structure parameters for 128 point FFT */
    S->twidCoefModifier = 32u;
    S->bitRevFactor = 32u;
    S->pBitRevTable = (uint16_t *) & armBitRevTable[31];
    break;

  case 64u:
    /*  Initializations of structure parameters for 64 point FFT */
    S->twidCoefModifier = 64u;
    S->bitRevFactor = 64u;
    S->pBitRevTable = (uint16_t *) & armBitRevTable[63];
    break;

  case 32u:
    /*  Initializations of structure parameters for 32 point FFT */
    S->twidCoefModifier = 128u;
    S->bitRevFactor = 128u;
    S->pBitRevTable = (uint16_t *) & armBitRevTable[127];
    break;

  case 16u:
    /*  Initializations of structure parameters for 16 point FFT */
    S->twidCoefModifier = 256u;
    S->bitRevFactor = 256u;
    S->pBitRevTable = (uint16_t *) & armBitRevTable[255];
    break;


  default:
    /*  Reporting argument error if fftSize is not valid value */
    status = ARM_MATH_ARGUMENT_ERROR;
    break;
  }

  return (status);
}

/**   
 * @} end of ComplexFFT group   
 */
