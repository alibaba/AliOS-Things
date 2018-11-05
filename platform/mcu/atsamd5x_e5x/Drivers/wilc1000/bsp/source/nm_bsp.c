/**
 *
 * \file
 *
 * \brief This module contains SAME54 BSP APIs implementation.
 *
 * Copyright (c) 2018 Atmel Corporation. All rights reserved.
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

#include "nm_bsp.h"
#include "common/include/nm_common.h"
#include "atmel_start.h"
#include "wilc_init.h"
#include "conf_wilc.h"
#include <stdlib.h>

static tpfNmBspIsr gpfIsr;

static void chip_isr(void)
{
	if (gpfIsr) {
		gpfIsr();
	}
}

/*
 *	@fn		nm_bsp_init
 *	@brief	Initialize BSP
 *	@return	0 in case of success and -1 in case of failure
 */
sint8 nm_bsp_init(void)
{
	gpfIsr = NULL;
	
	/* Perform chip reset. */
	nm_bsp_reset();	
	return M2M_SUCCESS;
}

/**
 *	@fn		nm_bsp_deinit
 *	@brief	De-iInitialize BSP
 *	@return	0 in case of success and -1 in case of failure
 */
sint8 nm_bsp_deinit(void)
{
	return M2M_SUCCESS;
}

/**
 *	@fn		nm_bsp_reset
 *	@brief	Reset WILC1000 SoC by setting CHIP_EN and RESET_N signals low,
 *           CHIP_EN high then RESET_N high
 */
void nm_bsp_reset(void)
{
	gpio_set_pin_level(CONF_WINC_PIN_CHIP_ENABLE, false);
	gpio_set_pin_level(CONF_WINC_PIN_RESET, false);
	nm_bsp_sleep(100);
	gpio_set_pin_level(CONF_WINC_PIN_CHIP_ENABLE, true);
	nm_bsp_sleep(100);
	gpio_set_pin_level(CONF_WINC_PIN_RESET, true);
	nm_bsp_sleep(100);
}

/*
 *	@fn		nm_bsp_sleep
 *	@brief	Sleep in units of mSec
 *	@param[IN]	u32TimeMsec
 *				Time in milliseconds
 */
void nm_bsp_sleep(uint32 u32TimeMsec)
{
	while (u32TimeMsec--) {
		delay_ms(1);
	}
}

/**
 * \internal Get the PIO hardware instance
 *
 * \param[in] pin The PIO pin
 *
 * \return The instance of PIO hardware
 */

/*
 *	@fn		nm_bsp_register_isr
 *	@brief	Register interrupt service routine
 *	@param[IN]	pfIsr
 *				Pointer to ISR handler
 */
void nm_bsp_register_isr(tpfNmBspIsr pfIsr)
{
	gpfIsr = pfIsr;

	ext_irq_register(CARD_CONF_WINC_EXT_INT_PIN, chip_isr);
}

/*
 *	@fn		nm_bsp_interrupt_ctrl
 *	@brief	Enable/Disable interrupts
 *	@param[IN]	u8Enable
 *				'0' disable interrupts. '1' enable interrupts
 */
void nm_bsp_interrupt_ctrl(uint8 u8Enable)
{

	_ext_irq_enable(CARD_CONF_WINC_EXT_INT_PIN, u8Enable);
}
/*
*	@fn		nm_bsp_malloc
*	@brief	Allocate memory
*	@param [in]   u32Size
*               Size of the requested memory
*	@return       Pointer to the allocated buffer, or NULL otherwise
*/
void* nm_bsp_malloc(uint32 u32Size)
{
	return malloc(u32Size);
}
/*
*	@fn		nm_bsp_free
*	@brief	Free memory
*	@param [in]   pvMemBuffer
*               Pointer to the buffer to be freed
*/
void nm_bsp_free(void* pvMemBuffer)
{
	free(pvMemBuffer);
}