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

#include "sys/endian.h"

#include "driver/chip/sdmmc/hal_sdhost.h"
#include "driver/chip/sdmmc/sdmmc.h"
#include "sdhost.h"
#include "core.h"
#ifdef CONFIG_USE_MMC
#include "_mmc.h"
#endif
#include "_sd.h"

#ifdef CONFIG_USE_SD
static const unsigned int tran_exp[] = {
	10000,		100000,		1000000,	10000000,
	0,		0,		0,		0
};

static const unsigned char tran_mant[] = {
	0,	10,	12,	13,	15,	20,	25,	30,
	35,	40,	45,	50,	55,	60,	70,	80,
};
/*
static const unsigned int tacc_exp[] = {
	1,	10,	100,	1000,	10000,	100000,	1000000, 10000000,
};

static const unsigned int tacc_mant[] = {
	0,	10,	12,	13,	15,	20,	25,	30,
	35,	40,	45,	50,	55,	60,	70,	80,
};
*/

static int32_t mmc_send_app_op_cond(struct mmc_host *host, uint32_t ocr, uint32_t *rocr)
{
	struct mmc_command cmd = {0};
	int32_t i, err;

	SD_BUG_ON(!host);

	cmd.opcode = SD_APP_OP_COND;
	cmd.arg = ocr;
	cmd.flags = MMC_RSP_SPI_R1 | MMC_RSP_R3 | MMC_CMD_BCR;

	for (i = 100; i; i--) {
		err = mmc_wait_for_app_cmd(host, NULL, &cmd);
		if (err)
			break;

		/* otherwise wait until reset completes */
		if (cmd.resp[0] & MMC_CARD_BUSY)
			break;

		cmd.arg = 0x41000000 | (cmd.resp[0] & 0xFF8000);

		err = -1;

		mmc_mdelay(10);
	}

	if (rocr)
		*rocr = cmd.resp[0];

	return err;
}

int32_t mmc_app_sd_status(struct mmc_card *card, uint8_t *ssr)
{
	struct mmc_request mrq;
	struct mmc_command cmd = {0};
	struct mmc_data data = {0};
	struct scatterlist sg;

	SD_BUG_ON(!ssr);

	if (mmc_app_cmd(card->host, card)) {
		return -1;
	}

	mrq.cmd = &cmd;
	mrq.data = &data;

	cmd.opcode = SD_APP_SD_STATUS;
	cmd.arg = 0;
	cmd.flags = MMC_RSP_SPI_R2 | MMC_RSP_R1 | MMC_CMD_ADTC;

	data.blksz = 64;
	data.blocks = 1;
	data.flags = MMC_DATA_READ;
	data.sg = &sg;
	data.sg_len = 1;

	sg.len = 64;
	sg.buffer = ssr;

	if (mmc_wait_for_req(card->host, &mrq)) {
		return -1;
	}

	SD_LOGN("card raw SD status:\n");
	sd_hex_dump_bytes((void *)ssr, 64);

	return 0;
}

int32_t mmc_app_send_scr(struct mmc_card *card, uint32_t *raw_scr)
{
	struct mmc_command cmd = {0};
	struct mmc_data data = {0};
	struct mmc_request mrq;
	struct scatterlist sg;

	if (mmc_app_cmd(card->host, card)) {
		return -1;
	}

	mrq.cmd = &cmd;
	mrq.data = &data;

	cmd.opcode = SD_APP_SEND_SCR;
	cmd.arg = 0;
	cmd.flags = MMC_RSP_SPI_R1 | MMC_RSP_R1 | MMC_CMD_ADTC;


	data.blksz = 8;
	data.blocks = 1;
	data.flags = MMC_DATA_READ;
	data.sg = &sg;
	data.sg_len = 1;

	sg.len = 8;
	sg.buffer = (void *)raw_scr;

	/* get scr, 8 bytes */
	if (mmc_wait_for_req(card->host, &mrq)) {
		return -1;
	}

	raw_scr[0] = be32_to_cpu(raw_scr[0]);
	raw_scr[1] = be32_to_cpu(raw_scr[1]);

	return 0;
}

