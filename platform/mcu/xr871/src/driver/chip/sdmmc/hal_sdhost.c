/**
  * @file  hal_sdhost.c
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

#include "../hal_base.h"

#include "sys/io.h"
#include "driver/chip/hal_gpio.h"

#include "driver/chip/sdmmc/hal_sdhost.h"
#ifdef CONFIG_USE_SDIO
#include "driver/chip/sdmmc/sdio.h"
#endif
#include "driver/chip/sdmmc/sdmmc.h"
#include "sdhost.h"
#include "core.h"

#include "pm/pm.h"

#if ((defined CONFIG_USE_SD) || (defined CONFIG_USE_MMC))
#define SDC_DMA_TIMEOUT         2000 /* not much data to write on this platform */
#else
#define SDC_DMA_TIMEOUT         300
#endif
#define SDC_THREAD_TIMEOUT      (SDC_DMA_TIMEOUT + 50)

#define SDC_SemCreate(l, n)     HAL_SemaphoreInit(l, n, OS_SEMAPHORE_MAX_COUNT)
#define SDC_SemDel(l)           HAL_SemaphoreDeinit(l)
#define SDC_SemPend(l, t)       HAL_SemaphoreWait(l, t)
#define SDC_SemPost(l)          HAL_SemaphoreRelease(l)

#define SDC_MutexCreate(m)      OS_MutexCreate(m);
#define SDC_MutexDelete(m)      OS_MutexDelete(m);
#define SDC_MutexLock(m, t)     OS_MutexLock(m, t)
#define SDC_MutexUnlock(m)      OS_MutexUnlock(m);

#define SDC_InitTimer(t, cb, arg, pms)  OS_TimerCreate(t, OS_TIMER_ONCE, cb, arg, pms)
#define SDC_DelTimer(t)                 OS_TimerDelete(t)
#define SDC_ModTimer(t, ms)             do {if (!ms) SDC_BUG_ON(1); \
                                            OS_TimerChangePeriod(t, ms);} while (0)

#define SDC_REQUEST_IRQ(n, hdl)         HAL_NVIC_SetIRQHandler(n, hdl)
#define SDC_SetPriority(n, l)           HAL_NVIC_SetPriority(n, l)
#define SDC_ENABLE_IRQ(n)               HAL_NVIC_EnableIRQ(n)
#define SDC_DISABLE_IRQ(n)              HAL_NVIC_DisableIRQ(n)
#define SDC_CLEAR_IRQPINGD(n)           HAL_NVIC_ClearPendingIRQ(n)
#define SDC_IRQHandler                  NVIC_IRQHandler

//#define NUSE_STANDARD_INTERFACE  1
#ifdef NUSE_STANDARD_INTERFACE
#define SDC_CCM_BASE                    (0x40040400)
#define SDC_CCM_SDC0_SCLK_CTRL          (SDC_CCM_BASE + 0x028)
#else
#define SDC_CCM_BusForceReset()         HAL_CCM_BusForcePeriphReset(CCM_BUS_PERIPH_BIT_SDC0)
#define SDC_CCM_BusReleaseRest()        HAL_CCM_BusReleasePeriphReset(CCM_BUS_PERIPH_BIT_SDC0)
#define SDC_CCM_BusEnableClock()        HAL_CCM_BusEnablePeriphClock(CCM_BUS_PERIPH_BIT_SDC0)
#define SDC_CCM_BusDisableClock()       HAL_CCM_BusDisablePeriphClock(CCM_BUS_PERIPH_BIT_SDC0)
#define SDC_CCM_EnableMClock()          HAL_CCM_SDC_EnableMClock()
#endif

#define MEMS_VA2PA(x) (x)

struct mmc_host *_mci_host;

static int32_t __mci_exit_host(struct mmc_host *host)
{
	uint32_t rval;

#ifdef CONFIG_SDC_SUPPORT_1V8
	host->voltage = SDC_WOLTAGE_OFF;
#endif

	rval = readl(SDXC_REG_GCTRL) | SDXC_HWReset;
	writel(rval, SDXC_REG_GCTRL);

	return 0;
}

static __inline void __mci_sel_access_mode(uint32_t access_mode)
{
	writel((readl(SDXC_REG_GCTRL) & (~SDXC_ACCESS_BY_AHB)) | access_mode, SDXC_REG_GCTRL);
}

static int32_t __mci_reset(void)
{
	uint32_t value;
	int32_t time = 0x0ffff;

	value = readl(SDXC_REG_GCTRL)|SDXC_HWReset;
	writel(value, SDXC_REG_GCTRL);
	while (time-- && (readl(SDXC_REG_GCTRL) & SDXC_SoftReset))
		;
	if (time <= 0) {
		SDC_LOGE("SDC reset time out\n");
		return -1;
	}
	SDC_LOGD("%s,%d SDC reset finish \n", __func__, __LINE__);

	return 0;
}

static int32_t __mci_program_clk(void)
{
	uint32_t value;
	int32_t time = 0xf000;
	int32_t ret = 0;

	/* disable command done interrupt */
	writel(readl(SDXC_REG_IMASK) & (~SDXC_CmdDone), SDXC_REG_IMASK);

	value = SDXC_Start | SDXC_UPCLKOnly | SDXC_WaitPreOver;
	writel(value, SDXC_REG_CMDR);

	do {
		value = readl(SDXC_REG_CMDR);
	} while (time-- && (value & SDXC_Start));

	if ((time <= 0) || (value & SDXC_Start)) {
		SDC_LOGE("%s,%d SDC change clock time out\n", __func__, __LINE__);
		ret = -1;
	}

	/* clear command done flag */
	value = readl(SDXC_REG_RINTR);
	writel(value, SDXC_REG_RINTR);

	/* enable command done interrupt */
	writel(readl(SDXC_REG_IMASK) | SDXC_CmdDone, SDXC_REG_IMASK);

	return ret;
}

static void __mci_trans_by_ahb(struct mmc_host *host, struct mmc_data *data)
{
	uint32_t i, j;
	uint32_t *buf_temp; /* Cortex-M3/4 can access data with unaligned address */
	volatile uint32_t time;

	for (i = 0; i < data->sg_len; i++) {
		buf_temp = data->sg[i].buffer;
		if (data->flags & MMC_DATA_READ) {
			for (j = 0; j < (data->sg[i].len >> 2); j++) { /* sg[i].len should be multiply of 4 */
				time = 0xfffff00;
				while ((readl(SDXC_REG_STAS) & SDXC_FIFOEmpty) && time--)
					;
				buf_temp[j] = readl(SDXC_REG_FIFO);
			}
		} else if (data->flags & MMC_DATA_WRITE) {
			for (j = 0; j < (data->sg[i].len >> 2); j++) { /* sg[i].len should be multiply of 4 */
				time = 0xfffff00;
				while ((readl(SDXC_REG_STAS) & SDXC_FIFOFull) && time--)
					;
				if (time <= 5)
					SDC_BUG_ON(1);
				writel(buf_temp[j], SDXC_REG_FIFO);
			}
		} else {
			SDC_LOGW("illigle data request\n");
			SDC_BUG_ON(1);
		}
	}
}

