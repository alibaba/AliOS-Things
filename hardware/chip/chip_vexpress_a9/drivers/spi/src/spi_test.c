/* Copyright Statement:
 *
 * @2015 MediaTek Inc. All rights reserved.
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. and/or its licensors.
 * Without the prior written permission of MediaTek Inc. and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein,
 * in whole or in part, shall be strictly prohibited.
 *
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES
 * AND AGREES THAT THE SOFTWARE/FIRMWARE AND ITS
 * DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES
 * ARE PROVIDED TO RECEIVER ON
 * AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO.
 * RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY
 * THIRD PARTY ALL PROPER LICENSES CONTAINED IN MEDIATEK SOFTWARE.
 * MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR
 * TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND
 * MEDIATEK'S ENTIRE AND CUMULATIVE LIABILITY WITH RESPECT TO
 * THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE.
 */

#define MTK_SPICLI_ENABLE

#if defined(MTK_SPICLI_ENABLE)

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <FreeRTOS.h>
#include "spim.h"
#include "spis.h"
#include "task.h"
#include "aos/cli.h"

struct mtk_chip_config spim_defualt_chip_config = {
	.setup_time = 100,
	.hold_time = 100,
	.high_time = 10,
	.low_time = 10,
	.cs_idletime = 2,
	.cpol = SPI_CPOL_0,
	.cpha = SPI_CPHA_0,
	.rx_mlsb = SPI_MSB,
	.tx_mlsb = SPI_MSB,
	.tx_endian = SPI_LENDIAN,
	.rx_endian = SPI_LENDIAN,
	.de_assert = DEASSERT_DISABLE,
	.tck_dly = TICK_DLY0,
	.cs_pin_sel = SPI_CS_N,
};

struct mtk_chip_config spim_chip_config;

struct mtk_spis_chip_config spis_defualt_chip_config = {
	.enable_tx = 1,
	.enable_rx = 1,
	.cpol = SPIS_CPOL_0,
	.cpha = SPIS_CPHA_0,
	.rx_mlsb = SPIS_MSB,
	.tx_mlsb = SPIS_MSB,
	.tx_endian = SPIS_LENDIAN,
	.rx_endian = SPIS_LENDIAN,
};

struct mtk_spis_chip_config spis_chip_config;


int mtk_spi_data_check(char *tx_buf, char *rx_buf, int len)
{
	char value;
	int i, abnormal = 0, err = 0;

	for (i = 0; i < len; i++) {
		value = *(tx_buf + i);

		if (value != *(rx_buf + i)) {
			SPI_LOG("tx[%d]:0x%x, rx[%d]:0x%x\r\n",
					i, value, i, *(rx_buf + i));
			err++;
		}
		if (!value || (value == 0xff))
			abnormal++;
	}

	if (abnormal == len) {
		SPI_LOG("The data is ALL 0 or 0xff!\n");
		err++;
	}

	if (err)
		return -1;
	else
		return 0;
}

void *mtk_spim_mem_alloc(unsigned int size)
{
	return pvPortMalloc(size);
}

void mtk_spim_mem_free(void *pAddr)
{
	vPortFree(pAddr);
}

