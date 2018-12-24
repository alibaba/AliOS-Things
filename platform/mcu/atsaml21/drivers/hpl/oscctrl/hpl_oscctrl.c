
/**
 * \file
 *
 * \brief SAM Oscillators Controller.
 *
 * Copyright (C) 2015 - 2018 Atmel Corporation. All rights reserved.
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
 * 4. This software may only be redistributed and used in connection with an
 *    Atmel microcontroller product.
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
#include <compiler.h>
#include <hpl_init.h>
#include <hpl_oscctrl_config.h>

/**
 * \brief Initialize clock sources
 */
void _oscctrl_init_sources(void)
{
	void *hw = (void *)OSCCTRL;

#if CONF_XOSC_CONFIG == 1
	hri_oscctrl_write_XOSCCTRL_reg(hw,
	                               OSCCTRL_XOSCCTRL_STARTUP(CONF_XOSC_STARTUP) | (0 << OSCCTRL_XOSCCTRL_AMPGC_Pos)
	                                   | OSCCTRL_XOSCCTRL_GAIN(CONF_XOSC_GAIN)
	                                   | (0 << OSCCTRL_XOSCCTRL_ONDEMAND_Pos)
	                                   | (CONF_XOSC_RUNSTDBY << OSCCTRL_XOSCCTRL_RUNSTDBY_Pos)
	                                   | (CONF_XOSC_XTALEN << OSCCTRL_XOSCCTRL_XTALEN_Pos)
	                                   | (CONF_XOSC_ENABLE << OSCCTRL_XOSCCTRL_ENABLE_Pos));

#endif

#if CONF_OSC16M_CONFIG == 1
	hri_oscctrl_write_OSC16MCTRL_reg(hw,
	                                 (CONF_OSC16M_ONDEMAND << OSCCTRL_OSC16MCTRL_ONDEMAND_Pos)
	                                     | (CONF_OSC16M_RUNSTDBY << OSCCTRL_OSC16MCTRL_RUNSTDBY_Pos)
	                                     | (CONF_OSC16M_ENABLE << OSCCTRL_OSC16MCTRL_ENABLE_Pos)
	                                     | OSCCTRL_OSC16MCTRL_FSEL(CONF_OSC16M_FSEL));
#endif

#if CONF_XOSC_CONFIG == 1
#if CONF_XOSC_ENABLE == 1
	while (!hri_oscctrl_get_STATUS_XOSCRDY_bit(hw))
		;
#endif
#if CONF_XOSC_AMPGC == 1
	hri_oscctrl_set_XOSCCTRL_AMPGC_bit(hw);
#endif
#if CONF_XOSC_ONDEMAND == 1
	hri_oscctrl_set_XOSCCTRL_ONDEMAND_bit(hw);
#endif
#endif

#if CONF_OSC16M_CONFIG == 1
#if CONF_OSC16M_ENABLE == 1
	while (!hri_oscctrl_get_STATUS_OSC16MRDY_bit(hw))
		;
#endif
#if CONF_OSC16M_ONDEMAND == 1
	hri_oscctrl_set_OSC16MCTRL_ONDEMAND_bit(hw);
#endif
#endif
	(void)hw;
}

