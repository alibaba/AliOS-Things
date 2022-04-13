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

/**
 * @file spis.c
 * The SPI interface is a bit-serial, four-pin transmission protocol.
 * The SPI interface controller is a master responsible of the data
 * transmission with the slave.
 */

 /**
 * @defgroup IP_group_spis SPIS
 *     The SPIS interface is a bit-serial, four-pin transmission protocol.\n
 *     The SPIS interface controller is a master responsible of the data \n
 *     transmission with the slave\n
 *
 *   @{
 *       @defgroup IP_group_spis_external EXTERNAL
 *         The external API document for SPIS. \n
 *
 *         @{
 *             @defgroup IP_group_spis_external_function 1.function
 *               External SPIS function.
 *             @defgroup IP_group_spis_external_struct 2.structure
 *               none. No extra structure in SPIS driver.
 *             @defgroup IP_group_spis_external_typedef 3.typedef
 *               none. No extra typedef in SPIS driver.
 *             @defgroup IP_group_spis_external_enum 4.enumeration
 *               External SPI enumeration.
 *             @defgroup IP_group_spis_external_def 5.define
 *               none. No extra define in SPIS driver.
 *         @}
 *
 *       @defgroup IP_group_spis_internal INTERNAL
 *         The internal API document for SPIS. \n
 *
 *         @{
 *             @defgroup IP_group_spis_internal_function 1.function
 *               none. No extra function in SPIS driver.
 *             @defgroup IP_group_spis_internal_struct 2.structure
 *               none. No extra structure in SPIS driver.
 *             @defgroup IP_group_spis_internal_typedef 3.typedef
 *               none. No extra typedef in SPIS driver.
 *             @defgroup IP_group_spis_internal_enum 4.enumeration
 *               none. No extra enumeration in SPIS driver.
 *             @defgroup IP_group_spis_internal_def 5.define
 *               Internal define used for SPIS.
 *         @}
 *   @}
 */
#include "spis.h"

enum {
	MTK_SPIS_FIFO_DEPTH = 128, /* unit: Byte */
	MTK_TX_FIFO_DEFAULT_VALUE = 0x5a, /* tx fifo default value */
	MTK_TIMEOUT_MS = 3000  /* unit: ms */

};

/* define the global source clk */
int g_spis_source_clock;

struct mtk_spis_bus mtk_spis[MTK_SPIS_MAX_PORT_NUMBER];

unsigned long spis_base_addr[MTK_SPIS_MAX_PORT_NUMBER] = {
	SPIS_BASE,
};

int spis_irq_num[MTK_SPIS_MAX_PORT_NUMBER] = {
	SPIS_IRQ,
};

#define MIN(x, y) (x < y ? x : y)

/** @ingroup IP_group_spis_internal_function
 * @par Description
 *   spi slave get bus info
 * @param
 *   bus_num: spis bus port number
 * @return
  *  none.
 * @par Boundary case and Limitation
 *  none.
 * @par Error case and Error handling
 *  none.
 * @par Call graph and Caller graph (refer to the graph below)
 * @par Refer to the source code
 */
struct mtk_spis_bus *mtk_get_spis_bus(int bus_num)
{
	return &mtk_spis[bus_num];
}

/** @ingroup IP_group_spi_internal_function
 * @par Description
 *   spi slave set pinmux
 * @param
 *   bus_num: spis bus port number
 * @return
  *  none.
 * @par Boundary case and Limitation
 *  none.
 * @par Error case and Error handling
 *  none.
 * @par Call graph and Caller graph (refer to the graph below)
 * @par Refer to the source code
 */
void mtk_spis_set_gpio_mode(int bus_num)
{
	SPIS_INFO("config port%d pinmux to spis mode\n", bus_num);

	switch (bus_num) {
	case 0:
		gpio_set_mode(48, 4);
		gpio_set_mode(49, 4);
		gpio_set_mode(50, 4);
		gpio_set_mode(51, 4);
		break;
	default:
		SPIS_LOG("invalid bus_num(%d)\n", bus_num);
		break;
	}
}

