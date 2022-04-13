/* Copyright Statement:
 *
 * @2018 MediaTek Inc. All rights reserved.
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

/**
 * @file spim.c
 * SPI-Master FreeRTOS Driver.
 * The SPI interface controller is a master responsible of the data
 * transmission with the slave.
 */

/**
 * @defgroup IP_group_spi SPI
 *     The SPI interface controller is a master responsible of the data \n
 *     transmission with the slave\n
 *
 *   @{
 *       @defgroup IP_group_spi_external EXTERNAL
 *         The external API document for SPI. \n
 *
 *         @{
 *             @defgroup IP_group_spi_external_function 1.function
 *               External SPI function.
 *             @defgroup IP_group_spi_external_struct 2.structure
 *               none. No extra structure in SPI driver.
 *             @defgroup IP_group_spi_external_typedef 3.typedef
 *               none. No extra typedef in SPI driver.
 *             @defgroup IP_group_spi_external_enum 4.enumeration
 *               External SPI enumeration.
 *             @defgroup IP_group_spi_external_def 5.define
 *               none. No extra define in SPI driver.
 *         @}
 *
 *       @defgroup IP_group_spi_internal INTERNAL
 *         The internal API document for SPI. \n
 *
 *         @{
 *             @defgroup IP_group_spi_internal_function 1.function
 *               none. No extra function in SPI driver.
 *             @defgroup IP_group_spi_internal_struct 2.structure
 *               none. No extra structure in SPI driver.
 *             @defgroup IP_group_spi_internal_typedef 3.typedef
 *               none. No extra typedef in SPI driver.
 *             @defgroup IP_group_spi_internal_enum 4.enumeration
 *               none. No extra enumeration in SPI driver.
 *             @defgroup IP_group_spi_internal_def 5.define
 *               Internal define used for SPI.
 *         @}
 *   @}
 */
#include "spim.h"

extern void mdelay(unsigned long msec);

void mtk_spim_mdelay(int delay)
{
	mdelay(delay);
}


#define MIN(x, y) (x < y ? x : y)
static inline unsigned long div_round_up(unsigned int n, unsigned int d)
{
	return (n + d - 1) / d;
}

/* record spi source clk */
static unsigned int g_spim_source_clock;
struct mtk_spim_bus mtk_spim[MTK_SPIM_MAX_PORT_NUMBER];


enum {
	MTK_FIFO_DEPTH = 32,
	MTK_PACKET_LENGTH = 1024,
	MTK_PACKET_LOOP_CNT = 256,
	MTK_TIMEOUT_MS = 1000,
	MTK_ARBITRARY_VALUE = 0xdeaddead
};

unsigned long spim_base_addr[MTK_SPIM_MAX_PORT_NUMBER] = {
	SPIM_BASE,
};

int spim_irq_num[MTK_SPIM_MAX_PORT_NUMBER] = {
	SPIM_IRQ,
};

/** @ingroup IP_group_spi_internal_function
 * @par Description
 *   spi master get bus info
 * @param
 *   bus_num: spi bus port number
 * @return
  *  none.
 * @par Boundary case and Limitation
 *  none.
 * @par Error case and Error handling
 *  none.
 * @par Call graph and Caller graph (refer to the graph below)
 * @par Refer to the source code
 */
static struct mtk_spim_bus *mtk_get_spim_bus(int bus_num)
{
	return &mtk_spim[bus_num];
}

/** @ingroup IP_group_spi_internal_function
 * @par Description
 *   dump data for debug
 * @param
 *   bus_num: spi bus port number
 * @return
  *  none.
 * @par Boundary case and Limitation
 *  none.
 * @par Error case and Error handling
 *  none.
 * @par Call graph and Caller graph (refer to the graph below)
 * @par Refer to the source code
 */
static void mtk_spim_dump_data(const char *name, char *data, int size)
{
#if MTK_SPI_DEBUG
	int i;


	SPI_LOG("%s:\n", name);
	for (i = 0; i < size; i++)
		SPI_LOG("data[%d]=0x%x\n", i, data[i]);
	SPI_LOG("\n");
#endif
}

/** @ingroup IP_group_spi_internal_function
 * @par Description
 *   dump register for debug
 * @param
 *   bus_num: spi bus port number
 * @return
  *  none.
 * @par Boundary case and Limitation
 *  none.
 * @par Error case and Error handling
 *  none.
 * @par Call graph and Caller graph (refer to the graph below)
 * @par Refer to the source code
 */
