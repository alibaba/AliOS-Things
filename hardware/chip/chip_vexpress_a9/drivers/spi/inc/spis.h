/* Copyright Statement:
 *
 * @2015 MediaTek Inc. All rights reserved.
 *
 * This software/firmware and related documentation ("MediaTek Software")
 * are protected under relevant copyright laws.
 * The information contained herein is confidential and proprietary
 * to MediaTek Inc. and/or its licensors.
 * Without the prior written permission of MediaTek Inc.
 * and/or its licensors, any reproduction, modification, use or disclosure
 * of MediaTek Software, and information contained herein,
 * in whole or in part, shall be strictly prohibited.
 *
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES
 * AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS
 * ("MEDIATEK SOFTWARE") RECEIVED FROM MEDIATEK AND/OR ITS
 * REPRESENTATIVES ARE PROVIDED TO RECEIVER ONAN "AS-IS" BASIS ONLY.
 * MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
 * OR NONINFRINGEMENT. NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY
 * WHATSOEVER WITH RESPECT TO THE SOFTWARE OF ANY THIRD PARTY WHICH
 * MAY BE USED BY, INCORPORATED IN, OR SUPPLIED WITH THE MEDIATEK
 * SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH THIRD PARTY FOR
 * ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY
 * THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE
 * RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO
 * CONFORM TO A PARTICULAR STANDARD OR OPEN FORUM.
 * RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S
 * ENTIRE AND CUMULATIVE LIABILITY WITH
 * RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 *
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE
 * AT ISSUE, OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE
 * PAID BY RECEIVER TO MEDIATEK FOR SUCH MEDIATEK SOFTWARE.
 */

#ifndef _SPIS_H_
#define _SPIS_H_

#include <FreeRTOS.h>
#include <stdint.h>
#include <stdio.h>
#include <semphr.h>
#include "mt_gpio.h"
#include <mt_reg_base.h>
#include <mt_gic_v3.h>
#include <clk.h>
#include <mt8512_clk.h>

/* debug log  switch */
#define MTK_SPIS_DEBUG 0
/* use default tx fifo value swich */
#define USE_DEFAULT_TX_FIFO_VALUE 0

#define MTK_SPIS_MAX_PORT_NUMBER 1

#define readl(addr)  (*(volatile unsigned int *)(addr))
#define writel(data, addr)   \
	((*(volatile unsigned int *)(addr)) = (unsigned int) data)

#define ARRAY_SIZE(x) (sizeof(x)/sizeof((x)[0]))