void _oscctrl_init_referenced_generators(void)
{
	void *                     hw  = (void *)OSCCTRL;
	hri_oscctrl_dfllctrl_reg_t tmp = 0;

#if CONF_DFLL_CONFIG == 1
#if CONF_DFLL_OVERWRITE_CALIBRATION == 0
#define NVM_DFLL_COARSE_POS 26
#define NVM_DFLL_COARSE_SIZE 6
	uint32_t coarse;
	coarse = *((uint32_t *)(NVMCTRL_OTP5)) >> NVM_DFLL_COARSE_POS;
#endif
#if CONF_DFLL_USBCRM != 1 && CONF_DFLL_MODE != 0
	hri_gclk_write_PCHCTRL_reg(GCLK, 0, (1 << GCLK_PCHCTRL_CHEN_Pos) | GCLK_PCHCTRL_GEN(CONF_DFLL_GCLK));
#endif
	hri_oscctrl_write_DFLLCTRL_reg(hw, OSCCTRL_DFLLCTRL_ENABLE);
	while (!hri_oscctrl_get_STATUS_DFLLRDY_bit(hw))
		;
	hri_oscctrl_write_DFLLMUL_reg(hw,
	                              OSCCTRL_DFLLMUL_CSTEP(CONF_DFLL_CSTEP) | OSCCTRL_DFLLMUL_FSTEP(CONF_DFLL_FSTEP)
	                                  | OSCCTRL_DFLLMUL_MUL(CONF_DFLL_MUL));
	while (!hri_oscctrl_get_STATUS_DFLLRDY_bit(hw))
		;

#if CONF_DFLL_OVERWRITE_CALIBRATION == 0
	/* FINE is set to fixed value, which defined by DFLL48M Characteristics */
	hri_oscctrl_write_DFLLVAL_reg(hw, OSCCTRL_DFLLVAL_COARSE(coarse) | OSCCTRL_DFLLVAL_FINE(512));
#else
	hri_oscctrl_write_DFLLVAL_reg(hw, OSCCTRL_DFLLVAL_COARSE(CONF_DFLL_COARSE) | OSCCTRL_DFLLVAL_FINE(CONF_DFLL_FINE));
#endif

	tmp = (CONF_DFLL_WAITLOCK << OSCCTRL_DFLLCTRL_WAITLOCK_Pos) | (CONF_DFLL_BPLCKC << OSCCTRL_DFLLCTRL_BPLCKC_Pos)
	      | (CONF_DFLL_QLDIS << OSCCTRL_DFLLCTRL_QLDIS_Pos) | (CONF_DFLL_CCDIS << OSCCTRL_DFLLCTRL_CCDIS_Pos)
	      | (CONF_DFLL_RUNSTDBY << OSCCTRL_DFLLCTRL_RUNSTDBY_Pos) | (CONF_DFLL_USBCRM << OSCCTRL_DFLLCTRL_USBCRM_Pos)
	      | (CONF_DFLL_LLAW << OSCCTRL_DFLLCTRL_LLAW_Pos) | (CONF_DFLL_STABLE << OSCCTRL_DFLLCTRL_STABLE_Pos)
	      | (CONF_DFLL_MODE << OSCCTRL_DFLLCTRL_MODE_Pos) | (CONF_DFLL_ENABLE << OSCCTRL_DFLLCTRL_ENABLE_Pos);
	hri_oscctrl_write_DFLLCTRL_reg(hw, tmp);

#endif

#if CONF_DPLL_CONFIG == 1
#if CONF_DPLL_REFCLK == 2
	hri_gclk_write_PCHCTRL_reg(GCLK, 1, (1 << GCLK_PCHCTRL_CHEN_Pos) | GCLK_PCHCTRL_GEN(CONF_DPLL_GCLK));
#endif
	hri_oscctrl_write_DPLLRATIO_reg(
	    hw, OSCCTRL_DPLLRATIO_LDRFRAC(CONF_DPLL_LDRFRAC) | OSCCTRL_DPLLRATIO_LDR(CONF_DPLL_LDR));
	hri_oscctrl_write_DPLLCTRLB_reg(hw,
	                                OSCCTRL_DPLLCTRLB_DIV(CONF_DPLL_DIV)
	                                    | (CONF_DPLL_LBYPASS << OSCCTRL_DPLLCTRLB_LBYPASS_Pos)
	                                    | OSCCTRL_DPLLCTRLB_LTIME(CONF_DPLL_LTIME)
	                                    | OSCCTRL_DPLLCTRLB_REFCLK(CONF_DPLL_REFCLK)
	                                    | (CONF_DPLL_WUF << OSCCTRL_DPLLCTRLB_WUF_Pos)
	                                    | (CONF_DPLL_LPEN << OSCCTRL_DPLLCTRLB_LPEN_Pos)
	                                    | OSCCTRL_DPLLCTRLB_FILTER(CONF_DPLL_FILTER));
	hri_oscctrl_write_DPLLPRESC_reg(hw, OSCCTRL_DPLLPRESC_PRESC(CONF_DPLL_PRESC));
	hri_oscctrl_write_DPLLCTRLA_reg(hw,
	                                (0 << OSCCTRL_DPLLCTRLA_ONDEMAND_Pos)
	                                    | (CONF_DPLL_RUNSTDBY << OSCCTRL_DPLLCTRLA_RUNSTDBY_Pos)
	                                    | (CONF_DPLL_ENABLE << OSCCTRL_DPLLCTRLA_ENABLE_Pos));
#endif

#if CONF_DFLL_CONFIG == 1
	if (hri_oscctrl_get_DFLLCTRL_MODE_bit(hw)) {
		hri_oscctrl_status_reg_t status_mask = OSCCTRL_STATUS_DFLLRDY | OSCCTRL_STATUS_DFLLLCKC;

		while (hri_oscctrl_get_STATUS_reg(hw, status_mask) != status_mask)
			;
	} else {
		while (!hri_oscctrl_get_STATUS_DFLLRDY_bit(hw))
			;
	}
#if CONF_DFLL_ONDEMAND == 1
	hri_oscctrl_set_DFLLCTRL_ONDEMAND_bit(hw);
#endif
#endif

#if CONF_DPLL_CONFIG == 1
#if CONF_DPLL_ENABLE == 1
	while (!(hri_oscctrl_get_DPLLSTATUS_LOCK_bit(hw) || hri_oscctrl_get_DPLLSTATUS_CLKRDY_bit(hw)))
		;
#endif
#if CONF_DPLL_ONDEMAND == 1
	hri_oscctrl_set_DPLLCTRLA_ONDEMAND_bit(hw);
#endif
#endif

#if CONF_DFLL_CONFIG == 1
	while (hri_gclk_read_SYNCBUSY_reg(GCLK))
		;
#endif
	(void)hw, (void)tmp;
}
