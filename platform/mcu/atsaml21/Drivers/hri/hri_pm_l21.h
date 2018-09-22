/**
 * \file
 *
 * \brief SAM PM
 *
 * Copyright (C) 2017 Atmel Corporation. All rights reserved.
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
 */

#ifdef _SAML21_PM_COMPONENT_
#ifndef _HRI_PM_L21_H_INCLUDED_
#define _HRI_PM_L21_H_INCLUDED_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <hal_atomic.h>

#if defined(ENABLE_PM_CRITICAL_SECTIONS)
#define PM_CRITICAL_SECTION_ENTER() CRITICAL_SECTION_ENTER()
#define PM_CRITICAL_SECTION_LEAVE() CRITICAL_SECTION_LEAVE()
#else
#define PM_CRITICAL_SECTION_ENTER()
#define PM_CRITICAL_SECTION_LEAVE()
#endif

typedef uint16_t hri_pm_stdbycfg_reg_t;
typedef uint8_t  hri_pm_ctrla_reg_t;
typedef uint8_t  hri_pm_intenset_reg_t;
typedef uint8_t  hri_pm_intflag_reg_t;
typedef uint8_t  hri_pm_plcfg_reg_t;
typedef uint8_t  hri_pm_pwsakdly_reg_t;
typedef uint8_t  hri_pm_sleepcfg_reg_t;

static inline bool hri_pm_get_INTFLAG_PLRDY_bit(const void *const hw)
{
	return (((Pm *)hw)->INTFLAG.reg & PM_INTFLAG_PLRDY) >> PM_INTFLAG_PLRDY_Pos;
}

static inline void hri_pm_clear_INTFLAG_PLRDY_bit(const void *const hw)
{
	((Pm *)hw)->INTFLAG.reg = PM_INTFLAG_PLRDY;
}

static inline bool hri_pm_get_interrupt_PLRDY_bit(const void *const hw)
{
	return (((Pm *)hw)->INTFLAG.reg & PM_INTFLAG_PLRDY) >> PM_INTFLAG_PLRDY_Pos;
}

static inline void hri_pm_clear_interrupt_PLRDY_bit(const void *const hw)
{
	((Pm *)hw)->INTFLAG.reg = PM_INTFLAG_PLRDY;
}

static inline hri_pm_intflag_reg_t hri_pm_get_INTFLAG_reg(const void *const hw, hri_pm_intflag_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Pm *)hw)->INTFLAG.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_pm_intflag_reg_t hri_pm_read_INTFLAG_reg(const void *const hw)
{
	return ((Pm *)hw)->INTFLAG.reg;
}

static inline void hri_pm_clear_INTFLAG_reg(const void *const hw, hri_pm_intflag_reg_t mask)
{
	((Pm *)hw)->INTFLAG.reg = mask;
}

static inline void hri_pm_set_INTEN_PLRDY_bit(const void *const hw)
{
	((Pm *)hw)->INTENSET.reg = PM_INTENSET_PLRDY;
}

static inline bool hri_pm_get_INTEN_PLRDY_bit(const void *const hw)
{
	return (((Pm *)hw)->INTENSET.reg & PM_INTENSET_PLRDY) >> PM_INTENSET_PLRDY_Pos;
}

static inline void hri_pm_write_INTEN_PLRDY_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((Pm *)hw)->INTENCLR.reg = PM_INTENSET_PLRDY;
	} else {
		((Pm *)hw)->INTENSET.reg = PM_INTENSET_PLRDY;
	}
}

static inline void hri_pm_clear_INTEN_PLRDY_bit(const void *const hw)
{
	((Pm *)hw)->INTENCLR.reg = PM_INTENSET_PLRDY;
}

static inline void hri_pm_set_INTEN_reg(const void *const hw, hri_pm_intenset_reg_t mask)
{
	((Pm *)hw)->INTENSET.reg = mask;
}

static inline hri_pm_intenset_reg_t hri_pm_get_INTEN_reg(const void *const hw, hri_pm_intenset_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Pm *)hw)->INTENSET.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_pm_intenset_reg_t hri_pm_read_INTEN_reg(const void *const hw)
{
	return ((Pm *)hw)->INTENSET.reg;
}

