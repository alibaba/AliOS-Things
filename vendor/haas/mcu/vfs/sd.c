/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */

#include <errno.h>
#include "aos/hal/sd.h"
#include "hal_sdmmc.h"
#include "hal_iomux.h"
#include "hal_iomux_best2001.h"
#include "hal_gpio.h"
#include "hal_trace.h"


// #define HAL_SD_DEBUG

static struct HAL_IOMUX_PIN_FUNCTION_MAP sd_pinmux[] = {
	{HAL_IOMUX_PIN_P1_2, HAL_IOMUX_FUNC_SDMMC_CMD, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_PULLUP_ENALBE},
	{HAL_IOMUX_PIN_P1_3, HAL_IOMUX_FUNC_SDMMC_CLK, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_PULLUP_ENALBE},
	{HAL_IOMUX_PIN_P1_4, HAL_IOMUX_FUNC_SDMMC_DATA0, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_PULLUP_ENALBE},
#if 0
	{HAL_IOMUX_PIN_P1_5, HAL_IOMUX_FUNC_SDMMC_DATA1, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_PULLUP_ENALBE},
	{HAL_IOMUX_PIN_P1_0, HAL_IOMUX_FUNC_SDMMC_DATA2, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_PULLUP_ENALBE},
	{HAL_IOMUX_PIN_P1_1, HAL_IOMUX_FUNC_SDMMC_DATA3, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_PULLUP_ENALBE},
#endif
};

#ifdef HAL_SD_DEBUG
static struct HAL_IOMUX_PIN_FUNCTION_MAP fix_board_error_pinmux[] = {
	/* just for bes mainboard */
	{HAL_IOMUX_PIN_P0_0, HAL_IOMUX_FUNC_GPIO, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_NOPULL},
	{HAL_IOMUX_PIN_P0_1, HAL_IOMUX_FUNC_GPIO, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_NOPULL},
	{HAL_IOMUX_PIN_P0_2, HAL_IOMUX_FUNC_GPIO, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_NOPULL},
	{HAL_IOMUX_PIN_P0_3, HAL_IOMUX_FUNC_GPIO, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_NOPULL},
	{HAL_IOMUX_PIN_P1_0, HAL_IOMUX_FUNC_GPIO, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_NOPULL},
	{HAL_IOMUX_PIN_P1_1, HAL_IOMUX_FUNC_GPIO, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_NOPULL},
};

void sd_card_fix_board_error()
{
	hal_iomux_init((struct HAL_IOMUX_PIN_FUNCTION_MAP *)fix_board_error_pinmux, sizeof(fix_board_error_pinmux) / sizeof(struct HAL_IOMUX_PIN_FUNCTION_MAP));
	for (int i = 0; i < sizeof(fix_board_error_pinmux) / sizeof(struct HAL_IOMUX_PIN_FUNCTION_MAP); i++)
	{
		hal_gpio_pin_set_dir(fix_board_error_pinmux[i].pin, HAL_GPIO_DIR_IN, 0);
	}
}
#endif

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

#ifdef HAL_SD_DEBUG
	sd_card_fix_board_error();
#endif

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

	if(blks == hal_sdmmc_read_blocks(HAL_SDMMC_ID_0, blk_addr, blks, data))
		return 0;
	else
		return EIO;
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

	if(blks == hal_sdmmc_write_blocks(HAL_SDMMC_ID_0, blk_addr, blks, data))
		return 0;
	else
		return EIO;
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

#ifdef HAL_SD_DEBUG
#include "hal_location.h"
static uint8_t write_buf[512] __attribute__((aligned(4)));
static uint8_t read_buf[512] __attribute__((aligned(4)));

static void dump_mem(uint8_t *mem, uint32_t count)
{
	uint8_t dump_count = 0;
	TRACE("dump %p start, count = %d", mem, count);
	while (count)
	{
		dump_count = count > 16 ? 16 : count;
		hal_trace_dump("0x%02x ", sizeof(uint8_t), dump_count, mem);
		count -= dump_count;
		mem += dump_count;
	}
	TRACE("dump end");
}

static int sd_wr_rst_check_and_reset(void)
{
	int i = 0;
	dump_mem(read_buf, 512);
	uint8_t op_error = 0;
	for (i = 0; i < 512; i++)
	{
		if (read_buf[i] != write_buf[i])
		{
			TRACE("ERROR: i=%d, write = %d, read = %d", i, write_buf[i], read_buf[i]);
			op_error = 1;
			break;
		}
	};

	if (op_error == 0)
	{
		TRACE("SUCC sdmmc read/write one block.");
	}

	memset(read_buf, 0, 512);
}

