/**
  * @file  hal_i2s.h
  * @author  XRADIO IOT WLAN Team
  */

/*
 * Copyright (C) 2017 XRADIO TECHNOLOGY CO., LTD. All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *    1. Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *    2. Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the
 *       distribution.
 *    3. Neither the name of XRADIO TECHNOLOGY CO., LTD. nor the names of
 *       its contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _DRIVER_CHIP_HAL_I2S_H_
#define _DRIVER_CHIP_HAL_I2S_H_

#include <stdbool.h>
#include "driver/chip/hal_def.h"
#include "driver/chip/hal_gpio.h"
#include "driver/chip/hal_audio.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
  * Digital Audio Controller
  */
typedef struct
{
	__IO uint32_t DA_CTL;        /* I2S Control Register, Address offset: 0x00 */
	__IO uint32_t DA_FMT0;       /* I2S Format Register0, Address offset: 0x04 */
	__IO uint32_t DA_FMT1;       /* I2S Format Register1, Address offset: 0x08 */
	__IO uint32_t DA_ISTA;       /* I2S Interrupt Status Register,	Address offset: 0x0C */
	     uint32_t DA_RXFIFO;     /* I2S RX FIFO Register, Address offset: 0x10 */
	__IO uint32_t DA_FCTL;       /* I2S FIFO Control Register, Address offset: 0x14 */
	__I  uint32_t DA_FSTA;       /* I2S FIFO Stauts Register, Address offset: 0x18 */
	__IO uint32_t DA_INT;        /* I2S DMA&Interrupt Control Register, Address offset: 0x1C */
	__O  uint32_t DA_TXFIFO;     /* I2S TX FIFO Register, Address offset: 0x20 */
	__IO uint32_t DA_CLKD;       /* I2S Clock Divide Register, ddress offset: 0x24 */
	__IO uint32_t DA_TXCNT;      /* I2S TX Sample Counter Register,	Address offset: 0x28 */
	__IO uint32_t DA_RXCNT;      /* I2S RX Sample Counter Register,	Address offset: 0x2C */
	__IO uint32_t DA_CHCFG;      /* I2S Channel Configuration Register, Address offset: 0x30 */
	__IO uint32_t DA_TX0CHSEL;   /* I2S TX0 Channel Select Register, Address offset: 0x34 */
	__IO uint32_t DA_TX1CHSEL;   /* I2S TX1 Channel Select Register, Address offset: 0x38 */
	__IO uint32_t DA_TX2CHSEL;   /* I2S TX2 Channel Select Register, Address offset: 0x3C */
	__IO uint32_t DA_TX3CHSEL;   /* I2S TX3 Channel Select Register, Address offset: 0x40 */
	__IO uint32_t DA_TX0CHMAP;   /* I2S TX0 Channel Mapping Register, Address offset: 0x44 */
	__IO uint32_t DA_TX1CHMAP;   /* I2S TX1 Channel Mapping Register, Address offset: 0x48 */
	__IO uint32_t DA_TX2CHMAP;   /* I2S TX2 Channel Mapping Register, Address offset: 0x4C */
	__IO uint32_t DA_TX3CHMAP;   /* I2S TX3 Channel Mapping Register, Address offset: 0x50 */
	__IO uint32_t DA_RXCHSEL;    /* I2S RX Channel Select Register,	Address offset: 0x54 */
	__IO uint32_t DA_RXCHMAP;    /* I2S RX Channel Mapping Register, Address offset: 0x58 */
} I2S_T;

#define I2S                                            ((I2S_T *)I2S_BASE)

/*
 * Bits definition for I2S_CTL Control  Register (0X0000)
 */
#define I2S_GLOBE_EN_BIT                               HAL_BIT(0)
#define I2S_RX_EN_BIT                                  HAL_BIT(1)
#define I2S_TX_EN_BIT                                  HAL_BIT(2)
#define I2S_LOOP_TSET_EN_EN_BIT                        HAL_BIT(3)

#define I2S_MODE_SEL_SHIFT                             (4)
#define I2S_MODE_SEL_MASK                              (0x3 << I2S_MODE_SEL_SHIFT)
typedef enum {
	I2S_PCM_MODE   = (0x0U << I2S_MODE_SEL_SHIFT),
	I2S_LEFT_MODE  = (0x1U << I2S_MODE_SEL_SHIFT),
	I2S_RIGHT_MODE = (0x2U << I2S_MODE_SEL_SHIFT),
	RESERVE_VAL    = (0x3U << I2S_MODE_SEL_SHIFT)
} I2S_Mode;

