
/**
 * @file xmc_dma_map.h
 * @date 2015-05-07
 *
 * @cond
 *********************************************************************************************************************
 * XMClib v2.1.18 - XMC Peripheral Driver Library 
 *
 * Copyright (c) 2015-2018, Infineon Technologies AG
 * All rights reserved.                        
 *                                             
 * Redistribution and use in source and binary forms, with or without modification,are permitted provided that the 
 * following conditions are met:   
 *                                                                              
 * Redistributions of source code must retain the above copyright notice, this list of conditions and the following 
 * disclaimer.                        
 * 
 * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following 
 * disclaimer in the documentation and/or other materials provided with the distribution.                       
 * 
 * Neither the name of the copyright holders nor the names of its contributors may be used to endorse or promote 
 * products derived from this software without specific prior written permission.                                           
 *                                                                              
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, 
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE  
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE  FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR  
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
 * WHETHER IN CONTRACT, STRICT LIABILITY,OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.                                                  
 *                                                                              
 * To improve the quality of the software, users are encouraged to share modifications, enhancements or bug fixes with 
 * Infineon Technologies AG dave@infineon.com).                                                          
 *********************************************************************************************************************
 *
 * Change History
 * --------------
 *
 * 2015-02-20:
 *     - Initial version
 *
 * 2015-05-07:
 *     - Change line numbering for DMA1 <br>
 *
 * @endcond
 */

#ifndef XMC_DMA_MAP_H
#define XMC_DMA_MAP_H

#define DMA_PERIPHERAL_REQUEST(line, sel) (uint8_t)(line | (sel << 4U))

/*
 * DMA LINE 0 of DMA0
 */

#define DMA0_PERIPHERAL_REQUEST_ERU0_SR0_0   DMA_PERIPHERAL_REQUEST(0, 0)
#define DMA0_PERIPHERAL_REQUEST_VADC_C0SR0_0 DMA_PERIPHERAL_REQUEST(0, 1)
#define DMA0_PERIPHERAL_REQUEST_VADC_G0SR3_0 DMA_PERIPHERAL_REQUEST(0, 2)

#if ((UC_SERIES == XMC48) || (UC_SERIES == XMC47) || (UC_SERIES == XMC45) || (UC_SERIES == XMC44))
#define DMA0_PERIPHERAL_REQUEST_VADC_G2SR0_0 DMA_PERIPHERAL_REQUEST(0, 3)
#define DMA0_PERIPHERAL_REQUEST_VADC_G2SR3_0 DMA_PERIPHERAL_REQUEST(0, 4)
#define DMA0_PERIPHERAL_REQUEST_DSD_SRM0_0   DMA_PERIPHERAL_REQUEST(0, 5)
#endif

#define DMA0_PERIPHERAL_REQUEST_CCU40_SR0_0  DMA_PERIPHERAL_REQUEST(0, 6)
#define DMA0_PERIPHERAL_REQUEST_CCU80_SR0_0  DMA_PERIPHERAL_REQUEST(0, 7)
#define DMA0_PERIPHERAL_REQUEST_CAN_SR0_0    DMA_PERIPHERAL_REQUEST(0, 9)
#define DMA0_PERIPHERAL_REQUEST_USIC0_SR0_0  DMA_PERIPHERAL_REQUEST(0, 10)
#define DMA0_PERIPHERAL_REQUEST_USIC1_SR0_0  DMA_PERIPHERAL_REQUEST(0, 11)

#if ((UC_SERIES == XMC48) || (UC_SERIES == XMC47) || (UC_SERIES == XMC45) || (UC_SERIES == XMC44))
#define DMA0_PERIPHERAL_REQUEST_VADC_G3SR3_0 DMA_PERIPHERAL_REQUEST(0, 13)
#define DMA0_PERIPHERAL_REQUEST_CCU42_SR0_0  DMA_PERIPHERAL_REQUEST(0, 14)
#endif