/** @ingroup IP_group_spis_internal_function
 * @par Description
 *   spi slave 0 interrupt function
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
void mtk_spis0_interrupt(void)
{
	int int_status;
	BaseType_t	x_higher_priority_task_woken = pdFALSE;

	struct mtk_spis_bus *bus = mtk_get_spis_bus(0);

	SPIS_INFO("now in mtk_spis_interrupt, spis port0\n");

	int_status = readl(bus->reg_base + SPIS_IRQ_ST_REG);
	writel(int_status, bus->reg_base + SPIS_IRQ_CLR_REG);
	SPIS_INFO("SPIS_IRQ_ST_REG(0x%x)\n", int_status);

	xSemaphoreGiveFromISR(bus->semaph,
				&x_higher_priority_task_woken);
	portYIELD_FROM_ISR(x_higher_priority_task_woken);

}

/** @ingroup IP_group_spis_internal_function
 * @par Description
 *   spi slave interrupt request
 * @param
 *   bus_num: spis bus port number
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
int mtk_spis_request_irq(int bus_num)
{
	int ret = 0;
	struct mtk_spis_bus *bus = mtk_get_spis_bus(bus_num);

	switch (bus_num) {
	case 0:
		request_irq(bus->irq_num,
			mtk_spis0_interrupt,IRQ_TYPE_LEVEL_LOW, "spis0 irq handler",NULL);
		break;
	default:
		SPIS_LOG("invalid bus_num: %d\n", bus_num);
		ret = -1;
	}

	return ret;
}


/** @ingroup IP_group_spis_internal_function
 * @par Description
 *   spi slave interrupt free
 * @param
 *   bus_num: spis bus port number
 * @return
  *  none.
 * @par Boundary case and Limitation
 *  none.
 * @par Error case and Error handling
 *  none.
 * @par Call graph and Caller graph (refer to the graph below)
 * @par Refer to the source code
 */
static void mtk_spis_free_irq(int bus_num)
{
	struct mtk_spis_bus *bus = mtk_get_spis_bus(bus_num);

	free_irq(bus->irq_num);
}

/** @ingroup IP_group_spis_internal_function
 * @par Description
 * spis bus struct init
 * @param xfer:
 * bus num.
 * @return
 *     none.
 * @par Boundary case and Limitation
 *     none.
 * @par Error case and Error handling
 *     none
 * @par Call graph and Caller graph (refer to the graph below)
 * @par Refer to the source code
 */
void mtk_spis_base_init(int bus_num)
{
	struct mtk_spis_bus *bus = mtk_get_spis_bus(bus_num);

	bus->reg_base = spis_base_addr[bus_num];
	bus->irq_num = spis_irq_num[bus_num];
	if (!bus->semaph)
		bus->semaph = xSemaphoreCreateBinary();

	SPIS_LOG("spis bus num(%d),base_addr(0x%x),irq_num(%d)\n",
		bus_num, bus->reg_base, bus->irq_num);
}

/** @ingroup IP_group_spis_internal_function
 * @par Description
 * dma buffer map
 * @param xfer:
 * bus num.
 * xfer: spis transfer struct
 * @return
 *     none.
 * @par Boundary case and Limitation
 *     none.
 * @par Error case and Error handling
 *     none
 * @par Call graph and Caller graph (refer to the graph below)
 * @par Refer to the source code
 */
int mtk_spis_dma_map(int bus_num, struct spis_transfer *xfer)
{
	if (xfer->tx_buf) {
		xfer->tx_dma = (unsigned long long)xfer->tx_buf;
		SPIS_INFO("mtk_spis_dma_map:tx_buf(%p), tx_dma(0x%lx)\n",
				xfer->tx_buf, xfer->tx_dma);
	}

	if (xfer->rx_buf) {
		xfer->rx_dma = (unsigned long long)xfer->rx_buf;
		SPIS_INFO("mtk_spis_dma_map:rx_buf(%p), rx_dma(0x%lx)\n",
				xfer->rx_buf, xfer->rx_dma);
	}

	return 0;
}

/** @ingroup IP_group_spis_internal_function
 * @par Description
 * dma buffer unmap
 * @param xfer:
 * bus num.
 * xfer: spis transfer struct
 * @return
 *     none.
 * @par Boundary case and Limitation
 *     none.
 * @par Error case and Error handling
 *     none
 * @par Call graph and Caller graph (refer to the graph below)
 * @par Refer to the source code
 */
void mtk_spis_dma_unmap(int bus_num, struct spis_transfer *xfer)
{
}