#define I2S_OUT_MUTE_SHIFT                             (6)
#define I2S_OUT_MUTE_MASK                              (0x1U << I2S_OUT_MUTE_SHIFT)
typedef enum {
	I2S_NOR_TRANSFER  = (0x0U << I2S_OUT_MUTE_SHIFT),
	I2S_OUT_MUTE      = (0x1U << I2S_OUT_MUTE_SHIFT),
} I2S_DoutMute;

#define I2S_SDO0_EN_BIT                                HAL_BIT(8)

#define I2S_LRCK_OUT_SHIFT                             (17)
#define I2S_LRCK_OUT_MASK                              (0x1U << I2S_LRCK_OUT_SHIFT)
typedef enum {
	I2S_LRCK_INPUT  = (0x0U << I2S_LRCK_OUT_SHIFT),
	I2S_LRCK_OUTPUT = (0x1U << I2S_LRCK_OUT_SHIFT),
} I2S_LrckMode;

#define I2S_BCLK_OUT_SHIFT                              (18)
#define I2S_BCLK_OUT_MASK                               (0x1U << I2S_BCLK_OUT_SHIFT)
typedef enum {
	I2S_BCLK_INPUT  = (0x0U << I2S_BCLK_OUT_SHIFT),
	I2S_BCLK_OUTPUT = (0x1U << I2S_BCLK_OUT_SHIFT),
} I2S_BclkMode;

/*
 * Bits definition for I2S_FMT0 Format Register0(0x0004)
 */
#define I2S_SW_SEC_SHIFT                                (0)
#define I2S_SW_SEC_MASK                                 (0x1U << I2S_SW_SEC_SHIFT)
typedef enum {
	I2S_SLOT_WIDTH_BIT8  = (0x1U << I2S_SW_SEC_SHIFT),
	I2S_SLOT_WIDTH_BIT12 = (0x2U << I2S_SW_SEC_SHIFT),
	I2S_SLOT_WIDTH_BIT16 = (0x3U << I2S_SW_SEC_SHIFT),
	I2S_SLOT_WIDTH_BIT20 = (0x4U << I2S_SW_SEC_SHIFT),
	I2S_SLOT_WIDTH_BIT24 = (0x5U << I2S_SW_SEC_SHIFT),
	I2S_SLOT_WIDTH_BIT28 = (0x6U << I2S_SW_SEC_SHIFT),
	I2S_SLOT_WIDTH_BIT32 = (0x7U << I2S_SW_SEC_SHIFT),
} I2S_SlotWidth;

#define I2S_EDGE_TRANSFER_SHIFT                         (3)
#define I2S_EDGE_TRANSFER_MASK                          (0x1U << I2S_EDGE_TRANSFER_SHIFT)
typedef enum {
	I2S_SEC_NEGATIVE_EDGE = (0x0U << I2S_EDGE_TRANSFER_SHIFT),
	I2S_SEC_POSITIVE_EDGE = (0x1U << I2S_EDGE_TRANSFER_SHIFT),
} I2S_EdgeTransfer;

#define I2S_SR_SHIFT                                   (4)
#define I2S_SR_MASK                                    (0x7U << I2S_SR_SHIFT)
/**
  * @brief sampling accuracy
  */
typedef enum {
	I2S_SR8BIT  = (0x1U << I2S_SR_SHIFT),
	I2S_SR12BIT = (0x2U << I2S_SR_SHIFT),
	I2S_SR16BIT = (0x3U << I2S_SR_SHIFT),
	I2S_SR20BIT = (0x4U << I2S_SR_SHIFT),
	I2S_SR24BIT = (0x5U << I2S_SR_SHIFT),
	I2S_SR28BIT = (0x6U << I2S_SR_SHIFT),
	I2S_SR32BIT = (0x7U << I2S_SR_SHIFT),
} I2S_SampleResolution;

#define I2S_BCLK_POLARITY_SHIFT                         (7)
#define I2S_BCLK_POLARITY_MASK                          (0x1U << I2S_BCLK_POLARITY_SHIFT)
typedef enum {
	I2S_SEC_NOR_MODE    = (0x0U << I2S_BCLK_POLARITY_SHIFT),
	I2S_SEC_INVERT_MODE = (0x1U << I2S_BCLK_POLARITY_SHIFT),
} I2S_BclkPolarity;

