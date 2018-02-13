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
* @file     ck_dmac_v2.h
* @brief    header file for DMAC Driver
* @version  V1.0
* @date     02. June 2017
******************************************************************************/
#ifndef __CK_DMAC_V2_H
#define __CK_DMAC_V2_H
#include <stdio.h>
#include <soc.h>

#define CK_DMA_MAXCHANNEL  16

#define CK_DMA_INT_EN      1
#define CK_DMA_CH_EN       1
#define CK_DMA_CH_REQ      1
#define CK_DMACCFG_EN      1

#define CK_DMA_TRGETCMPFR  0x8
#define CK_DMA_HTFR        0x4
#define CK_DMA_TFR         0x2
#define CK_DMA_ERR         0x1

#define CK_DMA_INTC        0x1f
#define CK_DMA_MASK        0x00

#define CK_CHINTM_RESETVALUE    0x0
#define CK_CHINTC_RESETVALUE    0x0

#define CK_DMA_ALL_CAHNNELS_PEND_INTR_VALID          0x0000ffff
#define CK_DMA_ALL_CAHNNELS_DATA_TRANS_BUSY_VALID    0x0000ffff

typedef enum {
    DMA_ADDR_INCREMENT = 0,
    DMA_ADDR_DECREMENT = 1,
    DMA_ADDR_NOCHANGE  = 2
} enum_addr_state_e;

typedef enum {
    DMA_DATAWIDTH_SIZE8  = 1,
    DMA_DATAWIDTH_SIZE16 = 2,
    DMA_DATAWIDTH_SIZE32 = 3
} dma_datawidth_e;

typedef enum {
    DMA_GROUP_LEN1  = 1,
    DMA_GROUP_LEN2  = 2,
    DMA_GROUP_LEN3  = 3,
    DMA_GROUP_LEN4  = 4,
    DMA_GROUP_LEN5  = 5,
    DMA_GROUP_LEN6  = 6,
    DMA_GROUP_LEN7  = 7,
    DMA_GROUP_LEN8  = 8
} dma_group_len_e;

typedef enum {
    DMA_HANDSHAKING_HARDWARE = 0,
    DMA_HANDSHAKING_SOFTWARE = 1,
} dma_handshaking_select_e;

typedef enum {
    DMA_PRIORITY0 =  0,
    DMA_PRIORITY1 =  1,
    DMA_PRIOTITY2 =  2,
    DMA_PRIOTITY3 =  3,
    DMA_PRIOTITY4 =  4,
    DMA_PRIOTITY5 =  5,
    DMA_PRIOTITY6 =  6,
    DMA_PRIOTITY7 =  7,
    DMA_PRIOTITY8 =  8,
    DMA_PRIOTITY9 =  9,
    DMA_PRIOTITY10 =  10,
    DMA_PRIOTITY11 =  11,
    DMA_PRIOTITY12 =  12,
    DMA_PRIOTITY13 =  13,
    DMA_PRIOTITY14 =  14,
    DMA_PRIOTITY15 = 15
} dma_priority_t;

#define CK_V2_DMAC_STATUSCHPEND    (CSKY_DMAC0_BASE + 0x330)
#define CK_V2_DMAC_CHSR            (CSKY_DMAC0_BASE + 0x330 + 0x8)
#define CK_V2_DMAC_DMACCFG         (CSKY_DMAC0_BASE + 0x330 + 0xc)

typedef struct {
    __IOM uint32_t   SAR;       /* offset: 0x00 (R/W) Channel Source Address Register        */
    __IOM uint32_t   DAR;       /* offset: 0x04 (R/W) Channel Destination Address Register   */
    __IOM uint32_t   CHCTRLA;   /* offset: 0x08 (R/W) Channel Control Register A             */
    __IOM uint32_t   CHCTRLB;   /* offset: 0x0C (R/W) Channel Control Register B             */
    __IOM uint8_t   CHINTM:5;    /* offset: 0x10 (R/W) Channel Interrupt Mask Register        */
    uint8_t  RESERVED0[3];
    __IM  uint8_t   CHINTS:4;    /* offset: 0x14 (R/ ) Channel Interrupt Status Register      */
    uint8_t  RESERVED1[3];
    __IOM uint8_t   CHINTC:5;    /* offset: 0x18 (R/W) Channel Interrupt Clear Register       */
    uint8_t  RESERVED2[3];
    __IOM uint8_t   CHSREQ:1;    /* offset: 0x1C (R/W) Channel Software Request Register      */
    uint8_t  RESERVED3[3];
    __IOM uint8_t   CHEN:1;      /* offset: 0x20 (R/W) Channel Enable Register                */
    uint8_t  RESERVED4[3];
} ck_dma_reg_t;
#endif /* __CK_DMAC_V2_H */