static smc_idma_des _mci_idma_des[SDXC_MAX_DES_NUM] __attribute__ ((aligned (8)));

static smc_idma_des *__mci_alloc_idma_des(struct mmc_data *data)
{
	smc_idma_des *pdes = _mci_idma_des;
	uint32_t des_idx = 0;
	uint32_t buff_frag_num = 0;
	uint32_t remain;
	uint32_t i, j;
	uint32_t config;

	/* init IDMA Descriptor, two mode: 1-fixed skip length, 2-chain mode */
	for (i = 0; i < data->sg_len; i++) {
		buff_frag_num = data->sg[i].len >> SDXC_DES_NUM_SHIFT; /* num = len/8192 = len>>13  */
		remain = data->sg[i].len & (SDXC_DES_BUFFER_MAX_LEN - 1);
		if (remain) {
			buff_frag_num++;
		} else {
			remain = SDXC_DES_BUFFER_MAX_LEN;
		}

		for (j = 0; j < buff_frag_num; j++, des_idx++) {
			HAL_Memset((void *)&pdes[des_idx], 0, sizeof(smc_idma_des));
			config = SDXC_IDMAC_DES0_CH | SDXC_IDMAC_DES0_OWN | SDXC_IDMAC_DES0_DIC;
			if (buff_frag_num > 1 && j != buff_frag_num - 1) {
				pdes[des_idx].data_buf1_sz = SDXC_DES_BUFFER_MAX_LEN;
			} else {
				pdes[des_idx].data_buf1_sz = remain;
			}

			pdes[des_idx].buf_addr_ptr1 = (uint32_t)MEMS_VA2PA(data->sg[i].buffer) + j * SDXC_DES_BUFFER_MAX_LEN;
			if (i == 0 && j == 0) {
				config |= SDXC_IDMAC_DES0_FD;
			}

			if ((i == data->sg_len - 1) && (j == buff_frag_num - 1)) {
				config &= ~SDXC_IDMAC_DES0_DIC;
				config |= SDXC_IDMAC_DES0_LD | SDXC_IDMAC_DES0_ER;
				pdes[des_idx].buf_addr_ptr2 = 0;
			} else {
				pdes[des_idx].buf_addr_ptr2 = (uint32_t)MEMS_VA2PA(&pdes[des_idx + 1]);
			}
			pdes[des_idx].config = config;
			SDC_LOGD("sg %d, frag %d, remain %d, des[%d](%p): [0]:%x, [1]:%x, [2]:%x, [3]:%x\n",
			         i, j, remain, des_idx, &pdes[des_idx],
			         ((uint32_t *)&pdes[des_idx])[0], ((uint32_t *)&pdes[des_idx])[1],
			         ((uint32_t *)&pdes[des_idx])[2], ((uint32_t *)&pdes[des_idx])[3]);
		}
	}

	return pdes;
}

static smc_idma_des *__mci_prepare_dma(struct mmc_host *host, struct mmc_data *data)
{
	uint32_t temp;
	smc_idma_des *pdes = NULL;

	/* creat descriptor list, two mode: 1-fixed skip length, 2-chain mode */
	pdes = __mci_alloc_idma_des(data);
	if (NULL == pdes) {
		SDC_LOGW("alloc IDMA descriptor failed\n");
		return NULL;
	}

	temp = readl(SDXC_REG_GCTRL);
	temp |= SDXC_DMAEnb;
	writel(temp, SDXC_REG_GCTRL);
	temp |= SDXC_DMAReset;
	writel(temp, SDXC_REG_GCTRL);
	writel(SDXC_IDMACSoftRST, SDXC_REG_DMAC); /* reset IDMAC */
	temp = SDXC_IDMACFixBurst | SDXC_IDMACIDMAOn;
	writel(temp, SDXC_REG_DMAC);
	/* enable IDMA interrupt, here not use */
	temp = readl(SDXC_REG_IDIE);
	temp &= ~(SDXC_IDMACReceiveInt | SDXC_IDMACTransmitInt);
	if (data->flags & MMC_DATA_WRITE) {
		temp |= SDXC_IDMACTransmitInt;
	} else {
		temp |= SDXC_IDMACReceiveInt;
	}
	writel(temp, SDXC_REG_IDIE);

	/* write descriptor address to register */
	writel((uint32_t)MEMS_VA2PA(pdes), SDXC_REG_DLBA);
	/* write water level */
	writel((BURST_SIZE << 28) | (SMC_RX_WLEVEL << 16) | SMC_TX_WLEVEL, SDXC_REG_FTRGL);

	return pdes;
}

static void __mci_free_idma_des(smc_idma_des *pdes)
{
	pdes->config &= ~SDXC_IDMAC_DES0_OWN;
}

int32_t __mci_wait_access_done(void)
{
	int32_t own_set = 0;
	int32_t timeout = 0x0f0000;

	while (!(readl(SDXC_REG_GCTRL) & SDXC_MemAccessDone) && timeout--)
		;
	if (!(readl(SDXC_REG_GCTRL) & SDXC_MemAccessDone)) {
		SDC_LOGW("wait memory access done timeout !!\n");
	}

	return own_set;
}