static void mtk_spim_dump_register(int bus_num)
{
#if MTK_SPI_DEBUG
	struct mtk_spim_bus *bus = mtk_get_spim_bus(bus_num);

	SPI_INFO("SPI_CFG0_REG:0x%x\n", readl(bus->reg_base + SPI_CFG0_REG));
	SPI_INFO("SPI_CFG1_REG:0x%x\n", readl(bus->reg_base + SPI_CFG1_REG));
	SPI_INFO("SPI_CFG2_REG:0x%x\n", readl(bus->reg_base + SPI_CFG2_REG));
	SPI_INFO("SPI_TX_SRC_REG:0x%x\n",
		readl(bus->reg_base + SPI_TX_SRC_REG));
	SPI_INFO("SPI_RX_DST_REG:0x%x\n",
		readl(bus->reg_base + SPI_RX_DST_REG));
	SPI_INFO("SPI_CMD_REG:0x%x\n", readl(bus->reg_base + SPI_CMD_REG));
	SPI_INFO("SPI_STATUS1_REG:0x%x\n",
		readl(bus->reg_base + SPI_STATUS1_REG));
#endif
}

/** @ingroup IP_group_spi_internal_function
 * @par Description
 *   spi master base initilize
 * @param
 *   bus_num: spi bus port number
 * @return
  *  none.
 * @par Boundary case and Limitation
 *  none.
 * @par Error case and Error handling
 *  none.
 * @par Call graph and Caller graph (refer to the graph below)
 * @par Refer to the source code
 */
static void mtk_spim_base_init(int bus_num)
{
	struct mtk_spim_bus *bus = mtk_get_spim_bus(bus_num);

	bus->reg_base = spim_base_addr[bus_num];
	bus->irq_num = spim_irq_num[bus_num];
	bus->pause_enable = 0;
	bus->state = MTK_SPI_IDLE;
	bus->semaph = xSemaphoreCreateBinary();

	SPI_LOG("spim bus num(%d),base_addr(0x%x),irq_num(%d)\n",
		bus_num, bus->reg_base, bus->irq_num);
}

/** @ingroup IP_group_spi_internal_function
 * @par Description
 *   spi master set pinmux
 * @param
 *   bus_num: spi bus port number
 * @return
  *  none.
 * @par Boundary case and Limitation
 *  none.
 * @par Error case and Error handling
 *  none.
 * @par Call graph and Caller graph (refer to the graph below)
 * @par Refer to the source code
 */
static void mtk_spim_set_gpio_mode(int bus_num)
{
	SPI_INFO("set bus_num%d pinmux to spi mode\n", bus_num);

	switch (bus_num) {
	case 0:
		gpio_set_mode(33, 6);
		gpio_set_mode(34, 6);
		gpio_set_mode(35, 6);
		gpio_set_mode(36, 6);
		break;
	default:
		SPI_LOG("invalid bus_num(%d)\n", bus_num);
		break;
	}
}

/** @ingroup IP_group_spi_internal_function
 * @par Description
 *   spi master 0 interrupt function
 * @param
 *   none.
 * @return
  *  none.
 * @par Boundary case and Limitation
 *  none.
 * @par Error case and Error handling
 *  none.
 * @par Call graph and Caller graph (refer to the graph below)
 * @par Refer to the source code
 */
static void mtk_spim0_interrupt(void)
{
	int reg_val;
	BaseType_t	x_higher_priority_task_woken = pdFALSE;
	struct mtk_spim_bus *bus = mtk_get_spim_bus(0);

	SPI_INFO("now in mtk_spim_interrupt, spim port 0\n");
	reg_val = readl(bus->reg_base + SPI_STATUS0_REG);
	//SPI_LOG("SPI_STATUS0_REG(0x%x)\n", reg_val);
	reg_val = readl(bus->reg_base + SPI_STATUS1_REG);
	//SPI_LOG("SPI_STATUS1_REG(0x%x)\n", reg_val);

	xSemaphoreGiveFromISR(bus->semaph,&x_higher_priority_task_woken);
	portYIELD_FROM_ISR(x_higher_priority_task_woken);
}

/** @ingroup IP_group_spi_internal_function
 * @par Description
 *   spi master interrupt request
 * @param
 *   bus_num: spi bus port number
 * @return
  *  0, pass
  *  -1, fail
 * @par Boundary case and Limitation
 *  none.
 * @par Error case and Error handling
 *  none.
 * @par Call graph and Caller graph (refer to the graph below)
 * @par Refer to the source code
 */
static int mtk_spim_request_irq(int bus_num)
{
	int ret = 0;

	struct mtk_spim_bus *bus = mtk_get_spim_bus(bus_num);

	switch (bus_num) {
	case 0:
		request_irq(bus->irq_num,
			mtk_spim0_interrupt,IRQ_TYPE_LEVEL_LOW, "spim0 irq handler",NULL);
		break;
	default:
		SPI_LOG("invalid bus_num: %d\n", bus_num);
		ret = -1;
	}

	return ret;
}