#if ((UC_SERIES == XMC44) || (UC_SERIES == XMC42) || (UC_SERIES == XMC41))
#define DMA0_PERIPHERAL_REQUEST_HRPWM0_SR0_0 DMA_PERIPHERAL_REQUEST(0, 15)
#endif

/*
 * DMA LINE 1 of DMA0
 */

#define DMA0_PERIPHERAL_REQUEST_ERU0_SR3_1   DMA_PERIPHERAL_REQUEST(1, 0)
#define DMA0_PERIPHERAL_REQUEST_VADC_C0SR1_1 DMA_PERIPHERAL_REQUEST(1, 1)
#define DMA0_PERIPHERAL_REQUEST_VADC_G0SR2_1 DMA_PERIPHERAL_REQUEST(1, 2)
#define DMA0_PERIPHERAL_REQUEST_VADC_G1SR0_1 DMA_PERIPHERAL_REQUEST(1, 3)

#if ((UC_SERIES == XMC48) || (UC_SERIES == XMC47) || (UC_SERIES == XMC45) || (UC_SERIES == XMC44))
#define DMA0_PERIPHERAL_REQUEST_VADC_G2SR2_1 DMA_PERIPHERAL_REQUEST(1, 4)
#endif

#define DMA0_PERIPHERAL_REQUEST_DAC_SR0_1    DMA_PERIPHERAL_REQUEST(1, 5)
#define DMA0_PERIPHERAL_REQUEST_CCU40_SR0_1  DMA_PERIPHERAL_REQUEST(1, 6)
#define DMA0_PERIPHERAL_REQUEST_CCU80_SR0_1  DMA_PERIPHERAL_REQUEST(1, 7)
#define DMA0_PERIPHERAL_REQUEST_CAN_SR0_1    DMA_PERIPHERAL_REQUEST(1, 9)
#define DMA0_PERIPHERAL_REQUEST_USIC0_SR0_1  DMA_PERIPHERAL_REQUEST(1, 10)
#define DMA0_PERIPHERAL_REQUEST_USIC1_SR0_1  DMA_PERIPHERAL_REQUEST(1, 11)

#if ((UC_SERIES == XMC48) || (UC_SERIES == XMC47) || (UC_SERIES == XMC45) || (UC_SERIES == XMC44))
#define DMA0_PERIPHERAL_REQUEST_VADC_G3SR0_1 DMA_PERIPHERAL_REQUEST(1, 13)
#define DMA0_PERIPHERAL_REQUEST_CCU42_SR0_1  DMA_PERIPHERAL_REQUEST(1, 14)
#endif

#if ((UC_SERIES == XMC44) || (UC_SERIES == XMC42) || (UC_SERIES == XMC41))
#define DMA0_PERIPHERAL_REQUEST_HRPWM0_SR1_1 DMA_PERIPHERAL_REQUEST(1, 15)
#endif

/*
 * DMA LINE 2 of DMA0
 */

#define DMA0_PERIPHERAL_REQUEST_ERU0_SR1_2   DMA_PERIPHERAL_REQUEST(2, 0)
#define DMA0_PERIPHERAL_REQUEST_VADC_C0SR2_2 DMA_PERIPHERAL_REQUEST(2, 1)
#define DMA0_PERIPHERAL_REQUEST_VADC_C0SR3_2 DMA_PERIPHERAL_REQUEST(2, 2)
#define DMA0_PERIPHERAL_REQUEST_VADC_G1SR3_2 DMA_PERIPHERAL_REQUEST(2, 3)

#if ((UC_SERIES == XMC48) || (UC_SERIES == XMC47) || (UC_SERIES == XMC45) || (UC_SERIES == XMC44))
#define DMA0_PERIPHERAL_REQUEST_VADC_G2SR1_2 DMA_PERIPHERAL_REQUEST(2, 4)
#define DMA0_PERIPHERAL_REQUEST_DSD_SRM1_2   DMA_PERIPHERAL_REQUEST(2, 5)
#define DMA0_PERIPHERAL_REQUEST_DSD_SRM3_2   DMA_PERIPHERAL_REQUEST(2, 6)
#endif