/** @ingroup IP_group_spis_internal_function
 * @par Description
 *    dump spis buf data
 * @param
 * bus num.
 * buffer pointer
 * buffer size
 * @return
 *     none.
 * @par Boundary case and Limitation
 *     none.
 * @par Error case and Error handling
 *     none
 * @par Call graph and Caller graph (refer to the graph below)
 * @par Refer to the source code
 */
static void mtk_spis_dump_data(const char *name, char *data, int size)
{
	int i;

	SPIS_INFO("%s:\n", name);
	for (i = 0; i < size; i++)
		SPIS_INFO("data[%d]=0x%x\n", i, data[i]);
	SPIS_INFO("\n");
}

/** @ingroup IP_group_spis_internal_function
 * @par Description
 *    dump spis register
 * @param : bus num.
 * @return
 *     none.
 * @par Boundary case and Limitation
 *     none.
 * @par Error case and Error handling
 *     none
 * @par Call graph and Caller graph (refer to the graph below)
 * @par Refer to the source code
 */
void mtk_spis_dump_register(int bus_num)
{
	struct mtk_spis_bus *bus = mtk_get_spis_bus(bus_num);

	SPIS_INFO("SPIS_IRQ_EN_REG:0x%x\n",
			readl(bus->reg_base + SPIS_IRQ_EN_REG));
	SPIS_INFO("SPIS_IRQ_CLR_REG:0x%x\n",
			readl(bus->reg_base + SPIS_IRQ_CLR_REG));
	SPIS_INFO("SPIS_IRQ_ST_REG:0x%x\n",
			readl(bus->reg_base + SPIS_IRQ_ST_REG));
	SPIS_INFO("SPIS_IRQ_MASK_REG:0x%x\n",
			readl(bus->reg_base + SPIS_IRQ_MASK_REG));
	SPIS_INFO("SPIS_CFG_REG:0x%x\n",
			readl(bus->reg_base + SPIS_CFG_REG));
	SPIS_INFO("SPIS_RX_DST_REG:0x%x\n",
			readl(bus->reg_base + SPIS_RX_DST_REG));
	SPIS_INFO("SPIS_TX_SRC_REG:0x%x\n",
			readl(bus->reg_base + SPIS_TX_SRC_REG));
	SPIS_INFO("SPIS_FIFO_ST_REG:0x%x\n",
			readl(bus->reg_base + SPIS_FIFO_ST_REG));
	SPIS_INFO("SPIS_DMA_CFG_REG:0x%x\n",
			readl(bus->reg_base + SPIS_DMA_CFG_REG));
	SPIS_INFO("SPIS_FIFO_THR_REG:0x%x\n",
			readl(bus->reg_base + SPIS_FIFO_THR_REG));
	SPIS_INFO("SPIS_DEBUG_ST_REG:0x%x\n",
			readl(bus->reg_base + SPIS_DEBUG_ST_REG));
	SPIS_INFO("SPIS_BYTE_CNT_REG:0x%x\n",
			readl(bus->reg_base + SPIS_BYTE_CNT_REG));
	SPIS_INFO("SPIS_SOFT_RST_REG:0x%x\n",
			readl(bus->reg_base + SPIS_SOFT_RST_REG));
	SPIS_INFO("SPIS_TX_FIFO_DEFAULT_REG:0x%x\n",
			readl(bus->reg_base + SPIS_TX_FIFO_DEFAULT_REG));
}

/** @ingroup IP_group_spis_internal_function
 * @par Description
 *    disable dma tx rx transfer
 * @param r: bus num.
 * @return
 *     none.
 * @par Boundary case and Limitation
 *     none.
 * @par Error case and Error handling
 *     none
 * @par Call graph and Caller graph (refer to the graph below)
 * @par Refer to the source code
 */
void mtk_spis_disable_dma(int bus_num)
{
	int reg_val;
	struct mtk_spis_bus *bus = mtk_get_spis_bus(bus_num);

	/* disable dma tx/rx */
	reg_val = readl(bus->reg_base + SPIS_DMA_CFG_REG);
	reg_val &= ~RX_DMA_EN;
	reg_val &= ~TX_DMA_EN;
	writel(reg_val, bus->reg_base + SPIS_DMA_CFG_REG);
}