#define I2S_LRCK_PERIOD_SHIFT                            (8)
#define I2S_LRCK_PERIOD_MASK                             (0x3FFU << I2S_LRCK_PERIOD_SHIFT)
#define I2S_LRCK_PERIOD(n)                               ((n-1) << I2S_LRCK_PERIOD_SHIFT)

#define I2S_LRCK_POLARITY_SHIFT                          (19)
#define I2S_LRCK_POLARITY_MASK                           (0x1U << I2S_LRCK_POLARITY_SHIFT)
typedef enum {
	I2S_LEFTCHANNEL_LRCKLOW  = (0x0U << I2S_LRCK_POLARITY_SHIFT),
	I2S_LEFTCHANNEL_LRCKHIGH = (0x1U << I2S_BCLK_POLARITY_SHIFT),
} I2S_LrckPolarity;


#define I2S_LRCK_WIDTH_SHIFT                                 (30)
#define I2S_LRCK_WIDTH_MASK                                  (0x1U << I2S_LRCK_WIDTH_SHIFT)
/**
  * @brief frame mode
  */
typedef enum {
	I2S_SHORT_FRAME = (0x0U << I2S_LRCK_WIDTH_SHIFT),
	I2S_LONG_FRAME  = (0x1U << I2S_LRCK_WIDTH_SHIFT),
} I2S_FrameMode;

/*
 * Bits definition for I2S_FMT1 Format Register1  (0X0008)
 */
#define I2S_PCM_TXMODE_SHIFT                                 (0)
#define I2S_PCM_TXMODE_MASK                                  (0x3U << I2S_PCM_TXMODE_SHIFT)
typedef enum {
	I2S_TX_LINEAR_PCM    = (0x0U << I2S_PCM_TXMODE_SHIFT),
	I2S_TX_PCM_NOVAL     = (0x1U << I2S_PCM_TXMODE_SHIFT),
	I2S_TX_PCM_ULAW_BIT8 = (0x2U << I2S_PCM_TXMODE_SHIFT),
	I2S_TX_PCM_ALAW_BIT8 = (0x3U << I2S_PCM_TXMODE_SHIFT),
} I2S_TXPDM;

#define I2S_PCM_RXMODE_SHIFT                                 (2)
#define I2S_PCM_RXMODE_MASK                                  (0x3U << I2S_PCM_RXMODE_SHIFT)
typedef enum {
	I2S_RX_LINEAR_PCM    = (0U << I2S_PCM_RXMODE_SHIFT),
	I2S_RX_PCM_NOVAL     = (1U << I2S_PCM_RXMODE_SHIFT),
	I2S_RX_PCM_ULAW_BIT8 = (2U << I2S_PCM_RXMODE_SHIFT),
	I2S_RX_PCM_ALAW_BIT8 = (3U << I2S_PCM_RXMODE_SHIFT),
} I2S_RXPDM;

#define I2S_SEXT_SHIFT                                      (4)
#define I2S_SEXT_MASK                                        (0x3U << I2S_SEXT_SHIFT)
typedef enum {
	I2S_PADDING_LSB = (0x0U << I2S_SEXT_SHIFT),
	I2S_SIGN_MSB    = (0x1U << I2S_SEXT_SHIFT),
	I2S_NO_SEXT_VAL = (0x2U << I2S_SEXT_SHIFT),
	I2S_ZERO_SLOT   = (0x3U << I2S_SEXT_SHIFT),
} I2S_SEXT;

#define I2S_TX_MLS_SHIFT                                    (6)
#define I2S_TX_MLS_MASK                                     (0x1U << I2S_TX_MLS_SHIFT)
typedef enum {
	I2S_TX_MSB_FIRST = (0x0U << I2S_TX_MLS_SHIFT),
	I2S_TX_LSB_FIRST = (0x1U << I2S_TX_MLS_SHIFT),
} I2S_TxMLS;

#define I2S_RX_MLS_SHIFT                                    (7)
#define I2S_RX_MLS_MASK                                     (0x1U << I2S_RX_MLS_SHIFT)
typedef enum {
	I2S_RX_MSB_FIRST = (0x0U << I2S_RX_MLS_SHIFT),
	I2S_RX_LSB_FIRST = (0x1U << I2S_RX_MLS_SHIFT),
} I2S_RxMLS;