/*
 * Given the decoded CSD structure, decode the raw CID to our CID structure.
 */
void mmc_decode_cid(struct mmc_card *card, uint32_t *resp)
{
	memset(&card->cid, 0, sizeof(struct mmc_cid));

	/*
	 * SD doesn't currently have a version field so we will
	 * have to assume we can parse this.
	 */
	card->cid.manfid = UNSTUFF_BITS(resp, 120, 8);
	card->cid.oemid = UNSTUFF_BITS(resp, 104, 8);
	card->cid.prod_name[0] = UNSTUFF_BITS(resp, 96, 8);
	card->cid.prod_name[1] = UNSTUFF_BITS(resp, 88, 8);
	card->cid.prod_name[2] = UNSTUFF_BITS(resp, 80, 8);
	card->cid.prod_name[3] = UNSTUFF_BITS(resp, 72, 8);
	card->cid.prod_name[4] = UNSTUFF_BITS(resp, 64, 8);
	card->cid.hwrev = UNSTUFF_BITS(resp, 60, 4);
	card->cid.fwrev = UNSTUFF_BITS(resp, 56, 4);
	card->cid.serial = UNSTUFF_BITS(resp, 24, 32);
	card->cid.year = UNSTUFF_BITS(resp, 12, 8);
	card->cid.month = UNSTUFF_BITS(resp, 8, 4);

	card->cid.year += 2000; /* SD cards year offset */
}

int32_t mmc_send_cid(struct mmc_card *card)
{
	struct mmc_command cmd = {0};
	uint32_t cid[4] = {0};

	cmd.opcode = MMC_SEND_CID;
	cmd.arg = card->rca << 16;
	cmd.flags = MMC_RSP_R2 | MMC_CMD_AC;

	if (mmc_wait_for_cmd(card->host, &cmd)) {
		return -1;
	}

	memcpy((void *)cid, (void *)cmd.resp, 16);
	SD_LOGN("card raw cid:\n");
	sd_hex_dump_bytes((void *)cid, 16);

	mmc_decode_cid(card, cid);

	return 0;
}

