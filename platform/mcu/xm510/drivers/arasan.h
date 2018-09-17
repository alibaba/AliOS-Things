/*
 * Author: Giuseppe Cavallaro <peppe.cavallaro@xxxxxx>
 *
 * copyright (c) 2010  stmicroelectronics ltd
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef __ARASAN_H
#define __ARASAN_H

#include <stdio.h>
#include <k_api.h>
#include <aos/kernel.h>

//#define ARASAN_CLOCKRATE_MAX	25000000
#define ARASAN_CLOCKRATE_MAX	50000000
#define ARASAN_DRIVER_NAME	"arasan"
#define ARASAN_DMA_DESC_NUM	128

/*
 * Register offsets
 */
#define ARASAN_SDMA_SYS_ADDR		0x000
#define ARASAN_BLK_SIZE			0x004
#define ARASAN_BLK_COUNT		0x006
#define ARASAN_ARG			0x008
#define ARASAN_XFER_MODE		0x00c
#define ARASAN_CMD			0x00e
#define ARASAN_RSP(i)			(0x010 + ((i)<<2))
#define ARASAN_RSP0			0x010
#define ARASAN_RSP1			0x012
#define ARASAN_RSP2			0x014
#define ARASAN_RSP3			0x016
#define ARASAN_RSP4			0x018
#define ARASAN_RSP5			0x01a
#define ARASAN_RSP6			0x01c
#define ARASAN_RSP7			0x01e
#define ARASAN_BUFF			0x020
#define ARASAN_PRESENT_STATE		0x024
#define ARASAN_HOST_CTRL		0x028
#define ARASAN_PWR_CTRL			0x029
#define ARASAN_GAP_CTRL			0x02a
#define ARASAN_GAP_WAKEUP		0x02b
#define ARASAN_CLOCK_CTRL		0x02c
#define ARASAN_TIMEOUT_CTRL		0x02e
#define ARASAN_SW_RESET			0x02f

#define ARASAN_NORMAL_INT_STATUS	0x030
#define ARASAN_ERR_INT_STATUS		0x032
#define ARASAN_NORMAL_INT_STATUS_EN	0x034
#define ARASAN_ERR_INT_STATUS_EN	0x036
#define ARASAN_NORMAL_INT_SIGN_EN	0x038
#define ARASAN_ERR_INT_SIGN_EN		0x03a

#define ARASAN_CMD12_ERR_STATUS		0x03c

#define ARASAN_CAPABILITIES		0x040

#define ARASAN_ADMA_ERR_STATUS		0x054
#define ARASAN_ADMA_ADDRESS		0x058

#define ARASAN_SPI_INT_SUPPORT		0x0f0
#define ARASAN_HOST_VERSION		0x0fe

/* Error Interrupt Status Register */
#define ARASAN_CMD_TIMEOUT		(1 << 0)
#define ARASAN_CMD_CRC_ERROR		(1 << 1)
#define ARASAN_CMD_END_BIT_ERROR	(1 << 2)
#define ARASAN_CMD_INDEX_ERROR		(1 << 3)
#define ARASAN_DATA_TIMEOUT_ERROR	(1 << 4)
#define ARASAN_DATA_CRC_ERROR		(1 << 5)
#define ARASAN_DATA_END_ERROR		(1 << 6)
#define ARASAN_CURRENT_LIMIT_ERROR	(1 << 7)
#define ARASAN_AUTO_CMD12_ERROR		(1 << 8)
#define ARASAN_ADMA_ERROR		(1 << 9)
#define ARASAN_TARGET_RESP_ERROR	(1 << 12)
#define ARASAN_CEATA_ERROR		(1 << 13)

