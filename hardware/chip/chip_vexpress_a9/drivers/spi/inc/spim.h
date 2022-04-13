/* Copyright Statement:
 *
 * 2018 MediaTek Inc. All rights reserved.
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

#ifndef _SPIM_H_
#define _SPIM_H_

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <FreeRTOS.h>
#include <semphr.h>
#include <mt_reg_base.h>
#include <mt_gpio.h>
#include <mt_gic_v3.h>
#include <clk.h>
#include <mt8512_clk.h>

typedef unsigned long int dma_addr_t;

#define MUTEX_LOCK(mutex)        xSemaphoreTake(mutex, portMAX_DELAY)
#define MUTEX_UNLOCK(mutex)      xSemaphoreGive(mutex)

/* debug flag */
#define MTK_SPI_DEBUG 0

#define MTK_SPIM_MAX_PORT_NUMBER	1

#define BIT(nr)             (1 << (nr))
#define readl(addr)  (*(volatile unsigned int *)(addr))
#define writel(data, addr) \
	((*(volatile unsigned int *)(addr)) = (unsigned int) data)

#define ARRAY_SIZE(x) (sizeof(x)/sizeof((x)[0]))

#if MTK_SPI_DEBUG
#define SPI_INFO(string, args...)  \
	printf("[spim] "string, ##args)
#else
#define SPI_INFO(string, args...)  do {} while (0)
#endif
#define SPI_LOG(string, args...)  printf("[spim] "string, ##args)

/* EPCOT SCP SPI-M IRQ number */
enum {
	SPIM_IRQ = 73,
};

/* clk config define */
#define MTK_SPIM_CLK_26M 26000
#define MTK_SPIM_UNIVPLL2_D2 208000
#define MTK_SPIM_SYSPLL2_D2 182000
#define MTK_SPIM_UNIVPLL1_D4 156000
#define MTK_SPIM_SYSPLL1_D4 136500
#define MTK_SPIM_UNIVPLL3_D2 124800
#define MTK_SPIM_UNIVPLL2_D4 104000
#define MTK_SPIM_SYSPLL4_D2 78000

#define CLK_CFG_UPDATE_REG	0x10000004
#define CLK_CFG0_REG		0x10000040
#define CLK_CFG0_SET_REG	0x10000044
#define CLK_CFG0_CLR_REG	0x10000048

/* SPI_CFG_UPDATE_REG */
enum {
	SPI_CFG_UPDATE_SPI_CK_UPDATE_SHIFT = 3,
	SPI_CFG_UPDATE_SPI_CK_UPDATE_MASK = 0x1 << SPI_CFG_UPDATE_SPI_CK_UPDATE_SHIFT,
};

/* SPI_CFG0_REG */
enum {
	SPI_CFG0_CLK_SPI_SEL_SHIFT = 24,
	SPI_CFG0_CLK_SPI_SEL_MASK = 0x7 << SPI_CFG0_CLK_SPI_SEL_SHIFT,
};

/* SPI_CFG0_SET_REG */
enum {
	SPI_CFG0_SET_CLK_SPI_SEL_SHIFT = 24,
	SPI_CFG0_SET_CLK_SPI_SEL_MASK = 0x7 << SPI_CFG0_SET_CLK_SPI_SEL_SHIFT,
};

/* SPI_CFG0_CLR_REG */
enum {
	SPI_CFG0_CLR_CLK_SPI_SEL_SHIFT = 24,
	SPI_CFG0_CLR_CLK_SPI_SEL_MASK = 0x7 << SPI_CFG0_CLR_CLK_SPI_SEL_SHIFT,
};

/* spi master register offset */
#define SPI_CFG0_REG	0x0000
#define SPI_CFG1_REG	0x0004
#define SPI_TX_SRC_REG	0x0008
#define SPI_RX_DST_REG	0x000C
#define SPI_TX_DATA_REG	0x0010
#define SPI_RX_DATA_REG	0x0014
#define SPI_CMD_REG		0x0018
#define SPI_STATUS0_REG	0x001C
#define SPI_STATUS1_REG	0x0020
#define SPI_CFG2_REG	0x0028

/* SPI_CFG0_REG */
enum {
	SPI_CFG0_CS_HOLD_SHIFT = 0,
	SPI_CFG0_CS_SETUP_SHIFT = 16,
};

/* SPI_CFG1_REG */
enum {
	SPI_CFG1_CS_IDLE_SHIFT = 0,
	SPI_CFG1_PACKET_LOOP_SHIFT = 8,
	SPI_CFG1_PACKET_LENGTH_SHIFT = 16,
	SPI_CFG1_GET_TICK_DLY_SHIFT = 29,

	SPI_CFG1_CS_IDLE_MASK = 0xff << SPI_CFG1_CS_IDLE_SHIFT,
	SPI_CFG1_PACKET_LOOP_MASK = 0xff << SPI_CFG1_PACKET_LOOP_SHIFT,
	SPI_CFG1_PACKET_LENGTH_MASK = 0x3ff << SPI_CFG1_PACKET_LENGTH_SHIFT,
	SPI_CFG1_GET_TICK_DLY_MASK = 0x7 << SPI_CFG1_GET_TICK_DLY_SHIFT,

};

/* SPI_CFG2_REG */
enum {
	SPI_CFG2_SCK_HIGH_SHIFT = 0,
	SPI_CFG2_SCK_LOW_SHIFT = 16,
	SPI_CFG2_SCK_HIGH_MASK = 0xffff,
	SPI_CFG2_SCK_LOW_MASK = 0xffff0000,
};

/* SPI_CMD */
enum {
	SPI_CMD_ACT_SHIFT = 0,
	SPI_CMD_RESUME_SHIFT = 1,
	SPI_CMD_RST_SHIFT = 2,
	SPI_CMD_PAUSE_EN_SHIFT = 4,
	SPI_CMD_DEASSERT_SHIFT = 5,
	SPI_CMD_SAMPLE_SEL_SHIFT = 6,
	SPI_CMD_CS_POL_SHIFT = 7,
	SPI_CMD_CPHA_SHIFT = 8,
	SPI_CMD_CPOL_SHIFT = 9,
	SPI_CMD_RX_DMA_SHIFT = 10,
	SPI_CMD_TX_DMA_SHIFT = 11,
	SPI_CMD_TXMSBF_SHIFT = 12,
	SPI_CMD_RXMSBF_SHIFT = 13,
	SPI_CMD_RX_ENDIAN_SHIFT = 14,
	SPI_CMD_TX_ENDIAN_SHIFT = 15,
	SPI_CMD_FINISH_IE_SHIFT = 16,
	SPI_CMD_PAUSE_IE_SHIFT = 17,
	SPI_CMD_CS_PIN_SEL_SHIFT = 18,

	SPI_CMD_ACT_EN = 1 << SPI_CMD_ACT_SHIFT,
	SPI_CMD_RESUME_EN = 1 << SPI_CMD_RESUME_SHIFT,
	SPI_CMD_RST_EN = 1 << SPI_CMD_RST_SHIFT,
	SPI_CMD_PAUSE_EN = 1 << SPI_CMD_PAUSE_EN_SHIFT,
	SPI_CMD_DEASSERT_EN = 1 << SPI_CMD_DEASSERT_SHIFT,
	SPI_CMD_CS_POL_EN = 1 << SPI_CMD_CS_POL_SHIFT,
	SPI_CMD_SAMPLE_SEL_EN = 1 << SPI_CMD_SAMPLE_SEL_SHIFT,
	SPI_CMD_CPHA_EN = 1 << SPI_CMD_CPHA_SHIFT,
	SPI_CMD_CPOL_EN = 1 << SPI_CMD_CPOL_SHIFT,
	SPI_CMD_RX_DMA_EN = 1 << SPI_CMD_RX_DMA_SHIFT,
	SPI_CMD_TX_DMA_EN = 1 << SPI_CMD_TX_DMA_SHIFT,
	SPI_CMD_TXMSBF_EN = 1 << SPI_CMD_TXMSBF_SHIFT,
	SPI_CMD_RXMSBF_EN = 1 << SPI_CMD_RXMSBF_SHIFT,
	SPI_CMD_RX_ENDIAN_EN = 1 << SPI_CMD_RX_ENDIAN_SHIFT,
	SPI_CMD_TX_ENDIAN_EN = 1 << SPI_CMD_TX_ENDIAN_SHIFT,
	SPI_CMD_FINISH_IE_EN = 1 << SPI_CMD_FINISH_IE_SHIFT,
	SPI_CMD_PAUSE_IE_EN = 1 << SPI_CMD_PAUSE_IE_SHIFT,
	SPI_CMD_CS_PIN_SEL_EN = 1 << SPI_CMD_CS_PIN_SEL_SHIFT,
};

/** @ingroup IP_group_spi_internal_enum
* @brief spim-m sample_sel setting \n
* value is from 0 to 1.
*/
enum spi_sample_sel {
	POSEDGE = 0,
	NEGEDGE = 1
};

/** @ingroup IP_group_spi_internal_enum
* @brief spim-m CS pin polarity setting \n
* value is from 0 to 1.
*/
enum spi_cs_pol {
	ACTIVE_LOW = 0,
	ACTIVE_HIGH = 1
};

/** @ingroup IP_group_spi_internal_enum
* @brief spim-m SCK polarity setting \n
* value is from 0 to 1.
*/
enum spi_cpol {
	SPI_CPOL_0 = 0,
	SPI_CPOL_1 = 1
};

/** @ingroup IP_group_spi_internal_enum
* @brief spim-m SPI Clock Format setting \n
* value is from 0 to 1.
*/
enum spi_cpha {
	SPI_CPHA_0 = 0,
	SPI_CPHA_1 = 1
};

/** @ingroup IP_group_spi_internal_enum
* @brief spim-m data MSB/LSB setting \n
* value is from 0 to 1.
*/
enum spi_mlsb {
	SPI_LSB = 0,
	SPI_MSB = 1
};

/** @ingroup IP_group_spi_internal_enum
* @brief spim-m endian order setting \n
* value is from 0 to 1.
*/
enum spi_endian {
	SPI_LENDIAN = 0,
	SPI_BENDIAN = 1
};

/** @ingroup IP_group_spi_internal_enum
* @brief spim-m sample_sel setting \n
* value is from 0 to 1.
*/
enum spi_pause_mode {
	PAUSE_MODE_DISABLE = 0,
	PAUSE_MODE_ENABLE = 1
};

/** @ingroup IP_group_spi_internal_enum
* @brief spim-m deassert mode index \n
* value is from 0 to 1.
*/
enum spi_deassert_mode {
	DEASSERT_DISABLE = 0,
	DEASSERT_ENABLE = 1
};

/** @ingroup IP_group_spi_internal_enum
* @brief spim-m get_tick timing setting \n
* value is from 0 to 7.
*/
enum spi_tckdly {
	TICK_DLY0 = 0,
	TICK_DLY1 = 1,
	TICK_DLY2 = 2,
	TICK_DLY3 = 3,
	TICK_DLY4 = 4,
	TICK_DLY5 = 5,
	TICK_DLY6 = 6,
	TICK_DLY7 = 7,
};

/** @ingroup IP_group_spi_internal_enum
* @brief spim-m cs pin select setting \n
* value is from 0 to 1.
*/
enum spi_cs_pin_sel {
	SPI_CS_N = 0,
	SPI_CS_GPIO = 1
};

/** @ingroup IP_group_spi_internal_enum
* @brief spim-m current state recored\n
* value is from 0 to 1.
*/
enum {
	MTK_SPI_IDLE = 0,
	MTK_SPI_PAUSE_IDLE = 1
};

/** @ingroup IP_group_spi_internal_enum
* @brief spim-m irq state \n
* value is from 0 or 3.
*/
enum {
	MTK_SPI_BUSY_STATUS = 1,
	MTK_SPI_PAUSE_FINISH_INT_STATUS = 3
};

/** @ingroup IP_group_spi_internal_struct
 * @brief External Used Structure.
 */
struct mtk_spim_bus {
	unsigned long reg_base;

	int irq_num;
	int state;
	int pause_enable; /*1: pause mode enable*/
	int cs_pin_sel; /* 0:use CS_N; 1: use gpio as CS */
	/* spi transfer complete semaphore pointer */
	QueueHandle_t semaph;
};

/** @ingroup IP_group_spi_external_struct
 * @brief External Used Structure.
 */
struct mtk_chip_config {
	int setup_time;
	int hold_time;
	int high_time;
	int low_time;
	int cs_idletime;

	enum spi_sample_sel sample_sel;
	enum spi_cs_pol cs_pol;
	enum spi_cpol cpol;
	enum spi_cpha cpha;
	enum spi_mlsb tx_mlsb;
	enum spi_mlsb rx_mlsb;
	enum spi_endian tx_endian;
	enum spi_endian rx_endian;
	enum spi_pause_mode pause;
	enum spi_deassert_mode de_assert;
	enum spi_tckdly tck_dly;
	enum spi_cs_pin_sel cs_pin_sel;
};

/** @ingroup IP_group_spi_external_struct
 * @brief External Used Structure.
 */
struct spi_transfer {
	char	*tx_buf;
	char	*rx_buf;
	unsigned	len;

	dma_addr_t	tx_dma;
	dma_addr_t	rx_dma;
};

extern int mtk_spim_init(int bus_num);
extern int mtk_spim_exit(int bus_num);
extern int mtk_spim_config(int bus_num, struct mtk_chip_config *chip_config);
extern int mtk_spim_transfer_one(int bus_num, struct spi_transfer *xfer);
extern int mtk_spim_enable_clock(uint32_t source_clock_hz);
#endif