static inline void hri_pm_write_INTEN_reg(const void *const hw, hri_pm_intenset_reg_t data)
{
	((Pm *)hw)->INTENSET.reg = data;
	((Pm *)hw)->INTENCLR.reg = ~data;
}

static inline void hri_pm_clear_INTEN_reg(const void *const hw, hri_pm_intenset_reg_t mask)
{
	((Pm *)hw)->INTENCLR.reg = mask;
}

static inline void hri_pm_set_CTRLA_IORET_bit(const void *const hw)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->CTRLA.reg |= PM_CTRLA_IORET;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_pm_get_CTRLA_IORET_bit(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Pm *)hw)->CTRLA.reg;
	tmp = (tmp & PM_CTRLA_IORET) >> PM_CTRLA_IORET_Pos;
	return (bool)tmp;
}

static inline void hri_pm_write_CTRLA_IORET_bit(const void *const hw, bool value)
{
	uint8_t tmp;
	PM_CRITICAL_SECTION_ENTER();
	tmp = ((Pm *)hw)->CTRLA.reg;
	tmp &= ~PM_CTRLA_IORET;
	tmp |= value << PM_CTRLA_IORET_Pos;
	((Pm *)hw)->CTRLA.reg = tmp;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_clear_CTRLA_IORET_bit(const void *const hw)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->CTRLA.reg &= ~PM_CTRLA_IORET;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_toggle_CTRLA_IORET_bit(const void *const hw)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->CTRLA.reg ^= PM_CTRLA_IORET;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_set_CTRLA_reg(const void *const hw, hri_pm_ctrla_reg_t mask)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->CTRLA.reg |= mask;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline hri_pm_ctrla_reg_t hri_pm_get_CTRLA_reg(const void *const hw, hri_pm_ctrla_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Pm *)hw)->CTRLA.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_pm_write_CTRLA_reg(const void *const hw, hri_pm_ctrla_reg_t data)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->CTRLA.reg = data;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_clear_CTRLA_reg(const void *const hw, hri_pm_ctrla_reg_t mask)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->CTRLA.reg &= ~mask;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_toggle_CTRLA_reg(const void *const hw, hri_pm_ctrla_reg_t mask)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->CTRLA.reg ^= mask;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline hri_pm_ctrla_reg_t hri_pm_read_CTRLA_reg(const void *const hw)
{
	return ((Pm *)hw)->CTRLA.reg;
}

static inline void hri_pm_set_SLEEPCFG_SLEEPMODE_bf(const void *const hw, hri_pm_sleepcfg_reg_t mask)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->SLEEPCFG.reg |= PM_SLEEPCFG_SLEEPMODE(mask);
	PM_CRITICAL_SECTION_LEAVE();
}

static inline hri_pm_sleepcfg_reg_t hri_pm_get_SLEEPCFG_SLEEPMODE_bf(const void *const hw, hri_pm_sleepcfg_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Pm *)hw)->SLEEPCFG.reg;
	tmp = (tmp & PM_SLEEPCFG_SLEEPMODE(mask)) >> PM_SLEEPCFG_SLEEPMODE_Pos;
	return tmp;
}

static inline void hri_pm_write_SLEEPCFG_SLEEPMODE_bf(const void *const hw, hri_pm_sleepcfg_reg_t data)
{
	uint8_t tmp;
	PM_CRITICAL_SECTION_ENTER();
	tmp = ((Pm *)hw)->SLEEPCFG.reg;
	tmp &= ~PM_SLEEPCFG_SLEEPMODE_Msk;
	tmp |= PM_SLEEPCFG_SLEEPMODE(data);
	((Pm *)hw)->SLEEPCFG.reg = tmp;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_clear_SLEEPCFG_SLEEPMODE_bf(const void *const hw, hri_pm_sleepcfg_reg_t mask)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->SLEEPCFG.reg &= ~PM_SLEEPCFG_SLEEPMODE(mask);
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_toggle_SLEEPCFG_SLEEPMODE_bf(const void *const hw, hri_pm_sleepcfg_reg_t mask)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->SLEEPCFG.reg ^= PM_SLEEPCFG_SLEEPMODE(mask);
	PM_CRITICAL_SECTION_LEAVE();
}