/*
 * Bits definition for I2S_ISTA Interrupt Status Register
 */
#define I2S_RX_FIFO_AVABLE_IT_BIT                           HAL_BIT(0)
#define I2S_RX_FIFO_OVERRUN_IT_BIT                          HAL_BIT(1)
#define I2S_RX_FIFO_UNDERRUN_IT_BIT                         HAL_BIT(2)
#define I2S_TX_FIFO_EMPTY_IT_BIT                            HAL_BIT(4)
#define I2S_TX_FIFO_OVERRUN_IT_BIT                          HAL_BIT(5)
#define I2S_TX_FIFO_UNDERRUN_IT_BIT                         HAL_BIT(6)

/*
 * Bits definition for I2S_FCTL FIFO Control Register (0x0014)
 */
#define I2S_RX_FIFO_MODE_SHIFT                              (0)
#define I2S_RX_FIFO_MODE_MASK                               (0x3U << I2S_RX_FIFO_MODE_SHIFT)
typedef enum {
	I2S_RX_FIFO_MODE0 = (0x0U << I2S_RX_FIFO_MODE_SHIFT),
	I2S_RX_FIFO_MODE1 = (0x1U << I2S_RX_FIFO_MODE_SHIFT),
	I2S_RX_FIFO_MODE2 = (0x2U << I2S_RX_FIFO_MODE_SHIFT),
	I2S_RX_FIFO_MODE3 = (0x3U << I2S_RX_FIFO_MODE_SHIFT),
} I2S_RXOM;

#define I2S_TX_FIFO_MODE_SHIFT 	                             (2)
#define I2S_TX_FIFO_MODE_MASK                                (0x1U << I2S_TX_FIFO_MODE_SHIFT)
typedef enum {
	I2S_TX_FIFO_MODE0 = (0x0U << I2S_TX_FIFO_MODE_SHIFT),
	I2S_TX_FIFO_MODE1 = (0x1U << I2S_TX_FIFO_MODE_SHIFT),
} I2S_TXIM;

#define I2S_RXFIFO_LEVEL_SHIFT                               (4)
#define I2S_RXFIFO_LEVEL_MASK                                (0x3FU << I2S_RXFIFO_LEVEL_SHIFT)
#define I2S_RXFIFO_TRIGGER_LEVEL(n)                          ((n+1) << I2S_RXFIFO_LEVEL_SHIFT)

#define I2S_TXFIFO_LEVEL_SHIFT                               (12)
#define I2S_TXFIFO_LEVEL_MASK                                (0x7FU << I2S_TXFIFO_LEVEL_SHIFT)
#define I2S_TXFIFO_TRIGGER_LEVEL(n)                          ((n+1) << I2S_TXFIFO_LEVEL_SHIFT)

#define I2S_RXFIFO_RESET_BIT                                  HAL_BIT(24)
#define I2S_TXFIFO_RESET_BIT                                  HAL_BIT(25)
#define I2S_HUB_EN_BIT                                        HAL_BIT(31)

/*
 * Bits definition for I2S_FSTA FIFO Status Register
 */
#define I2S_RXFIFO_CNT_SHIFT                                  (0)
#define I2S_RXFIFO_CNT_MASK                                   (0x7FU << I2S_RXFIFO_CNT_SHIFT)

#define I2S_RXFIFO_AVAILABLE_BIT                              HAL_BIT(8)

#define I2S_TXFIFO_EMPTY_SHIFT                                (16)
#define I2S_TXFIFO_EMPTY_MASK                                 (0xFFU << I2S_TXFIFO_EMPTY_SHIFT)

#define I2S_TXFIFO_EMPTY_BIT                                  HAL_BIT(28)

/*
 * Bits definition for I2S_INT DMA&Interrupt Control Register (0x001c)
 */
#define I2S_RXFIFO_AVABLE_ITEN_BIT                            HAL_BIT(0)
#define I2S_RXFIFO_OVERRUN_ITEN_BIT                           HAL_BIT(1)
#define I2S_RXFIFO_UNDERRUN_ITEN_BIT                          HAL_BIT(2)
#define I2S_RXFIFO_DMA_ITEN_BIT                               HAL_BIT(3)
#define I2S_TXFIFO_EMPTY_ITEN_BIT                             HAL_BIT(4)
#define I2S_TXFIFO_OVERRUN_ITEN_BIT                           HAL_BIT(5)
#define I2S_TXFIFO_UNDERRUN_ITEN_BIT                          HAL_BIT(6)
#define I2S_TXFIFO_DMA_ITEN_BIT                               HAL_BIT(7)

