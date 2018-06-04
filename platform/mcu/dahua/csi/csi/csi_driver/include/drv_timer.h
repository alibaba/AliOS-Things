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
 * @file     drv_timer.h
 * @brief    header file for timer driver
 * @version  V1.0
 * @date     02. June 2017
 ******************************************************************************/

#ifndef _CSI_TIMER_H_
#define _CSI_TIMER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <drv_common.h>
#include "dh_type.h"

typedef enum tagTimerMode
{
    TIMER_MODE_ONETIME = 0,
    TIMER_MODE_RELOAD  = 1
} TIMER_MODE_E;



/*******************************************************************************
* 函数名  : TIMER_DRV_ConfigMode
* 描  述  :
* 输  入  : - u32TimerId:定时器ID
* 输  出  : 无
* 返回值  : DH_SUCCESS  : 成功
*           DH_FAILURE: 失败
*******************************************************************************/
DH_S32 TIMER_DRV_ConfigMode(DH_U32 u32TimerId, DH_U32 u32Mode);

/*******************************************************************************
* 函数名  : TIMER_DRV_Start
* 描  述  : 启动定时器
* 输  入  : - u32TimerId:定时器ID
*         : - timeout:定时时间 微妙(us)为单位
* 输  出  : 无
* 返回值  : DH_SUCCESS  : 成功
*           DH_FAILURE: 失败
*******************************************************************************/
DH_S32 TIMER_DRV_Start(DH_U32 u32TimerId, DH_U32 timeout);

/*******************************************************************************
* 函数名  : TIMER_DRV_Stop
* 描  述  :
* 输  入  : - u32TimerId:定时器ID
* 输  出  : 无
* 返回值  : DH_SUCCESS  : 成功
*           DH_FAILURE: 失败
*******************************************************************************/
DH_S32 TIMER_DRV_Stop(DH_U32 u32TimerId);


/*******************************************************************************
*  函数名   : TIMER_DRV_Reload
*  描  述   : 在RELOAD模式下，实时修改定时周期
*  输  入   : hTimer: 定时器句柄
*           : u32Timeout: 定时周期 (微秒)
*  输  出   : 无。
*  返回值   : DH_SUCCESS: 成功。
*             DH_FAILURE: 失败。
*******************************************************************************/
DH_S32 TIMER_DRV_Reload(DH_U32 u32TimerId, DH_U32 u32Timeout);



/*******************************************************************************
* 函数名  : TIMER_DRV_Reset
* 描  述  :
* 输  入  : - u32TimerId:定时器ID
* 输  出  : 无
* 返回值  : DH_SUCCESS  : 成功
*           DH_FAILURE: 失败
*******************************************************************************/
DH_S32 TIMER_DRV_Reset(DH_U32 u32TimerId);


#ifdef __cplusplus
}
#endif

#endif /* _CSI_TIMER_H_ */

