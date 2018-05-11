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
 * @file     ck_usi.h
 * @brief    header file for usi driver
 * @version  V1.0
 * @date     02. June 2017
 ******************************************************************************/
#ifndef _CK_USI_H_
#define _CK_USI_H_

#include <stdio.h>
#include "errno.h"
#include "soc.h"
#define USI_RX_FIFO_EN          (0x1 << 3)
#define USI_TX_FIFO_EN          (0x1 << 2)
#define USI_FM_EN               (0x1 << 1)
#define USI_USI_EN              (0x1 << 0)

#define USI_FIFO_STA_RX_NUM(addr)      ((addr->USI_FIFO_STA & (0x1F << 16)) >> 16)
#define USI_FIFO_STA_TX_NUM(addr)      ((addr->USI_FIFO_STA & (0x1F << 8)) >> 8)

#define USI_FIFO_STA_RX_FULL     (0x1 << 3)
#define USI_FIFO_STA_RX_EMPTY    (0x1 << 2)
#define USI_FIFO_STA_TX_FULL     (0x1 << 1)
#define USI_FIFO_STA_TX_EMPTY    (0x1 << 0)

#define USI_INT_SPI_STOP         (0x1 << 17)
#define USI_INT_I2C_AERR         (0x1 << 16)
#define USI_INT_I2CS_GCALL       (0x1 << 15)
#define USI_INT_I2CM_LOSE_ARBI   (0x1 << 14)
#define USI_INT_I2C_NACK         (0x1 << 13)
#define USI_INT_I2C_STOP         (0x1 << 12)
#define USI_INT_UART_PERR        (0x1 << 11)
#define USI_INT_UART_STOP        (0x1 << 10)
#define USI_INT_RX_WERR          (0x1 << 9)
#define USI_INT_RX_RERR          (0x1 << 8)
#define USI_INT_RX_FULL          (0x1 << 7)
#define USI_INT_RX_EMPTY         (0x1 << 6)
#define USI_INT_RX_THOLD         (0x1 << 5)
#define USI_INT_TX_WERR          (0x1 << 4)
#define USI_INT_TX_RERR          (0x1 << 3)
#define USI_INT_TX_FULL          (0x1 << 2)
#define USI_INT_TX_EMPTY         (0x1 << 1)
#define USI_INT_TX_THOLD         (0x1 << 0)

#define USI_MODE_UART    0x0
#define USI_MODE_I2C     0x1
#define USI_MODE_SPI     0x2

#define USI_TX_MAX_FIFO        0x10
#define USI_RX_MAX_FIFO        0x10

#define USI_INTR_CTRL_TH_MODE       (1 << 16)
#define USI_INTR_CTRL_RXFIFO_TH     0xfffffcff
#define USI_INTR_CTRL_RXFIFO_TH_4   (0x1 << 8)  // 4-byte data
#define USI_INTR_CTRL_RXFIFO_TH_8   (0x2 << 8)  // 8-byte data
#define USI_INTR_CTRL_RXFIFO_TH_12  (0x3 << 8)  // 12-byte data
#define USI_INTR_CTRL_TXFIFO_TH     0xfffffffc
#define USI_INTR_CTRL_TXFIFO_TH_4   (0x1 << 0)  // 4-byte data
#define USI_INTR_CTRL_TXFIFO_TH_8   (0x2 << 0)  // 8-byte data
#define USI_INTR_CTRL_TXFIFO_TH_12  (0x3 << 0)  // 12-byte data

typedef struct {
    __IOM uint32_t USI_EN;                 /* Offset 0x000(R/W) */
    __IOM uint32_t USI_MODE_SEL;           /* Offset 0x004(R/W) */
    __IOM uint32_t USI_TX_RX_FIFO;         /* Offset 0x008(R/W) */
    __IOM uint32_t USI_FIFO_STA;           /* Offset 0x00c(R/W) */
    __IOM uint32_t USI_CLK_DIV0;           /* Offset 0x010(R/W) */
    __IOM uint32_t USI_CLK_DIV1;           /* Offset 0x014(R/W) */
    __IOM uint32_t USI_UART_CTRL;          /* Offset 0x018(R/W) */
    __IOM uint32_t USI_UART_STA;           /* Offset 0x01c(R/W) */
    __IOM uint32_t USI_I2C_MODE;           /* Offset 0x020(R/W) */
    __IOM uint32_t USI_I2C_ADDR;           /* Offset 0x024(R/W) */
    __IOM uint32_t USI_I2CM_CTRL;          /* Offset 0x028(R/W) */
    __IOM uint32_t USI_I2CM_CODE;          /* Offset 0x02c(R/W) */
    __IOM uint32_t USI_I2CS_CTRL;          /* Offset 0x030(R/W) */
    __IOM uint32_t USI_I2C_FM_DIV;         /* Offset 0x034(R/W) */
    __IOM uint32_t USI_I2C_HOLD;           /* Offset 0x038(R/W) */
    __IOM uint32_t USI_I2C_STA;            /* Offset 0x03c(R/W) */
    __IOM uint32_t USI_SPI_MODE;           /* Offset 0x040(R/W) */
    __IOM uint32_t USI_SPI_CTRL;           /* Offset 0x044(R/W) */
    __IOM uint32_t USI_SPI_STA;            /* Offset 0x048(R/W) */
    __IOM uint32_t USI_INTR_CTRL;          /* Offset 0x04c(R/W) */
    __IOM uint32_t USI_INTR_EN;            /* Offset 0x050(R/W) */
    __IOM uint32_t USI_INTR_STA;           /* Offset 0x054(R/W) */
    __IOM uint32_t USI_RAW_INTR_STA;       /* Offset 0x058(R/W) */
    __IOM uint32_t USI_INTR_UNMASK;        /* Offset 0x05c(R/W) */
    __IOM uint32_t USI_INTR_CLR;           /* Offset 0x060(R/W) */
    __IOM uint32_t USI_DMA_CTRL;           /* Offset 0x064(R/W) */
    __IOM uint32_t USI_DMA_THRESHOLD;      /* Offset 0x068(R/W) */
} ck_usi_reg_t;

void ck_usi_set_rxfifo_th(ck_usi_reg_t *addr, uint32_t length);
int32_t drv_usi_initialize(int32_t idx);
int32_t drv_usi_uninitialize(int32_t idx);
#endif /* _CK_USI_H_ */