#define DMA0_PERIPHERAL_REQUEST_CCU40_SR1_2  DMA_PERIPHERAL_REQUEST(2, 7)
#define DMA0_PERIPHERAL_REQUEST_CCU80_SR1_2  DMA_PERIPHERAL_REQUEST(2, 8)
#define DMA0_PERIPHERAL_REQUEST_CAN_SR1_2    DMA_PERIPHERAL_REQUEST(2, 10)
#define DMA0_PERIPHERAL_REQUEST_USIC0_SR1_2  DMA_PERIPHERAL_REQUEST(2, 11)
#define DMA0_PERIPHERAL_REQUEST_USIC1_SR1_2  DMA_PERIPHERAL_REQUEST(2, 12)

#if ((UC_SERIES == XMC48) || (UC_SERIES == XMC47) || (UC_SERIES == XMC45) || (UC_SERIES == XMC44))
#define DMA0_PERIPHERAL_REQUEST_VADC_G3SR2_2 DMA_PERIPHERAL_REQUEST(2, 13)
#define DMA0_PERIPHERAL_REQUEST_CCU42_SR1_2  DMA_PERIPHERAL_REQUEST(2, 14)
#endif

/*
 * DMA LINE 3 of DMA0
 */

#define DMA0_PERIPHERAL_REQUEST_ERU0_SR2_3   DMA_PERIPHERAL_REQUEST(3, 0)
#define DMA0_PERIPHERAL_REQUEST_VADC_C0SR2_3 DMA_PERIPHERAL_REQUEST(3, 1)
#define DMA0_PERIPHERAL_REQUEST_VADC_C0SR3_3 DMA_PERIPHERAL_REQUEST(3, 2)
#define DMA0_PERIPHERAL_REQUEST_VADC_G1SR1_3 DMA_PERIPHERAL_REQUEST(3, 3)
#define DMA0_PERIPHERAL_REQUEST_VADC_G1SR2_3 DMA_PERIPHERAL_REQUEST(3, 4)

#if ((UC_SERIES == XMC48) || (UC_SERIES == XMC47) || (UC_SERIES == XMC45) || (UC_SERIES == XMC44))
#define DMA0_PERIPHERAL_REQUEST_DSD_SRM2_3   DMA_PERIPHERAL_REQUEST(3, 5)
#endif

#define DMA0_PERIPHERAL_REQUEST_DAC_SR1_3    DMA_PERIPHERAL_REQUEST(3, 6)
#define DMA0_PERIPHERAL_REQUEST_CCU40_SR1_3  DMA_PERIPHERAL_REQUEST(3, 7)
#define DMA0_PERIPHERAL_REQUEST_CCU80_SR1_3  DMA_PERIPHERAL_REQUEST(3, 8)
#define DMA0_PERIPHERAL_REQUEST_CAN_SR1_3    DMA_PERIPHERAL_REQUEST(3, 10)
#define DMA0_PERIPHERAL_REQUEST_USIC0_SR1_3  DMA_PERIPHERAL_REQUEST(3, 11)
#define DMA0_PERIPHERAL_REQUEST_USIC1_SR1_3  DMA_PERIPHERAL_REQUEST(3, 12)

#if ((UC_SERIES == XMC48) || (UC_SERIES == XMC47) || (UC_SERIES == XMC45) || (UC_SERIES == XMC44))
#define DMA0_PERIPHERAL_REQUEST_VADC_G3SR1_3 DMA_PERIPHERAL_REQUEST(3, 13)
#define DMA0_PERIPHERAL_REQUEST_CCU42_SR1_3  DMA_PERIPHERAL_REQUEST(3, 14)
#endif

/*
 * DMA LINE 4 of DMA0
 */

