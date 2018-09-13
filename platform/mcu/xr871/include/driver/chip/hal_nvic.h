/**
  * @file  hal_nvic.h
  * @author  XRADIO IOT WLAN Team
  */

/*
 * Copyright (C) 2017 XRADIO TECHNOLOGY CO., LTD. All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *    1. Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *    2. Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the
 *       distribution.
 *    3. Neither the name of XRADIO TECHNOLOGY CO., LTD. nor the names of
 *       its contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _DRIVER_CHIP_HAL_NVIC_H_
#define _DRIVER_CHIP_HAL_NVIC_H_

#include "driver/chip/hal_def.h"

#ifdef __cplusplus
extern "C" {
#endif

#define NVIC_PRIORITYGROUP_0    (0x00000007U) /*!< 0 bits for preempt priority, 3 bits for subpriority */
#define NVIC_PRIORITYGROUP_1    (0x00000006U) /*!< 1 bits for preempt priority, 2 bits for subpriority */
#define NVIC_PRIORITYGROUP_2    (0x00000005U) /*!< 2 bits for preempt priority, 1 bits for subpriority */
#define NVIC_PRIORITYGROUP_3    (0x00000004U) /*!< 3 bits for preempt priority, 0 bits for subpriority */

/* Default NVIC priority grouping */
#define NVIC_PRIORITYGROUP_DEFAULT  NVIC_PRIORITYGROUP_3

/**
 * @brief Default priority for all peripherals
 * @note
 *     - Rang from 0 to 7 due to __NVIC_PRIO_BITS is 3
 *     - Should be set to [1, 7], MUST not be set to 0
 */
#define NVIC_PERIPHERAL_PRIORITY_DEFAULT    (4)

/** @brief Type define of NVIC interrupt handler */
typedef void (*NVIC_IRQHandler) (void);

void HAL_NVIC_SetIRQHandler(IRQn_Type IRQn, NVIC_IRQHandler handler);
NVIC_IRQHandler HAL_NVIC_GetIRQHandler(IRQn_Type IRQn);

void HAL_NVIC_SetPriorityGrouping(uint32_t priorityGroup);
uint32_t HAL_NVIC_GetPriorityGrouping(void);

void HAL_NVIC_SetPriority(IRQn_Type IRQn, uint32_t priority);
uint32_t HAL_NVIC_GetPriority(IRQn_Type IRQn);

void HAL_NVIC_EnableIRQ(IRQn_Type IRQn);
void HAL_NVIC_DisableIRQ(IRQn_Type IRQn);

void HAL_NVIC_SetPendingIRQ(IRQn_Type IRQn);
int HAL_NVIC_IsPendingIRQ(IRQn_Type IRQn);
void HAL_NVIC_ClearPendingIRQ(IRQn_Type IRQn);

void HAL_NVIC_Init(void);

#ifdef __cplusplus
}
#endif

#endif /* _DRIVER_CHIP_HAL_NVIC_H_ */
