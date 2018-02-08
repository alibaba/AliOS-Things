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

#include <sys/unistd.h>
#include "hal/soc/soc.h"
#include "hal/soc/sd.h"
#include "driver/chip/hal_def.h"
#include "driver/chip/sdmmc/hal_sdhost.h"
#include "driver/chip/sdmmc/sdmmc.h"

static struct mmc_card card;
static hal_sd_info_t sd_info;
static int sd_initialize = 0;

/**@brief Initialises a sd interface
*
* @param  sd       : the interface which should be initialised
* @param  config   : sd configuration structure
*
* @return    0     : on success.
* @return    EIO   : if an error occurred with any step
*/
int32_t hal_sd_init(sd_dev_t *sd)
{
	int32_t ret;
	SDC_InitTypeDef sdc_param = {0};

	if (!sd_initialize) {
		memset(&card, 0, sizeof(struct mmc_card));
		memset(&sd_info, 0, sizeof(hal_sd_info_t));

		sd->port = 0;
		HAL_SDC_Init(sd->port, &sdc_param);

		ret = mmc_rescan(&card, sd->port);
		if (ret != 0) {
			printf("[hal_sd]: init sd failed\n");
			return -1;
		}
	    sd_initialize = 1;
	}

	sd->config.bus_wide = card.bus_width;
	if (card.state & MMC_STATE_HIGHSPEED)
		sd->config.freq = 50000000;
	else
		sd->config.freq = 25000000;
	sd->priv = (void*)&card;
	sd_info.blk_size = 512;
	sd_info.blk_nums = card.csd.capacity/512;

	return 0;
}


/**@brief read sd blocks
*
* @param  sd       : the interface which should be initialised
* @param  data     : pointer to the buffer which will store incoming data
* @param  blk_addr : sd blk addr
* @param  blks     : sd blks
* @param  timeout  : timeout in milisecond
* @return    0     : on success.
* @return    EIO   : if an error occurred with any step
*/
int32_t hal_sd_blks_read(sd_dev_t *sd, uint8_t *data, uint32_t blk_addr, uint32_t blks, uint32_t timeout)
{
	int32_t ret;
	ret = mmc_block_read(sd->priv, data, blk_addr, blks);

	return ret;
}

/**@brief write sd blocks
*
* @param  sd       : the interface which should be initialised
* @param  data     : pointer to the buffer which will store incoming data
* @param  blk_addr : sd blk addr
* @param  blks     : sd blks
* @param  timeout  : timeout in milisecond
* @return    0     : on success.
* @return    EIO   : if an error occurred with any step
*/
int32_t hal_sd_blks_write(sd_dev_t *sd, uint8_t *data, uint32_t blk_addr, uint32_t blks, uint32_t timeout)
{
	int32_t ret;
	ret = mmc_block_write(sd->priv, data, blk_addr, blks);

	return ret;
}

/**@brief erase sd blocks
*
* @param  sd              : the interface which should be initialised
* @param  blk_start_addr  : sd blocks start addr
* @param  blk_end_addr    : sd blocks end addr
* @return    0            : on success.
* @return    EIO          : if an error occurred with any step
*/
int32_t hal_sd_erase(sd_dev_t *sd, uint32_t blk_start_addr, uint32_t blk_end_addr)
{
	//TODO:
	return 0;
}

/**@brief get sd state
*
* @param  sd       : the interface which should be initialised
* @param  stat     : pointer to the buffer which will store incoming data
* @return    0     : on success.
* @return    EIO   : if an error occurred with any step
*/
int32_t hal_sd_stat_get(sd_dev_t *sd, hal_sd_stat *stat)
{
	*stat = SD_STAT_TRANSFER;
	return 0;
}

/**@brief get sd info
*
* @param  sd       : the interface which should be initialised
* @param  stat     : pointer to the buffer which will store incoming data
* @return    0     : on success.
* @return    EIO   : if an error occurred with any step
*/
int32_t hal_sd_info_get(sd_dev_t *sd, hal_sd_info_t *info)
{
	info->blk_size = sd_info.blk_size;
	info->blk_nums = sd_info.blk_nums;
	return 0;
}

/**@brief Deinitialises a sd interface
*
* @param  sd       : the interface which should be initialised
* @return    0     : on success.
* @return    EIO   : if an error occurred with any step
*/
int32_t hal_sd_finalize(sd_dev_t *sd)
{
	sd_initialize = 0;
	if (!card.host)
		return 0;

	mmc_card_deinit(&card);
	HAL_SDC_Deinit(0);
	return 0;
}