static int32_t __mci_request_done(struct mmc_host *host)
{
	struct mmc_request *mrq = host->mrq;
	unsigned long iflags;
	uint32_t temp;
	int32_t ret = 0;

	iflags = arch_irq_save();
	if (host->wait != SDC_WAIT_FINALIZE) {
		arch_irq_restore(iflags);
		SDC_LOGW("%s nothing finalize, wt %x\n", __func__, host->wait);
		return -1;
	}
	host->wait = SDC_WAIT_NONE;
	host->trans_done = 0;
	host->dma_done = 0;
	arch_irq_restore(iflags);

	if (host->int_sum & SDXC_IntErrBit) {
		SDC_LOGE("SDC err, cmd %d, %s%s%s%s%s%s%s%s%s%s\n",
		         (host->smc_cmd & SDXC_CMD_OPCODE),
		         host->int_sum & SDXC_RespErr    ? " RE" : "",
		         host->int_sum & SDXC_RespCRCErr  ? " RCE" : "",
		         host->int_sum & SDXC_DataCRCErr  ? " DCE" : "",
		         host->int_sum & SDXC_RespTimeout ? " RTO" : "",
		         host->int_sum & SDXC_DataTimeout ? " DTO" : "",
		         host->int_sum & SDXC_DataStarve  ? " DS" : "",
		         host->int_sum & SDXC_FIFORunErr  ? " FRE" : "",
		         host->int_sum & SDXC_HardWLocked ? " HL" : "",
		         host->int_sum & SDXC_StartBitErr ? " SBE" : "",
		         host->int_sum & SDXC_EndBitErr   ? " EBE" : "");
		ret = -1;
		goto out;
	}

	if (mrq->cmd->flags & MMC_RSP_136) {
		mrq->cmd->resp[0] = readl(SDXC_REG_RESP3);
		mrq->cmd->resp[1] = readl(SDXC_REG_RESP2);
		mrq->cmd->resp[2] = readl(SDXC_REG_RESP1);
		mrq->cmd->resp[3] = readl(SDXC_REG_RESP0);
	} else
		mrq->cmd->resp[0] = readl(SDXC_REG_RESP0);

out:
	if (mrq->data) {
		if (host->dma_hdle) {
			__mci_wait_access_done();
			writel(0x337, SDXC_REG_IDST); /* clear interrupt flags */
			writel(0, SDXC_REG_IDIE); /* disable idma interrupt */
			writel(0, SDXC_REG_DMAC); /* idma off */
			temp = readl(SDXC_REG_GCTRL);
			writel(temp | SDXC_DMAReset, SDXC_REG_GCTRL);
			temp &= ~SDXC_DMAEnb;
			writel(temp, SDXC_REG_GCTRL); /* disable IDMA */
			temp |= SDXC_FIFOReset;
			writel(temp, SDXC_REG_GCTRL);
			__mci_free_idma_des((void *)host->dma_hdle);
			host->dma_hdle = NULL;
		}
		writel(readl(SDXC_REG_GCTRL) | SDXC_FIFOReset, SDXC_REG_GCTRL);
	}

#ifdef CONFIG_DETECT_CARD
	if (host->param.cd_mode == CARD_DETECT_BY_D3){
		writel(SDXC_CardInsert|SDXC_CardRemove, SDXC_REG_IMASK);
	} else
#endif
	{
		writel(0, SDXC_REG_IMASK);
	}
	if (host->int_sum & (SDXC_RespErr | SDXC_HardWLocked | SDXC_RespTimeout)) {
		SDC_LOGE("sdc %d abnormal status: %s\n", __LINE__,
		         host->int_sum & SDXC_HardWLocked ? "HardWLocked" : "RespErr");
	}

	writel(0xffff, SDXC_REG_RINTR);

	SDC_LOGD("SDC done, resp %x %x %x %x\n", mrq->cmd->resp[0],
	         mrq->cmd->resp[1], mrq->cmd->resp[2], mrq->cmd->resp[3]);

	if (mrq->data && (host->int_sum & SDXC_IntErrBit)) {
		SDC_LOGW("found data error, need to send stop command !!\n");
		__mci_reset();
		__mci_program_clk();
	}

	return ret;
}

#ifdef CONFIG_SDIO_IRQ_SUPPORT
static void __mci_enable_sdio_irq(struct mmc_host *host, int enable)
{
	uint32_t imask;

	imask = readl(SDXC_REG_IMASK);
	if (enable)
		imask |= SDXC_SDIOInt;
	else
		imask &= ~SDXC_SDIOInt;
	writel(imask, SDXC_REG_IMASK);
}

static inline void __mci_signal_sdio_irq(struct mmc_host *host)
{
	SDC_BUG_ON(!host->card || !host->card->irq_handler);

	__mmc_enable_sdio_irq(host, 0);
	host->card->irq_handler(host->card);
	__mmc_enable_sdio_irq(host, 1);
}
#endif

static void __mci_clk_prepare_enable(void)
{
	SDC_CCM_BusEnableClock(); /* clock enable */
	SDC_CCM_BusReleaseRest(); /* reset and gating */
	SDC_CCM_EnableMClock();
}

static void __mci_clk_disable_unprepare(void)
{
	SDC_CCM_BusDisableClock();
}

static void __mci_hold_io(struct mmc_host* host)
{
#ifdef __CONFIG_ARCH_APP_CORE
	/* disable gpio to avoid power leakage */
	HAL_BoardIoctl(HAL_BIR_PINMUX_DEINIT, HAL_MKDEV(HAL_DEV_MAJOR_SDC, host->sdc_id), SDCGPIO_BAS);
#endif
}

static void __mci_restore_io(struct mmc_host* host)
{
#ifdef __CONFIG_ARCH_APP_CORE
	HAL_BoardIoctl(HAL_BIR_PINMUX_INIT, HAL_MKDEV(HAL_DEV_MAJOR_SDC, host->sdc_id), SDCGPIO_BAS);
#endif
}

static void __mci_irq_handler(void)
{
	struct mmc_host *host = _mci_host;
	uint32_t sdio_int = 0;
	uint32_t raw_int;
	uint32_t msk_int;
	uint32_t idma_inte;
	uint32_t idma_int;

	if (!host->present) {
		SDC_CCM_BusEnableClock();
	}

	idma_int = readl(SDXC_REG_IDST);
	idma_inte = readl(SDXC_REG_IDIE);
	raw_int = readl(SDXC_REG_RINTR);
	msk_int = readl(SDXC_REG_MISTA);

	if (!msk_int && !idma_int) {
		SDC_LOGE("sdc nop irq: ri:%08x mi:%08x ie:%08x idi:%08x\n",
		         raw_int, msk_int, idma_inte, idma_int);
		return ;
	}

	host->int_sum = raw_int;
	SDC_LOGD("smc %d ri:%02x(%02x), mi:%x, ie:%x, idi:%x\n", __LINE__,
	         (int)raw_int, (int)host->int_sum,
	         (int)msk_int, (int)idma_inte, (int)idma_int);

	(void)idma_inte;

#ifdef CONFIG_SDIO_IRQ_SUPPORT
	if (msk_int & SDXC_SDIOInt) {
		sdio_int = 1;
		SDC_LOGE("%s,%d unsupport sdio int now!\n", __func__, __LINE__);
		writel(SDXC_SDIOInt, SDXC_REG_RINTR);
		goto sdio_out;
	}
#endif

#ifdef CONFIG_DETECT_CARD
	if (host->param.cd_mode == CARD_DETECT_BY_D3) {
		if (msk_int & SDXC_CardInsert) {
			SDC_LOGN("Card Insert !!\n");
			host->present = 1;
			SDC_ModTimer(&host->cd_timer, 10);
		} else if (msk_int & SDXC_CardRemove) {
			SDC_LOGN("Card Remove !!\n");
			host->present = 0;
			SDC_ModTimer(&host->cd_timer, 10);
		}
	}
#endif

	if (host->wait == SDC_WAIT_NONE && !sdio_int) {
		SDC_LOGE("%s nothing to complete, ri:%08x, mi:%08x\n",
		         __func__, raw_int, msk_int);
		goto irq_out;
	}

	if ((raw_int & SDXC_IntErrBit) || (idma_int & SDXC_IDMA_ERR)) {
		host->int_err = raw_int & SDXC_IntErrBit;
		host->wait = SDC_WAIT_FINALIZE;
		SDC_LOGE("%s,%d raw_int:%x err!\n", __func__, __LINE__, raw_int);
		goto irq_out;
	}
	if (raw_int & SDXC_HardWLocked) {
		SDC_LOGE("command hardware lock\n");
	}

	if (idma_int & (SDXC_IDMACTransmitInt|SDXC_IDMACReceiveInt)) {
		host->dma_done = 1;
		writel(idma_int, SDXC_REG_IDST);
	}
	if (msk_int & (SDXC_AutoCMDDone|SDXC_DataOver|SDXC_CmdDone|SDXC_VolChgDone))
		host->trans_done = 1;
	if ((host->trans_done && \
	    (host->wait == SDC_WAIT_AUTOCMD_DONE || host->wait == SDC_WAIT_DATA_OVER
	     || host->wait == SDC_WAIT_CMD_DONE || host->wait == SDC_WAIT_SWITCH1V8))
	     || (host->trans_done && host->dma_done && (host->wait & SDC_WAIT_IDMA_DONE))) {
		host->wait = SDC_WAIT_FINALIZE;
	}

irq_out:
	writel(msk_int & (~SDXC_SDIOInt), SDXC_REG_RINTR);

	if (host->wait == SDC_WAIT_FINALIZE) {
#ifdef CONFIG_DETECT_CARD
		if (host->param.cd_mode == CARD_DETECT_BY_D3){
			writel(SDXC_CardInsert|SDXC_CardRemove, SDXC_REG_IMASK);
		} else
#endif
		{
			writel(0, SDXC_REG_IMASK);
		}
		SDC_SemPost(&host->lock);
		SDC_LOGD("SDC irq post, trans:%d, dma:%d\n", (int)host->trans_done, (int)host->dma_done);
	}

#ifdef CONFIG_SDIO_IRQ_SUPPORT
sdio_out:
	if (sdio_int)
		__mci_signal_sdio_irq(host);
#endif
}