/* Error Interrupt Status ENABLE reg. (0- Masked, 1: Enabled) */
#define ARASAN_E_EN_CMD_TIMEOUT		(1 << 0)
#define ARASAN_E_EN_CMD_CRC_ERROR	(1 << 1)
#define ARASAN_E_EN_CMD_END_BIT_ERROR	(1 << 2)
#define ARASAN_E_EN_CMD_INDEX_ERROR	(1 << 3)
#define ARASAN_E_EN_DATA_TIMEOUT_ERROR	(1 << 4)
#define ARASAN_E_EN_DATA_CRC_ERROR	(1 << 5)
#define ARASAN_E_EN_DATA_END_ERROR	(1 << 6)
#define ARASAN_E_EN_CURRENT_LIMIT_ERROR	(1 << 7)
#define ARASAN_E_EN_AUTO_CMD12_ERROR	(1 << 8)
#define ARASAN_E_EN_ADMA_ERROR		(1 << 9)
#define ARASAN_E_EN_TARGET_RESP_ERROR	(1 << 12)
#define ARASAN_E_EN_CEATA_ERROR		(1 << 13)

/* Normal Interrupt Status Register */
#define ARASAN_N_CMD_COMPLETE		(1 << 0)
#define ARASAN_N_TRANS_COMPLETE		(1 << 1)
#define ARASAN_N_BLK_GAP_EVENT		(1 << 2)
#define ARASAN_N_DMA_IRQ		(1 << 3)
#define ARASAN_N_BUFF_WRITE		(1 << 4)
#define ARASAN_N_BUFF_READ		(1 << 5)
#define ARASAN_N_CARD_INS		(1 << 6)
#define ARASAN_N_CARD_REMOVAL		(1 << 7)
#define ARASAN_N_CARD_IRQ		(1 << 8)
#define ARASAN_N_ERROR_IRQ		(1 << 15)

/* Normal Interrupt Status ENABLE reg. (0- Masked, 1: Enabled) */
#define ARASAN_N_EN_CMD_COMPLETE	(1 << 0)
#define ARASAN_N_EN_TRANS_COMPL		(1 << 1)
#define ARASAN_N_EN_BLOCK_GAP		(1 << 2)
#define ARASAN_N_EN_DMA_IRQ		(1 << 3)
#define ARASAN_N_EN_BUFF_WRITE		(1 << 4)
#define ARASAN_N_EN_BUFF_READ		(1 << 5)
#define ARASAN_N_EN_CARD_INS		(1 << 6)
#define ARASAN_N_EN_CARD_REM		(1 << 7)
#define ARASAN_N_EN_CARD_IRQ		(1 << 8)

/* Default Enable Normal/Error interrupt mask */
#define ARASAN_IRQ_DEFAULT_MASK		0x02ff01fb
#define ARASAN_IRQ_DEFAULT_MASK_NOCARDIRQ		0x02ff00fb

/* Mask normal and error fields */
#define ARASAN_INT_DATA_MASK		0x0070003a
#define ARASAN_INT_CMD_MASK		0x000f0001

/* Command Register */
#define ARASAN_CMD_RSP_NONE		(0 << 0)
#define ARASAN_CMD_RSP_136		(1 << 0)
#define ARASAN_CMD_RSP_48		(2 << 0)
#define ARASAN_CMD_RSP_48BUSY		(3 << 0)
#define ARASAN_CMD_CHECK_CMDCRC		(1 << 3)
#define ARASAN_CMD_INDX_CHECK		(1 << 4)
#define ARASAN_CMD_DATA_PRESENT		(1 << 5)
#define ARASAN_COMMAD_TYPE_NORM		(0 << 6)
#define ARASAN_COMMAD_TYPE_SUSP		(1 << 6)
#define ARASAN_COMMAD_TYPE_RESU		(2 << 6)
#define ARASAN_COMMAD_TYPE_ABOR		(3 << 6)
#define ARASAN_CMD_INDEX(x)		((x) << 8)