static int32_t mmc_decode_csd(struct mmc_card *card, uint32_t *csd)
{
	int32_t e, m, csd_struct;
	uint32_t *resp = csd;

	csd_struct = UNSTUFF_BITS(resp, 126, 2);
	card->csd.csd_ver = csd_struct;

	switch (csd_struct) {
	case 0:
		//m = UNSTUFF_BITS(resp, 115, 4);
		//e = UNSTUFF_BITS(resp, 112, 3);
		//csd->tacc_ns = (tacc_exp[e] * tacc_mant[m] + 9) / 10;
		//csd->tacc_clks = UNSTUFF_BITS(resp, 104, 8) * 100;

		m = UNSTUFF_BITS(resp, 99, 4);
		e = UNSTUFF_BITS(resp, 96, 3);
		card->csd.max_dtr = tran_exp[e] * tran_mant[m];
		card->csd.cmdclass =  UNSTUFF_BITS(resp, 84, 12);

		e = UNSTUFF_BITS(resp, 47, 3);
		m = UNSTUFF_BITS(resp, 62, 12);
		card->csd.c_size_mult = e;
		card->csd.c_size = m;
		card->csd.capacity = (1 + m) << (e + 2);

		//csd->read_blkbits = UNSTUFF_BITS(resp, 80, 4);
		card->csd.read_blk_len = UNSTUFF_BITS(resp, 80, 4);
		//csd->read_partial = UNSTUFF_BITS(resp, 79, 1);
		//csd->write_misalign = UNSTUFF_BITS(resp, 78, 1);
		//csd->read_misalign = UNSTUFF_BITS(resp, 77, 1);
		//csd->r2w_factor = UNSTUFF_BITS(resp, 26, 3);
		//csd->write_blkbits = UNSTUFF_BITS(resp, 22, 4);
		//csd->write_partial = UNSTUFF_BITS(resp, 21, 1);

		//if (UNSTUFF_BITS(resp, 46, 1)) {
		//	csd->erase_size = 1;
		//} else if (csd->write_blkbits >= 9) {
		//	csd->erase_size = UNSTUFF_BITS(resp, 39, 7) + 1;
		//	csd->erase_size <<= csd->write_blkbits - 9;
		//}
		break;
	case 1:
		/*
		 * This is a block-addressed SDHC or SDXC card. Most
		 * interesting fields are unused and have fixed
		 * values. To avoid getting tripped by buggy cards,
		 * we assume those fixed values ourselves.
		 */
		mmc_card_set_blockaddr(card);

		//csd->tacc_ns = 0; /* Unused */
		//csd->tacc_clks = 0; /* Unused */

		m = UNSTUFF_BITS(resp, 99, 4);
		e = UNSTUFF_BITS(resp, 96, 3);
		card->csd.cmdclass =  UNSTUFF_BITS(resp, 84, 12);
		card->csd.read_blk_len = UNSTUFF_BITS(resp, 80, 4);
		card->csd.max_dtr = tran_exp[e] * tran_mant[m];

		m = UNSTUFF_BITS(resp, 48, 22);
		card->csd.c_size = m;
		card->csd.capacity = (1 + m) << 9;
		/* SDXC cards have a minimum C_SIZE of 0x00FFFF */
		if (card->csd.c_size >= 0xFFFF)
			mmc_card_set_ext_capacity(card);

		card->csd.c_size_mult = 0;

		//csd->read_blkbits = 9;
		//csd->read_partial = 0;
		//csd->write_misalign = 0;
		//csd->read_misalign = 0;
		//csd->r2w_factor = 4; /* Unused */
		//csd->write_blkbits = 9;
		//csd->write_partial = 0;
		//csd->erase_size = 1;
		break;
	default:
		SD_LOGE("%s: unrecognised CSD structure version %d\n",
			__func__, csd_struct);
		return -1;
	}

	//card->erase_size = csd->erase_size;
	SD_LOGD("%s %d c_z:%d ca:%d %d\n", __func__, csd_struct, card->csd.c_size,
	        card->csd.capacity, card->csd.c_size_mult);

	return 0;
}

/*
 * Given a 64-bit response, decode to our card SCR structure.
 */
static int32_t mmc_decode_scr(struct mmc_card *card, uint32_t *raw_scr)
{
	struct sd_scr *scr = &card->scr;
	uint32_t scr_struct;
	uint32_t resp[4];

	resp[3] = raw_scr[1];
	resp[2] = raw_scr[0];

	scr_struct = UNSTUFF_BITS(resp, 60, 4);
	if (scr_struct != 0) {
		SD_LOGW("sdc unrecognised SCR structure version %d\n", scr_struct);
		return -1;
	}

	scr->sda_vsn = UNSTUFF_BITS(resp, 56, 4);
	scr->security_sup = UNSTUFF_BITS(resp, 52, 3);
	scr->bus_widths = UNSTUFF_BITS(resp, 48, 4);

	if (scr->sda_vsn == SCR_SPEC_VER_2)
		/* Check if Physical Layer Spec v3.0 is supported */
		scr->sda_spec3 = UNSTUFF_BITS(resp, 47, 1);

	//if (UNSTUFF_BITS(resp, 55, 1))
	//	card->erased_byte = 0xFF;
	//else
	//	card->erased_byte = 0x0;

	if (scr->sda_spec3) {
		scr->cmds = UNSTUFF_BITS(resp, 32, 2);
		scr->sda_spec4 = UNSTUFF_BITS(resp, 42, 1);
		scr->sda_spec5 = UNSTUFF_BITS(resp, 38, 4);
	}

	return 0;
}

/*
 * Fetch and process SD Status register.
 */