/* clock config:
 * SYS_CRYSTAL: 26M
 * 400K, SCLK-26M, 26M/(2^3)/(7+1)/2 = 361K
 * SYS_CRYSTAL: 24M
 * 400K, SCLK-24M, 24M/(2^2)/(7+1)/2 = 375K
 *
 * 25M, SCLK-192M, 192M/(2^0)/(3+1)/2 = 24M
 * 50M, SCLK-192M, 192M/(2^0)/(1+1)/2 = 48M
 * 64M, SCLK-192M, 192M/(2^0)/(1+1)/2 = 48M
 */
#define DEFINE_SYS_CRYSTAL  HAL_GetHFClock()
#define DEFINE_SYS_DEVCLK   HAL_GetDevClock()

static int32_t __mci_update_clock(uint32_t cclk)
{
	uint32_t sclk;
	uint32_t div;
	uint32_t rval;
	uint32_t src = 0;
	uint32_t m, n;

	if (cclk > DEFINE_SYS_CRYSTAL/2) {
#ifdef NUSE_STANDARD_INTERFACE
		src = 1;
#else
		src = CCM_AHB_PERIPH_CLK_SRC_DEVCLK;
#endif
		sclk = DEFINE_SYS_DEVCLK;
	} else {
#ifdef NUSE_STANDARD_INTERFACE
		src = 0;
#else
		src = CCM_AHB_PERIPH_CLK_SRC_HFCLK;
#endif
		sclk = DEFINE_SYS_CRYSTAL;
	}
	cclk = cclk * 2; /* 2x MODE clock configure */
	div = (2 * sclk + cclk) / (2 * cclk);
	div = div == 0 ? 1 : div;
	if (div > 128) {
		n = 3;
		m = 16;
		SDC_LOGE("source clk is too high!\n");
	} else if (div > 64) {
		n = 3;
		m = div >> 3;
	} else if (div > 32) {
		n = 2;
		m = div >> 2;
	} else if (div > 16) {
		n = 1;
		m = div >> 1;
	} else {
		n = 0;
		m = div;
	}
	 m = m - 1;
#ifdef NUSE_STANDARD_INTERFACE
	rval = (1U << 31) | (src << 24) | (n << 16) | (m);
	if (src) {
		rval |= (2U << 20) | (1 << 8);
		SDC_LOGN("%s,%d check spec. bit20, 8!!\n", __func__, __LINE__);
	}
	writel(rval, SDC_CCM_SDC0_SCLK_CTRL);
	SDC_LOGN("SDC clock=%d kHz,src:%x, n:%d, m:%d\n",
		 (src?DEFINE_SYS_DEVCLK:DEFINE_SYS_CRYSTAL)/(1<<n)/(m+1)/2,
		 (int)src, (int)n, (int)m);
#else
	HAL_CCM_SDC_DisableMClock();
	HAL_CCM_SDC_SetMClock(src, n << CCM_PERIPH_CLK_DIV_N_SHIFT, m << CCM_PERIPH_CLK_DIV_M_SHIFT);
	SDC_CCM_EnableMClock();
	SDC_LOGN("SDC source:%d MHz clock=%d kHz,src:%x, n:%d, m:%d\n", sclk/1000000,
		 sclk/(1<<n)/(m+1)/2000, (int)src, (int)n, (int)m);
#endif
	/* clear internal divider */
	rval = readl(SDXC_REG_CLKCR) & (~0xff);
	writel(rval, SDXC_REG_CLKCR);

	return cclk;
}

int32_t HAL_SDC_Update_Clk(struct mmc_host *host, uint32_t clk)
{
	uint32_t rval;

	SDC_BUG_ON(!host);
	SDC_BUG_ON(clk < 200000); /* 200K */

	/* Disable Clock */
	rval = readl(SDXC_REG_CLKCR) & (~SDXC_CardClkOn) & (~SDXC_LowPowerOn);
	writel(rval, SDXC_REG_CLKCR);
	if (__mci_program_clk()) {
		SDC_LOGE("Clock Program Failed 0!!\n");
		return -1;
	}

	__mci_update_clock(clk);

	/* Re-enable Clock */
	rval = readl(SDXC_REG_CLKCR) | (SDXC_CardClkOn);// | SDXC_LowPowerOn;
	writel(rval, SDXC_REG_CLKCR);
	if (__mci_program_clk()) {
		SDC_LOGE("Clock Program Failed 1!!\n");
		return -1;
	}

	mmc_mdelay(5);

	host->clk = clk;

	return 0;
}

static int32_t __mci_update_clk(struct mmc_host *host)
{
	uint32_t rval;
	int32_t timeout = 0xf000;
	int32_t ret = 0;

	rval = SDXC_Start|SDXC_UPCLKOnly|SDXC_WaitPreOver;
#ifdef CONFIG_SDC_SUPPORT_1V8
	if (host->voltage_switching)
		rval |= SDXC_VolSwitch;
#endif
	writel(rval, SDXC_REG_CMDR);

	do {
		rval = readl(SDXC_REG_CMDR);
	} while (timeout-- && (rval & SDXC_Start));

	if (rval & SDXC_Start) {
		SDC_LOGE("sdc update clock timeout, fatal error!!!\n");
		ret = -1;
	}

	if (!ret)
		SDC_LOGD("sdc update clock ok\n");

	return ret;
}