/*
 * Bits definition for I2S_CLKD Clock Divide Register  (0x24)
 */
#define I2S_MCLKDIV_SHIFT                                     (0)
#define I2S_MCLKDIV_MASK                                      (0xFU << I2S_MCLKDIV_SHIFT)    /* MCLK divide ratio [3:0] */
typedef enum {
	I2S_MCLKDIV_1   = (0x1U << I2S_MCLKDIV_SHIFT),    /* MCLK divide by 1   */
	I2S_MCLKDIV_2   = (0x2U << I2S_MCLKDIV_SHIFT),    /* MCLK divide by 2   */
	I2S_MCLKDIV_4   = (0x3U << I2S_MCLKDIV_SHIFT),    /* MCLK divide by 4   */
	I2S_MCLKDIV_6   = (0x4U << I2S_MCLKDIV_SHIFT),    /* MCLK divide by 6   */
	I2S_MCLKDIV_8   = (0x5U << I2S_MCLKDIV_SHIFT),    /* MCLK divide by 8   */
	I2S_MCLKDIV_12  = (0x6U << I2S_MCLKDIV_SHIFT),    /* MCLK divide by 12  */
	I2S_MCLKDIV_16  = (0x7U << I2S_MCLKDIV_SHIFT),    /* MCLK divide by 16  */
	I2S_MCLKDIV_24  = (0x8U << I2S_MCLKDIV_SHIFT),    /* MCLK divide by 24  */
	I2S_MCLKDIV_32  = (0x9U << I2S_MCLKDIV_SHIFT),    /* MCLK divide by 32  */
	I2S_MCLKDIV_48  = (0xAU << I2S_MCLKDIV_SHIFT),    /* MCLK divide by 48  */
	I2S_MCLKDIV_64  = (0xBU << I2S_MCLKDIV_SHIFT),    /* MCLK divide by 64  */
	I2S_MCLKDIV_96  = (0xCU << I2S_MCLKDIV_SHIFT),    /* MCLK divide by 96  */
	I2S_MCLKDIV_128 = (0xDU << I2S_MCLKDIV_SHIFT),    /* MCLK divide by 128 */
	I2S_MCLKDIV_176 = (0xEU << I2S_MCLKDIV_SHIFT),    /* MCLK divide by 176 */
	I2S_MCLKDIV_192 = (0xFU << I2S_MCLKDIV_SHIFT),    /* MCLK divide by 192 */
} I2S_MCLKDIV;

#define I2S_BCLKDIV_SHIFT                                     (4)
#define I2S_BCLKDIV_MASK                                      (0xFU << I2S_BCLKDIV_SHIFT)    /* BCLK divide ratio [3:0] */
typedef enum {
	I2S_BCLKDIV_1   = (0x1U << I2S_BCLKDIV_SHIFT),    /* BCLK divide by 1   */
	I2S_BCLKDIV_2   = (0x2U << I2S_BCLKDIV_SHIFT),    /* BCLK divide by 2   */
	I2S_BCLKDIV_4   = (0x3U << I2S_BCLKDIV_SHIFT),    /* BCLK divide by 4   */
	I2S_BCLKDIV_6   = (0x4U << I2S_BCLKDIV_SHIFT),    /* BCLK divide by 6   */
	I2S_BCLKDIV_8   = (0x5U << I2S_BCLKDIV_SHIFT),    /* BCLK divide by 8   */
	I2S_BCLKDIV_12  = (0x6U << I2S_BCLKDIV_SHIFT),    /* BCLK divide by 12  */
	I2S_BCLKDIV_16  = (0x7U << I2S_BCLKDIV_SHIFT),    /* BCLK divide by 16  */
	I2S_BCLKDIV_24  = (0x8U << I2S_BCLKDIV_SHIFT),    /* BCLK divide by 24  */
	I2S_BCLKDIV_32  = (0x9U << I2S_BCLKDIV_SHIFT),    /* BCLK divide by 32  */
	I2S_BCLKDIV_48  = (0xAU << I2S_BCLKDIV_SHIFT),    /* BCLK divide by 48  */
	I2S_BCLKDIV_64  = (0xBU << I2S_BCLKDIV_SHIFT),    /* BCLK divide by 64  */
	I2S_BCLKDIV_96  = (0xCU << I2S_BCLKDIV_SHIFT),    /* BCLK divide by 96  */
	I2S_BCLKDIV_128 = (0xDU << I2S_BCLKDIV_SHIFT),    /* BCLK divide by 128 */
	I2S_BCLKDIV_176 = (0xEU << I2S_BCLKDIV_SHIFT),    /* BCLK divide by 176 */
	I2S_BCLKDIV_192 = (0xFU << I2S_BCLKDIV_SHIFT),    /* BCLK divide by 192 */
} I2S_BCLKDIV;