static int32_t mmc_read_ssr(struct mmc_card *card)
{
	int32_t err, i;
	uint32_t ssr[64/4];

	if (!(card->csd.cmdclass & CCC_APP_SPEC)) {
		SD_LOGW("%s: card lacks mandatory SD Status function.\n", __func__);
		return 0;
	}

	err = mmc_app_sd_status(card, (uint8_t *)ssr);
	if (err) {
		SD_LOGW("%s: problem reading SD Status register.\n", __func__);
		err = 0;
		goto out;
	}

	for (i = 0; i < 16; i++)
		ssr[i] = be32_to_cpu(ssr[i]);

	card->speed_class = UNSTUFF_BITS(ssr, 440 - 384, 8) * 2;
	if (card->speed_class == 8)
		card->speed_class = 10;
out:
	return err;
}

/*
 * Fetches and decodes switch information
 */
static int32_t mmc_read_switch(struct mmc_card *card)
{
	int32_t err;
	uint32_t status[64/sizeof(uint32_t)] = {0};
	uint8_t *p_sta = (uint8_t *)status;

	if (card->scr.sda_vsn < SCR_SPEC_VER_1) {
		SD_LOGW("Card ver. does not support to read switch info!\n");
		return -1;
	}
	if (!(card->csd.cmdclass & CCC_SWITCH)) {
		SD_LOGW("Card cmdclass does not support to read switch info!\n");
		return -1;
	}

	/* Find out the supported Bus Speed Modes. */
	err = mmc_sd_switch(card, SD_SWITCH_CHECK, SD_SWITCH_GRP_ACCESS_MODE,
	                    SD_SWITCH_ACCESS_HS, p_sta);
	if (err) {
		SD_LOGW("%s: problem reading Bus Speed modes.\n", __func__);
		goto out;
	}

	if (p_sta[13] & SD_MODE_HIGH_SPEED)
		card->sw_caps.hs_max_dtr = HIGH_SPEED_MAX_DTR;

	if (card->scr.sda_spec3) {
		card->sw_caps.sd3_bus_mode = p_sta[13];

		/* Find out Driver Strengths supported by the card */
		err = mmc_sd_switch(card, SD_SWITCH_CHECK, SD_SWITCH_GRP_DRV_STRENGTH,
		                    SD_SWITCH_ACCESS_HS, p_sta);
		if (err) {
			SD_LOGW("%s: problem reading Driver Strength.\n", __func__);
			goto out;
		}

		card->sw_caps.sd3_drv_type = p_sta[9];

		/* Find out Current Limits supported by the card */
		err = mmc_sd_switch(card, SD_SWITCH_CHECK, SD_SWITCH_GRP_CUR_LIMIT,
		                    SD_SWITCH_ACCESS_HS, p_sta);
		if (err) {
			SD_LOGW("%s: problem reading Current Limit.\n", __func__);
			goto out;
		}

		card->sw_caps.sd3_curr_limit = p_sta[7];
	}

out:
	return err;
}

/*
 * Test if the card supports high-speed mode and, if so, switch to it.
 */
int32_t mmc_sd_switch_hs(struct mmc_card *card)
{
	int32_t err;
	uint32_t status[64/sizeof(uint32_t)] = {0};
	uint8_t *p_sta = (uint8_t *)status;

	if (card->scr.sda_vsn < SCR_SPEC_VER_1) {
		SD_LOGW("Card ver. does not support to switch to high speed!\n");
		return -1;
	}

	if (!(card->csd.cmdclass & CCC_SWITCH)) {
		SD_LOGW("Card cmdclass not support to switch to high speed!\n");
		return -1;
	}

	if (!(card->host->caps & MMC_CAP_SD_HIGHSPEED))
		return -1;

	/* Check function */
	err = mmc_sd_switch(card, SD_SWITCH_CHECK, SD_SWITCH_GRP_ACCESS_MODE,
	                    SD_SWITCH_ACCESS_HS, p_sta);
	if (err)
		return err;

	if ((p_sta[16] & 0x0F) != 1) {
		SD_LOGW("%s: Problem switching card into high-speed mode!\n", __func__);
		err = -1;
	} else {
		err = 0;
	}

	return err;
}