/** @ingroup IP_group_spi_internal_function
 * @par Description
 *   spi master interrupt free
 * @param
 *   bus_num: spi bus port number
 * @return
  *  none.
 * @par Boundary case and Limitation
 *  none.
 * @par Error case and Error handling
 *  none.
 * @par Call graph and Caller graph (refer to the graph below)
 * @par Refer to the source code
 */
static void mtk_spim_free_irq(int bus_num)
{
	struct mtk_spim_bus *bus = mtk_get_spim_bus(bus_num);

	free_irq(bus->irq_num);
}

/** @ingroup IP_group_spi_internal_function
 * @par Description
 *   spi master dma map
 * @param
 *   bus_num: spi bus port number
  *   xfer: struct spi_transfer
 * @return
  *  none.
 * @par Boundary case and Limitation
 *  none.
 * @par Error case and Error handling
 *  none.
 * @par Call graph and Caller graph (refer to the graph below)
 * @par Refer to the source code
 */
static int mtk_spim_dma_map(int bus_num, struct spi_transfer *xfer)
{
	if (xfer->tx_buf) {
		xfer->tx_dma = (unsigned long int)xfer->tx_buf;
		SPI_INFO("mtk_spim_dma_map:tx_buf(0x%p), tx_dma(0x%lx)\n",
				xfer->tx_buf, xfer->tx_dma);
	}

	if (xfer->rx_buf) {
		xfer->rx_dma = (unsigned long int)xfer->rx_buf;
		SPI_INFO("mtk_spim_dma_map:rx_buf(0x%p), rx_dma(0x%lx)\n",
				xfer->rx_buf, xfer->rx_dma);
	}

	return 0;
}

/** @ingroup IP_group_spi_internal_function
 * @par Description
 *   spi master dma unmap
 * @param
 *   bus_num: spi bus port number
  *   xfer: struct spi_transfer
 * @return
  *  none.
 * @par Boundary case and Limitation
 *  none.
 * @par Error case and Error handling
 *  none.
 * @par Call graph and Caller graph (refer to the graph below)
 * @par Refer to the source code
 */
static void mtk_spim_dma_unmap(int bus_num, struct spi_transfer *xfer)
{
}

/** @ingroup IP_group_spi_internal_function
 * @par Description
 *   spi master software reset
 * @param
 *   bus_num: spi bus port number
 * @return
  *  none.
 * @par Boundary case and Limitation
 *  none.
 * @par Error case and Error handling
 *  none.
 * @par Call graph and Caller graph (refer to the graph below)
 * @par Refer to the source code
 */
static void mtk_spim_reset(int bus_num)
{
	int reg_val;
	struct mtk_spim_bus *bus = mtk_get_spim_bus(bus_num);

	reg_val = readl(bus->reg_base + SPI_CMD_REG);
	reg_val |= 1 << SPI_CMD_RST_SHIFT;
	writel(reg_val, bus->reg_base + SPI_CMD_REG);

	reg_val = readl(bus->reg_base + SPI_CMD_REG);
	reg_val &= ~(1 << SPI_CMD_RST_SHIFT);
	writel(reg_val, bus->reg_base + SPI_CMD_REG);
}

/** @ingroup IP_group_spi_internal_function
 * @par Description
 *   spi master enable transfer
 * @param
 *   bus_num: spi bus port number
 * @return
  *  none.
 * @par Boundary case and Limitation
 *  none.
 * @par Error case and Error handling
 *  none.
 * @par Call graph and Caller graph (refer to the graph below)
 * @par Refer to the source code
 */
static void mtk_spim_enable_transfer(int bus_num)
{
	int reg_val;
	struct mtk_spim_bus *bus = mtk_get_spim_bus(bus_num);

	reg_val = readl(bus->reg_base + SPI_CMD_REG);
	if (bus->state == MTK_SPI_IDLE) {
		reg_val |= 1 << SPI_CMD_ACT_SHIFT;
		bus->state = MTK_SPI_PAUSE_IDLE;
	} else if (bus->state == MTK_SPI_PAUSE_IDLE) {
		reg_val |= 1 << SPI_CMD_RESUME_SHIFT;
	}
	writel(reg_val, bus->reg_base + SPI_CMD_REG);
}

/** @ingroup IP_group_spi_internal_function
 * @par Description
 *   get transfer spend time
 * @param
 *   bus_num: spi bus port number
  *  len: transfer len
 * @return
 *   time_ms: transfer spend time.
 * @par Boundary case and Limitation
 *  none.
 * @par Error case and Error handling
 *  none.
 * @par Call graph and Caller graph (refer to the graph below)
 * @par Refer to the source code
 */