void spim_loopback_test(int bus, int size, int clk, int tick)
{
	int i, ret = 0;
	char *spim_tx_buf, *spim_rx_buf;
	struct spi_transfer xfer;

	int bus_num = bus;
	int packet_size = size;
	int clk_cnt = clk;
	int tick_dly = tick;

	mtk_spim_enable_clock(208000000);
	SPI_LOG("bus_num=%d,len=%d,clk_cnt=%d,tick_dly=%d\n",
		bus_num, packet_size, clk_cnt, tick_dly);

	spim_tx_buf = mtk_spim_mem_alloc(packet_size);
	if (!spim_tx_buf) {
		SPI_LOG("spim_tx_buf malloc fail!\n");
		goto exit;
	}
	spim_rx_buf = mtk_spim_mem_alloc(packet_size);
	if (!spim_rx_buf) {
		SPI_LOG("spim_rx_buf malloc fail!\n");
		goto free_spim_tx_buf;
	}

	memset(&xfer, 0, sizeof(struct spi_transfer));
	memcpy(&spim_chip_config, &spim_defualt_chip_config,
		sizeof(struct mtk_chip_config));

	spim_chip_config.high_time = clk_cnt;
	spim_chip_config.low_time = clk_cnt;
	spim_chip_config.tck_dly  = tick_dly;

	xfer.len = packet_size;
	xfer.tx_buf = spim_tx_buf;
	xfer.rx_buf = spim_rx_buf;

	for (i = 0; i < xfer.len; i++)
		*((char *)xfer.tx_buf + i) = i + 0x12;

	mtk_spim_init(bus_num);
	mtk_spim_config(bus_num, &spim_chip_config);
	ret = mtk_spim_transfer_one(bus_num, &xfer);
	if (ret) {
		SPI_LOG("mtk_spim_transfer_one fail!\n");
		ret = -1;
		goto free_spim_rx_buf;
	}
	ret = mtk_spi_data_check(spim_tx_buf, spim_rx_buf, packet_size);
	if (ret)
		SPI_LOG(" spi master loopback test fail!\n");
	else
		SPI_LOG(" spi master loopback test pass!\n");
free_spim_rx_buf:
	if (spim_tx_buf)
		mtk_spim_mem_free(spim_tx_buf);
free_spim_tx_buf:
	if (spim_rx_buf)
		mtk_spim_mem_free(spim_rx_buf);

exit:
	mtk_spim_exit(bus_num);
}

void spi_m2s_loopback_test(int m_bus, int s_bus, int size, int clk, int tick)
{
	int i, ret = 0;
	char *spim_tx_buf, *spim_rx_buf, *spis_tx_buf, *spis_rx_buf;
	struct spi_transfer spim_xfer;
	struct spis_transfer spis_xfer;

	int spim_bus_num = m_bus;
	int spis_bus_num = s_bus;
	int packet_size = size;
	int clk_cnt = clk;
	int tick_dly = tick;

	mtk_spim_enable_clock(208000000);
	mtk_spis_enable_clock();
	SPI_LOG("spim_bus_num = %d, spis_bus_num = %d, len = %d\n",
		spim_bus_num, spis_bus_num, packet_size);
	SPI_LOG("clk_cnt = %d, tick_dly = %d\n",
		clk_cnt, tick_dly);

	spim_tx_buf = mtk_spim_mem_alloc(packet_size);
	if (!spim_tx_buf) {
		SPI_LOG("spim_tx_buf malloc fail!\n");
		goto exit;
	}
	spim_rx_buf = mtk_spim_mem_alloc(packet_size);
	if (!spim_rx_buf) {
		SPI_LOG("spim_rx_buf malloc fail!\n");
		goto free_spim_tx_buf;
	}
	spis_tx_buf = mtk_spim_mem_alloc(packet_size);
	if (!spis_tx_buf) {
		SPI_LOG("spis_tx_buf malloc fail!\n");
		goto free_spim_rx_buf;
	}
	spis_rx_buf = mtk_spim_mem_alloc(packet_size);
	if (!spis_rx_buf) {
		SPI_LOG("spis_rx_buf malloc fail!\n");
		goto  free_spis_tx_buf;
	}

	memset(&spim_xfer, 0, sizeof(struct spi_transfer));
	memcpy(&spim_chip_config, &spim_defualt_chip_config,
		sizeof(struct mtk_chip_config));
	memset(&spis_xfer, 0, sizeof(struct spis_transfer));
	memcpy(&spis_chip_config, &spis_defualt_chip_config,
		sizeof(struct mtk_spis_chip_config));

	spim_chip_config.high_time = clk_cnt;
	spim_chip_config.low_time = clk_cnt;
	spim_chip_config.tck_dly  = tick_dly;

	spis_xfer.len = packet_size;
	spis_xfer.tx_buf = spis_tx_buf;
	spis_xfer.rx_buf = spis_rx_buf;

	spim_xfer.len = packet_size;
	spim_xfer.tx_buf = spim_tx_buf;
	spim_xfer.rx_buf = spim_rx_buf;

	for (i = 0; i < spim_xfer.len; i++)
		*((char *)spim_xfer.tx_buf + i) = i + 0x12;

	for (i = 0; i < spis_xfer.len; i++)
		*((char *)spis_xfer.tx_buf + i) = i + 0x12;

	mtk_spis_init(spis_bus_num);
	mtk_spis_config(spis_bus_num, &spis_chip_config);
	ret = mtk_spis_transfer_one(spis_bus_num, &spis_xfer);
	if (ret) {
		SPI_LOG("mtk_spis_transfer_one fail!\n");
		ret = -1;
		goto fail;
	}

	mtk_spim_init(spim_bus_num);
	mtk_spim_config(spim_bus_num, &spim_chip_config);
	ret = mtk_spim_transfer_one(spim_bus_num, &spim_xfer);
	if (ret) {
		SPI_LOG("mtk_spim_transfer_one fail!\n");
		ret = -1;
		goto fail;
	}

	ret = mtk_spis_wait_transfer_done(spis_bus_num, &spis_xfer);
	if (ret)
		SPI_LOG("mtk_spis_wait_for_transfer_done() fail!\n");

	SPI_LOG("spi master tx/rx data compare\n");
	ret = mtk_spi_data_check(spim_tx_buf, spim_rx_buf, packet_size);
	SPI_LOG("spi slave tx/rx data compare\n");
	ret += mtk_spi_data_check(spis_tx_buf, spis_rx_buf, packet_size);

	if (ret)
		SPI_LOG(" spi master to slave loopback test fail!\n");
	else
		SPI_LOG(" spi master to slave  loopback test pass!\n");


fail:
	if (spis_rx_buf)
		mtk_spim_mem_free(spis_rx_buf);
free_spis_tx_buf:
	if (spis_tx_buf)
		mtk_spim_mem_free(spis_tx_buf);
free_spim_rx_buf:
	if (spim_rx_buf)
		mtk_spim_mem_free(spim_rx_buf);
free_spim_tx_buf:
	if (spim_tx_buf)
		mtk_spim_mem_free(spim_tx_buf);

exit:
	mtk_spim_exit(spim_bus_num);
	mtk_spis_exit(spis_bus_num);

}