int32_t HAL_SDC_Clk_PWR_Opt(struct mmc_host *host, uint32_t oclk_en, uint32_t pwr_save)
{
	uint32_t rval;

	if (!host->power_on)
		return 0;

	rval = readl(SDXC_REG_CLKCR);

	rval &= ~(SDXC_CardClkOn | SDXC_LowPowerOn);
	if (oclk_en)
		rval |= SDXC_CardClkOn;
	if (pwr_save)
		rval |= SDXC_LowPowerOn;
	writel(rval, SDXC_REG_CLKCR);

	__mci_update_clk(host);

	return 0;
}

static void __mci_debounce_onoff(uint32_t onoff)
{
	uint32_t rval = readl(SDXC_REG_GCTRL);

	rval &= ~SDXC_DebounceEnb;
	if (onoff)
		rval |= SDXC_DebounceEnb;
	writel(rval, SDXC_REG_GCTRL);
}

uint32_t HAL_SDC_Is_Busy(struct mmc_host *host)
{
	return readl(SDXC_REG_STAS) & SDXC_CardBusy;
}

void HAL_SDC_Set_BusWidth(struct mmc_host *host, uint32_t width)
{
	SDC_BUG_ON(!host);

	switch (width) {
	case MMC_BUS_WIDTH_1:
		writel(SDXC_WIDTH1, SDXC_REG_WIDTH);
		break;
	case MMC_BUS_WIDTH_4:
		writel(SDXC_WIDTH4, SDXC_REG_WIDTH);
		break;
#ifdef CONFIG_USE_MMC
	case MMC_BUS_WIDTH_8:
		writel(SDXC_WIDTH8, SDXC_REG_WIDTH);
		break;
#endif
	default:
		SDC_BUG_ON(1);
	}

	host->buswidth = width;
}

static void __mci_send_cmd(struct mmc_host *host, struct mmc_command *cmd)
{
	uint32_t imask = SDXC_IntErrBit;
	uint32_t cmd_val = SDXC_Start | (cmd->opcode & 0x3f);
	unsigned long iflags;
	uint32_t wait = SDC_WAIT_CMD_DONE;
	struct mmc_data *data = cmd->data;

	if (cmd->opcode == MMC_GO_IDLE_STATE) {
		cmd_val |= SDXC_SendInitSeq;
		imask |= SDXC_CmdDone;
	}

#ifdef CONFIG_SDC_SUPPORT_1V8
	if (cmd->opcode == SD_SWITCH_VOLTAGE) {
		cmd_val |= SDXC_VolSwitch;
		imask |= SDXC_VolChgDone;
		host->voltage_switching = 1;
		wait = SDC_WAIT_SWITCH1V8;
		/* switch controller to high power mode */
		HAL_SDC_Clk_PWR_Opt(host, 1, 0);
	}
#endif

	if (cmd->flags & MMC_RSP_PRESENT) { /* with response */
		cmd_val |= SDXC_RspExp;
		if (cmd->flags & MMC_RSP_136) /* long response */
			cmd_val |= SDXC_LongRsp;
		if (cmd->flags & MMC_RSP_CRC)
			cmd_val |= SDXC_CheckRspCRC;

		if (mmc_cmd_type(cmd) == MMC_CMD_ADTC) { /* with data */
			SDC_BUG_ON(!data);
			cmd_val |= SDXC_DataExp | SDXC_WaitPreOver;
			wait = SDC_WAIT_DATA_OVER;
			if (data->flags & MMC_DATA_STREAM) { /* sequence mode */
				imask |= SDXC_AutoCMDDone;
				cmd_val |= SDXC_Seqmod | SDXC_SendAutoStop;
				wait = SDC_WAIT_AUTOCMD_DONE;
			}
			if (cmd->stop) {
				imask |= SDXC_AutoCMDDone;
				cmd_val |= SDXC_SendAutoStop;
				wait = SDC_WAIT_AUTOCMD_DONE;
			} else
				imask |= SDXC_DataOver;

			if (data->flags & MMC_DATA_WRITE) {
				cmd_val |= SDXC_Write;
			} else if (host->dma_hdle) {
				wait |= SDC_WAIT_IDMA_DONE;
			}
			SDC_LOGD("blk_size:%d, sg len:%d\n", data->blksz, data->sg->len);
		} else
			imask |= SDXC_CmdDone;
	} else
		imask |= SDXC_CmdDone;

	SDC_LOGD("smc cmd:%d(%x), arg:%x ie:%x wt:%x len:%d\n",
	         (cmd_val & SDXC_CMD_OPCODE), cmd_val, cmd->arg, imask, wait,
	         cmd->data ? cmd->data->blksz * cmd->data->blocks : 0);

	iflags = arch_irq_save();
	host->smc_cmd = cmd_val;
	host->wait = wait;

	writel(imask, SDXC_REG_IMASK);

	if (cmd_val & SDXC_SendAutoStop)
		writel(0, SDXC_REG_A12A);
	else
		writel(0xffff, SDXC_REG_A12A);

	writel(cmd->arg, SDXC_REG_CARG);
	writel(cmd_val, SDXC_REG_CMDR);

	arch_irq_restore(iflags);

	if (data && NULL == host->dma_hdle) {
		__mci_trans_by_ahb(host, data);
	}
}

int32_t HAL_SDC_Request(struct mmc_host *host, struct mmc_request *mrq)
{
	int32_t ret = 0;
	struct mmc_command *cmd = mrq->cmd;
	struct mmc_data *data = mrq->data;
#ifdef CONFIG_SDC_DMA_USED
	uint32_t sdc_use_dma = 1;
#else
	uint32_t sdc_use_dma = 0;
#endif

	if (!host->present) {
		SDC_LOGW("sdc no medium present\n");
		return -1;
	}

#ifdef CONFIG_SD_PM
	if (host->suspend) {
		SDC_LOGE("sdc has suspended!\n");
		return -1;
	}
#endif

	OS_MutexLock(&host->thread_lock, SDC_THREAD_TIMEOUT);

	/* disable debounce */
	__mci_debounce_onoff(0);

	host->mrq = mrq;

	if (data) {
		uint32_t i;
		uint32_t byte_cnt = 0;
		struct scatterlist *sg = data->sg;
		uint32_t sg_len = data->sg_len;

		for (i = 0; i < sg_len; i++) {
			byte_cnt += sg[i].len;
#ifdef CONFIG_SDC_DMA_USED
			if (((uint32_t)sg[i].buffer & 0x03)) {
				sdc_use_dma = 0;
				break;
			}
#endif
		}
#ifdef CONFIG_SDC_DMA_USED
		if (byte_cnt <= SDC_MAX_CPU_TRANS_LEN) {
			sdc_use_dma = 0;
		}
#endif

		writel(data->blksz, SDXC_REG_BLKSZ);
		writel(byte_cnt, SDXC_REG_BCNTR);

		if (sdc_use_dma && byte_cnt > SDC_MAX_CPU_TRANS_LEN) { /* transfer by idma */
			__mci_sel_access_mode(SDXC_ACCESS_BY_DMA);
			host->dma_hdle = __mci_prepare_dma(host, data);
			if (NULL == host->dma_hdle) {
				SDC_LOGW("SDC prepare DMA failed\n");
				__mci_sel_access_mode(SDXC_ACCESS_BY_AHB);
			}
		} else {
			/* switch data bus to ahb */
			__mci_sel_access_mode(SDXC_ACCESS_BY_AHB);
		}
	}

	__mci_send_cmd(host, cmd);

	ret = SDC_SemPend(&host->lock, SDC_DMA_TIMEOUT);
	if (ret != HAL_OK){
		SDC_LOGE("sdc cmd:%d, wait command done timeout !!\n",
		         (host->smc_cmd & SDXC_CMD_OPCODE));
		goto out;
	}

	ret = __mci_request_done(host);
	if (ret) {
		SDC_LOGE("int err %x\n", host->int_err);
		goto out;
	}

out:
	/* enable debounce */
	__mci_debounce_onoff(1);

	SDC_MutexUnlock(&host->thread_lock);

	return ret;
}