static unsigned int mtk_get_transfer_spend_time(int bus_num, int len)
{
	unsigned int time_ms, sclk, reg_val;
	int high_time, low_time;

	struct mtk_spim_bus *bus = mtk_get_spim_bus(bus_num);

	reg_val = readl(bus->reg_base + SPI_CFG2_REG);
	high_time = (reg_val & SPI_CFG2_SCK_HIGH_MASK)
		>> SPI_CFG2_SCK_HIGH_SHIFT;
	low_time = (reg_val & SPI_CFG2_SCK_LOW_MASK)
		>> SPI_CFG2_SCK_LOW_SHIFT;

	/* get spi master sclk */
	sclk = (g_spim_source_clock * 1000) / (high_time + low_time + 2);
	/* get spend time for this transfer base on transfer len */
	time_ms = (len * 8 * 1000) / sclk;

	return time_ms;
}


/** @ingroup IP_group_spi_internal_function
 * @par Description
 *   spi master wait for transfer done
 * @param
 *   bus_num: spi bus port number
 * @return
 *   0, function success
 *   -1, function success
 * @par Boundary case and Limitation
 *  none.
 * @par Error case and Error handling
 *  none.
 * @par Call graph and Caller graph (refer to the graph below)
 * @par Refer to the source code
 */
static int mtk_spim_wait_for_transfer_done(int bus_num, int xfer_len)
{
	int ret = 0;
	unsigned int time_ms;

	struct mtk_spim_bus *bus = mtk_get_spim_bus(bus_num);

	time_ms = mtk_get_transfer_spend_time(bus_num, xfer_len);
	time_ms += 1000;
	if (pdTRUE != xSemaphoreTake(bus->semaph,time_ms/portTICK_RATE_MS)) {
		SPI_LOG("Take spi master Semaphore timeout!\n");
		ret = -1;
		goto error_out;
	}

	return ret;

	error_out:
		SPI_INFO("timeout then dump reg:\n");
		mtk_spim_dump_register(bus_num);
		mtk_spim_reset(bus_num);
		bus->state = MTK_SPI_IDLE;

		return ret;
}

/** @ingroup IP_group_spi_internal_function
 * @par Description
 *   spi master setup packet
 * @param
 *   bus_num: spi bus port number
 *   size: spi master transfer size
 * @return
 *  none.
 * @par Boundary case and Limitation
 *  none.
 * @par Error case and Error handling
 *  none.
 * @par Call graph and Caller graph (refer to the graph below)
 * @par Refer to the source code
 */
static void mtk_spim_setup_packet(struct mtk_spim_bus *bus, int size)
{
	int packet_size, packet_loop, reg_val = 0;

	packet_size = MIN(size, MTK_PACKET_LENGTH);
	packet_loop = div_round_up(size, packet_size);

	/* set transfer packet and loop */
	reg_val = readl(bus->reg_base + SPI_CFG1_REG);
	reg_val &= ~(SPI_CFG1_PACKET_LENGTH_MASK | SPI_CFG1_PACKET_LOOP_MASK);
	reg_val |= (packet_size - 1) << SPI_CFG1_PACKET_LENGTH_SHIFT;
	reg_val |= (packet_loop - 1) << SPI_CFG1_PACKET_LOOP_SHIFT;
	writel(reg_val, bus->reg_base + SPI_CFG1_REG);
}

/** @ingroup IP_group_spi_internal_function
 * @par Description
 *   spi master FIFO transfer data.
 * @param
 *   bus_num: spi bus port number
 *   xfer: spi transfer struct
 * @return
 *   0, function success
 *   -1, function success
 * @par Boundary case and Limitation
 *  none.
 * @par Error case and Error handling
 *  none.
 * @par Call graph and Caller graph (refer to the graph below)
 * @par Refer to the source code
 */
