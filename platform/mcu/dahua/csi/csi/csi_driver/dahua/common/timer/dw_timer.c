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
 * @file     dw_timer.c
 * @brief    CSI Source File for timer Driver
 * @version  V1.0
 * @date     02. June 2017
 ******************************************************************************/
#include "drv_intc.h"
#include "drv_timer.h"
#include "dw_timer.h"
#include "soc.h"
#include "dh_type.h"

#define DRV_TIMER_TIMEOUT_US(useconds)          ((useconds)*(24))

inline DH_S32 TIMER_DRV_ClrInt(DH_U32 u32TimerId);
void dw_timer0_irqhandler(void)
{
    TIMER_DRV_ClrInt(0);
}


/*******************************************************************************
* 函数名  : TIMER_DRV_ClrInt
* 描  述  :
* 输  入  : - u32TimerId:定时器ID
* 输  出  : 无
* 返回值  : OSA_SOK  : 成功
*           OSA_EFAIL: 失败
*******************************************************************************/
inline DH_S32 TIMER_DRV_ClrInt(DH_U32 u32TimerId)
{

    DHC_TIMER_REG_EOI(u32TimerId);

    return DH_SUCCESS;
}


/*******************************************************************************
* 函数名  : TIMER_DRV_ConfigMode
* 描  述  :
* 输  入  : - u32TimerId:定时器ID
* 输  出  : 无
* 返回值  : DH_SUCCESS  : 成功
*           DH_FAILURE: 失败
*******************************************************************************/
DH_S32 TIMER_DRV_ConfigMode(DH_U32 u32TimerId, DH_U32 u32Mode)
{
    DH_U32 u32Val = 0;
    
    switch (u32Mode & 0x1)
    {
       case TIMER_MODE_ONETIME:
       {
           u32Val = DHC_TIMER_REG_CONTROL(u32TimerId);
           u32Val &= ~TIMER_RELODE_MODE;
           DHC_TIMER_REG_CONTROL(u32TimerId) = u32Val;
           break;
       }

       case TIMER_MODE_RELOAD:
       {
           u32Val = DHC_TIMER_REG_CONTROL(u32TimerId);
           u32Val |= TIMER_RELODE_MODE;
           DHC_TIMER_REG_CONTROL(u32TimerId) = u32Val;
           break;
       }
       default:
       {
           break;
       }
    }

    return DH_SUCCESS;
}


/*******************************************************************************
* 函数名  : TIMER_DRV_Start
* 描  述  : 启动定时器
* 输  入  : - u32TimerId:定时器ID
*         : - timeout:定时时间 微妙(us)为单位
* 输  出  : 无
* 返回值  : DH_SUCCESS  : 成功
*           DH_FAILURE: 失败
*******************************************************************************/
DH_S32 TIMER_DRV_Start(DH_U32 u32TimerId, DH_U32 timeout)
{
    DH_U32 u32WaitTime = 0;
    DH_U32 u32Val = 0;

    u32WaitTime = DRV_TIMER_TIMEOUT_US(timeout);

    DHC_TIMER_REG_LOAD_COUNT(u32TimerId) = (u32WaitTime);

    u32Val = DHC_TIMER_REG_CONTROL(u32TimerId);

    u32Val |= TIMER_ENABLE;
    u32Val &= ~TIMER_MASK;

    DHC_TIMER_REG_CONTROL(u32TimerId) = u32Val;

    return DH_SUCCESS;
}


/*******************************************************************************
* 函数名  : TIMER_DRV_Stop
* 描  述  :
* 输  入  : - u32TimerId:定时器ID
* 输  出  : 无
* 返回值  : DH_SUCCESS  : 成功
*           DH_FAILURE: 失败
*******************************************************************************/
DH_S32 TIMER_DRV_Stop(DH_U32 u32TimerId)
{
    DH_U32 u32Val = 0;

    u32Val = DHC_TIMER_REG_CONTROL(u32TimerId);

    u32Val &= ~TIMER_ENABLE;
    u32Val |= TIMER_MASK;

    DHC_TIMER_REG_CONTROL(u32TimerId) = u32Val;

    return DH_SUCCESS;
}


/*******************************************************************************
*  函数名   : TIMER_DRV_Reload
*  描  述   : 在RELOAD模式下，实时修改定时周期
*  输  入   : hTimer: 定时器句柄
*           : u32Timeout: 定时周期 (微秒)
*  输  出   : 无。
*  返回值   : DH_SUCCESS: 成功。
*             DH_FAILURE: 失败。
*******************************************************************************/
DH_S32 TIMER_DRV_Reload(DH_U32 u32TimerId, DH_U32 u32Timeout)
{
    DH_U32 u32WaitTime = 0;

    if(0 == u32Timeout)
    {
        return DH_FAILURE;
    }

    u32WaitTime = DRV_TIMER_TIMEOUT_US(u32Timeout);

    DHC_TIMER_REG_LOAD_COUNT(u32TimerId) = (u32WaitTime);

    return DH_SUCCESS;
}


/*******************************************************************************
* 函数名  : TIMER_DRV_Reset
* 描  述  :
* 输  入  : - u32TimerId:定时器ID
* 输  出  : 无
* 返回值  : DH_SUCCESS  : 成功
*           DH_FAILURE: 失败
*******************************************************************************/
DH_S32 TIMER_DRV_Reset(DH_U32 u32TimerId)
{
    /* 屏蔽中断，关使能 */
    DHC_TIMER_REG_CONTROL(u32TimerId) = 0x4;

    /* 清中断 */
    DHC_TIMER_REG_EOI(u32TimerId);

    return DH_SUCCESS;
}