#ifdef CONFIG_SDC_EXCLUSIVE_HOST
/**
 *	HAL_SDC_Claim_Host - exclusively claim a host
 *	@host: mmc host to claim
 *
 *	Claim a host for a set of operations.
 */
int32_t HAL_SDC_Claim_Host(struct mmc_host *host)
{
	return (SDC_SemPend(&host->exclusive_lock, OS_WAIT_FOREVER) == HAL_OK ? 0 : -1);
}

/**
 *	HAL_SDC_Release_Host - release a host
 *	@host: mmc host to release
 *
 *	Release a MMC host, allowing others to claim the host
 *	for their operations.
 */
void HAL_SDC_Release_Host(struct mmc_host *host)
{
	SDC_SemPost(&host->exclusive_lock);
}
#endif

#ifdef CONFIG_SDC_READONLY_USED
int32_t HAL_SDC_Get_ReadOnly(struct mmc_host *host)
{
	uint32_t wp_val;
	GPIO_PinMuxParam *ro_gpio = &host->ro_gpio;

	wp_val = = (GPIO_PIN_HIGH == HAL_GPIO_ReadPin(ro_gpio->port, ro_gpio->pin)) ? 1 : 0;
	SDC_LOGN("sdc fetch card wp pin status: %d \n", wp_val);

	if (!wp_val) {
		host->read_only = 0;
		return 0;
	} else {
		SDC_LOGN("Card is write-protected\n");
		host->read_only = 1;
		return 1;
	}

	return 0;
}
#endif

#ifdef CONFIG_DETECT_CARD
static void __mci_cd_timer(void *arg)
{
	struct mmc_host *host = (struct mmc_host *)arg;
	uint32_t gpio_val = 0;
	uint32_t present;

	gpio_val = (GPIO_PIN_HIGH == HAL_GPIO_ReadPin(host->cd_port, host->cd_pin)) ? 1 : 0;

	if (gpio_val) {
		present = 0;
	} else if (!gpio_val)
		present = 1;

	SDC_LOGD("cd %d, host present %d, cur present %d\n", gpio_val, host->present, present);

	if (host->present ^ present) {
		SDC_LOGD("sdc detect change, present %d\n", present);
		host->present = present;
		host->param.cd_cb(present);
	}

	return ;
}

static void __mci_dat3_det(void *arg)
{
	struct mmc_host *host = (struct mmc_host *)arg;

	SDC_LOGD("***dat3 det***\n");

	host->param.cd_cb(host->present);
}

static void __mci_cd_irq(void *arg)
{
	struct mmc_host *host = (struct mmc_host *)arg;

	SDC_LOGD("***in cd***\n");

	SDC_ModTimer(&host->cd_timer, 10);

	return ;
}
#endif

int32_t HAL_SDC_PowerOn(struct mmc_host *host)
{
	uint32_t rval;

	SDC_BUG_ON(!host);

	SDC_LOGD("MMC Driver init host\n");

	__mci_restore_io(host);
	__mci_clk_prepare_enable();

	/* delay 1ms ? */
	__mci_update_clock(400000);
	host->clk = 400000;

	/* reset controller */
	rval = readl(SDXC_REG_GCTRL)|SDXC_HWReset|SDXC_INTEnb|SDXC_AccessDoneDirect;
	writel(rval, SDXC_REG_GCTRL);

	writel(0xffffffff, SDXC_REG_RINTR);

#ifdef CONFIG_DETECT_CARD
	if (host->param.cd_mode == CARD_DETECT_BY_D3){
		writel(SDXC_CardInsert|SDXC_CardRemove, SDXC_REG_IMASK);
	} else
#endif
	{
		writel(0, SDXC_REG_IMASK);
	}

#define SDMC_DATA_TIMEOUT  0x0ffffffU
#define SDMC_RESP_TIMEOUT  0xffU
	/* Set Data & Response Timeout Value */
	writel((SDMC_DATA_TIMEOUT<<8)|SDMC_RESP_TIMEOUT, SDXC_REG_TMOUT);
#undef SDMC_RESP_TIMEOUT
#undef SDMC_DATA_TIMEOUT

	HAL_SDC_Set_BusWidth(host, MMC_BUS_WIDTH_1);

#ifdef SDC_DEBUG
	writel(0xdeb, SDXC_REG_DBGC);
	writel(0xceaa0000, SDXC_REG_FUNS);
#endif

#ifdef CONFIG_DETECT_CARD
	if (host->param.cd_mode == CARD_DETECT_BY_D3){
		rval |= SDXC_DebounceEnb;
	}
	writel(rval, SDXC_REG_GCTRL);
#endif

	SDC_ENABLE_IRQ(SDC_IRQn);

	host->power_on = 1;

	return 0;
}

int32_t HAL_SDC_PowerOff(struct mmc_host *host)
{
	SDC_BUG_ON(!host);

#ifdef CONFIG_DETECT_CARD
	if (host->param.cd_mode != CARD_DETECT_BY_D3)
#endif
	{
		uint32_t rval;

		SDC_DISABLE_IRQ(SDC_IRQn);

		rval = readl(SDXC_REG_GCTRL) | SDXC_HWReset;
		writel(rval, SDXC_REG_GCTRL);
		rval = readl(SDXC_REG_MISTA);
		writel(rval & (~SDXC_SDIOInt), SDXC_REG_RINTR);

		SDC_CLEAR_IRQPINGD(SDC_IRQn);

		__mci_clk_disable_unprepare();

		__mci_hold_io(host);
		host->power_on = 0;
	}

	return 0;
}

static struct mmc_host _mci_host_rel;