#define I2S_MCLK_OUT_EN_BIT                                   HAL_BIT(8)

/*
 * Bits definition for I2S_CHCFG Channel Configuration Register (0x30)
 */
#define I2S_TX_SLOT_NUM_SHIFT                                 (0)
#define I2S_TX_SLOT_NUM_MASK                                  (0x7U << I2S_TX_SLOT_NUM_SHIFT)
typedef enum {
	I2S_TX_SLOT_NUM1 = (0x0U << I2S_TX_SLOT_NUM_SHIFT),
	I2S_TX_SLOT_NUM2 = (0x1U << I2S_TX_SLOT_NUM_SHIFT),
	I2S_TX_SLOT_NUM3 = (0x2U << I2S_TX_SLOT_NUM_SHIFT),
	I2S_TX_SLOT_NUM4 = (0x3U << I2S_TX_SLOT_NUM_SHIFT),
	I2S_TX_SLOT_NUM5 = (0x4U << I2S_TX_SLOT_NUM_SHIFT),
	I2S_TX_SLOT_NUM6 = (0x5U << I2S_TX_SLOT_NUM_SHIFT),
	I2S_TX_SLOT_NUM7 = (0x6U << I2S_TX_SLOT_NUM_SHIFT),
	I2S_TX_SLOT_NUM8 = (0x7U << I2S_TX_SLOT_NUM_SHIFT),
} I2S_TxSlotNum;

#define I2S_RX_SLOT_NUM_SHIFT	                              (4)
#define I2S_RX_SLOT_NUM_MASK                                  (0x7U << I2S_RX_SLOT_NUM_SHIFT)
typedef enum {
	I2S_RX_SLOT_NUM1 = (0x0U << I2S_RX_SLOT_NUM_SHIFT),
	I2S_RX_SLOT_NUM2 = (0x1U << I2S_RX_SLOT_NUM_SHIFT),
	I2S_RX_SLOT_NUM3 = (0x2U << I2S_RX_SLOT_NUM_SHIFT),
	I2S_RX_SLOT_NUM4 = (0x3U << I2S_RX_SLOT_NUM_SHIFT),
	I2S_RX_SLOT_NUM5 = (0x4U << I2S_RX_SLOT_NUM_SHIFT),
	I2S_RX_SLOT_NUM6 = (0x5U << I2S_RX_SLOT_NUM_SHIFT),
	I2S_RX_SLOT_NUM7 = (0x6U << I2S_RX_SLOT_NUM_SHIFT),
	I2S_RX_SLOT_NUM8 = (0x7U << I2S_RX_SLOT_NUM_SHIFT),
} I2S_RxSlotNum;

#define I2S_TXN_STATE_SHIFT                                  (8)
#define I2S_TXN_STATEMASK                                    (0x1U << I2S_TXN_STATE_SHIFT)
typedef enum {
	I2S_TRANSFER0 = (0x0U << I2S_TXN_STATE_SHIFT),
	I2S_HIZ_STATE = (0x1U << I2S_TXN_STATE_SHIFT),
} I2S_TxnState;

#define I2S_TX_SLOT_HIZ_SHIFT                                (9)
#define I2S_TX_SLOT_HIZ_MASK                                 (0x1U << I2S_TX_SLOT_HIZ_SHIFT)
typedef enum {
	I2S_NORMAL_MODE = (0x0U << I2S_RX_SLOT_NUM_SHIFT),
	I2S_HI_Z_STATE  = (0x1U << I2S_RX_SLOT_NUM_SHIFT),
} I2S_TxSlotHIZ;