#define DMA0_PERIPHERAL_REQUEST_ERU0_SR2_4   DMA_PERIPHERAL_REQUEST(4, 0)
#define DMA0_PERIPHERAL_REQUEST_VADC_G0SR0_4 DMA_PERIPHERAL_REQUEST(4, 1)
#define DMA0_PERIPHERAL_REQUEST_VADC_G0SR1_4 DMA_PERIPHERAL_REQUEST(4, 2)

#if ((UC_SERIES == XMC48) || (UC_SERIES == XMC47) || (UC_SERIES == XMC45) || (UC_SERIES == XMC44))
#define DMA0_PERIPHERAL_REQUEST_VADC_G2SR1_4 DMA_PERIPHERAL_REQUEST(4, 3)
#define DMA0_PERIPHERAL_REQUEST_VADC_G2SR2_4 DMA_PERIPHERAL_REQUEST(4, 4)
#define DMA0_PERIPHERAL_REQUEST_DSD_SRM2_4   DMA_PERIPHERAL_REQUEST(4, 5)
#endif

#define DMA0_PERIPHERAL_REQUEST_DAC_SR1_4    DMA_PERIPHERAL_REQUEST(4, 6)
#define DMA0_PERIPHERAL_REQUEST_CCU41_SR0_4  DMA_PERIPHERAL_REQUEST(4, 7)

#if ((UC_SERIES == XMC48) || (UC_SERIES == XMC47) || (UC_SERIES == XMC45) || (UC_SERIES == XMC44))
#define DMA0_PERIPHERAL_REQUEST_CCU81_SR0_4  DMA_PERIPHERAL_REQUEST(4, 8)
#endif

#define DMA0_PERIPHERAL_REQUEST_CAN_SR2_4    DMA_PERIPHERAL_REQUEST(4, 10)
#define DMA0_PERIPHERAL_REQUEST_USIC0_SR0_4  DMA_PERIPHERAL_REQUEST(4, 11)
#define DMA0_PERIPHERAL_REQUEST_USIC1_SR0_4  DMA_PERIPHERAL_REQUEST(4, 12)

#if ((UC_SERIES == XMC48) || (UC_SERIES == XMC47) || (UC_SERIES == XMC45) || (UC_SERIES == XMC44))
#define DMA0_PERIPHERAL_REQUEST_VADC_G3SR1_4 DMA_PERIPHERAL_REQUEST(4, 13)
#define DMA0_PERIPHERAL_REQUEST_CCU43_SR0_4  DMA_PERIPHERAL_REQUEST(4, 14)
#endif

/*
 * DMA LINE 5 of DMA0
 */

#define DMA0_PERIPHERAL_REQUEST_ERU0_SR1_5   DMA_PERIPHERAL_REQUEST(5, 0)
#define DMA0_PERIPHERAL_REQUEST_VADC_G0SR0_5 DMA_PERIPHERAL_REQUEST(5, 1)
#define DMA0_PERIPHERAL_REQUEST_VADC_G0SR1_5 DMA_PERIPHERAL_REQUEST(5, 2)
#define DMA0_PERIPHERAL_REQUEST_VADC_G1SR2_5 DMA_PERIPHERAL_REQUEST(5, 3)

#if ((UC_SERIES == XMC48) || (UC_SERIES == XMC47) || (UC_SERIES == XMC45) || (UC_SERIES == XMC44))
#define DMA0_PERIPHERAL_REQUEST_VADC_G2SR0_5 DMA_PERIPHERAL_REQUEST(5, 4)
#endif

#define DMA0_PERIPHERAL_REQUEST_DAC_SR0_5    DMA_PERIPHERAL_REQUEST(5, 5)
#define DMA0_PERIPHERAL_REQUEST_CCU41_SR0_5  DMA_PERIPHERAL_REQUEST(5, 6)

#if ((UC_SERIES == XMC48) || (UC_SERIES == XMC47) || (UC_SERIES == XMC45) || (UC_SERIES == XMC44))
#define DMA0_PERIPHERAL_REQUEST_CCU81_SR0_5  DMA_PERIPHERAL_REQUEST(5, 7)
#endif

