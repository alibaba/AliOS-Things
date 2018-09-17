#include <k_api.h>
#include <board.h>
#include <assert.h>
#include "arasan.h"
#include "sdio/mmcsd_host.h"
#include "sdio/mmcsd_card.h"
#include "sdio/mmcsd_core.h"

//#define ARASAN_DEBUG
#ifdef ARASAN_DEBUG
#define DBG(fmt, args...)     \
    do                                  \
    {                                   \
        /*printf("DEBUG: tick-%d, ", rt_tick_get());   */\
        printf(fmt, ## args);       \
    }                                   \
    while(0)
#else
#define DBG(fmt, args...)  do { } while (0)
#endif

#define ARASAN_DMA_DESC_BUFF_SIZE 0x8000

//static int maxfreq = ARASAN_CLOCKRATE_MAX;
#define DMA_BUFF_SIZE 4096  //512*8
int adma = 1;


static void arasan_enable_sdio_irq(struct rt_mmcsd_host *mmc, int32_t enable);
static inline void arsan_sw_reset(struct arasan_host *host, uint32_t flag);

static uint32_t led;
static void arasan_ctrl_led(struct arasan_host *host, uint32_t flag)
{
	if (led) {
		uint8_t ctrl_reg = readb(host->base + ARASAN_HOST_CTRL);

		if (flag)
			ctrl_reg |= ARASAN_HOST_CTRL_LED;
		else
			ctrl_reg &= ~ARASAN_HOST_CTRL_LED;

		host->ctrl = ctrl_reg;
		writeb(host->ctrl, host->base + ARASAN_HOST_CTRL);
	}
}

void *mmc_priv(struct rt_mmcsd_host *host)
{
	return (void *)host->private_data;
}

static void arasan_start_cmd(struct arasan_host *host, struct rt_mmcsd_cmd *cmd)
{
	uint16_t cmdreg = 0;

	/* Command Request */
	cmdreg = ARASAN_CMD_INDEX(cmd->cmd_code);
	DBG("%s: cmd type %04lx,  \033[31;43mCMD%ld\033[0m\n", __func__, resp_type(cmd), cmd->cmd_code);

    switch (resp_type(cmd))
    {
        case RESP_NONE:
            //DBG("RESP NONE\n");
            break;
        case RESP_R1:
        case RESP_R5:
        case RESP_R6:
        case RESP_R7:
            cmdreg |= ARASAN_CMD_RSP_48;
            cmdreg |= ARASAN_CMD_CHECK_CMDCRC;
            cmdreg |= ARASAN_CMD_INDX_CHECK;
            //DBG("RESP R1567\n");
            break;
        case RESP_R1B:
            cmdreg |= ARASAN_CMD_RSP_48BUSY;
            cmdreg |= ARASAN_CMD_CHECK_CMDCRC;
            cmdreg |= ARASAN_CMD_INDX_CHECK;
            //DBG("RESP R1B\n");
            break;
        case RESP_R2:
            cmdreg |= ARASAN_CMD_RSP_136;
            cmdreg |= ARASAN_CMD_CHECK_CMDCRC;
            //DBG("RESP R2\n");
            break;
        case RESP_R3:
        case RESP_R4:
            cmdreg |= ARASAN_CMD_RSP_48;
            //DBG("RESP R34\n");
            break;
        default:
            printf("ERROR: %s, unknown cmd type %lx\n", __func__, resp_type(cmd));
            return;
    }

	/* Wait until the CMD line is not in use */
	do { } while ((readl(host->base + ARASAN_PRESENT_STATE)) &
		 ARASAN_PRESENT_STATE_CMD_INHIBIT);

	/* Set the argument register */
	writel(cmd->arg, host->base + ARASAN_ARG);

	/* Data present and must be transferred */
	if (host->mrq->data) {
		cmdreg |= ARASAN_CMD_DATA_PRESENT;
		if (resp_type(cmd) == RESP_R1B)
			/* Wait for data inhibit */
			do { } while ((readl(host->base + ARASAN_PRESENT_STATE)) 
                & ARASAN_PRESENT_STATE_DAT_INHIBIT);
	}

	/* Write the Command */
	writew(cmdreg, host->base + ARASAN_CMD);

	DBG("\tcmd: 0x%x cmd reg: 0x%x - cmd->arg 0x%lx, reg 0x%x\n",
	    cmdreg, readw(host->base + ARASAN_CMD), cmd->arg, 
        readl(host->base + ARASAN_ARG));
}

static void arasan_adma_table_pre(struct arasan_host *host, struct rt_mmcsd_data *data)
{
	uint8_t *desc;
	uint32_t addr = (uint32_t)(data->buf);
	uint32_t size = data->blks * data->blksize;
	uint16_t len = 0;
	desc = host->adma_desc;

	k_cpu_dcache_clean_invalidate(addr, size);

	while(size > ARASAN_DMA_DESC_BUFF_SIZE)
	{
		len = ARASAN_DMA_DESC_BUFF_SIZE;

		desc[7] = (addr >> 24) & 0xff;
		desc[6] = (addr >> 16) & 0xff;
		desc[5] = (addr >> 8) & 0xff;
		desc[4] = (addr >> 0) & 0xff;

		desc[3] = (len >> 8) & 0xff;
		desc[2] = (len >> 0) & 0xff;

		desc[1] = 0x00;
		desc[0] = 0x21;

 		desc += 8;
		size -= ARASAN_DMA_DESC_BUFF_SIZE;
		addr += ARASAN_DMA_DESC_BUFF_SIZE;
	}

	len = size & 0xffff;
	desc[7] = (addr >> 24) & 0xff;
	desc[6] = (addr >> 16) & 0xff;
	desc[5] = (addr >> 8) & 0xff;
	desc[4] = (addr >> 0) & 0xff;

	desc[3] = (len >> 8) & 0xff;
	desc[2] = (len >> 0) & 0xff;

	desc[1] = 0x00;
	desc[0] = 0x23;

	DBG("desc last addr is \033[32m0x%8lx\033[0m\n", addr);
	DBG("desc last len is \033[32m0x%x\033[0m\n", len);

	host->adma_addr = (uint32_t)(host->adma_desc);
	k_cpu_dcache_clean_invalidate(host->adma_addr, (ARASAN_DMA_DESC_NUM * 2 + 1) * 4);

	writel(host->adma_addr, host->base + ARASAN_ADMA_ADDRESS);
	DBG("%s: adma_addr is 0x%8lx\n", __func__, host->adma_addr);
}

static int arasan_setup_data(struct arasan_host *host)
{
	uint16_t blksz = 0;
	uint16_t xfer = 0;
	struct rt_mmcsd_data *data = host->mrq->data;

	DBG("%s : flags:0x%lx, blks:0x%lx, blksz:0x%lx\n", __func__, data->flags, data->blks, data->blksize);

	/* Transfer Direction */
	if (data->flags & DATA_DIR_READ) {
		xfer |= ARASAN_XFER_DATA_DIR;
		host->status = STATE_DATA_READ;
	} else {
		xfer &= ~ARASAN_XFER_DATA_DIR;
		host->status = STATE_DATA_WRITE;	
	}

	xfer |= ARASAN_XFER_BLK_COUNT_EN;

	if (data->blks > 1) {
		if (host->auto_cmd12) // sd2.0
			xfer |= ARASAN_XFER_MULTI_BLK | ARASAN_XFER_AUTOCMD12;
		else // sdio2.0
			xfer |= ARASAN_XFER_MULTI_BLK;
	}

	/* Set the block size register */
	blksz |= (data->blksize & ARASAN_BLOCK_SIZE_TRANSFER);
	blksz |= (data->blksize & 0x1000) ? ARASAN_BLOCK_SIZE_SDMA_8KB : 0;
	//DBG("blksz:0x%x\n", blksz);
	writew(blksz, host->base + ARASAN_BLK_SIZE);

	/* Set the block count register */
	writew(data->blks, host->base + ARASAN_BLK_COUNT);

	xfer |= ARASAN_XFER_DMA_EN;

	arasan_adma_table_pre(host, data);
	host->ctrl |= ARASAN_HOST_CTRL_ADMA2_32;
	
	DBG("%s: [xfer 0x%x] [crtl 0x%x]\n", __func__, xfer, host->ctrl);

	writeb(host->ctrl, host->base + ARASAN_HOST_CTRL);
	writew(xfer, host->base + ARASAN_XFER_MODE);

	DBG("\tHC Reg [xfer 0x%x] [blksz 0x%x] [blkcount 0x%x] [CRTL 0x%x]\n",
	    readw(host->base + ARASAN_XFER_MODE),
	    readw(host->base + ARASAN_BLK_SIZE),
	    readw(host->base + ARASAN_BLK_COUNT),
	    readb(host->base + ARASAN_HOST_CTRL));

	return 0;
}

static void arasan_finish_data(struct arasan_host *host)
{
	struct rt_mmcsd_data *data = host->mrq->data;

	DBG("\t%s\n", __func__);

	DBG("adma_addr is 0x%x\n", readl(host->base + ARASAN_ADMA_ADDRESS));
	data->bytes_xfered = data->blks * data->blksize;
	memset(host->adma_desc, 0, (ARASAN_DMA_DESC_NUM * 2 + 1) * 4);
	host->status = STATE_CMD;
}

static int arasan_finish_cmd(uint32_t err_status, uint32_t status, uint32_t opcode)
{
	int ret = 0;

	if (err_status) {
		if (err_status & ARASAN_CMD_TIMEOUT) {
			DBG("\tcmd_timeout...\n");
			//printk("\tcmd_timeout...\n");
			ret = -1;
		}
		if (err_status & ARASAN_CMD_CRC_ERROR) {
			DBG("\tcmd_crc_error...\n");
			ret = -1;
		}
		if (err_status & ARASAN_CMD_END_BIT_ERROR) {
			DBG("\tcmd_end_bit_error...\n");
			ret = -1;
		}
		if (err_status & ARASAN_CMD_INDEX_ERROR) {
			DBG("\tcmd_index_error...\n");
			ret = -1;
		}
	}
	if (status & ARASAN_N_CMD_COMPLETE)
		DBG("\tCommand (CMD%lu) Completed irq...\n", opcode);

	return ret;
}

void mmc_request_done(struct rt_mmcsd_host *mmc, struct rt_mmcsd_req *mrq)
{
    struct arasan_host *host = mmc_priv(mmc);
	DBG("%s: req done (CMD%lu): %ld: %08lx %08lx %08lx %08lx\n",
			"xm host", mrq->cmd->cmd_code, mrq->cmd->err,
			mrq->cmd->resp[0], mrq->cmd->resp[1],
			mrq->cmd->resp[2], mrq->cmd->resp[3]);

	if (mrq->data) 
	{
		DBG("%s:     %d bytes transferred: %ld\n",
				"xm host",
				mrq->data->bytes_xfered, mrq->data->err);
	}

	if (mrq->stop) 
	{
		DBG("%s:     (CMD%lu): %ld: %08lx %08lx %08lx %08lx\n",
				"xm host", mrq->stop->cmd_code,
				mrq->stop->err,
				mrq->stop->resp[0], mrq->stop->resp[1],
				mrq->stop->resp[2], mrq->stop->resp[3]);
	}

	//krhino_sem_give(&host->sem);
	mmcsd_req_complete(mmc);
}

static int arasan_test_card(struct arasan_host *host)
{
	int ret = 0;
	uint32_t present = readl(host->base + ARASAN_PRESENT_STATE);
	
	if (!host->need_detect)
		goto out;
	if (!(present & ARASAN_PRESENT_STATE_CARD_PRESENT))
		ret = -1;

out:
#ifdef ARASAN_DEBUG
	if (present & ARASAN_PRESENT_STATE_CARD_STABLE)
		printf("\tcard stable...");
	if (!(present & ARASAN_PRESENT_STATE_WR_EN))
		printf("\tcard Write protected...");
	if (present & ARASAN_PRESENT_STATE_BUFFER_RD_EN)
		printf("\tPIO Read Enable...");
	if (present & ARASAN_PRESENT_STATE_BUFFER_WR_EN)
		printf("\tPIO Write Enable...");
	if (present & ARASAN_PRESENT_STATE_RD_ACTIVE)
		printf("\tRead Xfer data...");
	if (present & ARASAN_PRESENT_STATE_WR_ACTIVE)
		printf("\tWrite Xfer data...");
	if (present & ARASAN_PRESENT_STATE_DAT_ACTIVE)
		printf("\tDAT line active...");
#endif
	return ret;
}

static void arasan_request(struct rt_mmcsd_host *mmc, struct rt_mmcsd_req *mrq)
{
    struct arasan_host *host = mmc_priv(mmc);
	struct rt_mmcsd_cmd *cmd = mrq->cmd;
	DBG(">>> araran_request:\n");

	//krhino_sem_take(&host->sem, RHINO_WAIT_FOREVER);
	assert(host->mrq == NULL);
	
#if 1
	/* Check that there is a card in the slot */
	if (arasan_test_card(host) < 0) {
		printf("Error: No card present...\n");
		mrq->cmd->err = -1;
		mmc_request_done(mmc, mrq); /* 来自core.c，通知上层操作已经完成 */
		return;
	}
#endif

    host->mrq = mrq;
	host->status = STATE_CMD;
    if (mrq->data)  
		arasan_setup_data(host);

	/* Turn-on/off the LED when send/complete a cmd */
	arasan_ctrl_led(host, 1);

	//rt_timer_control(host->timer, RT_TIMER_CTRL_SET_TIME, (void*)&timeout_value);
	krhino_timer_start(&host->timer);
	//mod_timer(&host->timer, jiffies + 5 * HZ);

	//DBG("start_cmd.\n");
	arasan_start_cmd(host, cmd);		


    DBG("<<< araran_request done!\n");
}

static void arasan_power_set(struct arasan_host *host, uint32_t pwr, uint16_t vdd)
{
	uint8_t pwr_reg;

	pwr_reg = readb(host->base + ARASAN_PWR_CTRL);

	host->vdd = 1 << vdd;

	if (pwr) {
		pwr_reg &= 0xf1;

		if ((host->vdd & VDD_165_195) && host->cap.voltage18)
			pwr_reg |= ARASAN_PWR_BUS_VOLTAGE_18;
		else if ((host->vdd & VDD_29_30) && host->cap.voltage30)
			pwr_reg |= ARASAN_PWR_BUS_VOLTAGE_30;
		else if ((host->vdd & (VDD_32_33 | VDD_33_34)) && host->cap.voltage33)
			pwr_reg |= ARASAN_PWR_BUS_VOLTAGE_33;

		//pwr_reg |= ARASAN_PWR_CTRL_UP;
		/* 注意：电路设计和外围电路设计相反 */
		pwr_reg &= ~ARASAN_PWR_CTRL_UP;
	} else
		//pwr_reg &= ~ARASAN_PWR_CTRL_UP;
		pwr_reg |= ARASAN_PWR_CTRL_UP;

	DBG("%s: pwr_reg 0x%x, host->vdd = 0x%lx\n", __func__, pwr_reg, host->vdd);

	writeb(pwr_reg, host->base + ARASAN_PWR_CTRL);
}

static void arasan_set_clock(struct arasan_host *host, uint32_t freq)
{
	uint16_t clock = 0;
	//uint32_t flags;

	/* 协商阶段400K-低速模式 */
	//spin_lock(&host->lock);

	if ((host->freq != freq) && (freq)) {
		uint16_t divisor;

		/* Ensure clock is off before making any changes */
		//writew(clock, host->base + ARASAN_CLOCK_CTRL);

		/* core checks if this is a good freq < max_freq */
		host->freq = freq;

		//DBG("%s:\n\tnew freq %d", __func__, host->freq);

		/* Work out divisor for specified clock frequency */
		for (divisor = 1; divisor <= 256; divisor *= 2)
			/* Find first divisor producing a frequency less
			 * than or equal to MHz */
			if ((ARASAN_CLOCKRATE_MAX / divisor) <= freq)
				break;
		//DBG("\tdivisor %d", divisor);

		/* Set the clock divisor and enable the internal clock */
		clock = divisor << (ARASAN_CLOCK_CTRL_SDCLK_SHIFT);
		clock &= ARASAN_CLOCK_CTRL_SDCLK_MASK;
		clock |= ARASAN_CLOCK_CTRL_ICLK_ENABLE;
		writew(clock, host->base + ARASAN_CLOCK_CTRL);

		/* Busy wait for the clock to become stable */
		do { } while (((readw(host->base + ARASAN_CLOCK_CTRL)) &
			  ARASAN_CLOCK_CTRL_ICLK_STABLE) == 0);

		/* Enable the SD clock */
		clock |= ARASAN_CLOCK_CTRL_SDCLK_ENABLE;
		writew(clock, host->base + ARASAN_CLOCK_CTRL);

		DBG("\tclk ctrl reg. [0x%lx]\n", (uint32_t)readw(host->base + ARASAN_CLOCK_CTRL));
	}

	//spin_unlock(&host->lock);
}

static void arasan_set_ios(struct rt_mmcsd_host *mmc, struct rt_mmcsd_io_cfg *ios)
{
	struct arasan_host *host = (struct arasan_host *)(mmc->private_data);
	uint8_t ctrl_reg = readb(host->base + ARASAN_HOST_CTRL);

	DBG("%s: pwr %d, clk %ld, vdd %d, bus_width %d\n",
	    __func__, ios->power_mode, ios->clock, ios->vdd, ios->bus_width);

	/* Set the power supply mode */
	if (ios->power_mode == MMCSD_POWER_OFF) 
		arasan_power_set(host, 0, ios->vdd);
	else 
		arasan_power_set(host, 1, ios->vdd);

	/* Clear the current bus width configuration */
	ctrl_reg &= ~ARASAN_HOST_CTRL_SD_MASK;

	/* Set SD bus bit mode */
	switch (ios->bus_width) {
	case MMCSD_BUS_WIDTH_8:
		ctrl_reg |= ARASAN_HOST_CTRL_SD8;
		break;
	case MMCSD_BUS_WIDTH_4:
		ctrl_reg |= ARASAN_HOST_CTRL_SD;
		break;
	/* added by me, 不能插拔识别卡的问题 */
	case MMCSD_BUS_WIDTH_1:
		ctrl_reg &= ~ARASAN_HOST_CTRL_SD;
		break;
	}

	/* Default to maximum timeout */
	writeb(0x0e, host->base + ARASAN_TIMEOUT_CTRL);

	/* Disable Card Interrupt in Host in case we change
	 * the Bus Width. */
	arasan_enable_sdio_irq(host->mmc, 0);

	host->ctrl = ctrl_reg;
	writeb(ctrl_reg, host->base + ARASAN_HOST_CTRL);

	arasan_enable_sdio_irq(host->mmc, 1);

	/* Set clock */
	arasan_set_clock(host, ios->clock);
	DBG("clk set done.\n");
}

static void arasan_enable_sdio_irq(struct rt_mmcsd_host *mmc, int32_t enable)
{
	//uint32_t flags;
	struct arasan_host *host = (struct arasan_host *)(mmc->private_data);
	static uint32_t arasan_irq_mask;

	DBG("%s: %s CARD_IRQ\n", __func__, enable ? "enable" : "disable");
	
	arasan_irq_mask = host->card_irq ?ARASAN_IRQ_DEFAULT_MASK: 
						ARASAN_IRQ_DEFAULT_MASK_NOCARDIRQ;
	//spin_lock(&host->lock);

	if (enable)
		host->intr_en = arasan_irq_mask;
	else 
		host->intr_en = 0;

	writel(host->intr_en, host->base + ARASAN_NORMAL_INT_STATUS_EN);
	writel(host->intr_en, host->base + ARASAN_NORMAL_INT_SIGN_EN);
	//spin_unlock(&host->lock);
}

static int32_t arasan_get_card_status(struct rt_mmcsd_host *mmc)
{
    return 0;
}

static const struct rt_mmcsd_host_ops arasan_ops =
{
    .request            = arasan_request,
    .set_iocfg          = arasan_set_ios,
    .enable_sdio_irq    = arasan_enable_sdio_irq,
    .get_card_status    = arasan_get_card_status,
};

/* Read the response from the card */
static void arasan_get_resp(struct rt_mmcsd_cmd *cmd, struct arasan_host *host)
{
	uint32_t i;
	uint32_t resp[4];

	for (i = 0; i < 4; i++)
		resp[i] = readl(host->base + ARASAN_RSP(i));

	if (resp_type(cmd) == RESP_R2) {
		cmd->resp[3] = (resp[0] << 8);
		cmd->resp[2] = (resp[0] >> 24) | (resp[1] << 8);
		cmd->resp[1] = (resp[1] >> 24) | (resp[2] << 8);
		cmd->resp[0] = (resp[2] >> 24) | (resp[3] << 8);
	} else {
		cmd->resp[0] = resp[0];
		cmd->resp[1] = resp[1];
	}
	
	/* 数据出错之后，response寄存器当中仍然会有接收到响应数据 */

	DBG("%s: resp length %s\n-(CMD%lu):\n %08lx %08lx %08lx %08lx\n"
	    "-RAW reg:\n %08lx %08lx %08lx %08lx\n",
	    __func__, (resp_type(cmd) == RESP_R2) ? "136" : "48", cmd->cmd_code,
	    cmd->resp[0], cmd->resp[1], cmd->resp[2], cmd->resp[3],
	    resp[0], resp[1], resp[2], resp[3]);
}

#ifdef ARASAN_DEBUG
static void arasan_adma_error(struct arasan_host *host)
{
	uint8_t status = readb(host->base + ARASAN_ADMA_ERR_STATUS);

	if (status & ARASAN_ADMA_ERROR_LENGTH)
		DBG("-ADMA Length Mismatch Error...");

	if ((status & 0x3) == ARASAN_ADMA_ERROR_ST_TFR)
		DBG("-Transfer Data Error desc: ");
	else if ((status & 0x3) == ARASAN_ADMA_ERROR_ST_FDS)
		DBG("-Fetch Data Error desc: ");
	else if ((status & 0x3) == ARASAN_ADMA_ERROR_ST_STOP)
		DBG("-Stop DMA Data Error desc: ");

	DBG("adma_addr is 0x%x\n", readl(host->base + ARASAN_ADMA_ADDRESS));
}
#else
static void arasan_adma_error(struct arasan_host *host)
{
}
#endif

static void arasan_irq(int irq, void *param)
{
    struct arasan_host *host = (struct arasan_host *)param;
    uint32_t status, err_status, handled = 0;
	uint16_t int_en;
	struct rt_mmcsd_cmd *cmd = NULL;
	struct rt_mmcsd_data *data = NULL;

	//spin_lock(&host->lock);

	/* Interrupt Status */
	status = readl(host->base + ARASAN_NORMAL_INT_STATUS);
	err_status = (status >> 16) & 0xffff;

	/* 清中断状态寄存器 */
	DBG("\tclear status and exit...\n");
	writel(status, host->base + ARASAN_NORMAL_INT_STATUS);

    if (status & ARASAN_N_CARD_IRQ) {
		/* 进入这个中断时不能直接退出，因为这个接收到这个中断
		 * 时，还可能接收到数据完成中断
		 */
		/*
		 * mmc_signal_sdio_irq(host->mmc);
		 * 这让人崩溃，这个函数里面将所有的中断关闭掉，在这期间是会有
		 * 其它的中断的(这是由于实现不同造成的), 所以这里用下面的方法
		 * 取代，只关闭sdio中断
		 */
		int_en = readl(host->base+ARASAN_NORMAL_INT_STATUS_EN);
		int_en &= ~(0x1<<8);
		writel(int_en, host->base + ARASAN_NORMAL_INT_STATUS_EN);
		/*
		 * 替代 wake_up_prcess,因为我们对外设由电源开关，而在
		 * 本分代码的逻辑上是先打开中断，然后在打开开关，这就造成
		 * 在打开中断时，dat1为低，造成中断，但此时，sdio_irq_thread
		 * 还未被赋值
		 */
		//host->mmc->sdio_irq_pending = true;
		if (host->mmc && host->mmc->sdio_irq_thread)
			sdio_irq_wakeup(host->mmc);
	}

    DBG("\033[31m%s\033[0m: Normal IRQ status  0x%lx, Error status 0x%lx\n",
	    __func__, status & 0xffff, err_status);

    if (!host->mrq) {
        DBG("host->mrq is NULL!");
        goto out;
    }

    cmd = host->mrq->cmd;
	data = host->mrq->data;

    cmd->err = 0;
	/* Check for any CMD interrupts */
	if (status & ARASAN_INT_CMD_MASK) {
		DBG("response to cmd.\n");
		cmd->err = arasan_finish_cmd(err_status, status, cmd->cmd_code);
		if (cmd->err)
			arsan_sw_reset(host, reset_cmd_line);

		if ((host->status == STATE_CMD) || cmd->err) {
			arasan_get_resp(cmd, host);
			handled = 1;
		}
	}

    /* Check for any data interrupts */
	if ((status & ARASAN_INT_DATA_MASK) && data) {
		data->err = 0;
		if (err_status) {
			//printk("error.\n");
			if (err_status & ARASAN_DATA_TIMEOUT_ERROR) {
				DBG("\tdata_timeout_error...\n");
				data->err = -1;
			}
			if (err_status & ARASAN_DATA_CRC_ERROR) {
				DBG("\tdata_crc_error...\n");
				data->err = -1;				
			}
			if (err_status & ARASAN_DATA_END_ERROR) {
				DBG("\tdata_end_error...\n");
				data->err = -1;
			}
			if (err_status & ARASAN_AUTO_CMD12_ERROR) {
				uint16_t err_cmd12 = readw(host->base + ARASAN_CMD12_ERR_STATUS);
				DBG("\tc12err 0x%04x\n", err_cmd12);

				if (err_cmd12 & ARASAN_AUTOCMD12_ERR_NOTEXE)
					data->stop->err = -1;

				if ((err_cmd12 & ARASAN_AUTOCMD12_ERR_TIMEOUT)
				    && !(err_cmd12 & ARASAN_AUTOCMD12_ERR_CRC))
					/* Timeout Error */
					data->stop->err = -1;
				else if (!(err_cmd12 & ARASAN_AUTOCMD12_ERR_TIMEOUT)
					 && (err_cmd12 & ARASAN_AUTOCMD12_ERR_CRC))
					/* CRC Error */
					data->stop->err = -1;
				else if ((err_cmd12 & ARASAN_AUTOCMD12_ERR_TIMEOUT)
					 && (err_cmd12 & ARASAN_AUTOCMD12_ERR_CRC))
					DBG("\tCMD line Conflict\n");
			}
			arsan_sw_reset(host, reset_dat_line);
			handled = 1;
		} else {
			if (((status & ARASAN_N_BUFF_READ) ||
				    status & ARASAN_N_BUFF_WRITE)) {
				DBG("\tData R/W interrupts...\n");
				//arasan_data_pio(host); // 使用pio的方式进行数据传输必须在中断当中手动将数据全部写入到buff当中去
			}

			if (status & ARASAN_N_DMA_IRQ)
				DBG("\tDMA interrupts...\n");

			if (status & ARASAN_N_TRANS_COMPLETE) {
				DBG("\tData XFER completed interrupts...\n");
				arasan_finish_data(host);
				if (data->stop) {
					uint32_t opcode = data->stop->cmd_code;
					data->stop->err = arasan_finish_cmd(err_status, status, opcode);
					arasan_get_resp(data->stop, host);
				}
				handled = 1;
			}

		}
	}
	if (err_status & ARASAN_ADMA_ERROR) {
		//printk("3.\n");
		DBG("\tADMA Error...\n");
		arasan_adma_error(host);
		cmd->err = -1;
	}
	if (err_status & ARASAN_CURRENT_LIMIT_ERROR) {
		//printk("4.\n");
		DBG("\tPower Fail...\n");
		cmd->err = -1;
	}

	if (host->mrq && handled) {
		//printk("5.\n");
		struct rt_mmcsd_req *mrq = host->mrq;

		arasan_ctrl_led(host, 0);

		krhino_timer_stop(&host->timer);

		host->mrq = NULL;
		DBG("\tcalling mmc_request_done...\n");
		mmc_request_done(host->mmc, mrq);
	}


out:
	//spin_unlock(&host->lock);
	return;
}

static inline void arasan_clear_interrupts(struct arasan_host *host)
{
	writel(0, host->base + ARASAN_NORMAL_INT_STATUS_EN);
	//writel(0, host->base + ARASAN_ERR_INT_STATUS_EN);
	writel(0, host->base + ARASAN_NORMAL_INT_SIGN_EN);
}

static inline void arsan_sw_reset(struct arasan_host *host, uint32_t flag)
{
	/* After completing the reset, wait the HC clears these bits */
	if (flag == reset_all) {
		writeb(ARSAN_RESET_ALL, host->base + ARASAN_SW_RESET);
		do { } while ((readb(host->base + ARASAN_SW_RESET)) & ARSAN_RESET_ALL);
	} else if (flag == reset_cmd_line) {
		writeb(ARSAN_RESET_CMD_LINE, host->base + ARASAN_SW_RESET);
		do { } while ((readb(host->base + ARASAN_SW_RESET)) & ARSAN_RESET_CMD_LINE);
	} else if (flag == reset_dat_line) {
		writeb(ARSAN_RESET_DAT_LINE, host->base + ARASAN_SW_RESET);
		do { } while ((readb(host->base + ARASAN_SW_RESET)) & ARSAN_RESET_DAT_LINE);
	}
}

static inline void arsan_hc_version(struct arasan_host *host)
{
#ifdef ARASAN_DEBUG
	uint16_t version;
	version = readw(host->base + ARASAN_HOST_VERSION);
	DBG("Arasan MMC/SDIO:\n");
	DBG("\tHC Vendor Version Number: %d\n", (version >> 8));
	DBG("\tHC SPEC Version Number: %d\n", (version & 0x00ff));
#endif
}

static void arasan_capabilities(struct arasan_host *host)
{
	uint32_t cap;
	uint32_t max_blk_len;

	cap = readl(host->base + ARASAN_CAPABILITIES);

	DBG("\tArasan capabilities: 0x%lx\n", cap);

	host->cap.timer_freq = cap & 0x3f;
	host->cap.timer_unit = (cap >> 7) & 0x1;

	DBG("\tTimeout Clock Freq: %ld %s\n", host->cap.timer_freq,
		 host->cap.timer_unit ? "MHz" : "KHz");

	host->cap.base_clk_sd = (cap >> 8) & 0x3f;
	DBG("\tBase Clock Freq for SD: %ld MHz\n", host->cap.base_clk_sd);

	max_blk_len = (cap >> 16) & 0x3;
	switch (max_blk_len) {
	case 0:
		host->cap.max_blk_len = 512;
		break;
	case 1:
		host->cap.max_blk_len = 1024;
		break;
	case 2:
		host->cap.max_blk_len = 2048;
		break;
	case 3:
		host->cap.max_blk_len = 4096;
		break;
	default:
		break;
	}
	DBG("\tMax Block size: %ld bytes\n", host->cap.max_blk_len);

	host->cap.adma2 = (cap >> 19) & 0x1;
	host->cap.high_speed = (cap >> 21) & 0x1;
	host->cap.sdma = (cap >> 22) & 0x1;

	DBG("\tadma2 %s, high speed %s, sdma %s\n",
		 host->cap.adma2 ? "Yes" : "Not",
		 host->cap.high_speed ? "Yes" : "Not",
		 host->cap.sdma ? "Yes" : "Not");

	host->cap.suspend = (cap >> 23) & 0x1;
	DBG("\tsuspend/resume %s suported\n",
		 host->cap.adma2 ? "is" : "Not");

	host->cap.voltage33 = (cap >> 24) & 0x1;
	host->cap.voltage30 = (cap >> 25) & 0x1;
	host->cap.voltage18 = (cap >> 26) & 0x1;
	host->cap.int_mode = (cap >> 27) & 0x1;
	host->cap.spi = (cap >> 29) & 0x1; /* 是否支持spi模式 */
	host->cap.spi_block = (cap >> 30) & 0x1;

	if (host->cap.voltage33)
		DBG("\t3.3V voltage suported\n");
	if (host->cap.voltage30)
		DBG("\t3.0V voltage suported\n");
	if (host->cap.voltage18)
		DBG("\t1.8V voltage suported\n");

	if (host->cap.int_mode)
		DBG("\tInterrupt Mode supported\n");
	if (host->cap.spi)
		DBG("\tSPI Mode supported\n");
	if (host->cap.spi_block)
		DBG("\tSPI Block Mode supported\n");
}

static void arasan_setup_hc(struct arasan_host *host)
{
	/* Clear all the interrupts before resetting */
	arasan_clear_interrupts(host);

	/* Reset All and get the HC version */
	arsan_sw_reset(host, reset_all);

	/* Print HC version and SPEC */
	arsan_hc_version(host);

	/* Set capabilities and print theri info */
	arasan_capabilities(host);
}

static void arasan_timeout_timer(void *timer, void *data)
{
	struct arasan_host *host = (struct arasan_host *)data;
	struct rt_mmcsd_req *mrq;

	//spin_lock(&host->lock);

	DBG("%s: timeout.\n", __func__);
	//printk("host->status:%d\n", host->status);
	if ((host->mrq) && ((host->status==STATE_CMD) ||
				(host->status==STATE_DATA_READ) || 
				(host->status==STATE_DATA_WRITE))) {
		mrq = host->mrq;

		printf("Timeout waiting for hardware interrupt.\n");

		writel(0xffffffff, host->base + ARASAN_NORMAL_INT_STATUS);
		arasan_enable_sdio_irq(host->mmc, 1);

		if (mrq->data) {
			arasan_finish_data(host);
			arsan_sw_reset(host, reset_dat_line);
			mrq->data->err = -1;
		}
		if (mrq->cmd) {
			mrq->cmd->err = -1;
			arsan_sw_reset(host, reset_cmd_line);
			arasan_get_resp(mrq->cmd, host);
		}
		arasan_ctrl_led(host, 0);
		host->mrq = NULL;
		mmc_request_done(host->mmc, mrq);
	}
	//spin_unlock(&host->lock);
}

int arasan_probe(void *priv_data)
{
    struct rt_mmcsd_host *mmc;
	struct arasan_host *host = NULL;
    struct arasan_platform_data *arasan_data = (struct arasan_platform_data *)priv_data;

	DBG("%s start\n", __func__);

	if (arasan_data->p_powerup != NULL) {
		//DBG("sdio power up!\n");
		arasan_data->p_powerup();
	}
	DBG("sdio powered up!\n");	

    mmc = mmcsd_alloc_host();
    if (!mmc)
    {
        printf("ERROR: %s, failed to malloc host\n", __func__);
        return -1;
    }

	host = (struct arasan_host*)aos_malloc(sizeof(struct arasan_host));
	if (!host) 
	{
		printf("alloc arasan_host failed!\n");
		goto err;
	}
    memset(host, 0, sizeof(struct arasan_host));

	mmc->private_data = (void *)host;
	host->mmc = mmc;
	host->base = arasan_data->base;
	host->irq = arasan_data->irq;
	host->card_irq = arasan_data->card_irq;
	host->auto_cmd12 = arasan_data->auto_cmd12;
	//host->use_pio = arasan_data->use_pio;
	host->need_detect = arasan_data->need_detect;
	host->sdio_4bit_data = arasan_data->sdio_4bit_data;  
	//host->need_poll = arasan_data->need_poll;
	arasan_setup_hc(host);
	
	//krhino_sem_create(&host->sem, "arasan_sem", 1);
	//spin_lock_init(&host->lock);

	host->freq = host->cap.timer_freq * 1000000;
	host->adma_desc = (uint8_t*)aos_malloc_align((ARASAN_DMA_DESC_NUM * 2 + 1) * 4, 32);

	/* 这些支持的电压值会被用做探测过程中协商host与sd卡的电压 */
	if (host->cap.voltage33)
		mmc->valid_ocr |= VDD_32_33 | VDD_33_34;
	if (host->cap.voltage30)
		mmc->valid_ocr |= VDD_29_30;
	if (host->cap.voltage18)
		mmc->valid_ocr |= VDD_165_195;

	mmc->flags = MMCSD_SUP_SDIO_IRQ | MMCSD_MUTBLKWRITE;
	if (host->sdio_4bit_data) { 
		mmc->flags |= MMCSD_BUSWIDTH_4 | MMCSD_BUSWIDTH_8;
	}
	if ((!host->card_irq) && (host->cap.high_speed))
		mmc->flags |= MMCSD_SUP_HIGHSPEED;
	
	/* 在子系统当中没多大用，只是填充一个字段，上层想要知道这个值时，能够有一种方法获取 */
	mmc->freq_max = ARASAN_CLOCKRATE_MAX;
	mmc->freq_min = ARASAN_CLOCKRATE_MAX / 256;
	mmc->max_blk_size = host->cap.max_blk_len;
	mmc->max_blk_count = 65535;
	mmc->max_dma_segs = 128;
	mmc->max_seg_size = 65536;
	mmc->ops = &arasan_ops;

	if(arasan_data->id == 0){
        os_hw_interrupt_create(host->irq, arasan_irq, (void *)host, "arasan_irq0");
		krhino_timer_create(&host->timer, "sdio_timer0", arasan_timeout_timer, krhino_ms_to_ticks(5000), 0, (void *)host, 0);
    }
    else if(arasan_data->id == 1){
        os_hw_interrupt_create(host->irq, arasan_irq, (void *)host, "arasan_irq1");
    }
	os_hw_interrupt_enable(host->irq);

	mmcsd_change(mmc);

	printf("sdio driver initialized...\n");

	led = 1;

	return 0;

err:
	mmcsd_free_host(mmc);

	return -1;
}

int arasan_exit(void *priv_data)
{
    return 0;
}
