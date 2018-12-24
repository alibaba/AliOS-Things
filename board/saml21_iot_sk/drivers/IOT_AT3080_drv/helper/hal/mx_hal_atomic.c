/**
 ******************************************************************************
 * @file    mx_hal_atomic.c
 * @author  William Xu
 * @version V1.0.0
 * @date    9-Apr-2018
 * @brief   Critical section enter and leave
 ******************************************************************************
 *
 * Copyright (c) 2009-2018 MXCHIP Co.,Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ******************************************************************************
 */

#include "mx_hal.h"
#include "cmsis_ccs.h"

/******************************************************************************
 *                              Function Definitions
 ******************************************************************************/

/**
 * \brief Disable interrupts, enter critical section
 */
void atomic_enter_critical(hal_atomic_t volatile *atomic)
{
	*atomic = __get_PRIMASK();
	__disable_irq();
	__DMB();
}

/**
 * \brief Exit atomic section
 */
void atomic_leave_critical(hal_atomic_t volatile *atomic)
{
	__DMB();
	__set_PRIMASK(*atomic);
}