#define DMA0_PERIPHERAL_REQUEST_CAN_SR2_5    DMA_PERIPHERAL_REQUEST(5, 9)
#define DMA0_PERIPHERAL_REQUEST_USIC0_SR0_5  DMA_PERIPHERAL_REQUEST(5, 10)
#define DMA0_PERIPHERAL_REQUEST_USIC1_SR0_5  DMA_PERIPHERAL_REQUEST(5, 11)

#if ((UC_SERIES == XMC48) || (UC_SERIES == XMC47) || (UC_SERIES == XMC45) || (UC_SERIES == XMC44))
#define DMA0_PERIPHERAL_REQUEST_VADC_G3SR2_5 DMA_PERIPHERAL_REQUEST(5, 13)
#define DMA0_PERIPHERAL_REQUEST_CCU43_SR0_5  DMA_PERIPHERAL_REQUEST(5, 14)
#endif

#if ((UC_SERIES == XMC44) || (UC_SERIES == XMC42) || (UC_SERIES == XMC41))
#define DMA0_PERIPHERAL_REQUEST_HRPWM0_SR0_5 DMA_PERIPHERAL_REQUEST(5, 15)
#endif

/*
 * DMA LINE 6 of DMA0
 */

#define DMA0_PERIPHERAL_REQUEST_ERU0_SR3_6   DMA_PERIPHERAL_REQUEST(6, 0)
#define DMA0_PERIPHERAL_REQUEST_VADC_C0SR1_6 DMA_PERIPHERAL_REQUEST(6, 1)
#define DMA0_PERIPHERAL_REQUEST_VADC_G0SR2_6 DMA_PERIPHERAL_REQUEST(6, 2)
#define DMA0_PERIPHERAL_REQUEST_VADC_G1SR1_6 DMA_PERIPHERAL_REQUEST(6, 3)

#if ((UC_SERIES == XMC48) || (UC_SERIES == XMC47) || (UC_SERIES == XMC45) || (UC_SERIES == XMC44))
#define DMA0_PERIPHERAL_REQUEST_VADC_G2SR3_6 DMA_PERIPHERAL_REQUEST(6, 4)
#define DMA0_PERIPHERAL_REQUEST_DSD_SRM1_6   DMA_PERIPHERAL_REQUEST(6, 5)
#define DMA0_PERIPHERAL_REQUEST_DSD_SRM3_6   DMA_PERIPHERAL_REQUEST(6, 6)
#endif

#define DMA0_PERIPHERAL_REQUEST_CCU41_SR1_6  DMA_PERIPHERAL_REQUEST(6, 7)

#if ((UC_SERIES == XMC48) || (UC_SERIES == XMC47) || (UC_SERIES == XMC45) || (UC_SERIES == XMC44))
#define DMA0_PERIPHERAL_REQUEST_CCU81_SR1_6  DMA_PERIPHERAL_REQUEST(6, 8)
#endif

#define DMA0_PERIPHERAL_REQUEST_CAN_SR3_6    DMA_PERIPHERAL_REQUEST(6, 10)
#define DMA0_PERIPHERAL_REQUEST_USIC0_SR1_6  DMA_PERIPHERAL_REQUEST(6, 11)
#define DMA0_PERIPHERAL_REQUEST_USIC1_SR1_6  DMA_PERIPHERAL_REQUEST(6, 12)

#if ((UC_SERIES == XMC48) || (UC_SERIES == XMC47) || (UC_SERIES == XMC45) || (UC_SERIES == XMC44))
#define DMA0_PERIPHERAL_REQUEST_VADC_G3SR0_6 DMA_PERIPHERAL_REQUEST(6, 13)
#define DMA0_PERIPHERAL_REQUEST_CCU43_SR1_6  DMA_PERIPHERAL_REQUEST(6, 14)
#endif