static int32_t mmc_send_cxd_native(struct mmc_host *host, uint32_t arg, uint32_t *cxd, int32_t opcode)
{
	int32_t err;
	struct mmc_command cmd = {0};

	SD_BUG_ON(!host);
	SD_BUG_ON(!cxd);

	cmd.opcode = opcode;
	cmd.arg = arg;
	cmd.flags = MMC_RSP_R2 | MMC_CMD_AC;

	err = mmc_wait_for_cmd(host, &cmd);
	if (err)
		return err;

	memcpy(cxd, cmd.resp, sizeof(uint32_t) * 4);

	return 0;
}

int32_t mmc_send_csd(struct mmc_card *card, uint32_t *csd)
{
	return mmc_send_cxd_native(card->host, card->rca << 16,
				csd, MMC_SEND_CSD);
}

int32_t mmc_sd_get_csd(struct mmc_card *card)
{
	int32_t err;
	uint32_t csd[4] = {0};

	/* Fetch CSD from card. */
	err = mmc_send_csd(card, csd);
	if (err)
		return err;

	SD_LOGN("card raw csd:\n");
	sd_hex_dump_bytes((void *)csd, 16);

	err = mmc_decode_csd(card, csd);
	if (err)
		return err;

	return 0;
}

int32_t mmc_sd_setup_card(struct mmc_host *host, struct mmc_card *card)
{
	int32_t err;
	int32_t retries;
	uint32_t raw_scr[2] = {0};

	/* Fetch SCR from card. */
	err = mmc_app_send_scr(card, raw_scr);
	if (err)
		return err;

	SD_LOGN("card raw scr:\n");
	sd_hex_dump_bytes((void *)raw_scr, 8);

	err = mmc_decode_scr(card, raw_scr);
	if (err)
		return err;

	/* Fetch and process SD Status register. */
	err = mmc_read_ssr(card);
	if (err)
		return err;

	/* Fetch switch information from card. */
	for (retries = 1; retries <= 3; retries++) {
		err = mmc_read_switch(card);
		if (!err) {
			if (retries > 1) {
				SD_LOGW("%s: recovered\n", __func__);
			}
			break;
		} else {
			SD_LOGW("%s: read switch failed (attempt %d)\n",
			        __func__, retries);
		}
	}

	if (err)
		return err;

	return 0;
}

uint32_t mmc_sd_get_max_clock(struct mmc_card *card)
{
	uint32_t max_dtr = (uint32_t)-1;

	if (mmc_card_highspeed(card)) {
		if (max_dtr > card->sw_caps.hs_max_dtr)
			max_dtr = card->sw_caps.hs_max_dtr;
	} else if (max_dtr > card->csd.max_dtr) {
		max_dtr = card->csd.max_dtr;
	}

	return max_dtr;
}

/*
 * Handle the detection and initialisation of a card.
 *
 * In the case of a resume, "oldcard" will contain the card
 * we're trying to reinitialise.
 */