#if MTK_SPIS_DEBUG
#define SPIS_INFO(string, args...) \
	printf("[spis] "string, ##args)
#else
#define SPIS_INFO(string, args...)  do {} while (0)
#endif
#define SPIS_LOG(string, args...)  printf("[spis] "string, ##args)

#define dma_addr_t unsigned int

#define MTK_SPIS_CLK_26M 26000
#define MTK_SPIS_UNIVPLL_D3 416000
#define MTK_SPIS_SYSPLL_D3 364000
#define MTK_SPIS_UNIVPLL1_D2 312000
#define MTK_SPIS_UNIVPLL2_D2 208000
#define MTK_SPIS_UNIVPLL1_D4 156000
#define MTK_SPIS_UNIVPLL2_D4 104000
#define MTK_SPIS_SYSPLL4_D2 78000

#define CLK_CFG_UPDATE_REG	0x10000004
#define CLK_CFG1_REG		0x10000050
#define CLK_CFG1_SET_REG	0x10000054
#define CLK_CFG1_CLR_REG	0x10000058

/* SPI_CFG_UPDATE_REG */
enum {
	SPI_CFG_UPDATE_SPIS_CK_UPDATE_SHIFT = 4,
	SPI_CFG_UPDATE_SPIS_CK_UPDATE_MASK = 0x1 << SPI_CFG_UPDATE_SPIS_CK_UPDATE_SHIFT,
};

/* SPI_CFG1_REG */
enum {
	SPI_CFG1_CLK_SPIS_SEL_SHIFT = 0,
	SPI_CFG1_CLK_SPIS_SEL_MASK = 0x7 << SPI_CFG1_CLK_SPIS_SEL_SHIFT,
};

/* SPI_CFG1_SET_REG */
enum {
	SPI_CFG1_SET_CLK_SPIS_SEL_SHIFT = 0,
	SPI_CFG1_SET_CLK_SPIS_SEL_MASK = 0x7 << SPI_CFG1_SET_CLK_SPIS_SEL_SHIFT,
};

/* SPI_CFG1_CLR_REG */
enum {
	SPI_CFG1_CLR_CLK_SPIS_SEL_SHIFT = 0,
	SPI_CFG1_CLR_CLK_SPIS_SEL_MASK = 0x7 << SPI_CFG1_CLR_CLK_SPIS_SEL_SHIFT,
};

/* EPCOT SCP SPI-S IRQ number */
enum {
	SPIS_IRQ = 74,
};

/* register map define */
#define BIT(nr) (1UL << (nr))

#define SPIS_IRQ_EN_REG				0x0
#define DATA_DONE_EN				BIT(2)
#define RX_FIFO_FULL_EN				BIT(3)
#define RX_FIFO_EMPTY_EN			BIT(4)
#define TX_FIFO_FULL_EN				BIT(5)
#define TX_FIFO_EMPTY_EN			BIT(6)
#define DMA_DONE_EN					BIT(7)

#define SPIS_IRQ_CLR_REG			0x4
#define DATA_DONE_CLR				BIT(2)
#define RX_FIFO_FULL_CLR			BIT(3)
#define RX_FIFO_EMPTY_CLR			BIT(4)
#define TX_FIFO_FULL_CLR			BIT(5)
#define TX_FIFO_EMPTY_CLR			BIT(6)
#define DMA_DONE_CLR				BIT(7)

#define SPIS_IRQ_ST_REG				0x8
#define DATA_DONE_ST				BIT(2)
#define RX_FIFO_FULL_ST				BIT(3)
#define RX_FIFO_EMPTY_ST			BIT(4)
#define TX_FIFO_FULL_ST				BIT(5)
#define TX_FIFO_EMPTY_ST			BIT(6)
#define DMA_DONE_ST					BIT(7)

#define SPIS_IRQ_MASK_REG			0xc
#define DATA_DONE_MASK				BIT(2)
#define RX_FIFO_FULL_MASK			BIT(3)
#define RX_FIFO_EMPTY_MASK			BIT(4)
#define TX_FIFO_FULL_MASK			BIT(5)
#define TX_FIFO_EMPTY_MASK			BIT(6)
#define DMA_DONE_MASK				BIT(7)

#define SPIS_CFG_REG				0x10
#define SPIS_RX_EN					BIT(0)
#define SPIS_TX_EN					BIT(1)
#define SPIS_CPOL					BIT(2)
#define SPIS_CPHA					BIT(3)
#define SPIS_RXMSBF					BIT(4)
#define SPIS_TXMSBF					BIT(5)
#define SPIS_RX_ENDIAN				BIT(6)
#define SPIS_TX_ENDIAN				BIT(7)

#define SPIS_RX_DATA_REG			0x14
#define SPIS_TX_DATA_REG			0x18
#define SPIS_RX_DST_REG				0x1c
#define SPIS_TX_SRC_REG				0x20

#define SPIS_FIFO_ST_REG			0x28
#define TX_FIFO_EMPTY				BIT(7)
#define TX_FIFO_FULL				BIT(8)
#define RX_FIFO_EMPTY				BIT(23)
#define RX_FIFO_FULL				BIT(24)

#define SPIS_DMA_CFG_REG			0x30
#define TX_DMA_LEN_MASK				0xFFFFF
#define RX_DMA_EN					BIT(29)
#define TX_DMA_EN					BIT(30)
#define TX_DMA_TRIG_EN				BIT(31)

#define SPIS_FIFO_THR_REG			0x34
#define SPIS_DEBUG_ST_REG			0x38

#define SPIS_BYTE_CNT_REG			0x3c
#define SPIS_BYTE_CNT_CLR			BIT(31)

#define SPIS_SOFT_RST_REG			0x40
#define SPIS_SOFT_RST1				BIT(0)
#define SPIS_DMA_ADDR_LOAD			BIT(1)

#define SPIS_TX_FIFO_DEFAULT_REG	0x44
#define SPIS_RX_DST_HIGH_REG		0x4c
#define SPIS_TX_SRC_HIGH_REG		0x50
/*********************************/

enum spis_cpol {
	SPIS_CPOL_0 = 0,
	SPIS_CPOL_1 = 1
};

enum spis_cpha {
	SPIS_CPHA_0 = 0,
	SPIS_CPHA_1 = 1
};

enum spis_mlsb {
	SPIS_LSB = 0,
	SPIS_MSB = 1
};

enum spis_endian {
	SPIS_LENDIAN = 0,
	SPIS_BENDIAN = 1
};

/* Board specific platform_data */
struct mtk_spis_chip_config {
	int enable_tx;
	int enable_rx;
	enum spis_cpol cpol;
	enum spis_cpha cpha;
	enum spis_mlsb tx_mlsb;
	enum spis_mlsb rx_mlsb;
	enum spis_endian tx_endian;
	enum spis_endian rx_endian;
};

struct mtk_spis_bus {
	unsigned long reg_base;
	int use_dma_mode; /* 0: fifo mode; 1: dma mode */
	int irq_num;

	int enable_tx;
	int enable_rx;
	QueueHandle_t semaph;

};

struct spis_transfer {
	char	*tx_buf;
	char	*rx_buf;
	unsigned	len;

	dma_addr_t	tx_dma;
	dma_addr_t	rx_dma;
};


extern int mtk_spis_init(int bus_num);
extern int mtk_spis_exit(int bus_num);
extern int mtk_spis_config(int bus_num,
	struct mtk_spis_chip_config *chip_config);
extern int mtk_spis_wait_transfer_done(int bus_num,
	struct spis_transfer *xfer);
extern int mtk_spis_transfer_one(int bus_num, struct spis_transfer *xfer);
extern int mtk_spis_enable_clock();

#endif