/** @ingroup IP_group_spis_internal_function
 * @par Description
 *    disable tx rx transfer
 * @param: bus num.
 * @return
 *     none.
 * @par Boundary case and Limitation
 *     none.
 * @par Error case and Error handling
 *     none
 * @par Call graph and Caller graph (refer to the graph below)
 * @par Refer to the source code
 */
void mtk_spis_disable_xfer(int bus_num)
{
	int reg_val;
	struct mtk_spis_bus *bus = mtk_get_spis_bus(bus_num);

	/* disable config reg tx rx_enable */
	reg_val = readl(bus->reg_base + SPIS_CFG_REG);
	reg_val &= ~SPIS_TX_EN;
	reg_val &= ~SPIS_RX_EN;
	writel(reg_val, bus->reg_base + SPIS_CFG_REG);
}

/** @ingroup IP_group_spis_internal_function
 * @par Description
 *    do spis fifo mode transfer prepare and wait for master trigger.
 * @param
 * bus num
 * xfer: struct spis_transfer.
 * @return
 *     none.
 * @par Boundary case and Limitation
 *     none.
 * @par Error case and Error handling
 *     none
 * @par Call graph and Caller graph (refer to the graph below)
 * @par Refer to the source code
 */
int mtk_spis_fifo_transfer(int bus_num, struct spis_transfer *xfer)
{
	int i, reg_val, cnt, remaind;
	unsigned int *outb = (unsigned int *)xfer->tx_buf;
	struct mtk_spis_bus *bus = mtk_get_spis_bus(bus_num);

	if (xfer->len > MTK_SPIS_FIFO_DEPTH) {
		SPIS_LOG("invaild length(%d)\n", xfer->len);
		return -1;
	}

	SPIS_INFO("%s:bus(%d),len(%d),enable_tx(%d),enable_rx(%d)\n",
		__func__, bus_num, xfer->len, bus->enable_tx, bus->enable_rx);

	bus->use_dma_mode = 0;

	/* just enable data_done enable and mask */
	reg_val = readl(bus->reg_base + SPIS_IRQ_EN_REG);
	reg_val |= DATA_DONE_EN;
	writel(reg_val, bus->reg_base + SPIS_IRQ_EN_REG);
	reg_val = readl(bus->reg_base + SPIS_IRQ_MASK_REG);
	reg_val |= DATA_DONE_MASK;
	writel(reg_val, bus->reg_base + SPIS_IRQ_MASK_REG);

	reg_val = readl(bus->reg_base + SPIS_CFG_REG);
	if (xfer->tx_buf && bus->enable_tx)
		reg_val |= SPIS_TX_EN;
	if (xfer->rx_buf && bus->enable_rx)
		reg_val |= SPIS_RX_EN;
	writel(reg_val, bus->reg_base + SPIS_CFG_REG);

#if USE_DEFAULT_TX_FIFO_VALUE
	writel(MTK_TX_FIFO_DEFAULT_VALUE,
		bus->reg_base + SPIS_TX_FIFO_DEFAULT_REG);
#else
	cnt = xfer->len / 4;
	remaind = xfer->len % 4;
	for (i = 0; i < cnt; i++)
		writel(outb[i], bus->reg_base + SPIS_TX_DATA_REG);
	if (remaind) {
		outb[cnt] &= (0xffffffff >> (4 - remaind) * 4);
		writel(outb[cnt], bus->reg_base + SPIS_TX_DATA_REG);
	}
#endif

	if (xfer->tx_buf)
		mtk_spis_dump_data("spis out data is", xfer->tx_buf, xfer->len);

	SPIS_INFO("Before transfer:\n");
	mtk_spis_dump_register(bus_num);
	SPIS_INFO("SPIS TX FIFO pointer(DWORD):%d\n",
		(readl(bus->reg_base + SPIS_FIFO_ST_REG) & 0x1F));
	SPIS_INFO("SPIS RX FIFO pointer(DWORD):%d\n",
		(readl(bus->reg_base + SPIS_FIFO_ST_REG) & 0x1F0000) >> 16);

	return 0;
}