static inline hri_pm_sleepcfg_reg_t hri_pm_read_SLEEPCFG_SLEEPMODE_bf(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Pm *)hw)->SLEEPCFG.reg;
	tmp = (tmp & PM_SLEEPCFG_SLEEPMODE_Msk) >> PM_SLEEPCFG_SLEEPMODE_Pos;
	return tmp;
}

static inline void hri_pm_set_SLEEPCFG_reg(const void *const hw, hri_pm_sleepcfg_reg_t mask)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->SLEEPCFG.reg |= mask;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline hri_pm_sleepcfg_reg_t hri_pm_get_SLEEPCFG_reg(const void *const hw, hri_pm_sleepcfg_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Pm *)hw)->SLEEPCFG.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_pm_write_SLEEPCFG_reg(const void *const hw, hri_pm_sleepcfg_reg_t data)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->SLEEPCFG.reg = data;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_clear_SLEEPCFG_reg(const void *const hw, hri_pm_sleepcfg_reg_t mask)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->SLEEPCFG.reg &= ~mask;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_toggle_SLEEPCFG_reg(const void *const hw, hri_pm_sleepcfg_reg_t mask)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->SLEEPCFG.reg ^= mask;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline hri_pm_sleepcfg_reg_t hri_pm_read_SLEEPCFG_reg(const void *const hw)
{
	return ((Pm *)hw)->SLEEPCFG.reg;
}

static inline void hri_pm_set_PLCFG_PLDIS_bit(const void *const hw)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->PLCFG.reg |= PM_PLCFG_PLDIS;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_pm_get_PLCFG_PLDIS_bit(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Pm *)hw)->PLCFG.reg;
	tmp = (tmp & PM_PLCFG_PLDIS) >> PM_PLCFG_PLDIS_Pos;
	return (bool)tmp;
}

static inline void hri_pm_write_PLCFG_PLDIS_bit(const void *const hw, bool value)
{
	uint8_t tmp;
	PM_CRITICAL_SECTION_ENTER();
	tmp = ((Pm *)hw)->PLCFG.reg;
	tmp &= ~PM_PLCFG_PLDIS;
	tmp |= value << PM_PLCFG_PLDIS_Pos;
	((Pm *)hw)->PLCFG.reg = tmp;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_clear_PLCFG_PLDIS_bit(const void *const hw)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->PLCFG.reg &= ~PM_PLCFG_PLDIS;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_toggle_PLCFG_PLDIS_bit(const void *const hw)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->PLCFG.reg ^= PM_PLCFG_PLDIS;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_set_PLCFG_PLSEL_bf(const void *const hw, hri_pm_plcfg_reg_t mask)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->PLCFG.reg |= PM_PLCFG_PLSEL(mask);
	PM_CRITICAL_SECTION_LEAVE();
}

static inline hri_pm_plcfg_reg_t hri_pm_get_PLCFG_PLSEL_bf(const void *const hw, hri_pm_plcfg_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Pm *)hw)->PLCFG.reg;
	tmp = (tmp & PM_PLCFG_PLSEL(mask)) >> PM_PLCFG_PLSEL_Pos;
	return tmp;
}

static inline void hri_pm_write_PLCFG_PLSEL_bf(const void *const hw, hri_pm_plcfg_reg_t data)
{
	uint8_t tmp;
	PM_CRITICAL_SECTION_ENTER();
	tmp = ((Pm *)hw)->PLCFG.reg;
	tmp &= ~PM_PLCFG_PLSEL_Msk;
	tmp |= PM_PLCFG_PLSEL(data);
	((Pm *)hw)->PLCFG.reg = tmp;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_clear_PLCFG_PLSEL_bf(const void *const hw, hri_pm_plcfg_reg_t mask)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->PLCFG.reg &= ~PM_PLCFG_PLSEL(mask);
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_toggle_PLCFG_PLSEL_bf(const void *const hw, hri_pm_plcfg_reg_t mask)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->PLCFG.reg ^= PM_PLCFG_PLSEL(mask);
	PM_CRITICAL_SECTION_LEAVE();
}

static inline hri_pm_plcfg_reg_t hri_pm_read_PLCFG_PLSEL_bf(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Pm *)hw)->PLCFG.reg;
	tmp = (tmp & PM_PLCFG_PLSEL_Msk) >> PM_PLCFG_PLSEL_Pos;
	return tmp;
}

