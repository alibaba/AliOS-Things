#ifndef _SDIO_HOST_H
#define _SDIO_HOST_H
#include "basic_types.h"
#include "rtl8195a_sdio_host.h"

typedef enum{
	SDIO_INIT_NONE = -1,
	SDIO_INIT_FAIL = 0,
	SDIO_INIT_OK = 1,
	SDIO_SD_NONE = 2,
	SDIO_SD_OK = 3,
}_sdio_init_s;


s8 sdio_init_host(void);	// init sdio host interface
void sdio_deinit_host(void);

s8 sdio_sd_init(void);	// init sd card through sdio
void sdio_sd_deinit(void);	//de-init sd card through sdio
s8 sdio_sd_status(void);
u32 sdio_sd_getCapacity(void);
s8 sdio_sd_getProtection(void);
s8 sdio_sd_setProtection(bool protection);
s8 sdio_sd_getCSD(u8* CSD);
s8 sdio_sd_isReady();
s8 sdio_sd_setClock(SD_CLK_FREQUENCY SDCLK);


s8 sdio_read_blocks(u32 sector, u8 *buffer, u32 count);
s8 sdio_write_blocks(u32 sector, const u8 *buffer, u32 count);

#endif
