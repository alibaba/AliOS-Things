/**
 * File: icp_si.h
 * Brief: bsp APIs of Sanechips icp
 *
 * Copyright (C) 2017 Sanechips Technology Co., Ltd.
 * Author:
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */


#ifndef _ICP_SI_H_
#define _ICP_SI_H_

/*******************************************************************************
 *                           Include header files                              *
 ******************************************************************************/
//#include "chip.h"
#include "drvs_irq.h"
#include "bsp/yunos_bsp_errno.h"
#include "reg_define_si.h"
#include <stdint.h>


/*******************************************************************************
 *                             Macro definitions                               *
 ******************************************************************************/
#define DEVICE_COUNT           (3)
#define CHANNEL_COUNT          (32)
#define ZX29_ICP_TO_M0_BASE    (ICP_BASEADDR + 0x8)
#define ZX29_ICP_TO_CEVA_BASE  (ICP_BASEADDR + 0x38)

/*******************************************************************************
 *                             Type definitions                                *
 ******************************************************************************/
struct icp_reg {
    uint32_t    config;     /*ÅäÖÃ¼Ä´æÆ÷            0x008*/
    uint32_t    sta_out;    /*±¾´¦ÀíÆ÷ÖÐ¶Ï×´Ì¬¼Ä´æÆ÷    0x00c*/
    uint32_t    clear;      /*ÖÐ¶ÏÇå³ý¼Ä´æÆ÷        0x010*/
    uint32_t    mask;       /*ÖÐ¶ÏÆÁ±Î¼Ä´æÆ÷        0x014*/
    uint32_t    sfn;        /*Ö¡ºÅ¼Ä´æÆ÷            0x018*/
    uint32_t    sta_in;     /*¶Ô¶Ë´¦ÀíÆ÷ÖÐ¶Ï×´Ì¬¼Ä´æÆ÷  0x01c*/
};

typedef void (*icp_isr_t)(void);

#endif /* _ICP_SI_H_ */