static void malloc_free_test(void)
{
	int i;
	char *spim_tx_buf;

	for(i=1;i<=1024*1024;i++)
	{
		SPI_LOG("spim_tx_buf malloc:%d!\n",i);
		spim_tx_buf = mtk_spim_mem_alloc(i);
		if (!spim_tx_buf)
			SPI_LOG("spim_tx_buf malloc fail!\n");
		mtk_spim_mem_free(spim_tx_buf);
	}
}

static int cli_spim_loopback_test (char *buf, int len, int argc, char **argv)
{
	int bus, size, clk, tick;
	
	SPI_LOG("enter cli_spim_loopback_test\n");
	bus = strtoul(argv[1], NULL, 0);
	size = strtoul(argv[2], NULL, 0);
	clk = strtoul(argv[3], NULL, 0);
	tick = strtoul(argv[4], NULL, 0);
	SPI_LOG("bus:%d, len:%d, clk:%d, tick:%d\n", bus, size, clk, tick);
	
	spim_loopback_test(bus, size, clk, tick);
}

static int cli_spi_m2s_loopback_test (char *buf, int len, int argc, char **argv)
{
	int m_bus, s_bus, size, clk, tick;
	
	SPI_LOG("enter cli_spi_m2s_loopback_test\n");
	m_bus = strtoul(argv[1], NULL, 0);
	s_bus = strtoul(argv[2], NULL, 0);
	size = strtoul(argv[3], NULL, 0);
	clk = strtoul(argv[4], NULL, 0);
	tick = strtoul(argv[5], NULL, 0);
	SPI_LOG("m_bus:%d, s_bus:%d, len:%d, clk:%d, tick:%d\n", m_bus, s_bus, size, clk, tick);
	
	spi_m2s_loopback_test(m_bus, s_bus, size, clk, tick);
}

static struct cli_command mt_spi_cmd[] = {
	{"spim_loopback_test", "spim_loopback_test, [#1]port, [#2]len, [#3]clk, [#4]tick", cli_spim_loopback_test},
	{"spi_m2s_loopback_test", "spi_m2s_loopback_test, [#1]port_m, [#2]port_s, [#3]len, [#4]clk, [#5]tick", cli_spi_m2s_loopback_test},
};

void spi_cli_init(void)
{
	aos_cli_register_commands(&mt_spi_cmd[0],sizeof(mt_spi_cmd) / sizeof(struct cli_command));
}

#endif