static int mtk_spim_fifo_transfer(int bus_num, struct spi_transfer *xfer)
{
	int ret = 0;
	int i, reg_val, cnt, remaind;
	unsigned int *outb = (unsigned int *)xfer->tx_buf;

	struct mtk_spim_bus *bus = mtk_get_spim_bus(bus_num);

	SPI_INFO("%s:bus(%d),len(%d)\n", __func__, bus_num, xfer->len);

	if (!xfer) {
		SPI_LOG("%s:xfer is NULL\n", __func__);
		return -1;
	}

	if (xfer->len > MTK_FIFO_DEPTH) {
		SPI_LOG("invaild length(%d)\n", xfer->len);
		return -1;
	}

	mtk_spim_setup_packet(bus, xfer->len);

	remaind = xfer->len % 4;
	cnt = xfer->len / 4;

	if (xfer->rx_buf && !xfer->tx_buf) {
		for (i = 0; i <= cnt; i++)
			writel(MTK_ARBITRARY_VALUE,
				   bus->reg_base + SPI_TX_DATA_REG);
	}

	if (xfer->tx_buf) {
		for (i = 0; i < cnt; i++)
			writel(outb[i], bus->reg_base + SPI_TX_DATA_REG);
		if (remaind) {
			outb[cnt] &= (0xffffffff >> (4 - remaind) * 4);
			writel(outb[cnt], bus->reg_base + SPI_TX_DATA_REG);
		}
	}

	SPI_INFO("\nspi before transfer:\n");
	mtk_spim_dump_register(bus_num);

	if (xfer->tx_buf)
		mtk_spim_dump_data("spi before transfer out data is",
				  xfer->tx_buf, xfer->len);

	mtk_spim_enable_transfer(bus_num);
	ret = mtk_spim_wait_for_transfer_done(bus_num, xfer->len);
	if (ret)
		return ret;

	if (xfer->rx_buf) {
		for (i = 0; i < xfer->len; i++) {
			if ((i & 0x03) == 0)
				reg_val = readl(bus->reg_base +
					  SPI_RX_DATA_REG);
			*(xfer->rx_buf + i) =
				(reg_val >> ((i & 0x03) << 3)) & 0xff;
		}
		mtk_spim_dump_data("spi after transfer in data is",
				  xfer->rx_buf, xfer->len);
	}

	SPI_INFO("\nspi after transfer:\n");
	mtk_spim_dump_register(bus_num);

	return ret;
}

/** @ingroup IP_group_spi_internal_function
 * @par Description
 *   spi master DMA transfer data.
 * @param
 *   bus_num: spi bus port number
 *   xfer: spi transfer struct
 * @return
 *   0, function success
 *   -1, function success
 * @par Boundary case and Limitation
 *  none.
 * @par Error case and Error handling
 *  none.
 * @par Call graph and Caller graph (refer to the graph below)
 * @par Refer to the source code
 */
static int mtk_spim_dma_transfer(int bus_num, struct spi_transfer *xfer)
{
	struct mtk_spim_bus *bus = mtk_get_spim_bus(bus_num);
	int ret, reg_val;

	SPI_INFO("%s:bus(%d),len(%d)\n", __func__, bus_num, xfer->len);

	if (!xfer) {
		SPI_LOG("%s:xfer is NULL\n", __func__);
		return -1;
	}

	reg_val = readl(bus->reg_base + SPI_CMD_REG);
	if (!xfer->rx_buf && xfer->tx_buf)
		reg_val |= 1 << SPI_CMD_TX_DMA_SHIFT;
	else
		reg_val |= (1 << SPI_CMD_RX_DMA_SHIFT) |
				 (1 << SPI_CMD_TX_DMA_SHIFT);
	writel(reg_val, bus->reg_base + SPI_CMD_REG);

	mtk_spim_setup_packet(bus, xfer->len);
	mtk_spim_dma_map(bus_num, xfer);

	if (xfer->tx_buf)
		writel(xfer->tx_dma, bus->reg_base + SPI_TX_SRC_REG);
	if (xfer->rx_buf)
		writel(xfer->rx_dma, bus->reg_base + SPI_RX_DST_REG);

	if (xfer->tx_buf)
		mtk_spim_dump_data("outb", xfer->tx_buf, xfer->len);

	SPI_INFO("before transfer:\n");
	mtk_spim_dump_register(bus_num);

	mtk_spim_enable_transfer(bus_num);

	ret = mtk_spim_wait_for_transfer_done(bus_num, xfer->len);
	if (ret)
		goto error_out;

	mtk_spim_dma_unmap(bus_num, xfer);

	reg_val = readl(bus->reg_base + SPI_CMD_REG);
	reg_val &= ~(1 << SPI_CMD_RX_DMA_SHIFT | 1 << SPI_CMD_TX_DMA_SHIFT);
	writel(reg_val, bus->reg_base + SPI_CMD_REG);

	SPI_INFO("after transfer:\n");
	mtk_spim_dump_register(bus_num);

	if (xfer->rx_buf)
		mtk_spim_dump_data("inb", xfer->rx_buf, xfer->len);

	return 0;
error_out:
	mtk_spim_dma_unmap(bus_num, xfer);
	mtk_spim_reset(bus_num);
	bus->state = MTK_SPI_IDLE;
	reg_val = readl(bus->reg_base + SPI_CMD_REG);
	reg_val &= ~((1 << SPI_CMD_RX_DMA_SHIFT) | (1 << SPI_CMD_TX_DMA_SHIFT));
	writel(reg_val, bus->reg_base + SPI_CMD_REG);
	return -1;
}

