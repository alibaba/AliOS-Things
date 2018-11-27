/**
 *
 * \file
 *
 * \brief This module contains WILC1000 bus wrapper APIs implementation.
 *
 * Copyright (c) 2015 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \asf_license_stop
 *
 */

#include <stdio.h>
#include "bsp/include/nm_bsp.h"
#include "common/include/nm_common.h"
#include "bus_wrapper/include/nm_bus_wrapper.h"
#include "atmel_start.h"
#ifdef CONF_WILC_USE_SDIO
#include "bus_wrapper/include/sdio_same54.h"
#endif
#include "wilc_init.h"

#define NM_BUS_MAX_TRX_SZ 4096
tstrNmBusCapabilities egstrNmBusCapabilities = {NM_BUS_MAX_TRX_SZ};

#ifdef defined CONF_WILC_USE_SPI
extern struct spi_m_sync_descriptor SPI_0;
struct io_descriptor *              io;
struct spi_m_sync_descriptor *      spi_instance;



void set_winc_spi_descriptor(struct spi_m_sync_descriptor *spi_inst)
{
	spi_instance = spi_inst;
}

static sint8 spi_rw(uint8 *pu8Mosi, uint8 *pu8Miso, uint16 u16Sz)
{
	uint8 u8Dummy    = 0;
	uint8 u8SkipMosi = 0, u8SkipMiso = 0;

	if (!pu8Mosi) {
		pu8Mosi    = &u8Dummy;
		u8SkipMosi = 1;
	} else if (!pu8Miso) {
		pu8Miso    = &u8Dummy;
		u8SkipMiso = 1;
	} else {
		return M2M_ERR_BUS_FAIL;
	}

	gpio_set_pin_level(CONF_WINC_PIN_CHIP_SELECT, false);
	if (!u8SkipMiso) {
		io_read(io, pu8Miso, u16Sz);
	}
	if (!u8SkipMosi) {
		io_write(io, pu8Mosi, u16Sz);
	}
	gpio_set_pin_level(CONF_WINC_PIN_CHIP_SELECT, true);

	return M2M_SUCCESS;
}
#endif
/*
 *	@fn		nm_bus_init
 *	@brief	Initialize the bus wrapper
 *	@return	M2M_SUCCESS in case of success and M2M_ERR_BUS_FAIL in case of failure
 */
sint8 nm_bus_init(void *pvinit)
{
	sint8 result = M2M_SUCCESS;
#ifdef defined CONF_WILC_USE_SPI
	spi_m_sync_get_io_descriptor(spi_instance, &io);
	spi_m_sync_enable(spi_instance);

	nm_bsp_reset();
	nm_bsp_sleep(1);
#elif defined CONF_WILC_USE_SDIO
	result = same54_sdio_init();
#endif
	return result;
}

/*
 *	@fn		nm_bus_ioctl
 *	@brief	send/receive from the bus
 *	@param[IN]	u8Cmd
 *					IOCTL command for the operation
 *	@param[IN]	pvParameter
 *					Arbitrary parameter depenging on IOCTL
 *	@return	M2M_SUCCESS in case of success and M2M_ERR_BUS_FAIL in case of failure
 *	@note	For SPI only, it's important to be able to send/receive at the same time
 */
sint8 nm_bus_ioctl(uint8 u8Cmd, void *pvParameter)
{
	sint8 s8Ret = 0;
	switch (u8Cmd) {
#ifdef CONF_WILC_USE_SPI
	case NM_BUS_IOCTL_RW: 
	{
		tstrNmSpiRw *pstrParam = (tstrNmSpiRw *)pvParameter;
		s8Ret                  = spi_rw(pstrParam->pu8InBuf, pstrParam->pu8OutBuf, pstrParam->u16Sz);
	} 
	break;
#elif defined CONF_WILC_USE_SDIO
		case NM_BUS_IOCTL_CMD_52: 
		{
			tstrNmSdioCmd52* pstrParam = (tstrNmSdioCmd52 *)pvParameter;
			s8Ret = nmi_sdio_cmd52(pstrParam);
		}
		break;
		case NM_BUS_IOCTL_CMD_53: 
		{
			tstrNmSdioCmd53* pstrParam = (tstrNmSdioCmd53 *)pvParameter;
			s8Ret = nmi_sdio_cmd53(pstrParam);
		}
		break;
#endif
	default:
		s8Ret = -1;
		M2M_ERR("invalide ioclt cmd\n");
		break;
	}

	return s8Ret;
}

/*
 *	@fn		nm_bus_deinit
 *	@brief	De-initialize the bus wrapper
 */
sint8 nm_bus_deinit(void)
{
	sint8 result = 0;
	return result;
}
/*
 *	@fn			nm_bus_reinit
 *	@brief		re-initialize the bus wrapper
 *	@param [in]	void *config
 *					re-init configuration data
 *	@return		M2M_SUCCESS in case of success and M2M_ERR_BUS_FAIL in case of failure
 */
sint8 nm_bus_reinit(void *config)
{
	return M2M_SUCCESS;
}
