/*
 * Copyright (c) 2016, Freescale Semiconductor, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of Freescale Semiconductor, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _FSL_USART_CMSIS_H_
#define _FSL_USART_CMSIS_H_

#include "fsl_common.h"
#include "Driver_USART.h"
#include "RTE_Device.h"
#include "fsl_usart.h"
#if (defined(FSL_FEATURE_SOC_DMA_COUNT) && FSL_FEATURE_SOC_DMA_COUNT)
#include "fsl_usart_dma.h"
#endif

#if defined(USART0)
extern ARM_DRIVER_USART USART0_NonBlocking_Driver;
#endif /* USART0 */

#if defined(USART1)
extern ARM_DRIVER_USART USART1_NonBlocking_Driver;
#endif /* USART1 */

#if defined(USART2)
extern ARM_DRIVER_USART USART2_NonBlocking_Driver;
#endif /* USART2 */

#if defined(USART3)
extern ARM_DRIVER_USART USART3_NonBlocking_Driver;
#endif /* USART3 */

#if defined(USART4)
extern ARM_DRIVER_USART USART4_NonBlocking_Driver;
#endif /* USART4 */

#if defined(USART5)
extern ARM_DRIVER_USART USART5_NonBlocking_Driver;
#endif /* USART5 */

#if defined(USART6)
extern ARM_DRIVER_USART USART6_NonBlocking_Driver;
#endif /* USART6 */

#if defined(USART7)
extern ARM_DRIVER_USART USART7_NonBlocking_Driver;
#endif /* USART7 */

#if (defined(FSL_FEATURE_SOC_DMA_COUNT) && FSL_FEATURE_SOC_DMA_COUNT)

#if defined(USART0)
extern ARM_DRIVER_USART USART0_Dma_Driver;
#endif /* USART0 */

#if defined(USART1)
extern ARM_DRIVER_USART USART1_Dma_Driver;
#endif /* USART1 */

#if defined(USART2)
extern ARM_DRIVER_USART USART2_Dma_Driver;
#endif /* USART2 */

#if defined(USART3)
extern ARM_DRIVER_USART USART3_Dma_Driver;
#endif /* USART3 */

#if defined(USART4)
extern ARM_DRIVER_USART USART4_Dma_Driver;
#endif /* USART4 */

#if defined(USART5)
extern ARM_DRIVER_USART USART5_Dma_Driver;
#endif /* USART5 */

#if defined(USART6)
extern ARM_DRIVER_USART USART6_Dma_Driver;
#endif /* USART6 */

#if defined(USART7)
extern ARM_DRIVER_USART USART7_Dma_Driver;
#endif /* USART7 */

#endif /* FSL_FEATURE_SOC_DMA_COUNT */

#endif /* _FSL_USART_CMSIS_H_ */