static inline void hri_pm_set_PLCFG_reg(const void *const hw, hri_pm_plcfg_reg_t mask)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->PLCFG.reg |= mask;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline hri_pm_plcfg_reg_t hri_pm_get_PLCFG_reg(const void *const hw, hri_pm_plcfg_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Pm *)hw)->PLCFG.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_pm_write_PLCFG_reg(const void *const hw, hri_pm_plcfg_reg_t data)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->PLCFG.reg = data;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_clear_PLCFG_reg(const void *const hw, hri_pm_plcfg_reg_t mask)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->PLCFG.reg &= ~mask;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_toggle_PLCFG_reg(const void *const hw, hri_pm_plcfg_reg_t mask)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->PLCFG.reg ^= mask;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline hri_pm_plcfg_reg_t hri_pm_read_PLCFG_reg(const void *const hw)
{
	return ((Pm *)hw)->PLCFG.reg;
}

static inline void hri_pm_set_STDBYCFG_DPGPD0_bit(const void *const hw)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->STDBYCFG.reg |= PM_STDBYCFG_DPGPD0;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_pm_get_STDBYCFG_DPGPD0_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Pm *)hw)->STDBYCFG.reg;
	tmp = (tmp & PM_STDBYCFG_DPGPD0) >> PM_STDBYCFG_DPGPD0_Pos;
	return (bool)tmp;
}

static inline void hri_pm_write_STDBYCFG_DPGPD0_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	PM_CRITICAL_SECTION_ENTER();
	tmp = ((Pm *)hw)->STDBYCFG.reg;
	tmp &= ~PM_STDBYCFG_DPGPD0;
	tmp |= value << PM_STDBYCFG_DPGPD0_Pos;
	((Pm *)hw)->STDBYCFG.reg = tmp;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_clear_STDBYCFG_DPGPD0_bit(const void *const hw)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->STDBYCFG.reg &= ~PM_STDBYCFG_DPGPD0;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_toggle_STDBYCFG_DPGPD0_bit(const void *const hw)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->STDBYCFG.reg ^= PM_STDBYCFG_DPGPD0;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_set_STDBYCFG_DPGPD1_bit(const void *const hw)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->STDBYCFG.reg |= PM_STDBYCFG_DPGPD1;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_pm_get_STDBYCFG_DPGPD1_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Pm *)hw)->STDBYCFG.reg;
	tmp = (tmp & PM_STDBYCFG_DPGPD1) >> PM_STDBYCFG_DPGPD1_Pos;
	return (bool)tmp;
}

static inline void hri_pm_write_STDBYCFG_DPGPD1_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	PM_CRITICAL_SECTION_ENTER();
	tmp = ((Pm *)hw)->STDBYCFG.reg;
	tmp &= ~PM_STDBYCFG_DPGPD1;
	tmp |= value << PM_STDBYCFG_DPGPD1_Pos;
	((Pm *)hw)->STDBYCFG.reg = tmp;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_clear_STDBYCFG_DPGPD1_bit(const void *const hw)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->STDBYCFG.reg &= ~PM_STDBYCFG_DPGPD1;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_toggle_STDBYCFG_DPGPD1_bit(const void *const hw)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->STDBYCFG.reg ^= PM_STDBYCFG_DPGPD1;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_set_STDBYCFG_PDCFG_bf(const void *const hw, hri_pm_stdbycfg_reg_t mask)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->STDBYCFG.reg |= PM_STDBYCFG_PDCFG(mask);
	PM_CRITICAL_SECTION_LEAVE();
}

static inline hri_pm_stdbycfg_reg_t hri_pm_get_STDBYCFG_PDCFG_bf(const void *const hw, hri_pm_stdbycfg_reg_t mask)
{
	uint16_t tmp;
	tmp = ((Pm *)hw)->STDBYCFG.reg;
	tmp = (tmp & PM_STDBYCFG_PDCFG(mask)) >> PM_STDBYCFG_PDCFG_Pos;
	return tmp;
}

