/**
 * \file
 *
 * \brief SAM NVIC
 *
 * Copyright (C) 2016 Atmel Corporation. All rights reserved.
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

#ifdef _SAME54_NVIC_COMPONENT_
#ifndef _HRI_NVIC_E54_H_INCLUDED_
#define _HRI_NVIC_E54_H_INCLUDED_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <hal_atomic.h>

#if defined(ENABLE_NVIC_CRITICAL_SECTIONS)
#define NVIC_CRITICAL_SECTION_ENTER() CRITICAL_SECTION_ENTER()
#define NVIC_CRITICAL_SECTION_LEAVE() CRITICAL_SECTION_LEAVE()
#else
#define NVIC_CRITICAL_SECTION_ENTER()
#define NVIC_CRITICAL_SECTION_LEAVE()
#endif

typedef uint32_t hri_nvic_nviciabr0_reg_t;
typedef uint32_t hri_nvic_nviciabr1_reg_t;
typedef uint32_t hri_nvic_nviciabr2_reg_t;
typedef uint32_t hri_nvic_nviciabr3_reg_t;
typedef uint32_t hri_nvic_nvicicer0_reg_t;
typedef uint32_t hri_nvic_nvicicer1_reg_t;
typedef uint32_t hri_nvic_nvicicer2_reg_t;
typedef uint32_t hri_nvic_nvicicer3_reg_t;
typedef uint32_t hri_nvic_nvicicpr0_reg_t;
typedef uint32_t hri_nvic_nvicicpr1_reg_t;
typedef uint32_t hri_nvic_nvicicpr2_reg_t;
typedef uint32_t hri_nvic_nvicicpr3_reg_t;
typedef uint32_t hri_nvic_nviciser0_reg_t;
typedef uint32_t hri_nvic_nviciser1_reg_t;
typedef uint32_t hri_nvic_nviciser2_reg_t;
typedef uint32_t hri_nvic_nviciser3_reg_t;
typedef uint32_t hri_nvic_nvicispr0_reg_t;
typedef uint32_t hri_nvic_nvicispr1_reg_t;
typedef uint32_t hri_nvic_nvicispr2_reg_t;
typedef uint32_t hri_nvic_nvicispr3_reg_t;
typedef uint32_t hri_nvic_nvicstir_reg_t;
typedef uint8_t  hri_nvic_nvicip0_reg_t;
typedef uint8_t  hri_nvic_nvicip100_reg_t;
typedef uint8_t  hri_nvic_nvicip101_reg_t;
typedef uint8_t  hri_nvic_nvicip102_reg_t;
typedef uint8_t  hri_nvic_nvicip103_reg_t;
typedef uint8_t  hri_nvic_nvicip104_reg_t;
typedef uint8_t  hri_nvic_nvicip105_reg_t;
typedef uint8_t  hri_nvic_nvicip10_reg_t;
typedef uint8_t  hri_nvic_nvicip11_reg_t;
typedef uint8_t  hri_nvic_nvicip12_reg_t;
typedef uint8_t  hri_nvic_nvicip13_reg_t;
typedef uint8_t  hri_nvic_nvicip14_reg_t;
typedef uint8_t  hri_nvic_nvicip15_reg_t;
typedef uint8_t  hri_nvic_nvicip16_reg_t;
typedef uint8_t  hri_nvic_nvicip17_reg_t;
typedef uint8_t  hri_nvic_nvicip18_reg_t;
typedef uint8_t  hri_nvic_nvicip19_reg_t;
typedef uint8_t  hri_nvic_nvicip1_reg_t;
typedef uint8_t  hri_nvic_nvicip20_reg_t;
typedef uint8_t  hri_nvic_nvicip21_reg_t;
typedef uint8_t  hri_nvic_nvicip22_reg_t;
typedef uint8_t  hri_nvic_nvicip23_reg_t;
typedef uint8_t  hri_nvic_nvicip24_reg_t;
typedef uint8_t  hri_nvic_nvicip25_reg_t;
typedef uint8_t  hri_nvic_nvicip26_reg_t;
typedef uint8_t  hri_nvic_nvicip27_reg_t;
typedef uint8_t  hri_nvic_nvicip28_reg_t;
typedef uint8_t  hri_nvic_nvicip29_reg_t;
typedef uint8_t  hri_nvic_nvicip2_reg_t;
typedef uint8_t  hri_nvic_nvicip30_reg_t;
typedef uint8_t  hri_nvic_nvicip31_reg_t;
typedef uint8_t  hri_nvic_nvicip32_reg_t;
typedef uint8_t  hri_nvic_nvicip33_reg_t;
typedef uint8_t  hri_nvic_nvicip34_reg_t;
typedef uint8_t  hri_nvic_nvicip35_reg_t;
typedef uint8_t  hri_nvic_nvicip36_reg_t;
typedef uint8_t  hri_nvic_nvicip37_reg_t;
typedef uint8_t  hri_nvic_nvicip38_reg_t;
typedef uint8_t  hri_nvic_nvicip39_reg_t;
typedef uint8_t  hri_nvic_nvicip3_reg_t;
typedef uint8_t  hri_nvic_nvicip40_reg_t;
typedef uint8_t  hri_nvic_nvicip41_reg_t;
typedef uint8_t  hri_nvic_nvicip42_reg_t;
typedef uint8_t  hri_nvic_nvicip43_reg_t;
typedef uint8_t  hri_nvic_nvicip44_reg_t;
typedef uint8_t  hri_nvic_nvicip45_reg_t;
typedef uint8_t  hri_nvic_nvicip46_reg_t;
typedef uint8_t  hri_nvic_nvicip47_reg_t;
typedef uint8_t  hri_nvic_nvicip48_reg_t;
typedef uint8_t  hri_nvic_nvicip49_reg_t;
typedef uint8_t  hri_nvic_nvicip4_reg_t;
typedef uint8_t  hri_nvic_nvicip50_reg_t;
typedef uint8_t  hri_nvic_nvicip51_reg_t;
typedef uint8_t  hri_nvic_nvicip52_reg_t;
typedef uint8_t  hri_nvic_nvicip53_reg_t;
typedef uint8_t  hri_nvic_nvicip54_reg_t;
typedef uint8_t  hri_nvic_nvicip55_reg_t;
typedef uint8_t  hri_nvic_nvicip56_reg_t;
typedef uint8_t  hri_nvic_nvicip57_reg_t;
typedef uint8_t  hri_nvic_nvicip58_reg_t;
typedef uint8_t  hri_nvic_nvicip59_reg_t;
typedef uint8_t  hri_nvic_nvicip5_reg_t;
typedef uint8_t  hri_nvic_nvicip60_reg_t;
typedef uint8_t  hri_nvic_nvicip61_reg_t;
typedef uint8_t  hri_nvic_nvicip62_reg_t;
typedef uint8_t  hri_nvic_nvicip63_reg_t;
typedef uint8_t  hri_nvic_nvicip64_reg_t;
typedef uint8_t  hri_nvic_nvicip65_reg_t;
typedef uint8_t  hri_nvic_nvicip66_reg_t;
typedef uint8_t  hri_nvic_nvicip67_reg_t;
typedef uint8_t  hri_nvic_nvicip68_reg_t;
typedef uint8_t  hri_nvic_nvicip69_reg_t;
typedef uint8_t  hri_nvic_nvicip6_reg_t;
typedef uint8_t  hri_nvic_nvicip70_reg_t;
typedef uint8_t  hri_nvic_nvicip71_reg_t;
typedef uint8_t  hri_nvic_nvicip72_reg_t;
typedef uint8_t  hri_nvic_nvicip73_reg_t;
typedef uint8_t  hri_nvic_nvicip74_reg_t;
typedef uint8_t  hri_nvic_nvicip75_reg_t;
typedef uint8_t  hri_nvic_nvicip76_reg_t;
typedef uint8_t  hri_nvic_nvicip77_reg_t;
typedef uint8_t  hri_nvic_nvicip78_reg_t;
typedef uint8_t  hri_nvic_nvicip79_reg_t;
typedef uint8_t  hri_nvic_nvicip7_reg_t;
typedef uint8_t  hri_nvic_nvicip80_reg_t;
typedef uint8_t  hri_nvic_nvicip81_reg_t;
typedef uint8_t  hri_nvic_nvicip82_reg_t;
typedef uint8_t  hri_nvic_nvicip83_reg_t;
typedef uint8_t  hri_nvic_nvicip84_reg_t;
typedef uint8_t  hri_nvic_nvicip85_reg_t;
typedef uint8_t  hri_nvic_nvicip86_reg_t;
typedef uint8_t  hri_nvic_nvicip87_reg_t;
typedef uint8_t  hri_nvic_nvicip88_reg_t;
typedef uint8_t  hri_nvic_nvicip89_reg_t;
typedef uint8_t  hri_nvic_nvicip8_reg_t;
typedef uint8_t  hri_nvic_nvicip90_reg_t;
typedef uint8_t  hri_nvic_nvicip91_reg_t;
typedef uint8_t  hri_nvic_nvicip92_reg_t;
typedef uint8_t  hri_nvic_nvicip93_reg_t;
typedef uint8_t  hri_nvic_nvicip94_reg_t;
typedef uint8_t  hri_nvic_nvicip95_reg_t;
typedef uint8_t  hri_nvic_nvicip96_reg_t;
typedef uint8_t  hri_nvic_nvicip97_reg_t;
typedef uint8_t  hri_nvic_nvicip98_reg_t;
typedef uint8_t  hri_nvic_nvicip99_reg_t;
typedef uint8_t  hri_nvic_nvicip9_reg_t;

static inline void hri_nvic_set_NVICISER0_SETENA_bf(const void *const hw, hri_nvic_nviciser0_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICISER0.reg |= NVIC_NVICISER0_SETENA(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nviciser0_reg_t hri_nvic_get_NVICISER0_SETENA_bf(const void *const        hw,
                                                                        hri_nvic_nviciser0_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Nvic *)hw)->NVICISER0.reg;
	tmp = (tmp & NVIC_NVICISER0_SETENA(mask)) >> 0;
	return tmp;
}

static inline void hri_nvic_write_NVICISER0_SETENA_bf(const void *const hw, hri_nvic_nviciser0_reg_t data)
{
	uint32_t tmp;
	NVIC_CRITICAL_SECTION_ENTER();
	tmp = ((Nvic *)hw)->NVICISER0.reg;
	tmp &= ~NVIC_NVICISER0_SETENA_Msk;
	tmp |= NVIC_NVICISER0_SETENA(data);
	((Nvic *)hw)->NVICISER0.reg = tmp;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICISER0_SETENA_bf(const void *const hw, hri_nvic_nviciser0_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICISER0.reg &= ~NVIC_NVICISER0_SETENA(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICISER0_SETENA_bf(const void *const hw, hri_nvic_nviciser0_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICISER0.reg ^= NVIC_NVICISER0_SETENA(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nviciser0_reg_t hri_nvic_read_NVICISER0_SETENA_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Nvic *)hw)->NVICISER0.reg;
	tmp = (tmp & NVIC_NVICISER0_SETENA_Msk) >> 0;
	return tmp;
}

static inline void hri_nvic_set_NVICISER0_reg(const void *const hw, hri_nvic_nviciser0_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICISER0.reg |= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nviciser0_reg_t hri_nvic_get_NVICISER0_reg(const void *const hw, hri_nvic_nviciser0_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Nvic *)hw)->NVICISER0.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_nvic_write_NVICISER0_reg(const void *const hw, hri_nvic_nviciser0_reg_t data)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICISER0.reg = data;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICISER0_reg(const void *const hw, hri_nvic_nviciser0_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICISER0.reg &= ~mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICISER0_reg(const void *const hw, hri_nvic_nviciser0_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICISER0.reg ^= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nviciser0_reg_t hri_nvic_read_NVICISER0_reg(const void *const hw)
{
	return ((Nvic *)hw)->NVICISER0.reg;
}

static inline void hri_nvic_set_NVICISER1_SETENA_bf(const void *const hw, hri_nvic_nviciser1_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICISER1.reg |= NVIC_NVICISER1_SETENA(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nviciser1_reg_t hri_nvic_get_NVICISER1_SETENA_bf(const void *const        hw,
                                                                        hri_nvic_nviciser1_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Nvic *)hw)->NVICISER1.reg;
	tmp = (tmp & NVIC_NVICISER1_SETENA(mask)) >> 0;
	return tmp;
}

static inline void hri_nvic_write_NVICISER1_SETENA_bf(const void *const hw, hri_nvic_nviciser1_reg_t data)
{
	uint32_t tmp;
	NVIC_CRITICAL_SECTION_ENTER();
	tmp = ((Nvic *)hw)->NVICISER1.reg;
	tmp &= ~NVIC_NVICISER1_SETENA_Msk;
	tmp |= NVIC_NVICISER1_SETENA(data);
	((Nvic *)hw)->NVICISER1.reg = tmp;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICISER1_SETENA_bf(const void *const hw, hri_nvic_nviciser1_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICISER1.reg &= ~NVIC_NVICISER1_SETENA(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICISER1_SETENA_bf(const void *const hw, hri_nvic_nviciser1_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICISER1.reg ^= NVIC_NVICISER1_SETENA(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nviciser1_reg_t hri_nvic_read_NVICISER1_SETENA_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Nvic *)hw)->NVICISER1.reg;
	tmp = (tmp & NVIC_NVICISER1_SETENA_Msk) >> 0;
	return tmp;
}

static inline void hri_nvic_set_NVICISER1_reg(const void *const hw, hri_nvic_nviciser1_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICISER1.reg |= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nviciser1_reg_t hri_nvic_get_NVICISER1_reg(const void *const hw, hri_nvic_nviciser1_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Nvic *)hw)->NVICISER1.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_nvic_write_NVICISER1_reg(const void *const hw, hri_nvic_nviciser1_reg_t data)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICISER1.reg = data;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICISER1_reg(const void *const hw, hri_nvic_nviciser1_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICISER1.reg &= ~mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICISER1_reg(const void *const hw, hri_nvic_nviciser1_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICISER1.reg ^= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nviciser1_reg_t hri_nvic_read_NVICISER1_reg(const void *const hw)
{
	return ((Nvic *)hw)->NVICISER1.reg;
}

static inline void hri_nvic_set_NVICISER2_SETENA_bf(const void *const hw, hri_nvic_nviciser2_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICISER2.reg |= NVIC_NVICISER2_SETENA(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nviciser2_reg_t hri_nvic_get_NVICISER2_SETENA_bf(const void *const        hw,
                                                                        hri_nvic_nviciser2_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Nvic *)hw)->NVICISER2.reg;
	tmp = (tmp & NVIC_NVICISER2_SETENA(mask)) >> 0;
	return tmp;
}

static inline void hri_nvic_write_NVICISER2_SETENA_bf(const void *const hw, hri_nvic_nviciser2_reg_t data)
{
	uint32_t tmp;
	NVIC_CRITICAL_SECTION_ENTER();
	tmp = ((Nvic *)hw)->NVICISER2.reg;
	tmp &= ~NVIC_NVICISER2_SETENA_Msk;
	tmp |= NVIC_NVICISER2_SETENA(data);
	((Nvic *)hw)->NVICISER2.reg = tmp;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICISER2_SETENA_bf(const void *const hw, hri_nvic_nviciser2_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICISER2.reg &= ~NVIC_NVICISER2_SETENA(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICISER2_SETENA_bf(const void *const hw, hri_nvic_nviciser2_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICISER2.reg ^= NVIC_NVICISER2_SETENA(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nviciser2_reg_t hri_nvic_read_NVICISER2_SETENA_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Nvic *)hw)->NVICISER2.reg;
	tmp = (tmp & NVIC_NVICISER2_SETENA_Msk) >> 0;
	return tmp;
}

static inline void hri_nvic_set_NVICISER2_reg(const void *const hw, hri_nvic_nviciser2_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICISER2.reg |= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nviciser2_reg_t hri_nvic_get_NVICISER2_reg(const void *const hw, hri_nvic_nviciser2_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Nvic *)hw)->NVICISER2.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_nvic_write_NVICISER2_reg(const void *const hw, hri_nvic_nviciser2_reg_t data)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICISER2.reg = data;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICISER2_reg(const void *const hw, hri_nvic_nviciser2_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICISER2.reg &= ~mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICISER2_reg(const void *const hw, hri_nvic_nviciser2_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICISER2.reg ^= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nviciser2_reg_t hri_nvic_read_NVICISER2_reg(const void *const hw)
{
	return ((Nvic *)hw)->NVICISER2.reg;
}

static inline void hri_nvic_set_NVICISER3_SETENA_bf(const void *const hw, hri_nvic_nviciser3_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICISER3.reg |= NVIC_NVICISER3_SETENA(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nviciser3_reg_t hri_nvic_get_NVICISER3_SETENA_bf(const void *const        hw,
                                                                        hri_nvic_nviciser3_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Nvic *)hw)->NVICISER3.reg;
	tmp = (tmp & NVIC_NVICISER3_SETENA(mask)) >> 0;
	return tmp;
}

static inline void hri_nvic_write_NVICISER3_SETENA_bf(const void *const hw, hri_nvic_nviciser3_reg_t data)
{
	uint32_t tmp;
	NVIC_CRITICAL_SECTION_ENTER();
	tmp = ((Nvic *)hw)->NVICISER3.reg;
	tmp &= ~NVIC_NVICISER3_SETENA_Msk;
	tmp |= NVIC_NVICISER3_SETENA(data);
	((Nvic *)hw)->NVICISER3.reg = tmp;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICISER3_SETENA_bf(const void *const hw, hri_nvic_nviciser3_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICISER3.reg &= ~NVIC_NVICISER3_SETENA(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICISER3_SETENA_bf(const void *const hw, hri_nvic_nviciser3_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICISER3.reg ^= NVIC_NVICISER3_SETENA(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nviciser3_reg_t hri_nvic_read_NVICISER3_SETENA_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Nvic *)hw)->NVICISER3.reg;
	tmp = (tmp & NVIC_NVICISER3_SETENA_Msk) >> 0;
	return tmp;
}

static inline void hri_nvic_set_NVICISER3_reg(const void *const hw, hri_nvic_nviciser3_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICISER3.reg |= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nviciser3_reg_t hri_nvic_get_NVICISER3_reg(const void *const hw, hri_nvic_nviciser3_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Nvic *)hw)->NVICISER3.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_nvic_write_NVICISER3_reg(const void *const hw, hri_nvic_nviciser3_reg_t data)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICISER3.reg = data;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICISER3_reg(const void *const hw, hri_nvic_nviciser3_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICISER3.reg &= ~mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICISER3_reg(const void *const hw, hri_nvic_nviciser3_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICISER3.reg ^= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nviciser3_reg_t hri_nvic_read_NVICISER3_reg(const void *const hw)
{
	return ((Nvic *)hw)->NVICISER3.reg;
}

static inline void hri_nvic_set_NVICICER0_CLRENA_bf(const void *const hw, hri_nvic_nvicicer0_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICICER0.reg |= NVIC_NVICICER0_CLRENA(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicicer0_reg_t hri_nvic_get_NVICICER0_CLRENA_bf(const void *const        hw,
                                                                        hri_nvic_nvicicer0_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Nvic *)hw)->NVICICER0.reg;
	tmp = (tmp & NVIC_NVICICER0_CLRENA(mask)) >> 0;
	return tmp;
}

static inline void hri_nvic_write_NVICICER0_CLRENA_bf(const void *const hw, hri_nvic_nvicicer0_reg_t data)
{
	uint32_t tmp;
	NVIC_CRITICAL_SECTION_ENTER();
	tmp = ((Nvic *)hw)->NVICICER0.reg;
	tmp &= ~NVIC_NVICICER0_CLRENA_Msk;
	tmp |= NVIC_NVICICER0_CLRENA(data);
	((Nvic *)hw)->NVICICER0.reg = tmp;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICICER0_CLRENA_bf(const void *const hw, hri_nvic_nvicicer0_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICICER0.reg &= ~NVIC_NVICICER0_CLRENA(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICICER0_CLRENA_bf(const void *const hw, hri_nvic_nvicicer0_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICICER0.reg ^= NVIC_NVICICER0_CLRENA(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicicer0_reg_t hri_nvic_read_NVICICER0_CLRENA_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Nvic *)hw)->NVICICER0.reg;
	tmp = (tmp & NVIC_NVICICER0_CLRENA_Msk) >> 0;
	return tmp;
}

static inline void hri_nvic_set_NVICICER0_reg(const void *const hw, hri_nvic_nvicicer0_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICICER0.reg |= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicicer0_reg_t hri_nvic_get_NVICICER0_reg(const void *const hw, hri_nvic_nvicicer0_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Nvic *)hw)->NVICICER0.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_nvic_write_NVICICER0_reg(const void *const hw, hri_nvic_nvicicer0_reg_t data)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICICER0.reg = data;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICICER0_reg(const void *const hw, hri_nvic_nvicicer0_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICICER0.reg &= ~mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICICER0_reg(const void *const hw, hri_nvic_nvicicer0_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICICER0.reg ^= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicicer0_reg_t hri_nvic_read_NVICICER0_reg(const void *const hw)
{
	return ((Nvic *)hw)->NVICICER0.reg;
}

static inline void hri_nvic_set_NVICICER1_CLRENA_bf(const void *const hw, hri_nvic_nvicicer1_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICICER1.reg |= NVIC_NVICICER1_CLRENA(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicicer1_reg_t hri_nvic_get_NVICICER1_CLRENA_bf(const void *const        hw,
                                                                        hri_nvic_nvicicer1_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Nvic *)hw)->NVICICER1.reg;
	tmp = (tmp & NVIC_NVICICER1_CLRENA(mask)) >> 0;
	return tmp;
}

static inline void hri_nvic_write_NVICICER1_CLRENA_bf(const void *const hw, hri_nvic_nvicicer1_reg_t data)
{
	uint32_t tmp;
	NVIC_CRITICAL_SECTION_ENTER();
	tmp = ((Nvic *)hw)->NVICICER1.reg;
	tmp &= ~NVIC_NVICICER1_CLRENA_Msk;
	tmp |= NVIC_NVICICER1_CLRENA(data);
	((Nvic *)hw)->NVICICER1.reg = tmp;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICICER1_CLRENA_bf(const void *const hw, hri_nvic_nvicicer1_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICICER1.reg &= ~NVIC_NVICICER1_CLRENA(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICICER1_CLRENA_bf(const void *const hw, hri_nvic_nvicicer1_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICICER1.reg ^= NVIC_NVICICER1_CLRENA(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicicer1_reg_t hri_nvic_read_NVICICER1_CLRENA_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Nvic *)hw)->NVICICER1.reg;
	tmp = (tmp & NVIC_NVICICER1_CLRENA_Msk) >> 0;
	return tmp;
}

static inline void hri_nvic_set_NVICICER1_reg(const void *const hw, hri_nvic_nvicicer1_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICICER1.reg |= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicicer1_reg_t hri_nvic_get_NVICICER1_reg(const void *const hw, hri_nvic_nvicicer1_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Nvic *)hw)->NVICICER1.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_nvic_write_NVICICER1_reg(const void *const hw, hri_nvic_nvicicer1_reg_t data)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICICER1.reg = data;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICICER1_reg(const void *const hw, hri_nvic_nvicicer1_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICICER1.reg &= ~mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICICER1_reg(const void *const hw, hri_nvic_nvicicer1_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICICER1.reg ^= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicicer1_reg_t hri_nvic_read_NVICICER1_reg(const void *const hw)
{
	return ((Nvic *)hw)->NVICICER1.reg;
}

static inline void hri_nvic_set_NVICICER2_CLRENA_bf(const void *const hw, hri_nvic_nvicicer2_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICICER2.reg |= NVIC_NVICICER2_CLRENA(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicicer2_reg_t hri_nvic_get_NVICICER2_CLRENA_bf(const void *const        hw,
                                                                        hri_nvic_nvicicer2_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Nvic *)hw)->NVICICER2.reg;
	tmp = (tmp & NVIC_NVICICER2_CLRENA(mask)) >> 0;
	return tmp;
}

static inline void hri_nvic_write_NVICICER2_CLRENA_bf(const void *const hw, hri_nvic_nvicicer2_reg_t data)
{
	uint32_t tmp;
	NVIC_CRITICAL_SECTION_ENTER();
	tmp = ((Nvic *)hw)->NVICICER2.reg;
	tmp &= ~NVIC_NVICICER2_CLRENA_Msk;
	tmp |= NVIC_NVICICER2_CLRENA(data);
	((Nvic *)hw)->NVICICER2.reg = tmp;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICICER2_CLRENA_bf(const void *const hw, hri_nvic_nvicicer2_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICICER2.reg &= ~NVIC_NVICICER2_CLRENA(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICICER2_CLRENA_bf(const void *const hw, hri_nvic_nvicicer2_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICICER2.reg ^= NVIC_NVICICER2_CLRENA(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicicer2_reg_t hri_nvic_read_NVICICER2_CLRENA_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Nvic *)hw)->NVICICER2.reg;
	tmp = (tmp & NVIC_NVICICER2_CLRENA_Msk) >> 0;
	return tmp;
}

static inline void hri_nvic_set_NVICICER2_reg(const void *const hw, hri_nvic_nvicicer2_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICICER2.reg |= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicicer2_reg_t hri_nvic_get_NVICICER2_reg(const void *const hw, hri_nvic_nvicicer2_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Nvic *)hw)->NVICICER2.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_nvic_write_NVICICER2_reg(const void *const hw, hri_nvic_nvicicer2_reg_t data)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICICER2.reg = data;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICICER2_reg(const void *const hw, hri_nvic_nvicicer2_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICICER2.reg &= ~mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICICER2_reg(const void *const hw, hri_nvic_nvicicer2_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICICER2.reg ^= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicicer2_reg_t hri_nvic_read_NVICICER2_reg(const void *const hw)
{
	return ((Nvic *)hw)->NVICICER2.reg;
}

static inline void hri_nvic_set_NVICICER3_CLRENA_bf(const void *const hw, hri_nvic_nvicicer3_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICICER3.reg |= NVIC_NVICICER3_CLRENA(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicicer3_reg_t hri_nvic_get_NVICICER3_CLRENA_bf(const void *const        hw,
                                                                        hri_nvic_nvicicer3_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Nvic *)hw)->NVICICER3.reg;
	tmp = (tmp & NVIC_NVICICER3_CLRENA(mask)) >> 0;
	return tmp;
}

static inline void hri_nvic_write_NVICICER3_CLRENA_bf(const void *const hw, hri_nvic_nvicicer3_reg_t data)
{
	uint32_t tmp;
	NVIC_CRITICAL_SECTION_ENTER();
	tmp = ((Nvic *)hw)->NVICICER3.reg;
	tmp &= ~NVIC_NVICICER3_CLRENA_Msk;
	tmp |= NVIC_NVICICER3_CLRENA(data);
	((Nvic *)hw)->NVICICER3.reg = tmp;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICICER3_CLRENA_bf(const void *const hw, hri_nvic_nvicicer3_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICICER3.reg &= ~NVIC_NVICICER3_CLRENA(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICICER3_CLRENA_bf(const void *const hw, hri_nvic_nvicicer3_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICICER3.reg ^= NVIC_NVICICER3_CLRENA(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicicer3_reg_t hri_nvic_read_NVICICER3_CLRENA_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Nvic *)hw)->NVICICER3.reg;
	tmp = (tmp & NVIC_NVICICER3_CLRENA_Msk) >> 0;
	return tmp;
}

static inline void hri_nvic_set_NVICICER3_reg(const void *const hw, hri_nvic_nvicicer3_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICICER3.reg |= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicicer3_reg_t hri_nvic_get_NVICICER3_reg(const void *const hw, hri_nvic_nvicicer3_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Nvic *)hw)->NVICICER3.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_nvic_write_NVICICER3_reg(const void *const hw, hri_nvic_nvicicer3_reg_t data)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICICER3.reg = data;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICICER3_reg(const void *const hw, hri_nvic_nvicicer3_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICICER3.reg &= ~mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICICER3_reg(const void *const hw, hri_nvic_nvicicer3_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICICER3.reg ^= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicicer3_reg_t hri_nvic_read_NVICICER3_reg(const void *const hw)
{
	return ((Nvic *)hw)->NVICICER3.reg;
}

static inline void hri_nvic_set_NVICISPR0_SETPEND_bf(const void *const hw, hri_nvic_nvicispr0_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICISPR0.reg |= NVIC_NVICISPR0_SETPEND(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicispr0_reg_t hri_nvic_get_NVICISPR0_SETPEND_bf(const void *const        hw,
                                                                         hri_nvic_nvicispr0_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Nvic *)hw)->NVICISPR0.reg;
	tmp = (tmp & NVIC_NVICISPR0_SETPEND(mask)) >> 0;
	return tmp;
}

static inline void hri_nvic_write_NVICISPR0_SETPEND_bf(const void *const hw, hri_nvic_nvicispr0_reg_t data)
{
	uint32_t tmp;
	NVIC_CRITICAL_SECTION_ENTER();
	tmp = ((Nvic *)hw)->NVICISPR0.reg;
	tmp &= ~NVIC_NVICISPR0_SETPEND_Msk;
	tmp |= NVIC_NVICISPR0_SETPEND(data);
	((Nvic *)hw)->NVICISPR0.reg = tmp;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICISPR0_SETPEND_bf(const void *const hw, hri_nvic_nvicispr0_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICISPR0.reg &= ~NVIC_NVICISPR0_SETPEND(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICISPR0_SETPEND_bf(const void *const hw, hri_nvic_nvicispr0_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICISPR0.reg ^= NVIC_NVICISPR0_SETPEND(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicispr0_reg_t hri_nvic_read_NVICISPR0_SETPEND_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Nvic *)hw)->NVICISPR0.reg;
	tmp = (tmp & NVIC_NVICISPR0_SETPEND_Msk) >> 0;
	return tmp;
}

static inline void hri_nvic_set_NVICISPR0_reg(const void *const hw, hri_nvic_nvicispr0_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICISPR0.reg |= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicispr0_reg_t hri_nvic_get_NVICISPR0_reg(const void *const hw, hri_nvic_nvicispr0_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Nvic *)hw)->NVICISPR0.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_nvic_write_NVICISPR0_reg(const void *const hw, hri_nvic_nvicispr0_reg_t data)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICISPR0.reg = data;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICISPR0_reg(const void *const hw, hri_nvic_nvicispr0_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICISPR0.reg &= ~mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICISPR0_reg(const void *const hw, hri_nvic_nvicispr0_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICISPR0.reg ^= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicispr0_reg_t hri_nvic_read_NVICISPR0_reg(const void *const hw)
{
	return ((Nvic *)hw)->NVICISPR0.reg;
}

static inline void hri_nvic_set_NVICISPR1_SETPEND_bf(const void *const hw, hri_nvic_nvicispr1_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICISPR1.reg |= NVIC_NVICISPR1_SETPEND(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicispr1_reg_t hri_nvic_get_NVICISPR1_SETPEND_bf(const void *const        hw,
                                                                         hri_nvic_nvicispr1_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Nvic *)hw)->NVICISPR1.reg;
	tmp = (tmp & NVIC_NVICISPR1_SETPEND(mask)) >> 0;
	return tmp;
}

static inline void hri_nvic_write_NVICISPR1_SETPEND_bf(const void *const hw, hri_nvic_nvicispr1_reg_t data)
{
	uint32_t tmp;
	NVIC_CRITICAL_SECTION_ENTER();
	tmp = ((Nvic *)hw)->NVICISPR1.reg;
	tmp &= ~NVIC_NVICISPR1_SETPEND_Msk;
	tmp |= NVIC_NVICISPR1_SETPEND(data);
	((Nvic *)hw)->NVICISPR1.reg = tmp;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICISPR1_SETPEND_bf(const void *const hw, hri_nvic_nvicispr1_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICISPR1.reg &= ~NVIC_NVICISPR1_SETPEND(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICISPR1_SETPEND_bf(const void *const hw, hri_nvic_nvicispr1_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICISPR1.reg ^= NVIC_NVICISPR1_SETPEND(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicispr1_reg_t hri_nvic_read_NVICISPR1_SETPEND_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Nvic *)hw)->NVICISPR1.reg;
	tmp = (tmp & NVIC_NVICISPR1_SETPEND_Msk) >> 0;
	return tmp;
}

static inline void hri_nvic_set_NVICISPR1_reg(const void *const hw, hri_nvic_nvicispr1_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICISPR1.reg |= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicispr1_reg_t hri_nvic_get_NVICISPR1_reg(const void *const hw, hri_nvic_nvicispr1_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Nvic *)hw)->NVICISPR1.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_nvic_write_NVICISPR1_reg(const void *const hw, hri_nvic_nvicispr1_reg_t data)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICISPR1.reg = data;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICISPR1_reg(const void *const hw, hri_nvic_nvicispr1_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICISPR1.reg &= ~mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICISPR1_reg(const void *const hw, hri_nvic_nvicispr1_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICISPR1.reg ^= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicispr1_reg_t hri_nvic_read_NVICISPR1_reg(const void *const hw)
{
	return ((Nvic *)hw)->NVICISPR1.reg;
}

static inline void hri_nvic_set_NVICISPR2_SETPEND_bf(const void *const hw, hri_nvic_nvicispr2_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICISPR2.reg |= NVIC_NVICISPR2_SETPEND(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicispr2_reg_t hri_nvic_get_NVICISPR2_SETPEND_bf(const void *const        hw,
                                                                         hri_nvic_nvicispr2_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Nvic *)hw)->NVICISPR2.reg;
	tmp = (tmp & NVIC_NVICISPR2_SETPEND(mask)) >> 0;
	return tmp;
}

static inline void hri_nvic_write_NVICISPR2_SETPEND_bf(const void *const hw, hri_nvic_nvicispr2_reg_t data)
{
	uint32_t tmp;
	NVIC_CRITICAL_SECTION_ENTER();
	tmp = ((Nvic *)hw)->NVICISPR2.reg;
	tmp &= ~NVIC_NVICISPR2_SETPEND_Msk;
	tmp |= NVIC_NVICISPR2_SETPEND(data);
	((Nvic *)hw)->NVICISPR2.reg = tmp;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICISPR2_SETPEND_bf(const void *const hw, hri_nvic_nvicispr2_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICISPR2.reg &= ~NVIC_NVICISPR2_SETPEND(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICISPR2_SETPEND_bf(const void *const hw, hri_nvic_nvicispr2_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICISPR2.reg ^= NVIC_NVICISPR2_SETPEND(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicispr2_reg_t hri_nvic_read_NVICISPR2_SETPEND_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Nvic *)hw)->NVICISPR2.reg;
	tmp = (tmp & NVIC_NVICISPR2_SETPEND_Msk) >> 0;
	return tmp;
}

static inline void hri_nvic_set_NVICISPR2_reg(const void *const hw, hri_nvic_nvicispr2_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICISPR2.reg |= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicispr2_reg_t hri_nvic_get_NVICISPR2_reg(const void *const hw, hri_nvic_nvicispr2_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Nvic *)hw)->NVICISPR2.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_nvic_write_NVICISPR2_reg(const void *const hw, hri_nvic_nvicispr2_reg_t data)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICISPR2.reg = data;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICISPR2_reg(const void *const hw, hri_nvic_nvicispr2_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICISPR2.reg &= ~mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICISPR2_reg(const void *const hw, hri_nvic_nvicispr2_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICISPR2.reg ^= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicispr2_reg_t hri_nvic_read_NVICISPR2_reg(const void *const hw)
{
	return ((Nvic *)hw)->NVICISPR2.reg;
}

static inline void hri_nvic_set_NVICISPR3_SETPEND_bf(const void *const hw, hri_nvic_nvicispr3_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICISPR3.reg |= NVIC_NVICISPR3_SETPEND(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicispr3_reg_t hri_nvic_get_NVICISPR3_SETPEND_bf(const void *const        hw,
                                                                         hri_nvic_nvicispr3_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Nvic *)hw)->NVICISPR3.reg;
	tmp = (tmp & NVIC_NVICISPR3_SETPEND(mask)) >> 0;
	return tmp;
}

static inline void hri_nvic_write_NVICISPR3_SETPEND_bf(const void *const hw, hri_nvic_nvicispr3_reg_t data)
{
	uint32_t tmp;
	NVIC_CRITICAL_SECTION_ENTER();
	tmp = ((Nvic *)hw)->NVICISPR3.reg;
	tmp &= ~NVIC_NVICISPR3_SETPEND_Msk;
	tmp |= NVIC_NVICISPR3_SETPEND(data);
	((Nvic *)hw)->NVICISPR3.reg = tmp;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICISPR3_SETPEND_bf(const void *const hw, hri_nvic_nvicispr3_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICISPR3.reg &= ~NVIC_NVICISPR3_SETPEND(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICISPR3_SETPEND_bf(const void *const hw, hri_nvic_nvicispr3_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICISPR3.reg ^= NVIC_NVICISPR3_SETPEND(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicispr3_reg_t hri_nvic_read_NVICISPR3_SETPEND_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Nvic *)hw)->NVICISPR3.reg;
	tmp = (tmp & NVIC_NVICISPR3_SETPEND_Msk) >> 0;
	return tmp;
}

static inline void hri_nvic_set_NVICISPR3_reg(const void *const hw, hri_nvic_nvicispr3_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICISPR3.reg |= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicispr3_reg_t hri_nvic_get_NVICISPR3_reg(const void *const hw, hri_nvic_nvicispr3_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Nvic *)hw)->NVICISPR3.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_nvic_write_NVICISPR3_reg(const void *const hw, hri_nvic_nvicispr3_reg_t data)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICISPR3.reg = data;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICISPR3_reg(const void *const hw, hri_nvic_nvicispr3_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICISPR3.reg &= ~mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICISPR3_reg(const void *const hw, hri_nvic_nvicispr3_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICISPR3.reg ^= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicispr3_reg_t hri_nvic_read_NVICISPR3_reg(const void *const hw)
{
	return ((Nvic *)hw)->NVICISPR3.reg;
}

static inline void hri_nvic_set_NVICICPR0_CLRPEND_bf(const void *const hw, hri_nvic_nvicicpr0_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICICPR0.reg |= NVIC_NVICICPR0_CLRPEND(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicicpr0_reg_t hri_nvic_get_NVICICPR0_CLRPEND_bf(const void *const        hw,
                                                                         hri_nvic_nvicicpr0_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Nvic *)hw)->NVICICPR0.reg;
	tmp = (tmp & NVIC_NVICICPR0_CLRPEND(mask)) >> 0;
	return tmp;
}

static inline void hri_nvic_write_NVICICPR0_CLRPEND_bf(const void *const hw, hri_nvic_nvicicpr0_reg_t data)
{
	uint32_t tmp;
	NVIC_CRITICAL_SECTION_ENTER();
	tmp = ((Nvic *)hw)->NVICICPR0.reg;
	tmp &= ~NVIC_NVICICPR0_CLRPEND_Msk;
	tmp |= NVIC_NVICICPR0_CLRPEND(data);
	((Nvic *)hw)->NVICICPR0.reg = tmp;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICICPR0_CLRPEND_bf(const void *const hw, hri_nvic_nvicicpr0_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICICPR0.reg &= ~NVIC_NVICICPR0_CLRPEND(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICICPR0_CLRPEND_bf(const void *const hw, hri_nvic_nvicicpr0_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICICPR0.reg ^= NVIC_NVICICPR0_CLRPEND(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicicpr0_reg_t hri_nvic_read_NVICICPR0_CLRPEND_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Nvic *)hw)->NVICICPR0.reg;
	tmp = (tmp & NVIC_NVICICPR0_CLRPEND_Msk) >> 0;
	return tmp;
}

static inline void hri_nvic_set_NVICICPR0_reg(const void *const hw, hri_nvic_nvicicpr0_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICICPR0.reg |= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicicpr0_reg_t hri_nvic_get_NVICICPR0_reg(const void *const hw, hri_nvic_nvicicpr0_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Nvic *)hw)->NVICICPR0.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_nvic_write_NVICICPR0_reg(const void *const hw, hri_nvic_nvicicpr0_reg_t data)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICICPR0.reg = data;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICICPR0_reg(const void *const hw, hri_nvic_nvicicpr0_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICICPR0.reg &= ~mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICICPR0_reg(const void *const hw, hri_nvic_nvicicpr0_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICICPR0.reg ^= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicicpr0_reg_t hri_nvic_read_NVICICPR0_reg(const void *const hw)
{
	return ((Nvic *)hw)->NVICICPR0.reg;
}

static inline void hri_nvic_set_NVICICPR1_CLRPEND_bf(const void *const hw, hri_nvic_nvicicpr1_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICICPR1.reg |= NVIC_NVICICPR1_CLRPEND(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicicpr1_reg_t hri_nvic_get_NVICICPR1_CLRPEND_bf(const void *const        hw,
                                                                         hri_nvic_nvicicpr1_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Nvic *)hw)->NVICICPR1.reg;
	tmp = (tmp & NVIC_NVICICPR1_CLRPEND(mask)) >> 0;
	return tmp;
}

static inline void hri_nvic_write_NVICICPR1_CLRPEND_bf(const void *const hw, hri_nvic_nvicicpr1_reg_t data)
{
	uint32_t tmp;
	NVIC_CRITICAL_SECTION_ENTER();
	tmp = ((Nvic *)hw)->NVICICPR1.reg;
	tmp &= ~NVIC_NVICICPR1_CLRPEND_Msk;
	tmp |= NVIC_NVICICPR1_CLRPEND(data);
	((Nvic *)hw)->NVICICPR1.reg = tmp;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICICPR1_CLRPEND_bf(const void *const hw, hri_nvic_nvicicpr1_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICICPR1.reg &= ~NVIC_NVICICPR1_CLRPEND(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICICPR1_CLRPEND_bf(const void *const hw, hri_nvic_nvicicpr1_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICICPR1.reg ^= NVIC_NVICICPR1_CLRPEND(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicicpr1_reg_t hri_nvic_read_NVICICPR1_CLRPEND_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Nvic *)hw)->NVICICPR1.reg;
	tmp = (tmp & NVIC_NVICICPR1_CLRPEND_Msk) >> 0;
	return tmp;
}

static inline void hri_nvic_set_NVICICPR1_reg(const void *const hw, hri_nvic_nvicicpr1_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICICPR1.reg |= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicicpr1_reg_t hri_nvic_get_NVICICPR1_reg(const void *const hw, hri_nvic_nvicicpr1_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Nvic *)hw)->NVICICPR1.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_nvic_write_NVICICPR1_reg(const void *const hw, hri_nvic_nvicicpr1_reg_t data)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICICPR1.reg = data;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICICPR1_reg(const void *const hw, hri_nvic_nvicicpr1_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICICPR1.reg &= ~mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICICPR1_reg(const void *const hw, hri_nvic_nvicicpr1_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICICPR1.reg ^= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicicpr1_reg_t hri_nvic_read_NVICICPR1_reg(const void *const hw)
{
	return ((Nvic *)hw)->NVICICPR1.reg;
}

static inline void hri_nvic_set_NVICICPR2_CLRPEND_bf(const void *const hw, hri_nvic_nvicicpr2_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICICPR2.reg |= NVIC_NVICICPR2_CLRPEND(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicicpr2_reg_t hri_nvic_get_NVICICPR2_CLRPEND_bf(const void *const        hw,
                                                                         hri_nvic_nvicicpr2_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Nvic *)hw)->NVICICPR2.reg;
	tmp = (tmp & NVIC_NVICICPR2_CLRPEND(mask)) >> 0;
	return tmp;
}

static inline void hri_nvic_write_NVICICPR2_CLRPEND_bf(const void *const hw, hri_nvic_nvicicpr2_reg_t data)
{
	uint32_t tmp;
	NVIC_CRITICAL_SECTION_ENTER();
	tmp = ((Nvic *)hw)->NVICICPR2.reg;
	tmp &= ~NVIC_NVICICPR2_CLRPEND_Msk;
	tmp |= NVIC_NVICICPR2_CLRPEND(data);
	((Nvic *)hw)->NVICICPR2.reg = tmp;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICICPR2_CLRPEND_bf(const void *const hw, hri_nvic_nvicicpr2_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICICPR2.reg &= ~NVIC_NVICICPR2_CLRPEND(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICICPR2_CLRPEND_bf(const void *const hw, hri_nvic_nvicicpr2_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICICPR2.reg ^= NVIC_NVICICPR2_CLRPEND(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicicpr2_reg_t hri_nvic_read_NVICICPR2_CLRPEND_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Nvic *)hw)->NVICICPR2.reg;
	tmp = (tmp & NVIC_NVICICPR2_CLRPEND_Msk) >> 0;
	return tmp;
}

static inline void hri_nvic_set_NVICICPR2_reg(const void *const hw, hri_nvic_nvicicpr2_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICICPR2.reg |= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicicpr2_reg_t hri_nvic_get_NVICICPR2_reg(const void *const hw, hri_nvic_nvicicpr2_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Nvic *)hw)->NVICICPR2.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_nvic_write_NVICICPR2_reg(const void *const hw, hri_nvic_nvicicpr2_reg_t data)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICICPR2.reg = data;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICICPR2_reg(const void *const hw, hri_nvic_nvicicpr2_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICICPR2.reg &= ~mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICICPR2_reg(const void *const hw, hri_nvic_nvicicpr2_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICICPR2.reg ^= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicicpr2_reg_t hri_nvic_read_NVICICPR2_reg(const void *const hw)
{
	return ((Nvic *)hw)->NVICICPR2.reg;
}

static inline void hri_nvic_set_NVICICPR3_CLRPEND_bf(const void *const hw, hri_nvic_nvicicpr3_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICICPR3.reg |= NVIC_NVICICPR3_CLRPEND(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicicpr3_reg_t hri_nvic_get_NVICICPR3_CLRPEND_bf(const void *const        hw,
                                                                         hri_nvic_nvicicpr3_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Nvic *)hw)->NVICICPR3.reg;
	tmp = (tmp & NVIC_NVICICPR3_CLRPEND(mask)) >> 0;
	return tmp;
}

static inline void hri_nvic_write_NVICICPR3_CLRPEND_bf(const void *const hw, hri_nvic_nvicicpr3_reg_t data)
{
	uint32_t tmp;
	NVIC_CRITICAL_SECTION_ENTER();
	tmp = ((Nvic *)hw)->NVICICPR3.reg;
	tmp &= ~NVIC_NVICICPR3_CLRPEND_Msk;
	tmp |= NVIC_NVICICPR3_CLRPEND(data);
	((Nvic *)hw)->NVICICPR3.reg = tmp;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICICPR3_CLRPEND_bf(const void *const hw, hri_nvic_nvicicpr3_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICICPR3.reg &= ~NVIC_NVICICPR3_CLRPEND(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICICPR3_CLRPEND_bf(const void *const hw, hri_nvic_nvicicpr3_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICICPR3.reg ^= NVIC_NVICICPR3_CLRPEND(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicicpr3_reg_t hri_nvic_read_NVICICPR3_CLRPEND_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Nvic *)hw)->NVICICPR3.reg;
	tmp = (tmp & NVIC_NVICICPR3_CLRPEND_Msk) >> 0;
	return tmp;
}

static inline void hri_nvic_set_NVICICPR3_reg(const void *const hw, hri_nvic_nvicicpr3_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICICPR3.reg |= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicicpr3_reg_t hri_nvic_get_NVICICPR3_reg(const void *const hw, hri_nvic_nvicicpr3_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Nvic *)hw)->NVICICPR3.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_nvic_write_NVICICPR3_reg(const void *const hw, hri_nvic_nvicicpr3_reg_t data)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICICPR3.reg = data;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICICPR3_reg(const void *const hw, hri_nvic_nvicicpr3_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICICPR3.reg &= ~mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICICPR3_reg(const void *const hw, hri_nvic_nvicicpr3_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICICPR3.reg ^= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicicpr3_reg_t hri_nvic_read_NVICICPR3_reg(const void *const hw)
{
	return ((Nvic *)hw)->NVICICPR3.reg;
}

static inline void hri_nvic_set_NVICIABR0_ACTIVE_bf(const void *const hw, hri_nvic_nviciabr0_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIABR0.reg |= NVIC_NVICIABR0_ACTIVE(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nviciabr0_reg_t hri_nvic_get_NVICIABR0_ACTIVE_bf(const void *const        hw,
                                                                        hri_nvic_nviciabr0_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Nvic *)hw)->NVICIABR0.reg;
	tmp = (tmp & NVIC_NVICIABR0_ACTIVE(mask)) >> 0;
	return tmp;
}

static inline void hri_nvic_write_NVICIABR0_ACTIVE_bf(const void *const hw, hri_nvic_nviciabr0_reg_t data)
{
	uint32_t tmp;
	NVIC_CRITICAL_SECTION_ENTER();
	tmp = ((Nvic *)hw)->NVICIABR0.reg;
	tmp &= ~NVIC_NVICIABR0_ACTIVE_Msk;
	tmp |= NVIC_NVICIABR0_ACTIVE(data);
	((Nvic *)hw)->NVICIABR0.reg = tmp;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIABR0_ACTIVE_bf(const void *const hw, hri_nvic_nviciabr0_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIABR0.reg &= ~NVIC_NVICIABR0_ACTIVE(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIABR0_ACTIVE_bf(const void *const hw, hri_nvic_nviciabr0_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIABR0.reg ^= NVIC_NVICIABR0_ACTIVE(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nviciabr0_reg_t hri_nvic_read_NVICIABR0_ACTIVE_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Nvic *)hw)->NVICIABR0.reg;
	tmp = (tmp & NVIC_NVICIABR0_ACTIVE_Msk) >> 0;
	return tmp;
}

static inline void hri_nvic_set_NVICIABR0_reg(const void *const hw, hri_nvic_nviciabr0_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIABR0.reg |= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nviciabr0_reg_t hri_nvic_get_NVICIABR0_reg(const void *const hw, hri_nvic_nviciabr0_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Nvic *)hw)->NVICIABR0.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_nvic_write_NVICIABR0_reg(const void *const hw, hri_nvic_nviciabr0_reg_t data)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIABR0.reg = data;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIABR0_reg(const void *const hw, hri_nvic_nviciabr0_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIABR0.reg &= ~mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIABR0_reg(const void *const hw, hri_nvic_nviciabr0_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIABR0.reg ^= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nviciabr0_reg_t hri_nvic_read_NVICIABR0_reg(const void *const hw)
{
	return ((Nvic *)hw)->NVICIABR0.reg;
}

static inline void hri_nvic_set_NVICIABR1_ACTIVE_bf(const void *const hw, hri_nvic_nviciabr1_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIABR1.reg |= NVIC_NVICIABR1_ACTIVE(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nviciabr1_reg_t hri_nvic_get_NVICIABR1_ACTIVE_bf(const void *const        hw,
                                                                        hri_nvic_nviciabr1_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Nvic *)hw)->NVICIABR1.reg;
	tmp = (tmp & NVIC_NVICIABR1_ACTIVE(mask)) >> 0;
	return tmp;
}

static inline void hri_nvic_write_NVICIABR1_ACTIVE_bf(const void *const hw, hri_nvic_nviciabr1_reg_t data)
{
	uint32_t tmp;
	NVIC_CRITICAL_SECTION_ENTER();
	tmp = ((Nvic *)hw)->NVICIABR1.reg;
	tmp &= ~NVIC_NVICIABR1_ACTIVE_Msk;
	tmp |= NVIC_NVICIABR1_ACTIVE(data);
	((Nvic *)hw)->NVICIABR1.reg = tmp;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIABR1_ACTIVE_bf(const void *const hw, hri_nvic_nviciabr1_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIABR1.reg &= ~NVIC_NVICIABR1_ACTIVE(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIABR1_ACTIVE_bf(const void *const hw, hri_nvic_nviciabr1_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIABR1.reg ^= NVIC_NVICIABR1_ACTIVE(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nviciabr1_reg_t hri_nvic_read_NVICIABR1_ACTIVE_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Nvic *)hw)->NVICIABR1.reg;
	tmp = (tmp & NVIC_NVICIABR1_ACTIVE_Msk) >> 0;
	return tmp;
}

static inline void hri_nvic_set_NVICIABR1_reg(const void *const hw, hri_nvic_nviciabr1_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIABR1.reg |= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nviciabr1_reg_t hri_nvic_get_NVICIABR1_reg(const void *const hw, hri_nvic_nviciabr1_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Nvic *)hw)->NVICIABR1.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_nvic_write_NVICIABR1_reg(const void *const hw, hri_nvic_nviciabr1_reg_t data)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIABR1.reg = data;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIABR1_reg(const void *const hw, hri_nvic_nviciabr1_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIABR1.reg &= ~mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIABR1_reg(const void *const hw, hri_nvic_nviciabr1_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIABR1.reg ^= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nviciabr1_reg_t hri_nvic_read_NVICIABR1_reg(const void *const hw)
{
	return ((Nvic *)hw)->NVICIABR1.reg;
}

static inline void hri_nvic_set_NVICIABR2_ACTIVE_bf(const void *const hw, hri_nvic_nviciabr2_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIABR2.reg |= NVIC_NVICIABR2_ACTIVE(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nviciabr2_reg_t hri_nvic_get_NVICIABR2_ACTIVE_bf(const void *const        hw,
                                                                        hri_nvic_nviciabr2_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Nvic *)hw)->NVICIABR2.reg;
	tmp = (tmp & NVIC_NVICIABR2_ACTIVE(mask)) >> 0;
	return tmp;
}

static inline void hri_nvic_write_NVICIABR2_ACTIVE_bf(const void *const hw, hri_nvic_nviciabr2_reg_t data)
{
	uint32_t tmp;
	NVIC_CRITICAL_SECTION_ENTER();
	tmp = ((Nvic *)hw)->NVICIABR2.reg;
	tmp &= ~NVIC_NVICIABR2_ACTIVE_Msk;
	tmp |= NVIC_NVICIABR2_ACTIVE(data);
	((Nvic *)hw)->NVICIABR2.reg = tmp;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIABR2_ACTIVE_bf(const void *const hw, hri_nvic_nviciabr2_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIABR2.reg &= ~NVIC_NVICIABR2_ACTIVE(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIABR2_ACTIVE_bf(const void *const hw, hri_nvic_nviciabr2_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIABR2.reg ^= NVIC_NVICIABR2_ACTIVE(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nviciabr2_reg_t hri_nvic_read_NVICIABR2_ACTIVE_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Nvic *)hw)->NVICIABR2.reg;
	tmp = (tmp & NVIC_NVICIABR2_ACTIVE_Msk) >> 0;
	return tmp;
}

static inline void hri_nvic_set_NVICIABR2_reg(const void *const hw, hri_nvic_nviciabr2_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIABR2.reg |= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nviciabr2_reg_t hri_nvic_get_NVICIABR2_reg(const void *const hw, hri_nvic_nviciabr2_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Nvic *)hw)->NVICIABR2.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_nvic_write_NVICIABR2_reg(const void *const hw, hri_nvic_nviciabr2_reg_t data)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIABR2.reg = data;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIABR2_reg(const void *const hw, hri_nvic_nviciabr2_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIABR2.reg &= ~mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIABR2_reg(const void *const hw, hri_nvic_nviciabr2_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIABR2.reg ^= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nviciabr2_reg_t hri_nvic_read_NVICIABR2_reg(const void *const hw)
{
	return ((Nvic *)hw)->NVICIABR2.reg;
}

static inline void hri_nvic_set_NVICIABR3_ACTIVE_bf(const void *const hw, hri_nvic_nviciabr3_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIABR3.reg |= NVIC_NVICIABR3_ACTIVE(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nviciabr3_reg_t hri_nvic_get_NVICIABR3_ACTIVE_bf(const void *const        hw,
                                                                        hri_nvic_nviciabr3_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Nvic *)hw)->NVICIABR3.reg;
	tmp = (tmp & NVIC_NVICIABR3_ACTIVE(mask)) >> 0;
	return tmp;
}

static inline void hri_nvic_write_NVICIABR3_ACTIVE_bf(const void *const hw, hri_nvic_nviciabr3_reg_t data)
{
	uint32_t tmp;
	NVIC_CRITICAL_SECTION_ENTER();
	tmp = ((Nvic *)hw)->NVICIABR3.reg;
	tmp &= ~NVIC_NVICIABR3_ACTIVE_Msk;
	tmp |= NVIC_NVICIABR3_ACTIVE(data);
	((Nvic *)hw)->NVICIABR3.reg = tmp;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIABR3_ACTIVE_bf(const void *const hw, hri_nvic_nviciabr3_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIABR3.reg &= ~NVIC_NVICIABR3_ACTIVE(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIABR3_ACTIVE_bf(const void *const hw, hri_nvic_nviciabr3_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIABR3.reg ^= NVIC_NVICIABR3_ACTIVE(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nviciabr3_reg_t hri_nvic_read_NVICIABR3_ACTIVE_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Nvic *)hw)->NVICIABR3.reg;
	tmp = (tmp & NVIC_NVICIABR3_ACTIVE_Msk) >> 0;
	return tmp;
}

static inline void hri_nvic_set_NVICIABR3_reg(const void *const hw, hri_nvic_nviciabr3_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIABR3.reg |= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nviciabr3_reg_t hri_nvic_get_NVICIABR3_reg(const void *const hw, hri_nvic_nviciabr3_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Nvic *)hw)->NVICIABR3.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_nvic_write_NVICIABR3_reg(const void *const hw, hri_nvic_nviciabr3_reg_t data)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIABR3.reg = data;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIABR3_reg(const void *const hw, hri_nvic_nviciabr3_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIABR3.reg &= ~mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIABR3_reg(const void *const hw, hri_nvic_nviciabr3_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIABR3.reg ^= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nviciabr3_reg_t hri_nvic_read_NVICIABR3_reg(const void *const hw)
{
	return ((Nvic *)hw)->NVICIABR3.reg;
}

static inline void hri_nvic_set_NVICIP0_PRI0_bf(const void *const hw, hri_nvic_nvicip0_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP0.reg |= NVIC_NVICIP0_PRI0(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip0_reg_t hri_nvic_get_NVICIP0_PRI0_bf(const void *const hw, hri_nvic_nvicip0_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP0.reg;
	tmp = (tmp & NVIC_NVICIP0_PRI0(mask)) >> 0;
	return tmp;
}

static inline void hri_nvic_write_NVICIP0_PRI0_bf(const void *const hw, hri_nvic_nvicip0_reg_t data)
{
	uint8_t tmp;
	NVIC_CRITICAL_SECTION_ENTER();
	tmp = ((Nvic *)hw)->NVICIP0.reg;
	tmp &= ~NVIC_NVICIP0_PRI0_Msk;
	tmp |= NVIC_NVICIP0_PRI0(data);
	((Nvic *)hw)->NVICIP0.reg = tmp;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP0_PRI0_bf(const void *const hw, hri_nvic_nvicip0_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP0.reg &= ~NVIC_NVICIP0_PRI0(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP0_PRI0_bf(const void *const hw, hri_nvic_nvicip0_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP0.reg ^= NVIC_NVICIP0_PRI0(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip0_reg_t hri_nvic_read_NVICIP0_PRI0_bf(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP0.reg;
	tmp = (tmp & NVIC_NVICIP0_PRI0_Msk) >> 0;
	return tmp;
}

static inline void hri_nvic_set_NVICIP0_reg(const void *const hw, hri_nvic_nvicip0_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP0.reg |= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip0_reg_t hri_nvic_get_NVICIP0_reg(const void *const hw, hri_nvic_nvicip0_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP0.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_nvic_write_NVICIP0_reg(const void *const hw, hri_nvic_nvicip0_reg_t data)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP0.reg = data;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP0_reg(const void *const hw, hri_nvic_nvicip0_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP0.reg &= ~mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP0_reg(const void *const hw, hri_nvic_nvicip0_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP0.reg ^= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip0_reg_t hri_nvic_read_NVICIP0_reg(const void *const hw)
{
	return ((Nvic *)hw)->NVICIP0.reg;
}

static inline void hri_nvic_set_NVICIP1_PRI1_bf(const void *const hw, hri_nvic_nvicip1_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP1.reg |= NVIC_NVICIP1_PRI1(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip1_reg_t hri_nvic_get_NVICIP1_PRI1_bf(const void *const hw, hri_nvic_nvicip1_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP1.reg;
	tmp = (tmp & NVIC_NVICIP1_PRI1(mask)) >> 0;
	return tmp;
}

static inline void hri_nvic_write_NVICIP1_PRI1_bf(const void *const hw, hri_nvic_nvicip1_reg_t data)
{
	uint8_t tmp;
	NVIC_CRITICAL_SECTION_ENTER();
	tmp = ((Nvic *)hw)->NVICIP1.reg;
	tmp &= ~NVIC_NVICIP1_PRI1_Msk;
	tmp |= NVIC_NVICIP1_PRI1(data);
	((Nvic *)hw)->NVICIP1.reg = tmp;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP1_PRI1_bf(const void *const hw, hri_nvic_nvicip1_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP1.reg &= ~NVIC_NVICIP1_PRI1(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP1_PRI1_bf(const void *const hw, hri_nvic_nvicip1_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP1.reg ^= NVIC_NVICIP1_PRI1(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip1_reg_t hri_nvic_read_NVICIP1_PRI1_bf(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP1.reg;
	tmp = (tmp & NVIC_NVICIP1_PRI1_Msk) >> 0;
	return tmp;
}

static inline void hri_nvic_set_NVICIP1_reg(const void *const hw, hri_nvic_nvicip1_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP1.reg |= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip1_reg_t hri_nvic_get_NVICIP1_reg(const void *const hw, hri_nvic_nvicip1_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP1.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_nvic_write_NVICIP1_reg(const void *const hw, hri_nvic_nvicip1_reg_t data)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP1.reg = data;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP1_reg(const void *const hw, hri_nvic_nvicip1_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP1.reg &= ~mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP1_reg(const void *const hw, hri_nvic_nvicip1_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP1.reg ^= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip1_reg_t hri_nvic_read_NVICIP1_reg(const void *const hw)
{
	return ((Nvic *)hw)->NVICIP1.reg;
}

static inline void hri_nvic_set_NVICIP2_PRI2_bf(const void *const hw, hri_nvic_nvicip2_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP2.reg |= NVIC_NVICIP2_PRI2(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip2_reg_t hri_nvic_get_NVICIP2_PRI2_bf(const void *const hw, hri_nvic_nvicip2_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP2.reg;
	tmp = (tmp & NVIC_NVICIP2_PRI2(mask)) >> 0;
	return tmp;
}

static inline void hri_nvic_write_NVICIP2_PRI2_bf(const void *const hw, hri_nvic_nvicip2_reg_t data)
{
	uint8_t tmp;
	NVIC_CRITICAL_SECTION_ENTER();
	tmp = ((Nvic *)hw)->NVICIP2.reg;
	tmp &= ~NVIC_NVICIP2_PRI2_Msk;
	tmp |= NVIC_NVICIP2_PRI2(data);
	((Nvic *)hw)->NVICIP2.reg = tmp;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP2_PRI2_bf(const void *const hw, hri_nvic_nvicip2_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP2.reg &= ~NVIC_NVICIP2_PRI2(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP2_PRI2_bf(const void *const hw, hri_nvic_nvicip2_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP2.reg ^= NVIC_NVICIP2_PRI2(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip2_reg_t hri_nvic_read_NVICIP2_PRI2_bf(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP2.reg;
	tmp = (tmp & NVIC_NVICIP2_PRI2_Msk) >> 0;
	return tmp;
}

static inline void hri_nvic_set_NVICIP2_reg(const void *const hw, hri_nvic_nvicip2_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP2.reg |= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip2_reg_t hri_nvic_get_NVICIP2_reg(const void *const hw, hri_nvic_nvicip2_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP2.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_nvic_write_NVICIP2_reg(const void *const hw, hri_nvic_nvicip2_reg_t data)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP2.reg = data;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP2_reg(const void *const hw, hri_nvic_nvicip2_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP2.reg &= ~mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP2_reg(const void *const hw, hri_nvic_nvicip2_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP2.reg ^= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip2_reg_t hri_nvic_read_NVICIP2_reg(const void *const hw)
{
	return ((Nvic *)hw)->NVICIP2.reg;
}

static inline void hri_nvic_set_NVICIP3_PRI3_bf(const void *const hw, hri_nvic_nvicip3_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP3.reg |= NVIC_NVICIP3_PRI3(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip3_reg_t hri_nvic_get_NVICIP3_PRI3_bf(const void *const hw, hri_nvic_nvicip3_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP3.reg;
	tmp = (tmp & NVIC_NVICIP3_PRI3(mask)) >> 0;
	return tmp;
}

static inline void hri_nvic_write_NVICIP3_PRI3_bf(const void *const hw, hri_nvic_nvicip3_reg_t data)
{
	uint8_t tmp;
	NVIC_CRITICAL_SECTION_ENTER();
	tmp = ((Nvic *)hw)->NVICIP3.reg;
	tmp &= ~NVIC_NVICIP3_PRI3_Msk;
	tmp |= NVIC_NVICIP3_PRI3(data);
	((Nvic *)hw)->NVICIP3.reg = tmp;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP3_PRI3_bf(const void *const hw, hri_nvic_nvicip3_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP3.reg &= ~NVIC_NVICIP3_PRI3(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP3_PRI3_bf(const void *const hw, hri_nvic_nvicip3_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP3.reg ^= NVIC_NVICIP3_PRI3(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip3_reg_t hri_nvic_read_NVICIP3_PRI3_bf(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP3.reg;
	tmp = (tmp & NVIC_NVICIP3_PRI3_Msk) >> 0;
	return tmp;
}

static inline void hri_nvic_set_NVICIP3_reg(const void *const hw, hri_nvic_nvicip3_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP3.reg |= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip3_reg_t hri_nvic_get_NVICIP3_reg(const void *const hw, hri_nvic_nvicip3_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP3.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_nvic_write_NVICIP3_reg(const void *const hw, hri_nvic_nvicip3_reg_t data)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP3.reg = data;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP3_reg(const void *const hw, hri_nvic_nvicip3_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP3.reg &= ~mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP3_reg(const void *const hw, hri_nvic_nvicip3_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP3.reg ^= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip3_reg_t hri_nvic_read_NVICIP3_reg(const void *const hw)
{
	return ((Nvic *)hw)->NVICIP3.reg;
}

static inline void hri_nvic_set_NVICIP4_PRI4_bf(const void *const hw, hri_nvic_nvicip4_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP4.reg |= NVIC_NVICIP4_PRI4(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip4_reg_t hri_nvic_get_NVICIP4_PRI4_bf(const void *const hw, hri_nvic_nvicip4_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP4.reg;
	tmp = (tmp & NVIC_NVICIP4_PRI4(mask)) >> 0;
	return tmp;
}

static inline void hri_nvic_write_NVICIP4_PRI4_bf(const void *const hw, hri_nvic_nvicip4_reg_t data)
{
	uint8_t tmp;
	NVIC_CRITICAL_SECTION_ENTER();
	tmp = ((Nvic *)hw)->NVICIP4.reg;
	tmp &= ~NVIC_NVICIP4_PRI4_Msk;
	tmp |= NVIC_NVICIP4_PRI4(data);
	((Nvic *)hw)->NVICIP4.reg = tmp;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP4_PRI4_bf(const void *const hw, hri_nvic_nvicip4_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP4.reg &= ~NVIC_NVICIP4_PRI4(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP4_PRI4_bf(const void *const hw, hri_nvic_nvicip4_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP4.reg ^= NVIC_NVICIP4_PRI4(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip4_reg_t hri_nvic_read_NVICIP4_PRI4_bf(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP4.reg;
	tmp = (tmp & NVIC_NVICIP4_PRI4_Msk) >> 0;
	return tmp;
}

static inline void hri_nvic_set_NVICIP4_reg(const void *const hw, hri_nvic_nvicip4_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP4.reg |= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip4_reg_t hri_nvic_get_NVICIP4_reg(const void *const hw, hri_nvic_nvicip4_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP4.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_nvic_write_NVICIP4_reg(const void *const hw, hri_nvic_nvicip4_reg_t data)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP4.reg = data;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP4_reg(const void *const hw, hri_nvic_nvicip4_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP4.reg &= ~mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP4_reg(const void *const hw, hri_nvic_nvicip4_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP4.reg ^= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip4_reg_t hri_nvic_read_NVICIP4_reg(const void *const hw)
{
	return ((Nvic *)hw)->NVICIP4.reg;
}

static inline void hri_nvic_set_NVICIP5_PRI5_bf(const void *const hw, hri_nvic_nvicip5_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP5.reg |= NVIC_NVICIP5_PRI5(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip5_reg_t hri_nvic_get_NVICIP5_PRI5_bf(const void *const hw, hri_nvic_nvicip5_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP5.reg;
	tmp = (tmp & NVIC_NVICIP5_PRI5(mask)) >> 0;
	return tmp;
}

static inline void hri_nvic_write_NVICIP5_PRI5_bf(const void *const hw, hri_nvic_nvicip5_reg_t data)
{
	uint8_t tmp;
	NVIC_CRITICAL_SECTION_ENTER();
	tmp = ((Nvic *)hw)->NVICIP5.reg;
	tmp &= ~NVIC_NVICIP5_PRI5_Msk;
	tmp |= NVIC_NVICIP5_PRI5(data);
	((Nvic *)hw)->NVICIP5.reg = tmp;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP5_PRI5_bf(const void *const hw, hri_nvic_nvicip5_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP5.reg &= ~NVIC_NVICIP5_PRI5(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP5_PRI5_bf(const void *const hw, hri_nvic_nvicip5_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP5.reg ^= NVIC_NVICIP5_PRI5(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip5_reg_t hri_nvic_read_NVICIP5_PRI5_bf(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP5.reg;
	tmp = (tmp & NVIC_NVICIP5_PRI5_Msk) >> 0;
	return tmp;
}

static inline void hri_nvic_set_NVICIP5_reg(const void *const hw, hri_nvic_nvicip5_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP5.reg |= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip5_reg_t hri_nvic_get_NVICIP5_reg(const void *const hw, hri_nvic_nvicip5_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP5.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_nvic_write_NVICIP5_reg(const void *const hw, hri_nvic_nvicip5_reg_t data)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP5.reg = data;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP5_reg(const void *const hw, hri_nvic_nvicip5_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP5.reg &= ~mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP5_reg(const void *const hw, hri_nvic_nvicip5_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP5.reg ^= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip5_reg_t hri_nvic_read_NVICIP5_reg(const void *const hw)
{
	return ((Nvic *)hw)->NVICIP5.reg;
}

static inline void hri_nvic_set_NVICIP6_PRI6_bf(const void *const hw, hri_nvic_nvicip6_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP6.reg |= NVIC_NVICIP6_PRI6(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip6_reg_t hri_nvic_get_NVICIP6_PRI6_bf(const void *const hw, hri_nvic_nvicip6_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP6.reg;
	tmp = (tmp & NVIC_NVICIP6_PRI6(mask)) >> 0;
	return tmp;
}

static inline void hri_nvic_write_NVICIP6_PRI6_bf(const void *const hw, hri_nvic_nvicip6_reg_t data)
{
	uint8_t tmp;
	NVIC_CRITICAL_SECTION_ENTER();
	tmp = ((Nvic *)hw)->NVICIP6.reg;
	tmp &= ~NVIC_NVICIP6_PRI6_Msk;
	tmp |= NVIC_NVICIP6_PRI6(data);
	((Nvic *)hw)->NVICIP6.reg = tmp;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP6_PRI6_bf(const void *const hw, hri_nvic_nvicip6_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP6.reg &= ~NVIC_NVICIP6_PRI6(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP6_PRI6_bf(const void *const hw, hri_nvic_nvicip6_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP6.reg ^= NVIC_NVICIP6_PRI6(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip6_reg_t hri_nvic_read_NVICIP6_PRI6_bf(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP6.reg;
	tmp = (tmp & NVIC_NVICIP6_PRI6_Msk) >> 0;
	return tmp;
}

static inline void hri_nvic_set_NVICIP6_reg(const void *const hw, hri_nvic_nvicip6_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP6.reg |= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip6_reg_t hri_nvic_get_NVICIP6_reg(const void *const hw, hri_nvic_nvicip6_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP6.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_nvic_write_NVICIP6_reg(const void *const hw, hri_nvic_nvicip6_reg_t data)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP6.reg = data;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP6_reg(const void *const hw, hri_nvic_nvicip6_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP6.reg &= ~mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP6_reg(const void *const hw, hri_nvic_nvicip6_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP6.reg ^= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip6_reg_t hri_nvic_read_NVICIP6_reg(const void *const hw)
{
	return ((Nvic *)hw)->NVICIP6.reg;
}

static inline void hri_nvic_set_NVICIP7_PRI7_bf(const void *const hw, hri_nvic_nvicip7_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP7.reg |= NVIC_NVICIP7_PRI7(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip7_reg_t hri_nvic_get_NVICIP7_PRI7_bf(const void *const hw, hri_nvic_nvicip7_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP7.reg;
	tmp = (tmp & NVIC_NVICIP7_PRI7(mask)) >> 0;
	return tmp;
}

static inline void hri_nvic_write_NVICIP7_PRI7_bf(const void *const hw, hri_nvic_nvicip7_reg_t data)
{
	uint8_t tmp;
	NVIC_CRITICAL_SECTION_ENTER();
	tmp = ((Nvic *)hw)->NVICIP7.reg;
	tmp &= ~NVIC_NVICIP7_PRI7_Msk;
	tmp |= NVIC_NVICIP7_PRI7(data);
	((Nvic *)hw)->NVICIP7.reg = tmp;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP7_PRI7_bf(const void *const hw, hri_nvic_nvicip7_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP7.reg &= ~NVIC_NVICIP7_PRI7(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP7_PRI7_bf(const void *const hw, hri_nvic_nvicip7_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP7.reg ^= NVIC_NVICIP7_PRI7(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip7_reg_t hri_nvic_read_NVICIP7_PRI7_bf(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP7.reg;
	tmp = (tmp & NVIC_NVICIP7_PRI7_Msk) >> 0;
	return tmp;
}

static inline void hri_nvic_set_NVICIP7_reg(const void *const hw, hri_nvic_nvicip7_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP7.reg |= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip7_reg_t hri_nvic_get_NVICIP7_reg(const void *const hw, hri_nvic_nvicip7_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP7.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_nvic_write_NVICIP7_reg(const void *const hw, hri_nvic_nvicip7_reg_t data)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP7.reg = data;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP7_reg(const void *const hw, hri_nvic_nvicip7_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP7.reg &= ~mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP7_reg(const void *const hw, hri_nvic_nvicip7_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP7.reg ^= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip7_reg_t hri_nvic_read_NVICIP7_reg(const void *const hw)
{
	return ((Nvic *)hw)->NVICIP7.reg;
}

static inline void hri_nvic_set_NVICIP8_PRI8_bf(const void *const hw, hri_nvic_nvicip8_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP8.reg |= NVIC_NVICIP8_PRI8(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip8_reg_t hri_nvic_get_NVICIP8_PRI8_bf(const void *const hw, hri_nvic_nvicip8_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP8.reg;
	tmp = (tmp & NVIC_NVICIP8_PRI8(mask)) >> 0;
	return tmp;
}

static inline void hri_nvic_write_NVICIP8_PRI8_bf(const void *const hw, hri_nvic_nvicip8_reg_t data)
{
	uint8_t tmp;
	NVIC_CRITICAL_SECTION_ENTER();
	tmp = ((Nvic *)hw)->NVICIP8.reg;
	tmp &= ~NVIC_NVICIP8_PRI8_Msk;
	tmp |= NVIC_NVICIP8_PRI8(data);
	((Nvic *)hw)->NVICIP8.reg = tmp;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP8_PRI8_bf(const void *const hw, hri_nvic_nvicip8_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP8.reg &= ~NVIC_NVICIP8_PRI8(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP8_PRI8_bf(const void *const hw, hri_nvic_nvicip8_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP8.reg ^= NVIC_NVICIP8_PRI8(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip8_reg_t hri_nvic_read_NVICIP8_PRI8_bf(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP8.reg;
	tmp = (tmp & NVIC_NVICIP8_PRI8_Msk) >> 0;
	return tmp;
}

static inline void hri_nvic_set_NVICIP8_reg(const void *const hw, hri_nvic_nvicip8_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP8.reg |= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip8_reg_t hri_nvic_get_NVICIP8_reg(const void *const hw, hri_nvic_nvicip8_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP8.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_nvic_write_NVICIP8_reg(const void *const hw, hri_nvic_nvicip8_reg_t data)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP8.reg = data;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP8_reg(const void *const hw, hri_nvic_nvicip8_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP8.reg &= ~mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP8_reg(const void *const hw, hri_nvic_nvicip8_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP8.reg ^= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip8_reg_t hri_nvic_read_NVICIP8_reg(const void *const hw)
{
	return ((Nvic *)hw)->NVICIP8.reg;
}

static inline void hri_nvic_set_NVICIP9_PRI9_bf(const void *const hw, hri_nvic_nvicip9_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP9.reg |= NVIC_NVICIP9_PRI9(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip9_reg_t hri_nvic_get_NVICIP9_PRI9_bf(const void *const hw, hri_nvic_nvicip9_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP9.reg;
	tmp = (tmp & NVIC_NVICIP9_PRI9(mask)) >> 0;
	return tmp;
}

static inline void hri_nvic_write_NVICIP9_PRI9_bf(const void *const hw, hri_nvic_nvicip9_reg_t data)
{
	uint8_t tmp;
	NVIC_CRITICAL_SECTION_ENTER();
	tmp = ((Nvic *)hw)->NVICIP9.reg;
	tmp &= ~NVIC_NVICIP9_PRI9_Msk;
	tmp |= NVIC_NVICIP9_PRI9(data);
	((Nvic *)hw)->NVICIP9.reg = tmp;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP9_PRI9_bf(const void *const hw, hri_nvic_nvicip9_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP9.reg &= ~NVIC_NVICIP9_PRI9(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP9_PRI9_bf(const void *const hw, hri_nvic_nvicip9_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP9.reg ^= NVIC_NVICIP9_PRI9(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip9_reg_t hri_nvic_read_NVICIP9_PRI9_bf(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP9.reg;
	tmp = (tmp & NVIC_NVICIP9_PRI9_Msk) >> 0;
	return tmp;
}

static inline void hri_nvic_set_NVICIP9_reg(const void *const hw, hri_nvic_nvicip9_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP9.reg |= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip9_reg_t hri_nvic_get_NVICIP9_reg(const void *const hw, hri_nvic_nvicip9_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP9.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_nvic_write_NVICIP9_reg(const void *const hw, hri_nvic_nvicip9_reg_t data)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP9.reg = data;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP9_reg(const void *const hw, hri_nvic_nvicip9_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP9.reg &= ~mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP9_reg(const void *const hw, hri_nvic_nvicip9_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP9.reg ^= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip9_reg_t hri_nvic_read_NVICIP9_reg(const void *const hw)
{
	return ((Nvic *)hw)->NVICIP9.reg;
}

static inline void hri_nvic_set_NVICIP10_PRI10_bf(const void *const hw, hri_nvic_nvicip10_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP10.reg |= NVIC_NVICIP10_PRI10(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip10_reg_t hri_nvic_get_NVICIP10_PRI10_bf(const void *const hw, hri_nvic_nvicip10_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP10.reg;
	tmp = (tmp & NVIC_NVICIP10_PRI10(mask)) >> 0;
	return tmp;
}

static inline void hri_nvic_write_NVICIP10_PRI10_bf(const void *const hw, hri_nvic_nvicip10_reg_t data)
{
	uint8_t tmp;
	NVIC_CRITICAL_SECTION_ENTER();
	tmp = ((Nvic *)hw)->NVICIP10.reg;
	tmp &= ~NVIC_NVICIP10_PRI10_Msk;
	tmp |= NVIC_NVICIP10_PRI10(data);
	((Nvic *)hw)->NVICIP10.reg = tmp;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP10_PRI10_bf(const void *const hw, hri_nvic_nvicip10_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP10.reg &= ~NVIC_NVICIP10_PRI10(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP10_PRI10_bf(const void *const hw, hri_nvic_nvicip10_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP10.reg ^= NVIC_NVICIP10_PRI10(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip10_reg_t hri_nvic_read_NVICIP10_PRI10_bf(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP10.reg;
	tmp = (tmp & NVIC_NVICIP10_PRI10_Msk) >> 0;
	return tmp;
}

static inline void hri_nvic_set_NVICIP10_reg(const void *const hw, hri_nvic_nvicip10_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP10.reg |= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip10_reg_t hri_nvic_get_NVICIP10_reg(const void *const hw, hri_nvic_nvicip10_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP10.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_nvic_write_NVICIP10_reg(const void *const hw, hri_nvic_nvicip10_reg_t data)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP10.reg = data;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP10_reg(const void *const hw, hri_nvic_nvicip10_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP10.reg &= ~mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP10_reg(const void *const hw, hri_nvic_nvicip10_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP10.reg ^= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip10_reg_t hri_nvic_read_NVICIP10_reg(const void *const hw)
{
	return ((Nvic *)hw)->NVICIP10.reg;
}

static inline void hri_nvic_set_NVICIP11_PRI11_bf(const void *const hw, hri_nvic_nvicip11_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP11.reg |= NVIC_NVICIP11_PRI11(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip11_reg_t hri_nvic_get_NVICIP11_PRI11_bf(const void *const hw, hri_nvic_nvicip11_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP11.reg;
	tmp = (tmp & NVIC_NVICIP11_PRI11(mask)) >> 0;
	return tmp;
}

static inline void hri_nvic_write_NVICIP11_PRI11_bf(const void *const hw, hri_nvic_nvicip11_reg_t data)
{
	uint8_t tmp;
	NVIC_CRITICAL_SECTION_ENTER();
	tmp = ((Nvic *)hw)->NVICIP11.reg;
	tmp &= ~NVIC_NVICIP11_PRI11_Msk;
	tmp |= NVIC_NVICIP11_PRI11(data);
	((Nvic *)hw)->NVICIP11.reg = tmp;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP11_PRI11_bf(const void *const hw, hri_nvic_nvicip11_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP11.reg &= ~NVIC_NVICIP11_PRI11(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP11_PRI11_bf(const void *const hw, hri_nvic_nvicip11_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP11.reg ^= NVIC_NVICIP11_PRI11(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip11_reg_t hri_nvic_read_NVICIP11_PRI11_bf(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP11.reg;
	tmp = (tmp & NVIC_NVICIP11_PRI11_Msk) >> 0;
	return tmp;
}

static inline void hri_nvic_set_NVICIP11_reg(const void *const hw, hri_nvic_nvicip11_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP11.reg |= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip11_reg_t hri_nvic_get_NVICIP11_reg(const void *const hw, hri_nvic_nvicip11_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP11.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_nvic_write_NVICIP11_reg(const void *const hw, hri_nvic_nvicip11_reg_t data)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP11.reg = data;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP11_reg(const void *const hw, hri_nvic_nvicip11_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP11.reg &= ~mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP11_reg(const void *const hw, hri_nvic_nvicip11_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP11.reg ^= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip11_reg_t hri_nvic_read_NVICIP11_reg(const void *const hw)
{
	return ((Nvic *)hw)->NVICIP11.reg;
}

static inline void hri_nvic_set_NVICIP12_PRI12_bf(const void *const hw, hri_nvic_nvicip12_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP12.reg |= NVIC_NVICIP12_PRI12(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip12_reg_t hri_nvic_get_NVICIP12_PRI12_bf(const void *const hw, hri_nvic_nvicip12_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP12.reg;
	tmp = (tmp & NVIC_NVICIP12_PRI12(mask)) >> 0;
	return tmp;
}

static inline void hri_nvic_write_NVICIP12_PRI12_bf(const void *const hw, hri_nvic_nvicip12_reg_t data)
{
	uint8_t tmp;
	NVIC_CRITICAL_SECTION_ENTER();
	tmp = ((Nvic *)hw)->NVICIP12.reg;
	tmp &= ~NVIC_NVICIP12_PRI12_Msk;
	tmp |= NVIC_NVICIP12_PRI12(data);
	((Nvic *)hw)->NVICIP12.reg = tmp;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP12_PRI12_bf(const void *const hw, hri_nvic_nvicip12_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP12.reg &= ~NVIC_NVICIP12_PRI12(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP12_PRI12_bf(const void *const hw, hri_nvic_nvicip12_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP12.reg ^= NVIC_NVICIP12_PRI12(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip12_reg_t hri_nvic_read_NVICIP12_PRI12_bf(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP12.reg;
	tmp = (tmp & NVIC_NVICIP12_PRI12_Msk) >> 0;
	return tmp;
}

static inline void hri_nvic_set_NVICIP12_reg(const void *const hw, hri_nvic_nvicip12_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP12.reg |= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip12_reg_t hri_nvic_get_NVICIP12_reg(const void *const hw, hri_nvic_nvicip12_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP12.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_nvic_write_NVICIP12_reg(const void *const hw, hri_nvic_nvicip12_reg_t data)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP12.reg = data;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP12_reg(const void *const hw, hri_nvic_nvicip12_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP12.reg &= ~mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP12_reg(const void *const hw, hri_nvic_nvicip12_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP12.reg ^= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip12_reg_t hri_nvic_read_NVICIP12_reg(const void *const hw)
{
	return ((Nvic *)hw)->NVICIP12.reg;
}

static inline void hri_nvic_set_NVICIP13_PRI13_bf(const void *const hw, hri_nvic_nvicip13_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP13.reg |= NVIC_NVICIP13_PRI13(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip13_reg_t hri_nvic_get_NVICIP13_PRI13_bf(const void *const hw, hri_nvic_nvicip13_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP13.reg;
	tmp = (tmp & NVIC_NVICIP13_PRI13(mask)) >> 0;
	return tmp;
}

static inline void hri_nvic_write_NVICIP13_PRI13_bf(const void *const hw, hri_nvic_nvicip13_reg_t data)
{
	uint8_t tmp;
	NVIC_CRITICAL_SECTION_ENTER();
	tmp = ((Nvic *)hw)->NVICIP13.reg;
	tmp &= ~NVIC_NVICIP13_PRI13_Msk;
	tmp |= NVIC_NVICIP13_PRI13(data);
	((Nvic *)hw)->NVICIP13.reg = tmp;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP13_PRI13_bf(const void *const hw, hri_nvic_nvicip13_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP13.reg &= ~NVIC_NVICIP13_PRI13(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP13_PRI13_bf(const void *const hw, hri_nvic_nvicip13_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP13.reg ^= NVIC_NVICIP13_PRI13(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip13_reg_t hri_nvic_read_NVICIP13_PRI13_bf(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP13.reg;
	tmp = (tmp & NVIC_NVICIP13_PRI13_Msk) >> 0;
	return tmp;
}

static inline void hri_nvic_set_NVICIP13_reg(const void *const hw, hri_nvic_nvicip13_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP13.reg |= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip13_reg_t hri_nvic_get_NVICIP13_reg(const void *const hw, hri_nvic_nvicip13_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP13.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_nvic_write_NVICIP13_reg(const void *const hw, hri_nvic_nvicip13_reg_t data)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP13.reg = data;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP13_reg(const void *const hw, hri_nvic_nvicip13_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP13.reg &= ~mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP13_reg(const void *const hw, hri_nvic_nvicip13_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP13.reg ^= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip13_reg_t hri_nvic_read_NVICIP13_reg(const void *const hw)
{
	return ((Nvic *)hw)->NVICIP13.reg;
}

static inline void hri_nvic_set_NVICIP14_PRI14_bf(const void *const hw, hri_nvic_nvicip14_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP14.reg |= NVIC_NVICIP14_PRI14(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip14_reg_t hri_nvic_get_NVICIP14_PRI14_bf(const void *const hw, hri_nvic_nvicip14_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP14.reg;
	tmp = (tmp & NVIC_NVICIP14_PRI14(mask)) >> 0;
	return tmp;
}

static inline void hri_nvic_write_NVICIP14_PRI14_bf(const void *const hw, hri_nvic_nvicip14_reg_t data)
{
	uint8_t tmp;
	NVIC_CRITICAL_SECTION_ENTER();
	tmp = ((Nvic *)hw)->NVICIP14.reg;
	tmp &= ~NVIC_NVICIP14_PRI14_Msk;
	tmp |= NVIC_NVICIP14_PRI14(data);
	((Nvic *)hw)->NVICIP14.reg = tmp;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP14_PRI14_bf(const void *const hw, hri_nvic_nvicip14_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP14.reg &= ~NVIC_NVICIP14_PRI14(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP14_PRI14_bf(const void *const hw, hri_nvic_nvicip14_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP14.reg ^= NVIC_NVICIP14_PRI14(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip14_reg_t hri_nvic_read_NVICIP14_PRI14_bf(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP14.reg;
	tmp = (tmp & NVIC_NVICIP14_PRI14_Msk) >> 0;
	return tmp;
}

static inline void hri_nvic_set_NVICIP14_reg(const void *const hw, hri_nvic_nvicip14_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP14.reg |= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip14_reg_t hri_nvic_get_NVICIP14_reg(const void *const hw, hri_nvic_nvicip14_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP14.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_nvic_write_NVICIP14_reg(const void *const hw, hri_nvic_nvicip14_reg_t data)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP14.reg = data;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP14_reg(const void *const hw, hri_nvic_nvicip14_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP14.reg &= ~mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP14_reg(const void *const hw, hri_nvic_nvicip14_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP14.reg ^= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip14_reg_t hri_nvic_read_NVICIP14_reg(const void *const hw)
{
	return ((Nvic *)hw)->NVICIP14.reg;
}

static inline void hri_nvic_set_NVICIP15_PRI15_bf(const void *const hw, hri_nvic_nvicip15_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP15.reg |= NVIC_NVICIP15_PRI15(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip15_reg_t hri_nvic_get_NVICIP15_PRI15_bf(const void *const hw, hri_nvic_nvicip15_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP15.reg;
	tmp = (tmp & NVIC_NVICIP15_PRI15(mask)) >> 0;
	return tmp;
}

static inline void hri_nvic_write_NVICIP15_PRI15_bf(const void *const hw, hri_nvic_nvicip15_reg_t data)
{
	uint8_t tmp;
	NVIC_CRITICAL_SECTION_ENTER();
	tmp = ((Nvic *)hw)->NVICIP15.reg;
	tmp &= ~NVIC_NVICIP15_PRI15_Msk;
	tmp |= NVIC_NVICIP15_PRI15(data);
	((Nvic *)hw)->NVICIP15.reg = tmp;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP15_PRI15_bf(const void *const hw, hri_nvic_nvicip15_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP15.reg &= ~NVIC_NVICIP15_PRI15(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP15_PRI15_bf(const void *const hw, hri_nvic_nvicip15_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP15.reg ^= NVIC_NVICIP15_PRI15(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip15_reg_t hri_nvic_read_NVICIP15_PRI15_bf(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP15.reg;
	tmp = (tmp & NVIC_NVICIP15_PRI15_Msk) >> 0;
	return tmp;
}

static inline void hri_nvic_set_NVICIP15_reg(const void *const hw, hri_nvic_nvicip15_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP15.reg |= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip15_reg_t hri_nvic_get_NVICIP15_reg(const void *const hw, hri_nvic_nvicip15_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP15.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_nvic_write_NVICIP15_reg(const void *const hw, hri_nvic_nvicip15_reg_t data)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP15.reg = data;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP15_reg(const void *const hw, hri_nvic_nvicip15_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP15.reg &= ~mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP15_reg(const void *const hw, hri_nvic_nvicip15_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP15.reg ^= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip15_reg_t hri_nvic_read_NVICIP15_reg(const void *const hw)
{
	return ((Nvic *)hw)->NVICIP15.reg;
}

static inline void hri_nvic_set_NVICIP16_PRI16_bf(const void *const hw, hri_nvic_nvicip16_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP16.reg |= NVIC_NVICIP16_PRI16(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip16_reg_t hri_nvic_get_NVICIP16_PRI16_bf(const void *const hw, hri_nvic_nvicip16_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP16.reg;
	tmp = (tmp & NVIC_NVICIP16_PRI16(mask)) >> 0;
	return tmp;
}

static inline void hri_nvic_write_NVICIP16_PRI16_bf(const void *const hw, hri_nvic_nvicip16_reg_t data)
{
	uint8_t tmp;
	NVIC_CRITICAL_SECTION_ENTER();
	tmp = ((Nvic *)hw)->NVICIP16.reg;
	tmp &= ~NVIC_NVICIP16_PRI16_Msk;
	tmp |= NVIC_NVICIP16_PRI16(data);
	((Nvic *)hw)->NVICIP16.reg = tmp;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP16_PRI16_bf(const void *const hw, hri_nvic_nvicip16_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP16.reg &= ~NVIC_NVICIP16_PRI16(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP16_PRI16_bf(const void *const hw, hri_nvic_nvicip16_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP16.reg ^= NVIC_NVICIP16_PRI16(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip16_reg_t hri_nvic_read_NVICIP16_PRI16_bf(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP16.reg;
	tmp = (tmp & NVIC_NVICIP16_PRI16_Msk) >> 0;
	return tmp;
}

static inline void hri_nvic_set_NVICIP16_reg(const void *const hw, hri_nvic_nvicip16_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP16.reg |= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip16_reg_t hri_nvic_get_NVICIP16_reg(const void *const hw, hri_nvic_nvicip16_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP16.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_nvic_write_NVICIP16_reg(const void *const hw, hri_nvic_nvicip16_reg_t data)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP16.reg = data;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP16_reg(const void *const hw, hri_nvic_nvicip16_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP16.reg &= ~mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP16_reg(const void *const hw, hri_nvic_nvicip16_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP16.reg ^= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip16_reg_t hri_nvic_read_NVICIP16_reg(const void *const hw)
{
	return ((Nvic *)hw)->NVICIP16.reg;
}

static inline void hri_nvic_set_NVICIP17_PRI17_bf(const void *const hw, hri_nvic_nvicip17_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP17.reg |= NVIC_NVICIP17_PRI17(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip17_reg_t hri_nvic_get_NVICIP17_PRI17_bf(const void *const hw, hri_nvic_nvicip17_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP17.reg;
	tmp = (tmp & NVIC_NVICIP17_PRI17(mask)) >> 0;
	return tmp;
}

static inline void hri_nvic_write_NVICIP17_PRI17_bf(const void *const hw, hri_nvic_nvicip17_reg_t data)
{
	uint8_t tmp;
	NVIC_CRITICAL_SECTION_ENTER();
	tmp = ((Nvic *)hw)->NVICIP17.reg;
	tmp &= ~NVIC_NVICIP17_PRI17_Msk;
	tmp |= NVIC_NVICIP17_PRI17(data);
	((Nvic *)hw)->NVICIP17.reg = tmp;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP17_PRI17_bf(const void *const hw, hri_nvic_nvicip17_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP17.reg &= ~NVIC_NVICIP17_PRI17(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP17_PRI17_bf(const void *const hw, hri_nvic_nvicip17_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP17.reg ^= NVIC_NVICIP17_PRI17(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip17_reg_t hri_nvic_read_NVICIP17_PRI17_bf(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP17.reg;
	tmp = (tmp & NVIC_NVICIP17_PRI17_Msk) >> 0;
	return tmp;
}

static inline void hri_nvic_set_NVICIP17_reg(const void *const hw, hri_nvic_nvicip17_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP17.reg |= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip17_reg_t hri_nvic_get_NVICIP17_reg(const void *const hw, hri_nvic_nvicip17_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP17.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_nvic_write_NVICIP17_reg(const void *const hw, hri_nvic_nvicip17_reg_t data)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP17.reg = data;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP17_reg(const void *const hw, hri_nvic_nvicip17_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP17.reg &= ~mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP17_reg(const void *const hw, hri_nvic_nvicip17_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP17.reg ^= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip17_reg_t hri_nvic_read_NVICIP17_reg(const void *const hw)
{
	return ((Nvic *)hw)->NVICIP17.reg;
}

static inline void hri_nvic_set_NVICIP18_PRI18_bf(const void *const hw, hri_nvic_nvicip18_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP18.reg |= NVIC_NVICIP18_PRI18(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip18_reg_t hri_nvic_get_NVICIP18_PRI18_bf(const void *const hw, hri_nvic_nvicip18_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP18.reg;
	tmp = (tmp & NVIC_NVICIP18_PRI18(mask)) >> 0;
	return tmp;
}

static inline void hri_nvic_write_NVICIP18_PRI18_bf(const void *const hw, hri_nvic_nvicip18_reg_t data)
{
	uint8_t tmp;
	NVIC_CRITICAL_SECTION_ENTER();
	tmp = ((Nvic *)hw)->NVICIP18.reg;
	tmp &= ~NVIC_NVICIP18_PRI18_Msk;
	tmp |= NVIC_NVICIP18_PRI18(data);
	((Nvic *)hw)->NVICIP18.reg = tmp;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP18_PRI18_bf(const void *const hw, hri_nvic_nvicip18_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP18.reg &= ~NVIC_NVICIP18_PRI18(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP18_PRI18_bf(const void *const hw, hri_nvic_nvicip18_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP18.reg ^= NVIC_NVICIP18_PRI18(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip18_reg_t hri_nvic_read_NVICIP18_PRI18_bf(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP18.reg;
	tmp = (tmp & NVIC_NVICIP18_PRI18_Msk) >> 0;
	return tmp;
}

static inline void hri_nvic_set_NVICIP18_reg(const void *const hw, hri_nvic_nvicip18_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP18.reg |= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip18_reg_t hri_nvic_get_NVICIP18_reg(const void *const hw, hri_nvic_nvicip18_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP18.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_nvic_write_NVICIP18_reg(const void *const hw, hri_nvic_nvicip18_reg_t data)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP18.reg = data;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP18_reg(const void *const hw, hri_nvic_nvicip18_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP18.reg &= ~mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP18_reg(const void *const hw, hri_nvic_nvicip18_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP18.reg ^= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip18_reg_t hri_nvic_read_NVICIP18_reg(const void *const hw)
{
	return ((Nvic *)hw)->NVICIP18.reg;
}

static inline void hri_nvic_set_NVICIP19_PRI19_bf(const void *const hw, hri_nvic_nvicip19_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP19.reg |= NVIC_NVICIP19_PRI19(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip19_reg_t hri_nvic_get_NVICIP19_PRI19_bf(const void *const hw, hri_nvic_nvicip19_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP19.reg;
	tmp = (tmp & NVIC_NVICIP19_PRI19(mask)) >> 0;
	return tmp;
}

static inline void hri_nvic_write_NVICIP19_PRI19_bf(const void *const hw, hri_nvic_nvicip19_reg_t data)
{
	uint8_t tmp;
	NVIC_CRITICAL_SECTION_ENTER();
	tmp = ((Nvic *)hw)->NVICIP19.reg;
	tmp &= ~NVIC_NVICIP19_PRI19_Msk;
	tmp |= NVIC_NVICIP19_PRI19(data);
	((Nvic *)hw)->NVICIP19.reg = tmp;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP19_PRI19_bf(const void *const hw, hri_nvic_nvicip19_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP19.reg &= ~NVIC_NVICIP19_PRI19(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP19_PRI19_bf(const void *const hw, hri_nvic_nvicip19_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP19.reg ^= NVIC_NVICIP19_PRI19(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip19_reg_t hri_nvic_read_NVICIP19_PRI19_bf(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP19.reg;
	tmp = (tmp & NVIC_NVICIP19_PRI19_Msk) >> 0;
	return tmp;
}

static inline void hri_nvic_set_NVICIP19_reg(const void *const hw, hri_nvic_nvicip19_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP19.reg |= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip19_reg_t hri_nvic_get_NVICIP19_reg(const void *const hw, hri_nvic_nvicip19_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP19.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_nvic_write_NVICIP19_reg(const void *const hw, hri_nvic_nvicip19_reg_t data)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP19.reg = data;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP19_reg(const void *const hw, hri_nvic_nvicip19_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP19.reg &= ~mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP19_reg(const void *const hw, hri_nvic_nvicip19_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP19.reg ^= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip19_reg_t hri_nvic_read_NVICIP19_reg(const void *const hw)
{
	return ((Nvic *)hw)->NVICIP19.reg;
}

static inline void hri_nvic_set_NVICIP20_PRI20_bf(const void *const hw, hri_nvic_nvicip20_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP20.reg |= NVIC_NVICIP20_PRI20(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip20_reg_t hri_nvic_get_NVICIP20_PRI20_bf(const void *const hw, hri_nvic_nvicip20_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP20.reg;
	tmp = (tmp & NVIC_NVICIP20_PRI20(mask)) >> 0;
	return tmp;
}

static inline void hri_nvic_write_NVICIP20_PRI20_bf(const void *const hw, hri_nvic_nvicip20_reg_t data)
{
	uint8_t tmp;
	NVIC_CRITICAL_SECTION_ENTER();
	tmp = ((Nvic *)hw)->NVICIP20.reg;
	tmp &= ~NVIC_NVICIP20_PRI20_Msk;
	tmp |= NVIC_NVICIP20_PRI20(data);
	((Nvic *)hw)->NVICIP20.reg = tmp;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP20_PRI20_bf(const void *const hw, hri_nvic_nvicip20_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP20.reg &= ~NVIC_NVICIP20_PRI20(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP20_PRI20_bf(const void *const hw, hri_nvic_nvicip20_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP20.reg ^= NVIC_NVICIP20_PRI20(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip20_reg_t hri_nvic_read_NVICIP20_PRI20_bf(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP20.reg;
	tmp = (tmp & NVIC_NVICIP20_PRI20_Msk) >> 0;
	return tmp;
}

static inline void hri_nvic_set_NVICIP20_reg(const void *const hw, hri_nvic_nvicip20_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP20.reg |= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip20_reg_t hri_nvic_get_NVICIP20_reg(const void *const hw, hri_nvic_nvicip20_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP20.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_nvic_write_NVICIP20_reg(const void *const hw, hri_nvic_nvicip20_reg_t data)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP20.reg = data;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP20_reg(const void *const hw, hri_nvic_nvicip20_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP20.reg &= ~mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP20_reg(const void *const hw, hri_nvic_nvicip20_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP20.reg ^= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip20_reg_t hri_nvic_read_NVICIP20_reg(const void *const hw)
{
	return ((Nvic *)hw)->NVICIP20.reg;
}

static inline void hri_nvic_set_NVICIP21_PRI21_bf(const void *const hw, hri_nvic_nvicip21_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP21.reg |= NVIC_NVICIP21_PRI21(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip21_reg_t hri_nvic_get_NVICIP21_PRI21_bf(const void *const hw, hri_nvic_nvicip21_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP21.reg;
	tmp = (tmp & NVIC_NVICIP21_PRI21(mask)) >> 0;
	return tmp;
}

static inline void hri_nvic_write_NVICIP21_PRI21_bf(const void *const hw, hri_nvic_nvicip21_reg_t data)
{
	uint8_t tmp;
	NVIC_CRITICAL_SECTION_ENTER();
	tmp = ((Nvic *)hw)->NVICIP21.reg;
	tmp &= ~NVIC_NVICIP21_PRI21_Msk;
	tmp |= NVIC_NVICIP21_PRI21(data);
	((Nvic *)hw)->NVICIP21.reg = tmp;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP21_PRI21_bf(const void *const hw, hri_nvic_nvicip21_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP21.reg &= ~NVIC_NVICIP21_PRI21(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP21_PRI21_bf(const void *const hw, hri_nvic_nvicip21_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP21.reg ^= NVIC_NVICIP21_PRI21(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip21_reg_t hri_nvic_read_NVICIP21_PRI21_bf(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP21.reg;
	tmp = (tmp & NVIC_NVICIP21_PRI21_Msk) >> 0;
	return tmp;
}

static inline void hri_nvic_set_NVICIP21_reg(const void *const hw, hri_nvic_nvicip21_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP21.reg |= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip21_reg_t hri_nvic_get_NVICIP21_reg(const void *const hw, hri_nvic_nvicip21_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP21.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_nvic_write_NVICIP21_reg(const void *const hw, hri_nvic_nvicip21_reg_t data)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP21.reg = data;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP21_reg(const void *const hw, hri_nvic_nvicip21_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP21.reg &= ~mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP21_reg(const void *const hw, hri_nvic_nvicip21_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP21.reg ^= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip21_reg_t hri_nvic_read_NVICIP21_reg(const void *const hw)
{
	return ((Nvic *)hw)->NVICIP21.reg;
}

static inline void hri_nvic_set_NVICIP22_PRI22_bf(const void *const hw, hri_nvic_nvicip22_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP22.reg |= NVIC_NVICIP22_PRI22(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip22_reg_t hri_nvic_get_NVICIP22_PRI22_bf(const void *const hw, hri_nvic_nvicip22_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP22.reg;
	tmp = (tmp & NVIC_NVICIP22_PRI22(mask)) >> 0;
	return tmp;
}

static inline void hri_nvic_write_NVICIP22_PRI22_bf(const void *const hw, hri_nvic_nvicip22_reg_t data)
{
	uint8_t tmp;
	NVIC_CRITICAL_SECTION_ENTER();
	tmp = ((Nvic *)hw)->NVICIP22.reg;
	tmp &= ~NVIC_NVICIP22_PRI22_Msk;
	tmp |= NVIC_NVICIP22_PRI22(data);
	((Nvic *)hw)->NVICIP22.reg = tmp;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP22_PRI22_bf(const void *const hw, hri_nvic_nvicip22_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP22.reg &= ~NVIC_NVICIP22_PRI22(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP22_PRI22_bf(const void *const hw, hri_nvic_nvicip22_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP22.reg ^= NVIC_NVICIP22_PRI22(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip22_reg_t hri_nvic_read_NVICIP22_PRI22_bf(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP22.reg;
	tmp = (tmp & NVIC_NVICIP22_PRI22_Msk) >> 0;
	return tmp;
}

static inline void hri_nvic_set_NVICIP22_reg(const void *const hw, hri_nvic_nvicip22_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP22.reg |= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip22_reg_t hri_nvic_get_NVICIP22_reg(const void *const hw, hri_nvic_nvicip22_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP22.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_nvic_write_NVICIP22_reg(const void *const hw, hri_nvic_nvicip22_reg_t data)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP22.reg = data;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP22_reg(const void *const hw, hri_nvic_nvicip22_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP22.reg &= ~mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP22_reg(const void *const hw, hri_nvic_nvicip22_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP22.reg ^= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip22_reg_t hri_nvic_read_NVICIP22_reg(const void *const hw)
{
	return ((Nvic *)hw)->NVICIP22.reg;
}

static inline void hri_nvic_set_NVICIP23_PRI23_bf(const void *const hw, hri_nvic_nvicip23_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP23.reg |= NVIC_NVICIP23_PRI23(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip23_reg_t hri_nvic_get_NVICIP23_PRI23_bf(const void *const hw, hri_nvic_nvicip23_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP23.reg;
	tmp = (tmp & NVIC_NVICIP23_PRI23(mask)) >> 0;
	return tmp;
}

static inline void hri_nvic_write_NVICIP23_PRI23_bf(const void *const hw, hri_nvic_nvicip23_reg_t data)
{
	uint8_t tmp;
	NVIC_CRITICAL_SECTION_ENTER();
	tmp = ((Nvic *)hw)->NVICIP23.reg;
	tmp &= ~NVIC_NVICIP23_PRI23_Msk;
	tmp |= NVIC_NVICIP23_PRI23(data);
	((Nvic *)hw)->NVICIP23.reg = tmp;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP23_PRI23_bf(const void *const hw, hri_nvic_nvicip23_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP23.reg &= ~NVIC_NVICIP23_PRI23(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP23_PRI23_bf(const void *const hw, hri_nvic_nvicip23_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP23.reg ^= NVIC_NVICIP23_PRI23(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip23_reg_t hri_nvic_read_NVICIP23_PRI23_bf(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP23.reg;
	tmp = (tmp & NVIC_NVICIP23_PRI23_Msk) >> 0;
	return tmp;
}

static inline void hri_nvic_set_NVICIP23_reg(const void *const hw, hri_nvic_nvicip23_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP23.reg |= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip23_reg_t hri_nvic_get_NVICIP23_reg(const void *const hw, hri_nvic_nvicip23_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP23.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_nvic_write_NVICIP23_reg(const void *const hw, hri_nvic_nvicip23_reg_t data)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP23.reg = data;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP23_reg(const void *const hw, hri_nvic_nvicip23_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP23.reg &= ~mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP23_reg(const void *const hw, hri_nvic_nvicip23_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP23.reg ^= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip23_reg_t hri_nvic_read_NVICIP23_reg(const void *const hw)
{
	return ((Nvic *)hw)->NVICIP23.reg;
}

static inline void hri_nvic_set_NVICIP24_PRI24_bf(const void *const hw, hri_nvic_nvicip24_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP24.reg |= NVIC_NVICIP24_PRI24(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip24_reg_t hri_nvic_get_NVICIP24_PRI24_bf(const void *const hw, hri_nvic_nvicip24_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP24.reg;
	tmp = (tmp & NVIC_NVICIP24_PRI24(mask)) >> 0;
	return tmp;
}

static inline void hri_nvic_write_NVICIP24_PRI24_bf(const void *const hw, hri_nvic_nvicip24_reg_t data)
{
	uint8_t tmp;
	NVIC_CRITICAL_SECTION_ENTER();
	tmp = ((Nvic *)hw)->NVICIP24.reg;
	tmp &= ~NVIC_NVICIP24_PRI24_Msk;
	tmp |= NVIC_NVICIP24_PRI24(data);
	((Nvic *)hw)->NVICIP24.reg = tmp;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP24_PRI24_bf(const void *const hw, hri_nvic_nvicip24_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP24.reg &= ~NVIC_NVICIP24_PRI24(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP24_PRI24_bf(const void *const hw, hri_nvic_nvicip24_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP24.reg ^= NVIC_NVICIP24_PRI24(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip24_reg_t hri_nvic_read_NVICIP24_PRI24_bf(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP24.reg;
	tmp = (tmp & NVIC_NVICIP24_PRI24_Msk) >> 0;
	return tmp;
}

static inline void hri_nvic_set_NVICIP24_reg(const void *const hw, hri_nvic_nvicip24_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP24.reg |= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip24_reg_t hri_nvic_get_NVICIP24_reg(const void *const hw, hri_nvic_nvicip24_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP24.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_nvic_write_NVICIP24_reg(const void *const hw, hri_nvic_nvicip24_reg_t data)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP24.reg = data;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP24_reg(const void *const hw, hri_nvic_nvicip24_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP24.reg &= ~mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP24_reg(const void *const hw, hri_nvic_nvicip24_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP24.reg ^= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip24_reg_t hri_nvic_read_NVICIP24_reg(const void *const hw)
{
	return ((Nvic *)hw)->NVICIP24.reg;
}

static inline void hri_nvic_set_NVICIP25_PRI25_bf(const void *const hw, hri_nvic_nvicip25_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP25.reg |= NVIC_NVICIP25_PRI25(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip25_reg_t hri_nvic_get_NVICIP25_PRI25_bf(const void *const hw, hri_nvic_nvicip25_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP25.reg;
	tmp = (tmp & NVIC_NVICIP25_PRI25(mask)) >> 0;
	return tmp;
}

static inline void hri_nvic_write_NVICIP25_PRI25_bf(const void *const hw, hri_nvic_nvicip25_reg_t data)
{
	uint8_t tmp;
	NVIC_CRITICAL_SECTION_ENTER();
	tmp = ((Nvic *)hw)->NVICIP25.reg;
	tmp &= ~NVIC_NVICIP25_PRI25_Msk;
	tmp |= NVIC_NVICIP25_PRI25(data);
	((Nvic *)hw)->NVICIP25.reg = tmp;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP25_PRI25_bf(const void *const hw, hri_nvic_nvicip25_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP25.reg &= ~NVIC_NVICIP25_PRI25(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP25_PRI25_bf(const void *const hw, hri_nvic_nvicip25_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP25.reg ^= NVIC_NVICIP25_PRI25(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip25_reg_t hri_nvic_read_NVICIP25_PRI25_bf(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP25.reg;
	tmp = (tmp & NVIC_NVICIP25_PRI25_Msk) >> 0;
	return tmp;
}

static inline void hri_nvic_set_NVICIP25_reg(const void *const hw, hri_nvic_nvicip25_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP25.reg |= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip25_reg_t hri_nvic_get_NVICIP25_reg(const void *const hw, hri_nvic_nvicip25_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP25.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_nvic_write_NVICIP25_reg(const void *const hw, hri_nvic_nvicip25_reg_t data)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP25.reg = data;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP25_reg(const void *const hw, hri_nvic_nvicip25_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP25.reg &= ~mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP25_reg(const void *const hw, hri_nvic_nvicip25_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP25.reg ^= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip25_reg_t hri_nvic_read_NVICIP25_reg(const void *const hw)
{
	return ((Nvic *)hw)->NVICIP25.reg;
}

static inline void hri_nvic_set_NVICIP26_PRI26_bf(const void *const hw, hri_nvic_nvicip26_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP26.reg |= NVIC_NVICIP26_PRI26(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip26_reg_t hri_nvic_get_NVICIP26_PRI26_bf(const void *const hw, hri_nvic_nvicip26_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP26.reg;
	tmp = (tmp & NVIC_NVICIP26_PRI26(mask)) >> 0;
	return tmp;
}

static inline void hri_nvic_write_NVICIP26_PRI26_bf(const void *const hw, hri_nvic_nvicip26_reg_t data)
{
	uint8_t tmp;
	NVIC_CRITICAL_SECTION_ENTER();
	tmp = ((Nvic *)hw)->NVICIP26.reg;
	tmp &= ~NVIC_NVICIP26_PRI26_Msk;
	tmp |= NVIC_NVICIP26_PRI26(data);
	((Nvic *)hw)->NVICIP26.reg = tmp;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP26_PRI26_bf(const void *const hw, hri_nvic_nvicip26_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP26.reg &= ~NVIC_NVICIP26_PRI26(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP26_PRI26_bf(const void *const hw, hri_nvic_nvicip26_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP26.reg ^= NVIC_NVICIP26_PRI26(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip26_reg_t hri_nvic_read_NVICIP26_PRI26_bf(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP26.reg;
	tmp = (tmp & NVIC_NVICIP26_PRI26_Msk) >> 0;
	return tmp;
}

static inline void hri_nvic_set_NVICIP26_reg(const void *const hw, hri_nvic_nvicip26_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP26.reg |= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip26_reg_t hri_nvic_get_NVICIP26_reg(const void *const hw, hri_nvic_nvicip26_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP26.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_nvic_write_NVICIP26_reg(const void *const hw, hri_nvic_nvicip26_reg_t data)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP26.reg = data;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP26_reg(const void *const hw, hri_nvic_nvicip26_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP26.reg &= ~mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP26_reg(const void *const hw, hri_nvic_nvicip26_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP26.reg ^= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip26_reg_t hri_nvic_read_NVICIP26_reg(const void *const hw)
{
	return ((Nvic *)hw)->NVICIP26.reg;
}

static inline void hri_nvic_set_NVICIP27_PRI27_bf(const void *const hw, hri_nvic_nvicip27_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP27.reg |= NVIC_NVICIP27_PRI27(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip27_reg_t hri_nvic_get_NVICIP27_PRI27_bf(const void *const hw, hri_nvic_nvicip27_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP27.reg;
	tmp = (tmp & NVIC_NVICIP27_PRI27(mask)) >> 0;
	return tmp;
}

static inline void hri_nvic_write_NVICIP27_PRI27_bf(const void *const hw, hri_nvic_nvicip27_reg_t data)
{
	uint8_t tmp;
	NVIC_CRITICAL_SECTION_ENTER();
	tmp = ((Nvic *)hw)->NVICIP27.reg;
	tmp &= ~NVIC_NVICIP27_PRI27_Msk;
	tmp |= NVIC_NVICIP27_PRI27(data);
	((Nvic *)hw)->NVICIP27.reg = tmp;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP27_PRI27_bf(const void *const hw, hri_nvic_nvicip27_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP27.reg &= ~NVIC_NVICIP27_PRI27(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP27_PRI27_bf(const void *const hw, hri_nvic_nvicip27_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP27.reg ^= NVIC_NVICIP27_PRI27(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip27_reg_t hri_nvic_read_NVICIP27_PRI27_bf(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP27.reg;
	tmp = (tmp & NVIC_NVICIP27_PRI27_Msk) >> 0;
	return tmp;
}

static inline void hri_nvic_set_NVICIP27_reg(const void *const hw, hri_nvic_nvicip27_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP27.reg |= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip27_reg_t hri_nvic_get_NVICIP27_reg(const void *const hw, hri_nvic_nvicip27_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP27.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_nvic_write_NVICIP27_reg(const void *const hw, hri_nvic_nvicip27_reg_t data)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP27.reg = data;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP27_reg(const void *const hw, hri_nvic_nvicip27_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP27.reg &= ~mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP27_reg(const void *const hw, hri_nvic_nvicip27_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP27.reg ^= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip27_reg_t hri_nvic_read_NVICIP27_reg(const void *const hw)
{
	return ((Nvic *)hw)->NVICIP27.reg;
}

static inline void hri_nvic_set_NVICIP28_PRI28_bf(const void *const hw, hri_nvic_nvicip28_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP28.reg |= NVIC_NVICIP28_PRI28(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip28_reg_t hri_nvic_get_NVICIP28_PRI28_bf(const void *const hw, hri_nvic_nvicip28_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP28.reg;
	tmp = (tmp & NVIC_NVICIP28_PRI28(mask)) >> 0;
	return tmp;
}

static inline void hri_nvic_write_NVICIP28_PRI28_bf(const void *const hw, hri_nvic_nvicip28_reg_t data)
{
	uint8_t tmp;
	NVIC_CRITICAL_SECTION_ENTER();
	tmp = ((Nvic *)hw)->NVICIP28.reg;
	tmp &= ~NVIC_NVICIP28_PRI28_Msk;
	tmp |= NVIC_NVICIP28_PRI28(data);
	((Nvic *)hw)->NVICIP28.reg = tmp;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP28_PRI28_bf(const void *const hw, hri_nvic_nvicip28_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP28.reg &= ~NVIC_NVICIP28_PRI28(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP28_PRI28_bf(const void *const hw, hri_nvic_nvicip28_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP28.reg ^= NVIC_NVICIP28_PRI28(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip28_reg_t hri_nvic_read_NVICIP28_PRI28_bf(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP28.reg;
	tmp = (tmp & NVIC_NVICIP28_PRI28_Msk) >> 0;
	return tmp;
}

static inline void hri_nvic_set_NVICIP28_reg(const void *const hw, hri_nvic_nvicip28_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP28.reg |= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip28_reg_t hri_nvic_get_NVICIP28_reg(const void *const hw, hri_nvic_nvicip28_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP28.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_nvic_write_NVICIP28_reg(const void *const hw, hri_nvic_nvicip28_reg_t data)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP28.reg = data;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP28_reg(const void *const hw, hri_nvic_nvicip28_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP28.reg &= ~mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP28_reg(const void *const hw, hri_nvic_nvicip28_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP28.reg ^= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip28_reg_t hri_nvic_read_NVICIP28_reg(const void *const hw)
{
	return ((Nvic *)hw)->NVICIP28.reg;
}

static inline void hri_nvic_set_NVICIP29_PRI29_bf(const void *const hw, hri_nvic_nvicip29_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP29.reg |= NVIC_NVICIP29_PRI29(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip29_reg_t hri_nvic_get_NVICIP29_PRI29_bf(const void *const hw, hri_nvic_nvicip29_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP29.reg;
	tmp = (tmp & NVIC_NVICIP29_PRI29(mask)) >> 0;
	return tmp;
}

static inline void hri_nvic_write_NVICIP29_PRI29_bf(const void *const hw, hri_nvic_nvicip29_reg_t data)
{
	uint8_t tmp;
	NVIC_CRITICAL_SECTION_ENTER();
	tmp = ((Nvic *)hw)->NVICIP29.reg;
	tmp &= ~NVIC_NVICIP29_PRI29_Msk;
	tmp |= NVIC_NVICIP29_PRI29(data);
	((Nvic *)hw)->NVICIP29.reg = tmp;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP29_PRI29_bf(const void *const hw, hri_nvic_nvicip29_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP29.reg &= ~NVIC_NVICIP29_PRI29(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP29_PRI29_bf(const void *const hw, hri_nvic_nvicip29_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP29.reg ^= NVIC_NVICIP29_PRI29(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip29_reg_t hri_nvic_read_NVICIP29_PRI29_bf(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP29.reg;
	tmp = (tmp & NVIC_NVICIP29_PRI29_Msk) >> 0;
	return tmp;
}

static inline void hri_nvic_set_NVICIP29_reg(const void *const hw, hri_nvic_nvicip29_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP29.reg |= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip29_reg_t hri_nvic_get_NVICIP29_reg(const void *const hw, hri_nvic_nvicip29_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP29.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_nvic_write_NVICIP29_reg(const void *const hw, hri_nvic_nvicip29_reg_t data)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP29.reg = data;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP29_reg(const void *const hw, hri_nvic_nvicip29_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP29.reg &= ~mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP29_reg(const void *const hw, hri_nvic_nvicip29_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP29.reg ^= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip29_reg_t hri_nvic_read_NVICIP29_reg(const void *const hw)
{
	return ((Nvic *)hw)->NVICIP29.reg;
}

static inline void hri_nvic_set_NVICIP30_PRI30_bf(const void *const hw, hri_nvic_nvicip30_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP30.reg |= NVIC_NVICIP30_PRI30(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip30_reg_t hri_nvic_get_NVICIP30_PRI30_bf(const void *const hw, hri_nvic_nvicip30_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP30.reg;
	tmp = (tmp & NVIC_NVICIP30_PRI30(mask)) >> 0;
	return tmp;
}

static inline void hri_nvic_write_NVICIP30_PRI30_bf(const void *const hw, hri_nvic_nvicip30_reg_t data)
{
	uint8_t tmp;
	NVIC_CRITICAL_SECTION_ENTER();
	tmp = ((Nvic *)hw)->NVICIP30.reg;
	tmp &= ~NVIC_NVICIP30_PRI30_Msk;
	tmp |= NVIC_NVICIP30_PRI30(data);
	((Nvic *)hw)->NVICIP30.reg = tmp;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP30_PRI30_bf(const void *const hw, hri_nvic_nvicip30_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP30.reg &= ~NVIC_NVICIP30_PRI30(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP30_PRI30_bf(const void *const hw, hri_nvic_nvicip30_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP30.reg ^= NVIC_NVICIP30_PRI30(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip30_reg_t hri_nvic_read_NVICIP30_PRI30_bf(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP30.reg;
	tmp = (tmp & NVIC_NVICIP30_PRI30_Msk) >> 0;
	return tmp;
}

static inline void hri_nvic_set_NVICIP30_reg(const void *const hw, hri_nvic_nvicip30_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP30.reg |= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip30_reg_t hri_nvic_get_NVICIP30_reg(const void *const hw, hri_nvic_nvicip30_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP30.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_nvic_write_NVICIP30_reg(const void *const hw, hri_nvic_nvicip30_reg_t data)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP30.reg = data;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP30_reg(const void *const hw, hri_nvic_nvicip30_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP30.reg &= ~mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP30_reg(const void *const hw, hri_nvic_nvicip30_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP30.reg ^= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip30_reg_t hri_nvic_read_NVICIP30_reg(const void *const hw)
{
	return ((Nvic *)hw)->NVICIP30.reg;
}

static inline void hri_nvic_set_NVICIP31_PRI31_bf(const void *const hw, hri_nvic_nvicip31_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP31.reg |= NVIC_NVICIP31_PRI31(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip31_reg_t hri_nvic_get_NVICIP31_PRI31_bf(const void *const hw, hri_nvic_nvicip31_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP31.reg;
	tmp = (tmp & NVIC_NVICIP31_PRI31(mask)) >> 0;
	return tmp;
}

static inline void hri_nvic_write_NVICIP31_PRI31_bf(const void *const hw, hri_nvic_nvicip31_reg_t data)
{
	uint8_t tmp;
	NVIC_CRITICAL_SECTION_ENTER();
	tmp = ((Nvic *)hw)->NVICIP31.reg;
	tmp &= ~NVIC_NVICIP31_PRI31_Msk;
	tmp |= NVIC_NVICIP31_PRI31(data);
	((Nvic *)hw)->NVICIP31.reg = tmp;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP31_PRI31_bf(const void *const hw, hri_nvic_nvicip31_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP31.reg &= ~NVIC_NVICIP31_PRI31(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP31_PRI31_bf(const void *const hw, hri_nvic_nvicip31_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP31.reg ^= NVIC_NVICIP31_PRI31(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip31_reg_t hri_nvic_read_NVICIP31_PRI31_bf(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP31.reg;
	tmp = (tmp & NVIC_NVICIP31_PRI31_Msk) >> 0;
	return tmp;
}

static inline void hri_nvic_set_NVICIP31_reg(const void *const hw, hri_nvic_nvicip31_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP31.reg |= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip31_reg_t hri_nvic_get_NVICIP31_reg(const void *const hw, hri_nvic_nvicip31_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP31.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_nvic_write_NVICIP31_reg(const void *const hw, hri_nvic_nvicip31_reg_t data)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP31.reg = data;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP31_reg(const void *const hw, hri_nvic_nvicip31_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP31.reg &= ~mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP31_reg(const void *const hw, hri_nvic_nvicip31_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP31.reg ^= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip31_reg_t hri_nvic_read_NVICIP31_reg(const void *const hw)
{
	return ((Nvic *)hw)->NVICIP31.reg;
}

static inline void hri_nvic_set_NVICIP32_PRI32_bf(const void *const hw, hri_nvic_nvicip32_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP32.reg |= NVIC_NVICIP32_PRI32(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip32_reg_t hri_nvic_get_NVICIP32_PRI32_bf(const void *const hw, hri_nvic_nvicip32_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP32.reg;
	tmp = (tmp & NVIC_NVICIP32_PRI32(mask)) >> 0;
	return tmp;
}

static inline void hri_nvic_write_NVICIP32_PRI32_bf(const void *const hw, hri_nvic_nvicip32_reg_t data)
{
	uint8_t tmp;
	NVIC_CRITICAL_SECTION_ENTER();
	tmp = ((Nvic *)hw)->NVICIP32.reg;
	tmp &= ~NVIC_NVICIP32_PRI32_Msk;
	tmp |= NVIC_NVICIP32_PRI32(data);
	((Nvic *)hw)->NVICIP32.reg = tmp;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP32_PRI32_bf(const void *const hw, hri_nvic_nvicip32_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP32.reg &= ~NVIC_NVICIP32_PRI32(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP32_PRI32_bf(const void *const hw, hri_nvic_nvicip32_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP32.reg ^= NVIC_NVICIP32_PRI32(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip32_reg_t hri_nvic_read_NVICIP32_PRI32_bf(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP32.reg;
	tmp = (tmp & NVIC_NVICIP32_PRI32_Msk) >> 0;
	return tmp;
}

static inline void hri_nvic_set_NVICIP32_reg(const void *const hw, hri_nvic_nvicip32_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP32.reg |= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip32_reg_t hri_nvic_get_NVICIP32_reg(const void *const hw, hri_nvic_nvicip32_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP32.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_nvic_write_NVICIP32_reg(const void *const hw, hri_nvic_nvicip32_reg_t data)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP32.reg = data;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP32_reg(const void *const hw, hri_nvic_nvicip32_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP32.reg &= ~mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP32_reg(const void *const hw, hri_nvic_nvicip32_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP32.reg ^= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip32_reg_t hri_nvic_read_NVICIP32_reg(const void *const hw)
{
	return ((Nvic *)hw)->NVICIP32.reg;
}

static inline void hri_nvic_set_NVICIP33_PRI33_bf(const void *const hw, hri_nvic_nvicip33_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP33.reg |= NVIC_NVICIP33_PRI33(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip33_reg_t hri_nvic_get_NVICIP33_PRI33_bf(const void *const hw, hri_nvic_nvicip33_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP33.reg;
	tmp = (tmp & NVIC_NVICIP33_PRI33(mask)) >> 0;
	return tmp;
}

static inline void hri_nvic_write_NVICIP33_PRI33_bf(const void *const hw, hri_nvic_nvicip33_reg_t data)
{
	uint8_t tmp;
	NVIC_CRITICAL_SECTION_ENTER();
	tmp = ((Nvic *)hw)->NVICIP33.reg;
	tmp &= ~NVIC_NVICIP33_PRI33_Msk;
	tmp |= NVIC_NVICIP33_PRI33(data);
	((Nvic *)hw)->NVICIP33.reg = tmp;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP33_PRI33_bf(const void *const hw, hri_nvic_nvicip33_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP33.reg &= ~NVIC_NVICIP33_PRI33(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP33_PRI33_bf(const void *const hw, hri_nvic_nvicip33_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP33.reg ^= NVIC_NVICIP33_PRI33(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip33_reg_t hri_nvic_read_NVICIP33_PRI33_bf(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP33.reg;
	tmp = (tmp & NVIC_NVICIP33_PRI33_Msk) >> 0;
	return tmp;
}

static inline void hri_nvic_set_NVICIP33_reg(const void *const hw, hri_nvic_nvicip33_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP33.reg |= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip33_reg_t hri_nvic_get_NVICIP33_reg(const void *const hw, hri_nvic_nvicip33_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP33.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_nvic_write_NVICIP33_reg(const void *const hw, hri_nvic_nvicip33_reg_t data)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP33.reg = data;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP33_reg(const void *const hw, hri_nvic_nvicip33_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP33.reg &= ~mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP33_reg(const void *const hw, hri_nvic_nvicip33_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP33.reg ^= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip33_reg_t hri_nvic_read_NVICIP33_reg(const void *const hw)
{
	return ((Nvic *)hw)->NVICIP33.reg;
}

static inline void hri_nvic_set_NVICIP34_PRI34_bf(const void *const hw, hri_nvic_nvicip34_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP34.reg |= NVIC_NVICIP34_PRI34(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip34_reg_t hri_nvic_get_NVICIP34_PRI34_bf(const void *const hw, hri_nvic_nvicip34_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP34.reg;
	tmp = (tmp & NVIC_NVICIP34_PRI34(mask)) >> 0;
	return tmp;
}

static inline void hri_nvic_write_NVICIP34_PRI34_bf(const void *const hw, hri_nvic_nvicip34_reg_t data)
{
	uint8_t tmp;
	NVIC_CRITICAL_SECTION_ENTER();
	tmp = ((Nvic *)hw)->NVICIP34.reg;
	tmp &= ~NVIC_NVICIP34_PRI34_Msk;
	tmp |= NVIC_NVICIP34_PRI34(data);
	((Nvic *)hw)->NVICIP34.reg = tmp;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP34_PRI34_bf(const void *const hw, hri_nvic_nvicip34_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP34.reg &= ~NVIC_NVICIP34_PRI34(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP34_PRI34_bf(const void *const hw, hri_nvic_nvicip34_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP34.reg ^= NVIC_NVICIP34_PRI34(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip34_reg_t hri_nvic_read_NVICIP34_PRI34_bf(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP34.reg;
	tmp = (tmp & NVIC_NVICIP34_PRI34_Msk) >> 0;
	return tmp;
}

static inline void hri_nvic_set_NVICIP34_reg(const void *const hw, hri_nvic_nvicip34_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP34.reg |= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip34_reg_t hri_nvic_get_NVICIP34_reg(const void *const hw, hri_nvic_nvicip34_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP34.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_nvic_write_NVICIP34_reg(const void *const hw, hri_nvic_nvicip34_reg_t data)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP34.reg = data;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP34_reg(const void *const hw, hri_nvic_nvicip34_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP34.reg &= ~mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP34_reg(const void *const hw, hri_nvic_nvicip34_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP34.reg ^= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip34_reg_t hri_nvic_read_NVICIP34_reg(const void *const hw)
{
	return ((Nvic *)hw)->NVICIP34.reg;
}

static inline void hri_nvic_set_NVICIP35_PRI35_bf(const void *const hw, hri_nvic_nvicip35_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP35.reg |= NVIC_NVICIP35_PRI35(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip35_reg_t hri_nvic_get_NVICIP35_PRI35_bf(const void *const hw, hri_nvic_nvicip35_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP35.reg;
	tmp = (tmp & NVIC_NVICIP35_PRI35(mask)) >> 0;
	return tmp;
}

static inline void hri_nvic_write_NVICIP35_PRI35_bf(const void *const hw, hri_nvic_nvicip35_reg_t data)
{
	uint8_t tmp;
	NVIC_CRITICAL_SECTION_ENTER();
	tmp = ((Nvic *)hw)->NVICIP35.reg;
	tmp &= ~NVIC_NVICIP35_PRI35_Msk;
	tmp |= NVIC_NVICIP35_PRI35(data);
	((Nvic *)hw)->NVICIP35.reg = tmp;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP35_PRI35_bf(const void *const hw, hri_nvic_nvicip35_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP35.reg &= ~NVIC_NVICIP35_PRI35(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP35_PRI35_bf(const void *const hw, hri_nvic_nvicip35_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP35.reg ^= NVIC_NVICIP35_PRI35(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip35_reg_t hri_nvic_read_NVICIP35_PRI35_bf(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP35.reg;
	tmp = (tmp & NVIC_NVICIP35_PRI35_Msk) >> 0;
	return tmp;
}

static inline void hri_nvic_set_NVICIP35_reg(const void *const hw, hri_nvic_nvicip35_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP35.reg |= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip35_reg_t hri_nvic_get_NVICIP35_reg(const void *const hw, hri_nvic_nvicip35_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP35.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_nvic_write_NVICIP35_reg(const void *const hw, hri_nvic_nvicip35_reg_t data)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP35.reg = data;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP35_reg(const void *const hw, hri_nvic_nvicip35_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP35.reg &= ~mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP35_reg(const void *const hw, hri_nvic_nvicip35_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP35.reg ^= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip35_reg_t hri_nvic_read_NVICIP35_reg(const void *const hw)
{
	return ((Nvic *)hw)->NVICIP35.reg;
}

static inline void hri_nvic_set_NVICIP36_PRI36_bf(const void *const hw, hri_nvic_nvicip36_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP36.reg |= NVIC_NVICIP36_PRI36(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip36_reg_t hri_nvic_get_NVICIP36_PRI36_bf(const void *const hw, hri_nvic_nvicip36_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP36.reg;
	tmp = (tmp & NVIC_NVICIP36_PRI36(mask)) >> 0;
	return tmp;
}

static inline void hri_nvic_write_NVICIP36_PRI36_bf(const void *const hw, hri_nvic_nvicip36_reg_t data)
{
	uint8_t tmp;
	NVIC_CRITICAL_SECTION_ENTER();
	tmp = ((Nvic *)hw)->NVICIP36.reg;
	tmp &= ~NVIC_NVICIP36_PRI36_Msk;
	tmp |= NVIC_NVICIP36_PRI36(data);
	((Nvic *)hw)->NVICIP36.reg = tmp;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP36_PRI36_bf(const void *const hw, hri_nvic_nvicip36_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP36.reg &= ~NVIC_NVICIP36_PRI36(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP36_PRI36_bf(const void *const hw, hri_nvic_nvicip36_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP36.reg ^= NVIC_NVICIP36_PRI36(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip36_reg_t hri_nvic_read_NVICIP36_PRI36_bf(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP36.reg;
	tmp = (tmp & NVIC_NVICIP36_PRI36_Msk) >> 0;
	return tmp;
}

static inline void hri_nvic_set_NVICIP36_reg(const void *const hw, hri_nvic_nvicip36_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP36.reg |= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip36_reg_t hri_nvic_get_NVICIP36_reg(const void *const hw, hri_nvic_nvicip36_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP36.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_nvic_write_NVICIP36_reg(const void *const hw, hri_nvic_nvicip36_reg_t data)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP36.reg = data;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP36_reg(const void *const hw, hri_nvic_nvicip36_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP36.reg &= ~mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP36_reg(const void *const hw, hri_nvic_nvicip36_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP36.reg ^= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip36_reg_t hri_nvic_read_NVICIP36_reg(const void *const hw)
{
	return ((Nvic *)hw)->NVICIP36.reg;
}

static inline void hri_nvic_set_NVICIP37_PRI37_bf(const void *const hw, hri_nvic_nvicip37_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP37.reg |= NVIC_NVICIP37_PRI37(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip37_reg_t hri_nvic_get_NVICIP37_PRI37_bf(const void *const hw, hri_nvic_nvicip37_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP37.reg;
	tmp = (tmp & NVIC_NVICIP37_PRI37(mask)) >> 0;
	return tmp;
}

static inline void hri_nvic_write_NVICIP37_PRI37_bf(const void *const hw, hri_nvic_nvicip37_reg_t data)
{
	uint8_t tmp;
	NVIC_CRITICAL_SECTION_ENTER();
	tmp = ((Nvic *)hw)->NVICIP37.reg;
	tmp &= ~NVIC_NVICIP37_PRI37_Msk;
	tmp |= NVIC_NVICIP37_PRI37(data);
	((Nvic *)hw)->NVICIP37.reg = tmp;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP37_PRI37_bf(const void *const hw, hri_nvic_nvicip37_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP37.reg &= ~NVIC_NVICIP37_PRI37(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP37_PRI37_bf(const void *const hw, hri_nvic_nvicip37_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP37.reg ^= NVIC_NVICIP37_PRI37(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip37_reg_t hri_nvic_read_NVICIP37_PRI37_bf(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP37.reg;
	tmp = (tmp & NVIC_NVICIP37_PRI37_Msk) >> 0;
	return tmp;
}

static inline void hri_nvic_set_NVICIP37_reg(const void *const hw, hri_nvic_nvicip37_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP37.reg |= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip37_reg_t hri_nvic_get_NVICIP37_reg(const void *const hw, hri_nvic_nvicip37_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP37.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_nvic_write_NVICIP37_reg(const void *const hw, hri_nvic_nvicip37_reg_t data)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP37.reg = data;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP37_reg(const void *const hw, hri_nvic_nvicip37_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP37.reg &= ~mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP37_reg(const void *const hw, hri_nvic_nvicip37_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP37.reg ^= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip37_reg_t hri_nvic_read_NVICIP37_reg(const void *const hw)
{
	return ((Nvic *)hw)->NVICIP37.reg;
}

static inline void hri_nvic_set_NVICIP38_PRI38_bf(const void *const hw, hri_nvic_nvicip38_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP38.reg |= NVIC_NVICIP38_PRI38(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip38_reg_t hri_nvic_get_NVICIP38_PRI38_bf(const void *const hw, hri_nvic_nvicip38_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP38.reg;
	tmp = (tmp & NVIC_NVICIP38_PRI38(mask)) >> 0;
	return tmp;
}

static inline void hri_nvic_write_NVICIP38_PRI38_bf(const void *const hw, hri_nvic_nvicip38_reg_t data)
{
	uint8_t tmp;
	NVIC_CRITICAL_SECTION_ENTER();
	tmp = ((Nvic *)hw)->NVICIP38.reg;
	tmp &= ~NVIC_NVICIP38_PRI38_Msk;
	tmp |= NVIC_NVICIP38_PRI38(data);
	((Nvic *)hw)->NVICIP38.reg = tmp;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP38_PRI38_bf(const void *const hw, hri_nvic_nvicip38_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP38.reg &= ~NVIC_NVICIP38_PRI38(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP38_PRI38_bf(const void *const hw, hri_nvic_nvicip38_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP38.reg ^= NVIC_NVICIP38_PRI38(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip38_reg_t hri_nvic_read_NVICIP38_PRI38_bf(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP38.reg;
	tmp = (tmp & NVIC_NVICIP38_PRI38_Msk) >> 0;
	return tmp;
}

static inline void hri_nvic_set_NVICIP38_reg(const void *const hw, hri_nvic_nvicip38_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP38.reg |= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip38_reg_t hri_nvic_get_NVICIP38_reg(const void *const hw, hri_nvic_nvicip38_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP38.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_nvic_write_NVICIP38_reg(const void *const hw, hri_nvic_nvicip38_reg_t data)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP38.reg = data;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP38_reg(const void *const hw, hri_nvic_nvicip38_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP38.reg &= ~mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP38_reg(const void *const hw, hri_nvic_nvicip38_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP38.reg ^= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip38_reg_t hri_nvic_read_NVICIP38_reg(const void *const hw)
{
	return ((Nvic *)hw)->NVICIP38.reg;
}

static inline void hri_nvic_set_NVICIP39_PRI39_bf(const void *const hw, hri_nvic_nvicip39_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP39.reg |= NVIC_NVICIP39_PRI39(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip39_reg_t hri_nvic_get_NVICIP39_PRI39_bf(const void *const hw, hri_nvic_nvicip39_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP39.reg;
	tmp = (tmp & NVIC_NVICIP39_PRI39(mask)) >> 0;
	return tmp;
}

static inline void hri_nvic_write_NVICIP39_PRI39_bf(const void *const hw, hri_nvic_nvicip39_reg_t data)
{
	uint8_t tmp;
	NVIC_CRITICAL_SECTION_ENTER();
	tmp = ((Nvic *)hw)->NVICIP39.reg;
	tmp &= ~NVIC_NVICIP39_PRI39_Msk;
	tmp |= NVIC_NVICIP39_PRI39(data);
	((Nvic *)hw)->NVICIP39.reg = tmp;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP39_PRI39_bf(const void *const hw, hri_nvic_nvicip39_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP39.reg &= ~NVIC_NVICIP39_PRI39(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP39_PRI39_bf(const void *const hw, hri_nvic_nvicip39_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP39.reg ^= NVIC_NVICIP39_PRI39(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip39_reg_t hri_nvic_read_NVICIP39_PRI39_bf(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP39.reg;
	tmp = (tmp & NVIC_NVICIP39_PRI39_Msk) >> 0;
	return tmp;
}

static inline void hri_nvic_set_NVICIP39_reg(const void *const hw, hri_nvic_nvicip39_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP39.reg |= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip39_reg_t hri_nvic_get_NVICIP39_reg(const void *const hw, hri_nvic_nvicip39_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP39.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_nvic_write_NVICIP39_reg(const void *const hw, hri_nvic_nvicip39_reg_t data)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP39.reg = data;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP39_reg(const void *const hw, hri_nvic_nvicip39_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP39.reg &= ~mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP39_reg(const void *const hw, hri_nvic_nvicip39_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP39.reg ^= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip39_reg_t hri_nvic_read_NVICIP39_reg(const void *const hw)
{
	return ((Nvic *)hw)->NVICIP39.reg;
}

static inline void hri_nvic_set_NVICIP40_PRI40_bf(const void *const hw, hri_nvic_nvicip40_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP40.reg |= NVIC_NVICIP40_PRI40(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip40_reg_t hri_nvic_get_NVICIP40_PRI40_bf(const void *const hw, hri_nvic_nvicip40_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP40.reg;
	tmp = (tmp & NVIC_NVICIP40_PRI40(mask)) >> 0;
	return tmp;
}

static inline void hri_nvic_write_NVICIP40_PRI40_bf(const void *const hw, hri_nvic_nvicip40_reg_t data)
{
	uint8_t tmp;
	NVIC_CRITICAL_SECTION_ENTER();
	tmp = ((Nvic *)hw)->NVICIP40.reg;
	tmp &= ~NVIC_NVICIP40_PRI40_Msk;
	tmp |= NVIC_NVICIP40_PRI40(data);
	((Nvic *)hw)->NVICIP40.reg = tmp;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP40_PRI40_bf(const void *const hw, hri_nvic_nvicip40_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP40.reg &= ~NVIC_NVICIP40_PRI40(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP40_PRI40_bf(const void *const hw, hri_nvic_nvicip40_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP40.reg ^= NVIC_NVICIP40_PRI40(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip40_reg_t hri_nvic_read_NVICIP40_PRI40_bf(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP40.reg;
	tmp = (tmp & NVIC_NVICIP40_PRI40_Msk) >> 0;
	return tmp;
}

static inline void hri_nvic_set_NVICIP40_reg(const void *const hw, hri_nvic_nvicip40_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP40.reg |= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip40_reg_t hri_nvic_get_NVICIP40_reg(const void *const hw, hri_nvic_nvicip40_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP40.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_nvic_write_NVICIP40_reg(const void *const hw, hri_nvic_nvicip40_reg_t data)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP40.reg = data;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP40_reg(const void *const hw, hri_nvic_nvicip40_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP40.reg &= ~mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP40_reg(const void *const hw, hri_nvic_nvicip40_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP40.reg ^= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip40_reg_t hri_nvic_read_NVICIP40_reg(const void *const hw)
{
	return ((Nvic *)hw)->NVICIP40.reg;
}

static inline void hri_nvic_set_NVICIP41_PRI41_bf(const void *const hw, hri_nvic_nvicip41_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP41.reg |= NVIC_NVICIP41_PRI41(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip41_reg_t hri_nvic_get_NVICIP41_PRI41_bf(const void *const hw, hri_nvic_nvicip41_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP41.reg;
	tmp = (tmp & NVIC_NVICIP41_PRI41(mask)) >> 0;
	return tmp;
}

static inline void hri_nvic_write_NVICIP41_PRI41_bf(const void *const hw, hri_nvic_nvicip41_reg_t data)
{
	uint8_t tmp;
	NVIC_CRITICAL_SECTION_ENTER();
	tmp = ((Nvic *)hw)->NVICIP41.reg;
	tmp &= ~NVIC_NVICIP41_PRI41_Msk;
	tmp |= NVIC_NVICIP41_PRI41(data);
	((Nvic *)hw)->NVICIP41.reg = tmp;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP41_PRI41_bf(const void *const hw, hri_nvic_nvicip41_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP41.reg &= ~NVIC_NVICIP41_PRI41(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP41_PRI41_bf(const void *const hw, hri_nvic_nvicip41_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP41.reg ^= NVIC_NVICIP41_PRI41(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip41_reg_t hri_nvic_read_NVICIP41_PRI41_bf(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP41.reg;
	tmp = (tmp & NVIC_NVICIP41_PRI41_Msk) >> 0;
	return tmp;
}

static inline void hri_nvic_set_NVICIP41_reg(const void *const hw, hri_nvic_nvicip41_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP41.reg |= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip41_reg_t hri_nvic_get_NVICIP41_reg(const void *const hw, hri_nvic_nvicip41_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP41.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_nvic_write_NVICIP41_reg(const void *const hw, hri_nvic_nvicip41_reg_t data)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP41.reg = data;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP41_reg(const void *const hw, hri_nvic_nvicip41_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP41.reg &= ~mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP41_reg(const void *const hw, hri_nvic_nvicip41_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP41.reg ^= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip41_reg_t hri_nvic_read_NVICIP41_reg(const void *const hw)
{
	return ((Nvic *)hw)->NVICIP41.reg;
}

static inline void hri_nvic_set_NVICIP42_PRI42_bf(const void *const hw, hri_nvic_nvicip42_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP42.reg |= NVIC_NVICIP42_PRI42(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip42_reg_t hri_nvic_get_NVICIP42_PRI42_bf(const void *const hw, hri_nvic_nvicip42_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP42.reg;
	tmp = (tmp & NVIC_NVICIP42_PRI42(mask)) >> 0;
	return tmp;
}

static inline void hri_nvic_write_NVICIP42_PRI42_bf(const void *const hw, hri_nvic_nvicip42_reg_t data)
{
	uint8_t tmp;
	NVIC_CRITICAL_SECTION_ENTER();
	tmp = ((Nvic *)hw)->NVICIP42.reg;
	tmp &= ~NVIC_NVICIP42_PRI42_Msk;
	tmp |= NVIC_NVICIP42_PRI42(data);
	((Nvic *)hw)->NVICIP42.reg = tmp;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP42_PRI42_bf(const void *const hw, hri_nvic_nvicip42_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP42.reg &= ~NVIC_NVICIP42_PRI42(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP42_PRI42_bf(const void *const hw, hri_nvic_nvicip42_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP42.reg ^= NVIC_NVICIP42_PRI42(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip42_reg_t hri_nvic_read_NVICIP42_PRI42_bf(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP42.reg;
	tmp = (tmp & NVIC_NVICIP42_PRI42_Msk) >> 0;
	return tmp;
}

static inline void hri_nvic_set_NVICIP42_reg(const void *const hw, hri_nvic_nvicip42_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP42.reg |= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip42_reg_t hri_nvic_get_NVICIP42_reg(const void *const hw, hri_nvic_nvicip42_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP42.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_nvic_write_NVICIP42_reg(const void *const hw, hri_nvic_nvicip42_reg_t data)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP42.reg = data;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP42_reg(const void *const hw, hri_nvic_nvicip42_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP42.reg &= ~mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP42_reg(const void *const hw, hri_nvic_nvicip42_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP42.reg ^= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip42_reg_t hri_nvic_read_NVICIP42_reg(const void *const hw)
{
	return ((Nvic *)hw)->NVICIP42.reg;
}

static inline void hri_nvic_set_NVICIP43_PRI43_bf(const void *const hw, hri_nvic_nvicip43_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP43.reg |= NVIC_NVICIP43_PRI43(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip43_reg_t hri_nvic_get_NVICIP43_PRI43_bf(const void *const hw, hri_nvic_nvicip43_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP43.reg;
	tmp = (tmp & NVIC_NVICIP43_PRI43(mask)) >> 0;
	return tmp;
}

static inline void hri_nvic_write_NVICIP43_PRI43_bf(const void *const hw, hri_nvic_nvicip43_reg_t data)
{
	uint8_t tmp;
	NVIC_CRITICAL_SECTION_ENTER();
	tmp = ((Nvic *)hw)->NVICIP43.reg;
	tmp &= ~NVIC_NVICIP43_PRI43_Msk;
	tmp |= NVIC_NVICIP43_PRI43(data);
	((Nvic *)hw)->NVICIP43.reg = tmp;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP43_PRI43_bf(const void *const hw, hri_nvic_nvicip43_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP43.reg &= ~NVIC_NVICIP43_PRI43(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP43_PRI43_bf(const void *const hw, hri_nvic_nvicip43_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP43.reg ^= NVIC_NVICIP43_PRI43(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip43_reg_t hri_nvic_read_NVICIP43_PRI43_bf(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP43.reg;
	tmp = (tmp & NVIC_NVICIP43_PRI43_Msk) >> 0;
	return tmp;
}

static inline void hri_nvic_set_NVICIP43_reg(const void *const hw, hri_nvic_nvicip43_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP43.reg |= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip43_reg_t hri_nvic_get_NVICIP43_reg(const void *const hw, hri_nvic_nvicip43_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP43.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_nvic_write_NVICIP43_reg(const void *const hw, hri_nvic_nvicip43_reg_t data)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP43.reg = data;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP43_reg(const void *const hw, hri_nvic_nvicip43_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP43.reg &= ~mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP43_reg(const void *const hw, hri_nvic_nvicip43_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP43.reg ^= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip43_reg_t hri_nvic_read_NVICIP43_reg(const void *const hw)
{
	return ((Nvic *)hw)->NVICIP43.reg;
}

static inline void hri_nvic_set_NVICIP44_PRI44_bf(const void *const hw, hri_nvic_nvicip44_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP44.reg |= NVIC_NVICIP44_PRI44(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip44_reg_t hri_nvic_get_NVICIP44_PRI44_bf(const void *const hw, hri_nvic_nvicip44_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP44.reg;
	tmp = (tmp & NVIC_NVICIP44_PRI44(mask)) >> 0;
	return tmp;
}

static inline void hri_nvic_write_NVICIP44_PRI44_bf(const void *const hw, hri_nvic_nvicip44_reg_t data)
{
	uint8_t tmp;
	NVIC_CRITICAL_SECTION_ENTER();
	tmp = ((Nvic *)hw)->NVICIP44.reg;
	tmp &= ~NVIC_NVICIP44_PRI44_Msk;
	tmp |= NVIC_NVICIP44_PRI44(data);
	((Nvic *)hw)->NVICIP44.reg = tmp;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP44_PRI44_bf(const void *const hw, hri_nvic_nvicip44_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP44.reg &= ~NVIC_NVICIP44_PRI44(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP44_PRI44_bf(const void *const hw, hri_nvic_nvicip44_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP44.reg ^= NVIC_NVICIP44_PRI44(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip44_reg_t hri_nvic_read_NVICIP44_PRI44_bf(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP44.reg;
	tmp = (tmp & NVIC_NVICIP44_PRI44_Msk) >> 0;
	return tmp;
}

static inline void hri_nvic_set_NVICIP44_reg(const void *const hw, hri_nvic_nvicip44_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP44.reg |= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip44_reg_t hri_nvic_get_NVICIP44_reg(const void *const hw, hri_nvic_nvicip44_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP44.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_nvic_write_NVICIP44_reg(const void *const hw, hri_nvic_nvicip44_reg_t data)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP44.reg = data;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP44_reg(const void *const hw, hri_nvic_nvicip44_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP44.reg &= ~mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP44_reg(const void *const hw, hri_nvic_nvicip44_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP44.reg ^= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip44_reg_t hri_nvic_read_NVICIP44_reg(const void *const hw)
{
	return ((Nvic *)hw)->NVICIP44.reg;
}

static inline void hri_nvic_set_NVICIP45_PRI45_bf(const void *const hw, hri_nvic_nvicip45_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP45.reg |= NVIC_NVICIP45_PRI45(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip45_reg_t hri_nvic_get_NVICIP45_PRI45_bf(const void *const hw, hri_nvic_nvicip45_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP45.reg;
	tmp = (tmp & NVIC_NVICIP45_PRI45(mask)) >> 0;
	return tmp;
}

static inline void hri_nvic_write_NVICIP45_PRI45_bf(const void *const hw, hri_nvic_nvicip45_reg_t data)
{
	uint8_t tmp;
	NVIC_CRITICAL_SECTION_ENTER();
	tmp = ((Nvic *)hw)->NVICIP45.reg;
	tmp &= ~NVIC_NVICIP45_PRI45_Msk;
	tmp |= NVIC_NVICIP45_PRI45(data);
	((Nvic *)hw)->NVICIP45.reg = tmp;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP45_PRI45_bf(const void *const hw, hri_nvic_nvicip45_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP45.reg &= ~NVIC_NVICIP45_PRI45(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP45_PRI45_bf(const void *const hw, hri_nvic_nvicip45_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP45.reg ^= NVIC_NVICIP45_PRI45(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip45_reg_t hri_nvic_read_NVICIP45_PRI45_bf(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP45.reg;
	tmp = (tmp & NVIC_NVICIP45_PRI45_Msk) >> 0;
	return tmp;
}

static inline void hri_nvic_set_NVICIP45_reg(const void *const hw, hri_nvic_nvicip45_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP45.reg |= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip45_reg_t hri_nvic_get_NVICIP45_reg(const void *const hw, hri_nvic_nvicip45_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP45.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_nvic_write_NVICIP45_reg(const void *const hw, hri_nvic_nvicip45_reg_t data)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP45.reg = data;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP45_reg(const void *const hw, hri_nvic_nvicip45_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP45.reg &= ~mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP45_reg(const void *const hw, hri_nvic_nvicip45_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP45.reg ^= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip45_reg_t hri_nvic_read_NVICIP45_reg(const void *const hw)
{
	return ((Nvic *)hw)->NVICIP45.reg;
}

static inline void hri_nvic_set_NVICIP46_PRI46_bf(const void *const hw, hri_nvic_nvicip46_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP46.reg |= NVIC_NVICIP46_PRI46(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip46_reg_t hri_nvic_get_NVICIP46_PRI46_bf(const void *const hw, hri_nvic_nvicip46_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP46.reg;
	tmp = (tmp & NVIC_NVICIP46_PRI46(mask)) >> 0;
	return tmp;
}

static inline void hri_nvic_write_NVICIP46_PRI46_bf(const void *const hw, hri_nvic_nvicip46_reg_t data)
{
	uint8_t tmp;
	NVIC_CRITICAL_SECTION_ENTER();
	tmp = ((Nvic *)hw)->NVICIP46.reg;
	tmp &= ~NVIC_NVICIP46_PRI46_Msk;
	tmp |= NVIC_NVICIP46_PRI46(data);
	((Nvic *)hw)->NVICIP46.reg = tmp;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP46_PRI46_bf(const void *const hw, hri_nvic_nvicip46_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP46.reg &= ~NVIC_NVICIP46_PRI46(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP46_PRI46_bf(const void *const hw, hri_nvic_nvicip46_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP46.reg ^= NVIC_NVICIP46_PRI46(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip46_reg_t hri_nvic_read_NVICIP46_PRI46_bf(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP46.reg;
	tmp = (tmp & NVIC_NVICIP46_PRI46_Msk) >> 0;
	return tmp;
}

static inline void hri_nvic_set_NVICIP46_reg(const void *const hw, hri_nvic_nvicip46_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP46.reg |= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip46_reg_t hri_nvic_get_NVICIP46_reg(const void *const hw, hri_nvic_nvicip46_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP46.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_nvic_write_NVICIP46_reg(const void *const hw, hri_nvic_nvicip46_reg_t data)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP46.reg = data;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP46_reg(const void *const hw, hri_nvic_nvicip46_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP46.reg &= ~mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP46_reg(const void *const hw, hri_nvic_nvicip46_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP46.reg ^= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip46_reg_t hri_nvic_read_NVICIP46_reg(const void *const hw)
{
	return ((Nvic *)hw)->NVICIP46.reg;
}

static inline void hri_nvic_set_NVICIP47_PRI47_bf(const void *const hw, hri_nvic_nvicip47_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP47.reg |= NVIC_NVICIP47_PRI47(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip47_reg_t hri_nvic_get_NVICIP47_PRI47_bf(const void *const hw, hri_nvic_nvicip47_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP47.reg;
	tmp = (tmp & NVIC_NVICIP47_PRI47(mask)) >> 0;
	return tmp;
}

static inline void hri_nvic_write_NVICIP47_PRI47_bf(const void *const hw, hri_nvic_nvicip47_reg_t data)
{
	uint8_t tmp;
	NVIC_CRITICAL_SECTION_ENTER();
	tmp = ((Nvic *)hw)->NVICIP47.reg;
	tmp &= ~NVIC_NVICIP47_PRI47_Msk;
	tmp |= NVIC_NVICIP47_PRI47(data);
	((Nvic *)hw)->NVICIP47.reg = tmp;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP47_PRI47_bf(const void *const hw, hri_nvic_nvicip47_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP47.reg &= ~NVIC_NVICIP47_PRI47(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP47_PRI47_bf(const void *const hw, hri_nvic_nvicip47_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP47.reg ^= NVIC_NVICIP47_PRI47(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip47_reg_t hri_nvic_read_NVICIP47_PRI47_bf(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP47.reg;
	tmp = (tmp & NVIC_NVICIP47_PRI47_Msk) >> 0;
	return tmp;
}

static inline void hri_nvic_set_NVICIP47_reg(const void *const hw, hri_nvic_nvicip47_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP47.reg |= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip47_reg_t hri_nvic_get_NVICIP47_reg(const void *const hw, hri_nvic_nvicip47_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP47.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_nvic_write_NVICIP47_reg(const void *const hw, hri_nvic_nvicip47_reg_t data)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP47.reg = data;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP47_reg(const void *const hw, hri_nvic_nvicip47_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP47.reg &= ~mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP47_reg(const void *const hw, hri_nvic_nvicip47_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP47.reg ^= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip47_reg_t hri_nvic_read_NVICIP47_reg(const void *const hw)
{
	return ((Nvic *)hw)->NVICIP47.reg;
}

static inline void hri_nvic_set_NVICIP48_PRI48_bf(const void *const hw, hri_nvic_nvicip48_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP48.reg |= NVIC_NVICIP48_PRI48(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip48_reg_t hri_nvic_get_NVICIP48_PRI48_bf(const void *const hw, hri_nvic_nvicip48_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP48.reg;
	tmp = (tmp & NVIC_NVICIP48_PRI48(mask)) >> 0;
	return tmp;
}

static inline void hri_nvic_write_NVICIP48_PRI48_bf(const void *const hw, hri_nvic_nvicip48_reg_t data)
{
	uint8_t tmp;
	NVIC_CRITICAL_SECTION_ENTER();
	tmp = ((Nvic *)hw)->NVICIP48.reg;
	tmp &= ~NVIC_NVICIP48_PRI48_Msk;
	tmp |= NVIC_NVICIP48_PRI48(data);
	((Nvic *)hw)->NVICIP48.reg = tmp;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP48_PRI48_bf(const void *const hw, hri_nvic_nvicip48_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP48.reg &= ~NVIC_NVICIP48_PRI48(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP48_PRI48_bf(const void *const hw, hri_nvic_nvicip48_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP48.reg ^= NVIC_NVICIP48_PRI48(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip48_reg_t hri_nvic_read_NVICIP48_PRI48_bf(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP48.reg;
	tmp = (tmp & NVIC_NVICIP48_PRI48_Msk) >> 0;
	return tmp;
}

static inline void hri_nvic_set_NVICIP48_reg(const void *const hw, hri_nvic_nvicip48_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP48.reg |= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip48_reg_t hri_nvic_get_NVICIP48_reg(const void *const hw, hri_nvic_nvicip48_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP48.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_nvic_write_NVICIP48_reg(const void *const hw, hri_nvic_nvicip48_reg_t data)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP48.reg = data;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP48_reg(const void *const hw, hri_nvic_nvicip48_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP48.reg &= ~mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP48_reg(const void *const hw, hri_nvic_nvicip48_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP48.reg ^= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip48_reg_t hri_nvic_read_NVICIP48_reg(const void *const hw)
{
	return ((Nvic *)hw)->NVICIP48.reg;
}

static inline void hri_nvic_set_NVICIP49_PRI49_bf(const void *const hw, hri_nvic_nvicip49_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP49.reg |= NVIC_NVICIP49_PRI49(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip49_reg_t hri_nvic_get_NVICIP49_PRI49_bf(const void *const hw, hri_nvic_nvicip49_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP49.reg;
	tmp = (tmp & NVIC_NVICIP49_PRI49(mask)) >> 0;
	return tmp;
}

static inline void hri_nvic_write_NVICIP49_PRI49_bf(const void *const hw, hri_nvic_nvicip49_reg_t data)
{
	uint8_t tmp;
	NVIC_CRITICAL_SECTION_ENTER();
	tmp = ((Nvic *)hw)->NVICIP49.reg;
	tmp &= ~NVIC_NVICIP49_PRI49_Msk;
	tmp |= NVIC_NVICIP49_PRI49(data);
	((Nvic *)hw)->NVICIP49.reg = tmp;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP49_PRI49_bf(const void *const hw, hri_nvic_nvicip49_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP49.reg &= ~NVIC_NVICIP49_PRI49(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP49_PRI49_bf(const void *const hw, hri_nvic_nvicip49_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP49.reg ^= NVIC_NVICIP49_PRI49(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip49_reg_t hri_nvic_read_NVICIP49_PRI49_bf(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP49.reg;
	tmp = (tmp & NVIC_NVICIP49_PRI49_Msk) >> 0;
	return tmp;
}

static inline void hri_nvic_set_NVICIP49_reg(const void *const hw, hri_nvic_nvicip49_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP49.reg |= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip49_reg_t hri_nvic_get_NVICIP49_reg(const void *const hw, hri_nvic_nvicip49_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP49.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_nvic_write_NVICIP49_reg(const void *const hw, hri_nvic_nvicip49_reg_t data)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP49.reg = data;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP49_reg(const void *const hw, hri_nvic_nvicip49_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP49.reg &= ~mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP49_reg(const void *const hw, hri_nvic_nvicip49_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP49.reg ^= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip49_reg_t hri_nvic_read_NVICIP49_reg(const void *const hw)
{
	return ((Nvic *)hw)->NVICIP49.reg;
}

static inline void hri_nvic_set_NVICIP50_PRI50_bf(const void *const hw, hri_nvic_nvicip50_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP50.reg |= NVIC_NVICIP50_PRI50(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip50_reg_t hri_nvic_get_NVICIP50_PRI50_bf(const void *const hw, hri_nvic_nvicip50_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP50.reg;
	tmp = (tmp & NVIC_NVICIP50_PRI50(mask)) >> 0;
	return tmp;
}

static inline void hri_nvic_write_NVICIP50_PRI50_bf(const void *const hw, hri_nvic_nvicip50_reg_t data)
{
	uint8_t tmp;
	NVIC_CRITICAL_SECTION_ENTER();
	tmp = ((Nvic *)hw)->NVICIP50.reg;
	tmp &= ~NVIC_NVICIP50_PRI50_Msk;
	tmp |= NVIC_NVICIP50_PRI50(data);
	((Nvic *)hw)->NVICIP50.reg = tmp;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP50_PRI50_bf(const void *const hw, hri_nvic_nvicip50_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP50.reg &= ~NVIC_NVICIP50_PRI50(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP50_PRI50_bf(const void *const hw, hri_nvic_nvicip50_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP50.reg ^= NVIC_NVICIP50_PRI50(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip50_reg_t hri_nvic_read_NVICIP50_PRI50_bf(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP50.reg;
	tmp = (tmp & NVIC_NVICIP50_PRI50_Msk) >> 0;
	return tmp;
}

static inline void hri_nvic_set_NVICIP50_reg(const void *const hw, hri_nvic_nvicip50_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP50.reg |= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip50_reg_t hri_nvic_get_NVICIP50_reg(const void *const hw, hri_nvic_nvicip50_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP50.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_nvic_write_NVICIP50_reg(const void *const hw, hri_nvic_nvicip50_reg_t data)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP50.reg = data;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP50_reg(const void *const hw, hri_nvic_nvicip50_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP50.reg &= ~mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP50_reg(const void *const hw, hri_nvic_nvicip50_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP50.reg ^= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip50_reg_t hri_nvic_read_NVICIP50_reg(const void *const hw)
{
	return ((Nvic *)hw)->NVICIP50.reg;
}

static inline void hri_nvic_set_NVICIP51_PRI51_bf(const void *const hw, hri_nvic_nvicip51_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP51.reg |= NVIC_NVICIP51_PRI51(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip51_reg_t hri_nvic_get_NVICIP51_PRI51_bf(const void *const hw, hri_nvic_nvicip51_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP51.reg;
	tmp = (tmp & NVIC_NVICIP51_PRI51(mask)) >> 0;
	return tmp;
}

static inline void hri_nvic_write_NVICIP51_PRI51_bf(const void *const hw, hri_nvic_nvicip51_reg_t data)
{
	uint8_t tmp;
	NVIC_CRITICAL_SECTION_ENTER();
	tmp = ((Nvic *)hw)->NVICIP51.reg;
	tmp &= ~NVIC_NVICIP51_PRI51_Msk;
	tmp |= NVIC_NVICIP51_PRI51(data);
	((Nvic *)hw)->NVICIP51.reg = tmp;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP51_PRI51_bf(const void *const hw, hri_nvic_nvicip51_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP51.reg &= ~NVIC_NVICIP51_PRI51(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP51_PRI51_bf(const void *const hw, hri_nvic_nvicip51_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP51.reg ^= NVIC_NVICIP51_PRI51(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip51_reg_t hri_nvic_read_NVICIP51_PRI51_bf(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP51.reg;
	tmp = (tmp & NVIC_NVICIP51_PRI51_Msk) >> 0;
	return tmp;
}

static inline void hri_nvic_set_NVICIP51_reg(const void *const hw, hri_nvic_nvicip51_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP51.reg |= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip51_reg_t hri_nvic_get_NVICIP51_reg(const void *const hw, hri_nvic_nvicip51_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP51.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_nvic_write_NVICIP51_reg(const void *const hw, hri_nvic_nvicip51_reg_t data)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP51.reg = data;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP51_reg(const void *const hw, hri_nvic_nvicip51_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP51.reg &= ~mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP51_reg(const void *const hw, hri_nvic_nvicip51_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP51.reg ^= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip51_reg_t hri_nvic_read_NVICIP51_reg(const void *const hw)
{
	return ((Nvic *)hw)->NVICIP51.reg;
}

static inline void hri_nvic_set_NVICIP52_PRI52_bf(const void *const hw, hri_nvic_nvicip52_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP52.reg |= NVIC_NVICIP52_PRI52(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip52_reg_t hri_nvic_get_NVICIP52_PRI52_bf(const void *const hw, hri_nvic_nvicip52_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP52.reg;
	tmp = (tmp & NVIC_NVICIP52_PRI52(mask)) >> 0;
	return tmp;
}

static inline void hri_nvic_write_NVICIP52_PRI52_bf(const void *const hw, hri_nvic_nvicip52_reg_t data)
{
	uint8_t tmp;
	NVIC_CRITICAL_SECTION_ENTER();
	tmp = ((Nvic *)hw)->NVICIP52.reg;
	tmp &= ~NVIC_NVICIP52_PRI52_Msk;
	tmp |= NVIC_NVICIP52_PRI52(data);
	((Nvic *)hw)->NVICIP52.reg = tmp;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP52_PRI52_bf(const void *const hw, hri_nvic_nvicip52_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP52.reg &= ~NVIC_NVICIP52_PRI52(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP52_PRI52_bf(const void *const hw, hri_nvic_nvicip52_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP52.reg ^= NVIC_NVICIP52_PRI52(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip52_reg_t hri_nvic_read_NVICIP52_PRI52_bf(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP52.reg;
	tmp = (tmp & NVIC_NVICIP52_PRI52_Msk) >> 0;
	return tmp;
}

static inline void hri_nvic_set_NVICIP52_reg(const void *const hw, hri_nvic_nvicip52_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP52.reg |= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip52_reg_t hri_nvic_get_NVICIP52_reg(const void *const hw, hri_nvic_nvicip52_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP52.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_nvic_write_NVICIP52_reg(const void *const hw, hri_nvic_nvicip52_reg_t data)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP52.reg = data;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP52_reg(const void *const hw, hri_nvic_nvicip52_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP52.reg &= ~mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP52_reg(const void *const hw, hri_nvic_nvicip52_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP52.reg ^= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip52_reg_t hri_nvic_read_NVICIP52_reg(const void *const hw)
{
	return ((Nvic *)hw)->NVICIP52.reg;
}

static inline void hri_nvic_set_NVICIP53_PRI53_bf(const void *const hw, hri_nvic_nvicip53_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP53.reg |= NVIC_NVICIP53_PRI53(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip53_reg_t hri_nvic_get_NVICIP53_PRI53_bf(const void *const hw, hri_nvic_nvicip53_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP53.reg;
	tmp = (tmp & NVIC_NVICIP53_PRI53(mask)) >> 0;
	return tmp;
}

static inline void hri_nvic_write_NVICIP53_PRI53_bf(const void *const hw, hri_nvic_nvicip53_reg_t data)
{
	uint8_t tmp;
	NVIC_CRITICAL_SECTION_ENTER();
	tmp = ((Nvic *)hw)->NVICIP53.reg;
	tmp &= ~NVIC_NVICIP53_PRI53_Msk;
	tmp |= NVIC_NVICIP53_PRI53(data);
	((Nvic *)hw)->NVICIP53.reg = tmp;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP53_PRI53_bf(const void *const hw, hri_nvic_nvicip53_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP53.reg &= ~NVIC_NVICIP53_PRI53(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP53_PRI53_bf(const void *const hw, hri_nvic_nvicip53_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP53.reg ^= NVIC_NVICIP53_PRI53(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip53_reg_t hri_nvic_read_NVICIP53_PRI53_bf(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP53.reg;
	tmp = (tmp & NVIC_NVICIP53_PRI53_Msk) >> 0;
	return tmp;
}

static inline void hri_nvic_set_NVICIP53_reg(const void *const hw, hri_nvic_nvicip53_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP53.reg |= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip53_reg_t hri_nvic_get_NVICIP53_reg(const void *const hw, hri_nvic_nvicip53_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP53.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_nvic_write_NVICIP53_reg(const void *const hw, hri_nvic_nvicip53_reg_t data)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP53.reg = data;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP53_reg(const void *const hw, hri_nvic_nvicip53_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP53.reg &= ~mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP53_reg(const void *const hw, hri_nvic_nvicip53_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP53.reg ^= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip53_reg_t hri_nvic_read_NVICIP53_reg(const void *const hw)
{
	return ((Nvic *)hw)->NVICIP53.reg;
}

static inline void hri_nvic_set_NVICIP54_PRI54_bf(const void *const hw, hri_nvic_nvicip54_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP54.reg |= NVIC_NVICIP54_PRI54(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip54_reg_t hri_nvic_get_NVICIP54_PRI54_bf(const void *const hw, hri_nvic_nvicip54_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP54.reg;
	tmp = (tmp & NVIC_NVICIP54_PRI54(mask)) >> 0;
	return tmp;
}

static inline void hri_nvic_write_NVICIP54_PRI54_bf(const void *const hw, hri_nvic_nvicip54_reg_t data)
{
	uint8_t tmp;
	NVIC_CRITICAL_SECTION_ENTER();
	tmp = ((Nvic *)hw)->NVICIP54.reg;
	tmp &= ~NVIC_NVICIP54_PRI54_Msk;
	tmp |= NVIC_NVICIP54_PRI54(data);
	((Nvic *)hw)->NVICIP54.reg = tmp;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP54_PRI54_bf(const void *const hw, hri_nvic_nvicip54_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP54.reg &= ~NVIC_NVICIP54_PRI54(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP54_PRI54_bf(const void *const hw, hri_nvic_nvicip54_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP54.reg ^= NVIC_NVICIP54_PRI54(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip54_reg_t hri_nvic_read_NVICIP54_PRI54_bf(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP54.reg;
	tmp = (tmp & NVIC_NVICIP54_PRI54_Msk) >> 0;
	return tmp;
}

static inline void hri_nvic_set_NVICIP54_reg(const void *const hw, hri_nvic_nvicip54_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP54.reg |= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip54_reg_t hri_nvic_get_NVICIP54_reg(const void *const hw, hri_nvic_nvicip54_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP54.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_nvic_write_NVICIP54_reg(const void *const hw, hri_nvic_nvicip54_reg_t data)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP54.reg = data;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP54_reg(const void *const hw, hri_nvic_nvicip54_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP54.reg &= ~mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP54_reg(const void *const hw, hri_nvic_nvicip54_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP54.reg ^= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip54_reg_t hri_nvic_read_NVICIP54_reg(const void *const hw)
{
	return ((Nvic *)hw)->NVICIP54.reg;
}

static inline void hri_nvic_set_NVICIP55_PRI55_bf(const void *const hw, hri_nvic_nvicip55_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP55.reg |= NVIC_NVICIP55_PRI55(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip55_reg_t hri_nvic_get_NVICIP55_PRI55_bf(const void *const hw, hri_nvic_nvicip55_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP55.reg;
	tmp = (tmp & NVIC_NVICIP55_PRI55(mask)) >> 0;
	return tmp;
}

static inline void hri_nvic_write_NVICIP55_PRI55_bf(const void *const hw, hri_nvic_nvicip55_reg_t data)
{
	uint8_t tmp;
	NVIC_CRITICAL_SECTION_ENTER();
	tmp = ((Nvic *)hw)->NVICIP55.reg;
	tmp &= ~NVIC_NVICIP55_PRI55_Msk;
	tmp |= NVIC_NVICIP55_PRI55(data);
	((Nvic *)hw)->NVICIP55.reg = tmp;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP55_PRI55_bf(const void *const hw, hri_nvic_nvicip55_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP55.reg &= ~NVIC_NVICIP55_PRI55(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP55_PRI55_bf(const void *const hw, hri_nvic_nvicip55_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP55.reg ^= NVIC_NVICIP55_PRI55(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip55_reg_t hri_nvic_read_NVICIP55_PRI55_bf(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP55.reg;
	tmp = (tmp & NVIC_NVICIP55_PRI55_Msk) >> 0;
	return tmp;
}

static inline void hri_nvic_set_NVICIP55_reg(const void *const hw, hri_nvic_nvicip55_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP55.reg |= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip55_reg_t hri_nvic_get_NVICIP55_reg(const void *const hw, hri_nvic_nvicip55_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP55.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_nvic_write_NVICIP55_reg(const void *const hw, hri_nvic_nvicip55_reg_t data)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP55.reg = data;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP55_reg(const void *const hw, hri_nvic_nvicip55_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP55.reg &= ~mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP55_reg(const void *const hw, hri_nvic_nvicip55_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP55.reg ^= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip55_reg_t hri_nvic_read_NVICIP55_reg(const void *const hw)
{
	return ((Nvic *)hw)->NVICIP55.reg;
}

static inline void hri_nvic_set_NVICIP56_PRI56_bf(const void *const hw, hri_nvic_nvicip56_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP56.reg |= NVIC_NVICIP56_PRI56(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip56_reg_t hri_nvic_get_NVICIP56_PRI56_bf(const void *const hw, hri_nvic_nvicip56_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP56.reg;
	tmp = (tmp & NVIC_NVICIP56_PRI56(mask)) >> 0;
	return tmp;
}

static inline void hri_nvic_write_NVICIP56_PRI56_bf(const void *const hw, hri_nvic_nvicip56_reg_t data)
{
	uint8_t tmp;
	NVIC_CRITICAL_SECTION_ENTER();
	tmp = ((Nvic *)hw)->NVICIP56.reg;
	tmp &= ~NVIC_NVICIP56_PRI56_Msk;
	tmp |= NVIC_NVICIP56_PRI56(data);
	((Nvic *)hw)->NVICIP56.reg = tmp;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP56_PRI56_bf(const void *const hw, hri_nvic_nvicip56_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP56.reg &= ~NVIC_NVICIP56_PRI56(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP56_PRI56_bf(const void *const hw, hri_nvic_nvicip56_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP56.reg ^= NVIC_NVICIP56_PRI56(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip56_reg_t hri_nvic_read_NVICIP56_PRI56_bf(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP56.reg;
	tmp = (tmp & NVIC_NVICIP56_PRI56_Msk) >> 0;
	return tmp;
}

static inline void hri_nvic_set_NVICIP56_reg(const void *const hw, hri_nvic_nvicip56_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP56.reg |= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip56_reg_t hri_nvic_get_NVICIP56_reg(const void *const hw, hri_nvic_nvicip56_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP56.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_nvic_write_NVICIP56_reg(const void *const hw, hri_nvic_nvicip56_reg_t data)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP56.reg = data;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP56_reg(const void *const hw, hri_nvic_nvicip56_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP56.reg &= ~mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP56_reg(const void *const hw, hri_nvic_nvicip56_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP56.reg ^= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip56_reg_t hri_nvic_read_NVICIP56_reg(const void *const hw)
{
	return ((Nvic *)hw)->NVICIP56.reg;
}

static inline void hri_nvic_set_NVICIP57_PRI57_bf(const void *const hw, hri_nvic_nvicip57_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP57.reg |= NVIC_NVICIP57_PRI57(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip57_reg_t hri_nvic_get_NVICIP57_PRI57_bf(const void *const hw, hri_nvic_nvicip57_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP57.reg;
	tmp = (tmp & NVIC_NVICIP57_PRI57(mask)) >> 0;
	return tmp;
}

static inline void hri_nvic_write_NVICIP57_PRI57_bf(const void *const hw, hri_nvic_nvicip57_reg_t data)
{
	uint8_t tmp;
	NVIC_CRITICAL_SECTION_ENTER();
	tmp = ((Nvic *)hw)->NVICIP57.reg;
	tmp &= ~NVIC_NVICIP57_PRI57_Msk;
	tmp |= NVIC_NVICIP57_PRI57(data);
	((Nvic *)hw)->NVICIP57.reg = tmp;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP57_PRI57_bf(const void *const hw, hri_nvic_nvicip57_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP57.reg &= ~NVIC_NVICIP57_PRI57(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP57_PRI57_bf(const void *const hw, hri_nvic_nvicip57_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP57.reg ^= NVIC_NVICIP57_PRI57(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip57_reg_t hri_nvic_read_NVICIP57_PRI57_bf(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP57.reg;
	tmp = (tmp & NVIC_NVICIP57_PRI57_Msk) >> 0;
	return tmp;
}

static inline void hri_nvic_set_NVICIP57_reg(const void *const hw, hri_nvic_nvicip57_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP57.reg |= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip57_reg_t hri_nvic_get_NVICIP57_reg(const void *const hw, hri_nvic_nvicip57_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP57.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_nvic_write_NVICIP57_reg(const void *const hw, hri_nvic_nvicip57_reg_t data)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP57.reg = data;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP57_reg(const void *const hw, hri_nvic_nvicip57_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP57.reg &= ~mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP57_reg(const void *const hw, hri_nvic_nvicip57_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP57.reg ^= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip57_reg_t hri_nvic_read_NVICIP57_reg(const void *const hw)
{
	return ((Nvic *)hw)->NVICIP57.reg;
}

static inline void hri_nvic_set_NVICIP58_PRI58_bf(const void *const hw, hri_nvic_nvicip58_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP58.reg |= NVIC_NVICIP58_PRI58(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip58_reg_t hri_nvic_get_NVICIP58_PRI58_bf(const void *const hw, hri_nvic_nvicip58_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP58.reg;
	tmp = (tmp & NVIC_NVICIP58_PRI58(mask)) >> 0;
	return tmp;
}

static inline void hri_nvic_write_NVICIP58_PRI58_bf(const void *const hw, hri_nvic_nvicip58_reg_t data)
{
	uint8_t tmp;
	NVIC_CRITICAL_SECTION_ENTER();
	tmp = ((Nvic *)hw)->NVICIP58.reg;
	tmp &= ~NVIC_NVICIP58_PRI58_Msk;
	tmp |= NVIC_NVICIP58_PRI58(data);
	((Nvic *)hw)->NVICIP58.reg = tmp;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP58_PRI58_bf(const void *const hw, hri_nvic_nvicip58_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP58.reg &= ~NVIC_NVICIP58_PRI58(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP58_PRI58_bf(const void *const hw, hri_nvic_nvicip58_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP58.reg ^= NVIC_NVICIP58_PRI58(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip58_reg_t hri_nvic_read_NVICIP58_PRI58_bf(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP58.reg;
	tmp = (tmp & NVIC_NVICIP58_PRI58_Msk) >> 0;
	return tmp;
}

static inline void hri_nvic_set_NVICIP58_reg(const void *const hw, hri_nvic_nvicip58_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP58.reg |= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip58_reg_t hri_nvic_get_NVICIP58_reg(const void *const hw, hri_nvic_nvicip58_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP58.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_nvic_write_NVICIP58_reg(const void *const hw, hri_nvic_nvicip58_reg_t data)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP58.reg = data;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP58_reg(const void *const hw, hri_nvic_nvicip58_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP58.reg &= ~mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP58_reg(const void *const hw, hri_nvic_nvicip58_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP58.reg ^= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip58_reg_t hri_nvic_read_NVICIP58_reg(const void *const hw)
{
	return ((Nvic *)hw)->NVICIP58.reg;
}

static inline void hri_nvic_set_NVICIP59_PRI59_bf(const void *const hw, hri_nvic_nvicip59_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP59.reg |= NVIC_NVICIP59_PRI59(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip59_reg_t hri_nvic_get_NVICIP59_PRI59_bf(const void *const hw, hri_nvic_nvicip59_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP59.reg;
	tmp = (tmp & NVIC_NVICIP59_PRI59(mask)) >> 0;
	return tmp;
}

static inline void hri_nvic_write_NVICIP59_PRI59_bf(const void *const hw, hri_nvic_nvicip59_reg_t data)
{
	uint8_t tmp;
	NVIC_CRITICAL_SECTION_ENTER();
	tmp = ((Nvic *)hw)->NVICIP59.reg;
	tmp &= ~NVIC_NVICIP59_PRI59_Msk;
	tmp |= NVIC_NVICIP59_PRI59(data);
	((Nvic *)hw)->NVICIP59.reg = tmp;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP59_PRI59_bf(const void *const hw, hri_nvic_nvicip59_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP59.reg &= ~NVIC_NVICIP59_PRI59(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP59_PRI59_bf(const void *const hw, hri_nvic_nvicip59_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP59.reg ^= NVIC_NVICIP59_PRI59(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip59_reg_t hri_nvic_read_NVICIP59_PRI59_bf(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP59.reg;
	tmp = (tmp & NVIC_NVICIP59_PRI59_Msk) >> 0;
	return tmp;
}

static inline void hri_nvic_set_NVICIP59_reg(const void *const hw, hri_nvic_nvicip59_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP59.reg |= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip59_reg_t hri_nvic_get_NVICIP59_reg(const void *const hw, hri_nvic_nvicip59_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP59.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_nvic_write_NVICIP59_reg(const void *const hw, hri_nvic_nvicip59_reg_t data)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP59.reg = data;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP59_reg(const void *const hw, hri_nvic_nvicip59_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP59.reg &= ~mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP59_reg(const void *const hw, hri_nvic_nvicip59_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP59.reg ^= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip59_reg_t hri_nvic_read_NVICIP59_reg(const void *const hw)
{
	return ((Nvic *)hw)->NVICIP59.reg;
}

static inline void hri_nvic_set_NVICIP60_PRI60_bf(const void *const hw, hri_nvic_nvicip60_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP60.reg |= NVIC_NVICIP60_PRI60(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip60_reg_t hri_nvic_get_NVICIP60_PRI60_bf(const void *const hw, hri_nvic_nvicip60_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP60.reg;
	tmp = (tmp & NVIC_NVICIP60_PRI60(mask)) >> 0;
	return tmp;
}

static inline void hri_nvic_write_NVICIP60_PRI60_bf(const void *const hw, hri_nvic_nvicip60_reg_t data)
{
	uint8_t tmp;
	NVIC_CRITICAL_SECTION_ENTER();
	tmp = ((Nvic *)hw)->NVICIP60.reg;
	tmp &= ~NVIC_NVICIP60_PRI60_Msk;
	tmp |= NVIC_NVICIP60_PRI60(data);
	((Nvic *)hw)->NVICIP60.reg = tmp;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP60_PRI60_bf(const void *const hw, hri_nvic_nvicip60_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP60.reg &= ~NVIC_NVICIP60_PRI60(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP60_PRI60_bf(const void *const hw, hri_nvic_nvicip60_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP60.reg ^= NVIC_NVICIP60_PRI60(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip60_reg_t hri_nvic_read_NVICIP60_PRI60_bf(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP60.reg;
	tmp = (tmp & NVIC_NVICIP60_PRI60_Msk) >> 0;
	return tmp;
}

static inline void hri_nvic_set_NVICIP60_reg(const void *const hw, hri_nvic_nvicip60_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP60.reg |= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip60_reg_t hri_nvic_get_NVICIP60_reg(const void *const hw, hri_nvic_nvicip60_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP60.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_nvic_write_NVICIP60_reg(const void *const hw, hri_nvic_nvicip60_reg_t data)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP60.reg = data;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP60_reg(const void *const hw, hri_nvic_nvicip60_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP60.reg &= ~mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP60_reg(const void *const hw, hri_nvic_nvicip60_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP60.reg ^= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip60_reg_t hri_nvic_read_NVICIP60_reg(const void *const hw)
{
	return ((Nvic *)hw)->NVICIP60.reg;
}

static inline void hri_nvic_set_NVICIP61_PRI61_bf(const void *const hw, hri_nvic_nvicip61_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP61.reg |= NVIC_NVICIP61_PRI61(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip61_reg_t hri_nvic_get_NVICIP61_PRI61_bf(const void *const hw, hri_nvic_nvicip61_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP61.reg;
	tmp = (tmp & NVIC_NVICIP61_PRI61(mask)) >> 0;
	return tmp;
}

static inline void hri_nvic_write_NVICIP61_PRI61_bf(const void *const hw, hri_nvic_nvicip61_reg_t data)
{
	uint8_t tmp;
	NVIC_CRITICAL_SECTION_ENTER();
	tmp = ((Nvic *)hw)->NVICIP61.reg;
	tmp &= ~NVIC_NVICIP61_PRI61_Msk;
	tmp |= NVIC_NVICIP61_PRI61(data);
	((Nvic *)hw)->NVICIP61.reg = tmp;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP61_PRI61_bf(const void *const hw, hri_nvic_nvicip61_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP61.reg &= ~NVIC_NVICIP61_PRI61(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP61_PRI61_bf(const void *const hw, hri_nvic_nvicip61_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP61.reg ^= NVIC_NVICIP61_PRI61(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip61_reg_t hri_nvic_read_NVICIP61_PRI61_bf(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP61.reg;
	tmp = (tmp & NVIC_NVICIP61_PRI61_Msk) >> 0;
	return tmp;
}

static inline void hri_nvic_set_NVICIP61_reg(const void *const hw, hri_nvic_nvicip61_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP61.reg |= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip61_reg_t hri_nvic_get_NVICIP61_reg(const void *const hw, hri_nvic_nvicip61_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP61.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_nvic_write_NVICIP61_reg(const void *const hw, hri_nvic_nvicip61_reg_t data)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP61.reg = data;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP61_reg(const void *const hw, hri_nvic_nvicip61_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP61.reg &= ~mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP61_reg(const void *const hw, hri_nvic_nvicip61_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP61.reg ^= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip61_reg_t hri_nvic_read_NVICIP61_reg(const void *const hw)
{
	return ((Nvic *)hw)->NVICIP61.reg;
}

static inline void hri_nvic_set_NVICIP62_PRI62_bf(const void *const hw, hri_nvic_nvicip62_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP62.reg |= NVIC_NVICIP62_PRI62(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip62_reg_t hri_nvic_get_NVICIP62_PRI62_bf(const void *const hw, hri_nvic_nvicip62_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP62.reg;
	tmp = (tmp & NVIC_NVICIP62_PRI62(mask)) >> 0;
	return tmp;
}

static inline void hri_nvic_write_NVICIP62_PRI62_bf(const void *const hw, hri_nvic_nvicip62_reg_t data)
{
	uint8_t tmp;
	NVIC_CRITICAL_SECTION_ENTER();
	tmp = ((Nvic *)hw)->NVICIP62.reg;
	tmp &= ~NVIC_NVICIP62_PRI62_Msk;
	tmp |= NVIC_NVICIP62_PRI62(data);
	((Nvic *)hw)->NVICIP62.reg = tmp;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP62_PRI62_bf(const void *const hw, hri_nvic_nvicip62_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP62.reg &= ~NVIC_NVICIP62_PRI62(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP62_PRI62_bf(const void *const hw, hri_nvic_nvicip62_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP62.reg ^= NVIC_NVICIP62_PRI62(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip62_reg_t hri_nvic_read_NVICIP62_PRI62_bf(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP62.reg;
	tmp = (tmp & NVIC_NVICIP62_PRI62_Msk) >> 0;
	return tmp;
}

static inline void hri_nvic_set_NVICIP62_reg(const void *const hw, hri_nvic_nvicip62_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP62.reg |= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip62_reg_t hri_nvic_get_NVICIP62_reg(const void *const hw, hri_nvic_nvicip62_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP62.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_nvic_write_NVICIP62_reg(const void *const hw, hri_nvic_nvicip62_reg_t data)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP62.reg = data;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP62_reg(const void *const hw, hri_nvic_nvicip62_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP62.reg &= ~mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP62_reg(const void *const hw, hri_nvic_nvicip62_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP62.reg ^= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip62_reg_t hri_nvic_read_NVICIP62_reg(const void *const hw)
{
	return ((Nvic *)hw)->NVICIP62.reg;
}

static inline void hri_nvic_set_NVICIP63_PRI63_bf(const void *const hw, hri_nvic_nvicip63_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP63.reg |= NVIC_NVICIP63_PRI63(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip63_reg_t hri_nvic_get_NVICIP63_PRI63_bf(const void *const hw, hri_nvic_nvicip63_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP63.reg;
	tmp = (tmp & NVIC_NVICIP63_PRI63(mask)) >> 0;
	return tmp;
}

static inline void hri_nvic_write_NVICIP63_PRI63_bf(const void *const hw, hri_nvic_nvicip63_reg_t data)
{
	uint8_t tmp;
	NVIC_CRITICAL_SECTION_ENTER();
	tmp = ((Nvic *)hw)->NVICIP63.reg;
	tmp &= ~NVIC_NVICIP63_PRI63_Msk;
	tmp |= NVIC_NVICIP63_PRI63(data);
	((Nvic *)hw)->NVICIP63.reg = tmp;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP63_PRI63_bf(const void *const hw, hri_nvic_nvicip63_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP63.reg &= ~NVIC_NVICIP63_PRI63(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP63_PRI63_bf(const void *const hw, hri_nvic_nvicip63_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP63.reg ^= NVIC_NVICIP63_PRI63(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip63_reg_t hri_nvic_read_NVICIP63_PRI63_bf(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP63.reg;
	tmp = (tmp & NVIC_NVICIP63_PRI63_Msk) >> 0;
	return tmp;
}

static inline void hri_nvic_set_NVICIP63_reg(const void *const hw, hri_nvic_nvicip63_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP63.reg |= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip63_reg_t hri_nvic_get_NVICIP63_reg(const void *const hw, hri_nvic_nvicip63_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP63.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_nvic_write_NVICIP63_reg(const void *const hw, hri_nvic_nvicip63_reg_t data)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP63.reg = data;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP63_reg(const void *const hw, hri_nvic_nvicip63_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP63.reg &= ~mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP63_reg(const void *const hw, hri_nvic_nvicip63_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP63.reg ^= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip63_reg_t hri_nvic_read_NVICIP63_reg(const void *const hw)
{
	return ((Nvic *)hw)->NVICIP63.reg;
}

static inline void hri_nvic_set_NVICIP64_PRI64_bf(const void *const hw, hri_nvic_nvicip64_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP64.reg |= NVIC_NVICIP64_PRI64(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip64_reg_t hri_nvic_get_NVICIP64_PRI64_bf(const void *const hw, hri_nvic_nvicip64_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP64.reg;
	tmp = (tmp & NVIC_NVICIP64_PRI64(mask)) >> 0;
	return tmp;
}

static inline void hri_nvic_write_NVICIP64_PRI64_bf(const void *const hw, hri_nvic_nvicip64_reg_t data)
{
	uint8_t tmp;
	NVIC_CRITICAL_SECTION_ENTER();
	tmp = ((Nvic *)hw)->NVICIP64.reg;
	tmp &= ~NVIC_NVICIP64_PRI64_Msk;
	tmp |= NVIC_NVICIP64_PRI64(data);
	((Nvic *)hw)->NVICIP64.reg = tmp;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP64_PRI64_bf(const void *const hw, hri_nvic_nvicip64_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP64.reg &= ~NVIC_NVICIP64_PRI64(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP64_PRI64_bf(const void *const hw, hri_nvic_nvicip64_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP64.reg ^= NVIC_NVICIP64_PRI64(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip64_reg_t hri_nvic_read_NVICIP64_PRI64_bf(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP64.reg;
	tmp = (tmp & NVIC_NVICIP64_PRI64_Msk) >> 0;
	return tmp;
}

static inline void hri_nvic_set_NVICIP64_reg(const void *const hw, hri_nvic_nvicip64_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP64.reg |= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip64_reg_t hri_nvic_get_NVICIP64_reg(const void *const hw, hri_nvic_nvicip64_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP64.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_nvic_write_NVICIP64_reg(const void *const hw, hri_nvic_nvicip64_reg_t data)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP64.reg = data;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP64_reg(const void *const hw, hri_nvic_nvicip64_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP64.reg &= ~mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP64_reg(const void *const hw, hri_nvic_nvicip64_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP64.reg ^= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip64_reg_t hri_nvic_read_NVICIP64_reg(const void *const hw)
{
	return ((Nvic *)hw)->NVICIP64.reg;
}

static inline void hri_nvic_set_NVICIP65_PRI65_bf(const void *const hw, hri_nvic_nvicip65_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP65.reg |= NVIC_NVICIP65_PRI65(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip65_reg_t hri_nvic_get_NVICIP65_PRI65_bf(const void *const hw, hri_nvic_nvicip65_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP65.reg;
	tmp = (tmp & NVIC_NVICIP65_PRI65(mask)) >> 0;
	return tmp;
}

static inline void hri_nvic_write_NVICIP65_PRI65_bf(const void *const hw, hri_nvic_nvicip65_reg_t data)
{
	uint8_t tmp;
	NVIC_CRITICAL_SECTION_ENTER();
	tmp = ((Nvic *)hw)->NVICIP65.reg;
	tmp &= ~NVIC_NVICIP65_PRI65_Msk;
	tmp |= NVIC_NVICIP65_PRI65(data);
	((Nvic *)hw)->NVICIP65.reg = tmp;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP65_PRI65_bf(const void *const hw, hri_nvic_nvicip65_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP65.reg &= ~NVIC_NVICIP65_PRI65(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP65_PRI65_bf(const void *const hw, hri_nvic_nvicip65_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP65.reg ^= NVIC_NVICIP65_PRI65(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip65_reg_t hri_nvic_read_NVICIP65_PRI65_bf(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP65.reg;
	tmp = (tmp & NVIC_NVICIP65_PRI65_Msk) >> 0;
	return tmp;
}

static inline void hri_nvic_set_NVICIP65_reg(const void *const hw, hri_nvic_nvicip65_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP65.reg |= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip65_reg_t hri_nvic_get_NVICIP65_reg(const void *const hw, hri_nvic_nvicip65_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP65.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_nvic_write_NVICIP65_reg(const void *const hw, hri_nvic_nvicip65_reg_t data)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP65.reg = data;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP65_reg(const void *const hw, hri_nvic_nvicip65_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP65.reg &= ~mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP65_reg(const void *const hw, hri_nvic_nvicip65_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP65.reg ^= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip65_reg_t hri_nvic_read_NVICIP65_reg(const void *const hw)
{
	return ((Nvic *)hw)->NVICIP65.reg;
}

static inline void hri_nvic_set_NVICIP66_PRI66_bf(const void *const hw, hri_nvic_nvicip66_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP66.reg |= NVIC_NVICIP66_PRI66(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip66_reg_t hri_nvic_get_NVICIP66_PRI66_bf(const void *const hw, hri_nvic_nvicip66_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP66.reg;
	tmp = (tmp & NVIC_NVICIP66_PRI66(mask)) >> 0;
	return tmp;
}

static inline void hri_nvic_write_NVICIP66_PRI66_bf(const void *const hw, hri_nvic_nvicip66_reg_t data)
{
	uint8_t tmp;
	NVIC_CRITICAL_SECTION_ENTER();
	tmp = ((Nvic *)hw)->NVICIP66.reg;
	tmp &= ~NVIC_NVICIP66_PRI66_Msk;
	tmp |= NVIC_NVICIP66_PRI66(data);
	((Nvic *)hw)->NVICIP66.reg = tmp;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP66_PRI66_bf(const void *const hw, hri_nvic_nvicip66_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP66.reg &= ~NVIC_NVICIP66_PRI66(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP66_PRI66_bf(const void *const hw, hri_nvic_nvicip66_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP66.reg ^= NVIC_NVICIP66_PRI66(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip66_reg_t hri_nvic_read_NVICIP66_PRI66_bf(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP66.reg;
	tmp = (tmp & NVIC_NVICIP66_PRI66_Msk) >> 0;
	return tmp;
}

static inline void hri_nvic_set_NVICIP66_reg(const void *const hw, hri_nvic_nvicip66_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP66.reg |= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip66_reg_t hri_nvic_get_NVICIP66_reg(const void *const hw, hri_nvic_nvicip66_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP66.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_nvic_write_NVICIP66_reg(const void *const hw, hri_nvic_nvicip66_reg_t data)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP66.reg = data;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP66_reg(const void *const hw, hri_nvic_nvicip66_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP66.reg &= ~mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP66_reg(const void *const hw, hri_nvic_nvicip66_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP66.reg ^= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip66_reg_t hri_nvic_read_NVICIP66_reg(const void *const hw)
{
	return ((Nvic *)hw)->NVICIP66.reg;
}

static inline void hri_nvic_set_NVICIP67_PRI67_bf(const void *const hw, hri_nvic_nvicip67_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP67.reg |= NVIC_NVICIP67_PRI67(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip67_reg_t hri_nvic_get_NVICIP67_PRI67_bf(const void *const hw, hri_nvic_nvicip67_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP67.reg;
	tmp = (tmp & NVIC_NVICIP67_PRI67(mask)) >> 0;
	return tmp;
}

static inline void hri_nvic_write_NVICIP67_PRI67_bf(const void *const hw, hri_nvic_nvicip67_reg_t data)
{
	uint8_t tmp;
	NVIC_CRITICAL_SECTION_ENTER();
	tmp = ((Nvic *)hw)->NVICIP67.reg;
	tmp &= ~NVIC_NVICIP67_PRI67_Msk;
	tmp |= NVIC_NVICIP67_PRI67(data);
	((Nvic *)hw)->NVICIP67.reg = tmp;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP67_PRI67_bf(const void *const hw, hri_nvic_nvicip67_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP67.reg &= ~NVIC_NVICIP67_PRI67(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP67_PRI67_bf(const void *const hw, hri_nvic_nvicip67_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP67.reg ^= NVIC_NVICIP67_PRI67(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip67_reg_t hri_nvic_read_NVICIP67_PRI67_bf(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP67.reg;
	tmp = (tmp & NVIC_NVICIP67_PRI67_Msk) >> 0;
	return tmp;
}

static inline void hri_nvic_set_NVICIP67_reg(const void *const hw, hri_nvic_nvicip67_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP67.reg |= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip67_reg_t hri_nvic_get_NVICIP67_reg(const void *const hw, hri_nvic_nvicip67_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP67.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_nvic_write_NVICIP67_reg(const void *const hw, hri_nvic_nvicip67_reg_t data)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP67.reg = data;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP67_reg(const void *const hw, hri_nvic_nvicip67_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP67.reg &= ~mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP67_reg(const void *const hw, hri_nvic_nvicip67_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP67.reg ^= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip67_reg_t hri_nvic_read_NVICIP67_reg(const void *const hw)
{
	return ((Nvic *)hw)->NVICIP67.reg;
}

static inline void hri_nvic_set_NVICIP68_PRI68_bf(const void *const hw, hri_nvic_nvicip68_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP68.reg |= NVIC_NVICIP68_PRI68(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip68_reg_t hri_nvic_get_NVICIP68_PRI68_bf(const void *const hw, hri_nvic_nvicip68_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP68.reg;
	tmp = (tmp & NVIC_NVICIP68_PRI68(mask)) >> 0;
	return tmp;
}

static inline void hri_nvic_write_NVICIP68_PRI68_bf(const void *const hw, hri_nvic_nvicip68_reg_t data)
{
	uint8_t tmp;
	NVIC_CRITICAL_SECTION_ENTER();
	tmp = ((Nvic *)hw)->NVICIP68.reg;
	tmp &= ~NVIC_NVICIP68_PRI68_Msk;
	tmp |= NVIC_NVICIP68_PRI68(data);
	((Nvic *)hw)->NVICIP68.reg = tmp;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP68_PRI68_bf(const void *const hw, hri_nvic_nvicip68_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP68.reg &= ~NVIC_NVICIP68_PRI68(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP68_PRI68_bf(const void *const hw, hri_nvic_nvicip68_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP68.reg ^= NVIC_NVICIP68_PRI68(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip68_reg_t hri_nvic_read_NVICIP68_PRI68_bf(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP68.reg;
	tmp = (tmp & NVIC_NVICIP68_PRI68_Msk) >> 0;
	return tmp;
}

static inline void hri_nvic_set_NVICIP68_reg(const void *const hw, hri_nvic_nvicip68_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP68.reg |= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip68_reg_t hri_nvic_get_NVICIP68_reg(const void *const hw, hri_nvic_nvicip68_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP68.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_nvic_write_NVICIP68_reg(const void *const hw, hri_nvic_nvicip68_reg_t data)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP68.reg = data;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP68_reg(const void *const hw, hri_nvic_nvicip68_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP68.reg &= ~mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP68_reg(const void *const hw, hri_nvic_nvicip68_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP68.reg ^= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip68_reg_t hri_nvic_read_NVICIP68_reg(const void *const hw)
{
	return ((Nvic *)hw)->NVICIP68.reg;
}

static inline void hri_nvic_set_NVICIP69_PRI69_bf(const void *const hw, hri_nvic_nvicip69_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP69.reg |= NVIC_NVICIP69_PRI69(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip69_reg_t hri_nvic_get_NVICIP69_PRI69_bf(const void *const hw, hri_nvic_nvicip69_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP69.reg;
	tmp = (tmp & NVIC_NVICIP69_PRI69(mask)) >> 0;
	return tmp;
}

static inline void hri_nvic_write_NVICIP69_PRI69_bf(const void *const hw, hri_nvic_nvicip69_reg_t data)
{
	uint8_t tmp;
	NVIC_CRITICAL_SECTION_ENTER();
	tmp = ((Nvic *)hw)->NVICIP69.reg;
	tmp &= ~NVIC_NVICIP69_PRI69_Msk;
	tmp |= NVIC_NVICIP69_PRI69(data);
	((Nvic *)hw)->NVICIP69.reg = tmp;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP69_PRI69_bf(const void *const hw, hri_nvic_nvicip69_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP69.reg &= ~NVIC_NVICIP69_PRI69(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP69_PRI69_bf(const void *const hw, hri_nvic_nvicip69_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP69.reg ^= NVIC_NVICIP69_PRI69(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip69_reg_t hri_nvic_read_NVICIP69_PRI69_bf(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP69.reg;
	tmp = (tmp & NVIC_NVICIP69_PRI69_Msk) >> 0;
	return tmp;
}

static inline void hri_nvic_set_NVICIP69_reg(const void *const hw, hri_nvic_nvicip69_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP69.reg |= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip69_reg_t hri_nvic_get_NVICIP69_reg(const void *const hw, hri_nvic_nvicip69_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP69.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_nvic_write_NVICIP69_reg(const void *const hw, hri_nvic_nvicip69_reg_t data)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP69.reg = data;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP69_reg(const void *const hw, hri_nvic_nvicip69_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP69.reg &= ~mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP69_reg(const void *const hw, hri_nvic_nvicip69_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP69.reg ^= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip69_reg_t hri_nvic_read_NVICIP69_reg(const void *const hw)
{
	return ((Nvic *)hw)->NVICIP69.reg;
}

static inline void hri_nvic_set_NVICIP70_PRI70_bf(const void *const hw, hri_nvic_nvicip70_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP70.reg |= NVIC_NVICIP70_PRI70(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip70_reg_t hri_nvic_get_NVICIP70_PRI70_bf(const void *const hw, hri_nvic_nvicip70_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP70.reg;
	tmp = (tmp & NVIC_NVICIP70_PRI70(mask)) >> 0;
	return tmp;
}

static inline void hri_nvic_write_NVICIP70_PRI70_bf(const void *const hw, hri_nvic_nvicip70_reg_t data)
{
	uint8_t tmp;
	NVIC_CRITICAL_SECTION_ENTER();
	tmp = ((Nvic *)hw)->NVICIP70.reg;
	tmp &= ~NVIC_NVICIP70_PRI70_Msk;
	tmp |= NVIC_NVICIP70_PRI70(data);
	((Nvic *)hw)->NVICIP70.reg = tmp;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP70_PRI70_bf(const void *const hw, hri_nvic_nvicip70_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP70.reg &= ~NVIC_NVICIP70_PRI70(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP70_PRI70_bf(const void *const hw, hri_nvic_nvicip70_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP70.reg ^= NVIC_NVICIP70_PRI70(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip70_reg_t hri_nvic_read_NVICIP70_PRI70_bf(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP70.reg;
	tmp = (tmp & NVIC_NVICIP70_PRI70_Msk) >> 0;
	return tmp;
}

static inline void hri_nvic_set_NVICIP70_reg(const void *const hw, hri_nvic_nvicip70_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP70.reg |= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip70_reg_t hri_nvic_get_NVICIP70_reg(const void *const hw, hri_nvic_nvicip70_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP70.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_nvic_write_NVICIP70_reg(const void *const hw, hri_nvic_nvicip70_reg_t data)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP70.reg = data;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP70_reg(const void *const hw, hri_nvic_nvicip70_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP70.reg &= ~mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP70_reg(const void *const hw, hri_nvic_nvicip70_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP70.reg ^= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip70_reg_t hri_nvic_read_NVICIP70_reg(const void *const hw)
{
	return ((Nvic *)hw)->NVICIP70.reg;
}

static inline void hri_nvic_set_NVICIP71_PRI71_bf(const void *const hw, hri_nvic_nvicip71_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP71.reg |= NVIC_NVICIP71_PRI71(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip71_reg_t hri_nvic_get_NVICIP71_PRI71_bf(const void *const hw, hri_nvic_nvicip71_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP71.reg;
	tmp = (tmp & NVIC_NVICIP71_PRI71(mask)) >> 0;
	return tmp;
}

static inline void hri_nvic_write_NVICIP71_PRI71_bf(const void *const hw, hri_nvic_nvicip71_reg_t data)
{
	uint8_t tmp;
	NVIC_CRITICAL_SECTION_ENTER();
	tmp = ((Nvic *)hw)->NVICIP71.reg;
	tmp &= ~NVIC_NVICIP71_PRI71_Msk;
	tmp |= NVIC_NVICIP71_PRI71(data);
	((Nvic *)hw)->NVICIP71.reg = tmp;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP71_PRI71_bf(const void *const hw, hri_nvic_nvicip71_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP71.reg &= ~NVIC_NVICIP71_PRI71(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP71_PRI71_bf(const void *const hw, hri_nvic_nvicip71_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP71.reg ^= NVIC_NVICIP71_PRI71(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip71_reg_t hri_nvic_read_NVICIP71_PRI71_bf(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP71.reg;
	tmp = (tmp & NVIC_NVICIP71_PRI71_Msk) >> 0;
	return tmp;
}

static inline void hri_nvic_set_NVICIP71_reg(const void *const hw, hri_nvic_nvicip71_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP71.reg |= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip71_reg_t hri_nvic_get_NVICIP71_reg(const void *const hw, hri_nvic_nvicip71_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP71.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_nvic_write_NVICIP71_reg(const void *const hw, hri_nvic_nvicip71_reg_t data)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP71.reg = data;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP71_reg(const void *const hw, hri_nvic_nvicip71_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP71.reg &= ~mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP71_reg(const void *const hw, hri_nvic_nvicip71_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP71.reg ^= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip71_reg_t hri_nvic_read_NVICIP71_reg(const void *const hw)
{
	return ((Nvic *)hw)->NVICIP71.reg;
}

static inline void hri_nvic_set_NVICIP72_PRI72_bf(const void *const hw, hri_nvic_nvicip72_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP72.reg |= NVIC_NVICIP72_PRI72(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip72_reg_t hri_nvic_get_NVICIP72_PRI72_bf(const void *const hw, hri_nvic_nvicip72_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP72.reg;
	tmp = (tmp & NVIC_NVICIP72_PRI72(mask)) >> 0;
	return tmp;
}

static inline void hri_nvic_write_NVICIP72_PRI72_bf(const void *const hw, hri_nvic_nvicip72_reg_t data)
{
	uint8_t tmp;
	NVIC_CRITICAL_SECTION_ENTER();
	tmp = ((Nvic *)hw)->NVICIP72.reg;
	tmp &= ~NVIC_NVICIP72_PRI72_Msk;
	tmp |= NVIC_NVICIP72_PRI72(data);
	((Nvic *)hw)->NVICIP72.reg = tmp;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP72_PRI72_bf(const void *const hw, hri_nvic_nvicip72_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP72.reg &= ~NVIC_NVICIP72_PRI72(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP72_PRI72_bf(const void *const hw, hri_nvic_nvicip72_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP72.reg ^= NVIC_NVICIP72_PRI72(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip72_reg_t hri_nvic_read_NVICIP72_PRI72_bf(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP72.reg;
	tmp = (tmp & NVIC_NVICIP72_PRI72_Msk) >> 0;
	return tmp;
}

static inline void hri_nvic_set_NVICIP72_reg(const void *const hw, hri_nvic_nvicip72_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP72.reg |= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip72_reg_t hri_nvic_get_NVICIP72_reg(const void *const hw, hri_nvic_nvicip72_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP72.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_nvic_write_NVICIP72_reg(const void *const hw, hri_nvic_nvicip72_reg_t data)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP72.reg = data;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP72_reg(const void *const hw, hri_nvic_nvicip72_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP72.reg &= ~mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP72_reg(const void *const hw, hri_nvic_nvicip72_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP72.reg ^= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip72_reg_t hri_nvic_read_NVICIP72_reg(const void *const hw)
{
	return ((Nvic *)hw)->NVICIP72.reg;
}

static inline void hri_nvic_set_NVICIP73_PRI73_bf(const void *const hw, hri_nvic_nvicip73_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP73.reg |= NVIC_NVICIP73_PRI73(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip73_reg_t hri_nvic_get_NVICIP73_PRI73_bf(const void *const hw, hri_nvic_nvicip73_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP73.reg;
	tmp = (tmp & NVIC_NVICIP73_PRI73(mask)) >> 0;
	return tmp;
}

static inline void hri_nvic_write_NVICIP73_PRI73_bf(const void *const hw, hri_nvic_nvicip73_reg_t data)
{
	uint8_t tmp;
	NVIC_CRITICAL_SECTION_ENTER();
	tmp = ((Nvic *)hw)->NVICIP73.reg;
	tmp &= ~NVIC_NVICIP73_PRI73_Msk;
	tmp |= NVIC_NVICIP73_PRI73(data);
	((Nvic *)hw)->NVICIP73.reg = tmp;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP73_PRI73_bf(const void *const hw, hri_nvic_nvicip73_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP73.reg &= ~NVIC_NVICIP73_PRI73(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP73_PRI73_bf(const void *const hw, hri_nvic_nvicip73_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP73.reg ^= NVIC_NVICIP73_PRI73(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip73_reg_t hri_nvic_read_NVICIP73_PRI73_bf(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP73.reg;
	tmp = (tmp & NVIC_NVICIP73_PRI73_Msk) >> 0;
	return tmp;
}

static inline void hri_nvic_set_NVICIP73_reg(const void *const hw, hri_nvic_nvicip73_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP73.reg |= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip73_reg_t hri_nvic_get_NVICIP73_reg(const void *const hw, hri_nvic_nvicip73_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP73.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_nvic_write_NVICIP73_reg(const void *const hw, hri_nvic_nvicip73_reg_t data)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP73.reg = data;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP73_reg(const void *const hw, hri_nvic_nvicip73_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP73.reg &= ~mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP73_reg(const void *const hw, hri_nvic_nvicip73_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP73.reg ^= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip73_reg_t hri_nvic_read_NVICIP73_reg(const void *const hw)
{
	return ((Nvic *)hw)->NVICIP73.reg;
}

static inline void hri_nvic_set_NVICIP74_PRI74_bf(const void *const hw, hri_nvic_nvicip74_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP74.reg |= NVIC_NVICIP74_PRI74(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip74_reg_t hri_nvic_get_NVICIP74_PRI74_bf(const void *const hw, hri_nvic_nvicip74_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP74.reg;
	tmp = (tmp & NVIC_NVICIP74_PRI74(mask)) >> 0;
	return tmp;
}

static inline void hri_nvic_write_NVICIP74_PRI74_bf(const void *const hw, hri_nvic_nvicip74_reg_t data)
{
	uint8_t tmp;
	NVIC_CRITICAL_SECTION_ENTER();
	tmp = ((Nvic *)hw)->NVICIP74.reg;
	tmp &= ~NVIC_NVICIP74_PRI74_Msk;
	tmp |= NVIC_NVICIP74_PRI74(data);
	((Nvic *)hw)->NVICIP74.reg = tmp;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP74_PRI74_bf(const void *const hw, hri_nvic_nvicip74_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP74.reg &= ~NVIC_NVICIP74_PRI74(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP74_PRI74_bf(const void *const hw, hri_nvic_nvicip74_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP74.reg ^= NVIC_NVICIP74_PRI74(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip74_reg_t hri_nvic_read_NVICIP74_PRI74_bf(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP74.reg;
	tmp = (tmp & NVIC_NVICIP74_PRI74_Msk) >> 0;
	return tmp;
}

static inline void hri_nvic_set_NVICIP74_reg(const void *const hw, hri_nvic_nvicip74_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP74.reg |= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip74_reg_t hri_nvic_get_NVICIP74_reg(const void *const hw, hri_nvic_nvicip74_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP74.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_nvic_write_NVICIP74_reg(const void *const hw, hri_nvic_nvicip74_reg_t data)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP74.reg = data;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP74_reg(const void *const hw, hri_nvic_nvicip74_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP74.reg &= ~mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP74_reg(const void *const hw, hri_nvic_nvicip74_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP74.reg ^= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip74_reg_t hri_nvic_read_NVICIP74_reg(const void *const hw)
{
	return ((Nvic *)hw)->NVICIP74.reg;
}

static inline void hri_nvic_set_NVICIP75_PRI75_bf(const void *const hw, hri_nvic_nvicip75_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP75.reg |= NVIC_NVICIP75_PRI75(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip75_reg_t hri_nvic_get_NVICIP75_PRI75_bf(const void *const hw, hri_nvic_nvicip75_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP75.reg;
	tmp = (tmp & NVIC_NVICIP75_PRI75(mask)) >> 0;
	return tmp;
}

static inline void hri_nvic_write_NVICIP75_PRI75_bf(const void *const hw, hri_nvic_nvicip75_reg_t data)
{
	uint8_t tmp;
	NVIC_CRITICAL_SECTION_ENTER();
	tmp = ((Nvic *)hw)->NVICIP75.reg;
	tmp &= ~NVIC_NVICIP75_PRI75_Msk;
	tmp |= NVIC_NVICIP75_PRI75(data);
	((Nvic *)hw)->NVICIP75.reg = tmp;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP75_PRI75_bf(const void *const hw, hri_nvic_nvicip75_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP75.reg &= ~NVIC_NVICIP75_PRI75(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP75_PRI75_bf(const void *const hw, hri_nvic_nvicip75_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP75.reg ^= NVIC_NVICIP75_PRI75(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip75_reg_t hri_nvic_read_NVICIP75_PRI75_bf(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP75.reg;
	tmp = (tmp & NVIC_NVICIP75_PRI75_Msk) >> 0;
	return tmp;
}

static inline void hri_nvic_set_NVICIP75_reg(const void *const hw, hri_nvic_nvicip75_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP75.reg |= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip75_reg_t hri_nvic_get_NVICIP75_reg(const void *const hw, hri_nvic_nvicip75_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP75.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_nvic_write_NVICIP75_reg(const void *const hw, hri_nvic_nvicip75_reg_t data)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP75.reg = data;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP75_reg(const void *const hw, hri_nvic_nvicip75_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP75.reg &= ~mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP75_reg(const void *const hw, hri_nvic_nvicip75_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP75.reg ^= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip75_reg_t hri_nvic_read_NVICIP75_reg(const void *const hw)
{
	return ((Nvic *)hw)->NVICIP75.reg;
}

static inline void hri_nvic_set_NVICIP76_PRI76_bf(const void *const hw, hri_nvic_nvicip76_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP76.reg |= NVIC_NVICIP76_PRI76(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip76_reg_t hri_nvic_get_NVICIP76_PRI76_bf(const void *const hw, hri_nvic_nvicip76_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP76.reg;
	tmp = (tmp & NVIC_NVICIP76_PRI76(mask)) >> 0;
	return tmp;
}

static inline void hri_nvic_write_NVICIP76_PRI76_bf(const void *const hw, hri_nvic_nvicip76_reg_t data)
{
	uint8_t tmp;
	NVIC_CRITICAL_SECTION_ENTER();
	tmp = ((Nvic *)hw)->NVICIP76.reg;
	tmp &= ~NVIC_NVICIP76_PRI76_Msk;
	tmp |= NVIC_NVICIP76_PRI76(data);
	((Nvic *)hw)->NVICIP76.reg = tmp;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP76_PRI76_bf(const void *const hw, hri_nvic_nvicip76_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP76.reg &= ~NVIC_NVICIP76_PRI76(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP76_PRI76_bf(const void *const hw, hri_nvic_nvicip76_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP76.reg ^= NVIC_NVICIP76_PRI76(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip76_reg_t hri_nvic_read_NVICIP76_PRI76_bf(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP76.reg;
	tmp = (tmp & NVIC_NVICIP76_PRI76_Msk) >> 0;
	return tmp;
}

static inline void hri_nvic_set_NVICIP76_reg(const void *const hw, hri_nvic_nvicip76_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP76.reg |= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip76_reg_t hri_nvic_get_NVICIP76_reg(const void *const hw, hri_nvic_nvicip76_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP76.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_nvic_write_NVICIP76_reg(const void *const hw, hri_nvic_nvicip76_reg_t data)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP76.reg = data;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP76_reg(const void *const hw, hri_nvic_nvicip76_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP76.reg &= ~mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP76_reg(const void *const hw, hri_nvic_nvicip76_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP76.reg ^= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip76_reg_t hri_nvic_read_NVICIP76_reg(const void *const hw)
{
	return ((Nvic *)hw)->NVICIP76.reg;
}

static inline void hri_nvic_set_NVICIP77_PRI77_bf(const void *const hw, hri_nvic_nvicip77_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP77.reg |= NVIC_NVICIP77_PRI77(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip77_reg_t hri_nvic_get_NVICIP77_PRI77_bf(const void *const hw, hri_nvic_nvicip77_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP77.reg;
	tmp = (tmp & NVIC_NVICIP77_PRI77(mask)) >> 0;
	return tmp;
}

static inline void hri_nvic_write_NVICIP77_PRI77_bf(const void *const hw, hri_nvic_nvicip77_reg_t data)
{
	uint8_t tmp;
	NVIC_CRITICAL_SECTION_ENTER();
	tmp = ((Nvic *)hw)->NVICIP77.reg;
	tmp &= ~NVIC_NVICIP77_PRI77_Msk;
	tmp |= NVIC_NVICIP77_PRI77(data);
	((Nvic *)hw)->NVICIP77.reg = tmp;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP77_PRI77_bf(const void *const hw, hri_nvic_nvicip77_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP77.reg &= ~NVIC_NVICIP77_PRI77(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP77_PRI77_bf(const void *const hw, hri_nvic_nvicip77_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP77.reg ^= NVIC_NVICIP77_PRI77(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip77_reg_t hri_nvic_read_NVICIP77_PRI77_bf(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP77.reg;
	tmp = (tmp & NVIC_NVICIP77_PRI77_Msk) >> 0;
	return tmp;
}

static inline void hri_nvic_set_NVICIP77_reg(const void *const hw, hri_nvic_nvicip77_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP77.reg |= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip77_reg_t hri_nvic_get_NVICIP77_reg(const void *const hw, hri_nvic_nvicip77_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP77.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_nvic_write_NVICIP77_reg(const void *const hw, hri_nvic_nvicip77_reg_t data)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP77.reg = data;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP77_reg(const void *const hw, hri_nvic_nvicip77_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP77.reg &= ~mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP77_reg(const void *const hw, hri_nvic_nvicip77_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP77.reg ^= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip77_reg_t hri_nvic_read_NVICIP77_reg(const void *const hw)
{
	return ((Nvic *)hw)->NVICIP77.reg;
}

static inline void hri_nvic_set_NVICIP78_PRI78_bf(const void *const hw, hri_nvic_nvicip78_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP78.reg |= NVIC_NVICIP78_PRI78(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip78_reg_t hri_nvic_get_NVICIP78_PRI78_bf(const void *const hw, hri_nvic_nvicip78_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP78.reg;
	tmp = (tmp & NVIC_NVICIP78_PRI78(mask)) >> 0;
	return tmp;
}

static inline void hri_nvic_write_NVICIP78_PRI78_bf(const void *const hw, hri_nvic_nvicip78_reg_t data)
{
	uint8_t tmp;
	NVIC_CRITICAL_SECTION_ENTER();
	tmp = ((Nvic *)hw)->NVICIP78.reg;
	tmp &= ~NVIC_NVICIP78_PRI78_Msk;
	tmp |= NVIC_NVICIP78_PRI78(data);
	((Nvic *)hw)->NVICIP78.reg = tmp;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP78_PRI78_bf(const void *const hw, hri_nvic_nvicip78_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP78.reg &= ~NVIC_NVICIP78_PRI78(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP78_PRI78_bf(const void *const hw, hri_nvic_nvicip78_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP78.reg ^= NVIC_NVICIP78_PRI78(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip78_reg_t hri_nvic_read_NVICIP78_PRI78_bf(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP78.reg;
	tmp = (tmp & NVIC_NVICIP78_PRI78_Msk) >> 0;
	return tmp;
}

static inline void hri_nvic_set_NVICIP78_reg(const void *const hw, hri_nvic_nvicip78_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP78.reg |= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip78_reg_t hri_nvic_get_NVICIP78_reg(const void *const hw, hri_nvic_nvicip78_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP78.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_nvic_write_NVICIP78_reg(const void *const hw, hri_nvic_nvicip78_reg_t data)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP78.reg = data;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP78_reg(const void *const hw, hri_nvic_nvicip78_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP78.reg &= ~mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP78_reg(const void *const hw, hri_nvic_nvicip78_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP78.reg ^= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip78_reg_t hri_nvic_read_NVICIP78_reg(const void *const hw)
{
	return ((Nvic *)hw)->NVICIP78.reg;
}

static inline void hri_nvic_set_NVICIP79_PRI79_bf(const void *const hw, hri_nvic_nvicip79_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP79.reg |= NVIC_NVICIP79_PRI79(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip79_reg_t hri_nvic_get_NVICIP79_PRI79_bf(const void *const hw, hri_nvic_nvicip79_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP79.reg;
	tmp = (tmp & NVIC_NVICIP79_PRI79(mask)) >> 0;
	return tmp;
}

static inline void hri_nvic_write_NVICIP79_PRI79_bf(const void *const hw, hri_nvic_nvicip79_reg_t data)
{
	uint8_t tmp;
	NVIC_CRITICAL_SECTION_ENTER();
	tmp = ((Nvic *)hw)->NVICIP79.reg;
	tmp &= ~NVIC_NVICIP79_PRI79_Msk;
	tmp |= NVIC_NVICIP79_PRI79(data);
	((Nvic *)hw)->NVICIP79.reg = tmp;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP79_PRI79_bf(const void *const hw, hri_nvic_nvicip79_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP79.reg &= ~NVIC_NVICIP79_PRI79(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP79_PRI79_bf(const void *const hw, hri_nvic_nvicip79_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP79.reg ^= NVIC_NVICIP79_PRI79(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip79_reg_t hri_nvic_read_NVICIP79_PRI79_bf(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP79.reg;
	tmp = (tmp & NVIC_NVICIP79_PRI79_Msk) >> 0;
	return tmp;
}

static inline void hri_nvic_set_NVICIP79_reg(const void *const hw, hri_nvic_nvicip79_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP79.reg |= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip79_reg_t hri_nvic_get_NVICIP79_reg(const void *const hw, hri_nvic_nvicip79_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP79.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_nvic_write_NVICIP79_reg(const void *const hw, hri_nvic_nvicip79_reg_t data)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP79.reg = data;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP79_reg(const void *const hw, hri_nvic_nvicip79_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP79.reg &= ~mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP79_reg(const void *const hw, hri_nvic_nvicip79_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP79.reg ^= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip79_reg_t hri_nvic_read_NVICIP79_reg(const void *const hw)
{
	return ((Nvic *)hw)->NVICIP79.reg;
}

static inline void hri_nvic_set_NVICIP80_PRI80_bf(const void *const hw, hri_nvic_nvicip80_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP80.reg |= NVIC_NVICIP80_PRI80(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip80_reg_t hri_nvic_get_NVICIP80_PRI80_bf(const void *const hw, hri_nvic_nvicip80_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP80.reg;
	tmp = (tmp & NVIC_NVICIP80_PRI80(mask)) >> 0;
	return tmp;
}

static inline void hri_nvic_write_NVICIP80_PRI80_bf(const void *const hw, hri_nvic_nvicip80_reg_t data)
{
	uint8_t tmp;
	NVIC_CRITICAL_SECTION_ENTER();
	tmp = ((Nvic *)hw)->NVICIP80.reg;
	tmp &= ~NVIC_NVICIP80_PRI80_Msk;
	tmp |= NVIC_NVICIP80_PRI80(data);
	((Nvic *)hw)->NVICIP80.reg = tmp;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP80_PRI80_bf(const void *const hw, hri_nvic_nvicip80_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP80.reg &= ~NVIC_NVICIP80_PRI80(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP80_PRI80_bf(const void *const hw, hri_nvic_nvicip80_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP80.reg ^= NVIC_NVICIP80_PRI80(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip80_reg_t hri_nvic_read_NVICIP80_PRI80_bf(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP80.reg;
	tmp = (tmp & NVIC_NVICIP80_PRI80_Msk) >> 0;
	return tmp;
}

static inline void hri_nvic_set_NVICIP80_reg(const void *const hw, hri_nvic_nvicip80_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP80.reg |= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip80_reg_t hri_nvic_get_NVICIP80_reg(const void *const hw, hri_nvic_nvicip80_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP80.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_nvic_write_NVICIP80_reg(const void *const hw, hri_nvic_nvicip80_reg_t data)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP80.reg = data;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP80_reg(const void *const hw, hri_nvic_nvicip80_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP80.reg &= ~mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP80_reg(const void *const hw, hri_nvic_nvicip80_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP80.reg ^= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip80_reg_t hri_nvic_read_NVICIP80_reg(const void *const hw)
{
	return ((Nvic *)hw)->NVICIP80.reg;
}

static inline void hri_nvic_set_NVICIP81_PRI81_bf(const void *const hw, hri_nvic_nvicip81_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP81.reg |= NVIC_NVICIP81_PRI81(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip81_reg_t hri_nvic_get_NVICIP81_PRI81_bf(const void *const hw, hri_nvic_nvicip81_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP81.reg;
	tmp = (tmp & NVIC_NVICIP81_PRI81(mask)) >> 0;
	return tmp;
}

static inline void hri_nvic_write_NVICIP81_PRI81_bf(const void *const hw, hri_nvic_nvicip81_reg_t data)
{
	uint8_t tmp;
	NVIC_CRITICAL_SECTION_ENTER();
	tmp = ((Nvic *)hw)->NVICIP81.reg;
	tmp &= ~NVIC_NVICIP81_PRI81_Msk;
	tmp |= NVIC_NVICIP81_PRI81(data);
	((Nvic *)hw)->NVICIP81.reg = tmp;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP81_PRI81_bf(const void *const hw, hri_nvic_nvicip81_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP81.reg &= ~NVIC_NVICIP81_PRI81(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP81_PRI81_bf(const void *const hw, hri_nvic_nvicip81_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP81.reg ^= NVIC_NVICIP81_PRI81(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip81_reg_t hri_nvic_read_NVICIP81_PRI81_bf(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP81.reg;
	tmp = (tmp & NVIC_NVICIP81_PRI81_Msk) >> 0;
	return tmp;
}

static inline void hri_nvic_set_NVICIP81_reg(const void *const hw, hri_nvic_nvicip81_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP81.reg |= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip81_reg_t hri_nvic_get_NVICIP81_reg(const void *const hw, hri_nvic_nvicip81_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP81.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_nvic_write_NVICIP81_reg(const void *const hw, hri_nvic_nvicip81_reg_t data)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP81.reg = data;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP81_reg(const void *const hw, hri_nvic_nvicip81_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP81.reg &= ~mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP81_reg(const void *const hw, hri_nvic_nvicip81_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP81.reg ^= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip81_reg_t hri_nvic_read_NVICIP81_reg(const void *const hw)
{
	return ((Nvic *)hw)->NVICIP81.reg;
}

static inline void hri_nvic_set_NVICIP82_PRI82_bf(const void *const hw, hri_nvic_nvicip82_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP82.reg |= NVIC_NVICIP82_PRI82(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip82_reg_t hri_nvic_get_NVICIP82_PRI82_bf(const void *const hw, hri_nvic_nvicip82_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP82.reg;
	tmp = (tmp & NVIC_NVICIP82_PRI82(mask)) >> 0;
	return tmp;
}

static inline void hri_nvic_write_NVICIP82_PRI82_bf(const void *const hw, hri_nvic_nvicip82_reg_t data)
{
	uint8_t tmp;
	NVIC_CRITICAL_SECTION_ENTER();
	tmp = ((Nvic *)hw)->NVICIP82.reg;
	tmp &= ~NVIC_NVICIP82_PRI82_Msk;
	tmp |= NVIC_NVICIP82_PRI82(data);
	((Nvic *)hw)->NVICIP82.reg = tmp;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP82_PRI82_bf(const void *const hw, hri_nvic_nvicip82_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP82.reg &= ~NVIC_NVICIP82_PRI82(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP82_PRI82_bf(const void *const hw, hri_nvic_nvicip82_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP82.reg ^= NVIC_NVICIP82_PRI82(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip82_reg_t hri_nvic_read_NVICIP82_PRI82_bf(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP82.reg;
	tmp = (tmp & NVIC_NVICIP82_PRI82_Msk) >> 0;
	return tmp;
}

static inline void hri_nvic_set_NVICIP82_reg(const void *const hw, hri_nvic_nvicip82_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP82.reg |= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip82_reg_t hri_nvic_get_NVICIP82_reg(const void *const hw, hri_nvic_nvicip82_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP82.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_nvic_write_NVICIP82_reg(const void *const hw, hri_nvic_nvicip82_reg_t data)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP82.reg = data;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP82_reg(const void *const hw, hri_nvic_nvicip82_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP82.reg &= ~mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP82_reg(const void *const hw, hri_nvic_nvicip82_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP82.reg ^= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip82_reg_t hri_nvic_read_NVICIP82_reg(const void *const hw)
{
	return ((Nvic *)hw)->NVICIP82.reg;
}

static inline void hri_nvic_set_NVICIP83_PRI83_bf(const void *const hw, hri_nvic_nvicip83_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP83.reg |= NVIC_NVICIP83_PRI83(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip83_reg_t hri_nvic_get_NVICIP83_PRI83_bf(const void *const hw, hri_nvic_nvicip83_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP83.reg;
	tmp = (tmp & NVIC_NVICIP83_PRI83(mask)) >> 0;
	return tmp;
}

static inline void hri_nvic_write_NVICIP83_PRI83_bf(const void *const hw, hri_nvic_nvicip83_reg_t data)
{
	uint8_t tmp;
	NVIC_CRITICAL_SECTION_ENTER();
	tmp = ((Nvic *)hw)->NVICIP83.reg;
	tmp &= ~NVIC_NVICIP83_PRI83_Msk;
	tmp |= NVIC_NVICIP83_PRI83(data);
	((Nvic *)hw)->NVICIP83.reg = tmp;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP83_PRI83_bf(const void *const hw, hri_nvic_nvicip83_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP83.reg &= ~NVIC_NVICIP83_PRI83(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP83_PRI83_bf(const void *const hw, hri_nvic_nvicip83_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP83.reg ^= NVIC_NVICIP83_PRI83(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip83_reg_t hri_nvic_read_NVICIP83_PRI83_bf(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP83.reg;
	tmp = (tmp & NVIC_NVICIP83_PRI83_Msk) >> 0;
	return tmp;
}

static inline void hri_nvic_set_NVICIP83_reg(const void *const hw, hri_nvic_nvicip83_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP83.reg |= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip83_reg_t hri_nvic_get_NVICIP83_reg(const void *const hw, hri_nvic_nvicip83_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP83.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_nvic_write_NVICIP83_reg(const void *const hw, hri_nvic_nvicip83_reg_t data)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP83.reg = data;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP83_reg(const void *const hw, hri_nvic_nvicip83_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP83.reg &= ~mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP83_reg(const void *const hw, hri_nvic_nvicip83_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP83.reg ^= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip83_reg_t hri_nvic_read_NVICIP83_reg(const void *const hw)
{
	return ((Nvic *)hw)->NVICIP83.reg;
}

static inline void hri_nvic_set_NVICIP84_PRI84_bf(const void *const hw, hri_nvic_nvicip84_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP84.reg |= NVIC_NVICIP84_PRI84(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip84_reg_t hri_nvic_get_NVICIP84_PRI84_bf(const void *const hw, hri_nvic_nvicip84_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP84.reg;
	tmp = (tmp & NVIC_NVICIP84_PRI84(mask)) >> 0;
	return tmp;
}

static inline void hri_nvic_write_NVICIP84_PRI84_bf(const void *const hw, hri_nvic_nvicip84_reg_t data)
{
	uint8_t tmp;
	NVIC_CRITICAL_SECTION_ENTER();
	tmp = ((Nvic *)hw)->NVICIP84.reg;
	tmp &= ~NVIC_NVICIP84_PRI84_Msk;
	tmp |= NVIC_NVICIP84_PRI84(data);
	((Nvic *)hw)->NVICIP84.reg = tmp;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP84_PRI84_bf(const void *const hw, hri_nvic_nvicip84_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP84.reg &= ~NVIC_NVICIP84_PRI84(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP84_PRI84_bf(const void *const hw, hri_nvic_nvicip84_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP84.reg ^= NVIC_NVICIP84_PRI84(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip84_reg_t hri_nvic_read_NVICIP84_PRI84_bf(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP84.reg;
	tmp = (tmp & NVIC_NVICIP84_PRI84_Msk) >> 0;
	return tmp;
}

static inline void hri_nvic_set_NVICIP84_reg(const void *const hw, hri_nvic_nvicip84_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP84.reg |= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip84_reg_t hri_nvic_get_NVICIP84_reg(const void *const hw, hri_nvic_nvicip84_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP84.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_nvic_write_NVICIP84_reg(const void *const hw, hri_nvic_nvicip84_reg_t data)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP84.reg = data;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP84_reg(const void *const hw, hri_nvic_nvicip84_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP84.reg &= ~mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP84_reg(const void *const hw, hri_nvic_nvicip84_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP84.reg ^= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip84_reg_t hri_nvic_read_NVICIP84_reg(const void *const hw)
{
	return ((Nvic *)hw)->NVICIP84.reg;
}

static inline void hri_nvic_set_NVICIP85_PRI85_bf(const void *const hw, hri_nvic_nvicip85_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP85.reg |= NVIC_NVICIP85_PRI85(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip85_reg_t hri_nvic_get_NVICIP85_PRI85_bf(const void *const hw, hri_nvic_nvicip85_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP85.reg;
	tmp = (tmp & NVIC_NVICIP85_PRI85(mask)) >> 0;
	return tmp;
}

static inline void hri_nvic_write_NVICIP85_PRI85_bf(const void *const hw, hri_nvic_nvicip85_reg_t data)
{
	uint8_t tmp;
	NVIC_CRITICAL_SECTION_ENTER();
	tmp = ((Nvic *)hw)->NVICIP85.reg;
	tmp &= ~NVIC_NVICIP85_PRI85_Msk;
	tmp |= NVIC_NVICIP85_PRI85(data);
	((Nvic *)hw)->NVICIP85.reg = tmp;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP85_PRI85_bf(const void *const hw, hri_nvic_nvicip85_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP85.reg &= ~NVIC_NVICIP85_PRI85(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP85_PRI85_bf(const void *const hw, hri_nvic_nvicip85_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP85.reg ^= NVIC_NVICIP85_PRI85(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip85_reg_t hri_nvic_read_NVICIP85_PRI85_bf(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP85.reg;
	tmp = (tmp & NVIC_NVICIP85_PRI85_Msk) >> 0;
	return tmp;
}

static inline void hri_nvic_set_NVICIP85_reg(const void *const hw, hri_nvic_nvicip85_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP85.reg |= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip85_reg_t hri_nvic_get_NVICIP85_reg(const void *const hw, hri_nvic_nvicip85_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP85.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_nvic_write_NVICIP85_reg(const void *const hw, hri_nvic_nvicip85_reg_t data)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP85.reg = data;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP85_reg(const void *const hw, hri_nvic_nvicip85_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP85.reg &= ~mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP85_reg(const void *const hw, hri_nvic_nvicip85_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP85.reg ^= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip85_reg_t hri_nvic_read_NVICIP85_reg(const void *const hw)
{
	return ((Nvic *)hw)->NVICIP85.reg;
}

static inline void hri_nvic_set_NVICIP86_PRI86_bf(const void *const hw, hri_nvic_nvicip86_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP86.reg |= NVIC_NVICIP86_PRI86(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip86_reg_t hri_nvic_get_NVICIP86_PRI86_bf(const void *const hw, hri_nvic_nvicip86_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP86.reg;
	tmp = (tmp & NVIC_NVICIP86_PRI86(mask)) >> 0;
	return tmp;
}

static inline void hri_nvic_write_NVICIP86_PRI86_bf(const void *const hw, hri_nvic_nvicip86_reg_t data)
{
	uint8_t tmp;
	NVIC_CRITICAL_SECTION_ENTER();
	tmp = ((Nvic *)hw)->NVICIP86.reg;
	tmp &= ~NVIC_NVICIP86_PRI86_Msk;
	tmp |= NVIC_NVICIP86_PRI86(data);
	((Nvic *)hw)->NVICIP86.reg = tmp;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP86_PRI86_bf(const void *const hw, hri_nvic_nvicip86_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP86.reg &= ~NVIC_NVICIP86_PRI86(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP86_PRI86_bf(const void *const hw, hri_nvic_nvicip86_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP86.reg ^= NVIC_NVICIP86_PRI86(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip86_reg_t hri_nvic_read_NVICIP86_PRI86_bf(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP86.reg;
	tmp = (tmp & NVIC_NVICIP86_PRI86_Msk) >> 0;
	return tmp;
}

static inline void hri_nvic_set_NVICIP86_reg(const void *const hw, hri_nvic_nvicip86_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP86.reg |= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip86_reg_t hri_nvic_get_NVICIP86_reg(const void *const hw, hri_nvic_nvicip86_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP86.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_nvic_write_NVICIP86_reg(const void *const hw, hri_nvic_nvicip86_reg_t data)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP86.reg = data;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP86_reg(const void *const hw, hri_nvic_nvicip86_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP86.reg &= ~mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP86_reg(const void *const hw, hri_nvic_nvicip86_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP86.reg ^= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip86_reg_t hri_nvic_read_NVICIP86_reg(const void *const hw)
{
	return ((Nvic *)hw)->NVICIP86.reg;
}

static inline void hri_nvic_set_NVICIP87_PRI87_bf(const void *const hw, hri_nvic_nvicip87_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP87.reg |= NVIC_NVICIP87_PRI87(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip87_reg_t hri_nvic_get_NVICIP87_PRI87_bf(const void *const hw, hri_nvic_nvicip87_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP87.reg;
	tmp = (tmp & NVIC_NVICIP87_PRI87(mask)) >> 0;
	return tmp;
}

static inline void hri_nvic_write_NVICIP87_PRI87_bf(const void *const hw, hri_nvic_nvicip87_reg_t data)
{
	uint8_t tmp;
	NVIC_CRITICAL_SECTION_ENTER();
	tmp = ((Nvic *)hw)->NVICIP87.reg;
	tmp &= ~NVIC_NVICIP87_PRI87_Msk;
	tmp |= NVIC_NVICIP87_PRI87(data);
	((Nvic *)hw)->NVICIP87.reg = tmp;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP87_PRI87_bf(const void *const hw, hri_nvic_nvicip87_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP87.reg &= ~NVIC_NVICIP87_PRI87(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP87_PRI87_bf(const void *const hw, hri_nvic_nvicip87_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP87.reg ^= NVIC_NVICIP87_PRI87(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip87_reg_t hri_nvic_read_NVICIP87_PRI87_bf(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP87.reg;
	tmp = (tmp & NVIC_NVICIP87_PRI87_Msk) >> 0;
	return tmp;
}

static inline void hri_nvic_set_NVICIP87_reg(const void *const hw, hri_nvic_nvicip87_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP87.reg |= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip87_reg_t hri_nvic_get_NVICIP87_reg(const void *const hw, hri_nvic_nvicip87_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP87.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_nvic_write_NVICIP87_reg(const void *const hw, hri_nvic_nvicip87_reg_t data)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP87.reg = data;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP87_reg(const void *const hw, hri_nvic_nvicip87_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP87.reg &= ~mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP87_reg(const void *const hw, hri_nvic_nvicip87_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP87.reg ^= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip87_reg_t hri_nvic_read_NVICIP87_reg(const void *const hw)
{
	return ((Nvic *)hw)->NVICIP87.reg;
}

static inline void hri_nvic_set_NVICIP88_PRI88_bf(const void *const hw, hri_nvic_nvicip88_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP88.reg |= NVIC_NVICIP88_PRI88(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip88_reg_t hri_nvic_get_NVICIP88_PRI88_bf(const void *const hw, hri_nvic_nvicip88_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP88.reg;
	tmp = (tmp & NVIC_NVICIP88_PRI88(mask)) >> 0;
	return tmp;
}

static inline void hri_nvic_write_NVICIP88_PRI88_bf(const void *const hw, hri_nvic_nvicip88_reg_t data)
{
	uint8_t tmp;
	NVIC_CRITICAL_SECTION_ENTER();
	tmp = ((Nvic *)hw)->NVICIP88.reg;
	tmp &= ~NVIC_NVICIP88_PRI88_Msk;
	tmp |= NVIC_NVICIP88_PRI88(data);
	((Nvic *)hw)->NVICIP88.reg = tmp;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP88_PRI88_bf(const void *const hw, hri_nvic_nvicip88_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP88.reg &= ~NVIC_NVICIP88_PRI88(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP88_PRI88_bf(const void *const hw, hri_nvic_nvicip88_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP88.reg ^= NVIC_NVICIP88_PRI88(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip88_reg_t hri_nvic_read_NVICIP88_PRI88_bf(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP88.reg;
	tmp = (tmp & NVIC_NVICIP88_PRI88_Msk) >> 0;
	return tmp;
}

static inline void hri_nvic_set_NVICIP88_reg(const void *const hw, hri_nvic_nvicip88_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP88.reg |= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip88_reg_t hri_nvic_get_NVICIP88_reg(const void *const hw, hri_nvic_nvicip88_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP88.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_nvic_write_NVICIP88_reg(const void *const hw, hri_nvic_nvicip88_reg_t data)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP88.reg = data;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP88_reg(const void *const hw, hri_nvic_nvicip88_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP88.reg &= ~mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP88_reg(const void *const hw, hri_nvic_nvicip88_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP88.reg ^= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip88_reg_t hri_nvic_read_NVICIP88_reg(const void *const hw)
{
	return ((Nvic *)hw)->NVICIP88.reg;
}

static inline void hri_nvic_set_NVICIP89_PRI89_bf(const void *const hw, hri_nvic_nvicip89_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP89.reg |= NVIC_NVICIP89_PRI89(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip89_reg_t hri_nvic_get_NVICIP89_PRI89_bf(const void *const hw, hri_nvic_nvicip89_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP89.reg;
	tmp = (tmp & NVIC_NVICIP89_PRI89(mask)) >> 0;
	return tmp;
}

static inline void hri_nvic_write_NVICIP89_PRI89_bf(const void *const hw, hri_nvic_nvicip89_reg_t data)
{
	uint8_t tmp;
	NVIC_CRITICAL_SECTION_ENTER();
	tmp = ((Nvic *)hw)->NVICIP89.reg;
	tmp &= ~NVIC_NVICIP89_PRI89_Msk;
	tmp |= NVIC_NVICIP89_PRI89(data);
	((Nvic *)hw)->NVICIP89.reg = tmp;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP89_PRI89_bf(const void *const hw, hri_nvic_nvicip89_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP89.reg &= ~NVIC_NVICIP89_PRI89(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP89_PRI89_bf(const void *const hw, hri_nvic_nvicip89_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP89.reg ^= NVIC_NVICIP89_PRI89(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip89_reg_t hri_nvic_read_NVICIP89_PRI89_bf(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP89.reg;
	tmp = (tmp & NVIC_NVICIP89_PRI89_Msk) >> 0;
	return tmp;
}

static inline void hri_nvic_set_NVICIP89_reg(const void *const hw, hri_nvic_nvicip89_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP89.reg |= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip89_reg_t hri_nvic_get_NVICIP89_reg(const void *const hw, hri_nvic_nvicip89_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP89.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_nvic_write_NVICIP89_reg(const void *const hw, hri_nvic_nvicip89_reg_t data)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP89.reg = data;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP89_reg(const void *const hw, hri_nvic_nvicip89_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP89.reg &= ~mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP89_reg(const void *const hw, hri_nvic_nvicip89_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP89.reg ^= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip89_reg_t hri_nvic_read_NVICIP89_reg(const void *const hw)
{
	return ((Nvic *)hw)->NVICIP89.reg;
}

static inline void hri_nvic_set_NVICIP90_PRI90_bf(const void *const hw, hri_nvic_nvicip90_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP90.reg |= NVIC_NVICIP90_PRI90(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip90_reg_t hri_nvic_get_NVICIP90_PRI90_bf(const void *const hw, hri_nvic_nvicip90_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP90.reg;
	tmp = (tmp & NVIC_NVICIP90_PRI90(mask)) >> 0;
	return tmp;
}

static inline void hri_nvic_write_NVICIP90_PRI90_bf(const void *const hw, hri_nvic_nvicip90_reg_t data)
{
	uint8_t tmp;
	NVIC_CRITICAL_SECTION_ENTER();
	tmp = ((Nvic *)hw)->NVICIP90.reg;
	tmp &= ~NVIC_NVICIP90_PRI90_Msk;
	tmp |= NVIC_NVICIP90_PRI90(data);
	((Nvic *)hw)->NVICIP90.reg = tmp;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP90_PRI90_bf(const void *const hw, hri_nvic_nvicip90_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP90.reg &= ~NVIC_NVICIP90_PRI90(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP90_PRI90_bf(const void *const hw, hri_nvic_nvicip90_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP90.reg ^= NVIC_NVICIP90_PRI90(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip90_reg_t hri_nvic_read_NVICIP90_PRI90_bf(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP90.reg;
	tmp = (tmp & NVIC_NVICIP90_PRI90_Msk) >> 0;
	return tmp;
}

static inline void hri_nvic_set_NVICIP90_reg(const void *const hw, hri_nvic_nvicip90_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP90.reg |= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip90_reg_t hri_nvic_get_NVICIP90_reg(const void *const hw, hri_nvic_nvicip90_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP90.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_nvic_write_NVICIP90_reg(const void *const hw, hri_nvic_nvicip90_reg_t data)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP90.reg = data;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP90_reg(const void *const hw, hri_nvic_nvicip90_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP90.reg &= ~mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP90_reg(const void *const hw, hri_nvic_nvicip90_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP90.reg ^= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip90_reg_t hri_nvic_read_NVICIP90_reg(const void *const hw)
{
	return ((Nvic *)hw)->NVICIP90.reg;
}

static inline void hri_nvic_set_NVICIP91_PRI91_bf(const void *const hw, hri_nvic_nvicip91_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP91.reg |= NVIC_NVICIP91_PRI91(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip91_reg_t hri_nvic_get_NVICIP91_PRI91_bf(const void *const hw, hri_nvic_nvicip91_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP91.reg;
	tmp = (tmp & NVIC_NVICIP91_PRI91(mask)) >> 0;
	return tmp;
}

static inline void hri_nvic_write_NVICIP91_PRI91_bf(const void *const hw, hri_nvic_nvicip91_reg_t data)
{
	uint8_t tmp;
	NVIC_CRITICAL_SECTION_ENTER();
	tmp = ((Nvic *)hw)->NVICIP91.reg;
	tmp &= ~NVIC_NVICIP91_PRI91_Msk;
	tmp |= NVIC_NVICIP91_PRI91(data);
	((Nvic *)hw)->NVICIP91.reg = tmp;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP91_PRI91_bf(const void *const hw, hri_nvic_nvicip91_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP91.reg &= ~NVIC_NVICIP91_PRI91(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP91_PRI91_bf(const void *const hw, hri_nvic_nvicip91_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP91.reg ^= NVIC_NVICIP91_PRI91(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip91_reg_t hri_nvic_read_NVICIP91_PRI91_bf(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP91.reg;
	tmp = (tmp & NVIC_NVICIP91_PRI91_Msk) >> 0;
	return tmp;
}

static inline void hri_nvic_set_NVICIP91_reg(const void *const hw, hri_nvic_nvicip91_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP91.reg |= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip91_reg_t hri_nvic_get_NVICIP91_reg(const void *const hw, hri_nvic_nvicip91_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP91.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_nvic_write_NVICIP91_reg(const void *const hw, hri_nvic_nvicip91_reg_t data)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP91.reg = data;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP91_reg(const void *const hw, hri_nvic_nvicip91_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP91.reg &= ~mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP91_reg(const void *const hw, hri_nvic_nvicip91_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP91.reg ^= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip91_reg_t hri_nvic_read_NVICIP91_reg(const void *const hw)
{
	return ((Nvic *)hw)->NVICIP91.reg;
}

static inline void hri_nvic_set_NVICIP92_PRI92_bf(const void *const hw, hri_nvic_nvicip92_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP92.reg |= NVIC_NVICIP92_PRI92(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip92_reg_t hri_nvic_get_NVICIP92_PRI92_bf(const void *const hw, hri_nvic_nvicip92_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP92.reg;
	tmp = (tmp & NVIC_NVICIP92_PRI92(mask)) >> 0;
	return tmp;
}

static inline void hri_nvic_write_NVICIP92_PRI92_bf(const void *const hw, hri_nvic_nvicip92_reg_t data)
{
	uint8_t tmp;
	NVIC_CRITICAL_SECTION_ENTER();
	tmp = ((Nvic *)hw)->NVICIP92.reg;
	tmp &= ~NVIC_NVICIP92_PRI92_Msk;
	tmp |= NVIC_NVICIP92_PRI92(data);
	((Nvic *)hw)->NVICIP92.reg = tmp;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP92_PRI92_bf(const void *const hw, hri_nvic_nvicip92_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP92.reg &= ~NVIC_NVICIP92_PRI92(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP92_PRI92_bf(const void *const hw, hri_nvic_nvicip92_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP92.reg ^= NVIC_NVICIP92_PRI92(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip92_reg_t hri_nvic_read_NVICIP92_PRI92_bf(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP92.reg;
	tmp = (tmp & NVIC_NVICIP92_PRI92_Msk) >> 0;
	return tmp;
}

static inline void hri_nvic_set_NVICIP92_reg(const void *const hw, hri_nvic_nvicip92_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP92.reg |= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip92_reg_t hri_nvic_get_NVICIP92_reg(const void *const hw, hri_nvic_nvicip92_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP92.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_nvic_write_NVICIP92_reg(const void *const hw, hri_nvic_nvicip92_reg_t data)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP92.reg = data;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP92_reg(const void *const hw, hri_nvic_nvicip92_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP92.reg &= ~mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP92_reg(const void *const hw, hri_nvic_nvicip92_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP92.reg ^= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip92_reg_t hri_nvic_read_NVICIP92_reg(const void *const hw)
{
	return ((Nvic *)hw)->NVICIP92.reg;
}

static inline void hri_nvic_set_NVICIP93_PRI93_bf(const void *const hw, hri_nvic_nvicip93_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP93.reg |= NVIC_NVICIP93_PRI93(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip93_reg_t hri_nvic_get_NVICIP93_PRI93_bf(const void *const hw, hri_nvic_nvicip93_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP93.reg;
	tmp = (tmp & NVIC_NVICIP93_PRI93(mask)) >> 0;
	return tmp;
}

static inline void hri_nvic_write_NVICIP93_PRI93_bf(const void *const hw, hri_nvic_nvicip93_reg_t data)
{
	uint8_t tmp;
	NVIC_CRITICAL_SECTION_ENTER();
	tmp = ((Nvic *)hw)->NVICIP93.reg;
	tmp &= ~NVIC_NVICIP93_PRI93_Msk;
	tmp |= NVIC_NVICIP93_PRI93(data);
	((Nvic *)hw)->NVICIP93.reg = tmp;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP93_PRI93_bf(const void *const hw, hri_nvic_nvicip93_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP93.reg &= ~NVIC_NVICIP93_PRI93(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP93_PRI93_bf(const void *const hw, hri_nvic_nvicip93_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP93.reg ^= NVIC_NVICIP93_PRI93(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip93_reg_t hri_nvic_read_NVICIP93_PRI93_bf(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP93.reg;
	tmp = (tmp & NVIC_NVICIP93_PRI93_Msk) >> 0;
	return tmp;
}

static inline void hri_nvic_set_NVICIP93_reg(const void *const hw, hri_nvic_nvicip93_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP93.reg |= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip93_reg_t hri_nvic_get_NVICIP93_reg(const void *const hw, hri_nvic_nvicip93_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP93.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_nvic_write_NVICIP93_reg(const void *const hw, hri_nvic_nvicip93_reg_t data)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP93.reg = data;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP93_reg(const void *const hw, hri_nvic_nvicip93_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP93.reg &= ~mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP93_reg(const void *const hw, hri_nvic_nvicip93_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP93.reg ^= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip93_reg_t hri_nvic_read_NVICIP93_reg(const void *const hw)
{
	return ((Nvic *)hw)->NVICIP93.reg;
}

static inline void hri_nvic_set_NVICIP94_PRI94_bf(const void *const hw, hri_nvic_nvicip94_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP94.reg |= NVIC_NVICIP94_PRI94(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip94_reg_t hri_nvic_get_NVICIP94_PRI94_bf(const void *const hw, hri_nvic_nvicip94_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP94.reg;
	tmp = (tmp & NVIC_NVICIP94_PRI94(mask)) >> 0;
	return tmp;
}

static inline void hri_nvic_write_NVICIP94_PRI94_bf(const void *const hw, hri_nvic_nvicip94_reg_t data)
{
	uint8_t tmp;
	NVIC_CRITICAL_SECTION_ENTER();
	tmp = ((Nvic *)hw)->NVICIP94.reg;
	tmp &= ~NVIC_NVICIP94_PRI94_Msk;
	tmp |= NVIC_NVICIP94_PRI94(data);
	((Nvic *)hw)->NVICIP94.reg = tmp;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP94_PRI94_bf(const void *const hw, hri_nvic_nvicip94_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP94.reg &= ~NVIC_NVICIP94_PRI94(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP94_PRI94_bf(const void *const hw, hri_nvic_nvicip94_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP94.reg ^= NVIC_NVICIP94_PRI94(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip94_reg_t hri_nvic_read_NVICIP94_PRI94_bf(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP94.reg;
	tmp = (tmp & NVIC_NVICIP94_PRI94_Msk) >> 0;
	return tmp;
}

static inline void hri_nvic_set_NVICIP94_reg(const void *const hw, hri_nvic_nvicip94_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP94.reg |= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip94_reg_t hri_nvic_get_NVICIP94_reg(const void *const hw, hri_nvic_nvicip94_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP94.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_nvic_write_NVICIP94_reg(const void *const hw, hri_nvic_nvicip94_reg_t data)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP94.reg = data;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP94_reg(const void *const hw, hri_nvic_nvicip94_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP94.reg &= ~mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP94_reg(const void *const hw, hri_nvic_nvicip94_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP94.reg ^= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip94_reg_t hri_nvic_read_NVICIP94_reg(const void *const hw)
{
	return ((Nvic *)hw)->NVICIP94.reg;
}

static inline void hri_nvic_set_NVICIP95_PRI95_bf(const void *const hw, hri_nvic_nvicip95_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP95.reg |= NVIC_NVICIP95_PRI95(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip95_reg_t hri_nvic_get_NVICIP95_PRI95_bf(const void *const hw, hri_nvic_nvicip95_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP95.reg;
	tmp = (tmp & NVIC_NVICIP95_PRI95(mask)) >> 0;
	return tmp;
}

static inline void hri_nvic_write_NVICIP95_PRI95_bf(const void *const hw, hri_nvic_nvicip95_reg_t data)
{
	uint8_t tmp;
	NVIC_CRITICAL_SECTION_ENTER();
	tmp = ((Nvic *)hw)->NVICIP95.reg;
	tmp &= ~NVIC_NVICIP95_PRI95_Msk;
	tmp |= NVIC_NVICIP95_PRI95(data);
	((Nvic *)hw)->NVICIP95.reg = tmp;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP95_PRI95_bf(const void *const hw, hri_nvic_nvicip95_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP95.reg &= ~NVIC_NVICIP95_PRI95(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP95_PRI95_bf(const void *const hw, hri_nvic_nvicip95_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP95.reg ^= NVIC_NVICIP95_PRI95(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip95_reg_t hri_nvic_read_NVICIP95_PRI95_bf(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP95.reg;
	tmp = (tmp & NVIC_NVICIP95_PRI95_Msk) >> 0;
	return tmp;
}

static inline void hri_nvic_set_NVICIP95_reg(const void *const hw, hri_nvic_nvicip95_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP95.reg |= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip95_reg_t hri_nvic_get_NVICIP95_reg(const void *const hw, hri_nvic_nvicip95_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP95.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_nvic_write_NVICIP95_reg(const void *const hw, hri_nvic_nvicip95_reg_t data)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP95.reg = data;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP95_reg(const void *const hw, hri_nvic_nvicip95_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP95.reg &= ~mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP95_reg(const void *const hw, hri_nvic_nvicip95_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP95.reg ^= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip95_reg_t hri_nvic_read_NVICIP95_reg(const void *const hw)
{
	return ((Nvic *)hw)->NVICIP95.reg;
}

static inline void hri_nvic_set_NVICIP96_PRI96_bf(const void *const hw, hri_nvic_nvicip96_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP96.reg |= NVIC_NVICIP96_PRI96(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip96_reg_t hri_nvic_get_NVICIP96_PRI96_bf(const void *const hw, hri_nvic_nvicip96_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP96.reg;
	tmp = (tmp & NVIC_NVICIP96_PRI96(mask)) >> 0;
	return tmp;
}

static inline void hri_nvic_write_NVICIP96_PRI96_bf(const void *const hw, hri_nvic_nvicip96_reg_t data)
{
	uint8_t tmp;
	NVIC_CRITICAL_SECTION_ENTER();
	tmp = ((Nvic *)hw)->NVICIP96.reg;
	tmp &= ~NVIC_NVICIP96_PRI96_Msk;
	tmp |= NVIC_NVICIP96_PRI96(data);
	((Nvic *)hw)->NVICIP96.reg = tmp;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP96_PRI96_bf(const void *const hw, hri_nvic_nvicip96_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP96.reg &= ~NVIC_NVICIP96_PRI96(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP96_PRI96_bf(const void *const hw, hri_nvic_nvicip96_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP96.reg ^= NVIC_NVICIP96_PRI96(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip96_reg_t hri_nvic_read_NVICIP96_PRI96_bf(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP96.reg;
	tmp = (tmp & NVIC_NVICIP96_PRI96_Msk) >> 0;
	return tmp;
}

static inline void hri_nvic_set_NVICIP96_reg(const void *const hw, hri_nvic_nvicip96_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP96.reg |= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip96_reg_t hri_nvic_get_NVICIP96_reg(const void *const hw, hri_nvic_nvicip96_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP96.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_nvic_write_NVICIP96_reg(const void *const hw, hri_nvic_nvicip96_reg_t data)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP96.reg = data;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP96_reg(const void *const hw, hri_nvic_nvicip96_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP96.reg &= ~mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP96_reg(const void *const hw, hri_nvic_nvicip96_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP96.reg ^= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip96_reg_t hri_nvic_read_NVICIP96_reg(const void *const hw)
{
	return ((Nvic *)hw)->NVICIP96.reg;
}

static inline void hri_nvic_set_NVICIP97_PRI97_bf(const void *const hw, hri_nvic_nvicip97_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP97.reg |= NVIC_NVICIP97_PRI97(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip97_reg_t hri_nvic_get_NVICIP97_PRI97_bf(const void *const hw, hri_nvic_nvicip97_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP97.reg;
	tmp = (tmp & NVIC_NVICIP97_PRI97(mask)) >> 0;
	return tmp;
}

static inline void hri_nvic_write_NVICIP97_PRI97_bf(const void *const hw, hri_nvic_nvicip97_reg_t data)
{
	uint8_t tmp;
	NVIC_CRITICAL_SECTION_ENTER();
	tmp = ((Nvic *)hw)->NVICIP97.reg;
	tmp &= ~NVIC_NVICIP97_PRI97_Msk;
	tmp |= NVIC_NVICIP97_PRI97(data);
	((Nvic *)hw)->NVICIP97.reg = tmp;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP97_PRI97_bf(const void *const hw, hri_nvic_nvicip97_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP97.reg &= ~NVIC_NVICIP97_PRI97(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP97_PRI97_bf(const void *const hw, hri_nvic_nvicip97_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP97.reg ^= NVIC_NVICIP97_PRI97(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip97_reg_t hri_nvic_read_NVICIP97_PRI97_bf(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP97.reg;
	tmp = (tmp & NVIC_NVICIP97_PRI97_Msk) >> 0;
	return tmp;
}

static inline void hri_nvic_set_NVICIP97_reg(const void *const hw, hri_nvic_nvicip97_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP97.reg |= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip97_reg_t hri_nvic_get_NVICIP97_reg(const void *const hw, hri_nvic_nvicip97_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP97.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_nvic_write_NVICIP97_reg(const void *const hw, hri_nvic_nvicip97_reg_t data)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP97.reg = data;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP97_reg(const void *const hw, hri_nvic_nvicip97_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP97.reg &= ~mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP97_reg(const void *const hw, hri_nvic_nvicip97_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP97.reg ^= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip97_reg_t hri_nvic_read_NVICIP97_reg(const void *const hw)
{
	return ((Nvic *)hw)->NVICIP97.reg;
}

static inline void hri_nvic_set_NVICIP98_PRI98_bf(const void *const hw, hri_nvic_nvicip98_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP98.reg |= NVIC_NVICIP98_PRI98(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip98_reg_t hri_nvic_get_NVICIP98_PRI98_bf(const void *const hw, hri_nvic_nvicip98_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP98.reg;
	tmp = (tmp & NVIC_NVICIP98_PRI98(mask)) >> 0;
	return tmp;
}

static inline void hri_nvic_write_NVICIP98_PRI98_bf(const void *const hw, hri_nvic_nvicip98_reg_t data)
{
	uint8_t tmp;
	NVIC_CRITICAL_SECTION_ENTER();
	tmp = ((Nvic *)hw)->NVICIP98.reg;
	tmp &= ~NVIC_NVICIP98_PRI98_Msk;
	tmp |= NVIC_NVICIP98_PRI98(data);
	((Nvic *)hw)->NVICIP98.reg = tmp;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP98_PRI98_bf(const void *const hw, hri_nvic_nvicip98_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP98.reg &= ~NVIC_NVICIP98_PRI98(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP98_PRI98_bf(const void *const hw, hri_nvic_nvicip98_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP98.reg ^= NVIC_NVICIP98_PRI98(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip98_reg_t hri_nvic_read_NVICIP98_PRI98_bf(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP98.reg;
	tmp = (tmp & NVIC_NVICIP98_PRI98_Msk) >> 0;
	return tmp;
}

static inline void hri_nvic_set_NVICIP98_reg(const void *const hw, hri_nvic_nvicip98_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP98.reg |= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip98_reg_t hri_nvic_get_NVICIP98_reg(const void *const hw, hri_nvic_nvicip98_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP98.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_nvic_write_NVICIP98_reg(const void *const hw, hri_nvic_nvicip98_reg_t data)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP98.reg = data;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP98_reg(const void *const hw, hri_nvic_nvicip98_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP98.reg &= ~mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP98_reg(const void *const hw, hri_nvic_nvicip98_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP98.reg ^= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip98_reg_t hri_nvic_read_NVICIP98_reg(const void *const hw)
{
	return ((Nvic *)hw)->NVICIP98.reg;
}

static inline void hri_nvic_set_NVICIP99_PRI99_bf(const void *const hw, hri_nvic_nvicip99_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP99.reg |= NVIC_NVICIP99_PRI99(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip99_reg_t hri_nvic_get_NVICIP99_PRI99_bf(const void *const hw, hri_nvic_nvicip99_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP99.reg;
	tmp = (tmp & NVIC_NVICIP99_PRI99(mask)) >> 0;
	return tmp;
}

static inline void hri_nvic_write_NVICIP99_PRI99_bf(const void *const hw, hri_nvic_nvicip99_reg_t data)
{
	uint8_t tmp;
	NVIC_CRITICAL_SECTION_ENTER();
	tmp = ((Nvic *)hw)->NVICIP99.reg;
	tmp &= ~NVIC_NVICIP99_PRI99_Msk;
	tmp |= NVIC_NVICIP99_PRI99(data);
	((Nvic *)hw)->NVICIP99.reg = tmp;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP99_PRI99_bf(const void *const hw, hri_nvic_nvicip99_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP99.reg &= ~NVIC_NVICIP99_PRI99(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP99_PRI99_bf(const void *const hw, hri_nvic_nvicip99_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP99.reg ^= NVIC_NVICIP99_PRI99(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip99_reg_t hri_nvic_read_NVICIP99_PRI99_bf(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP99.reg;
	tmp = (tmp & NVIC_NVICIP99_PRI99_Msk) >> 0;
	return tmp;
}

static inline void hri_nvic_set_NVICIP99_reg(const void *const hw, hri_nvic_nvicip99_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP99.reg |= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip99_reg_t hri_nvic_get_NVICIP99_reg(const void *const hw, hri_nvic_nvicip99_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP99.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_nvic_write_NVICIP99_reg(const void *const hw, hri_nvic_nvicip99_reg_t data)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP99.reg = data;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP99_reg(const void *const hw, hri_nvic_nvicip99_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP99.reg &= ~mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP99_reg(const void *const hw, hri_nvic_nvicip99_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP99.reg ^= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip99_reg_t hri_nvic_read_NVICIP99_reg(const void *const hw)
{
	return ((Nvic *)hw)->NVICIP99.reg;
}

static inline void hri_nvic_set_NVICIP100_PRI100_bf(const void *const hw, hri_nvic_nvicip100_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP100.reg |= NVIC_NVICIP100_PRI100(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip100_reg_t hri_nvic_get_NVICIP100_PRI100_bf(const void *const        hw,
                                                                        hri_nvic_nvicip100_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP100.reg;
	tmp = (tmp & NVIC_NVICIP100_PRI100(mask)) >> 0;
	return tmp;
}

static inline void hri_nvic_write_NVICIP100_PRI100_bf(const void *const hw, hri_nvic_nvicip100_reg_t data)
{
	uint8_t tmp;
	NVIC_CRITICAL_SECTION_ENTER();
	tmp = ((Nvic *)hw)->NVICIP100.reg;
	tmp &= ~NVIC_NVICIP100_PRI100_Msk;
	tmp |= NVIC_NVICIP100_PRI100(data);
	((Nvic *)hw)->NVICIP100.reg = tmp;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP100_PRI100_bf(const void *const hw, hri_nvic_nvicip100_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP100.reg &= ~NVIC_NVICIP100_PRI100(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP100_PRI100_bf(const void *const hw, hri_nvic_nvicip100_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP100.reg ^= NVIC_NVICIP100_PRI100(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip100_reg_t hri_nvic_read_NVICIP100_PRI100_bf(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP100.reg;
	tmp = (tmp & NVIC_NVICIP100_PRI100_Msk) >> 0;
	return tmp;
}

static inline void hri_nvic_set_NVICIP100_reg(const void *const hw, hri_nvic_nvicip100_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP100.reg |= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip100_reg_t hri_nvic_get_NVICIP100_reg(const void *const hw, hri_nvic_nvicip100_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP100.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_nvic_write_NVICIP100_reg(const void *const hw, hri_nvic_nvicip100_reg_t data)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP100.reg = data;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP100_reg(const void *const hw, hri_nvic_nvicip100_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP100.reg &= ~mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP100_reg(const void *const hw, hri_nvic_nvicip100_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP100.reg ^= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip100_reg_t hri_nvic_read_NVICIP100_reg(const void *const hw)
{
	return ((Nvic *)hw)->NVICIP100.reg;
}

static inline void hri_nvic_set_NVICIP101_PRI101_bf(const void *const hw, hri_nvic_nvicip101_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP101.reg |= NVIC_NVICIP101_PRI101(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip101_reg_t hri_nvic_get_NVICIP101_PRI101_bf(const void *const        hw,
                                                                        hri_nvic_nvicip101_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP101.reg;
	tmp = (tmp & NVIC_NVICIP101_PRI101(mask)) >> 0;
	return tmp;
}

static inline void hri_nvic_write_NVICIP101_PRI101_bf(const void *const hw, hri_nvic_nvicip101_reg_t data)
{
	uint8_t tmp;
	NVIC_CRITICAL_SECTION_ENTER();
	tmp = ((Nvic *)hw)->NVICIP101.reg;
	tmp &= ~NVIC_NVICIP101_PRI101_Msk;
	tmp |= NVIC_NVICIP101_PRI101(data);
	((Nvic *)hw)->NVICIP101.reg = tmp;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP101_PRI101_bf(const void *const hw, hri_nvic_nvicip101_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP101.reg &= ~NVIC_NVICIP101_PRI101(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP101_PRI101_bf(const void *const hw, hri_nvic_nvicip101_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP101.reg ^= NVIC_NVICIP101_PRI101(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip101_reg_t hri_nvic_read_NVICIP101_PRI101_bf(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP101.reg;
	tmp = (tmp & NVIC_NVICIP101_PRI101_Msk) >> 0;
	return tmp;
}

static inline void hri_nvic_set_NVICIP101_reg(const void *const hw, hri_nvic_nvicip101_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP101.reg |= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip101_reg_t hri_nvic_get_NVICIP101_reg(const void *const hw, hri_nvic_nvicip101_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP101.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_nvic_write_NVICIP101_reg(const void *const hw, hri_nvic_nvicip101_reg_t data)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP101.reg = data;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP101_reg(const void *const hw, hri_nvic_nvicip101_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP101.reg &= ~mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP101_reg(const void *const hw, hri_nvic_nvicip101_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP101.reg ^= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip101_reg_t hri_nvic_read_NVICIP101_reg(const void *const hw)
{
	return ((Nvic *)hw)->NVICIP101.reg;
}

static inline void hri_nvic_set_NVICIP102_PRI102_bf(const void *const hw, hri_nvic_nvicip102_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP102.reg |= NVIC_NVICIP102_PRI102(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip102_reg_t hri_nvic_get_NVICIP102_PRI102_bf(const void *const        hw,
                                                                        hri_nvic_nvicip102_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP102.reg;
	tmp = (tmp & NVIC_NVICIP102_PRI102(mask)) >> 0;
	return tmp;
}

static inline void hri_nvic_write_NVICIP102_PRI102_bf(const void *const hw, hri_nvic_nvicip102_reg_t data)
{
	uint8_t tmp;
	NVIC_CRITICAL_SECTION_ENTER();
	tmp = ((Nvic *)hw)->NVICIP102.reg;
	tmp &= ~NVIC_NVICIP102_PRI102_Msk;
	tmp |= NVIC_NVICIP102_PRI102(data);
	((Nvic *)hw)->NVICIP102.reg = tmp;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP102_PRI102_bf(const void *const hw, hri_nvic_nvicip102_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP102.reg &= ~NVIC_NVICIP102_PRI102(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP102_PRI102_bf(const void *const hw, hri_nvic_nvicip102_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP102.reg ^= NVIC_NVICIP102_PRI102(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip102_reg_t hri_nvic_read_NVICIP102_PRI102_bf(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP102.reg;
	tmp = (tmp & NVIC_NVICIP102_PRI102_Msk) >> 0;
	return tmp;
}

static inline void hri_nvic_set_NVICIP102_reg(const void *const hw, hri_nvic_nvicip102_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP102.reg |= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip102_reg_t hri_nvic_get_NVICIP102_reg(const void *const hw, hri_nvic_nvicip102_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP102.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_nvic_write_NVICIP102_reg(const void *const hw, hri_nvic_nvicip102_reg_t data)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP102.reg = data;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP102_reg(const void *const hw, hri_nvic_nvicip102_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP102.reg &= ~mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP102_reg(const void *const hw, hri_nvic_nvicip102_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP102.reg ^= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip102_reg_t hri_nvic_read_NVICIP102_reg(const void *const hw)
{
	return ((Nvic *)hw)->NVICIP102.reg;
}

static inline void hri_nvic_set_NVICIP103_PRI103_bf(const void *const hw, hri_nvic_nvicip103_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP103.reg |= NVIC_NVICIP103_PRI103(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip103_reg_t hri_nvic_get_NVICIP103_PRI103_bf(const void *const        hw,
                                                                        hri_nvic_nvicip103_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP103.reg;
	tmp = (tmp & NVIC_NVICIP103_PRI103(mask)) >> 0;
	return tmp;
}

static inline void hri_nvic_write_NVICIP103_PRI103_bf(const void *const hw, hri_nvic_nvicip103_reg_t data)
{
	uint8_t tmp;
	NVIC_CRITICAL_SECTION_ENTER();
	tmp = ((Nvic *)hw)->NVICIP103.reg;
	tmp &= ~NVIC_NVICIP103_PRI103_Msk;
	tmp |= NVIC_NVICIP103_PRI103(data);
	((Nvic *)hw)->NVICIP103.reg = tmp;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP103_PRI103_bf(const void *const hw, hri_nvic_nvicip103_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP103.reg &= ~NVIC_NVICIP103_PRI103(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP103_PRI103_bf(const void *const hw, hri_nvic_nvicip103_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP103.reg ^= NVIC_NVICIP103_PRI103(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip103_reg_t hri_nvic_read_NVICIP103_PRI103_bf(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP103.reg;
	tmp = (tmp & NVIC_NVICIP103_PRI103_Msk) >> 0;
	return tmp;
}

static inline void hri_nvic_set_NVICIP103_reg(const void *const hw, hri_nvic_nvicip103_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP103.reg |= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip103_reg_t hri_nvic_get_NVICIP103_reg(const void *const hw, hri_nvic_nvicip103_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP103.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_nvic_write_NVICIP103_reg(const void *const hw, hri_nvic_nvicip103_reg_t data)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP103.reg = data;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP103_reg(const void *const hw, hri_nvic_nvicip103_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP103.reg &= ~mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP103_reg(const void *const hw, hri_nvic_nvicip103_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP103.reg ^= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip103_reg_t hri_nvic_read_NVICIP103_reg(const void *const hw)
{
	return ((Nvic *)hw)->NVICIP103.reg;
}

static inline void hri_nvic_set_NVICIP104_PRI104_bf(const void *const hw, hri_nvic_nvicip104_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP104.reg |= NVIC_NVICIP104_PRI104(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip104_reg_t hri_nvic_get_NVICIP104_PRI104_bf(const void *const        hw,
                                                                        hri_nvic_nvicip104_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP104.reg;
	tmp = (tmp & NVIC_NVICIP104_PRI104(mask)) >> 0;
	return tmp;
}

static inline void hri_nvic_write_NVICIP104_PRI104_bf(const void *const hw, hri_nvic_nvicip104_reg_t data)
{
	uint8_t tmp;
	NVIC_CRITICAL_SECTION_ENTER();
	tmp = ((Nvic *)hw)->NVICIP104.reg;
	tmp &= ~NVIC_NVICIP104_PRI104_Msk;
	tmp |= NVIC_NVICIP104_PRI104(data);
	((Nvic *)hw)->NVICIP104.reg = tmp;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP104_PRI104_bf(const void *const hw, hri_nvic_nvicip104_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP104.reg &= ~NVIC_NVICIP104_PRI104(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP104_PRI104_bf(const void *const hw, hri_nvic_nvicip104_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP104.reg ^= NVIC_NVICIP104_PRI104(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip104_reg_t hri_nvic_read_NVICIP104_PRI104_bf(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP104.reg;
	tmp = (tmp & NVIC_NVICIP104_PRI104_Msk) >> 0;
	return tmp;
}

static inline void hri_nvic_set_NVICIP104_reg(const void *const hw, hri_nvic_nvicip104_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP104.reg |= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip104_reg_t hri_nvic_get_NVICIP104_reg(const void *const hw, hri_nvic_nvicip104_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP104.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_nvic_write_NVICIP104_reg(const void *const hw, hri_nvic_nvicip104_reg_t data)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP104.reg = data;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP104_reg(const void *const hw, hri_nvic_nvicip104_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP104.reg &= ~mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP104_reg(const void *const hw, hri_nvic_nvicip104_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP104.reg ^= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip104_reg_t hri_nvic_read_NVICIP104_reg(const void *const hw)
{
	return ((Nvic *)hw)->NVICIP104.reg;
}

static inline void hri_nvic_set_NVICIP105_PRI105_bf(const void *const hw, hri_nvic_nvicip105_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP105.reg |= NVIC_NVICIP105_PRI105(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip105_reg_t hri_nvic_get_NVICIP105_PRI105_bf(const void *const        hw,
                                                                        hri_nvic_nvicip105_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP105.reg;
	tmp = (tmp & NVIC_NVICIP105_PRI105(mask)) >> 0;
	return tmp;
}

static inline void hri_nvic_write_NVICIP105_PRI105_bf(const void *const hw, hri_nvic_nvicip105_reg_t data)
{
	uint8_t tmp;
	NVIC_CRITICAL_SECTION_ENTER();
	tmp = ((Nvic *)hw)->NVICIP105.reg;
	tmp &= ~NVIC_NVICIP105_PRI105_Msk;
	tmp |= NVIC_NVICIP105_PRI105(data);
	((Nvic *)hw)->NVICIP105.reg = tmp;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP105_PRI105_bf(const void *const hw, hri_nvic_nvicip105_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP105.reg &= ~NVIC_NVICIP105_PRI105(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP105_PRI105_bf(const void *const hw, hri_nvic_nvicip105_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP105.reg ^= NVIC_NVICIP105_PRI105(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip105_reg_t hri_nvic_read_NVICIP105_PRI105_bf(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP105.reg;
	tmp = (tmp & NVIC_NVICIP105_PRI105_Msk) >> 0;
	return tmp;
}

static inline void hri_nvic_set_NVICIP105_reg(const void *const hw, hri_nvic_nvicip105_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP105.reg |= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip105_reg_t hri_nvic_get_NVICIP105_reg(const void *const hw, hri_nvic_nvicip105_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvic *)hw)->NVICIP105.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_nvic_write_NVICIP105_reg(const void *const hw, hri_nvic_nvicip105_reg_t data)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP105.reg = data;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICIP105_reg(const void *const hw, hri_nvic_nvicip105_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP105.reg &= ~mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICIP105_reg(const void *const hw, hri_nvic_nvicip105_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICIP105.reg ^= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicip105_reg_t hri_nvic_read_NVICIP105_reg(const void *const hw)
{
	return ((Nvic *)hw)->NVICIP105.reg;
}

static inline void hri_nvic_set_NVICSTIR_INTID_bf(const void *const hw, hri_nvic_nvicstir_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICSTIR.reg |= NVIC_NVICSTIR_INTID(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicstir_reg_t hri_nvic_get_NVICSTIR_INTID_bf(const void *const hw, hri_nvic_nvicstir_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Nvic *)hw)->NVICSTIR.reg;
	tmp = (tmp & NVIC_NVICSTIR_INTID(mask)) >> 0;
	return tmp;
}

static inline void hri_nvic_write_NVICSTIR_INTID_bf(const void *const hw, hri_nvic_nvicstir_reg_t data)
{
	uint32_t tmp;
	NVIC_CRITICAL_SECTION_ENTER();
	tmp = ((Nvic *)hw)->NVICSTIR.reg;
	tmp &= ~NVIC_NVICSTIR_INTID_Msk;
	tmp |= NVIC_NVICSTIR_INTID(data);
	((Nvic *)hw)->NVICSTIR.reg = tmp;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICSTIR_INTID_bf(const void *const hw, hri_nvic_nvicstir_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICSTIR.reg &= ~NVIC_NVICSTIR_INTID(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICSTIR_INTID_bf(const void *const hw, hri_nvic_nvicstir_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICSTIR.reg ^= NVIC_NVICSTIR_INTID(mask);
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicstir_reg_t hri_nvic_read_NVICSTIR_INTID_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Nvic *)hw)->NVICSTIR.reg;
	tmp = (tmp & NVIC_NVICSTIR_INTID_Msk) >> 0;
	return tmp;
}

static inline void hri_nvic_set_NVICSTIR_reg(const void *const hw, hri_nvic_nvicstir_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICSTIR.reg |= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicstir_reg_t hri_nvic_get_NVICSTIR_reg(const void *const hw, hri_nvic_nvicstir_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Nvic *)hw)->NVICSTIR.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_nvic_write_NVICSTIR_reg(const void *const hw, hri_nvic_nvicstir_reg_t data)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICSTIR.reg = data;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_NVICSTIR_reg(const void *const hw, hri_nvic_nvicstir_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICSTIR.reg &= ~mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_NVICSTIR_reg(const void *const hw, hri_nvic_nvicstir_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->NVICSTIR.reg ^= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_nvicstir_reg_t hri_nvic_read_NVICSTIR_reg(const void *const hw)
{
	return ((Nvic *)hw)->NVICSTIR.reg;
}

#ifdef __cplusplus
}
#endif

#endif /* _HRI_NVIC_E54_H_INCLUDED */
#endif /* _SAME54_NVIC_COMPONENT_ */