/* Transfer Mode Register */
#define ARASAN_XFER_DMA_EN		(1 << 0)
#define ARASAN_XFER_BLK_COUNT_EN	(1 << 1)
#define ARASAN_XFER_AUTOCMD12		(1 << 2)	/* 1: Enable */
#define ARASAN_XFER_DATA_DIR		(1 << 4)	/* 0: Write, 1: Read */
#define ARASAN_XFER_MULTI_BLK		(1 << 5)	/* 0: Single 1: Multi */
#define ARASAN_XFER_SPI_MODE		(1 << 7)	/* 1: SPI 0: SD Mode */

enum xfer_dat_cmd_status {
STATE_CMD = 0,
STATE_DATA_WRITE = 1,
STATE_DATA_READ = 2,
STATE_DATA_STOP = 3,
};

/* Software Reset */
#define ARSAN_RESET_ALL			0x1
#define ARSAN_RESET_CMD_LINE		0x2
#define ARSAN_RESET_DAT_LINE		0x4

enum sw_reset_cmd {
	reset_all = 0,
	reset_cmd_line = 1,
	reset_dat_line = 2,
};

/* Host Control Register */
#define ARASAN_HOST_CTRL_LED		(1 << 0)
#define ARASAN_HOST_CTRL_SD		(1 << 1)	/* 1: 4 bit mode */
#define ARASAN_HOST_CTRL_HIGH_SPEED	(1 << 2)
#define ARASAN_HOST_CTRL_SDMA_SEL	(0 << 3)
#define ARASAN_HOST_CTRL_ADMA1		(1 << 3)
#define ARASAN_HOST_CTRL_ADMA2_32	(2 << 3)
#define ARASAN_HOST_CTRL_ADMA2_64	(3 << 3)
#define ARASAN_HOST_CTRL_SD8		(1 << 5)
#define ARASAN_HOST_CTRL_CARD_LEV_TEST	(1 << 6)
#define ARASAN_HOST_CTRL_CARD_SIG_TEST	(1 << 7)

#define ARASAN_HOST_CTRL_SD_MASK	0x22

/* Clock Control Register */
#define ARASAN_CLOCK_CTRL_SDCLK_MASK	0xff00
#define ARASAN_CLOCK_CTRL_SDCLK_SHIFT	7
#define ARASAN_CLOCK_CTRL_SDCLK_256	0x8000
#define ARASAN_CLOCK_CTRL_SDCLK_128	0x4000
#define ARASAN_CLOCK_CTRL_SDCLK_64	0x2000
#define ARASAN_CLOCK_CTRL_SDCLK_32	0x1000
#define ARASAN_CLOCK_CTRL_SDCLK_16	0x0800
#define ARASAN_CLOCK_CTRL_SDCLK_8	0x0400
#define ARASAN_CLOCK_CTRL_SDCLK_4	0x0200
#define ARASAN_CLOCK_CTRL_SDCLK_2	0x0100
#define ARASAN_CLOCK_CTRL_SDCLK_1	0x0000
#define ARASAN_CLOCK_CTRL_SDCLK_ENABLE	(1 << 2)
#define ARASAN_CLOCK_CTRL_ICLK_STABLE	(1 << 1)
#define ARASAN_CLOCK_CTRL_ICLK_ENABLE	(1 << 0)

/* Power Control Register */
#define ARASAN_PWR_CTRL_UP		(1 << 0)	/* 1: Power-On */
#define ARASAN_PWR_BUS_VOLTAGE_33	(7 << 1)
#define ARASAN_PWR_BUS_VOLTAGE_30	(6 << 1)
#define ARASAN_PWR_BUS_VOLTAGE_18	(5 << 1)

/* CMD12 error status bits */
#define ARASAN_AUTOCMD12_ERR_NOTEXE	(1 << 0)
#define ARASAN_AUTOCMD12_ERR_TIMEOUT	(1 << 1)
#define ARASAN_AUTOCMD12_ERR_CRC	(1 << 2)
#define ARASAN_AUTOCMD12_ERR_ENDBIT	(1 << 3)
#define ARASAN_AUTOCMD12_ERR_INDEX	(1 << 4)
#define ARASAN_AUTOCMD12_ERR_NOT_ISSUED	(1 << 7)