/*
 * DMA LINE 7 of DMA0
 */

#define DMA0_PERIPHERAL_REQUEST_ERU0_SR0_7    DMA_PERIPHERAL_REQUEST(7, 0)
#define DMA0_PERIPHERAL_REQUEST_VADC_C0SR0_7  DMA_PERIPHERAL_REQUEST(7, 1)
#define DMA0_PERIPHERAL_REQUEST_VADC_G0SR3_7  DMA_PERIPHERAL_REQUEST(7, 2)
#define DMA0_PERIPHERAL_REQUEST_VADC_G1SR0_7  DMA_PERIPHERAL_REQUEST(7, 3)
#define DMA0_PERIPHERAL_REQUEST_VADC_G1SR3_7  DMA_PERIPHERAL_REQUEST(7, 4)

#if ((UC_SERIES == XMC48) || (UC_SERIES == XMC47) || (UC_SERIES == XMC45) || (UC_SERIES == XMC44))
#define DMA0_PERIPHERAL_REQUEST_DSD_SRM0_7    DMA_PERIPHERAL_REQUEST(7, 5)
#endif

#define DMA0_PERIPHERAL_REQUEST_CCU41_SR1_7   DMA_PERIPHERAL_REQUEST(7, 6)

#if ((UC_SERIES == XMC48) || (UC_SERIES == XMC47) || (UC_SERIES == XMC45) || (UC_SERIES == XMC44))
#define DMA0_PERIPHERAL_REQUEST_CCU81_SR1_7   DMA_PERIPHERAL_REQUEST(7, 7)
#endif

#define DMA0_PERIPHERAL_REQUEST_CAN_SR3_7     DMA_PERIPHERAL_REQUEST(7, 9)
#define DMA0_PERIPHERAL_REQUEST_USIC0_SR1_7   DMA_PERIPHERAL_REQUEST(7, 10)
#define DMA0_PERIPHERAL_REQUEST_USIC1_SR1_7   DMA_PERIPHERAL_REQUEST(7, 11)

#if ((UC_SERIES == XMC48) || (UC_SERIES == XMC47) || (UC_SERIES == XMC45) || (UC_SERIES == XMC44))
#define DMA0_PERIPHERAL_REQUEST_VADC_G3SR3_7  DMA_PERIPHERAL_REQUEST(7, 13)
#define DMA0_PERIPHERAL_REQUEST_CCU43_SR1_7   DMA_PERIPHERAL_REQUEST(7, 14)
#endif

#if ((UC_SERIES == XMC44) || (UC_SERIES == XMC42) || (UC_SERIES == XMC41))
#define DMA0_PERIPHERAL_REQUEST_HRPWM0_SR1_7  DMA_PERIPHERAL_REQUEST(7, 15)
#endif

#if ((UC_SERIES == XMC48) || (UC_SERIES == XMC47) || UC_SERIES == XMC45)
/*
 * DMA LINE 0 of DMA1
 */

#define DMA1_PERIPHERAL_REQUEST_ERU0_SR0_8    DMA_PERIPHERAL_REQUEST(0, 0)
#define DMA1_PERIPHERAL_REQUEST_VADC_C0SR0_8  DMA_PERIPHERAL_REQUEST(0, 1)
#define DMA1_PERIPHERAL_REQUEST_VADC_G3SR0_8  DMA_PERIPHERAL_REQUEST(0, 2)
#define DMA1_PERIPHERAL_REQUEST_DSD_SRM0_8    DMA_PERIPHERAL_REQUEST(0, 3)
#define DMA1_PERIPHERAL_REQUEST_DAC_SR0_8     DMA_PERIPHERAL_REQUEST(0, 4)
#define DMA1_PERIPHERAL_REQUEST_CCU42_SR0_8   DMA_PERIPHERAL_REQUEST(0, 5)
#define DMA1_PERIPHERAL_REQUEST_USIC2_SR0_8   DMA_PERIPHERAL_REQUEST(0, 6)
#define DMA1_PERIPHERAL_REQUEST_USIC2_SR2_8   DMA_PERIPHERAL_REQUEST(0, 7)