static inline void hri_pm_write_STDBYCFG_PDCFG_bf(const void *const hw, hri_pm_stdbycfg_reg_t data)
{
	uint16_t tmp;
	PM_CRITICAL_SECTION_ENTER();
	tmp = ((Pm *)hw)->STDBYCFG.reg;
	tmp &= ~PM_STDBYCFG_PDCFG_Msk;
	tmp |= PM_STDBYCFG_PDCFG(data);
	((Pm *)hw)->STDBYCFG.reg = tmp;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_clear_STDBYCFG_PDCFG_bf(const void *const hw, hri_pm_stdbycfg_reg_t mask)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->STDBYCFG.reg &= ~PM_STDBYCFG_PDCFG(mask);
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_toggle_STDBYCFG_PDCFG_bf(const void *const hw, hri_pm_stdbycfg_reg_t mask)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->STDBYCFG.reg ^= PM_STDBYCFG_PDCFG(mask);
	PM_CRITICAL_SECTION_LEAVE();
}

static inline hri_pm_stdbycfg_reg_t hri_pm_read_STDBYCFG_PDCFG_bf(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Pm *)hw)->STDBYCFG.reg;
	tmp = (tmp & PM_STDBYCFG_PDCFG_Msk) >> PM_STDBYCFG_PDCFG_Pos;
	return tmp;
}

static inline void hri_pm_set_STDBYCFG_VREGSMOD_bf(const void *const hw, hri_pm_stdbycfg_reg_t mask)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->STDBYCFG.reg |= PM_STDBYCFG_VREGSMOD(mask);
	PM_CRITICAL_SECTION_LEAVE();
}

static inline hri_pm_stdbycfg_reg_t hri_pm_get_STDBYCFG_VREGSMOD_bf(const void *const hw, hri_pm_stdbycfg_reg_t mask)
{
	uint16_t tmp;
	tmp = ((Pm *)hw)->STDBYCFG.reg;
	tmp = (tmp & PM_STDBYCFG_VREGSMOD(mask)) >> PM_STDBYCFG_VREGSMOD_Pos;
	return tmp;
}

static inline void hri_pm_write_STDBYCFG_VREGSMOD_bf(const void *const hw, hri_pm_stdbycfg_reg_t data)
{
	uint16_t tmp;
	PM_CRITICAL_SECTION_ENTER();
	tmp = ((Pm *)hw)->STDBYCFG.reg;
	tmp &= ~PM_STDBYCFG_VREGSMOD_Msk;
	tmp |= PM_STDBYCFG_VREGSMOD(data);
	((Pm *)hw)->STDBYCFG.reg = tmp;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_clear_STDBYCFG_VREGSMOD_bf(const void *const hw, hri_pm_stdbycfg_reg_t mask)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->STDBYCFG.reg &= ~PM_STDBYCFG_VREGSMOD(mask);
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_toggle_STDBYCFG_VREGSMOD_bf(const void *const hw, hri_pm_stdbycfg_reg_t mask)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->STDBYCFG.reg ^= PM_STDBYCFG_VREGSMOD(mask);
	PM_CRITICAL_SECTION_LEAVE();
}

static inline hri_pm_stdbycfg_reg_t hri_pm_read_STDBYCFG_VREGSMOD_bf(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Pm *)hw)->STDBYCFG.reg;
	tmp = (tmp & PM_STDBYCFG_VREGSMOD_Msk) >> PM_STDBYCFG_VREGSMOD_Pos;
	return tmp;
}

static inline void hri_pm_set_STDBYCFG_LINKPD_bf(const void *const hw, hri_pm_stdbycfg_reg_t mask)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->STDBYCFG.reg |= PM_STDBYCFG_LINKPD(mask);
	PM_CRITICAL_SECTION_LEAVE();
}

static inline hri_pm_stdbycfg_reg_t hri_pm_get_STDBYCFG_LINKPD_bf(const void *const hw, hri_pm_stdbycfg_reg_t mask)
{
	uint16_t tmp;
	tmp = ((Pm *)hw)->STDBYCFG.reg;
	tmp = (tmp & PM_STDBYCFG_LINKPD(mask)) >> PM_STDBYCFG_LINKPD_Pos;
	return tmp;
}