/*
 * Bits definition for I2S_TXnCHSEL Channel Select Register  (0X34)
 */
#define I2S_TXN_CHANNEL_SEL_SHIFT                             (0)
#define I2S_TXN_CHANNEL_SEL_MASK                              (0x7U << I2S_TXN_CHANNEL_SEL_SHIFT)
typedef enum {
	I2S_TXN_CHANNEL_NUM1 = (0x0U << I2S_TXN_CHANNEL_SEL_SHIFT),
	I2S_TXN_CHANNEL_NUM2 = (0x1U << I2S_TXN_CHANNEL_SEL_SHIFT),
	I2S_TXN_CHANNEL_NUM3 = (0x2U << I2S_TXN_CHANNEL_SEL_SHIFT),
	I2S_TXN_CHANNEL_NUM4 = (0x3U << I2S_TXN_CHANNEL_SEL_SHIFT),
	I2S_TXN_CHANNEL_NUM5 = (0x4U << I2S_TXN_CHANNEL_SEL_SHIFT),
	I2S_TXN_CHANNEL_NUM6 = (0x5U << I2S_TXN_CHANNEL_SEL_SHIFT),
	I2S_TXN_CHANNEL_NUM7 = (0x6U << I2S_TXN_CHANNEL_SEL_SHIFT),
	I2S_TXN_CHANNEL_NUM8 = (0x7U << I2S_TXN_CHANNEL_SEL_SHIFT),
} I2S_TxnChNum;

#define I2S_TXN_CHANNEL_SLOT_ENABLE_SHIFT                         (4)
#define I2S_TXN_CHANNEL_SLOT_ENABLE_MASK                          (0xFFU << I2S_TXN_CHANNEL_SLOT_ENABLE_SHIFT)
#define I2S_TXN_CHANNEL_SLOTS_ENABLE(n)                           (((1 << n) -1) << I2S_TXN_CHANNEL_SLOT_ENABLE_SHIFT)
#define I2S_TXN_OFFSET_SHIFT                                      (12)
#define I2S_TXN_OFFSET_MASK                                       (0x1U << I2S_TXN_OFFSET_SHIFT)
typedef enum {
	I2S_TX_NO_OFFSET      = (0x0U << I2S_TXN_OFFSET_SHIFT),
	I2S_TX_ONEBCLK_OFFSET = (0x1U << I2S_TXN_OFFSET_SHIFT),
} I2S_TxnOffset;

/*
 * Bits definition for I2S_TXnCHMAP Channel Mapping Register (0x44)
 */
#define I2S_TXN_CHX_MAP_MASK(m)                                   (0x7U << (4*m))
#define I2S_TXN_CHX_MAP(m)                                        (m << 4*m)

/*
 * Bits definition for I2S_RXCHSEL Channel Mapping Register (0x54)
 */
#define I2S_RXN_CHANNEL_SEL_SHIFT                                  0
#define I2S_RXN_CHANNEL_SEL_MASK                                   (0x7U << I2S_RXN_CHANNEL_SEL_SHIFT)

typedef enum {
	I2S_RXN_CHANNEL_NUM1 = (0X0U << I2S_RXN_CHANNEL_SEL_SHIFT),
	I2S_RXN_CHANNEL_NUM2 = (0X1U << I2S_RXN_CHANNEL_SEL_SHIFT),
	I2S_RXN_CHANNEL_NUM3 = (0X2U << I2S_RXN_CHANNEL_SEL_SHIFT),
	I2S_RXN_CHANNEL_NUM4 = (0X3U << I2S_RXN_CHANNEL_SEL_SHIFT),
	I2S_RXN_CHANNEL_NUM5 = (0X4U << I2S_RXN_CHANNEL_SEL_SHIFT),
	I2S_RXN_CHANNEL_NUM6 = (0X5U << I2S_RXN_CHANNEL_SEL_SHIFT),
	I2S_RXN_CHANNEL_NUM7 = (0X6U << I2S_RXN_CHANNEL_SEL_SHIFT),
	I2S_RXN_CHANNEL_NUM8 = (0X7U << I2S_RXN_CHANNEL_SEL_SHIFT),
} I2S_RxnChNum;