/** @ingroup IP_group_spi_external_function
 * @par Description
 *   spi master transfer initialize
 * @param
 *   bus_num: spi bus port number
 * @return
 *   0, function success
 *   -1, function success
 * @par Boundary case and Limitation
 *  none.
 * @par Error case and Error handling
 *  none.
 * @par Call graph and Caller graph (refer to the graph below)
 * @par Refer to the source code
 */
int mtk_spim_init(int bus_num)
{
	if (bus_num > MTK_SPIM_MAX_PORT_NUMBER) {
		SPI_LOG("%s:invalid bus num\n", __func__);
		return -1;
	}

	mtk_spim_base_init(bus_num);
	mtk_spim_set_gpio_mode(bus_num);
	mtk_spim_request_irq(bus_num);

	return 0;
}

/** @ingroup IP_group_spi_external_function
 * @par Description
 *   spi master transfer exit
 * @param
 *   bus_num: spi bus port number
 * @return
 *   0, function success
 *   -1, function success
 * @par Boundary case and Limitation
 *  none.
 * @par Error case and Error handling
 *  none.
 * @par Call graph and Caller graph (refer to the graph below)
 * @par Refer to the source code
 */
int mtk_spim_exit(int bus_num)
{
	struct mtk_spim_bus *bus = mtk_get_spim_bus(bus_num);

	if (bus_num > MTK_SPIM_MAX_PORT_NUMBER) {
		SPI_LOG("%s:invalid bus num\n", __func__);
		return -1;
	}
	mtk_spim_free_irq(bus_num);
	if (bus->semaph)
		vQueueDelete(bus->semaph);

	bus->semaph = NULL;

	return 0;
}

/** @ingroup IP_group_spi_external_function
 * @par Description
 *     Enable spi clock,Select spi source clock.
 * @param[in]
 *     source_clock_hz: spi source clock value.
 * @return
 *     0, select spi source clock successfully.\n
 *     -EINVAL, invalid spi source clock.
 * @par Boundary case and Limitation
 *     none.
 * @par Error case and Error handling
 *     1. Invalid spi source clock, return -EINVAL.
 * @par Call graph and Caller graph
 * @par Refer to the source code
 */
 int mtk_spim_enable_clock(uint32_t source_clock_hz)
{
	int ret1,ret2,ret3;
	struct clk *clk, *clk_parent, *infra;

	clk = clk_get(CLK_TOP_SPI_SEL);
	if(is_err_or_null_clk(clk)){
		return -1;
	}
	ret1 = clk_enable(clk);
	if(ret1 == 0)
		SPI_INFO("Enable spim clk success.\n");
	else
		SPI_INFO("Enable spim clk fail.\n");

	g_spim_source_clock = source_clock_hz/1000;
	switch(g_spim_source_clock) {
		case MTK_SPIM_CLK_26M:
			clk_parent = clk_get(CLK_TOP_CLK26M);/*26M*/
			SPI_INFO("SourceClk:26M\n");
			break;
		case MTK_SPIM_UNIVPLL2_D2:
			clk_parent = clk_get(CLK_TOP_UNIVPLL2_D2);/*208M*/
			SPI_INFO("SourceClk:208M\n");
			break;
		case MTK_SPIM_SYSPLL2_D2:
			clk_parent = clk_get(CLK_TOP_SYSPLL2_D2);/*182M*/
			SPI_INFO("SourceClk:182M\n");
			break;
		case MTK_SPIM_UNIVPLL1_D4:
			clk_parent = clk_get(CLK_TOP_UNIVPLL1_D4);/*156M*/
			SPI_INFO("SourceClk:156M\n");
			break;
		case MTK_SPIM_SYSPLL1_D4:
			clk_parent = clk_get(CLK_TOP_SYSPLL1_D4);/*136.5M*/
			SPI_INFO("SourceClk:136.5M\n");
			break;
		case MTK_SPIM_UNIVPLL3_D2:
			clk_parent = clk_get(CLK_TOP_UNIVPLL3_D2);/*124.8M*/
			SPI_INFO("SourceClk:124.8M\n");
			break;
		case MTK_SPIM_UNIVPLL2_D4:
			clk_parent = clk_get(CLK_TOP_UNIVPLL2_D4);/*104M*/
			SPI_INFO("SourceClk:104M\n");
			break;
		case MTK_SPIM_SYSPLL4_D2:
			clk_parent = clk_get(CLK_TOP_SYSPLL4_D2);/*78M*/
			SPI_INFO("SourceClk:78M\n");
			break;
		default:
			clk_parent = clk_get(CLK_TOP_UNIVPLL2_D2);/*208M*/
			SPI_INFO("DEFAULT:SourceClk:208M\n");
			break;
	}
	if(is_err_or_null_clk(clk_parent)){
		return -2;
	}
	ret2 = clk_enable(clk_parent);
	if(ret2 == 0)
		SPI_INFO("Enable spim clk_parent success.\n");
	else
		SPI_INFO("Enable spim clk_parent fail.\n");

	infra = clk_get(CLK_INFRA_SPI);
	if(is_err_or_null_clk(infra)){
		return -3;
	}
	ret3 = clk_enable(infra);
	if(ret3 == 0)
		SPI_INFO("Enable spim infra success.\n");
	else
		SPI_INFO("Enable spim infra fail.\n");

	clk_set_parent(clk,clk_parent);

	return 0;

}