static inline void hri_pm_write_STDBYCFG_LINKPD_bf(const void *const hw, hri_pm_stdbycfg_reg_t data)
{
	uint16_t tmp;
	PM_CRITICAL_SECTION_ENTER();
	tmp = ((Pm *)hw)->STDBYCFG.reg;
	tmp &= ~PM_STDBYCFG_LINKPD_Msk;
	tmp |= PM_STDBYCFG_LINKPD(data);
	((Pm *)hw)->STDBYCFG.reg = tmp;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_clear_STDBYCFG_LINKPD_bf(const void *const hw, hri_pm_stdbycfg_reg_t mask)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->STDBYCFG.reg &= ~PM_STDBYCFG_LINKPD(mask);
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_toggle_STDBYCFG_LINKPD_bf(const void *const hw, hri_pm_stdbycfg_reg_t mask)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->STDBYCFG.reg ^= PM_STDBYCFG_LINKPD(mask);
	PM_CRITICAL_SECTION_LEAVE();
}

static inline hri_pm_stdbycfg_reg_t hri_pm_read_STDBYCFG_LINKPD_bf(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Pm *)hw)->STDBYCFG.reg;
	tmp = (tmp & PM_STDBYCFG_LINKPD_Msk) >> PM_STDBYCFG_LINKPD_Pos;
	return tmp;
}

static inline void hri_pm_set_STDBYCFG_BBIASHS_bf(const void *const hw, hri_pm_stdbycfg_reg_t mask)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->STDBYCFG.reg |= PM_STDBYCFG_BBIASHS(mask);
	PM_CRITICAL_SECTION_LEAVE();
}

static inline hri_pm_stdbycfg_reg_t hri_pm_get_STDBYCFG_BBIASHS_bf(const void *const hw, hri_pm_stdbycfg_reg_t mask)
{
	uint16_t tmp;
	tmp = ((Pm *)hw)->STDBYCFG.reg;
	tmp = (tmp & PM_STDBYCFG_BBIASHS(mask)) >> PM_STDBYCFG_BBIASHS_Pos;
	return tmp;
}

static inline void hri_pm_write_STDBYCFG_BBIASHS_bf(const void *const hw, hri_pm_stdbycfg_reg_t data)
{
	uint16_t tmp;
	PM_CRITICAL_SECTION_ENTER();
	tmp = ((Pm *)hw)->STDBYCFG.reg;
	tmp &= ~PM_STDBYCFG_BBIASHS_Msk;
	tmp |= PM_STDBYCFG_BBIASHS(data);
	((Pm *)hw)->STDBYCFG.reg = tmp;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_clear_STDBYCFG_BBIASHS_bf(const void *const hw, hri_pm_stdbycfg_reg_t mask)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->STDBYCFG.reg &= ~PM_STDBYCFG_BBIASHS(mask);
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_toggle_STDBYCFG_BBIASHS_bf(const void *const hw, hri_pm_stdbycfg_reg_t mask)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->STDBYCFG.reg ^= PM_STDBYCFG_BBIASHS(mask);
	PM_CRITICAL_SECTION_LEAVE();
}

static inline hri_pm_stdbycfg_reg_t hri_pm_read_STDBYCFG_BBIASHS_bf(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Pm *)hw)->STDBYCFG.reg;
	tmp = (tmp & PM_STDBYCFG_BBIASHS_Msk) >> PM_STDBYCFG_BBIASHS_Pos;
	return tmp;
}

static inline void hri_pm_set_STDBYCFG_BBIASLP_bf(const void *const hw, hri_pm_stdbycfg_reg_t mask)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->STDBYCFG.reg |= PM_STDBYCFG_BBIASLP(mask);
	PM_CRITICAL_SECTION_LEAVE();
}

static inline hri_pm_stdbycfg_reg_t hri_pm_get_STDBYCFG_BBIASLP_bf(const void *const hw, hri_pm_stdbycfg_reg_t mask)
{
	uint16_t tmp;
	tmp = ((Pm *)hw)->STDBYCFG.reg;
	tmp = (tmp & PM_STDBYCFG_BBIASLP(mask)) >> PM_STDBYCFG_BBIASLP_Pos;
	return tmp;
}

static inline void hri_pm_write_STDBYCFG_BBIASLP_bf(const void *const hw, hri_pm_stdbycfg_reg_t data)
{
	uint16_t tmp;
	PM_CRITICAL_SECTION_ENTER();
	tmp = ((Pm *)hw)->STDBYCFG.reg;
	tmp &= ~PM_STDBYCFG_BBIASLP_Msk;
	tmp |= PM_STDBYCFG_BBIASLP(data);
	((Pm *)hw)->STDBYCFG.reg = tmp;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_clear_STDBYCFG_BBIASLP_bf(const void *const hw, hri_pm_stdbycfg_reg_t mask)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->STDBYCFG.reg &= ~PM_STDBYCFG_BBIASLP(mask);
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_toggle_STDBYCFG_BBIASLP_bf(const void *const hw, hri_pm_stdbycfg_reg_t mask)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->STDBYCFG.reg ^= PM_STDBYCFG_BBIASLP(mask);
	PM_CRITICAL_SECTION_LEAVE();
}