static int32_t mmc_sd_init_card(struct mmc_card *card, struct mmc_host *host)
{
	int32_t err = 0;

	/*
	 * I/O voltage should be 3.3 V here for the initialization needed.
	 */

	/* cmd2, send cid, check if card support 3.3V */
	err = mmc_all_send_cid(host, card->cidno);
	if (err) {
		SD_LOGW("Cards all send CID number failed !!\n");
		return -1;
	} else
		SD_LOGN("Card CID number:%x\n", card->cidno[0]);

	card->type = MMC_TYPE_SD;

	/* cmd3, For native busses: get card RCA and quit open drain mode. */
	err = mmc_send_relative_addr(card->host, &card->rca);
	if (err) {
		SD_LOGW("Card public new RCA failed !!\n");
		return -1;
	} else
		SD_LOGD("Card public new RCA:%x\n", card->rca);

	/* cmd10, get CID register */
	if (mmc_send_cid(card)) {
		SD_LOGW("Card send CID reg failed !!\n");
		return -1;
	}

	/* cmd9, get CSD register */
	if (mmc_sd_get_csd(card)) {
		SD_LOGW("Card send CSD reg failed !!\n");
		return -1;
	}

	/* cmd7, Select card  to standby state, as all following commands rely on that. */
	if (mmc_select_card(card, 1)) {
		SD_LOGW("mmc_select_card failed !!\n");
		return -1;
	}

	err = mmc_sd_setup_card(host, card);
	if (err)
		goto free_card;

	/* Initialization sequence for UHS-I cards */
	{
		uint32_t clk;
		uint32_t retries = 3;

		while (retries) {
			err = mmc_sd_switch_hs(card);
			if (err < 0) {
				SD_LOGE("%s: Re-switch hs, err %d (retries = %d)\n",
				        __func__, err, retries);
				mmc_mdelay(5);
				retries--;
				continue;
			}
			break;
		}

		if (err) {
			SD_LOGW("switch to high speed error, use DS: 25 MHz\n");
			clk = 25000000;
			HAL_SDC_Update_Clk(card->host, clk);
			err = 0;
		} else {
			mmc_card_set_highspeed(card);
			card->sd_bus_speed = HIGH_SPEED_BUS_SPEED;

			clk = mmc_sd_get_max_clock(card);
			HAL_SDC_Update_Clk(card->host, clk);
			SD_LOGN("card is switched to high speed mode, clk:%d KHz\n", clk/1000);
		}

		/* Switch to wider bus (if supported). */
		if ((host->caps & MMC_CAP_4_BIT_DATA) &&
		    (card->scr.bus_widths & SD_SCR_BUS_WIDTH_4)) {
			err = mmc_app_set_bus_width(card, MMC_BUS_WIDTH_4);
			if (err) {
				SD_LOGW("Set bus width error, use default 1 bit !!\n");
				return -1;
			}

			/* config SDMMC controller bus width */
			HAL_SDC_Set_BusWidth(card->host, MMC_BUS_WIDTH_4);
			SD_LOGN("Set bus width type: %d\n", MMC_BUS_WIDTH_4);
		}
	}

	mmc_add_card(card);
	card->host->card = card;

	return 0;

free_card:
	card->host->card = NULL;
	return err;
}

#ifdef CONFIG_SD_PM
static uint32_t mmc_sd_suspending;

static int32_t mmc_sd_suspend(struct mmc_host *host)
{
	mmc_sd_suspending = 1;
	mmc_card_deinit(host->card);
	SD_LOGD("%s ok\n", __func__);

	return 0;
}

static int32_t mmc_sd_resume(struct mmc_host *host)
{
	mmc_rescan(host->card, host->sdc_id);
	mmc_sd_suspending = 0;
	SD_LOGD("%s ok\n", __func__);

	return 0;
}

static const struct mmc_bus_ops sd_bus_ops = {
	.suspend = mmc_sd_suspend,
	.resume = mmc_sd_resume,
};
#endif

/*
 * Starting point for SD card init.
 */
int32_t mmc_attach_sd(struct mmc_card *card, struct mmc_host *host)
{
	int32_t err = 0;
	uint32_t ocr;

	SD_BUG_ON(!host);

	/* send cmd41/55 to check operation condition */
	err = mmc_send_app_op_cond(host, 0, &ocr);
	if (err) {
		return err;
	}
	SD_LOGN("card ocr: %08x\n", ocr);

	/*
	 * Sanity check the voltages that the card claims to
	 * support.
	 */
	if (ocr & 0x7F) {
		SD_LOGW("%s: card claims to support voltages below the defined range."
		        " These will be ignored.\n", __func__);
		ocr &= ~0x7F;
	}

	card->ocr.ocr = 0x7fffffff & ocr; /* set card not in busy state */

	err = mmc_sd_init_card(card, host);

#ifdef CONFIG_SD_PM
	if (!mmc_sd_suspending) {
		mmc_attach_bus(host, &sd_bus_ops);
	}
#endif

	return err;
}

void mmc_deattach_sd(struct mmc_card *card, struct mmc_host *host)
{
	mmc_select_card(host->card, 0);
	host->card->state &= ~MMC_STATE_HIGHSPEED;

#ifdef CONFIG_SD_PM
	if (!mmc_sd_suspending) {
		mmc_detach_bus(host);
	}
#endif
}

#endif /* CONFIG_USE_SD */
