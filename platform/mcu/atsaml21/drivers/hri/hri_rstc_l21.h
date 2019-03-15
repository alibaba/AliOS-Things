/**
 * \file
 *
 * \brief SAM RSTC
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

#ifdef _SAML21_RSTC_COMPONENT_
#ifndef _HRI_RSTC_L21_H_INCLUDED_
#define _HRI_RSTC_L21_H_INCLUDED_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <hal_atomic.h>

#if defined(ENABLE_RSTC_CRITICAL_SECTIONS)
#define RSTC_CRITICAL_SECTION_ENTER() CRITICAL_SECTION_ENTER()
#define RSTC_CRITICAL_SECTION_LEAVE() CRITICAL_SECTION_LEAVE()
#else
#define RSTC_CRITICAL_SECTION_ENTER()
#define RSTC_CRITICAL_SECTION_LEAVE()
#endif

typedef uint16_t hri_rstc_wkcause_reg_t;
typedef uint16_t hri_rstc_wken_reg_t;
typedef uint16_t hri_rstc_wkpol_reg_t;
typedef uint8_t  hri_rstc_bkupexit_reg_t;
typedef uint8_t  hri_rstc_rcause_reg_t;
typedef uint8_t  hri_rstc_wkdbconf_reg_t;

static inline bool hri_rstc_get_RCAUSE_POR_bit(const void *const hw)
{
	return (((Rstc *)hw)->RCAUSE.reg & RSTC_RCAUSE_POR) >> RSTC_RCAUSE_POR_Pos;
}

static inline bool hri_rstc_get_RCAUSE_BOD12_bit(const void *const hw)
{
	return (((Rstc *)hw)->RCAUSE.reg & RSTC_RCAUSE_BOD12) >> RSTC_RCAUSE_BOD12_Pos;
}

static inline bool hri_rstc_get_RCAUSE_BOD33_bit(const void *const hw)
{
	return (((Rstc *)hw)->RCAUSE.reg & RSTC_RCAUSE_BOD33) >> RSTC_RCAUSE_BOD33_Pos;
}

static inline bool hri_rstc_get_RCAUSE_EXT_bit(const void *const hw)
{
	return (((Rstc *)hw)->RCAUSE.reg & RSTC_RCAUSE_EXT) >> RSTC_RCAUSE_EXT_Pos;
}

static inline bool hri_rstc_get_RCAUSE_WDT_bit(const void *const hw)
{
	return (((Rstc *)hw)->RCAUSE.reg & RSTC_RCAUSE_WDT) >> RSTC_RCAUSE_WDT_Pos;
}

static inline bool hri_rstc_get_RCAUSE_SYST_bit(const void *const hw)
{
	return (((Rstc *)hw)->RCAUSE.reg & RSTC_RCAUSE_SYST) >> RSTC_RCAUSE_SYST_Pos;
}

static inline bool hri_rstc_get_RCAUSE_BACKUP_bit(const void *const hw)
{
	return (((Rstc *)hw)->RCAUSE.reg & RSTC_RCAUSE_BACKUP) >> RSTC_RCAUSE_BACKUP_Pos;
}

static inline hri_rstc_rcause_reg_t hri_rstc_get_RCAUSE_reg(const void *const hw, hri_rstc_rcause_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Rstc *)hw)->RCAUSE.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_rstc_rcause_reg_t hri_rstc_read_RCAUSE_reg(const void *const hw)
{
	return ((Rstc *)hw)->RCAUSE.reg;
}

static inline bool hri_rstc_get_BKUPEXIT_EXTWAKE_bit(const void *const hw)
{
	return (((Rstc *)hw)->BKUPEXIT.reg & RSTC_BKUPEXIT_EXTWAKE) >> RSTC_BKUPEXIT_EXTWAKE_Pos;
}

static inline bool hri_rstc_get_BKUPEXIT_RTC_bit(const void *const hw)
{
	return (((Rstc *)hw)->BKUPEXIT.reg & RSTC_BKUPEXIT_RTC) >> RSTC_BKUPEXIT_RTC_Pos;
}

static inline bool hri_rstc_get_BKUPEXIT_BBPS_bit(const void *const hw)
{
	return (((Rstc *)hw)->BKUPEXIT.reg & RSTC_BKUPEXIT_BBPS) >> RSTC_BKUPEXIT_BBPS_Pos;
}

static inline hri_rstc_bkupexit_reg_t hri_rstc_get_BKUPEXIT_reg(const void *const hw, hri_rstc_bkupexit_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Rstc *)hw)->BKUPEXIT.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_rstc_bkupexit_reg_t hri_rstc_read_BKUPEXIT_reg(const void *const hw)
{
	return ((Rstc *)hw)->BKUPEXIT.reg;
}

static inline void hri_rstc_set_WKDBCONF_WKDBCNT_bf(const void *const hw, hri_rstc_wkdbconf_reg_t mask)
{
	RSTC_CRITICAL_SECTION_ENTER();
	((Rstc *)hw)->WKDBCONF.reg |= RSTC_WKDBCONF_WKDBCNT(mask);
	RSTC_CRITICAL_SECTION_LEAVE();
}

static inline hri_rstc_wkdbconf_reg_t hri_rstc_get_WKDBCONF_WKDBCNT_bf(const void *const       hw,
                                                                       hri_rstc_wkdbconf_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Rstc *)hw)->WKDBCONF.reg;
	tmp = (tmp & RSTC_WKDBCONF_WKDBCNT(mask)) >> RSTC_WKDBCONF_WKDBCNT_Pos;
	return tmp;
}

static inline void hri_rstc_write_WKDBCONF_WKDBCNT_bf(const void *const hw, hri_rstc_wkdbconf_reg_t data)
{
	uint8_t tmp;
	RSTC_CRITICAL_SECTION_ENTER();
	tmp = ((Rstc *)hw)->WKDBCONF.reg;
	tmp &= ~RSTC_WKDBCONF_WKDBCNT_Msk;
	tmp |= RSTC_WKDBCONF_WKDBCNT(data);
	((Rstc *)hw)->WKDBCONF.reg = tmp;
	RSTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rstc_clear_WKDBCONF_WKDBCNT_bf(const void *const hw, hri_rstc_wkdbconf_reg_t mask)
{
	RSTC_CRITICAL_SECTION_ENTER();
	((Rstc *)hw)->WKDBCONF.reg &= ~RSTC_WKDBCONF_WKDBCNT(mask);
	RSTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rstc_toggle_WKDBCONF_WKDBCNT_bf(const void *const hw, hri_rstc_wkdbconf_reg_t mask)
{
	RSTC_CRITICAL_SECTION_ENTER();
	((Rstc *)hw)->WKDBCONF.reg ^= RSTC_WKDBCONF_WKDBCNT(mask);
	RSTC_CRITICAL_SECTION_LEAVE();
}

static inline hri_rstc_wkdbconf_reg_t hri_rstc_read_WKDBCONF_WKDBCNT_bf(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Rstc *)hw)->WKDBCONF.reg;
	tmp = (tmp & RSTC_WKDBCONF_WKDBCNT_Msk) >> RSTC_WKDBCONF_WKDBCNT_Pos;
	return tmp;
}

static inline void hri_rstc_set_WKDBCONF_reg(const void *const hw, hri_rstc_wkdbconf_reg_t mask)
{
	RSTC_CRITICAL_SECTION_ENTER();
	((Rstc *)hw)->WKDBCONF.reg |= mask;
	RSTC_CRITICAL_SECTION_LEAVE();
}

static inline hri_rstc_wkdbconf_reg_t hri_rstc_get_WKDBCONF_reg(const void *const hw, hri_rstc_wkdbconf_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Rstc *)hw)->WKDBCONF.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_rstc_write_WKDBCONF_reg(const void *const hw, hri_rstc_wkdbconf_reg_t data)
{
	RSTC_CRITICAL_SECTION_ENTER();
	((Rstc *)hw)->WKDBCONF.reg = data;
	RSTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rstc_clear_WKDBCONF_reg(const void *const hw, hri_rstc_wkdbconf_reg_t mask)
{
	RSTC_CRITICAL_SECTION_ENTER();
	((Rstc *)hw)->WKDBCONF.reg &= ~mask;
	RSTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rstc_toggle_WKDBCONF_reg(const void *const hw, hri_rstc_wkdbconf_reg_t mask)
{
	RSTC_CRITICAL_SECTION_ENTER();
	((Rstc *)hw)->WKDBCONF.reg ^= mask;
	RSTC_CRITICAL_SECTION_LEAVE();
}

static inline hri_rstc_wkdbconf_reg_t hri_rstc_read_WKDBCONF_reg(const void *const hw)
{
	return ((Rstc *)hw)->WKDBCONF.reg;
}

static inline void hri_rstc_set_WKPOL_WKPOL_bf(const void *const hw, hri_rstc_wkpol_reg_t mask)
{
	RSTC_CRITICAL_SECTION_ENTER();
	((Rstc *)hw)->WKPOL.reg |= RSTC_WKPOL_WKPOL(mask);
	RSTC_CRITICAL_SECTION_LEAVE();
}

static inline hri_rstc_wkpol_reg_t hri_rstc_get_WKPOL_WKPOL_bf(const void *const hw, hri_rstc_wkpol_reg_t mask)
{
	uint16_t tmp;
	tmp = ((Rstc *)hw)->WKPOL.reg;
	tmp = (tmp & RSTC_WKPOL_WKPOL(mask)) >> RSTC_WKPOL_WKPOL_Pos;
	return tmp;
}

static inline void hri_rstc_write_WKPOL_WKPOL_bf(const void *const hw, hri_rstc_wkpol_reg_t data)
{
	uint16_t tmp;
	RSTC_CRITICAL_SECTION_ENTER();
	tmp = ((Rstc *)hw)->WKPOL.reg;
	tmp &= ~RSTC_WKPOL_WKPOL_Msk;
	tmp |= RSTC_WKPOL_WKPOL(data);
	((Rstc *)hw)->WKPOL.reg = tmp;
	RSTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rstc_clear_WKPOL_WKPOL_bf(const void *const hw, hri_rstc_wkpol_reg_t mask)
{
	RSTC_CRITICAL_SECTION_ENTER();
	((Rstc *)hw)->WKPOL.reg &= ~RSTC_WKPOL_WKPOL(mask);
	RSTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rstc_toggle_WKPOL_WKPOL_bf(const void *const hw, hri_rstc_wkpol_reg_t mask)
{
	RSTC_CRITICAL_SECTION_ENTER();
	((Rstc *)hw)->WKPOL.reg ^= RSTC_WKPOL_WKPOL(mask);
	RSTC_CRITICAL_SECTION_LEAVE();
}

static inline hri_rstc_wkpol_reg_t hri_rstc_read_WKPOL_WKPOL_bf(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Rstc *)hw)->WKPOL.reg;
	tmp = (tmp & RSTC_WKPOL_WKPOL_Msk) >> RSTC_WKPOL_WKPOL_Pos;
	return tmp;
}

static inline void hri_rstc_set_WKPOL_reg(const void *const hw, hri_rstc_wkpol_reg_t mask)
{
	RSTC_CRITICAL_SECTION_ENTER();
	((Rstc *)hw)->WKPOL.reg |= mask;
	RSTC_CRITICAL_SECTION_LEAVE();
}

static inline hri_rstc_wkpol_reg_t hri_rstc_get_WKPOL_reg(const void *const hw, hri_rstc_wkpol_reg_t mask)
{
	uint16_t tmp;
	tmp = ((Rstc *)hw)->WKPOL.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_rstc_write_WKPOL_reg(const void *const hw, hri_rstc_wkpol_reg_t data)
{
	RSTC_CRITICAL_SECTION_ENTER();
	((Rstc *)hw)->WKPOL.reg = data;
	RSTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rstc_clear_WKPOL_reg(const void *const hw, hri_rstc_wkpol_reg_t mask)
{
	RSTC_CRITICAL_SECTION_ENTER();
	((Rstc *)hw)->WKPOL.reg &= ~mask;
	RSTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rstc_toggle_WKPOL_reg(const void *const hw, hri_rstc_wkpol_reg_t mask)
{
	RSTC_CRITICAL_SECTION_ENTER();
	((Rstc *)hw)->WKPOL.reg ^= mask;
	RSTC_CRITICAL_SECTION_LEAVE();
}

static inline hri_rstc_wkpol_reg_t hri_rstc_read_WKPOL_reg(const void *const hw)
{
	return ((Rstc *)hw)->WKPOL.reg;
}

static inline void hri_rstc_set_WKEN_WKEN_bf(const void *const hw, hri_rstc_wken_reg_t mask)
{
	RSTC_CRITICAL_SECTION_ENTER();
	((Rstc *)hw)->WKEN.reg |= RSTC_WKEN_WKEN(mask);
	RSTC_CRITICAL_SECTION_LEAVE();
}

static inline hri_rstc_wken_reg_t hri_rstc_get_WKEN_WKEN_bf(const void *const hw, hri_rstc_wken_reg_t mask)
{
	uint16_t tmp;
	tmp = ((Rstc *)hw)->WKEN.reg;
	tmp = (tmp & RSTC_WKEN_WKEN(mask)) >> RSTC_WKEN_WKEN_Pos;
	return tmp;
}

static inline void hri_rstc_write_WKEN_WKEN_bf(const void *const hw, hri_rstc_wken_reg_t data)
{
	uint16_t tmp;
	RSTC_CRITICAL_SECTION_ENTER();
	tmp = ((Rstc *)hw)->WKEN.reg;
	tmp &= ~RSTC_WKEN_WKEN_Msk;
	tmp |= RSTC_WKEN_WKEN(data);
	((Rstc *)hw)->WKEN.reg = tmp;
	RSTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rstc_clear_WKEN_WKEN_bf(const void *const hw, hri_rstc_wken_reg_t mask)
{
	RSTC_CRITICAL_SECTION_ENTER();
	((Rstc *)hw)->WKEN.reg &= ~RSTC_WKEN_WKEN(mask);
	RSTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rstc_toggle_WKEN_WKEN_bf(const void *const hw, hri_rstc_wken_reg_t mask)
{
	RSTC_CRITICAL_SECTION_ENTER();
	((Rstc *)hw)->WKEN.reg ^= RSTC_WKEN_WKEN(mask);
	RSTC_CRITICAL_SECTION_LEAVE();
}

static inline hri_rstc_wken_reg_t hri_rstc_read_WKEN_WKEN_bf(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Rstc *)hw)->WKEN.reg;
	tmp = (tmp & RSTC_WKEN_WKEN_Msk) >> RSTC_WKEN_WKEN_Pos;
	return tmp;
}

static inline void hri_rstc_set_WKEN_reg(const void *const hw, hri_rstc_wken_reg_t mask)
{
	RSTC_CRITICAL_SECTION_ENTER();
	((Rstc *)hw)->WKEN.reg |= mask;
	RSTC_CRITICAL_SECTION_LEAVE();
}

static inline hri_rstc_wken_reg_t hri_rstc_get_WKEN_reg(const void *const hw, hri_rstc_wken_reg_t mask)
{
	uint16_t tmp;
	tmp = ((Rstc *)hw)->WKEN.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_rstc_write_WKEN_reg(const void *const hw, hri_rstc_wken_reg_t data)
{
	RSTC_CRITICAL_SECTION_ENTER();
	((Rstc *)hw)->WKEN.reg = data;
	RSTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rstc_clear_WKEN_reg(const void *const hw, hri_rstc_wken_reg_t mask)
{
	RSTC_CRITICAL_SECTION_ENTER();
	((Rstc *)hw)->WKEN.reg &= ~mask;
	RSTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rstc_toggle_WKEN_reg(const void *const hw, hri_rstc_wken_reg_t mask)
{
	RSTC_CRITICAL_SECTION_ENTER();
	((Rstc *)hw)->WKEN.reg ^= mask;
	RSTC_CRITICAL_SECTION_LEAVE();
}

static inline hri_rstc_wken_reg_t hri_rstc_read_WKEN_reg(const void *const hw)
{
	return ((Rstc *)hw)->WKEN.reg;
}

static inline void hri_rstc_set_WKCAUSE_WKCAUSE_bf(const void *const hw, hri_rstc_wkcause_reg_t mask)
{
	RSTC_CRITICAL_SECTION_ENTER();
	((Rstc *)hw)->WKCAUSE.reg |= RSTC_WKCAUSE_WKCAUSE(mask);
	RSTC_CRITICAL_SECTION_LEAVE();
}

static inline hri_rstc_wkcause_reg_t hri_rstc_get_WKCAUSE_WKCAUSE_bf(const void *const hw, hri_rstc_wkcause_reg_t mask)
{
	uint16_t tmp;
	tmp = ((Rstc *)hw)->WKCAUSE.reg;
	tmp = (tmp & RSTC_WKCAUSE_WKCAUSE(mask)) >> RSTC_WKCAUSE_WKCAUSE_Pos;
	return tmp;
}

static inline void hri_rstc_write_WKCAUSE_WKCAUSE_bf(const void *const hw, hri_rstc_wkcause_reg_t data)
{
	uint16_t tmp;
	RSTC_CRITICAL_SECTION_ENTER();
	tmp = ((Rstc *)hw)->WKCAUSE.reg;
	tmp &= ~RSTC_WKCAUSE_WKCAUSE_Msk;
	tmp |= RSTC_WKCAUSE_WKCAUSE(data);
	((Rstc *)hw)->WKCAUSE.reg = tmp;
	RSTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rstc_clear_WKCAUSE_WKCAUSE_bf(const void *const hw, hri_rstc_wkcause_reg_t mask)
{
	RSTC_CRITICAL_SECTION_ENTER();
	((Rstc *)hw)->WKCAUSE.reg &= ~RSTC_WKCAUSE_WKCAUSE(mask);
	RSTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rstc_toggle_WKCAUSE_WKCAUSE_bf(const void *const hw, hri_rstc_wkcause_reg_t mask)
{
	RSTC_CRITICAL_SECTION_ENTER();
	((Rstc *)hw)->WKCAUSE.reg ^= RSTC_WKCAUSE_WKCAUSE(mask);
	RSTC_CRITICAL_SECTION_LEAVE();
}

static inline hri_rstc_wkcause_reg_t hri_rstc_read_WKCAUSE_WKCAUSE_bf(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Rstc *)hw)->WKCAUSE.reg;
	tmp = (tmp & RSTC_WKCAUSE_WKCAUSE_Msk) >> RSTC_WKCAUSE_WKCAUSE_Pos;
	return tmp;
}

static inline void hri_rstc_set_WKCAUSE_reg(const void *const hw, hri_rstc_wkcause_reg_t mask)
{
	RSTC_CRITICAL_SECTION_ENTER();
	((Rstc *)hw)->WKCAUSE.reg |= mask;
	RSTC_CRITICAL_SECTION_LEAVE();
}

static inline hri_rstc_wkcause_reg_t hri_rstc_get_WKCAUSE_reg(const void *const hw, hri_rstc_wkcause_reg_t mask)
{
	uint16_t tmp;
	tmp = ((Rstc *)hw)->WKCAUSE.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_rstc_write_WKCAUSE_reg(const void *const hw, hri_rstc_wkcause_reg_t data)
{
	RSTC_CRITICAL_SECTION_ENTER();
	((Rstc *)hw)->WKCAUSE.reg = data;
	RSTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rstc_clear_WKCAUSE_reg(const void *const hw, hri_rstc_wkcause_reg_t mask)
{
	RSTC_CRITICAL_SECTION_ENTER();
	((Rstc *)hw)->WKCAUSE.reg &= ~mask;
	RSTC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_rstc_toggle_WKCAUSE_reg(const void *const hw, hri_rstc_wkcause_reg_t mask)
{
	RSTC_CRITICAL_SECTION_ENTER();
	((Rstc *)hw)->WKCAUSE.reg ^= mask;
	RSTC_CRITICAL_SECTION_LEAVE();
}

static inline hri_rstc_wkcause_reg_t hri_rstc_read_WKCAUSE_reg(const void *const hw)
{
	return ((Rstc *)hw)->WKCAUSE.reg;
}

#ifdef __cplusplus
}
#endif

#endif /* _HRI_RSTC_L21_H_INCLUDED */
#endif /* _SAML21_RSTC_COMPONENT_ */