static inline hri_pm_stdbycfg_reg_t hri_pm_read_STDBYCFG_BBIASLP_bf(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Pm *)hw)->STDBYCFG.reg;
	tmp = (tmp & PM_STDBYCFG_BBIASLP_Msk) >> PM_STDBYCFG_BBIASLP_Pos;
	return tmp;
}

static inline void hri_pm_set_STDBYCFG_BBIASPP_bf(const void *const hw, hri_pm_stdbycfg_reg_t mask)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->STDBYCFG.reg |= PM_STDBYCFG_BBIASPP(mask);
	PM_CRITICAL_SECTION_LEAVE();
}

static inline hri_pm_stdbycfg_reg_t hri_pm_get_STDBYCFG_BBIASPP_bf(const void *const hw, hri_pm_stdbycfg_reg_t mask)
{
	uint16_t tmp;
	tmp = ((Pm *)hw)->STDBYCFG.reg;
	tmp = (tmp & PM_STDBYCFG_BBIASPP(mask)) >> PM_STDBYCFG_BBIASPP_Pos;
	return tmp;
}

static inline void hri_pm_write_STDBYCFG_BBIASPP_bf(const void *const hw, hri_pm_stdbycfg_reg_t data)
{
	uint16_t tmp;
	PM_CRITICAL_SECTION_ENTER();
	tmp = ((Pm *)hw)->STDBYCFG.reg;
	tmp &= ~PM_STDBYCFG_BBIASPP_Msk;
	tmp |= PM_STDBYCFG_BBIASPP(data);
	((Pm *)hw)->STDBYCFG.reg = tmp;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_clear_STDBYCFG_BBIASPP_bf(const void *const hw, hri_pm_stdbycfg_reg_t mask)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->STDBYCFG.reg &= ~PM_STDBYCFG_BBIASPP(mask);
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_toggle_STDBYCFG_BBIASPP_bf(const void *const hw, hri_pm_stdbycfg_reg_t mask)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->STDBYCFG.reg ^= PM_STDBYCFG_BBIASPP(mask);
	PM_CRITICAL_SECTION_LEAVE();
}

static inline hri_pm_stdbycfg_reg_t hri_pm_read_STDBYCFG_BBIASPP_bf(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Pm *)hw)->STDBYCFG.reg;
	tmp = (tmp & PM_STDBYCFG_BBIASPP_Msk) >> PM_STDBYCFG_BBIASPP_Pos;
	return tmp;
}

static inline void hri_pm_set_STDBYCFG_reg(const void *const hw, hri_pm_stdbycfg_reg_t mask)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->STDBYCFG.reg |= mask;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline hri_pm_stdbycfg_reg_t hri_pm_get_STDBYCFG_reg(const void *const hw, hri_pm_stdbycfg_reg_t mask)
{
	uint16_t tmp;
	tmp = ((Pm *)hw)->STDBYCFG.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_pm_write_STDBYCFG_reg(const void *const hw, hri_pm_stdbycfg_reg_t data)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->STDBYCFG.reg = data;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_clear_STDBYCFG_reg(const void *const hw, hri_pm_stdbycfg_reg_t mask)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->STDBYCFG.reg &= ~mask;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_toggle_STDBYCFG_reg(const void *const hw, hri_pm_stdbycfg_reg_t mask)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->STDBYCFG.reg ^= mask;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline hri_pm_stdbycfg_reg_t hri_pm_read_STDBYCFG_reg(const void *const hw)
{
	return ((Pm *)hw)->STDBYCFG.reg;
}

static inline void hri_pm_set_PWSAKDLY_IGNACK_bit(const void *const hw)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->PWSAKDLY.reg |= PM_PWSAKDLY_IGNACK;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_pm_get_PWSAKDLY_IGNACK_bit(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Pm *)hw)->PWSAKDLY.reg;
	tmp = (tmp & PM_PWSAKDLY_IGNACK) >> PM_PWSAKDLY_IGNACK_Pos;
	return (bool)tmp;
}

