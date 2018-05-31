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
 * @file     dw_intc.h
 * @brief    CSI CK802 Core Peripheral Access Layer Header File
 * @version  V1.0
 * @date     02. June 2017
 ******************************************************************************/

#ifndef __DW_INTC_H
#define __DW_INTC_H

#include <stdint.h>
//#include <soc.h>

#ifdef CONFIG_CHIP_DH5010M

#else
#define INTC_BASE DH5021A_INTC_BASE

#define INTC    ((dw_intc_reg_t *)INTC_BASE)
typedef struct {
    __IOM uint32_t IRQ_INTEN_L;           /* Offset: 0x000 (R/W) Enable the Interrupt Register */
    __IOM uint32_t IRQ_INTEN_H;           /* Offset: 0x004 (R/W) Enable the Interrupt Register */
    __IOM uint32_t IRQ_INTMASK_L;         /* Offset: 0x008 (R/W) Mask Interrupt Register */
    __IOM uint32_t IRQ_INTMASK_H;         /* Offset: 0x00C (R/W) Mask Interrupt Register */
    __IOM uint32_t INTFORCE_L;            /* Offset: 0x010 (R/W) Enable  the Soft Interrupt Register */
    __IOM uint32_t INTFORCE_H;            /* Offset: 0x014 (R/W) Enable  the Soft Interrupt Register */
    __IM uint32_t IRG_RAWSTATUS_L;        /* Offset: 0x018 (R) Raw Interrupt Status Register */
    __IM uint32_t IRG_RAWSTATUS_H;        /* Offset: 0x01C (R) Raw Interrupt Status Register */
    __IM uint32_t IRQ_STATUS_L;           /* Offset: 0x020 (R) Interrupt Status Register */
    __IM uint32_t IRQ_STATUS_H;           /* Offset: 0x024 (R) Interrupt Status Register */
    __IM uint32_t IRQ_MASKSTATUS_L;       /* Offset: 0x028 (R) Mask Interrupt Status Register */
    __IM uint32_t IRQ_MASKSTATUS_H;       /* Offset: 0x02C (R) Mask Interrupt Status Register */
    __IOM uint32_t IRQ_FINALSTATUS_L;     /* Offset: 0x030 (R/W) Final Interrupt Status Register */
    __IOM uint32_t IRQ_FINALSTATUS_H;     /* Offset: 0x034 (R/W) Final Interrupt Status Register */
} dw_intc_reg_t;
#endif

#endif /* __DW_INTC_H */