/** @ingroup IP_group_spis_internal_function
 * @par Description
 *    do spis dma mode transfer prepare and wait for master trigger.
 * @param xfer: struct spis_transfer.
 * @return
 *     none.
 * @par Boundary case and Limitation
 *     none.
 * @par Error case and Error handling
 *     none
 * @par Call graph and Caller graph (refer to the graph below)
 * @par Refer to the source code
 */
int mtk_spis_dma_transfer(int bus_num, struct spis_transfer *xfer)
{
	int reg_val;
	struct mtk_spis_bus *bus = mtk_get_spis_bus(bus_num);

	SPIS_INFO("%s:bus(%d),len(%d),enable_tx(%d),enable_rx(%d)\n",
		__func__, bus_num, xfer->len, bus->enable_tx, bus->enable_rx);

	bus->use_dma_mode = 1;

	/* soft rest for dma */
	writel(SPIS_SOFT_RST1, bus->reg_base + SPIS_SOFT_RST_REG);

	/* just enable dma_done enable and mask */
	reg_val = readl(bus->reg_base + SPIS_IRQ_EN_REG);
	reg_val |= DMA_DONE_EN;
	writel(reg_val, bus->reg_base + SPIS_IRQ_EN_REG);
	reg_val = readl(bus->reg_base + SPIS_IRQ_MASK_REG);
	reg_val |= DMA_DONE_MASK;
	writel(reg_val, bus->reg_base + SPIS_IRQ_MASK_REG);

	mtk_spis_disable_dma(bus_num);
	mtk_spis_disable_xfer(bus_num);

	mtk_spis_dma_map(bus_num, xfer);

	if (xfer->tx_buf && bus->enable_tx)
		writel(xfer->tx_dma, bus->reg_base + SPIS_TX_SRC_REG);

	if (xfer->rx_buf && bus->enable_rx)
		writel(xfer->rx_dma, bus->reg_base + SPIS_RX_DST_REG);

	writel(SPIS_DMA_ADDR_LOAD, bus->reg_base + SPIS_SOFT_RST_REG);

	/* enable config reg tx/rx_enable */
	reg_val = readl(bus->reg_base + SPIS_CFG_REG);
	if (xfer->tx_buf && bus->enable_tx)
		reg_val |= SPIS_TX_EN;
	if (xfer->rx_buf && bus->enable_rx)
		reg_val |= SPIS_RX_EN;
	writel(reg_val, bus->reg_base + SPIS_CFG_REG);

	/* config dma */
	reg_val = 0;
	reg_val &= ~TX_DMA_LEN_MASK;
	reg_val |= (xfer->len - 1) & TX_DMA_LEN_MASK;
	writel(reg_val, bus->reg_base + SPIS_DMA_CFG_REG);

	reg_val = readl(bus->reg_base + SPIS_DMA_CFG_REG);
	if (xfer->tx_buf && bus->enable_tx) {
		reg_val |= TX_DMA_EN;
		reg_val |= TX_DMA_TRIG_EN;
	}

	if (xfer->rx_buf && bus->enable_rx)
		reg_val |= RX_DMA_EN;

	writel(reg_val, bus->reg_base + SPIS_DMA_CFG_REG);

	if (xfer->tx_buf)
		mtk_spis_dump_data("spis out data is", xfer->tx_buf, xfer->len);

	SPIS_INFO("Before transfer:\n");
	mtk_spis_dump_register(bus_num);

	return 0;
}

/** @ingroup IP_group_spis_external_function
 * @par Description
 *    Register interrupt isr handler function
 * @param id: spis id.
 * @return
 *     SPIS_NO_ERROR, function success.\n
 *     -EINVAL, Invalid argument .
 *	-ESEMAPH, create semaphore  error
 *	-EQUEUE, create queue  error
 *	-ETASK, create task  error
 * @par Boundary case and Limitation
 *     none.
 * @par Error case and Error handling
 *     none
 * @par Call graph and Caller graph (refer to the graph below)
 * @par Refer to the source code
 */
int mtk_spis_init(int bus_num)
{
	if (bus_num > MTK_SPIS_MAX_PORT_NUMBER) {
		SPIS_LOG("%s:invalid bus num\n", __func__);
		return -1;
	}

	mtk_spis_base_init(bus_num);
	mtk_spis_request_irq(bus_num);

	mtk_spis_set_gpio_mode(bus_num);

	mtk_spis_disable_dma(bus_num);
	mtk_spis_disable_xfer(bus_num);

	return 0;
}

