/**
 * \file
 *
 * \brief SAM MCLK
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

#ifdef _SAML21_MCLK_COMPONENT_
#ifndef _HRI_MCLK_L21_H_INCLUDED_
#define _HRI_MCLK_L21_H_INCLUDED_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <hal_atomic.h>

#if defined(ENABLE_MCLK_CRITICAL_SECTIONS)
#define MCLK_CRITICAL_SECTION_ENTER() CRITICAL_SECTION_ENTER()
#define MCLK_CRITICAL_SECTION_LEAVE() CRITICAL_SECTION_LEAVE()
#else
#define MCLK_CRITICAL_SECTION_ENTER()
#define MCLK_CRITICAL_SECTION_LEAVE()
#endif

typedef uint32_t hri_mclk_ahbmask_reg_t;
typedef uint32_t hri_mclk_apbamask_reg_t;
typedef uint32_t hri_mclk_apbbmask_reg_t;
typedef uint32_t hri_mclk_apbcmask_reg_t;
typedef uint32_t hri_mclk_apbdmask_reg_t;
typedef uint32_t hri_mclk_apbemask_reg_t;
typedef uint8_t  hri_mclk_bupdiv_reg_t;
typedef uint8_t  hri_mclk_cpudiv_reg_t;
typedef uint8_t  hri_mclk_ctrla_reg_t;
typedef uint8_t  hri_mclk_intenset_reg_t;
typedef uint8_t  hri_mclk_intflag_reg_t;
typedef uint8_t  hri_mclk_lpdiv_reg_t;

static inline bool hri_mclk_get_INTFLAG_CKRDY_bit(const void *const hw)
{
	return (((Mclk *)hw)->INTFLAG.reg & MCLK_INTFLAG_CKRDY) >> MCLK_INTFLAG_CKRDY_Pos;
}

static inline void hri_mclk_clear_INTFLAG_CKRDY_bit(const void *const hw)
{
	((Mclk *)hw)->INTFLAG.reg = MCLK_INTFLAG_CKRDY;
}

static inline bool hri_mclk_get_interrupt_CKRDY_bit(const void *const hw)
{
	return (((Mclk *)hw)->INTFLAG.reg & MCLK_INTFLAG_CKRDY) >> MCLK_INTFLAG_CKRDY_Pos;
}

static inline void hri_mclk_clear_interrupt_CKRDY_bit(const void *const hw)
{
	((Mclk *)hw)->INTFLAG.reg = MCLK_INTFLAG_CKRDY;
}

static inline hri_mclk_intflag_reg_t hri_mclk_get_INTFLAG_reg(const void *const hw, hri_mclk_intflag_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Mclk *)hw)->INTFLAG.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_mclk_intflag_reg_t hri_mclk_read_INTFLAG_reg(const void *const hw)
{
	return ((Mclk *)hw)->INTFLAG.reg;
}

static inline void hri_mclk_clear_INTFLAG_reg(const void *const hw, hri_mclk_intflag_reg_t mask)
{
	((Mclk *)hw)->INTFLAG.reg = mask;
}

static inline void hri_mclk_set_INTEN_CKRDY_bit(const void *const hw)
{
	((Mclk *)hw)->INTENSET.reg = MCLK_INTENSET_CKRDY;
}

static inline bool hri_mclk_get_INTEN_CKRDY_bit(const void *const hw)
{
	return (((Mclk *)hw)->INTENSET.reg & MCLK_INTENSET_CKRDY) >> MCLK_INTENSET_CKRDY_Pos;
}

static inline void hri_mclk_write_INTEN_CKRDY_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((Mclk *)hw)->INTENCLR.reg = MCLK_INTENSET_CKRDY;
	} else {
		((Mclk *)hw)->INTENSET.reg = MCLK_INTENSET_CKRDY;
	}
}

static inline void hri_mclk_clear_INTEN_CKRDY_bit(const void *const hw)
{
	((Mclk *)hw)->INTENCLR.reg = MCLK_INTENSET_CKRDY;
}

static inline void hri_mclk_set_INTEN_reg(const void *const hw, hri_mclk_intenset_reg_t mask)
{
	((Mclk *)hw)->INTENSET.reg = mask;
}

static inline hri_mclk_intenset_reg_t hri_mclk_get_INTEN_reg(const void *const hw, hri_mclk_intenset_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Mclk *)hw)->INTENSET.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_mclk_intenset_reg_t hri_mclk_read_INTEN_reg(const void *const hw)
{
	return ((Mclk *)hw)->INTENSET.reg;
}

static inline void hri_mclk_write_INTEN_reg(const void *const hw, hri_mclk_intenset_reg_t data)
{
	((Mclk *)hw)->INTENSET.reg = data;
	((Mclk *)hw)->INTENCLR.reg = ~data;
}

static inline void hri_mclk_clear_INTEN_reg(const void *const hw, hri_mclk_intenset_reg_t mask)
{
	((Mclk *)hw)->INTENCLR.reg = mask;
}

static inline void hri_mclk_set_CTRLA_reg(const void *const hw, hri_mclk_ctrla_reg_t mask)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->CTRLA.reg |= mask;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline hri_mclk_ctrla_reg_t hri_mclk_get_CTRLA_reg(const void *const hw, hri_mclk_ctrla_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Mclk *)hw)->CTRLA.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_mclk_write_CTRLA_reg(const void *const hw, hri_mclk_ctrla_reg_t data)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->CTRLA.reg = data;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_clear_CTRLA_reg(const void *const hw, hri_mclk_ctrla_reg_t mask)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->CTRLA.reg &= ~mask;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_toggle_CTRLA_reg(const void *const hw, hri_mclk_ctrla_reg_t mask)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->CTRLA.reg ^= mask;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline hri_mclk_ctrla_reg_t hri_mclk_read_CTRLA_reg(const void *const hw)
{
	return ((Mclk *)hw)->CTRLA.reg;
}

static inline void hri_mclk_set_CPUDIV_CPUDIV_bf(const void *const hw, hri_mclk_cpudiv_reg_t mask)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->CPUDIV.reg |= MCLK_CPUDIV_CPUDIV(mask);
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline hri_mclk_cpudiv_reg_t hri_mclk_get_CPUDIV_CPUDIV_bf(const void *const hw, hri_mclk_cpudiv_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Mclk *)hw)->CPUDIV.reg;
	tmp = (tmp & MCLK_CPUDIV_CPUDIV(mask)) >> MCLK_CPUDIV_CPUDIV_Pos;
	return tmp;
}

static inline void hri_mclk_write_CPUDIV_CPUDIV_bf(const void *const hw, hri_mclk_cpudiv_reg_t data)
{
	uint8_t tmp;
	MCLK_CRITICAL_SECTION_ENTER();
	tmp = ((Mclk *)hw)->CPUDIV.reg;
	tmp &= ~MCLK_CPUDIV_CPUDIV_Msk;
	tmp |= MCLK_CPUDIV_CPUDIV(data);
	((Mclk *)hw)->CPUDIV.reg = tmp;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_clear_CPUDIV_CPUDIV_bf(const void *const hw, hri_mclk_cpudiv_reg_t mask)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->CPUDIV.reg &= ~MCLK_CPUDIV_CPUDIV(mask);
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_toggle_CPUDIV_CPUDIV_bf(const void *const hw, hri_mclk_cpudiv_reg_t mask)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->CPUDIV.reg ^= MCLK_CPUDIV_CPUDIV(mask);
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline hri_mclk_cpudiv_reg_t hri_mclk_read_CPUDIV_CPUDIV_bf(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Mclk *)hw)->CPUDIV.reg;
	tmp = (tmp & MCLK_CPUDIV_CPUDIV_Msk) >> MCLK_CPUDIV_CPUDIV_Pos;
	return tmp;
}

static inline void hri_mclk_set_CPUDIV_reg(const void *const hw, hri_mclk_cpudiv_reg_t mask)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->CPUDIV.reg |= mask;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline hri_mclk_cpudiv_reg_t hri_mclk_get_CPUDIV_reg(const void *const hw, hri_mclk_cpudiv_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Mclk *)hw)->CPUDIV.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_mclk_write_CPUDIV_reg(const void *const hw, hri_mclk_cpudiv_reg_t data)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->CPUDIV.reg = data;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_clear_CPUDIV_reg(const void *const hw, hri_mclk_cpudiv_reg_t mask)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->CPUDIV.reg &= ~mask;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_toggle_CPUDIV_reg(const void *const hw, hri_mclk_cpudiv_reg_t mask)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->CPUDIV.reg ^= mask;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline hri_mclk_cpudiv_reg_t hri_mclk_read_CPUDIV_reg(const void *const hw)
{
	return ((Mclk *)hw)->CPUDIV.reg;
}

static inline void hri_mclk_set_LPDIV_LPDIV_bf(const void *const hw, hri_mclk_lpdiv_reg_t mask)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->LPDIV.reg |= MCLK_LPDIV_LPDIV(mask);
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline hri_mclk_lpdiv_reg_t hri_mclk_get_LPDIV_LPDIV_bf(const void *const hw, hri_mclk_lpdiv_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Mclk *)hw)->LPDIV.reg;
	tmp = (tmp & MCLK_LPDIV_LPDIV(mask)) >> MCLK_LPDIV_LPDIV_Pos;
	return tmp;
}

static inline void hri_mclk_write_LPDIV_LPDIV_bf(const void *const hw, hri_mclk_lpdiv_reg_t data)
{
	uint8_t tmp;
	MCLK_CRITICAL_SECTION_ENTER();
	tmp = ((Mclk *)hw)->LPDIV.reg;
	tmp &= ~MCLK_LPDIV_LPDIV_Msk;
	tmp |= MCLK_LPDIV_LPDIV(data);
	((Mclk *)hw)->LPDIV.reg = tmp;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_clear_LPDIV_LPDIV_bf(const void *const hw, hri_mclk_lpdiv_reg_t mask)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->LPDIV.reg &= ~MCLK_LPDIV_LPDIV(mask);
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_toggle_LPDIV_LPDIV_bf(const void *const hw, hri_mclk_lpdiv_reg_t mask)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->LPDIV.reg ^= MCLK_LPDIV_LPDIV(mask);
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline hri_mclk_lpdiv_reg_t hri_mclk_read_LPDIV_LPDIV_bf(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Mclk *)hw)->LPDIV.reg;
	tmp = (tmp & MCLK_LPDIV_LPDIV_Msk) >> MCLK_LPDIV_LPDIV_Pos;
	return tmp;
}

static inline void hri_mclk_set_LPDIV_reg(const void *const hw, hri_mclk_lpdiv_reg_t mask)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->LPDIV.reg |= mask;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline hri_mclk_lpdiv_reg_t hri_mclk_get_LPDIV_reg(const void *const hw, hri_mclk_lpdiv_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Mclk *)hw)->LPDIV.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_mclk_write_LPDIV_reg(const void *const hw, hri_mclk_lpdiv_reg_t data)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->LPDIV.reg = data;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_clear_LPDIV_reg(const void *const hw, hri_mclk_lpdiv_reg_t mask)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->LPDIV.reg &= ~mask;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_toggle_LPDIV_reg(const void *const hw, hri_mclk_lpdiv_reg_t mask)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->LPDIV.reg ^= mask;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline hri_mclk_lpdiv_reg_t hri_mclk_read_LPDIV_reg(const void *const hw)
{
	return ((Mclk *)hw)->LPDIV.reg;
}

static inline void hri_mclk_set_BUPDIV_BUPDIV_bf(const void *const hw, hri_mclk_bupdiv_reg_t mask)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->BUPDIV.reg |= MCLK_BUPDIV_BUPDIV(mask);
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline hri_mclk_bupdiv_reg_t hri_mclk_get_BUPDIV_BUPDIV_bf(const void *const hw, hri_mclk_bupdiv_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Mclk *)hw)->BUPDIV.reg;
	tmp = (tmp & MCLK_BUPDIV_BUPDIV(mask)) >> MCLK_BUPDIV_BUPDIV_Pos;
	return tmp;
}

static inline void hri_mclk_write_BUPDIV_BUPDIV_bf(const void *const hw, hri_mclk_bupdiv_reg_t data)
{
	uint8_t tmp;
	MCLK_CRITICAL_SECTION_ENTER();
	tmp = ((Mclk *)hw)->BUPDIV.reg;
	tmp &= ~MCLK_BUPDIV_BUPDIV_Msk;
	tmp |= MCLK_BUPDIV_BUPDIV(data);
	((Mclk *)hw)->BUPDIV.reg = tmp;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_clear_BUPDIV_BUPDIV_bf(const void *const hw, hri_mclk_bupdiv_reg_t mask)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->BUPDIV.reg &= ~MCLK_BUPDIV_BUPDIV(mask);
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_toggle_BUPDIV_BUPDIV_bf(const void *const hw, hri_mclk_bupdiv_reg_t mask)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->BUPDIV.reg ^= MCLK_BUPDIV_BUPDIV(mask);
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline hri_mclk_bupdiv_reg_t hri_mclk_read_BUPDIV_BUPDIV_bf(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Mclk *)hw)->BUPDIV.reg;
	tmp = (tmp & MCLK_BUPDIV_BUPDIV_Msk) >> MCLK_BUPDIV_BUPDIV_Pos;
	return tmp;
}

static inline void hri_mclk_set_BUPDIV_reg(const void *const hw, hri_mclk_bupdiv_reg_t mask)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->BUPDIV.reg |= mask;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline hri_mclk_bupdiv_reg_t hri_mclk_get_BUPDIV_reg(const void *const hw, hri_mclk_bupdiv_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Mclk *)hw)->BUPDIV.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_mclk_write_BUPDIV_reg(const void *const hw, hri_mclk_bupdiv_reg_t data)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->BUPDIV.reg = data;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_clear_BUPDIV_reg(const void *const hw, hri_mclk_bupdiv_reg_t mask)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->BUPDIV.reg &= ~mask;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_toggle_BUPDIV_reg(const void *const hw, hri_mclk_bupdiv_reg_t mask)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->BUPDIV.reg ^= mask;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline hri_mclk_bupdiv_reg_t hri_mclk_read_BUPDIV_reg(const void *const hw)
{
	return ((Mclk *)hw)->BUPDIV.reg;
}

static inline void hri_mclk_set_AHBMASK_HPB0_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->AHBMASK.reg |= MCLK_AHBMASK_HPB0;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_mclk_get_AHBMASK_HPB0_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Mclk *)hw)->AHBMASK.reg;
	tmp = (tmp & MCLK_AHBMASK_HPB0) >> MCLK_AHBMASK_HPB0_Pos;
	return (bool)tmp;
}

static inline void hri_mclk_write_AHBMASK_HPB0_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	MCLK_CRITICAL_SECTION_ENTER();
	tmp = ((Mclk *)hw)->AHBMASK.reg;
	tmp &= ~MCLK_AHBMASK_HPB0;
	tmp |= value << MCLK_AHBMASK_HPB0_Pos;
	((Mclk *)hw)->AHBMASK.reg = tmp;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_clear_AHBMASK_HPB0_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->AHBMASK.reg &= ~MCLK_AHBMASK_HPB0;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_toggle_AHBMASK_HPB0_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->AHBMASK.reg ^= MCLK_AHBMASK_HPB0;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_set_AHBMASK_HPB1_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->AHBMASK.reg |= MCLK_AHBMASK_HPB1;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_mclk_get_AHBMASK_HPB1_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Mclk *)hw)->AHBMASK.reg;
	tmp = (tmp & MCLK_AHBMASK_HPB1) >> MCLK_AHBMASK_HPB1_Pos;
	return (bool)tmp;
}

static inline void hri_mclk_write_AHBMASK_HPB1_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	MCLK_CRITICAL_SECTION_ENTER();
	tmp = ((Mclk *)hw)->AHBMASK.reg;
	tmp &= ~MCLK_AHBMASK_HPB1;
	tmp |= value << MCLK_AHBMASK_HPB1_Pos;
	((Mclk *)hw)->AHBMASK.reg = tmp;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_clear_AHBMASK_HPB1_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->AHBMASK.reg &= ~MCLK_AHBMASK_HPB1;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_toggle_AHBMASK_HPB1_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->AHBMASK.reg ^= MCLK_AHBMASK_HPB1;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_set_AHBMASK_HPB2_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->AHBMASK.reg |= MCLK_AHBMASK_HPB2;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_mclk_get_AHBMASK_HPB2_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Mclk *)hw)->AHBMASK.reg;
	tmp = (tmp & MCLK_AHBMASK_HPB2) >> MCLK_AHBMASK_HPB2_Pos;
	return (bool)tmp;
}

static inline void hri_mclk_write_AHBMASK_HPB2_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	MCLK_CRITICAL_SECTION_ENTER();
	tmp = ((Mclk *)hw)->AHBMASK.reg;
	tmp &= ~MCLK_AHBMASK_HPB2;
	tmp |= value << MCLK_AHBMASK_HPB2_Pos;
	((Mclk *)hw)->AHBMASK.reg = tmp;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_clear_AHBMASK_HPB2_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->AHBMASK.reg &= ~MCLK_AHBMASK_HPB2;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_toggle_AHBMASK_HPB2_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->AHBMASK.reg ^= MCLK_AHBMASK_HPB2;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_set_AHBMASK_HPB3_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->AHBMASK.reg |= MCLK_AHBMASK_HPB3;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_mclk_get_AHBMASK_HPB3_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Mclk *)hw)->AHBMASK.reg;
	tmp = (tmp & MCLK_AHBMASK_HPB3) >> MCLK_AHBMASK_HPB3_Pos;
	return (bool)tmp;
}

static inline void hri_mclk_write_AHBMASK_HPB3_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	MCLK_CRITICAL_SECTION_ENTER();
	tmp = ((Mclk *)hw)->AHBMASK.reg;
	tmp &= ~MCLK_AHBMASK_HPB3;
	tmp |= value << MCLK_AHBMASK_HPB3_Pos;
	((Mclk *)hw)->AHBMASK.reg = tmp;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_clear_AHBMASK_HPB3_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->AHBMASK.reg &= ~MCLK_AHBMASK_HPB3;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_toggle_AHBMASK_HPB3_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->AHBMASK.reg ^= MCLK_AHBMASK_HPB3;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_set_AHBMASK_HPB4_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->AHBMASK.reg |= MCLK_AHBMASK_HPB4;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_mclk_get_AHBMASK_HPB4_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Mclk *)hw)->AHBMASK.reg;
	tmp = (tmp & MCLK_AHBMASK_HPB4) >> MCLK_AHBMASK_HPB4_Pos;
	return (bool)tmp;
}

static inline void hri_mclk_write_AHBMASK_HPB4_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	MCLK_CRITICAL_SECTION_ENTER();
	tmp = ((Mclk *)hw)->AHBMASK.reg;
	tmp &= ~MCLK_AHBMASK_HPB4;
	tmp |= value << MCLK_AHBMASK_HPB4_Pos;
	((Mclk *)hw)->AHBMASK.reg = tmp;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_clear_AHBMASK_HPB4_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->AHBMASK.reg &= ~MCLK_AHBMASK_HPB4;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_toggle_AHBMASK_HPB4_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->AHBMASK.reg ^= MCLK_AHBMASK_HPB4;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_set_AHBMASK_DSU_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->AHBMASK.reg |= MCLK_AHBMASK_DSU;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_mclk_get_AHBMASK_DSU_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Mclk *)hw)->AHBMASK.reg;
	tmp = (tmp & MCLK_AHBMASK_DSU) >> MCLK_AHBMASK_DSU_Pos;
	return (bool)tmp;
}

static inline void hri_mclk_write_AHBMASK_DSU_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	MCLK_CRITICAL_SECTION_ENTER();
	tmp = ((Mclk *)hw)->AHBMASK.reg;
	tmp &= ~MCLK_AHBMASK_DSU;
	tmp |= value << MCLK_AHBMASK_DSU_Pos;
	((Mclk *)hw)->AHBMASK.reg = tmp;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_clear_AHBMASK_DSU_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->AHBMASK.reg &= ~MCLK_AHBMASK_DSU;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_toggle_AHBMASK_DSU_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->AHBMASK.reg ^= MCLK_AHBMASK_DSU;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_set_AHBMASK_NVMCTRL_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->AHBMASK.reg |= MCLK_AHBMASK_NVMCTRL;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_mclk_get_AHBMASK_NVMCTRL_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Mclk *)hw)->AHBMASK.reg;
	tmp = (tmp & MCLK_AHBMASK_NVMCTRL) >> MCLK_AHBMASK_NVMCTRL_Pos;
	return (bool)tmp;
}

static inline void hri_mclk_write_AHBMASK_NVMCTRL_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	MCLK_CRITICAL_SECTION_ENTER();
	tmp = ((Mclk *)hw)->AHBMASK.reg;
	tmp &= ~MCLK_AHBMASK_NVMCTRL;
	tmp |= value << MCLK_AHBMASK_NVMCTRL_Pos;
	((Mclk *)hw)->AHBMASK.reg = tmp;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_clear_AHBMASK_NVMCTRL_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->AHBMASK.reg &= ~MCLK_AHBMASK_NVMCTRL;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_toggle_AHBMASK_NVMCTRL_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->AHBMASK.reg ^= MCLK_AHBMASK_NVMCTRL;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_set_AHBMASK_HSRAM_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->AHBMASK.reg |= MCLK_AHBMASK_HSRAM;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_mclk_get_AHBMASK_HSRAM_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Mclk *)hw)->AHBMASK.reg;
	tmp = (tmp & MCLK_AHBMASK_HSRAM) >> MCLK_AHBMASK_HSRAM_Pos;
	return (bool)tmp;
}

static inline void hri_mclk_write_AHBMASK_HSRAM_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	MCLK_CRITICAL_SECTION_ENTER();
	tmp = ((Mclk *)hw)->AHBMASK.reg;
	tmp &= ~MCLK_AHBMASK_HSRAM;
	tmp |= value << MCLK_AHBMASK_HSRAM_Pos;
	((Mclk *)hw)->AHBMASK.reg = tmp;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_clear_AHBMASK_HSRAM_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->AHBMASK.reg &= ~MCLK_AHBMASK_HSRAM;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_toggle_AHBMASK_HSRAM_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->AHBMASK.reg ^= MCLK_AHBMASK_HSRAM;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_set_AHBMASK_LPRAM_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->AHBMASK.reg |= MCLK_AHBMASK_LPRAM;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_mclk_get_AHBMASK_LPRAM_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Mclk *)hw)->AHBMASK.reg;
	tmp = (tmp & MCLK_AHBMASK_LPRAM) >> MCLK_AHBMASK_LPRAM_Pos;
	return (bool)tmp;
}

static inline void hri_mclk_write_AHBMASK_LPRAM_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	MCLK_CRITICAL_SECTION_ENTER();
	tmp = ((Mclk *)hw)->AHBMASK.reg;
	tmp &= ~MCLK_AHBMASK_LPRAM;
	tmp |= value << MCLK_AHBMASK_LPRAM_Pos;
	((Mclk *)hw)->AHBMASK.reg = tmp;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_clear_AHBMASK_LPRAM_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->AHBMASK.reg &= ~MCLK_AHBMASK_LPRAM;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_toggle_AHBMASK_LPRAM_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->AHBMASK.reg ^= MCLK_AHBMASK_LPRAM;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_set_AHBMASK_DMAC_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->AHBMASK.reg |= MCLK_AHBMASK_DMAC;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_mclk_get_AHBMASK_DMAC_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Mclk *)hw)->AHBMASK.reg;
	tmp = (tmp & MCLK_AHBMASK_DMAC) >> MCLK_AHBMASK_DMAC_Pos;
	return (bool)tmp;
}

static inline void hri_mclk_write_AHBMASK_DMAC_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	MCLK_CRITICAL_SECTION_ENTER();
	tmp = ((Mclk *)hw)->AHBMASK.reg;
	tmp &= ~MCLK_AHBMASK_DMAC;
	tmp |= value << MCLK_AHBMASK_DMAC_Pos;
	((Mclk *)hw)->AHBMASK.reg = tmp;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_clear_AHBMASK_DMAC_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->AHBMASK.reg &= ~MCLK_AHBMASK_DMAC;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_toggle_AHBMASK_DMAC_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->AHBMASK.reg ^= MCLK_AHBMASK_DMAC;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_set_AHBMASK_USB_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->AHBMASK.reg |= MCLK_AHBMASK_USB;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_mclk_get_AHBMASK_USB_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Mclk *)hw)->AHBMASK.reg;
	tmp = (tmp & MCLK_AHBMASK_USB) >> MCLK_AHBMASK_USB_Pos;
	return (bool)tmp;
}

static inline void hri_mclk_write_AHBMASK_USB_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	MCLK_CRITICAL_SECTION_ENTER();
	tmp = ((Mclk *)hw)->AHBMASK.reg;
	tmp &= ~MCLK_AHBMASK_USB;
	tmp |= value << MCLK_AHBMASK_USB_Pos;
	((Mclk *)hw)->AHBMASK.reg = tmp;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_clear_AHBMASK_USB_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->AHBMASK.reg &= ~MCLK_AHBMASK_USB;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_toggle_AHBMASK_USB_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->AHBMASK.reg ^= MCLK_AHBMASK_USB;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_set_AHBMASK_PAC_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->AHBMASK.reg |= MCLK_AHBMASK_PAC;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_mclk_get_AHBMASK_PAC_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Mclk *)hw)->AHBMASK.reg;
	tmp = (tmp & MCLK_AHBMASK_PAC) >> MCLK_AHBMASK_PAC_Pos;
	return (bool)tmp;
}

static inline void hri_mclk_write_AHBMASK_PAC_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	MCLK_CRITICAL_SECTION_ENTER();
	tmp = ((Mclk *)hw)->AHBMASK.reg;
	tmp &= ~MCLK_AHBMASK_PAC;
	tmp |= value << MCLK_AHBMASK_PAC_Pos;
	((Mclk *)hw)->AHBMASK.reg = tmp;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_clear_AHBMASK_PAC_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->AHBMASK.reg &= ~MCLK_AHBMASK_PAC;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_toggle_AHBMASK_PAC_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->AHBMASK.reg ^= MCLK_AHBMASK_PAC;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_set_AHBMASK_NVMCTRL_PICACHU_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->AHBMASK.reg |= MCLK_AHBMASK_NVMCTRL_PICACHU;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_mclk_get_AHBMASK_NVMCTRL_PICACHU_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Mclk *)hw)->AHBMASK.reg;
	tmp = (tmp & MCLK_AHBMASK_NVMCTRL_PICACHU) >> MCLK_AHBMASK_NVMCTRL_PICACHU_Pos;
	return (bool)tmp;
}

static inline void hri_mclk_write_AHBMASK_NVMCTRL_PICACHU_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	MCLK_CRITICAL_SECTION_ENTER();
	tmp = ((Mclk *)hw)->AHBMASK.reg;
	tmp &= ~MCLK_AHBMASK_NVMCTRL_PICACHU;
	tmp |= value << MCLK_AHBMASK_NVMCTRL_PICACHU_Pos;
	((Mclk *)hw)->AHBMASK.reg = tmp;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_clear_AHBMASK_NVMCTRL_PICACHU_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->AHBMASK.reg &= ~MCLK_AHBMASK_NVMCTRL_PICACHU;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_toggle_AHBMASK_NVMCTRL_PICACHU_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->AHBMASK.reg ^= MCLK_AHBMASK_NVMCTRL_PICACHU;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_set_AHBMASK_L2HBRIDGES_H_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->AHBMASK.reg |= MCLK_AHBMASK_L2HBRIDGES_H;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_mclk_get_AHBMASK_L2HBRIDGES_H_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Mclk *)hw)->AHBMASK.reg;
	tmp = (tmp & MCLK_AHBMASK_L2HBRIDGES_H) >> MCLK_AHBMASK_L2HBRIDGES_H_Pos;
	return (bool)tmp;
}

static inline void hri_mclk_write_AHBMASK_L2HBRIDGES_H_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	MCLK_CRITICAL_SECTION_ENTER();
	tmp = ((Mclk *)hw)->AHBMASK.reg;
	tmp &= ~MCLK_AHBMASK_L2HBRIDGES_H;
	tmp |= value << MCLK_AHBMASK_L2HBRIDGES_H_Pos;
	((Mclk *)hw)->AHBMASK.reg = tmp;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_clear_AHBMASK_L2HBRIDGES_H_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->AHBMASK.reg &= ~MCLK_AHBMASK_L2HBRIDGES_H;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_toggle_AHBMASK_L2HBRIDGES_H_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->AHBMASK.reg ^= MCLK_AHBMASK_L2HBRIDGES_H;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_set_AHBMASK_H2LBRIDGES_H_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->AHBMASK.reg |= MCLK_AHBMASK_H2LBRIDGES_H;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_mclk_get_AHBMASK_H2LBRIDGES_H_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Mclk *)hw)->AHBMASK.reg;
	tmp = (tmp & MCLK_AHBMASK_H2LBRIDGES_H) >> MCLK_AHBMASK_H2LBRIDGES_H_Pos;
	return (bool)tmp;
}

static inline void hri_mclk_write_AHBMASK_H2LBRIDGES_H_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	MCLK_CRITICAL_SECTION_ENTER();
	tmp = ((Mclk *)hw)->AHBMASK.reg;
	tmp &= ~MCLK_AHBMASK_H2LBRIDGES_H;
	tmp |= value << MCLK_AHBMASK_H2LBRIDGES_H_Pos;
	((Mclk *)hw)->AHBMASK.reg = tmp;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_clear_AHBMASK_H2LBRIDGES_H_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->AHBMASK.reg &= ~MCLK_AHBMASK_H2LBRIDGES_H;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_toggle_AHBMASK_H2LBRIDGES_H_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->AHBMASK.reg ^= MCLK_AHBMASK_H2LBRIDGES_H;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_set_AHBMASK_HSRAM_AHBSETUPKEEPER_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->AHBMASK.reg |= MCLK_AHBMASK_HSRAM_AHBSETUPKEEPER;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_mclk_get_AHBMASK_HSRAM_AHBSETUPKEEPER_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Mclk *)hw)->AHBMASK.reg;
	tmp = (tmp & MCLK_AHBMASK_HSRAM_AHBSETUPKEEPER) >> MCLK_AHBMASK_HSRAM_AHBSETUPKEEPER_Pos;
	return (bool)tmp;
}

static inline void hri_mclk_write_AHBMASK_HSRAM_AHBSETUPKEEPER_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	MCLK_CRITICAL_SECTION_ENTER();
	tmp = ((Mclk *)hw)->AHBMASK.reg;
	tmp &= ~MCLK_AHBMASK_HSRAM_AHBSETUPKEEPER;
	tmp |= value << MCLK_AHBMASK_HSRAM_AHBSETUPKEEPER_Pos;
	((Mclk *)hw)->AHBMASK.reg = tmp;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_clear_AHBMASK_HSRAM_AHBSETUPKEEPER_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->AHBMASK.reg &= ~MCLK_AHBMASK_HSRAM_AHBSETUPKEEPER;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_toggle_AHBMASK_HSRAM_AHBSETUPKEEPER_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->AHBMASK.reg ^= MCLK_AHBMASK_HSRAM_AHBSETUPKEEPER;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_set_AHBMASK_HSRAM_HMATRIXLP2HMCRAMCHSBRIDGE_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->AHBMASK.reg |= MCLK_AHBMASK_HSRAM_HMATRIXLP2HMCRAMCHSBRIDGE;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_mclk_get_AHBMASK_HSRAM_HMATRIXLP2HMCRAMCHSBRIDGE_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Mclk *)hw)->AHBMASK.reg;
	tmp = (tmp & MCLK_AHBMASK_HSRAM_HMATRIXLP2HMCRAMCHSBRIDGE) >> MCLK_AHBMASK_HSRAM_HMATRIXLP2HMCRAMCHSBRIDGE_Pos;
	return (bool)tmp;
}

static inline void hri_mclk_write_AHBMASK_HSRAM_HMATRIXLP2HMCRAMCHSBRIDGE_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	MCLK_CRITICAL_SECTION_ENTER();
	tmp = ((Mclk *)hw)->AHBMASK.reg;
	tmp &= ~MCLK_AHBMASK_HSRAM_HMATRIXLP2HMCRAMCHSBRIDGE;
	tmp |= value << MCLK_AHBMASK_HSRAM_HMATRIXLP2HMCRAMCHSBRIDGE_Pos;
	((Mclk *)hw)->AHBMASK.reg = tmp;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_clear_AHBMASK_HSRAM_HMATRIXLP2HMCRAMCHSBRIDGE_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->AHBMASK.reg &= ~MCLK_AHBMASK_HSRAM_HMATRIXLP2HMCRAMCHSBRIDGE;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_toggle_AHBMASK_HSRAM_HMATRIXLP2HMCRAMCHSBRIDGE_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->AHBMASK.reg ^= MCLK_AHBMASK_HSRAM_HMATRIXLP2HMCRAMCHSBRIDGE;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_set_AHBMASK_reg(const void *const hw, hri_mclk_ahbmask_reg_t mask)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->AHBMASK.reg |= mask;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline hri_mclk_ahbmask_reg_t hri_mclk_get_AHBMASK_reg(const void *const hw, hri_mclk_ahbmask_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Mclk *)hw)->AHBMASK.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_mclk_write_AHBMASK_reg(const void *const hw, hri_mclk_ahbmask_reg_t data)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->AHBMASK.reg = data;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_clear_AHBMASK_reg(const void *const hw, hri_mclk_ahbmask_reg_t mask)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->AHBMASK.reg &= ~mask;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_toggle_AHBMASK_reg(const void *const hw, hri_mclk_ahbmask_reg_t mask)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->AHBMASK.reg ^= mask;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline hri_mclk_ahbmask_reg_t hri_mclk_read_AHBMASK_reg(const void *const hw)
{
	return ((Mclk *)hw)->AHBMASK.reg;
}

static inline void hri_mclk_set_APBAMASK_PM_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBAMASK.reg |= MCLK_APBAMASK_PM;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_mclk_get_APBAMASK_PM_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Mclk *)hw)->APBAMASK.reg;
	tmp = (tmp & MCLK_APBAMASK_PM) >> MCLK_APBAMASK_PM_Pos;
	return (bool)tmp;
}

static inline void hri_mclk_write_APBAMASK_PM_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	MCLK_CRITICAL_SECTION_ENTER();
	tmp = ((Mclk *)hw)->APBAMASK.reg;
	tmp &= ~MCLK_APBAMASK_PM;
	tmp |= value << MCLK_APBAMASK_PM_Pos;
	((Mclk *)hw)->APBAMASK.reg = tmp;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_clear_APBAMASK_PM_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBAMASK.reg &= ~MCLK_APBAMASK_PM;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_toggle_APBAMASK_PM_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBAMASK.reg ^= MCLK_APBAMASK_PM;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_set_APBAMASK_MCLK_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBAMASK.reg |= MCLK_APBAMASK_MCLK;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_mclk_get_APBAMASK_MCLK_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Mclk *)hw)->APBAMASK.reg;
	tmp = (tmp & MCLK_APBAMASK_MCLK) >> MCLK_APBAMASK_MCLK_Pos;
	return (bool)tmp;
}

static inline void hri_mclk_write_APBAMASK_MCLK_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	MCLK_CRITICAL_SECTION_ENTER();
	tmp = ((Mclk *)hw)->APBAMASK.reg;
	tmp &= ~MCLK_APBAMASK_MCLK;
	tmp |= value << MCLK_APBAMASK_MCLK_Pos;
	((Mclk *)hw)->APBAMASK.reg = tmp;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_clear_APBAMASK_MCLK_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBAMASK.reg &= ~MCLK_APBAMASK_MCLK;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_toggle_APBAMASK_MCLK_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBAMASK.reg ^= MCLK_APBAMASK_MCLK;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_set_APBAMASK_RSTC_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBAMASK.reg |= MCLK_APBAMASK_RSTC;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_mclk_get_APBAMASK_RSTC_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Mclk *)hw)->APBAMASK.reg;
	tmp = (tmp & MCLK_APBAMASK_RSTC) >> MCLK_APBAMASK_RSTC_Pos;
	return (bool)tmp;
}

static inline void hri_mclk_write_APBAMASK_RSTC_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	MCLK_CRITICAL_SECTION_ENTER();
	tmp = ((Mclk *)hw)->APBAMASK.reg;
	tmp &= ~MCLK_APBAMASK_RSTC;
	tmp |= value << MCLK_APBAMASK_RSTC_Pos;
	((Mclk *)hw)->APBAMASK.reg = tmp;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_clear_APBAMASK_RSTC_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBAMASK.reg &= ~MCLK_APBAMASK_RSTC;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_toggle_APBAMASK_RSTC_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBAMASK.reg ^= MCLK_APBAMASK_RSTC;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_set_APBAMASK_OSCCTRL_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBAMASK.reg |= MCLK_APBAMASK_OSCCTRL;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_mclk_get_APBAMASK_OSCCTRL_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Mclk *)hw)->APBAMASK.reg;
	tmp = (tmp & MCLK_APBAMASK_OSCCTRL) >> MCLK_APBAMASK_OSCCTRL_Pos;
	return (bool)tmp;
}

static inline void hri_mclk_write_APBAMASK_OSCCTRL_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	MCLK_CRITICAL_SECTION_ENTER();
	tmp = ((Mclk *)hw)->APBAMASK.reg;
	tmp &= ~MCLK_APBAMASK_OSCCTRL;
	tmp |= value << MCLK_APBAMASK_OSCCTRL_Pos;
	((Mclk *)hw)->APBAMASK.reg = tmp;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_clear_APBAMASK_OSCCTRL_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBAMASK.reg &= ~MCLK_APBAMASK_OSCCTRL;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_toggle_APBAMASK_OSCCTRL_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBAMASK.reg ^= MCLK_APBAMASK_OSCCTRL;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_set_APBAMASK_OSC32KCTRL_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBAMASK.reg |= MCLK_APBAMASK_OSC32KCTRL;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_mclk_get_APBAMASK_OSC32KCTRL_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Mclk *)hw)->APBAMASK.reg;
	tmp = (tmp & MCLK_APBAMASK_OSC32KCTRL) >> MCLK_APBAMASK_OSC32KCTRL_Pos;
	return (bool)tmp;
}

static inline void hri_mclk_write_APBAMASK_OSC32KCTRL_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	MCLK_CRITICAL_SECTION_ENTER();
	tmp = ((Mclk *)hw)->APBAMASK.reg;
	tmp &= ~MCLK_APBAMASK_OSC32KCTRL;
	tmp |= value << MCLK_APBAMASK_OSC32KCTRL_Pos;
	((Mclk *)hw)->APBAMASK.reg = tmp;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_clear_APBAMASK_OSC32KCTRL_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBAMASK.reg &= ~MCLK_APBAMASK_OSC32KCTRL;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_toggle_APBAMASK_OSC32KCTRL_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBAMASK.reg ^= MCLK_APBAMASK_OSC32KCTRL;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_set_APBAMASK_SUPC_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBAMASK.reg |= MCLK_APBAMASK_SUPC;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_mclk_get_APBAMASK_SUPC_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Mclk *)hw)->APBAMASK.reg;
	tmp = (tmp & MCLK_APBAMASK_SUPC) >> MCLK_APBAMASK_SUPC_Pos;
	return (bool)tmp;
}

static inline void hri_mclk_write_APBAMASK_SUPC_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	MCLK_CRITICAL_SECTION_ENTER();
	tmp = ((Mclk *)hw)->APBAMASK.reg;
	tmp &= ~MCLK_APBAMASK_SUPC;
	tmp |= value << MCLK_APBAMASK_SUPC_Pos;
	((Mclk *)hw)->APBAMASK.reg = tmp;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_clear_APBAMASK_SUPC_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBAMASK.reg &= ~MCLK_APBAMASK_SUPC;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_toggle_APBAMASK_SUPC_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBAMASK.reg ^= MCLK_APBAMASK_SUPC;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_set_APBAMASK_GCLK_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBAMASK.reg |= MCLK_APBAMASK_GCLK;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_mclk_get_APBAMASK_GCLK_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Mclk *)hw)->APBAMASK.reg;
	tmp = (tmp & MCLK_APBAMASK_GCLK) >> MCLK_APBAMASK_GCLK_Pos;
	return (bool)tmp;
}

static inline void hri_mclk_write_APBAMASK_GCLK_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	MCLK_CRITICAL_SECTION_ENTER();
	tmp = ((Mclk *)hw)->APBAMASK.reg;
	tmp &= ~MCLK_APBAMASK_GCLK;
	tmp |= value << MCLK_APBAMASK_GCLK_Pos;
	((Mclk *)hw)->APBAMASK.reg = tmp;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_clear_APBAMASK_GCLK_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBAMASK.reg &= ~MCLK_APBAMASK_GCLK;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_toggle_APBAMASK_GCLK_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBAMASK.reg ^= MCLK_APBAMASK_GCLK;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_set_APBAMASK_WDT_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBAMASK.reg |= MCLK_APBAMASK_WDT;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_mclk_get_APBAMASK_WDT_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Mclk *)hw)->APBAMASK.reg;
	tmp = (tmp & MCLK_APBAMASK_WDT) >> MCLK_APBAMASK_WDT_Pos;
	return (bool)tmp;
}

static inline void hri_mclk_write_APBAMASK_WDT_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	MCLK_CRITICAL_SECTION_ENTER();
	tmp = ((Mclk *)hw)->APBAMASK.reg;
	tmp &= ~MCLK_APBAMASK_WDT;
	tmp |= value << MCLK_APBAMASK_WDT_Pos;
	((Mclk *)hw)->APBAMASK.reg = tmp;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_clear_APBAMASK_WDT_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBAMASK.reg &= ~MCLK_APBAMASK_WDT;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_toggle_APBAMASK_WDT_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBAMASK.reg ^= MCLK_APBAMASK_WDT;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_set_APBAMASK_RTC_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBAMASK.reg |= MCLK_APBAMASK_RTC;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_mclk_get_APBAMASK_RTC_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Mclk *)hw)->APBAMASK.reg;
	tmp = (tmp & MCLK_APBAMASK_RTC) >> MCLK_APBAMASK_RTC_Pos;
	return (bool)tmp;
}

static inline void hri_mclk_write_APBAMASK_RTC_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	MCLK_CRITICAL_SECTION_ENTER();
	tmp = ((Mclk *)hw)->APBAMASK.reg;
	tmp &= ~MCLK_APBAMASK_RTC;
	tmp |= value << MCLK_APBAMASK_RTC_Pos;
	((Mclk *)hw)->APBAMASK.reg = tmp;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_clear_APBAMASK_RTC_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBAMASK.reg &= ~MCLK_APBAMASK_RTC;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_toggle_APBAMASK_RTC_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBAMASK.reg ^= MCLK_APBAMASK_RTC;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_set_APBAMASK_EIC_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBAMASK.reg |= MCLK_APBAMASK_EIC;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_mclk_get_APBAMASK_EIC_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Mclk *)hw)->APBAMASK.reg;
	tmp = (tmp & MCLK_APBAMASK_EIC) >> MCLK_APBAMASK_EIC_Pos;
	return (bool)tmp;
}

static inline void hri_mclk_write_APBAMASK_EIC_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	MCLK_CRITICAL_SECTION_ENTER();
	tmp = ((Mclk *)hw)->APBAMASK.reg;
	tmp &= ~MCLK_APBAMASK_EIC;
	tmp |= value << MCLK_APBAMASK_EIC_Pos;
	((Mclk *)hw)->APBAMASK.reg = tmp;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_clear_APBAMASK_EIC_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBAMASK.reg &= ~MCLK_APBAMASK_EIC;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_toggle_APBAMASK_EIC_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBAMASK.reg ^= MCLK_APBAMASK_EIC;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_set_APBAMASK_PORT_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBAMASK.reg |= MCLK_APBAMASK_PORT;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_mclk_get_APBAMASK_PORT_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Mclk *)hw)->APBAMASK.reg;
	tmp = (tmp & MCLK_APBAMASK_PORT) >> MCLK_APBAMASK_PORT_Pos;
	return (bool)tmp;
}

static inline void hri_mclk_write_APBAMASK_PORT_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	MCLK_CRITICAL_SECTION_ENTER();
	tmp = ((Mclk *)hw)->APBAMASK.reg;
	tmp &= ~MCLK_APBAMASK_PORT;
	tmp |= value << MCLK_APBAMASK_PORT_Pos;
	((Mclk *)hw)->APBAMASK.reg = tmp;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_clear_APBAMASK_PORT_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBAMASK.reg &= ~MCLK_APBAMASK_PORT;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_toggle_APBAMASK_PORT_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBAMASK.reg ^= MCLK_APBAMASK_PORT;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_set_APBAMASK_reg(const void *const hw, hri_mclk_apbamask_reg_t mask)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBAMASK.reg |= mask;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline hri_mclk_apbamask_reg_t hri_mclk_get_APBAMASK_reg(const void *const hw, hri_mclk_apbamask_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Mclk *)hw)->APBAMASK.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_mclk_write_APBAMASK_reg(const void *const hw, hri_mclk_apbamask_reg_t data)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBAMASK.reg = data;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_clear_APBAMASK_reg(const void *const hw, hri_mclk_apbamask_reg_t mask)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBAMASK.reg &= ~mask;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_toggle_APBAMASK_reg(const void *const hw, hri_mclk_apbamask_reg_t mask)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBAMASK.reg ^= mask;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline hri_mclk_apbamask_reg_t hri_mclk_read_APBAMASK_reg(const void *const hw)
{
	return ((Mclk *)hw)->APBAMASK.reg;
}

static inline void hri_mclk_set_APBBMASK_USB_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBBMASK.reg |= MCLK_APBBMASK_USB;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_mclk_get_APBBMASK_USB_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Mclk *)hw)->APBBMASK.reg;
	tmp = (tmp & MCLK_APBBMASK_USB) >> MCLK_APBBMASK_USB_Pos;
	return (bool)tmp;
}

static inline void hri_mclk_write_APBBMASK_USB_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	MCLK_CRITICAL_SECTION_ENTER();
	tmp = ((Mclk *)hw)->APBBMASK.reg;
	tmp &= ~MCLK_APBBMASK_USB;
	tmp |= value << MCLK_APBBMASK_USB_Pos;
	((Mclk *)hw)->APBBMASK.reg = tmp;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_clear_APBBMASK_USB_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBBMASK.reg &= ~MCLK_APBBMASK_USB;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_toggle_APBBMASK_USB_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBBMASK.reg ^= MCLK_APBBMASK_USB;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_set_APBBMASK_DSU_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBBMASK.reg |= MCLK_APBBMASK_DSU;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_mclk_get_APBBMASK_DSU_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Mclk *)hw)->APBBMASK.reg;
	tmp = (tmp & MCLK_APBBMASK_DSU) >> MCLK_APBBMASK_DSU_Pos;
	return (bool)tmp;
}

static inline void hri_mclk_write_APBBMASK_DSU_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	MCLK_CRITICAL_SECTION_ENTER();
	tmp = ((Mclk *)hw)->APBBMASK.reg;
	tmp &= ~MCLK_APBBMASK_DSU;
	tmp |= value << MCLK_APBBMASK_DSU_Pos;
	((Mclk *)hw)->APBBMASK.reg = tmp;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_clear_APBBMASK_DSU_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBBMASK.reg &= ~MCLK_APBBMASK_DSU;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_toggle_APBBMASK_DSU_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBBMASK.reg ^= MCLK_APBBMASK_DSU;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_set_APBBMASK_NVMCTRL_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBBMASK.reg |= MCLK_APBBMASK_NVMCTRL;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_mclk_get_APBBMASK_NVMCTRL_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Mclk *)hw)->APBBMASK.reg;
	tmp = (tmp & MCLK_APBBMASK_NVMCTRL) >> MCLK_APBBMASK_NVMCTRL_Pos;
	return (bool)tmp;
}

static inline void hri_mclk_write_APBBMASK_NVMCTRL_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	MCLK_CRITICAL_SECTION_ENTER();
	tmp = ((Mclk *)hw)->APBBMASK.reg;
	tmp &= ~MCLK_APBBMASK_NVMCTRL;
	tmp |= value << MCLK_APBBMASK_NVMCTRL_Pos;
	((Mclk *)hw)->APBBMASK.reg = tmp;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_clear_APBBMASK_NVMCTRL_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBBMASK.reg &= ~MCLK_APBBMASK_NVMCTRL;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_toggle_APBBMASK_NVMCTRL_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBBMASK.reg ^= MCLK_APBBMASK_NVMCTRL;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_set_APBBMASK_reg(const void *const hw, hri_mclk_apbbmask_reg_t mask)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBBMASK.reg |= mask;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline hri_mclk_apbbmask_reg_t hri_mclk_get_APBBMASK_reg(const void *const hw, hri_mclk_apbbmask_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Mclk *)hw)->APBBMASK.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_mclk_write_APBBMASK_reg(const void *const hw, hri_mclk_apbbmask_reg_t data)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBBMASK.reg = data;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_clear_APBBMASK_reg(const void *const hw, hri_mclk_apbbmask_reg_t mask)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBBMASK.reg &= ~mask;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_toggle_APBBMASK_reg(const void *const hw, hri_mclk_apbbmask_reg_t mask)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBBMASK.reg ^= mask;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline hri_mclk_apbbmask_reg_t hri_mclk_read_APBBMASK_reg(const void *const hw)
{
	return ((Mclk *)hw)->APBBMASK.reg;
}

static inline void hri_mclk_set_APBCMASK_SERCOM0_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBCMASK.reg |= MCLK_APBCMASK_SERCOM0;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_mclk_get_APBCMASK_SERCOM0_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Mclk *)hw)->APBCMASK.reg;
	tmp = (tmp & MCLK_APBCMASK_SERCOM0) >> MCLK_APBCMASK_SERCOM0_Pos;
	return (bool)tmp;
}

static inline void hri_mclk_write_APBCMASK_SERCOM0_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	MCLK_CRITICAL_SECTION_ENTER();
	tmp = ((Mclk *)hw)->APBCMASK.reg;
	tmp &= ~MCLK_APBCMASK_SERCOM0;
	tmp |= value << MCLK_APBCMASK_SERCOM0_Pos;
	((Mclk *)hw)->APBCMASK.reg = tmp;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_clear_APBCMASK_SERCOM0_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBCMASK.reg &= ~MCLK_APBCMASK_SERCOM0;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_toggle_APBCMASK_SERCOM0_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBCMASK.reg ^= MCLK_APBCMASK_SERCOM0;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_set_APBCMASK_SERCOM1_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBCMASK.reg |= MCLK_APBCMASK_SERCOM1;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_mclk_get_APBCMASK_SERCOM1_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Mclk *)hw)->APBCMASK.reg;
	tmp = (tmp & MCLK_APBCMASK_SERCOM1) >> MCLK_APBCMASK_SERCOM1_Pos;
	return (bool)tmp;
}

static inline void hri_mclk_write_APBCMASK_SERCOM1_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	MCLK_CRITICAL_SECTION_ENTER();
	tmp = ((Mclk *)hw)->APBCMASK.reg;
	tmp &= ~MCLK_APBCMASK_SERCOM1;
	tmp |= value << MCLK_APBCMASK_SERCOM1_Pos;
	((Mclk *)hw)->APBCMASK.reg = tmp;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_clear_APBCMASK_SERCOM1_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBCMASK.reg &= ~MCLK_APBCMASK_SERCOM1;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_toggle_APBCMASK_SERCOM1_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBCMASK.reg ^= MCLK_APBCMASK_SERCOM1;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_set_APBCMASK_SERCOM2_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBCMASK.reg |= MCLK_APBCMASK_SERCOM2;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_mclk_get_APBCMASK_SERCOM2_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Mclk *)hw)->APBCMASK.reg;
	tmp = (tmp & MCLK_APBCMASK_SERCOM2) >> MCLK_APBCMASK_SERCOM2_Pos;
	return (bool)tmp;
}

static inline void hri_mclk_write_APBCMASK_SERCOM2_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	MCLK_CRITICAL_SECTION_ENTER();
	tmp = ((Mclk *)hw)->APBCMASK.reg;
	tmp &= ~MCLK_APBCMASK_SERCOM2;
	tmp |= value << MCLK_APBCMASK_SERCOM2_Pos;
	((Mclk *)hw)->APBCMASK.reg = tmp;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_clear_APBCMASK_SERCOM2_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBCMASK.reg &= ~MCLK_APBCMASK_SERCOM2;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_toggle_APBCMASK_SERCOM2_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBCMASK.reg ^= MCLK_APBCMASK_SERCOM2;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_set_APBCMASK_SERCOM3_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBCMASK.reg |= MCLK_APBCMASK_SERCOM3;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_mclk_get_APBCMASK_SERCOM3_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Mclk *)hw)->APBCMASK.reg;
	tmp = (tmp & MCLK_APBCMASK_SERCOM3) >> MCLK_APBCMASK_SERCOM3_Pos;
	return (bool)tmp;
}

static inline void hri_mclk_write_APBCMASK_SERCOM3_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	MCLK_CRITICAL_SECTION_ENTER();
	tmp = ((Mclk *)hw)->APBCMASK.reg;
	tmp &= ~MCLK_APBCMASK_SERCOM3;
	tmp |= value << MCLK_APBCMASK_SERCOM3_Pos;
	((Mclk *)hw)->APBCMASK.reg = tmp;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_clear_APBCMASK_SERCOM3_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBCMASK.reg &= ~MCLK_APBCMASK_SERCOM3;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_toggle_APBCMASK_SERCOM3_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBCMASK.reg ^= MCLK_APBCMASK_SERCOM3;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_set_APBCMASK_SERCOM4_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBCMASK.reg |= MCLK_APBCMASK_SERCOM4;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_mclk_get_APBCMASK_SERCOM4_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Mclk *)hw)->APBCMASK.reg;
	tmp = (tmp & MCLK_APBCMASK_SERCOM4) >> MCLK_APBCMASK_SERCOM4_Pos;
	return (bool)tmp;
}

static inline void hri_mclk_write_APBCMASK_SERCOM4_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	MCLK_CRITICAL_SECTION_ENTER();
	tmp = ((Mclk *)hw)->APBCMASK.reg;
	tmp &= ~MCLK_APBCMASK_SERCOM4;
	tmp |= value << MCLK_APBCMASK_SERCOM4_Pos;
	((Mclk *)hw)->APBCMASK.reg = tmp;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_clear_APBCMASK_SERCOM4_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBCMASK.reg &= ~MCLK_APBCMASK_SERCOM4;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_toggle_APBCMASK_SERCOM4_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBCMASK.reg ^= MCLK_APBCMASK_SERCOM4;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_set_APBCMASK_TCC0_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBCMASK.reg |= MCLK_APBCMASK_TCC0;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_mclk_get_APBCMASK_TCC0_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Mclk *)hw)->APBCMASK.reg;
	tmp = (tmp & MCLK_APBCMASK_TCC0) >> MCLK_APBCMASK_TCC0_Pos;
	return (bool)tmp;
}

static inline void hri_mclk_write_APBCMASK_TCC0_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	MCLK_CRITICAL_SECTION_ENTER();
	tmp = ((Mclk *)hw)->APBCMASK.reg;
	tmp &= ~MCLK_APBCMASK_TCC0;
	tmp |= value << MCLK_APBCMASK_TCC0_Pos;
	((Mclk *)hw)->APBCMASK.reg = tmp;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_clear_APBCMASK_TCC0_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBCMASK.reg &= ~MCLK_APBCMASK_TCC0;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_toggle_APBCMASK_TCC0_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBCMASK.reg ^= MCLK_APBCMASK_TCC0;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_set_APBCMASK_TCC1_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBCMASK.reg |= MCLK_APBCMASK_TCC1;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_mclk_get_APBCMASK_TCC1_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Mclk *)hw)->APBCMASK.reg;
	tmp = (tmp & MCLK_APBCMASK_TCC1) >> MCLK_APBCMASK_TCC1_Pos;
	return (bool)tmp;
}

static inline void hri_mclk_write_APBCMASK_TCC1_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	MCLK_CRITICAL_SECTION_ENTER();
	tmp = ((Mclk *)hw)->APBCMASK.reg;
	tmp &= ~MCLK_APBCMASK_TCC1;
	tmp |= value << MCLK_APBCMASK_TCC1_Pos;
	((Mclk *)hw)->APBCMASK.reg = tmp;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_clear_APBCMASK_TCC1_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBCMASK.reg &= ~MCLK_APBCMASK_TCC1;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_toggle_APBCMASK_TCC1_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBCMASK.reg ^= MCLK_APBCMASK_TCC1;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_set_APBCMASK_TCC2_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBCMASK.reg |= MCLK_APBCMASK_TCC2;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_mclk_get_APBCMASK_TCC2_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Mclk *)hw)->APBCMASK.reg;
	tmp = (tmp & MCLK_APBCMASK_TCC2) >> MCLK_APBCMASK_TCC2_Pos;
	return (bool)tmp;
}

static inline void hri_mclk_write_APBCMASK_TCC2_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	MCLK_CRITICAL_SECTION_ENTER();
	tmp = ((Mclk *)hw)->APBCMASK.reg;
	tmp &= ~MCLK_APBCMASK_TCC2;
	tmp |= value << MCLK_APBCMASK_TCC2_Pos;
	((Mclk *)hw)->APBCMASK.reg = tmp;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_clear_APBCMASK_TCC2_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBCMASK.reg &= ~MCLK_APBCMASK_TCC2;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_toggle_APBCMASK_TCC2_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBCMASK.reg ^= MCLK_APBCMASK_TCC2;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_set_APBCMASK_TC0_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBCMASK.reg |= MCLK_APBCMASK_TC0;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_mclk_get_APBCMASK_TC0_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Mclk *)hw)->APBCMASK.reg;
	tmp = (tmp & MCLK_APBCMASK_TC0) >> MCLK_APBCMASK_TC0_Pos;
	return (bool)tmp;
}

static inline void hri_mclk_write_APBCMASK_TC0_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	MCLK_CRITICAL_SECTION_ENTER();
	tmp = ((Mclk *)hw)->APBCMASK.reg;
	tmp &= ~MCLK_APBCMASK_TC0;
	tmp |= value << MCLK_APBCMASK_TC0_Pos;
	((Mclk *)hw)->APBCMASK.reg = tmp;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_clear_APBCMASK_TC0_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBCMASK.reg &= ~MCLK_APBCMASK_TC0;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_toggle_APBCMASK_TC0_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBCMASK.reg ^= MCLK_APBCMASK_TC0;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_set_APBCMASK_TC1_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBCMASK.reg |= MCLK_APBCMASK_TC1;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_mclk_get_APBCMASK_TC1_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Mclk *)hw)->APBCMASK.reg;
	tmp = (tmp & MCLK_APBCMASK_TC1) >> MCLK_APBCMASK_TC1_Pos;
	return (bool)tmp;
}

static inline void hri_mclk_write_APBCMASK_TC1_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	MCLK_CRITICAL_SECTION_ENTER();
	tmp = ((Mclk *)hw)->APBCMASK.reg;
	tmp &= ~MCLK_APBCMASK_TC1;
	tmp |= value << MCLK_APBCMASK_TC1_Pos;
	((Mclk *)hw)->APBCMASK.reg = tmp;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_clear_APBCMASK_TC1_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBCMASK.reg &= ~MCLK_APBCMASK_TC1;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_toggle_APBCMASK_TC1_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBCMASK.reg ^= MCLK_APBCMASK_TC1;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_set_APBCMASK_TC2_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBCMASK.reg |= MCLK_APBCMASK_TC2;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_mclk_get_APBCMASK_TC2_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Mclk *)hw)->APBCMASK.reg;
	tmp = (tmp & MCLK_APBCMASK_TC2) >> MCLK_APBCMASK_TC2_Pos;
	return (bool)tmp;
}

static inline void hri_mclk_write_APBCMASK_TC2_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	MCLK_CRITICAL_SECTION_ENTER();
	tmp = ((Mclk *)hw)->APBCMASK.reg;
	tmp &= ~MCLK_APBCMASK_TC2;
	tmp |= value << MCLK_APBCMASK_TC2_Pos;
	((Mclk *)hw)->APBCMASK.reg = tmp;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_clear_APBCMASK_TC2_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBCMASK.reg &= ~MCLK_APBCMASK_TC2;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_toggle_APBCMASK_TC2_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBCMASK.reg ^= MCLK_APBCMASK_TC2;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_set_APBCMASK_TC3_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBCMASK.reg |= MCLK_APBCMASK_TC3;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_mclk_get_APBCMASK_TC3_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Mclk *)hw)->APBCMASK.reg;
	tmp = (tmp & MCLK_APBCMASK_TC3) >> MCLK_APBCMASK_TC3_Pos;
	return (bool)tmp;
}

static inline void hri_mclk_write_APBCMASK_TC3_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	MCLK_CRITICAL_SECTION_ENTER();
	tmp = ((Mclk *)hw)->APBCMASK.reg;
	tmp &= ~MCLK_APBCMASK_TC3;
	tmp |= value << MCLK_APBCMASK_TC3_Pos;
	((Mclk *)hw)->APBCMASK.reg = tmp;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_clear_APBCMASK_TC3_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBCMASK.reg &= ~MCLK_APBCMASK_TC3;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_toggle_APBCMASK_TC3_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBCMASK.reg ^= MCLK_APBCMASK_TC3;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_set_APBCMASK_DAC_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBCMASK.reg |= MCLK_APBCMASK_DAC;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_mclk_get_APBCMASK_DAC_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Mclk *)hw)->APBCMASK.reg;
	tmp = (tmp & MCLK_APBCMASK_DAC) >> MCLK_APBCMASK_DAC_Pos;
	return (bool)tmp;
}

static inline void hri_mclk_write_APBCMASK_DAC_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	MCLK_CRITICAL_SECTION_ENTER();
	tmp = ((Mclk *)hw)->APBCMASK.reg;
	tmp &= ~MCLK_APBCMASK_DAC;
	tmp |= value << MCLK_APBCMASK_DAC_Pos;
	((Mclk *)hw)->APBCMASK.reg = tmp;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_clear_APBCMASK_DAC_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBCMASK.reg &= ~MCLK_APBCMASK_DAC;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_toggle_APBCMASK_DAC_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBCMASK.reg ^= MCLK_APBCMASK_DAC;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_set_APBCMASK_AES_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBCMASK.reg |= MCLK_APBCMASK_AES;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_mclk_get_APBCMASK_AES_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Mclk *)hw)->APBCMASK.reg;
	tmp = (tmp & MCLK_APBCMASK_AES) >> MCLK_APBCMASK_AES_Pos;
	return (bool)tmp;
}

static inline void hri_mclk_write_APBCMASK_AES_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	MCLK_CRITICAL_SECTION_ENTER();
	tmp = ((Mclk *)hw)->APBCMASK.reg;
	tmp &= ~MCLK_APBCMASK_AES;
	tmp |= value << MCLK_APBCMASK_AES_Pos;
	((Mclk *)hw)->APBCMASK.reg = tmp;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_clear_APBCMASK_AES_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBCMASK.reg &= ~MCLK_APBCMASK_AES;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_toggle_APBCMASK_AES_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBCMASK.reg ^= MCLK_APBCMASK_AES;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_set_APBCMASK_TRNG_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBCMASK.reg |= MCLK_APBCMASK_TRNG;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_mclk_get_APBCMASK_TRNG_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Mclk *)hw)->APBCMASK.reg;
	tmp = (tmp & MCLK_APBCMASK_TRNG) >> MCLK_APBCMASK_TRNG_Pos;
	return (bool)tmp;
}

static inline void hri_mclk_write_APBCMASK_TRNG_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	MCLK_CRITICAL_SECTION_ENTER();
	tmp = ((Mclk *)hw)->APBCMASK.reg;
	tmp &= ~MCLK_APBCMASK_TRNG;
	tmp |= value << MCLK_APBCMASK_TRNG_Pos;
	((Mclk *)hw)->APBCMASK.reg = tmp;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_clear_APBCMASK_TRNG_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBCMASK.reg &= ~MCLK_APBCMASK_TRNG;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_toggle_APBCMASK_TRNG_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBCMASK.reg ^= MCLK_APBCMASK_TRNG;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_set_APBCMASK_reg(const void *const hw, hri_mclk_apbcmask_reg_t mask)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBCMASK.reg |= mask;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline hri_mclk_apbcmask_reg_t hri_mclk_get_APBCMASK_reg(const void *const hw, hri_mclk_apbcmask_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Mclk *)hw)->APBCMASK.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_mclk_write_APBCMASK_reg(const void *const hw, hri_mclk_apbcmask_reg_t data)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBCMASK.reg = data;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_clear_APBCMASK_reg(const void *const hw, hri_mclk_apbcmask_reg_t mask)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBCMASK.reg &= ~mask;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_toggle_APBCMASK_reg(const void *const hw, hri_mclk_apbcmask_reg_t mask)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBCMASK.reg ^= mask;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline hri_mclk_apbcmask_reg_t hri_mclk_read_APBCMASK_reg(const void *const hw)
{
	return ((Mclk *)hw)->APBCMASK.reg;
}

static inline void hri_mclk_set_APBDMASK_EVSYS_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBDMASK.reg |= MCLK_APBDMASK_EVSYS;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_mclk_get_APBDMASK_EVSYS_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Mclk *)hw)->APBDMASK.reg;
	tmp = (tmp & MCLK_APBDMASK_EVSYS) >> MCLK_APBDMASK_EVSYS_Pos;
	return (bool)tmp;
}

static inline void hri_mclk_write_APBDMASK_EVSYS_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	MCLK_CRITICAL_SECTION_ENTER();
	tmp = ((Mclk *)hw)->APBDMASK.reg;
	tmp &= ~MCLK_APBDMASK_EVSYS;
	tmp |= value << MCLK_APBDMASK_EVSYS_Pos;
	((Mclk *)hw)->APBDMASK.reg = tmp;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_clear_APBDMASK_EVSYS_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBDMASK.reg &= ~MCLK_APBDMASK_EVSYS;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_toggle_APBDMASK_EVSYS_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBDMASK.reg ^= MCLK_APBDMASK_EVSYS;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_set_APBDMASK_SERCOM5_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBDMASK.reg |= MCLK_APBDMASK_SERCOM5;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_mclk_get_APBDMASK_SERCOM5_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Mclk *)hw)->APBDMASK.reg;
	tmp = (tmp & MCLK_APBDMASK_SERCOM5) >> MCLK_APBDMASK_SERCOM5_Pos;
	return (bool)tmp;
}

static inline void hri_mclk_write_APBDMASK_SERCOM5_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	MCLK_CRITICAL_SECTION_ENTER();
	tmp = ((Mclk *)hw)->APBDMASK.reg;
	tmp &= ~MCLK_APBDMASK_SERCOM5;
	tmp |= value << MCLK_APBDMASK_SERCOM5_Pos;
	((Mclk *)hw)->APBDMASK.reg = tmp;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_clear_APBDMASK_SERCOM5_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBDMASK.reg &= ~MCLK_APBDMASK_SERCOM5;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_toggle_APBDMASK_SERCOM5_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBDMASK.reg ^= MCLK_APBDMASK_SERCOM5;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_set_APBDMASK_TC4_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBDMASK.reg |= MCLK_APBDMASK_TC4;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_mclk_get_APBDMASK_TC4_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Mclk *)hw)->APBDMASK.reg;
	tmp = (tmp & MCLK_APBDMASK_TC4) >> MCLK_APBDMASK_TC4_Pos;
	return (bool)tmp;
}

static inline void hri_mclk_write_APBDMASK_TC4_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	MCLK_CRITICAL_SECTION_ENTER();
	tmp = ((Mclk *)hw)->APBDMASK.reg;
	tmp &= ~MCLK_APBDMASK_TC4;
	tmp |= value << MCLK_APBDMASK_TC4_Pos;
	((Mclk *)hw)->APBDMASK.reg = tmp;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_clear_APBDMASK_TC4_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBDMASK.reg &= ~MCLK_APBDMASK_TC4;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_toggle_APBDMASK_TC4_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBDMASK.reg ^= MCLK_APBDMASK_TC4;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_set_APBDMASK_ADC_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBDMASK.reg |= MCLK_APBDMASK_ADC;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_mclk_get_APBDMASK_ADC_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Mclk *)hw)->APBDMASK.reg;
	tmp = (tmp & MCLK_APBDMASK_ADC) >> MCLK_APBDMASK_ADC_Pos;
	return (bool)tmp;
}

static inline void hri_mclk_write_APBDMASK_ADC_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	MCLK_CRITICAL_SECTION_ENTER();
	tmp = ((Mclk *)hw)->APBDMASK.reg;
	tmp &= ~MCLK_APBDMASK_ADC;
	tmp |= value << MCLK_APBDMASK_ADC_Pos;
	((Mclk *)hw)->APBDMASK.reg = tmp;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_clear_APBDMASK_ADC_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBDMASK.reg &= ~MCLK_APBDMASK_ADC;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_toggle_APBDMASK_ADC_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBDMASK.reg ^= MCLK_APBDMASK_ADC;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_set_APBDMASK_AC_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBDMASK.reg |= MCLK_APBDMASK_AC;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_mclk_get_APBDMASK_AC_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Mclk *)hw)->APBDMASK.reg;
	tmp = (tmp & MCLK_APBDMASK_AC) >> MCLK_APBDMASK_AC_Pos;
	return (bool)tmp;
}

static inline void hri_mclk_write_APBDMASK_AC_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	MCLK_CRITICAL_SECTION_ENTER();
	tmp = ((Mclk *)hw)->APBDMASK.reg;
	tmp &= ~MCLK_APBDMASK_AC;
	tmp |= value << MCLK_APBDMASK_AC_Pos;
	((Mclk *)hw)->APBDMASK.reg = tmp;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_clear_APBDMASK_AC_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBDMASK.reg &= ~MCLK_APBDMASK_AC;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_toggle_APBDMASK_AC_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBDMASK.reg ^= MCLK_APBDMASK_AC;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_set_APBDMASK_PTC_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBDMASK.reg |= MCLK_APBDMASK_PTC;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_mclk_get_APBDMASK_PTC_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Mclk *)hw)->APBDMASK.reg;
	tmp = (tmp & MCLK_APBDMASK_PTC) >> MCLK_APBDMASK_PTC_Pos;
	return (bool)tmp;
}

static inline void hri_mclk_write_APBDMASK_PTC_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	MCLK_CRITICAL_SECTION_ENTER();
	tmp = ((Mclk *)hw)->APBDMASK.reg;
	tmp &= ~MCLK_APBDMASK_PTC;
	tmp |= value << MCLK_APBDMASK_PTC_Pos;
	((Mclk *)hw)->APBDMASK.reg = tmp;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_clear_APBDMASK_PTC_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBDMASK.reg &= ~MCLK_APBDMASK_PTC;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_toggle_APBDMASK_PTC_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBDMASK.reg ^= MCLK_APBDMASK_PTC;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_set_APBDMASK_OPAMP_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBDMASK.reg |= MCLK_APBDMASK_OPAMP;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_mclk_get_APBDMASK_OPAMP_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Mclk *)hw)->APBDMASK.reg;
	tmp = (tmp & MCLK_APBDMASK_OPAMP) >> MCLK_APBDMASK_OPAMP_Pos;
	return (bool)tmp;
}

static inline void hri_mclk_write_APBDMASK_OPAMP_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	MCLK_CRITICAL_SECTION_ENTER();
	tmp = ((Mclk *)hw)->APBDMASK.reg;
	tmp &= ~MCLK_APBDMASK_OPAMP;
	tmp |= value << MCLK_APBDMASK_OPAMP_Pos;
	((Mclk *)hw)->APBDMASK.reg = tmp;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_clear_APBDMASK_OPAMP_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBDMASK.reg &= ~MCLK_APBDMASK_OPAMP;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_toggle_APBDMASK_OPAMP_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBDMASK.reg ^= MCLK_APBDMASK_OPAMP;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_set_APBDMASK_CCL_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBDMASK.reg |= MCLK_APBDMASK_CCL;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_mclk_get_APBDMASK_CCL_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Mclk *)hw)->APBDMASK.reg;
	tmp = (tmp & MCLK_APBDMASK_CCL) >> MCLK_APBDMASK_CCL_Pos;
	return (bool)tmp;
}

static inline void hri_mclk_write_APBDMASK_CCL_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	MCLK_CRITICAL_SECTION_ENTER();
	tmp = ((Mclk *)hw)->APBDMASK.reg;
	tmp &= ~MCLK_APBDMASK_CCL;
	tmp |= value << MCLK_APBDMASK_CCL_Pos;
	((Mclk *)hw)->APBDMASK.reg = tmp;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_clear_APBDMASK_CCL_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBDMASK.reg &= ~MCLK_APBDMASK_CCL;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_toggle_APBDMASK_CCL_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBDMASK.reg ^= MCLK_APBDMASK_CCL;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_set_APBDMASK_reg(const void *const hw, hri_mclk_apbdmask_reg_t mask)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBDMASK.reg |= mask;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline hri_mclk_apbdmask_reg_t hri_mclk_get_APBDMASK_reg(const void *const hw, hri_mclk_apbdmask_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Mclk *)hw)->APBDMASK.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_mclk_write_APBDMASK_reg(const void *const hw, hri_mclk_apbdmask_reg_t data)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBDMASK.reg = data;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_clear_APBDMASK_reg(const void *const hw, hri_mclk_apbdmask_reg_t mask)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBDMASK.reg &= ~mask;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_toggle_APBDMASK_reg(const void *const hw, hri_mclk_apbdmask_reg_t mask)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBDMASK.reg ^= mask;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline hri_mclk_apbdmask_reg_t hri_mclk_read_APBDMASK_reg(const void *const hw)
{
	return ((Mclk *)hw)->APBDMASK.reg;
}

static inline void hri_mclk_set_APBEMASK_PAC_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBEMASK.reg |= MCLK_APBEMASK_PAC;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_mclk_get_APBEMASK_PAC_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Mclk *)hw)->APBEMASK.reg;
	tmp = (tmp & MCLK_APBEMASK_PAC) >> MCLK_APBEMASK_PAC_Pos;
	return (bool)tmp;
}

static inline void hri_mclk_write_APBEMASK_PAC_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	MCLK_CRITICAL_SECTION_ENTER();
	tmp = ((Mclk *)hw)->APBEMASK.reg;
	tmp &= ~MCLK_APBEMASK_PAC;
	tmp |= value << MCLK_APBEMASK_PAC_Pos;
	((Mclk *)hw)->APBEMASK.reg = tmp;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_clear_APBEMASK_PAC_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBEMASK.reg &= ~MCLK_APBEMASK_PAC;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_toggle_APBEMASK_PAC_bit(const void *const hw)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBEMASK.reg ^= MCLK_APBEMASK_PAC;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_set_APBEMASK_reg(const void *const hw, hri_mclk_apbemask_reg_t mask)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBEMASK.reg |= mask;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline hri_mclk_apbemask_reg_t hri_mclk_get_APBEMASK_reg(const void *const hw, hri_mclk_apbemask_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Mclk *)hw)->APBEMASK.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_mclk_write_APBEMASK_reg(const void *const hw, hri_mclk_apbemask_reg_t data)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBEMASK.reg = data;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_clear_APBEMASK_reg(const void *const hw, hri_mclk_apbemask_reg_t mask)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBEMASK.reg &= ~mask;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mclk_toggle_APBEMASK_reg(const void *const hw, hri_mclk_apbemask_reg_t mask)
{
	MCLK_CRITICAL_SECTION_ENTER();
	((Mclk *)hw)->APBEMASK.reg ^= mask;
	MCLK_CRITICAL_SECTION_LEAVE();
}

static inline hri_mclk_apbemask_reg_t hri_mclk_read_APBEMASK_reg(const void *const hw)
{
	return ((Mclk *)hw)->APBEMASK.reg;
}

#ifdef __cplusplus
}
#endif

#endif /* _HRI_MCLK_L21_H_INCLUDED */
#endif /* _SAML21_MCLK_COMPONENT_ */