/* Present State Register */
#define ARASAN_PRESENT_STATE_DAT7_4       0x1e000000
#define ARASAN_PRESENT_STATE_CMD_LINE     0x01000000
#define ARASAN_PRESENT_STATE_DAT3_0       0x00f00000
#define ARASAN_PRESENT_STATE_WR_EN        0x00080000
#define ARASAN_PRESENT_STATE_CARD_DETECT  0x00040000
#define ARASAN_PRESENT_STATE_CARD_STABLE  0x00020000
#define ARASAN_PRESENT_STATE_CARD_PRESENT 0x00010000
#define ARASAN_PRESENT_STATE_BUFFER_RD_EN 0x00000800
#define ARASAN_PRESENT_STATE_BUFFER_WR_EN 0x00000400
#define ARASAN_PRESENT_STATE_RD_ACTIVE    0x00000200
#define ARASAN_PRESENT_STATE_WR_ACTIVE    0x00000100
#define ARASAN_PRESENT_STATE_DAT_ACTIVE   0x00000004
#define ARASAN_PRESENT_STATE_DAT_INHIBIT  0x00000002
#define ARASAN_PRESENT_STATE_CMD_INHIBIT  0x00000001

/* Block size register defines */
#define ARASAN_BLOCK_SIZE_SDMA_512KB	0x7000
#define ARASAN_BLOCK_SIZE_SDMA_256KB	0x6000
#define ARASAN_BLOCK_SIZE_SDMA_128KB	0x5000
#define ARASAN_BLOCK_SIZE_SDMA_64KB	0x4000
#define ARASAN_BLOCK_SIZE_SDMA_32KB	0x3000
#define ARASAN_BLOCK_SIZE_SDMA_16KB	0x2000
#define ARASAN_BLOCK_SIZE_SDMA_8KB	0x1000
#define ARASAN_BLOCK_SIZE_SDMA_4KB	0x0000
#define ARASAN_BLOCK_SIZE_TRANSFER	0x0fff

/* ADMA Error Status Register */
#define ARASAN_ADMA_ERROR_LENGTH          0x04
#define ARASAN_ADMA_ERROR_ST_TFR          0x03
#define ARASAN_ADMA_ERROR_ST_FDS          0x01
#define ARASAN_ADMA_ERROR_ST_STOP         0x00



struct arasan_cap {
	uint32_t timer_freq;
	uint32_t timer_unit;
	uint32_t base_clk_sd;
	uint32_t max_blk_len;
	uint32_t adma2;
	uint32_t high_speed;
	uint32_t sdma;
	uint32_t suspend;
	uint32_t voltage33;
	uint32_t voltage30;
	uint32_t voltage18;
	uint32_t int_mode;
	uint32_t spi;
	uint32_t spi_block;
};

struct arasan_host {
	uint32_t base;
	struct rt_mmcsd_req *mrq;
	uint32_t intr_en;
	uint8_t ctrl;
	ktimer_t timer;
	struct rt_mmcsd_host *mmc;
	int irq;
	struct arasan_cap cap;
	uint32_t vdd;
	uint32_t freq;
	uint32_t status;
	ksem_t   sem;
	uint8_t *adma_desc;
	uint32_t adma_addr;
	uint32_t need_detect;
	uint32_t card_irq;
	uint32_t auto_cmd12;
	uint32_t sdio_4bit_data;
};

typedef void (*func_t)(void);
struct arasan_platform_data {
	uint32_t id;
	uint32_t irq;
	uint32_t need_detect;
	uint32_t card_irq;
	uint32_t auto_cmd12; 
	uint32_t base;
	uint32_t sdio_4bit_data; 
	func_t p_powerup;
};

int arasan_probe(void *priv_data);

#endif