/** @ingroup IP_group_spis_external_function
 * @par Description
 *    free interrupt isr and release this spis id source
 * @param id: spis id.
 * @return
 *     SPIS_NO_ERROR, function success.\n
 *     -EINVAL, Invalid argument .
 * @par Boundary case and Limitation
 *     none.
 * @par Error case and Error handling
 *     none
 * @par Call graph and Caller graph (refer to the graph below)
 * @par Refer to the source code
 */
int mtk_spis_exit(int bus_num)
{
	struct mtk_spis_bus *bus = mtk_get_spis_bus(bus_num);

	if (bus_num > MTK_SPIS_MAX_PORT_NUMBER) {
		SPIS_LOG("%s:invalid bus num\n", __func__);
		return -1;
	}

	mtk_spis_free_irq(bus_num);
	if (bus->semaph)
		vQueueDelete(bus->semaph);
	bus->semaph = NULL;

	return 0;
}

/** @ingroup IP_group_spis_external_function
 * @par Description
 *     SPIS Transfer configuration.
 * @param
 *     xfer: struct of spis related config and \n
 *			its tx data source & rx data destination.
 * @return
 *   0, function success.\n
 *  -EINVAL, spis id is wrong.
  * -ETIMEOUT, Timeout error.
 * @par Boundary case and Limitation
 *  none.
 * @par Error case and Error handling
 *  none.
 * @par Call graph and Caller graph (refer to the graph below)
 * @par Refer to the source code
 */
int mtk_spis_config(int bus_num, struct mtk_spis_chip_config *chip_config)
{
	int reg_val;
	struct mtk_spis_bus *bus = mtk_get_spis_bus(bus_num);

	if (bus_num > MTK_SPIS_MAX_PORT_NUMBER) {
		SPIS_LOG("%s:invalid bus num\n", __func__);
		return -1;
	}

	if (!chip_config) {
		SPIS_LOG("%s:chip_config is NULL\n", __func__);
		return -1;
	}

	bus->enable_tx = chip_config->enable_tx;
	bus->enable_rx = chip_config->enable_rx;

	reg_val = readl(bus->reg_base + SPIS_CFG_REG);
	if (chip_config->tx_mlsb)
		reg_val |= SPIS_TXMSBF;
	else
		reg_val &= ~SPIS_TXMSBF;

	if (chip_config->rx_mlsb)
		reg_val |= SPIS_RXMSBF;
	else
		reg_val &= ~SPIS_RXMSBF;

	if (chip_config->cpha)
		reg_val |= SPIS_CPHA;
	else
		reg_val &= ~SPIS_CPHA;

	if (chip_config->cpol)
		reg_val |= SPIS_CPOL;
	else
		reg_val &= ~SPIS_CPOL;

	if (chip_config->tx_endian)
		reg_val |= SPIS_TX_ENDIAN;
	else
		reg_val &= ~SPIS_TX_ENDIAN;

	if (chip_config->rx_endian)
		reg_val |= SPIS_RX_ENDIAN;
	else
		reg_val &= ~SPIS_RX_ENDIAN;
	writel(reg_val, bus->reg_base + SPIS_CFG_REG);

	return 0;
}

/** @ingroup IP_group_spis_external_function
 * @par Description
 *    enable spis clock.
 * @param[in]
 *     none.
 * @return
 *     0, select spis source clock successfully.\n
 *     -EINVAL, invalid spi source clock.
 * @par Boundary case and Limitation
 *     none.
 * @par Error case and Error handling
 *     1. Invalid spi  clock, return -EINVAL.
 * @par Call graph and Caller graph
 * @par Refer to the source code
 */
int mtk_spis_enable_clock()
{
	int ret1,ret2;
	struct clk *clk, *infra;

	clk = clk_get(CLK_TOP_SPIS_SEL);
	if(is_err_or_null_clk(clk)){
		return -1;
	}
	ret1 = clk_enable(clk);
	if(ret1==0)
		SPIS_INFO("Enable spis clk success.\n");
	else
		SPIS_INFO("Enable spis clk fail.\n");

	infra = clk_get(CLK_INFRA_SPIS);
	if(is_err_or_null_clk(infra)){
		return -2;
	}
	ret2 = clk_enable(infra);
	if(ret2==0)
		SPIS_INFO("Enable spis infra success.\n");
	else
		SPIS_INFO("Enable spis infra fail.\n");

	return 0;
}