void sd_test()
{
	int ret = -1;
	sd_dev_t sd;
	memset(&sd, 0, sizeof(sd_dev_t));
	int i = 0;
	hal_sd_info_t info;
	memset(&info, 0, sizeof(hal_sd_info_t));

	uint8_t write_val = 0;
	for (i = 0; i < 512; i++)
	{
		write_buf[i] = write_val++;
	}
	dump_mem(write_buf, 512);

	uint32_t sector_count = 0;
	uint32_t sector_size = 0;
	int op_blk_addr = 31166976;
	int op_blk_count = 1;
	uint8_t *op_data_buf = NULL;

	TRACE("open ....");
	sd_card_fix_board_error();
	hal_iomux_init((struct HAL_IOMUX_PIN_FUNCTION_MAP *)sd_pinmux, sizeof(sd_pinmux) / sizeof(struct HAL_IOMUX_PIN_FUNCTION_MAP));
	ret = hal_sdmmc_open(HAL_SDMMC_ID_0);
	TRACE("open ret = %d", ret);

	TRACE("get_info ....");
	hal_sdmmc_info(HAL_SDMMC_ID_0, &sector_count, &sector_size);
	TRACE("get_info sector_count %d, sector_size %d", sector_count, sector_size);

	TRACE("read ....");
	op_data_buf = read_buf;
	ret = hal_sdmmc_read_blocks(HAL_SDMMC_ID_0, op_blk_addr, op_blk_count, op_data_buf);
	TRACE("read ret = %d, blk_addr = %d, blks = %d", ret, op_blk_addr, op_blk_count);
	dump_mem(op_data_buf, 16);

	TRACE("write ....");
	op_data_buf = write_buf;
	ret = hal_sdmmc_write_blocks(HAL_SDMMC_ID_0, op_blk_addr, op_blk_count, op_data_buf);
	TRACE("write ret = %d, blk_addr = %d, blks = %d", ret, op_blk_addr, op_blk_count);

	TRACE("write next block ....");
	op_blk_addr++;
	op_data_buf = write_buf;
	ret = hal_sdmmc_write_blocks(HAL_SDMMC_ID_0, op_blk_addr, op_blk_count, op_data_buf);
	TRACE("write ret = %d, blk_addr = %d, blks = %d", ret, op_blk_addr, op_blk_count);

	TRACE("read ....");
	op_data_buf = read_buf;
	ret = hal_sdmmc_read_blocks(HAL_SDMMC_ID_0, op_blk_addr, op_blk_count, op_data_buf);
	TRACE("read ret = %d, blk_addr = %d, blks = %d", ret, op_blk_addr, op_blk_count);
	sd_wr_rst_check_and_reset();

	TRACE("read 2....");
	op_data_buf = read_buf;
	ret = hal_sdmmc_read_blocks(HAL_SDMMC_ID_0, op_blk_addr, op_blk_count, op_data_buf);
	TRACE("read ret = %d, blk_addr = %d, blks = %d", ret, op_blk_addr, op_blk_count);
	sd_wr_rst_check_and_reset();

	TRACE("write next block ....");
	op_blk_addr++;
	op_data_buf = write_buf;
	ret = hal_sdmmc_write_blocks(HAL_SDMMC_ID_0, op_blk_addr, op_blk_count, op_data_buf);
	TRACE("write ret = %d, blk_addr = %d, blks = %d", ret, op_blk_addr, op_blk_count);

	TRACE("read next block ....");
	op_data_buf = read_buf;
	ret = hal_sdmmc_read_blocks(HAL_SDMMC_ID_0, op_blk_addr, op_blk_count, op_data_buf);
	TRACE("read ret = %d, blk_addr = %d, blks = %d", ret, op_blk_addr, op_blk_count);
	sd_wr_rst_check_and_reset();

	TRACE("read next block 2....");
	op_data_buf = read_buf;
	ret = hal_sdmmc_read_blocks(HAL_SDMMC_ID_0, op_blk_addr, op_blk_count, op_data_buf);
	TRACE("read ret = %d, blk_addr = %d, blks = %d", ret, op_blk_addr, op_blk_count);
	sd_wr_rst_check_and_reset();

	TRACE("close ....");
	hal_sdmmc_close(HAL_SDMMC_ID_0);
}
#endif