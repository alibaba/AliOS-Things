/**
 * \file
 *
 * \brief SAM NVIC
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

#ifdef _SAML21_NVIC_COMPONENT_
#ifndef _HRI_NVIC_L21_H_INCLUDED_
#define _HRI_NVIC_L21_H_INCLUDED_

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

typedef uint32_t hri_nvic_icer_reg_t;
typedef uint32_t hri_nvic_icpr_reg_t;
typedef uint32_t hri_nvic_ipr_reg_t;
typedef uint32_t hri_nvic_iser_reg_t;
typedef uint32_t hri_nvic_ispr_reg_t;

static inline void hri_nvic_set_ISER_reg(const void *const hw, hri_nvic_iser_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->ISER.reg |= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_iser_reg_t hri_nvic_get_ISER_reg(const void *const hw, hri_nvic_iser_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Nvic *)hw)->ISER.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_nvic_write_ISER_reg(const void *const hw, hri_nvic_iser_reg_t data)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->ISER.reg = data;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_ISER_reg(const void *const hw, hri_nvic_iser_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->ISER.reg &= ~mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_ISER_reg(const void *const hw, hri_nvic_iser_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->ISER.reg ^= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_iser_reg_t hri_nvic_read_ISER_reg(const void *const hw)
{
	return ((Nvic *)hw)->ISER.reg;
}

static inline void hri_nvic_set_ICER_reg(const void *const hw, hri_nvic_icer_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->ICER.reg |= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_icer_reg_t hri_nvic_get_ICER_reg(const void *const hw, hri_nvic_icer_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Nvic *)hw)->ICER.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_nvic_write_ICER_reg(const void *const hw, hri_nvic_icer_reg_t data)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->ICER.reg = data;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_ICER_reg(const void *const hw, hri_nvic_icer_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->ICER.reg &= ~mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_ICER_reg(const void *const hw, hri_nvic_icer_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->ICER.reg ^= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_icer_reg_t hri_nvic_read_ICER_reg(const void *const hw)
{
	return ((Nvic *)hw)->ICER.reg;
}

static inline void hri_nvic_set_ISPR_reg(const void *const hw, hri_nvic_ispr_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->ISPR.reg |= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_ispr_reg_t hri_nvic_get_ISPR_reg(const void *const hw, hri_nvic_ispr_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Nvic *)hw)->ISPR.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_nvic_write_ISPR_reg(const void *const hw, hri_nvic_ispr_reg_t data)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->ISPR.reg = data;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_ISPR_reg(const void *const hw, hri_nvic_ispr_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->ISPR.reg &= ~mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_ISPR_reg(const void *const hw, hri_nvic_ispr_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->ISPR.reg ^= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_ispr_reg_t hri_nvic_read_ISPR_reg(const void *const hw)
{
	return ((Nvic *)hw)->ISPR.reg;
}

static inline void hri_nvic_set_ICPR_reg(const void *const hw, hri_nvic_icpr_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->ICPR.reg |= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_icpr_reg_t hri_nvic_get_ICPR_reg(const void *const hw, hri_nvic_icpr_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Nvic *)hw)->ICPR.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_nvic_write_ICPR_reg(const void *const hw, hri_nvic_icpr_reg_t data)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->ICPR.reg = data;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_ICPR_reg(const void *const hw, hri_nvic_icpr_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->ICPR.reg &= ~mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_ICPR_reg(const void *const hw, hri_nvic_icpr_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->ICPR.reg ^= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_icpr_reg_t hri_nvic_read_ICPR_reg(const void *const hw)
{
	return ((Nvic *)hw)->ICPR.reg;
}

static inline void hri_nvic_set_IPR_reg(const void *const hw, uint8_t index, hri_nvic_ipr_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->IPR[index].reg |= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_ipr_reg_t hri_nvic_get_IPR_reg(const void *const hw, uint8_t index, hri_nvic_ipr_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Nvic *)hw)->IPR[index].reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_nvic_write_IPR_reg(const void *const hw, uint8_t index, hri_nvic_ipr_reg_t data)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->IPR[index].reg = data;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_clear_IPR_reg(const void *const hw, uint8_t index, hri_nvic_ipr_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->IPR[index].reg &= ~mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvic_toggle_IPR_reg(const void *const hw, uint8_t index, hri_nvic_ipr_reg_t mask)
{
	NVIC_CRITICAL_SECTION_ENTER();
	((Nvic *)hw)->IPR[index].reg ^= mask;
	NVIC_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvic_ipr_reg_t hri_nvic_read_IPR_reg(const void *const hw, uint8_t index)
{
	return ((Nvic *)hw)->IPR[index].reg;
}

#ifdef __cplusplus
}
#endif

#endif /* _HRI_NVIC_L21_H_INCLUDED */
#endif /* _SAML21_NVIC_COMPONENT_ */
