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
 * @file     zx29_iic.h
 * @brief    header File for IIC Driver
 * @version  V1.0
 * @date     02. June 2017
 ******************************************************************************/
#ifndef __ZX29_IIC_H
#define __ZX29_IIC_H

#include "soc.h"
#include "io.h"

#define I2C_WCLK_SEL_LF          WCLK_I2C_19M2
#define I2C_WCLK_SEL_HF          WCLK_I2C_76M8

#define SI_IRQ_NUM_I2C0          PIC_IRQ_I2C0
#define SI_IRQ_NUM_I2C1          ICU_IRQ_I2C1

#define I2C_WCLK_LF              (19200 * 1000)
#define I2C_WCLK_HF              (76800 * 1000)
#define I2C_WCLK_RATE            (I2C_WCLK_LF)

#define I2C_SCL_FREQ_STANDARD    (100 * 1000)
#define I2C_SCL_FREQ_FAST        (400 * 1000)

#define I2C_CLK_DIV_STANDARD    ((I2C_WCLK_RATE / 4) / I2C_SCL_FREQ_STANDARD - 1)
#define I2C_CLK_DIV_FAST        ((I2C_WCLK_RATE / 4) / I2C_SCL_FREQ_FAST - 1)


/* si i2c reg [I2C_CMD][0x04] */
#define I2C_CMD_IE_SLV_STOP          BIT(12)
#define I2C_CMD_IE_RXF_FULL          BIT(11)
#define I2C_CMD_IE_TXF_EMPTY         BIT(10)
#define I2C_CMD_IE_TIME_OUT          BIT(9)
#define I2C_CMD_IE_MST_STOP          BIT(8)
#define I2C_CMD_START                BIT(6)
#define I2C_CMD_CMB_RW               BIT(5)
#define I2C_CMD_I2C_RW               BIT(4)
#define I2C_CMD_ADDR_MODE            BIT(1)
#define I2C_CMD_MS_SEL               BIT(0)
#define I2C_CMD_IE                   (I2C_CMD_IE_RXF_FULL    | \
                                      I2C_CMD_IE_TXF_EMPTY    | \
                                      I2C_CMD_IE_MST_STOP)

/* si i2c reg [TX_CTRL][0x1c] */
#define I2C_TX_CTRL_FIFO_RST         BIT(16)
#define I2C_TX_CTRL_QTY_SHIFT        0
#define I2C_TX_CTRL_QTY_MASK         (BIT_MASK(16) << I2C_TX_CTRL_QTY_SHIFT)

/* si i2c reg [RX_CTRL][0x20] */
#define I2C_RX_CTRL_FIFO_RST         BIT(16)
#define I2C_RX_CTRL_QTY_SHIFT        0
#define I2C_RX_CTRL_QTY_MASK         (BIT_MASK(16) << I2C_RX_CTRL_QTY_SHIFT)

/* si i2c reg [I2C_STATUS][0x28] */
#define I2C_STATUS_S_BUSY            BIT(7)
#define I2C_STATUS_I_SLV_STOP        BIT(6)
#define I2C_STATUS_I_RXF_FULL        BIT(5)
#define I2C_STATUS_I_TXF_EMPTY       BIT(4)
#define I2C_STATUS_I_TIME_OUT        BIT(3)
#define I2C_STATUS_S_ERR_DATA        BIT(2)
#define I2C_STATUS_S_ERR_DEVICE      BIT(1)
#define I2C_STATUS_I_MST_STOP        BIT(0)
#define I2C_STATUS_CLEAR             0x7F
#define I2C_STATUS_ERR               (I2C_STATUS_S_ERR_DATA    | I2C_STATUS_S_ERR_DEVICE)
#define I2C_STATUS_INT               (I2C_STATUS_I_RXF_FULL    | \
                                      I2C_STATUS_I_TXF_EMPTY    | \
                                      I2C_STATUS_I_MST_STOP)

/* si i2c reg [TXF_STATUS][0x2c] */
#define I2C_TXF_STAT_CNT_SHIFT       2
#define I2C_TXF_STAT_CNT_MASK        (BIT_MASK(6) << I2C_TXF_STAT_CNT_SHIFT)
#define I2C_TXF_STAT_EMPTY           BIT(1)
#define I2C_TXF_STAT_FULL            BIT(0)

/* si i2c reg [RXF_STATUS][0x30] */
#define I2C_RXF_STAT_CNT_SHIFT       2
#define I2C_RXF_STAT_CNT_MASK        (BIT_MASK(6) << I2C_RXF_STAT_CNT_SHIFT)
#define I2C_RXF_STAT_EMPTY           BIT(1)
#define I2C_RXF_STAT_FULL            BIT(0)

#define I2C_FIFO_DEPTH               32
#define I2C_TRANS_LEN_MAX            0xFFFF

enum i2c_state_e {
    IIC_STATE_NONE = 0,          /* Send start + (first part of) address. */
    IIC_STATE_DATASEND,          /* Send data. */
    IIC_STATE_WFDATA,            /* Wait for data. */
    IIC_STATE_WFSTOPSENT,        /* Wait for STOP to have been transmitted. */
    IIC_STATE_DONE,              /* Transfer completed successfully. */
    IIC_STATE_ERROR              /* Transfer error. */
};

typedef struct {
    __IOM uint32_t    version;        /* Offset: 0x000 (R/W) */
    __IOM uint32_t    cmd;            /* Offset: 0x004 (R/W) */
    __IOM uint32_t    reserved0;
    __IOM uint32_t    device;         /* Offset: 0x00c (R/W) */
    __IOM uint32_t    reserved1;
    __IOM uint32_t    clk_div;        /* Offset: 0x014 (R/W) */
    __IOM uint32_t    reserved2;
    __IOM uint32_t    tx_ctrl;        /* Offset: 0x01c (R/W) */
    __IOM uint32_t    rx_ctrl;        /* Offset: 0x020 (R/W) */
    __IOM uint32_t    data;           /* Offset: 0x024 (R/W) */
    __IOM uint32_t    status;         /* Offset: 0x028 (R/W) */
    __IOM uint32_t    txf_stat;       /* Offset: 0x02c (R/W) */
    __IOM uint32_t    rxf_stat;       /* Offset: 0x030 (R/W) */
    __IOM uint32_t    debug;          /* Offset: 0x034 (R/W) */
} zx29_iic_reg_t;

#endif /* __ZX29_IIC_H */
