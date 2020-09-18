/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <errno.h>
#include "aos/hal/sd.h"
#include "hal_sdmmc.h"
#include "hal_iomux.h"
#include "hal_iomux_haas1000.h"
#include "hal_gpio.h"
#include "hal_trace.h"

static struct HAL_IOMUX_PIN_FUNCTION_MAP sd_pinmux[] = {
	{HAL_IOMUX_PIN_P1_2, HAL_IOMUX_FUNC_SDMMC_CMD, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_PULLUP_ENALBE},
	{HAL_IOMUX_PIN_P1_3, HAL_IOMUX_FUNC_SDMMC_CLK, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_PULLUP_ENALBE},
	{HAL_IOMUX_PIN_P1_4, HAL_IOMUX_FUNC_SDMMC_DATA0, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_PULLUP_ENALBE},
	{HAL_IOMUX_PIN_P1_5, HAL_IOMUX_FUNC_SDMMC_DATA1, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_PULLUP_ENALBE},
	{HAL_IOMUX_PIN_P1_0, HAL_IOMUX_FUNC_SDMMC_DATA2, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_PULLUP_ENALBE},
	{HAL_IOMUX_PIN_P1_1, HAL_IOMUX_FUNC_SDMMC_DATA3, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_PULLUP_ENALBE},
};

static int sd_initialised = 0;

/**
 * Initialises a sd interface
 *
 * @param[in]  sd  the interface which should be initialised
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_sd_init(sd_dev_t *sd)
{
	int ret;

	hal_iomux_init((struct HAL_IOMUX_PIN_FUNCTION_MAP *)sd_pinmux, sizeof(sd_pinmux) / sizeof(struct HAL_IOMUX_PIN_FUNCTION_MAP));

	ret = hal_sdmmc_open(HAL_SDMMC_ID_0);

	if (0 == ret)
	{
		sd_initialised = 1;
		if (sd)
		{
			sd->port = HAL_SDMMC_ID_0;
			sd->config.bus_wide = hal_sdmmc_get_bus_width(HAL_SDMMC_ID_0);
			sd->config.freq = hal_sdmmc_get_clock(HAL_SDMMC_ID_0);
			sd->priv = NULL;
		}
	} else {
        printf("sd init failed!\n");
    }

	return ret;
}

/**
 * Read sd blocks
 *
 * @param[in]   sd        the interface which should be initialised
 * @param[out]  data      pointer to the buffer which will store incoming data
 * @param[in]   blk_addr  sd blk addr
 * @param[in]   blks      sd blks
 * @param[in]   timeout   timeout in milisecond
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_sd_blks_read(sd_dev_t *sd, uint8_t *data, uint32_t blk_addr,
						 uint32_t blks, uint32_t timeout)
{
	if (data == NULL || 0 == sd_initialised)
		return EIO;

	if(blks == hal_sdmmc_read_blocks(HAL_SDMMC_ID_0, blk_addr, blks, data)) {
		return 0;
    } else {
        printf("sd read blocks failed\n");
		return EIO;
    }
}

/**
 * Write sd blocks
 *
 * @param[in]  sd        the interface which should be initialised
 * @param[in]  data      pointer to the buffer which will store incoming data
 * @param[in]  blk_addr  sd blk addr
 * @param[in]  blks      sd blks
 * @param[in]  timeout   timeout in milisecond
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_sd_blks_write(sd_dev_t *sd, uint8_t *data, uint32_t blk_addr,
						  uint32_t blks, uint32_t timeout)
{
	if (data == NULL || 0 == sd_initialised)
		return EIO;

	if(blks == hal_sdmmc_write_blocks(HAL_SDMMC_ID_0, blk_addr, blks, data)) {
        return 0;
    } else {
        printf("sd write blocks failed\n");
		return EIO;
    }
}

/**
 * Erase sd blocks
 *
 * @param[in]  sd              the interface which should be initialised
 * @param[in]  blk_start_addr  sd blocks start addr
 * @param[in]  blk_end_addr    sd blocks end addr
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_sd_erase(sd_dev_t *sd, uint32_t blk_start_addr, uint32_t blk_end_addr)
{
	return 0;
}

/**
 * Get sd state
 *
 * @param[in]   sd    the interface which should be initialised
 * @param[out]  stat  pointer to the buffer which will store incoming data
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_sd_stat_get(sd_dev_t *sd, hal_sd_stat *stat)
{
	if(sd_initialised)
		*stat = SD_STAT_TRANSFER;
	else
		*stat = SD_STAT_RESET;
	return 0;
}

/**
 * Get sd info
 *
 * @param[in]   sd    the interface which should be initialised
 * @param[out]  stat  pointer to the buffer which will store incoming data
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_sd_info_get(sd_dev_t *sd, hal_sd_info_t *info)
{
	if (info == NULL || 0 == sd_initialised)
		return EIO;

	uint32_t sector_count = 0;
	uint32_t sector_size = 0;

	hal_sdmmc_info(HAL_SDMMC_ID_0, &sector_count, &sector_size);
	info->blk_nums = sector_count;
	info->blk_size = sector_size;
	return 0;
}

/**
 * Deinitialises a sd interface
 *
 * @param[in]  sd  the interface which should be initialised
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_sd_finalize(sd_dev_t *sd)
{
	if (0 == sd_initialised)
		return 0;

	hal_sdmmc_close(HAL_SDMMC_ID_0);
	sd_initialised = 0;
	return 0;
}