/** @ingroup IP_group_spi_external_function
 * @par Description
 *   spi master transfer config
 * @param
 *   bus_num: spi bus port number
 *   chip_config: spi transfer config struct
 * @return
 *   0, function success
 *   -1, function success
 * @par Boundary case and Limitation
 *  none.
 * @par Error case and Error handling
 *  none.
 * @par Call graph and Caller graph (refer to the graph below)
 * @par Refer to the source code
 */
int mtk_spim_config(int bus_num, struct mtk_chip_config *chip_config)
{
	int reg_val;
	struct mtk_spim_bus *bus = mtk_get_spim_bus(bus_num);

	if (bus_num > MTK_SPIM_MAX_PORT_NUMBER) {
		SPI_LOG("%s:invalid bus num\n", __func__);
		return -1;
	}

	if (!chip_config) {
		SPI_LOG("%s:chip_config is NULL\n", __func__);
		return -1;
	}
	/* cs hold/setup time */
	writel(((chip_config->hold_time - 1) << SPI_CFG0_CS_HOLD_SHIFT) |
		((chip_config->setup_time - 1) << SPI_CFG0_CS_SETUP_SHIFT),
		bus->reg_base + SPI_CFG0_REG);

	/* clock high/low time */
	writel(((chip_config->low_time - 1) << SPI_CFG2_SCK_LOW_SHIFT) |
		((chip_config->high_time - 1) << SPI_CFG2_SCK_HIGH_SHIFT),
		bus->reg_base + SPI_CFG2_REG);

	/* cs idletime/tick_dly */
	reg_val = readl(bus->reg_base + SPI_CFG1_REG);
	reg_val &= ~SPI_CFG1_CS_IDLE_MASK;
	reg_val |= (chip_config->cs_idletime - 1) << SPI_CFG1_CS_IDLE_SHIFT;
	reg_val &= ~SPI_CFG1_GET_TICK_DLY_MASK;
	reg_val |= (chip_config->tck_dly) << SPI_CFG1_GET_TICK_DLY_SHIFT;
	writel(reg_val, bus->reg_base + SPI_CFG1_REG);

	reg_val = readl(bus->reg_base + SPI_CMD_REG);
	/* cpol/cpha */
	if (chip_config->cpha)
		reg_val |= SPI_CMD_CPHA_EN;
	else
		reg_val &= ~SPI_CMD_CPHA_EN;
	if (chip_config->cpol)
		reg_val |= SPI_CMD_CPOL_EN;
	else
		reg_val &= ~SPI_CMD_CPOL_EN;

	/* set the mlsbx and mlsbtx */
	if (chip_config->tx_mlsb)
		reg_val |= SPI_CMD_TXMSBF_EN;
	else
		reg_val &= ~SPI_CMD_TXMSBF_EN;
	if (chip_config->rx_mlsb)
		reg_val |= SPI_CMD_RXMSBF_EN;
	else
		reg_val &= ~SPI_CMD_RXMSBF_EN;

	/* set the tx/rx endian */
	if (chip_config->tx_endian)
		reg_val |= SPI_CMD_TX_ENDIAN_EN;
	else
		reg_val &= ~SPI_CMD_TX_ENDIAN_EN;
	if (chip_config->rx_endian)
		reg_val |= SPI_CMD_RX_ENDIAN_EN;
	else
		reg_val &= ~SPI_CMD_RX_ENDIAN_EN;

	/* set pause mode */
	if (chip_config->pause) {
		reg_val |= SPI_CMD_PAUSE_EN;
		reg_val |= SPI_CMD_PAUSE_IE_EN;
		bus->pause_enable = 1;
	} else {
		reg_val &= ~SPI_CMD_PAUSE_EN;
		reg_val &= ~SPI_CMD_PAUSE_IE_EN;
		bus->pause_enable = 0;
	}

	/* set CS_N */
	if (chip_config->cs_pin_sel) {
		reg_val |= SPI_CMD_CS_PIN_SEL_EN;
		bus->cs_pin_sel = 1;
	} else {
		reg_val &= ~SPI_CMD_CS_PIN_SEL_EN;
		bus->cs_pin_sel = 0;
	}

	/* set finish interrupt always enable */
	reg_val |= SPI_CMD_FINISH_IE_EN;

	/* disable dma mode */
	reg_val &= ~(SPI_CMD_TX_DMA_EN | SPI_CMD_RX_DMA_EN);

	/* set deassert mode */
	if (chip_config->de_assert)
		reg_val |= SPI_CMD_DEASSERT_EN;
	else
		reg_val &= ~SPI_CMD_DEASSERT_EN;

	/* set cs_pol */
	if (chip_config->cs_pol)
		reg_val |= SPI_CMD_CS_POL_EN;
	else
		reg_val &= ~SPI_CMD_CS_POL_EN;

	/* set sample_sel */
	if (chip_config->sample_sel)
		reg_val |= SPI_CMD_SAMPLE_SEL_EN;
	else
		reg_val &= ~SPI_CMD_SAMPLE_SEL_EN;

	writel(reg_val, bus->reg_base + SPI_CMD_REG);

	return 0;
}

