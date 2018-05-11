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
 * @file     ck_intc.h
 * @brief    C-SKY Peripheral Access Layer Header File
 * @version  V1.0
 * @date     02. Jan 2018
 ******************************************************************************/

#ifndef __CK_INTC_H__
#define __CK_INTC_H__

#include <stdint.h>
#include <soc.h>

#define INTC_BASE  CSKY_INTC_BASE
#define CSKY_INTC_PRIO_BASE     (CSKY_INTC_BASE + 0x40)
#define CSKY_INTC_NIER_BASE     (CSKY_INTC_BASE + 0x10)

#define INTC    ((ck_intc_reg_t *)INTC_BASE)

#define AVE_BIT    31

#define INTC_ICR_DEFAULT_VAL    0x00000000
#define PRIO_LOWEST     0
#define PRIO_HIGHEST    31
#define PRIO_REG_WIDTH  8

typedef struct {
    __IOM uint16_t ICR;           /* Offset: 0x000 (R/W) Enable the Interrupt Register */
    __IM uint16_t ISR;         /* Offset: 0x008 (R/W) Mask Interrupt Register */
    __IOM uint32_t IFR;            /* Offset: 0x010 (R/W) Enable  the Soft Interrupt Register */
    __IM uint32_t IPR;        /* Offset: 0x018 (R) Raw Interrupt Status Register */
    __IOM uint32_t NIER;           /* Offset: 0x020 (R) Interrupt Status Register */
    __IM uint32_t NIPR;       /* Offset: 0x028 (R) Mask Interrupt Status Register */
    __IOM uint32_t FIER;           /* Offset: 0x020 (R) Interrupt Status Register */
    __IM uint32_t FIPR;       /* Offset: 0x028 (R) Mask Interrupt Status Register */
    __IOM uint8_t PR0;     /* Offset: 0x040 (R/W) priority level select Register */
    __IOM uint8_t PR1;     /* Offset: 0x040 (R/W) priority level select Register */
    __IOM uint8_t PR2;     /* Offset: 0x040 (R/W) priority level select Register */
    __IOM uint8_t PR3;     /* Offset: 0x040 (R/W) priority level select Register */
    __IOM uint8_t PR4;     /* Offset: 0x040 (R/W) priority level select Register */
    __IOM uint8_t PR5;     /* Offset: 0x040 (R/W) priority level select Register */
    __IOM uint8_t PR6;     /* Offset: 0x040 (R/W) priority level select Register */
    __IOM uint8_t PR7;     /* Offset: 0x040 (R/W) priority level select Register */
    __IOM uint8_t PR8;     /* Offset: 0x040 (R/W) priority level select Register */
    __IOM uint8_t PR9;     /* Offset: 0x040 (R/W) priority level select Register */
    __IOM uint8_t PR10;     /* Offset: 0x040 (R/W) priority level select Register */
    __IOM uint8_t PR11;     /* Offset: 0x040 (R/W) priority level select Register */
    __IOM uint8_t PR12;     /* Offset: 0x040 (R/W) priority level select Register */
    __IOM uint8_t PR13;     /* Offset: 0x040 (R/W) priority level select Register */
    __IOM uint8_t PR14;     /* Offset: 0x040 (R/W) priority level select Register */
    __IOM uint8_t PR15;     /* Offset: 0x040 (R/W) priority level select Register */
    __IOM uint8_t PR16;     /* Offset: 0x040 (R/W) priority level select Register */
    __IOM uint8_t PR17;     /* Offset: 0x040 (R/W) priority level select Register */
    __IOM uint8_t PR18;     /* Offset: 0x040 (R/W) priority level select Register */
    __IOM uint8_t PR19;     /* Offset: 0x040 (R/W) priority level select Register */
    __IOM uint8_t PR20;     /* Offset: 0x040 (R/W) priority level select Register */
    __IOM uint8_t PR21;     /* Offset: 0x040 (R/W) priority level select Register */
    __IOM uint8_t PR22;     /* Offset: 0x040 (R/W) priority level select Register */
    __IOM uint8_t PR23;     /* Offset: 0x040 (R/W) priority level select Register */
    __IOM uint8_t PR24;     /* Offset: 0x040 (R/W) priority level select Register */
    __IOM uint8_t PR25;     /* Offset: 0x040 (R/W) priority level select Register */
    __IOM uint8_t PR26;     /* Offset: 0x040 (R/W) priority level select Register */
    __IOM uint8_t PR27;     /* Offset: 0x040 (R/W) priority level select Register */
    __IOM uint8_t PR28;     /* Offset: 0x040 (R/W) priority level select Register */
    __IOM uint8_t PR29;     /* Offset: 0x040 (R/W) priority level select Register */
    __IOM uint8_t PR30;     /* Offset: 0x040 (R/W) priority level select Register */
    __IOM uint8_t PR31;     /* Offset: 0x040 (R/W) priority level select Register */
} ck_intc_reg_t;

#endif /* __CK_INTC_H__ */