/** @ingroup IP_group_spis_external_function
 * @par Description
 *    wait spis to be idle.
 * @param id: spis id.
 * @return
 *    0,  take semaphore success
 *   -1, take semaphore fail
 * @par Boundary case and Limitation
 *     none.
 * @par Error case and Error handling
 *     none
 * @par Call graph and Caller graph (refer to the graph below)
 * @par Refer to the source code
 */
int mtk_spis_wait_transfer_done(int bus_num, struct spis_transfer *xfer)
{
	int ret = 0;
	int i, reg_val;
	struct mtk_spis_bus *bus = mtk_get_spis_bus(bus_num);

	if (bus_num > MTK_SPIS_MAX_PORT_NUMBER) {
		SPIS_LOG("%s:invalid bus num\n", __func__);
		return -1;
	}

	if (!bus->semaph) {
		SPIS_LOG("bus_num(%d) semaph is NULL\n", bus_num);
		ret = -1;
		goto out;
	}
	if (pdTRUE != xSemaphoreTake(bus->semaph,
		MTK_TIMEOUT_MS/portTICK_RATE_MS)) {
		SPIS_LOG("Take spi slave Semaphore timeout!\n");
		ret = -1;
	}

	SPIS_INFO("After transfer:\n");
	mtk_spis_dump_register(bus_num);

out:
	if (bus->use_dma_mode) {
		writel(SPIS_SOFT_RST1, bus->reg_base + SPIS_SOFT_RST_REG);
		mtk_spis_disable_dma(bus_num);
		mtk_spis_dma_unmap(bus_num, xfer);

	} else {
		SPIS_INFO("SPIS TX FIFO pointer(DWORD):%d\n",
		(readl(bus->reg_base + SPIS_FIFO_ST_REG) & 0x1F));
		SPIS_INFO("SPIS RX FIFO pointer(DWORD):%d\n",
		(readl(bus->reg_base + SPIS_FIFO_ST_REG) & 0x1F0000) >> 16);

		if (xfer->rx_buf && bus->enable_rx) {
			for (i = 0; i < xfer->len; i++) {
				if (i % 4 == 0)
					reg_val =
					readl(bus->reg_base + SPIS_RX_DATA_REG);
				*(xfer->rx_buf + i) =
					(reg_val >> ((i % 4) * 8)) & 0xff;
			}
		}
	}

	mtk_spis_disable_xfer(bus_num);
	/* disable irq enable and mask */
	writel(0, bus->reg_base + SPIS_IRQ_EN_REG);
	writel(0, bus->reg_base + SPIS_IRQ_MASK_REG);

	if (xfer->rx_buf)
		mtk_spis_dump_data("spis in data is", xfer->rx_buf, xfer->len);

	return ret;
}

/** @ingroup IP_group_spis_external_function
 * @par Description
 *     SPIS TX/RX Transfer Function.
 * @param
 *     xfer: struct of spis related config and \n
 *			its tx data source & rx data destination.
 * @return
 *     If return value is 0, function success.\n
 *     If return value is -1, reset not released.
 * @par Boundary case and Limitation
 *  none.
 * @par Error case and Error handling
 *  none.
 * @par Call graph and Caller graph (refer to the graph below)
 * @par Refer to the source code
 */
int mtk_spis_transfer_one(int bus_num, struct spis_transfer *xfer)
{
	int ret = 0;

	if (bus_num > MTK_SPIS_MAX_PORT_NUMBER) {
		SPIS_LOG("%s:invalid bus num\n", __func__);
		return -1;
	}

	if (!xfer) {
		SPIS_LOG("%s:xfer is NULL\n", __func__);
		return -1;
	}
	if ((!xfer->len) || ((!xfer->tx_buf) && (!xfer->rx_buf))) {
		SPIS_LOG("%s:Invalid parameter\n", __func__);
		return -1;
	}
	SPIS_LOG("spis transfer len = %d\n", xfer->len);

	if (xfer->len > 32)
		ret = mtk_spis_dma_transfer(bus_num, xfer);
	else
		ret = mtk_spis_fifo_transfer(bus_num, xfer);

	return ret;
}