/** @ingroup IP_group_spi_external_function
 * @par Description
 *   spi master transfer data api.
 * @param
 *   bus_num: spi bus port number
 *   xfer: spi transfer struct
 * @return
 *   0, function success
 *   -1, function success
 * @par Boundary case and Limitation
 *  none.
 * @par Error case and Error handling
 *  none.
 * @par Call graph and Caller graph (refer to the graph below)
 * @par Refer to the source code
 */
int mtk_spim_transfer_one(int bus_num, struct spi_transfer *xfer)
{
	int ret = 0, reg_val;
	char dma_en;
	int len_raw = 0, len_xferd = 0;
	struct spi_transfer current_xfer;
	struct mtk_spim_bus *bus = mtk_get_spim_bus(bus_num);

	if (bus_num > MTK_SPIM_MAX_PORT_NUMBER) {
		SPI_LOG("%s:invalid bus num\n", __func__);
		return -1;
	}

	if (!xfer) {
		SPI_LOG("%s:xfer is NULL\n", __func__);
		return -1;
	}

	if ((!xfer->len) || ((!xfer->tx_buf) && (!xfer->rx_buf))) {
		SPI_LOG("%s:Invalid parameter\n", __func__);
		return -1;
	}
	//SPI_LOG("spim transfer len = %d\n", xfer->len);

	len_raw = xfer->len;
	if (len_raw > 32)
		dma_en = 1;
	else
		dma_en = 0;

	/* pause mode enable */
	reg_val = readl(bus->reg_base + SPI_CMD_REG);
	reg_val |= SPI_CMD_PAUSE_EN;
	reg_val |= SPI_CMD_PAUSE_IE_EN;
	writel(reg_val, bus->reg_base + SPI_CMD_REG);
	bus->pause_enable = 1;

	while (len_raw) {
		current_xfer.len = MIN(len_raw, MTK_PACKET_LENGTH);
		if (xfer->tx_buf)
			current_xfer.tx_buf = xfer->tx_buf + len_xferd;
		if (xfer->rx_buf)
			current_xfer.rx_buf = xfer->rx_buf + len_xferd;

		if (dma_en){
			SPI_INFO("dma transfer.\n");
			ret = mtk_spim_dma_transfer(bus_num, &current_xfer);
		}
		else{
			SPI_INFO("fifo transfer.\n");
			ret = mtk_spim_fifo_transfer(bus_num, &current_xfer);
		}

		if (ret)
			break;

		len_raw -= current_xfer.len;
		len_xferd += current_xfer.len;

		SPI_INFO("\nlen_raw = %d, len_xferd = %d\n",
			len_raw, len_xferd);
	}

	/* pause mode disable*/
	reg_val = readl(bus->reg_base + SPI_CMD_REG);
	reg_val &= ~SPI_CMD_PAUSE_EN;
	reg_val &= ~SPI_CMD_PAUSE_IE_EN;
	writel(reg_val, bus->reg_base + SPI_CMD_REG);
	bus->pause_enable = 0;

	mtk_spim_reset(bus_num);
	bus->state = MTK_SPI_IDLE;

	return ret;
}