static inline void hri_pm_write_PWSAKDLY_IGNACK_bit(const void *const hw, bool value)
{
	uint8_t tmp;
	PM_CRITICAL_SECTION_ENTER();
	tmp = ((Pm *)hw)->PWSAKDLY.reg;
	tmp &= ~PM_PWSAKDLY_IGNACK;
	tmp |= value << PM_PWSAKDLY_IGNACK_Pos;
	((Pm *)hw)->PWSAKDLY.reg = tmp;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_clear_PWSAKDLY_IGNACK_bit(const void *const hw)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->PWSAKDLY.reg &= ~PM_PWSAKDLY_IGNACK;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_toggle_PWSAKDLY_IGNACK_bit(const void *const hw)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->PWSAKDLY.reg ^= PM_PWSAKDLY_IGNACK;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_set_PWSAKDLY_DLYVAL_bf(const void *const hw, hri_pm_pwsakdly_reg_t mask)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->PWSAKDLY.reg |= PM_PWSAKDLY_DLYVAL(mask);
	PM_CRITICAL_SECTION_LEAVE();
}

static inline hri_pm_pwsakdly_reg_t hri_pm_get_PWSAKDLY_DLYVAL_bf(const void *const hw, hri_pm_pwsakdly_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Pm *)hw)->PWSAKDLY.reg;
	tmp = (tmp & PM_PWSAKDLY_DLYVAL(mask)) >> PM_PWSAKDLY_DLYVAL_Pos;
	return tmp;
}

static inline void hri_pm_write_PWSAKDLY_DLYVAL_bf(const void *const hw, hri_pm_pwsakdly_reg_t data)
{
	uint8_t tmp;
	PM_CRITICAL_SECTION_ENTER();
	tmp = ((Pm *)hw)->PWSAKDLY.reg;
	tmp &= ~PM_PWSAKDLY_DLYVAL_Msk;
	tmp |= PM_PWSAKDLY_DLYVAL(data);
	((Pm *)hw)->PWSAKDLY.reg = tmp;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_clear_PWSAKDLY_DLYVAL_bf(const void *const hw, hri_pm_pwsakdly_reg_t mask)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->PWSAKDLY.reg &= ~PM_PWSAKDLY_DLYVAL(mask);
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_toggle_PWSAKDLY_DLYVAL_bf(const void *const hw, hri_pm_pwsakdly_reg_t mask)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->PWSAKDLY.reg ^= PM_PWSAKDLY_DLYVAL(mask);
	PM_CRITICAL_SECTION_LEAVE();
}

static inline hri_pm_pwsakdly_reg_t hri_pm_read_PWSAKDLY_DLYVAL_bf(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Pm *)hw)->PWSAKDLY.reg;
	tmp = (tmp & PM_PWSAKDLY_DLYVAL_Msk) >> PM_PWSAKDLY_DLYVAL_Pos;
	return tmp;
}

static inline void hri_pm_set_PWSAKDLY_reg(const void *const hw, hri_pm_pwsakdly_reg_t mask)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->PWSAKDLY.reg |= mask;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline hri_pm_pwsakdly_reg_t hri_pm_get_PWSAKDLY_reg(const void *const hw, hri_pm_pwsakdly_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Pm *)hw)->PWSAKDLY.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_pm_write_PWSAKDLY_reg(const void *const hw, hri_pm_pwsakdly_reg_t data)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->PWSAKDLY.reg = data;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_clear_PWSAKDLY_reg(const void *const hw, hri_pm_pwsakdly_reg_t mask)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->PWSAKDLY.reg &= ~mask;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_toggle_PWSAKDLY_reg(const void *const hw, hri_pm_pwsakdly_reg_t mask)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->PWSAKDLY.reg ^= mask;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline hri_pm_pwsakdly_reg_t hri_pm_read_PWSAKDLY_reg(const void *const hw)
{
	return ((Pm *)hw)->PWSAKDLY.reg;
}

#ifdef __cplusplus
}
#endif

#endif /* _HRI_PM_L21_H_INCLUDED */
#endif /* _SAML21_PM_COMPONENT_ */