#define I2S_RXN_OFFSET_SHIFT                                      (12)
#define I2S_RXN_OFFSET_MASK                                       (0x1U << I2S_RXN_OFFSET_SHIFT)
typedef enum {
	I2S_RX_NO_OFFSET      = (0x0U << I2S_RXN_OFFSET_SHIFT),
	I2S_RX_ONEBCLK_OFFSET = (0x1U << I2S_RXN_OFFSET_SHIFT),
} I2S_RxnOffset;

/*
 * Bits definition for I2S_RXCHMAP Channel Mapping Register (0x58)
 */
#define I2S_RXN_CHX_MAP_MASK(m)                                   (0x7U << (4*m))
#define I2S_RXN_CHX_MAP(m)                                        (m << (4*m))

/**
  * @brief I2S sample rate definition
  */
typedef enum {
	I2S_SR8K        = 0,  /* 8000Hz  */
	I2S_SR12K       = 1,  /* 12000Hz */
	I2S_SR16K       = 2,  /* 16000Hz */
	I2S_SR24K       = 3,  /* 24000Hz */
	I2S_SR32K       = 4,  /* 32000Hz */
	I2S_SR48K       = 5,  /* 48000Hz */
	I2S_SR11K       = 6,  /* 11025Hz */
	I2S_SR22K       = 7,  /* 22050Hz */
	I2S_SR44K       = 8,  /* 44100Hz */
} I2S_SampleRate;

/**
  * @brief stream direction
  */
typedef enum {
	PLAYBACK,
	RECORD,
} I2S_StreamDir;

/**
  * @brief i2s data init structure definition
  */
typedef struct {
	I2S_SampleRate        sampleRate;   /*!< Specifies the sampling rate of the transmitted data.    */
	I2S_StreamDir         direction;    /*!< Specifies the direction of the transmitted data.    */
	I2S_SampleResolution  resolution;   /*!< Specifies the sampling accuracy of the transmitted data.    */
	uint32_t              channels;     /*!< Specifies the number of channels to transmit data.    */
	uint32_t              bufSize;      /*!< Specifies the buffer size of the transmitted data.    */
} I2S_DataParam;

/**
  * @brief external device clock structure definition
  */
typedef struct {
	uint32_t   clkSource; /*!< S`	pecifies the clock frequency provided.    */
	bool       isDevclk;  /*!< Specifies whether i2s provide an external device clock.    */
} DEV_Param;

/**
  * @brief I2S low level hardware init structure definition
  */
typedef struct {
	bool          i2sFormat;      /*!< Specifies the I2S operating format.    */
	PCM_ClkMode   clkMode;        /*!< Specifies the I2S clk mode.    */
	PCM_TranFmt   transferFormat; /*!< Specifies the I2S transfer format.    */
	PCM_SignalInv signalInterval; /*!< Specifies the idle state of the I2S clock.    */
	uint32_t      lrckPeriod;     /*!< Specifies The lrck division factor.    */
	I2S_FrameMode frameMode;      /*!< Specifies the frame format.    */
	I2S_TxMLS     txMsbFirst;     /*!< Specifies high first pass or low first pass when transmission data.    */
	I2S_RxMLS     rxMsbFirst;     /*!< Specifies high first pass or low first pass when receive data.    */
	uint32_t      txFifoLevel;    /*!< Specifies the depth of tx Fifo.     */
	uint32_t      rxFifoLevel;    /*!< Specifies the depth of rx Fifo.    */
	DEV_Param     codecClk;       /*!< I2S MCLK parameters for external device     */
	uint8_t		  codecClkDiv;	  /*!< I2S MCLK division      */
} I2S_HWParam;

/**
  * @brief I2S module init structure definition
  */
typedef struct {
	uint8_t 	mclkDiv;		/* I2S mclk division */
	I2S_HWParam *hwParam; 		/*!< I2S Hardware init structure.    */
} I2S_Param;

HAL_Status HAL_I2S_Init(I2S_Param *param);
void HAL_I2S_DeInit();
HAL_Status HAL_I2S_Open(I2S_DataParam *param);
HAL_Status HAL_I2S_Close(uint32_t dir);
int32_t HAL_I2S_Read_DMA(uint8_t *buf, uint32_t size);
int32_t HAL_I2S_Write_DMA(uint8_t *buf, uint32_t size);
void HAL_I2S_REG_DEBUG();

#ifdef __cplusplus
}
#endif

#endif /* _DRIVER_CHIP_HAL_I2S_H_ */