#ifdef CONFIG_SD_PM

struct __mci_ctrl_regs {
	uint32_t gctrl;
	uint32_t clkc;
	uint32_t timeout;
	uint32_t buswid;
	uint32_t waterlvl;
	uint32_t funcsel;
	uint32_t idmacc;
};

static struct __mci_ctrl_regs _mci_pm_bak_regs;
#ifdef __CONFIG_ARCH_APP_CORE
static SDC_InitTypeDef g_sdc_param;
#endif

static void __mci_regs_save(struct mmc_host *host)
{
	struct __mci_ctrl_regs* bak_regs = &_mci_pm_bak_regs;

	bak_regs->gctrl = readl(SDXC_REG_GCTRL);
	bak_regs->clkc = readl(SDXC_REG_CLKCR);
	bak_regs->timeout = readl(SDXC_REG_TMOUT);
	bak_regs->buswid = readl(SDXC_REG_WIDTH);
	bak_regs->waterlvl = readl(SDXC_REG_FTRGL);
	bak_regs->funcsel = readl(SDXC_REG_FUNS);
	bak_regs->idmacc = readl(SDXC_REG_DMAC);
}

static void __mci_regs_restore(struct mmc_host *host)
{
	struct __mci_ctrl_regs* bak_regs = &_mci_pm_bak_regs;

	writel(bak_regs->gctrl, SDXC_REG_GCTRL);
	writel(bak_regs->clkc, SDXC_REG_CLKCR);
	writel(bak_regs->timeout, SDXC_REG_TMOUT);
	writel(bak_regs->buswid, SDXC_REG_WIDTH);
	writel(bak_regs->waterlvl, SDXC_REG_FTRGL);
	writel(bak_regs->funcsel, SDXC_REG_FUNS);
#ifdef SDC_DEBUG
	writel(0xdeb, SDXC_REG_DBGC);
	writel(0xceaa0000, SDXC_REG_FUNS);
#endif
	writel(bak_regs->idmacc, SDXC_REG_DMAC);
}

static int __mci_suspend(struct soc_device *dev, enum suspend_state_t state)
{
	int ret = 0;
	struct mmc_host *host = _mci_host;

#ifdef CONFIG_DETECT_CARD
	if (cancel_delayed_work(&host->detect))
		wake_unlock(&host->detect_wake_lock);
#endif
	if (host->bus_ops && host->bus_ops->suspend)
		ret = host->bus_ops->suspend(host);

	host->suspend = 1;
	__mci_regs_save(host);

	/* gate clock for lower power */
	HAL_CCM_SDC_DisableMClock();
	SDC_CCM_BusForceReset();
	__mci_clk_disable_unprepare();

#ifdef CONFIG_DETECT_CARD
	if (host->cd_mode == CARD_DETECT_BY_D3) {
		__mci_exit_host(host);

		HAL_CCM_SDC_DisableMClock();
		SDC_CCM_BusForceReset();
		__mci_hold_io(host);

		SDC_LOGD("sdc card_power_off ok\n");

		//__mci_set_vddio(host, SDC_WOLTAGE_OFF);
		//usleep_range(1000, 1500);
		host->power_on = 0;

	}
#endif

	SDC_LOGD("%s okay\n", __func__);

	return ret;
}

static int __mci_resume(struct soc_device *dev, enum suspend_state_t state)
{
	int ret = 0;
	struct mmc_host *host = _mci_host;
	uint32_t clk = host->clk;

	__mci_restore_io(host);

	SDC_CCM_BusForceReset();
	__mci_clk_disable_unprepare();
	mmc_udelay(35);

	/* enable clock for resotre */
	__mci_clk_prepare_enable();
	mmc_udelay(50);
	__mci_update_clock(clk);
	mmc_udelay(100);

	__mci_regs_restore(host);
#ifdef CONFIG_DETECT_CARD
#else
	writel(SDXC_CardInsert, SDXC_REG_RINTR);
#endif
	__mci_update_clk(host);

	/* register IRQ */
	SDC_REQUEST_IRQ(SDC_IRQn, (SDC_IRQHandler)&__mci_irq_handler);
	SDC_SetPriority(SDC_IRQn, NVIC_PERIPHERAL_PRIORITY_DEFAULT);
	SDC_ENABLE_IRQ(SDC_IRQn);
	mmc_udelay(100);

#ifdef CONFIG_DETECT_CARD
	if (host->cd_mode == CARD_DETECT_BY_GPIO_IRQ)
		__mci_cd_timer(host);

	if (host->cd_mode == CARD_DETECT_BY_D3) {
		uint32_t rval = 0;

		//__mci_set_vddio(host, host->regulator_voltage);
		//usleep_range(1000, 1500);
		host->power_on = 1;

		__mci_restore_io(host);
		__mci_clk_prepare_enable();

		mmc_mdelay(1);
		rval = readl(SDXC_REG_RINTR);
		SDC_LOGD(">> REG_RINTR=0x%x\n", rval);
	}
#endif

	host->suspend = 0;

	if (host->bus_ops && host->bus_ops->resume)
		ret = host->bus_ops->resume(host);

	SDC_LOGD("sdc resume okay\n");

	return ret;
}

static struct soc_device_driver sdc_drv = {
	.name = "sdc",
	.suspend = __mci_suspend,
	.resume = __mci_resume,
};

static struct soc_device sdc_dev = {
	.name = "sdc",
	.driver = &sdc_drv,
	.platform_data = (void *)&_mci_host_rel,
};

#define SDC_DEV (&sdc_dev)

#else /* CONFIG_SD_PM */

#define SDC_DEV NULL

#endif /* CONFIG_SD_PM */

/**
 * @brief Initializes the SDC peripheral.
 * @param sdc_id:
 *        @arg sdc_id->SDC ID.
 * @param param:
 *        @arg param->[in] The configuration information.
 * @retval  SDC handler.
 */
#ifdef __CONFIG_ARCH_APP_CORE
struct mmc_host *HAL_SDC_Init(uint32_t sdc_id, SDC_InitTypeDef *param)
#else
/**
 * @brief Initializes the SDC peripheral.
 * @param sdc_id:
 *        @arg sdc_id->SDC ID.
 * @retval  SDC handler.
 */
