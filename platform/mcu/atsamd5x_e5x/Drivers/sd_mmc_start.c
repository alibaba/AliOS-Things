/*
 * Code generated from Atmel Start.
 *
 * This file will be overwritten when reconfiguring your Atmel Start project.
 * Please copy examples or other code you want to keep to a separate file or main.c
 * to avoid loosing it when reconfiguring.
 */

#include "atmel_start.h"
#include "sd_mmc.h"

#include "conf_sd_mmc.h"

/* Card Detect (CD) pin settings */
static sd_mmc_detect_t SDMMC_ACCESS_0_cd[CONF_SD_MMC_MEM_CNT] = {

    {-1, CONF_SD_MMC_0_CD_DETECT_VALUE},
};

/* Write Protect (WP) pin settings */
static sd_mmc_detect_t SDMMC_ACCESS_0_wp[CONF_SD_MMC_MEM_CNT] = {

    {-1, CONF_SD_MMC_0_WP_DETECT_VALUE},
};

static uint8_t sd_mmc_block[512];

/*
 * Example
 */
void SDMMC_ACCESS_0_example(void)
{
	while (SD_MMC_OK != sd_mmc_check(0)) {
		/* Wait card ready. */
	}
	if (sd_mmc_get_type(0) & (CARD_TYPE_SD | CARD_TYPE_MMC)) {
		/* Read card block 0 */
		sd_mmc_init_read_blocks(0, 0, 1);
		sd_mmc_start_read_blocks(sd_mmc_block, 1);
		sd_mmc_wait_end_of_read_blocks(false);
	}
	if (sd_mmc_get_type(0) & CARD_TYPE_SDIO) {
		/* Read 22 bytes from SDIO Function 0 (CIA) */
		sdio_read_extended(0, 0, 0, 1, sd_mmc_block, 22);
	}
}

void sd_mmc_stack_init(void)
{

	sd_mmc_init(&IO_BUS, SDMMC_ACCESS_0_cd, SDMMC_ACCESS_0_wp);

	#if 1
    //printf("Card install checking\n\r");
    sd_mmc_err_t err;
	uint8_t slot = 0;
		do {
		//printf("log, sd_mmc_check\n\r", err);
		err = sd_mmc_check(slot);
		//printf("log, err=%d\n\r", err);
		if ((SD_MMC_ERR_NO_CARD != err)
		&& (SD_MMC_INIT_ONGOING != err)
		&& (SD_MMC_OK != err)) {
			//printf("Card install FAILED\n\r");
			//printf("Please unplug and re-plug the card.\n\r");
			delay_ms(1000);
		}
	} while (SD_MMC_OK != err);
	delay_ms(1000);
	//while (1) {};
    //printf("Card install success\n\r");
#endif
}
