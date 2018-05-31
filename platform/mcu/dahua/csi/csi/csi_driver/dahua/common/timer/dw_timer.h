/*
 * Copyright (C) 2017 C-SKY Microsystems Co., Ltd. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
/******************************************************************************
 * @file     dw_timer.h
 * @brief    header file for timer driver
 * @version  V1.0
 * @date     02. June 2017
 ******************************************************************************/
#ifndef __DW_TIMER_H
#define __DW_TIMER_H

#include <stdio.h>
//#include "soc.h"

#define DHC_TIMER_REG_LOAD_COUNT(timerNum)       ( *( volatile unsigned int *)( DH5021A_TIM0_BASE +  0x00 + timerNum*0x14) )
#define DHC_TIMER_REG_CURRENT_VALUE(timerNum)    ( *( volatile unsigned int *)( DH5021A_TIM0_BASE +  0x04 + timerNum*0x14) )
#define DHC_TIMER_REG_CONTROL(timerNum)          ( *( volatile unsigned int *)( DH5021A_TIM0_BASE +  0x08 + timerNum*0x14) )
#define DHC_TIMER_REG_EOI(timerNum)              ( *( volatile unsigned int *)( DH5021A_TIM0_BASE +  0x0c + timerNum*0x14) )
#define DHC_TIMER_REG_INT_STATUS(timerNum)       ( *( volatile unsigned int *)( DH5021A_TIM0_BASE +  0x10 + timerNum*0x14) )

#define DHC_TIMERS_REG_INT_STATUS                ( *( volatile unsigned int *)( DH5021A_TIM0_BASE +  0xa0 ) )
#define DHC_TIMERS_REG_EOI                       ( *( volatile unsigned int *)( DH5021A_TIM0_BASE +  0xa4 ) )
#define DHC_TIMERS_REG_RAW_INT_STATUS            ( *( volatile unsigned int *)( DH5021A_TIM0_BASE +  0xa8 ) )


#define TIMER_ENABLE              (1 << 0)
#define TIMER_RELODE_MODE         (1 << 1)
#define TIMER_MASK                (1 << 2)

#endif /* __DW_TIMER_H */