struct mmc_host *HAL_SDC_Init(uint32_t sdc_id)
#endif
{
	struct mmc_host *host;
#ifdef __CONFIG_ARCH_APP_CORE
	const HAL_SDCGPIOCfg *sd_gpio_cfg;
#endif

	host = &_mci_host_rel;
	_mci_host = host;

	if (host->State != SDC_STATE_RESET) {
		SDC_LOGW("%s reinit sdc!\n", __func__);
		return NULL;
	}

	host->sdc_id = sdc_id;
#ifdef __CONFIG_ARCH_APP_CORE
	memcpy(&host->param, param, sizeof(SDC_InitTypeDef));
#endif

	host->caps = MMC_CAP_MMC_HIGHSPEED | MMC_CAP_SD_HIGHSPEED | MMC_CAP_WAIT_WHILE_BUSY |
	             MMC_CAP_UHS_SDR12 | MMC_CAP_UHS_SDR25 | MMC_CAP_UHS_SDR50;
#ifdef CONFIG_SD_PM
	host->pm_caps = MMC_PM_KEEP_POWER|MMC_PM_WAKE_SDIO_IRQ;
#endif

#ifdef __CONFIG_ARCH_APP_CORE
	HAL_BoardIoctl(HAL_BIR_GET_CFG, HAL_MKDEV(HAL_DEV_MAJOR_SDC, host->sdc_id),
	               (uint32_t)&sd_gpio_cfg);
	if (sd_gpio_cfg->data_bits == 8)
		host->caps |= MMC_CAP_8_BIT_DATA;
	else if (sd_gpio_cfg->data_bits == 4)
		host->caps |= MMC_CAP_4_BIT_DATA;
#else
	host->caps |= MMC_CAP_4_BIT_DATA;
#endif

	__mci_restore_io(host);

	/* register IRQ */
	SDC_REQUEST_IRQ(SDC_IRQn, (SDC_IRQHandler)&__mci_irq_handler);
	SDC_SetPriority(SDC_IRQn, NVIC_PERIPHERAL_PRIORITY_DEFAULT);

#ifdef CONFIG_DETECT_CARD
	if (host->param.cd_mode == CARD_ALWAYS_PRESENT) {
		host->present = 1;
	} else if (host->param.cd_mode == CARD_DETECT_BY_GPIO_IRQ) {
		SDC_BUG_ON(!param->cd_cb);
		SDC_BUG_ON(!sd_gpio_cfg->has_detect_gpio);
		GPIO_IrqParam Irq_param;

		host->cd_port = sd_gpio_cfg->detect_port;
		host->cd_pin = sd_gpio_cfg->detect_pin;
		HAL_BoardIoctl(HAL_BIR_PINMUX_INIT, HAL_MKDEV(HAL_DEV_MAJOR_SDC, host->sdc_id), SDCGPIO_DET);

		SDC_InitTimer(&host->cd_timer, &__mci_cd_timer, host, 300);

		//HAL_GPIO_SetDebounce(&Irq_param, (2U << 4) | 1); /* set debounce clock */
		Irq_param.event = GPIO_IRQ_EVT_BOTH_EDGE;
		Irq_param.callback = &__mci_cd_irq;
		Irq_param.arg = host;
		HAL_GPIO_EnableIRQ(host->cd_port, host->cd_pin, &Irq_param);
		host->present = !((GPIO_PIN_HIGH ==
		                   HAL_GPIO_ReadPin(host->cd_port, host->cd_pin)) ? 1 : 0);
	} else if (host->param.cd_mode == CARD_DETECT_BY_D3) {
		uint32_t rval;
		SDC_BUG_ON(!param->cd_cb);

		__mci_clk_prepare_enable();
		mmc_mdelay(1);
		host->present = 1;
		rval = readl(SDXC_REG_RINTR);
		SDC_LOGD("sdc +> REG_RINTR=0x%x\n", rval);
		if ((rval & SDXC_CardRemove)) {
			SDC_LOGD("sdc data[3] detect Card Remove\n");
			host->present = 0;
		}
		SDC_InitTimer(&host->cd_timer, &__mci_dat3_det, host, 300);
	}
#else
	host->present = 1;
#endif

	host->max_blk_count = 8192;
	host->max_blk_size = 4096;
	host->max_req_size = host->max_blk_size * host->max_blk_count;
	host->max_seg_size = host->max_req_size;
	host->max_segs = 128;
	host->ocr_avail = MMC_VDD_28_29 | MMC_VDD_29_30 | MMC_VDD_30_31 | MMC_VDD_31_32
	                      | MMC_VDD_32_33 | MMC_VDD_33_34;

	SDC_LOGD("SDC Host Capability:0x%x Ocr avail:0x%x\n", host->caps, host->ocr_avail);

	/* init semaphore */
	SDC_SemCreate(&host->lock, 0);
	SDC_MutexCreate(&host->thread_lock);
#ifdef CONFIG_SDC_EXCLUSIVE_HOST
	SDC_SemCreate(&host->exclusive_lock, 1);
#endif

#ifdef CONFIG_DETECT_CARD
	if (host->param.cd_mode == CARD_DETECT_BY_D3 && host->present == 0) {
		SDC_LOGD("sdc power init.\n");
		HAL_SDC_PowerOn(host);
	} else if (host->present == 0) {
		/* if card is not present and the card detect mode is not CARD_DETECT_BY_D3,
		 * we shutdown io voltage to save power. */
		SDC_LOGD("sdc no card detected, shutdown io voltage.\n");
		__mci_hold_io(host);
		//__mci_set_vddio(host, SDC_VOLTAGE_OFF);
	}
#endif

#ifdef CONFIG_SD_PM
#ifdef __CONFIG_ARCH_APP_CORE
	memcpy(&g_sdc_param, param, sizeof(SDC_InitTypeDef));
#endif
	pm_register_ops(SDC_DEV);
#endif

	host->State = SDC_STATE_READY;
	SDC_LOGD("sdc init ok.\n");

	return host;
}

/**
 * @brief DeInitializes the SDC peripheral.
 * @param sdc_id:
 *        @arg sdc_id-> SDC ID.
 * @retval  None.
 */
int32_t HAL_SDC_Deinit(uint32_t sdc_id)
{
	struct mmc_host *host = _mci_host;
#ifdef CONFIG_DETECT_CARD
	host->param.cd_mode = 0;
#endif
	SDC_BUG_ON(!host);

#ifdef CONFIG_SD_PM
	pm_unregister_ops(SDC_DEV);
#endif

	__mci_exit_host(host);
	HAL_CCM_SDC_DisableMClock();
	SDC_CCM_BusForceReset();

#ifdef CONFIG_DETECT_CARD
	HAL_BoardIoctl(HAL_BIR_PINMUX_DEINIT, HAL_MKDEV(HAL_DEV_MAJOR_SDC, host->sdc_id), SDCGPIO_DET);
#endif

#ifdef CONFIG_SDC_EXCLUSIVE_HOST
	SDC_SemDel(&host->exclusive_lock);
#endif
	SDC_MutexDelete(&host->thread_lock);
	SDC_SemDel(&host->lock);

#ifdef CONFIG_DETECT_CARD
	if ((host->param.cd_mode == CARD_DETECT_BY_GPIO_IRQ)||
	    (host->param.cd_mode == CARD_DETECT_BY_D3)) {
		SDC_DelTimer(&host->cd_timer);
	}
#endif

	memset((void *)_mci_host, 0, sizeof(struct mmc_host)); /* SDC_STATE_RESET */
	_mci_host = NULL;
	SDC_LOGD("sdc deinit ok.\n");

	return 0;
}