/*
 * DMA LINE 1 of DMA1
 */

#define DMA1_PERIPHERAL_REQUEST_ERU0_SR1_9    DMA_PERIPHERAL_REQUEST(1, 0)
#define DMA1_PERIPHERAL_REQUEST_VADC_C0SR1_9  DMA_PERIPHERAL_REQUEST(1, 1)
#define DMA1_PERIPHERAL_REQUEST_VADC_G3SR1_9  DMA_PERIPHERAL_REQUEST(1, 2)
#define DMA1_PERIPHERAL_REQUEST_DSD_SRM1_9    DMA_PERIPHERAL_REQUEST(1, 3)
#define DMA1_PERIPHERAL_REQUEST_DAC_SR1_9     DMA_PERIPHERAL_REQUEST(1, 4)
#define DMA1_PERIPHERAL_REQUEST_CCU42_SR1_9   DMA_PERIPHERAL_REQUEST(1, 5)
#define DMA1_PERIPHERAL_REQUEST_USIC2_SR1_9   DMA_PERIPHERAL_REQUEST(1, 6)
#define DMA1_PERIPHERAL_REQUEST_USIC2_SR3_9   DMA_PERIPHERAL_REQUEST(1, 7)

/*
 * DMA LINE 2 of DMA1
 */

#define DMA1_PERIPHERAL_REQUEST_ERU0_SR2_10   DMA_PERIPHERAL_REQUEST(2, 0)
#define DMA1_PERIPHERAL_REQUEST_VADC_C0SR2_10 DMA_PERIPHERAL_REQUEST(2, 1)
#define DMA1_PERIPHERAL_REQUEST_VADC_G3SR2_10 DMA_PERIPHERAL_REQUEST(2, 2)
#define DMA1_PERIPHERAL_REQUEST_DSD_SRM2_10   DMA_PERIPHERAL_REQUEST(2, 3)
#define DMA1_PERIPHERAL_REQUEST_DAC_SR0_10    DMA_PERIPHERAL_REQUEST(2, 4)
#define DMA1_PERIPHERAL_REQUEST_CCU43_SR0_10  DMA_PERIPHERAL_REQUEST(2, 5)
#define DMA1_PERIPHERAL_REQUEST_USIC2_SR0_10  DMA_PERIPHERAL_REQUEST(2, 6)
#define DMA1_PERIPHERAL_REQUEST_USIC2_SR2_10  DMA_PERIPHERAL_REQUEST(2, 7)

/*
 * DMA LINE 3 of DMA1
 */

#define DMA1_PERIPHERAL_REQUEST_ERU0_SR3_11   DMA_PERIPHERAL_REQUEST(3, 0)
#define DMA1_PERIPHERAL_REQUEST_VADC_C0SR3_11 DMA_PERIPHERAL_REQUEST(3, 1)
#define DMA1_PERIPHERAL_REQUEST_VADC_G3SR3_11 DMA_PERIPHERAL_REQUEST(3, 2)
#define DMA1_PERIPHERAL_REQUEST_DSD_SRM3_11   DMA_PERIPHERAL_REQUEST(3, 3)
#define DMA1_PERIPHERAL_REQUEST_DAC_SR1_11    DMA_PERIPHERAL_REQUEST(3, 4)
#define DMA1_PERIPHERAL_REQUEST_CCU43_SR1_11  DMA_PERIPHERAL_REQUEST(3, 5)
#define DMA1_PERIPHERAL_REQUEST_USIC2_SR1_11  DMA_PERIPHERAL_REQUEST(3, 6)
#define DMA1_PERIPHERAL_REQUEST_USIC2_SR3_11  DMA_PERIPHERAL_REQUEST(3, 7)

#endif /* (UC_SERIES == XMC45) */

#endif /* XMC_DMA_MAP_H */
