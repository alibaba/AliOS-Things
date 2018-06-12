/**
 * \file
 *
 * \brief SAM SystemControl
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

#ifdef _SAME54_SystemControl_COMPONENT_
#ifndef _HRI_SystemControl_E54_H_INCLUDED_
#define _HRI_SystemControl_E54_H_INCLUDED_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <hal_atomic.h>

#if defined(ENABLE_SystemControl_CRITICAL_SECTIONS)
#define SystemControl_CRITICAL_SECTION_ENTER() CRITICAL_SECTION_ENTER()
#define SystemControl_CRITICAL_SECTION_LEAVE() CRITICAL_SECTION_LEAVE()
#else
#define SystemControl_CRITICAL_SECTION_ENTER()
#define SystemControl_CRITICAL_SECTION_LEAVE()
#endif

typedef uint32_t hri_systemcontrol_actlr_reg_t;
typedef uint32_t hri_systemcontrol_afsr_reg_t;
typedef uint32_t hri_systemcontrol_aircr_reg_t;
typedef uint32_t hri_systemcontrol_bfar_reg_t;
typedef uint32_t hri_systemcontrol_ccr_reg_t;
typedef uint32_t hri_systemcontrol_cfsr_reg_t;
typedef uint32_t hri_systemcontrol_cpuid_reg_t;
typedef uint32_t hri_systemcontrol_dfsr_reg_t;
typedef uint32_t hri_systemcontrol_hfsr_reg_t;
typedef uint32_t hri_systemcontrol_icsr_reg_t;
typedef uint32_t hri_systemcontrol_mmfar_reg_t;
typedef uint32_t hri_systemcontrol_scr_reg_t;
typedef uint32_t hri_systemcontrol_shcsr_reg_t;
typedef uint32_t hri_systemcontrol_shpr1_reg_t;
typedef uint32_t hri_systemcontrol_shpr2_reg_t;
typedef uint32_t hri_systemcontrol_shpr3_reg_t;
typedef uint32_t hri_systemcontrol_vtor_reg_t;

static inline void hri_systemcontrol_set_ACTLR_DISMCYCINT_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->ACTLR.reg |= SystemControl_ACTLR_DISMCYCINT;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_systemcontrol_get_ACTLR_DISMCYCINT_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Systemcontrol *)hw)->ACTLR.reg;
	tmp = (tmp & SystemControl_ACTLR_DISMCYCINT) >> 0;
	return (bool)tmp;
}

static inline void hri_systemcontrol_write_ACTLR_DISMCYCINT_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	SystemControl_CRITICAL_SECTION_ENTER();
	tmp = ((Systemcontrol *)hw)->ACTLR.reg;
	tmp &= ~SystemControl_ACTLR_DISMCYCINT;
	tmp |= value << 0;
	((Systemcontrol *)hw)->ACTLR.reg = tmp;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_clear_ACTLR_DISMCYCINT_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->ACTLR.reg &= ~SystemControl_ACTLR_DISMCYCINT;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_toggle_ACTLR_DISMCYCINT_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->ACTLR.reg ^= SystemControl_ACTLR_DISMCYCINT;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_set_ACTLR_DISDEFWBUF_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->ACTLR.reg |= SystemControl_ACTLR_DISDEFWBUF;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_systemcontrol_get_ACTLR_DISDEFWBUF_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Systemcontrol *)hw)->ACTLR.reg;
	tmp = (tmp & SystemControl_ACTLR_DISDEFWBUF) >> 1;
	return (bool)tmp;
}

static inline void hri_systemcontrol_write_ACTLR_DISDEFWBUF_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	SystemControl_CRITICAL_SECTION_ENTER();
	tmp = ((Systemcontrol *)hw)->ACTLR.reg;
	tmp &= ~SystemControl_ACTLR_DISDEFWBUF;
	tmp |= value << 1;
	((Systemcontrol *)hw)->ACTLR.reg = tmp;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_clear_ACTLR_DISDEFWBUF_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->ACTLR.reg &= ~SystemControl_ACTLR_DISDEFWBUF;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_toggle_ACTLR_DISDEFWBUF_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->ACTLR.reg ^= SystemControl_ACTLR_DISDEFWBUF;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_set_ACTLR_DISFOLD_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->ACTLR.reg |= SystemControl_ACTLR_DISFOLD;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_systemcontrol_get_ACTLR_DISFOLD_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Systemcontrol *)hw)->ACTLR.reg;
	tmp = (tmp & SystemControl_ACTLR_DISFOLD) >> 2;
	return (bool)tmp;
}

static inline void hri_systemcontrol_write_ACTLR_DISFOLD_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	SystemControl_CRITICAL_SECTION_ENTER();
	tmp = ((Systemcontrol *)hw)->ACTLR.reg;
	tmp &= ~SystemControl_ACTLR_DISFOLD;
	tmp |= value << 2;
	((Systemcontrol *)hw)->ACTLR.reg = tmp;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_clear_ACTLR_DISFOLD_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->ACTLR.reg &= ~SystemControl_ACTLR_DISFOLD;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_toggle_ACTLR_DISFOLD_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->ACTLR.reg ^= SystemControl_ACTLR_DISFOLD;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_set_ACTLR_reg(const void *const hw, hri_systemcontrol_actlr_reg_t mask)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->ACTLR.reg |= mask;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline hri_systemcontrol_actlr_reg_t hri_systemcontrol_get_ACTLR_reg(const void *const             hw,
                                                                            hri_systemcontrol_actlr_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Systemcontrol *)hw)->ACTLR.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_systemcontrol_write_ACTLR_reg(const void *const hw, hri_systemcontrol_actlr_reg_t data)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->ACTLR.reg = data;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_clear_ACTLR_reg(const void *const hw, hri_systemcontrol_actlr_reg_t mask)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->ACTLR.reg &= ~mask;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_toggle_ACTLR_reg(const void *const hw, hri_systemcontrol_actlr_reg_t mask)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->ACTLR.reg ^= mask;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline hri_systemcontrol_actlr_reg_t hri_systemcontrol_read_ACTLR_reg(const void *const hw)
{
	return ((Systemcontrol *)hw)->ACTLR.reg;
}

static inline void hri_systemcontrol_set_CPUID_REVISION_bf(const void *const hw, hri_systemcontrol_cpuid_reg_t mask)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->CPUID.reg |= SystemControl_CPUID_REVISION(mask);
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline hri_systemcontrol_cpuid_reg_t hri_systemcontrol_get_CPUID_REVISION_bf(const void *const             hw,
                                                                                    hri_systemcontrol_cpuid_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Systemcontrol *)hw)->CPUID.reg;
	tmp = (tmp & SystemControl_CPUID_REVISION(mask)) >> 0;
	return tmp;
}

static inline void hri_systemcontrol_write_CPUID_REVISION_bf(const void *const hw, hri_systemcontrol_cpuid_reg_t data)
{
	uint32_t tmp;
	SystemControl_CRITICAL_SECTION_ENTER();
	tmp = ((Systemcontrol *)hw)->CPUID.reg;
	tmp &= ~SystemControl_CPUID_REVISION_Msk;
	tmp |= SystemControl_CPUID_REVISION(data);
	((Systemcontrol *)hw)->CPUID.reg = tmp;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_clear_CPUID_REVISION_bf(const void *const hw, hri_systemcontrol_cpuid_reg_t mask)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->CPUID.reg &= ~SystemControl_CPUID_REVISION(mask);
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_toggle_CPUID_REVISION_bf(const void *const hw, hri_systemcontrol_cpuid_reg_t mask)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->CPUID.reg ^= SystemControl_CPUID_REVISION(mask);
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline hri_systemcontrol_cpuid_reg_t hri_systemcontrol_read_CPUID_REVISION_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Systemcontrol *)hw)->CPUID.reg;
	tmp = (tmp & SystemControl_CPUID_REVISION_Msk) >> 0;
	return tmp;
}

static inline void hri_systemcontrol_set_CPUID_PARTNO_bf(const void *const hw, hri_systemcontrol_cpuid_reg_t mask)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->CPUID.reg |= SystemControl_CPUID_PARTNO(mask);
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline hri_systemcontrol_cpuid_reg_t hri_systemcontrol_get_CPUID_PARTNO_bf(const void *const             hw,
                                                                                  hri_systemcontrol_cpuid_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Systemcontrol *)hw)->CPUID.reg;
	tmp = (tmp & SystemControl_CPUID_PARTNO(mask)) >> 4;
	return tmp;
}

static inline void hri_systemcontrol_write_CPUID_PARTNO_bf(const void *const hw, hri_systemcontrol_cpuid_reg_t data)
{
	uint32_t tmp;
	SystemControl_CRITICAL_SECTION_ENTER();
	tmp = ((Systemcontrol *)hw)->CPUID.reg;
	tmp &= ~SystemControl_CPUID_PARTNO_Msk;
	tmp |= SystemControl_CPUID_PARTNO(data);
	((Systemcontrol *)hw)->CPUID.reg = tmp;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_clear_CPUID_PARTNO_bf(const void *const hw, hri_systemcontrol_cpuid_reg_t mask)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->CPUID.reg &= ~SystemControl_CPUID_PARTNO(mask);
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_toggle_CPUID_PARTNO_bf(const void *const hw, hri_systemcontrol_cpuid_reg_t mask)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->CPUID.reg ^= SystemControl_CPUID_PARTNO(mask);
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline hri_systemcontrol_cpuid_reg_t hri_systemcontrol_read_CPUID_PARTNO_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Systemcontrol *)hw)->CPUID.reg;
	tmp = (tmp & SystemControl_CPUID_PARTNO_Msk) >> 4;
	return tmp;
}

static inline void hri_systemcontrol_set_CPUID_VARIANT_bf(const void *const hw, hri_systemcontrol_cpuid_reg_t mask)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->CPUID.reg |= SystemControl_CPUID_VARIANT(mask);
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline hri_systemcontrol_cpuid_reg_t hri_systemcontrol_get_CPUID_VARIANT_bf(const void *const             hw,
                                                                                   hri_systemcontrol_cpuid_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Systemcontrol *)hw)->CPUID.reg;
	tmp = (tmp & SystemControl_CPUID_VARIANT(mask)) >> 20;
	return tmp;
}

static inline void hri_systemcontrol_write_CPUID_VARIANT_bf(const void *const hw, hri_systemcontrol_cpuid_reg_t data)
{
	uint32_t tmp;
	SystemControl_CRITICAL_SECTION_ENTER();
	tmp = ((Systemcontrol *)hw)->CPUID.reg;
	tmp &= ~SystemControl_CPUID_VARIANT_Msk;
	tmp |= SystemControl_CPUID_VARIANT(data);
	((Systemcontrol *)hw)->CPUID.reg = tmp;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_clear_CPUID_VARIANT_bf(const void *const hw, hri_systemcontrol_cpuid_reg_t mask)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->CPUID.reg &= ~SystemControl_CPUID_VARIANT(mask);
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_toggle_CPUID_VARIANT_bf(const void *const hw, hri_systemcontrol_cpuid_reg_t mask)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->CPUID.reg ^= SystemControl_CPUID_VARIANT(mask);
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline hri_systemcontrol_cpuid_reg_t hri_systemcontrol_read_CPUID_VARIANT_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Systemcontrol *)hw)->CPUID.reg;
	tmp = (tmp & SystemControl_CPUID_VARIANT_Msk) >> 20;
	return tmp;
}

static inline void hri_systemcontrol_set_CPUID_IMPLEMENTER_bf(const void *const hw, hri_systemcontrol_cpuid_reg_t mask)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->CPUID.reg |= SystemControl_CPUID_IMPLEMENTER(mask);
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline hri_systemcontrol_cpuid_reg_t
hri_systemcontrol_get_CPUID_IMPLEMENTER_bf(const void *const hw, hri_systemcontrol_cpuid_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Systemcontrol *)hw)->CPUID.reg;
	tmp = (tmp & SystemControl_CPUID_IMPLEMENTER(mask)) >> 24;
	return tmp;
}

static inline void hri_systemcontrol_write_CPUID_IMPLEMENTER_bf(const void *const             hw,
                                                                hri_systemcontrol_cpuid_reg_t data)
{
	uint32_t tmp;
	SystemControl_CRITICAL_SECTION_ENTER();
	tmp = ((Systemcontrol *)hw)->CPUID.reg;
	tmp &= ~SystemControl_CPUID_IMPLEMENTER_Msk;
	tmp |= SystemControl_CPUID_IMPLEMENTER(data);
	((Systemcontrol *)hw)->CPUID.reg = tmp;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_clear_CPUID_IMPLEMENTER_bf(const void *const             hw,
                                                                hri_systemcontrol_cpuid_reg_t mask)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->CPUID.reg &= ~SystemControl_CPUID_IMPLEMENTER(mask);
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_toggle_CPUID_IMPLEMENTER_bf(const void *const             hw,
                                                                 hri_systemcontrol_cpuid_reg_t mask)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->CPUID.reg ^= SystemControl_CPUID_IMPLEMENTER(mask);
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline hri_systemcontrol_cpuid_reg_t hri_systemcontrol_read_CPUID_IMPLEMENTER_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Systemcontrol *)hw)->CPUID.reg;
	tmp = (tmp & SystemControl_CPUID_IMPLEMENTER_Msk) >> 24;
	return tmp;
}

static inline void hri_systemcontrol_set_CPUID_reg(const void *const hw, hri_systemcontrol_cpuid_reg_t mask)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->CPUID.reg |= mask;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline hri_systemcontrol_cpuid_reg_t hri_systemcontrol_get_CPUID_reg(const void *const             hw,
                                                                            hri_systemcontrol_cpuid_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Systemcontrol *)hw)->CPUID.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_systemcontrol_write_CPUID_reg(const void *const hw, hri_systemcontrol_cpuid_reg_t data)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->CPUID.reg = data;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_clear_CPUID_reg(const void *const hw, hri_systemcontrol_cpuid_reg_t mask)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->CPUID.reg &= ~mask;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_toggle_CPUID_reg(const void *const hw, hri_systemcontrol_cpuid_reg_t mask)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->CPUID.reg ^= mask;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline hri_systemcontrol_cpuid_reg_t hri_systemcontrol_read_CPUID_reg(const void *const hw)
{
	return ((Systemcontrol *)hw)->CPUID.reg;
}

static inline void hri_systemcontrol_set_ICSR_RETTOBASE_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->ICSR.reg |= SystemControl_ICSR_RETTOBASE;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_systemcontrol_get_ICSR_RETTOBASE_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Systemcontrol *)hw)->ICSR.reg;
	tmp = (tmp & SystemControl_ICSR_RETTOBASE) >> 11;
	return (bool)tmp;
}

static inline void hri_systemcontrol_write_ICSR_RETTOBASE_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	SystemControl_CRITICAL_SECTION_ENTER();
	tmp = ((Systemcontrol *)hw)->ICSR.reg;
	tmp &= ~SystemControl_ICSR_RETTOBASE;
	tmp |= value << 11;
	((Systemcontrol *)hw)->ICSR.reg = tmp;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_clear_ICSR_RETTOBASE_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->ICSR.reg &= ~SystemControl_ICSR_RETTOBASE;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_toggle_ICSR_RETTOBASE_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->ICSR.reg ^= SystemControl_ICSR_RETTOBASE;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_set_ICSR_ISRPENDING_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->ICSR.reg |= SystemControl_ICSR_ISRPENDING;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_systemcontrol_get_ICSR_ISRPENDING_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Systemcontrol *)hw)->ICSR.reg;
	tmp = (tmp & SystemControl_ICSR_ISRPENDING) >> 22;
	return (bool)tmp;
}

static inline void hri_systemcontrol_write_ICSR_ISRPENDING_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	SystemControl_CRITICAL_SECTION_ENTER();
	tmp = ((Systemcontrol *)hw)->ICSR.reg;
	tmp &= ~SystemControl_ICSR_ISRPENDING;
	tmp |= value << 22;
	((Systemcontrol *)hw)->ICSR.reg = tmp;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_clear_ICSR_ISRPENDING_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->ICSR.reg &= ~SystemControl_ICSR_ISRPENDING;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_toggle_ICSR_ISRPENDING_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->ICSR.reg ^= SystemControl_ICSR_ISRPENDING;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_set_ICSR_ISRPREEMPT_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->ICSR.reg |= SystemControl_ICSR_ISRPREEMPT;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_systemcontrol_get_ICSR_ISRPREEMPT_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Systemcontrol *)hw)->ICSR.reg;
	tmp = (tmp & SystemControl_ICSR_ISRPREEMPT) >> 23;
	return (bool)tmp;
}

static inline void hri_systemcontrol_write_ICSR_ISRPREEMPT_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	SystemControl_CRITICAL_SECTION_ENTER();
	tmp = ((Systemcontrol *)hw)->ICSR.reg;
	tmp &= ~SystemControl_ICSR_ISRPREEMPT;
	tmp |= value << 23;
	((Systemcontrol *)hw)->ICSR.reg = tmp;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_clear_ICSR_ISRPREEMPT_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->ICSR.reg &= ~SystemControl_ICSR_ISRPREEMPT;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_toggle_ICSR_ISRPREEMPT_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->ICSR.reg ^= SystemControl_ICSR_ISRPREEMPT;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_set_ICSR_PENDSTCLR_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->ICSR.reg |= SystemControl_ICSR_PENDSTCLR;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_systemcontrol_get_ICSR_PENDSTCLR_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Systemcontrol *)hw)->ICSR.reg;
	tmp = (tmp & SystemControl_ICSR_PENDSTCLR) >> 25;
	return (bool)tmp;
}

static inline void hri_systemcontrol_write_ICSR_PENDSTCLR_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	SystemControl_CRITICAL_SECTION_ENTER();
	tmp = ((Systemcontrol *)hw)->ICSR.reg;
	tmp &= ~SystemControl_ICSR_PENDSTCLR;
	tmp |= value << 25;
	((Systemcontrol *)hw)->ICSR.reg = tmp;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_clear_ICSR_PENDSTCLR_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->ICSR.reg &= ~SystemControl_ICSR_PENDSTCLR;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_toggle_ICSR_PENDSTCLR_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->ICSR.reg ^= SystemControl_ICSR_PENDSTCLR;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_set_ICSR_PENDSTSET_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->ICSR.reg |= SystemControl_ICSR_PENDSTSET;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_systemcontrol_get_ICSR_PENDSTSET_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Systemcontrol *)hw)->ICSR.reg;
	tmp = (tmp & SystemControl_ICSR_PENDSTSET) >> 26;
	return (bool)tmp;
}

static inline void hri_systemcontrol_write_ICSR_PENDSTSET_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	SystemControl_CRITICAL_SECTION_ENTER();
	tmp = ((Systemcontrol *)hw)->ICSR.reg;
	tmp &= ~SystemControl_ICSR_PENDSTSET;
	tmp |= value << 26;
	((Systemcontrol *)hw)->ICSR.reg = tmp;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_clear_ICSR_PENDSTSET_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->ICSR.reg &= ~SystemControl_ICSR_PENDSTSET;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_toggle_ICSR_PENDSTSET_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->ICSR.reg ^= SystemControl_ICSR_PENDSTSET;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_set_ICSR_PENDSVCLR_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->ICSR.reg |= SystemControl_ICSR_PENDSVCLR;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_systemcontrol_get_ICSR_PENDSVCLR_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Systemcontrol *)hw)->ICSR.reg;
	tmp = (tmp & SystemControl_ICSR_PENDSVCLR) >> 27;
	return (bool)tmp;
}

static inline void hri_systemcontrol_write_ICSR_PENDSVCLR_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	SystemControl_CRITICAL_SECTION_ENTER();
	tmp = ((Systemcontrol *)hw)->ICSR.reg;
	tmp &= ~SystemControl_ICSR_PENDSVCLR;
	tmp |= value << 27;
	((Systemcontrol *)hw)->ICSR.reg = tmp;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_clear_ICSR_PENDSVCLR_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->ICSR.reg &= ~SystemControl_ICSR_PENDSVCLR;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_toggle_ICSR_PENDSVCLR_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->ICSR.reg ^= SystemControl_ICSR_PENDSVCLR;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_set_ICSR_PENDSVSET_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->ICSR.reg |= SystemControl_ICSR_PENDSVSET;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_systemcontrol_get_ICSR_PENDSVSET_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Systemcontrol *)hw)->ICSR.reg;
	tmp = (tmp & SystemControl_ICSR_PENDSVSET) >> 28;
	return (bool)tmp;
}

static inline void hri_systemcontrol_write_ICSR_PENDSVSET_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	SystemControl_CRITICAL_SECTION_ENTER();
	tmp = ((Systemcontrol *)hw)->ICSR.reg;
	tmp &= ~SystemControl_ICSR_PENDSVSET;
	tmp |= value << 28;
	((Systemcontrol *)hw)->ICSR.reg = tmp;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_clear_ICSR_PENDSVSET_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->ICSR.reg &= ~SystemControl_ICSR_PENDSVSET;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_toggle_ICSR_PENDSVSET_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->ICSR.reg ^= SystemControl_ICSR_PENDSVSET;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_set_ICSR_NMIPENDSET_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->ICSR.reg |= SystemControl_ICSR_NMIPENDSET;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_systemcontrol_get_ICSR_NMIPENDSET_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Systemcontrol *)hw)->ICSR.reg;
	tmp = (tmp & SystemControl_ICSR_NMIPENDSET) >> 31;
	return (bool)tmp;
}

static inline void hri_systemcontrol_write_ICSR_NMIPENDSET_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	SystemControl_CRITICAL_SECTION_ENTER();
	tmp = ((Systemcontrol *)hw)->ICSR.reg;
	tmp &= ~SystemControl_ICSR_NMIPENDSET;
	tmp |= value << 31;
	((Systemcontrol *)hw)->ICSR.reg = tmp;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_clear_ICSR_NMIPENDSET_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->ICSR.reg &= ~SystemControl_ICSR_NMIPENDSET;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_toggle_ICSR_NMIPENDSET_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->ICSR.reg ^= SystemControl_ICSR_NMIPENDSET;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_set_ICSR_VECTACTIVE_bf(const void *const hw, hri_systemcontrol_icsr_reg_t mask)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->ICSR.reg |= SystemControl_ICSR_VECTACTIVE(mask);
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline hri_systemcontrol_icsr_reg_t hri_systemcontrol_get_ICSR_VECTACTIVE_bf(const void *const            hw,
                                                                                    hri_systemcontrol_icsr_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Systemcontrol *)hw)->ICSR.reg;
	tmp = (tmp & SystemControl_ICSR_VECTACTIVE(mask)) >> 0;
	return tmp;
}

static inline void hri_systemcontrol_write_ICSR_VECTACTIVE_bf(const void *const hw, hri_systemcontrol_icsr_reg_t data)
{
	uint32_t tmp;
	SystemControl_CRITICAL_SECTION_ENTER();
	tmp = ((Systemcontrol *)hw)->ICSR.reg;
	tmp &= ~SystemControl_ICSR_VECTACTIVE_Msk;
	tmp |= SystemControl_ICSR_VECTACTIVE(data);
	((Systemcontrol *)hw)->ICSR.reg = tmp;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_clear_ICSR_VECTACTIVE_bf(const void *const hw, hri_systemcontrol_icsr_reg_t mask)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->ICSR.reg &= ~SystemControl_ICSR_VECTACTIVE(mask);
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_toggle_ICSR_VECTACTIVE_bf(const void *const hw, hri_systemcontrol_icsr_reg_t mask)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->ICSR.reg ^= SystemControl_ICSR_VECTACTIVE(mask);
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline hri_systemcontrol_icsr_reg_t hri_systemcontrol_read_ICSR_VECTACTIVE_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Systemcontrol *)hw)->ICSR.reg;
	tmp = (tmp & SystemControl_ICSR_VECTACTIVE_Msk) >> 0;
	return tmp;
}

static inline void hri_systemcontrol_set_ICSR_VECTPENDING_bf(const void *const hw, hri_systemcontrol_icsr_reg_t mask)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->ICSR.reg |= SystemControl_ICSR_VECTPENDING(mask);
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline hri_systemcontrol_icsr_reg_t hri_systemcontrol_get_ICSR_VECTPENDING_bf(const void *const            hw,
                                                                                     hri_systemcontrol_icsr_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Systemcontrol *)hw)->ICSR.reg;
	tmp = (tmp & SystemControl_ICSR_VECTPENDING(mask)) >> 12;
	return tmp;
}

static inline void hri_systemcontrol_write_ICSR_VECTPENDING_bf(const void *const hw, hri_systemcontrol_icsr_reg_t data)
{
	uint32_t tmp;
	SystemControl_CRITICAL_SECTION_ENTER();
	tmp = ((Systemcontrol *)hw)->ICSR.reg;
	tmp &= ~SystemControl_ICSR_VECTPENDING_Msk;
	tmp |= SystemControl_ICSR_VECTPENDING(data);
	((Systemcontrol *)hw)->ICSR.reg = tmp;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_clear_ICSR_VECTPENDING_bf(const void *const hw, hri_systemcontrol_icsr_reg_t mask)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->ICSR.reg &= ~SystemControl_ICSR_VECTPENDING(mask);
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_toggle_ICSR_VECTPENDING_bf(const void *const hw, hri_systemcontrol_icsr_reg_t mask)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->ICSR.reg ^= SystemControl_ICSR_VECTPENDING(mask);
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline hri_systemcontrol_icsr_reg_t hri_systemcontrol_read_ICSR_VECTPENDING_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Systemcontrol *)hw)->ICSR.reg;
	tmp = (tmp & SystemControl_ICSR_VECTPENDING_Msk) >> 12;
	return tmp;
}

static inline void hri_systemcontrol_set_ICSR_reg(const void *const hw, hri_systemcontrol_icsr_reg_t mask)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->ICSR.reg |= mask;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline hri_systemcontrol_icsr_reg_t hri_systemcontrol_get_ICSR_reg(const void *const            hw,
                                                                          hri_systemcontrol_icsr_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Systemcontrol *)hw)->ICSR.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_systemcontrol_write_ICSR_reg(const void *const hw, hri_systemcontrol_icsr_reg_t data)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->ICSR.reg = data;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_clear_ICSR_reg(const void *const hw, hri_systemcontrol_icsr_reg_t mask)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->ICSR.reg &= ~mask;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_toggle_ICSR_reg(const void *const hw, hri_systemcontrol_icsr_reg_t mask)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->ICSR.reg ^= mask;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline hri_systemcontrol_icsr_reg_t hri_systemcontrol_read_ICSR_reg(const void *const hw)
{
	return ((Systemcontrol *)hw)->ICSR.reg;
}

static inline void hri_systemcontrol_set_VTOR_TBLOFF_bf(const void *const hw, hri_systemcontrol_vtor_reg_t mask)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->VTOR.reg |= SystemControl_VTOR_TBLOFF(mask);
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline hri_systemcontrol_vtor_reg_t hri_systemcontrol_get_VTOR_TBLOFF_bf(const void *const            hw,
                                                                                hri_systemcontrol_vtor_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Systemcontrol *)hw)->VTOR.reg;
	tmp = (tmp & SystemControl_VTOR_TBLOFF(mask)) >> 7;
	return tmp;
}

static inline void hri_systemcontrol_write_VTOR_TBLOFF_bf(const void *const hw, hri_systemcontrol_vtor_reg_t data)
{
	uint32_t tmp;
	SystemControl_CRITICAL_SECTION_ENTER();
	tmp = ((Systemcontrol *)hw)->VTOR.reg;
	tmp &= ~SystemControl_VTOR_TBLOFF_Msk;
	tmp |= SystemControl_VTOR_TBLOFF(data);
	((Systemcontrol *)hw)->VTOR.reg = tmp;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_clear_VTOR_TBLOFF_bf(const void *const hw, hri_systemcontrol_vtor_reg_t mask)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->VTOR.reg &= ~SystemControl_VTOR_TBLOFF(mask);
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_toggle_VTOR_TBLOFF_bf(const void *const hw, hri_systemcontrol_vtor_reg_t mask)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->VTOR.reg ^= SystemControl_VTOR_TBLOFF(mask);
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline hri_systemcontrol_vtor_reg_t hri_systemcontrol_read_VTOR_TBLOFF_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Systemcontrol *)hw)->VTOR.reg;
	tmp = (tmp & SystemControl_VTOR_TBLOFF_Msk) >> 7;
	return tmp;
}

static inline void hri_systemcontrol_set_VTOR_reg(const void *const hw, hri_systemcontrol_vtor_reg_t mask)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->VTOR.reg |= mask;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline hri_systemcontrol_vtor_reg_t hri_systemcontrol_get_VTOR_reg(const void *const            hw,
                                                                          hri_systemcontrol_vtor_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Systemcontrol *)hw)->VTOR.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_systemcontrol_write_VTOR_reg(const void *const hw, hri_systemcontrol_vtor_reg_t data)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->VTOR.reg = data;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_clear_VTOR_reg(const void *const hw, hri_systemcontrol_vtor_reg_t mask)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->VTOR.reg &= ~mask;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_toggle_VTOR_reg(const void *const hw, hri_systemcontrol_vtor_reg_t mask)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->VTOR.reg ^= mask;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline hri_systemcontrol_vtor_reg_t hri_systemcontrol_read_VTOR_reg(const void *const hw)
{
	return ((Systemcontrol *)hw)->VTOR.reg;
}

static inline void hri_systemcontrol_set_AIRCR_VECTRESET_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->AIRCR.reg |= SystemControl_AIRCR_VECTRESET;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_systemcontrol_get_AIRCR_VECTRESET_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Systemcontrol *)hw)->AIRCR.reg;
	tmp = (tmp & SystemControl_AIRCR_VECTRESET) >> 0;
	return (bool)tmp;
}

static inline void hri_systemcontrol_write_AIRCR_VECTRESET_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	SystemControl_CRITICAL_SECTION_ENTER();
	tmp = ((Systemcontrol *)hw)->AIRCR.reg;
	tmp &= ~SystemControl_AIRCR_VECTRESET;
	tmp |= value << 0;
	((Systemcontrol *)hw)->AIRCR.reg = tmp;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_clear_AIRCR_VECTRESET_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->AIRCR.reg &= ~SystemControl_AIRCR_VECTRESET;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_toggle_AIRCR_VECTRESET_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->AIRCR.reg ^= SystemControl_AIRCR_VECTRESET;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_set_AIRCR_VECTCLRACTIVE_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->AIRCR.reg |= SystemControl_AIRCR_VECTCLRACTIVE;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_systemcontrol_get_AIRCR_VECTCLRACTIVE_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Systemcontrol *)hw)->AIRCR.reg;
	tmp = (tmp & SystemControl_AIRCR_VECTCLRACTIVE) >> 1;
	return (bool)tmp;
}

static inline void hri_systemcontrol_write_AIRCR_VECTCLRACTIVE_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	SystemControl_CRITICAL_SECTION_ENTER();
	tmp = ((Systemcontrol *)hw)->AIRCR.reg;
	tmp &= ~SystemControl_AIRCR_VECTCLRACTIVE;
	tmp |= value << 1;
	((Systemcontrol *)hw)->AIRCR.reg = tmp;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_clear_AIRCR_VECTCLRACTIVE_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->AIRCR.reg &= ~SystemControl_AIRCR_VECTCLRACTIVE;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_toggle_AIRCR_VECTCLRACTIVE_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->AIRCR.reg ^= SystemControl_AIRCR_VECTCLRACTIVE;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_set_AIRCR_SYSRESETREQ_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->AIRCR.reg |= SystemControl_AIRCR_SYSRESETREQ;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_systemcontrol_get_AIRCR_SYSRESETREQ_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Systemcontrol *)hw)->AIRCR.reg;
	tmp = (tmp & SystemControl_AIRCR_SYSRESETREQ) >> 2;
	return (bool)tmp;
}

static inline void hri_systemcontrol_write_AIRCR_SYSRESETREQ_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	SystemControl_CRITICAL_SECTION_ENTER();
	tmp = ((Systemcontrol *)hw)->AIRCR.reg;
	tmp &= ~SystemControl_AIRCR_SYSRESETREQ;
	tmp |= value << 2;
	((Systemcontrol *)hw)->AIRCR.reg = tmp;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_clear_AIRCR_SYSRESETREQ_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->AIRCR.reg &= ~SystemControl_AIRCR_SYSRESETREQ;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_toggle_AIRCR_SYSRESETREQ_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->AIRCR.reg ^= SystemControl_AIRCR_SYSRESETREQ;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_set_AIRCR_ENDIANNESS_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->AIRCR.reg |= SystemControl_AIRCR_ENDIANNESS;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_systemcontrol_get_AIRCR_ENDIANNESS_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Systemcontrol *)hw)->AIRCR.reg;
	tmp = (tmp & SystemControl_AIRCR_ENDIANNESS) >> 15;
	return (bool)tmp;
}

static inline void hri_systemcontrol_write_AIRCR_ENDIANNESS_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	SystemControl_CRITICAL_SECTION_ENTER();
	tmp = ((Systemcontrol *)hw)->AIRCR.reg;
	tmp &= ~SystemControl_AIRCR_ENDIANNESS;
	tmp |= value << 15;
	((Systemcontrol *)hw)->AIRCR.reg = tmp;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_clear_AIRCR_ENDIANNESS_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->AIRCR.reg &= ~SystemControl_AIRCR_ENDIANNESS;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_toggle_AIRCR_ENDIANNESS_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->AIRCR.reg ^= SystemControl_AIRCR_ENDIANNESS;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_set_AIRCR_PRIGROUP_bf(const void *const hw, hri_systemcontrol_aircr_reg_t mask)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->AIRCR.reg |= SystemControl_AIRCR_PRIGROUP(mask);
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline hri_systemcontrol_aircr_reg_t hri_systemcontrol_get_AIRCR_PRIGROUP_bf(const void *const             hw,
                                                                                    hri_systemcontrol_aircr_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Systemcontrol *)hw)->AIRCR.reg;
	tmp = (tmp & SystemControl_AIRCR_PRIGROUP(mask)) >> 8;
	return tmp;
}

static inline void hri_systemcontrol_write_AIRCR_PRIGROUP_bf(const void *const hw, hri_systemcontrol_aircr_reg_t data)
{
	uint32_t tmp;
	SystemControl_CRITICAL_SECTION_ENTER();
	tmp = ((Systemcontrol *)hw)->AIRCR.reg;
	tmp &= ~SystemControl_AIRCR_PRIGROUP_Msk;
	tmp |= SystemControl_AIRCR_PRIGROUP(data);
	((Systemcontrol *)hw)->AIRCR.reg = tmp;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_clear_AIRCR_PRIGROUP_bf(const void *const hw, hri_systemcontrol_aircr_reg_t mask)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->AIRCR.reg &= ~SystemControl_AIRCR_PRIGROUP(mask);
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_toggle_AIRCR_PRIGROUP_bf(const void *const hw, hri_systemcontrol_aircr_reg_t mask)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->AIRCR.reg ^= SystemControl_AIRCR_PRIGROUP(mask);
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline hri_systemcontrol_aircr_reg_t hri_systemcontrol_read_AIRCR_PRIGROUP_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Systemcontrol *)hw)->AIRCR.reg;
	tmp = (tmp & SystemControl_AIRCR_PRIGROUP_Msk) >> 8;
	return tmp;
}

static inline void hri_systemcontrol_set_AIRCR_VECTKEY_bf(const void *const hw, hri_systemcontrol_aircr_reg_t mask)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->AIRCR.reg |= SystemControl_AIRCR_VECTKEY(mask);
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline hri_systemcontrol_aircr_reg_t hri_systemcontrol_get_AIRCR_VECTKEY_bf(const void *const             hw,
                                                                                   hri_systemcontrol_aircr_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Systemcontrol *)hw)->AIRCR.reg;
	tmp = (tmp & SystemControl_AIRCR_VECTKEY(mask)) >> 16;
	return tmp;
}

static inline void hri_systemcontrol_write_AIRCR_VECTKEY_bf(const void *const hw, hri_systemcontrol_aircr_reg_t data)
{
	uint32_t tmp;
	SystemControl_CRITICAL_SECTION_ENTER();
	tmp = ((Systemcontrol *)hw)->AIRCR.reg;
	tmp &= ~SystemControl_AIRCR_VECTKEY_Msk;
	tmp |= SystemControl_AIRCR_VECTKEY(data);
	((Systemcontrol *)hw)->AIRCR.reg = tmp;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_clear_AIRCR_VECTKEY_bf(const void *const hw, hri_systemcontrol_aircr_reg_t mask)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->AIRCR.reg &= ~SystemControl_AIRCR_VECTKEY(mask);
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_toggle_AIRCR_VECTKEY_bf(const void *const hw, hri_systemcontrol_aircr_reg_t mask)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->AIRCR.reg ^= SystemControl_AIRCR_VECTKEY(mask);
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline hri_systemcontrol_aircr_reg_t hri_systemcontrol_read_AIRCR_VECTKEY_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Systemcontrol *)hw)->AIRCR.reg;
	tmp = (tmp & SystemControl_AIRCR_VECTKEY_Msk) >> 16;
	return tmp;
}

static inline void hri_systemcontrol_set_AIRCR_reg(const void *const hw, hri_systemcontrol_aircr_reg_t mask)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->AIRCR.reg |= mask;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline hri_systemcontrol_aircr_reg_t hri_systemcontrol_get_AIRCR_reg(const void *const             hw,
                                                                            hri_systemcontrol_aircr_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Systemcontrol *)hw)->AIRCR.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_systemcontrol_write_AIRCR_reg(const void *const hw, hri_systemcontrol_aircr_reg_t data)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->AIRCR.reg = data;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_clear_AIRCR_reg(const void *const hw, hri_systemcontrol_aircr_reg_t mask)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->AIRCR.reg &= ~mask;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_toggle_AIRCR_reg(const void *const hw, hri_systemcontrol_aircr_reg_t mask)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->AIRCR.reg ^= mask;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline hri_systemcontrol_aircr_reg_t hri_systemcontrol_read_AIRCR_reg(const void *const hw)
{
	return ((Systemcontrol *)hw)->AIRCR.reg;
}

static inline void hri_systemcontrol_set_SCR_SLEEPONEXIT_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->SCR.reg |= SystemControl_SCR_SLEEPONEXIT;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_systemcontrol_get_SCR_SLEEPONEXIT_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Systemcontrol *)hw)->SCR.reg;
	tmp = (tmp & SystemControl_SCR_SLEEPONEXIT) >> 1;
	return (bool)tmp;
}

static inline void hri_systemcontrol_write_SCR_SLEEPONEXIT_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	SystemControl_CRITICAL_SECTION_ENTER();
	tmp = ((Systemcontrol *)hw)->SCR.reg;
	tmp &= ~SystemControl_SCR_SLEEPONEXIT;
	tmp |= value << 1;
	((Systemcontrol *)hw)->SCR.reg = tmp;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_clear_SCR_SLEEPONEXIT_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->SCR.reg &= ~SystemControl_SCR_SLEEPONEXIT;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_toggle_SCR_SLEEPONEXIT_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->SCR.reg ^= SystemControl_SCR_SLEEPONEXIT;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_set_SCR_SLEEPDEEP_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->SCR.reg |= SystemControl_SCR_SLEEPDEEP;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_systemcontrol_get_SCR_SLEEPDEEP_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Systemcontrol *)hw)->SCR.reg;
	tmp = (tmp & SystemControl_SCR_SLEEPDEEP) >> 2;
	return (bool)tmp;
}

static inline void hri_systemcontrol_write_SCR_SLEEPDEEP_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	SystemControl_CRITICAL_SECTION_ENTER();
	tmp = ((Systemcontrol *)hw)->SCR.reg;
	tmp &= ~SystemControl_SCR_SLEEPDEEP;
	tmp |= value << 2;
	((Systemcontrol *)hw)->SCR.reg = tmp;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_clear_SCR_SLEEPDEEP_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->SCR.reg &= ~SystemControl_SCR_SLEEPDEEP;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_toggle_SCR_SLEEPDEEP_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->SCR.reg ^= SystemControl_SCR_SLEEPDEEP;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_set_SCR_SEVONPEND_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->SCR.reg |= SystemControl_SCR_SEVONPEND;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_systemcontrol_get_SCR_SEVONPEND_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Systemcontrol *)hw)->SCR.reg;
	tmp = (tmp & SystemControl_SCR_SEVONPEND) >> 4;
	return (bool)tmp;
}

static inline void hri_systemcontrol_write_SCR_SEVONPEND_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	SystemControl_CRITICAL_SECTION_ENTER();
	tmp = ((Systemcontrol *)hw)->SCR.reg;
	tmp &= ~SystemControl_SCR_SEVONPEND;
	tmp |= value << 4;
	((Systemcontrol *)hw)->SCR.reg = tmp;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_clear_SCR_SEVONPEND_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->SCR.reg &= ~SystemControl_SCR_SEVONPEND;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_toggle_SCR_SEVONPEND_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->SCR.reg ^= SystemControl_SCR_SEVONPEND;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_set_SCR_reg(const void *const hw, hri_systemcontrol_scr_reg_t mask)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->SCR.reg |= mask;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline hri_systemcontrol_scr_reg_t hri_systemcontrol_get_SCR_reg(const void *const           hw,
                                                                        hri_systemcontrol_scr_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Systemcontrol *)hw)->SCR.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_systemcontrol_write_SCR_reg(const void *const hw, hri_systemcontrol_scr_reg_t data)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->SCR.reg = data;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_clear_SCR_reg(const void *const hw, hri_systemcontrol_scr_reg_t mask)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->SCR.reg &= ~mask;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_toggle_SCR_reg(const void *const hw, hri_systemcontrol_scr_reg_t mask)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->SCR.reg ^= mask;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline hri_systemcontrol_scr_reg_t hri_systemcontrol_read_SCR_reg(const void *const hw)
{
	return ((Systemcontrol *)hw)->SCR.reg;
}

static inline void hri_systemcontrol_set_CCR_NONBASETHRDENA_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->CCR.reg |= SystemControl_CCR_NONBASETHRDENA;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_systemcontrol_get_CCR_NONBASETHRDENA_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Systemcontrol *)hw)->CCR.reg;
	tmp = (tmp & SystemControl_CCR_NONBASETHRDENA) >> 0;
	return (bool)tmp;
}

static inline void hri_systemcontrol_write_CCR_NONBASETHRDENA_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	SystemControl_CRITICAL_SECTION_ENTER();
	tmp = ((Systemcontrol *)hw)->CCR.reg;
	tmp &= ~SystemControl_CCR_NONBASETHRDENA;
	tmp |= value << 0;
	((Systemcontrol *)hw)->CCR.reg = tmp;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_clear_CCR_NONBASETHRDENA_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->CCR.reg &= ~SystemControl_CCR_NONBASETHRDENA;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_toggle_CCR_NONBASETHRDENA_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->CCR.reg ^= SystemControl_CCR_NONBASETHRDENA;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_set_CCR_USERSETMPEND_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->CCR.reg |= SystemControl_CCR_USERSETMPEND;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_systemcontrol_get_CCR_USERSETMPEND_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Systemcontrol *)hw)->CCR.reg;
	tmp = (tmp & SystemControl_CCR_USERSETMPEND) >> 1;
	return (bool)tmp;
}

static inline void hri_systemcontrol_write_CCR_USERSETMPEND_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	SystemControl_CRITICAL_SECTION_ENTER();
	tmp = ((Systemcontrol *)hw)->CCR.reg;
	tmp &= ~SystemControl_CCR_USERSETMPEND;
	tmp |= value << 1;
	((Systemcontrol *)hw)->CCR.reg = tmp;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_clear_CCR_USERSETMPEND_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->CCR.reg &= ~SystemControl_CCR_USERSETMPEND;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_toggle_CCR_USERSETMPEND_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->CCR.reg ^= SystemControl_CCR_USERSETMPEND;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_set_CCR_UNALIGN_TRP_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->CCR.reg |= SystemControl_CCR_UNALIGN_TRP;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_systemcontrol_get_CCR_UNALIGN_TRP_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Systemcontrol *)hw)->CCR.reg;
	tmp = (tmp & SystemControl_CCR_UNALIGN_TRP) >> 3;
	return (bool)tmp;
}

static inline void hri_systemcontrol_write_CCR_UNALIGN_TRP_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	SystemControl_CRITICAL_SECTION_ENTER();
	tmp = ((Systemcontrol *)hw)->CCR.reg;
	tmp &= ~SystemControl_CCR_UNALIGN_TRP;
	tmp |= value << 3;
	((Systemcontrol *)hw)->CCR.reg = tmp;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_clear_CCR_UNALIGN_TRP_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->CCR.reg &= ~SystemControl_CCR_UNALIGN_TRP;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_toggle_CCR_UNALIGN_TRP_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->CCR.reg ^= SystemControl_CCR_UNALIGN_TRP;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_set_CCR_DIV_0_TRP_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->CCR.reg |= SystemControl_CCR_DIV_0_TRP;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_systemcontrol_get_CCR_DIV_0_TRP_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Systemcontrol *)hw)->CCR.reg;
	tmp = (tmp & SystemControl_CCR_DIV_0_TRP) >> 4;
	return (bool)tmp;
}

static inline void hri_systemcontrol_write_CCR_DIV_0_TRP_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	SystemControl_CRITICAL_SECTION_ENTER();
	tmp = ((Systemcontrol *)hw)->CCR.reg;
	tmp &= ~SystemControl_CCR_DIV_0_TRP;
	tmp |= value << 4;
	((Systemcontrol *)hw)->CCR.reg = tmp;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_clear_CCR_DIV_0_TRP_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->CCR.reg &= ~SystemControl_CCR_DIV_0_TRP;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_toggle_CCR_DIV_0_TRP_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->CCR.reg ^= SystemControl_CCR_DIV_0_TRP;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_set_CCR_BFHFNMIGN_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->CCR.reg |= SystemControl_CCR_BFHFNMIGN;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_systemcontrol_get_CCR_BFHFNMIGN_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Systemcontrol *)hw)->CCR.reg;
	tmp = (tmp & SystemControl_CCR_BFHFNMIGN) >> 8;
	return (bool)tmp;
}

static inline void hri_systemcontrol_write_CCR_BFHFNMIGN_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	SystemControl_CRITICAL_SECTION_ENTER();
	tmp = ((Systemcontrol *)hw)->CCR.reg;
	tmp &= ~SystemControl_CCR_BFHFNMIGN;
	tmp |= value << 8;
	((Systemcontrol *)hw)->CCR.reg = tmp;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_clear_CCR_BFHFNMIGN_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->CCR.reg &= ~SystemControl_CCR_BFHFNMIGN;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_toggle_CCR_BFHFNMIGN_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->CCR.reg ^= SystemControl_CCR_BFHFNMIGN;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_set_CCR_STKALIGN_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->CCR.reg |= SystemControl_CCR_STKALIGN;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_systemcontrol_get_CCR_STKALIGN_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Systemcontrol *)hw)->CCR.reg;
	tmp = (tmp & SystemControl_CCR_STKALIGN) >> 9;
	return (bool)tmp;
}

static inline void hri_systemcontrol_write_CCR_STKALIGN_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	SystemControl_CRITICAL_SECTION_ENTER();
	tmp = ((Systemcontrol *)hw)->CCR.reg;
	tmp &= ~SystemControl_CCR_STKALIGN;
	tmp |= value << 9;
	((Systemcontrol *)hw)->CCR.reg = tmp;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_clear_CCR_STKALIGN_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->CCR.reg &= ~SystemControl_CCR_STKALIGN;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_toggle_CCR_STKALIGN_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->CCR.reg ^= SystemControl_CCR_STKALIGN;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_set_CCR_reg(const void *const hw, hri_systemcontrol_ccr_reg_t mask)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->CCR.reg |= mask;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline hri_systemcontrol_ccr_reg_t hri_systemcontrol_get_CCR_reg(const void *const           hw,
                                                                        hri_systemcontrol_ccr_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Systemcontrol *)hw)->CCR.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_systemcontrol_write_CCR_reg(const void *const hw, hri_systemcontrol_ccr_reg_t data)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->CCR.reg = data;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_clear_CCR_reg(const void *const hw, hri_systemcontrol_ccr_reg_t mask)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->CCR.reg &= ~mask;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_toggle_CCR_reg(const void *const hw, hri_systemcontrol_ccr_reg_t mask)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->CCR.reg ^= mask;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline hri_systemcontrol_ccr_reg_t hri_systemcontrol_read_CCR_reg(const void *const hw)
{
	return ((Systemcontrol *)hw)->CCR.reg;
}

static inline void hri_systemcontrol_set_SHPR1_PRI_4_bf(const void *const hw, hri_systemcontrol_shpr1_reg_t mask)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->SHPR1.reg |= SystemControl_SHPR1_PRI_4(mask);
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline hri_systemcontrol_shpr1_reg_t hri_systemcontrol_get_SHPR1_PRI_4_bf(const void *const             hw,
                                                                                 hri_systemcontrol_shpr1_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Systemcontrol *)hw)->SHPR1.reg;
	tmp = (tmp & SystemControl_SHPR1_PRI_4(mask)) >> 0;
	return tmp;
}

static inline void hri_systemcontrol_write_SHPR1_PRI_4_bf(const void *const hw, hri_systemcontrol_shpr1_reg_t data)
{
	uint32_t tmp;
	SystemControl_CRITICAL_SECTION_ENTER();
	tmp = ((Systemcontrol *)hw)->SHPR1.reg;
	tmp &= ~SystemControl_SHPR1_PRI_4_Msk;
	tmp |= SystemControl_SHPR1_PRI_4(data);
	((Systemcontrol *)hw)->SHPR1.reg = tmp;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_clear_SHPR1_PRI_4_bf(const void *const hw, hri_systemcontrol_shpr1_reg_t mask)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->SHPR1.reg &= ~SystemControl_SHPR1_PRI_4(mask);
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_toggle_SHPR1_PRI_4_bf(const void *const hw, hri_systemcontrol_shpr1_reg_t mask)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->SHPR1.reg ^= SystemControl_SHPR1_PRI_4(mask);
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline hri_systemcontrol_shpr1_reg_t hri_systemcontrol_read_SHPR1_PRI_4_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Systemcontrol *)hw)->SHPR1.reg;
	tmp = (tmp & SystemControl_SHPR1_PRI_4_Msk) >> 0;
	return tmp;
}

static inline void hri_systemcontrol_set_SHPR1_PRI_5_bf(const void *const hw, hri_systemcontrol_shpr1_reg_t mask)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->SHPR1.reg |= SystemControl_SHPR1_PRI_5(mask);
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline hri_systemcontrol_shpr1_reg_t hri_systemcontrol_get_SHPR1_PRI_5_bf(const void *const             hw,
                                                                                 hri_systemcontrol_shpr1_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Systemcontrol *)hw)->SHPR1.reg;
	tmp = (tmp & SystemControl_SHPR1_PRI_5(mask)) >> 8;
	return tmp;
}

static inline void hri_systemcontrol_write_SHPR1_PRI_5_bf(const void *const hw, hri_systemcontrol_shpr1_reg_t data)
{
	uint32_t tmp;
	SystemControl_CRITICAL_SECTION_ENTER();
	tmp = ((Systemcontrol *)hw)->SHPR1.reg;
	tmp &= ~SystemControl_SHPR1_PRI_5_Msk;
	tmp |= SystemControl_SHPR1_PRI_5(data);
	((Systemcontrol *)hw)->SHPR1.reg = tmp;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_clear_SHPR1_PRI_5_bf(const void *const hw, hri_systemcontrol_shpr1_reg_t mask)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->SHPR1.reg &= ~SystemControl_SHPR1_PRI_5(mask);
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_toggle_SHPR1_PRI_5_bf(const void *const hw, hri_systemcontrol_shpr1_reg_t mask)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->SHPR1.reg ^= SystemControl_SHPR1_PRI_5(mask);
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline hri_systemcontrol_shpr1_reg_t hri_systemcontrol_read_SHPR1_PRI_5_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Systemcontrol *)hw)->SHPR1.reg;
	tmp = (tmp & SystemControl_SHPR1_PRI_5_Msk) >> 8;
	return tmp;
}

static inline void hri_systemcontrol_set_SHPR1_PRI_6_bf(const void *const hw, hri_systemcontrol_shpr1_reg_t mask)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->SHPR1.reg |= SystemControl_SHPR1_PRI_6(mask);
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline hri_systemcontrol_shpr1_reg_t hri_systemcontrol_get_SHPR1_PRI_6_bf(const void *const             hw,
                                                                                 hri_systemcontrol_shpr1_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Systemcontrol *)hw)->SHPR1.reg;
	tmp = (tmp & SystemControl_SHPR1_PRI_6(mask)) >> 16;
	return tmp;
}

static inline void hri_systemcontrol_write_SHPR1_PRI_6_bf(const void *const hw, hri_systemcontrol_shpr1_reg_t data)
{
	uint32_t tmp;
	SystemControl_CRITICAL_SECTION_ENTER();
	tmp = ((Systemcontrol *)hw)->SHPR1.reg;
	tmp &= ~SystemControl_SHPR1_PRI_6_Msk;
	tmp |= SystemControl_SHPR1_PRI_6(data);
	((Systemcontrol *)hw)->SHPR1.reg = tmp;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_clear_SHPR1_PRI_6_bf(const void *const hw, hri_systemcontrol_shpr1_reg_t mask)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->SHPR1.reg &= ~SystemControl_SHPR1_PRI_6(mask);
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_toggle_SHPR1_PRI_6_bf(const void *const hw, hri_systemcontrol_shpr1_reg_t mask)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->SHPR1.reg ^= SystemControl_SHPR1_PRI_6(mask);
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline hri_systemcontrol_shpr1_reg_t hri_systemcontrol_read_SHPR1_PRI_6_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Systemcontrol *)hw)->SHPR1.reg;
	tmp = (tmp & SystemControl_SHPR1_PRI_6_Msk) >> 16;
	return tmp;
}

static inline void hri_systemcontrol_set_SHPR1_reg(const void *const hw, hri_systemcontrol_shpr1_reg_t mask)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->SHPR1.reg |= mask;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline hri_systemcontrol_shpr1_reg_t hri_systemcontrol_get_SHPR1_reg(const void *const             hw,
                                                                            hri_systemcontrol_shpr1_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Systemcontrol *)hw)->SHPR1.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_systemcontrol_write_SHPR1_reg(const void *const hw, hri_systemcontrol_shpr1_reg_t data)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->SHPR1.reg = data;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_clear_SHPR1_reg(const void *const hw, hri_systemcontrol_shpr1_reg_t mask)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->SHPR1.reg &= ~mask;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_toggle_SHPR1_reg(const void *const hw, hri_systemcontrol_shpr1_reg_t mask)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->SHPR1.reg ^= mask;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline hri_systemcontrol_shpr1_reg_t hri_systemcontrol_read_SHPR1_reg(const void *const hw)
{
	return ((Systemcontrol *)hw)->SHPR1.reg;
}

static inline void hri_systemcontrol_set_SHPR2_PRI_11_bf(const void *const hw, hri_systemcontrol_shpr2_reg_t mask)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->SHPR2.reg |= SystemControl_SHPR2_PRI_11(mask);
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline hri_systemcontrol_shpr2_reg_t hri_systemcontrol_get_SHPR2_PRI_11_bf(const void *const             hw,
                                                                                  hri_systemcontrol_shpr2_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Systemcontrol *)hw)->SHPR2.reg;
	tmp = (tmp & SystemControl_SHPR2_PRI_11(mask)) >> 24;
	return tmp;
}

static inline void hri_systemcontrol_write_SHPR2_PRI_11_bf(const void *const hw, hri_systemcontrol_shpr2_reg_t data)
{
	uint32_t tmp;
	SystemControl_CRITICAL_SECTION_ENTER();
	tmp = ((Systemcontrol *)hw)->SHPR2.reg;
	tmp &= ~SystemControl_SHPR2_PRI_11_Msk;
	tmp |= SystemControl_SHPR2_PRI_11(data);
	((Systemcontrol *)hw)->SHPR2.reg = tmp;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_clear_SHPR2_PRI_11_bf(const void *const hw, hri_systemcontrol_shpr2_reg_t mask)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->SHPR2.reg &= ~SystemControl_SHPR2_PRI_11(mask);
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_toggle_SHPR2_PRI_11_bf(const void *const hw, hri_systemcontrol_shpr2_reg_t mask)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->SHPR2.reg ^= SystemControl_SHPR2_PRI_11(mask);
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline hri_systemcontrol_shpr2_reg_t hri_systemcontrol_read_SHPR2_PRI_11_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Systemcontrol *)hw)->SHPR2.reg;
	tmp = (tmp & SystemControl_SHPR2_PRI_11_Msk) >> 24;
	return tmp;
}

static inline void hri_systemcontrol_set_SHPR2_reg(const void *const hw, hri_systemcontrol_shpr2_reg_t mask)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->SHPR2.reg |= mask;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline hri_systemcontrol_shpr2_reg_t hri_systemcontrol_get_SHPR2_reg(const void *const             hw,
                                                                            hri_systemcontrol_shpr2_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Systemcontrol *)hw)->SHPR2.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_systemcontrol_write_SHPR2_reg(const void *const hw, hri_systemcontrol_shpr2_reg_t data)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->SHPR2.reg = data;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_clear_SHPR2_reg(const void *const hw, hri_systemcontrol_shpr2_reg_t mask)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->SHPR2.reg &= ~mask;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_toggle_SHPR2_reg(const void *const hw, hri_systemcontrol_shpr2_reg_t mask)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->SHPR2.reg ^= mask;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline hri_systemcontrol_shpr2_reg_t hri_systemcontrol_read_SHPR2_reg(const void *const hw)
{
	return ((Systemcontrol *)hw)->SHPR2.reg;
}

static inline void hri_systemcontrol_set_SHPR3_PRI_14_bf(const void *const hw, hri_systemcontrol_shpr3_reg_t mask)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->SHPR3.reg |= SystemControl_SHPR3_PRI_14(mask);
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline hri_systemcontrol_shpr3_reg_t hri_systemcontrol_get_SHPR3_PRI_14_bf(const void *const             hw,
                                                                                  hri_systemcontrol_shpr3_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Systemcontrol *)hw)->SHPR3.reg;
	tmp = (tmp & SystemControl_SHPR3_PRI_14(mask)) >> 16;
	return tmp;
}

static inline void hri_systemcontrol_write_SHPR3_PRI_14_bf(const void *const hw, hri_systemcontrol_shpr3_reg_t data)
{
	uint32_t tmp;
	SystemControl_CRITICAL_SECTION_ENTER();
	tmp = ((Systemcontrol *)hw)->SHPR3.reg;
	tmp &= ~SystemControl_SHPR3_PRI_14_Msk;
	tmp |= SystemControl_SHPR3_PRI_14(data);
	((Systemcontrol *)hw)->SHPR3.reg = tmp;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_clear_SHPR3_PRI_14_bf(const void *const hw, hri_systemcontrol_shpr3_reg_t mask)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->SHPR3.reg &= ~SystemControl_SHPR3_PRI_14(mask);
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_toggle_SHPR3_PRI_14_bf(const void *const hw, hri_systemcontrol_shpr3_reg_t mask)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->SHPR3.reg ^= SystemControl_SHPR3_PRI_14(mask);
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline hri_systemcontrol_shpr3_reg_t hri_systemcontrol_read_SHPR3_PRI_14_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Systemcontrol *)hw)->SHPR3.reg;
	tmp = (tmp & SystemControl_SHPR3_PRI_14_Msk) >> 16;
	return tmp;
}

static inline void hri_systemcontrol_set_SHPR3_PRI_15_bf(const void *const hw, hri_systemcontrol_shpr3_reg_t mask)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->SHPR3.reg |= SystemControl_SHPR3_PRI_15(mask);
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline hri_systemcontrol_shpr3_reg_t hri_systemcontrol_get_SHPR3_PRI_15_bf(const void *const             hw,
                                                                                  hri_systemcontrol_shpr3_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Systemcontrol *)hw)->SHPR3.reg;
	tmp = (tmp & SystemControl_SHPR3_PRI_15(mask)) >> 24;
	return tmp;
}

static inline void hri_systemcontrol_write_SHPR3_PRI_15_bf(const void *const hw, hri_systemcontrol_shpr3_reg_t data)
{
	uint32_t tmp;
	SystemControl_CRITICAL_SECTION_ENTER();
	tmp = ((Systemcontrol *)hw)->SHPR3.reg;
	tmp &= ~SystemControl_SHPR3_PRI_15_Msk;
	tmp |= SystemControl_SHPR3_PRI_15(data);
	((Systemcontrol *)hw)->SHPR3.reg = tmp;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_clear_SHPR3_PRI_15_bf(const void *const hw, hri_systemcontrol_shpr3_reg_t mask)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->SHPR3.reg &= ~SystemControl_SHPR3_PRI_15(mask);
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_toggle_SHPR3_PRI_15_bf(const void *const hw, hri_systemcontrol_shpr3_reg_t mask)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->SHPR3.reg ^= SystemControl_SHPR3_PRI_15(mask);
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline hri_systemcontrol_shpr3_reg_t hri_systemcontrol_read_SHPR3_PRI_15_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Systemcontrol *)hw)->SHPR3.reg;
	tmp = (tmp & SystemControl_SHPR3_PRI_15_Msk) >> 24;
	return tmp;
}

static inline void hri_systemcontrol_set_SHPR3_reg(const void *const hw, hri_systemcontrol_shpr3_reg_t mask)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->SHPR3.reg |= mask;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline hri_systemcontrol_shpr3_reg_t hri_systemcontrol_get_SHPR3_reg(const void *const             hw,
                                                                            hri_systemcontrol_shpr3_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Systemcontrol *)hw)->SHPR3.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_systemcontrol_write_SHPR3_reg(const void *const hw, hri_systemcontrol_shpr3_reg_t data)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->SHPR3.reg = data;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_clear_SHPR3_reg(const void *const hw, hri_systemcontrol_shpr3_reg_t mask)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->SHPR3.reg &= ~mask;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_toggle_SHPR3_reg(const void *const hw, hri_systemcontrol_shpr3_reg_t mask)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->SHPR3.reg ^= mask;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline hri_systemcontrol_shpr3_reg_t hri_systemcontrol_read_SHPR3_reg(const void *const hw)
{
	return ((Systemcontrol *)hw)->SHPR3.reg;
}

static inline void hri_systemcontrol_set_SHCSR_MEMFAULTACT_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->SHCSR.reg |= SystemControl_SHCSR_MEMFAULTACT;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_systemcontrol_get_SHCSR_MEMFAULTACT_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Systemcontrol *)hw)->SHCSR.reg;
	tmp = (tmp & SystemControl_SHCSR_MEMFAULTACT) >> 0;
	return (bool)tmp;
}

static inline void hri_systemcontrol_write_SHCSR_MEMFAULTACT_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	SystemControl_CRITICAL_SECTION_ENTER();
	tmp = ((Systemcontrol *)hw)->SHCSR.reg;
	tmp &= ~SystemControl_SHCSR_MEMFAULTACT;
	tmp |= value << 0;
	((Systemcontrol *)hw)->SHCSR.reg = tmp;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_clear_SHCSR_MEMFAULTACT_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->SHCSR.reg &= ~SystemControl_SHCSR_MEMFAULTACT;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_toggle_SHCSR_MEMFAULTACT_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->SHCSR.reg ^= SystemControl_SHCSR_MEMFAULTACT;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_set_SHCSR_BUSFAULTACT_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->SHCSR.reg |= SystemControl_SHCSR_BUSFAULTACT;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_systemcontrol_get_SHCSR_BUSFAULTACT_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Systemcontrol *)hw)->SHCSR.reg;
	tmp = (tmp & SystemControl_SHCSR_BUSFAULTACT) >> 1;
	return (bool)tmp;
}

static inline void hri_systemcontrol_write_SHCSR_BUSFAULTACT_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	SystemControl_CRITICAL_SECTION_ENTER();
	tmp = ((Systemcontrol *)hw)->SHCSR.reg;
	tmp &= ~SystemControl_SHCSR_BUSFAULTACT;
	tmp |= value << 1;
	((Systemcontrol *)hw)->SHCSR.reg = tmp;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_clear_SHCSR_BUSFAULTACT_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->SHCSR.reg &= ~SystemControl_SHCSR_BUSFAULTACT;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_toggle_SHCSR_BUSFAULTACT_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->SHCSR.reg ^= SystemControl_SHCSR_BUSFAULTACT;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_set_SHCSR_USGFAULTACT_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->SHCSR.reg |= SystemControl_SHCSR_USGFAULTACT;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_systemcontrol_get_SHCSR_USGFAULTACT_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Systemcontrol *)hw)->SHCSR.reg;
	tmp = (tmp & SystemControl_SHCSR_USGFAULTACT) >> 3;
	return (bool)tmp;
}

static inline void hri_systemcontrol_write_SHCSR_USGFAULTACT_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	SystemControl_CRITICAL_SECTION_ENTER();
	tmp = ((Systemcontrol *)hw)->SHCSR.reg;
	tmp &= ~SystemControl_SHCSR_USGFAULTACT;
	tmp |= value << 3;
	((Systemcontrol *)hw)->SHCSR.reg = tmp;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_clear_SHCSR_USGFAULTACT_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->SHCSR.reg &= ~SystemControl_SHCSR_USGFAULTACT;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_toggle_SHCSR_USGFAULTACT_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->SHCSR.reg ^= SystemControl_SHCSR_USGFAULTACT;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_set_SHCSR_SVCALLACT_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->SHCSR.reg |= SystemControl_SHCSR_SVCALLACT;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_systemcontrol_get_SHCSR_SVCALLACT_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Systemcontrol *)hw)->SHCSR.reg;
	tmp = (tmp & SystemControl_SHCSR_SVCALLACT) >> 7;
	return (bool)tmp;
}

static inline void hri_systemcontrol_write_SHCSR_SVCALLACT_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	SystemControl_CRITICAL_SECTION_ENTER();
	tmp = ((Systemcontrol *)hw)->SHCSR.reg;
	tmp &= ~SystemControl_SHCSR_SVCALLACT;
	tmp |= value << 7;
	((Systemcontrol *)hw)->SHCSR.reg = tmp;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_clear_SHCSR_SVCALLACT_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->SHCSR.reg &= ~SystemControl_SHCSR_SVCALLACT;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_toggle_SHCSR_SVCALLACT_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->SHCSR.reg ^= SystemControl_SHCSR_SVCALLACT;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_set_SHCSR_MONITORACT_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->SHCSR.reg |= SystemControl_SHCSR_MONITORACT;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_systemcontrol_get_SHCSR_MONITORACT_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Systemcontrol *)hw)->SHCSR.reg;
	tmp = (tmp & SystemControl_SHCSR_MONITORACT) >> 8;
	return (bool)tmp;
}

static inline void hri_systemcontrol_write_SHCSR_MONITORACT_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	SystemControl_CRITICAL_SECTION_ENTER();
	tmp = ((Systemcontrol *)hw)->SHCSR.reg;
	tmp &= ~SystemControl_SHCSR_MONITORACT;
	tmp |= value << 8;
	((Systemcontrol *)hw)->SHCSR.reg = tmp;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_clear_SHCSR_MONITORACT_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->SHCSR.reg &= ~SystemControl_SHCSR_MONITORACT;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_toggle_SHCSR_MONITORACT_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->SHCSR.reg ^= SystemControl_SHCSR_MONITORACT;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_set_SHCSR_PENDSVACT_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->SHCSR.reg |= SystemControl_SHCSR_PENDSVACT;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_systemcontrol_get_SHCSR_PENDSVACT_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Systemcontrol *)hw)->SHCSR.reg;
	tmp = (tmp & SystemControl_SHCSR_PENDSVACT) >> 10;
	return (bool)tmp;
}

static inline void hri_systemcontrol_write_SHCSR_PENDSVACT_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	SystemControl_CRITICAL_SECTION_ENTER();
	tmp = ((Systemcontrol *)hw)->SHCSR.reg;
	tmp &= ~SystemControl_SHCSR_PENDSVACT;
	tmp |= value << 10;
	((Systemcontrol *)hw)->SHCSR.reg = tmp;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_clear_SHCSR_PENDSVACT_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->SHCSR.reg &= ~SystemControl_SHCSR_PENDSVACT;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_toggle_SHCSR_PENDSVACT_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->SHCSR.reg ^= SystemControl_SHCSR_PENDSVACT;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_set_SHCSR_SYSTICKACT_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->SHCSR.reg |= SystemControl_SHCSR_SYSTICKACT;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_systemcontrol_get_SHCSR_SYSTICKACT_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Systemcontrol *)hw)->SHCSR.reg;
	tmp = (tmp & SystemControl_SHCSR_SYSTICKACT) >> 11;
	return (bool)tmp;
}

static inline void hri_systemcontrol_write_SHCSR_SYSTICKACT_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	SystemControl_CRITICAL_SECTION_ENTER();
	tmp = ((Systemcontrol *)hw)->SHCSR.reg;
	tmp &= ~SystemControl_SHCSR_SYSTICKACT;
	tmp |= value << 11;
	((Systemcontrol *)hw)->SHCSR.reg = tmp;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_clear_SHCSR_SYSTICKACT_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->SHCSR.reg &= ~SystemControl_SHCSR_SYSTICKACT;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_toggle_SHCSR_SYSTICKACT_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->SHCSR.reg ^= SystemControl_SHCSR_SYSTICKACT;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_set_SHCSR_USGFAULTPENDED_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->SHCSR.reg |= SystemControl_SHCSR_USGFAULTPENDED;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_systemcontrol_get_SHCSR_USGFAULTPENDED_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Systemcontrol *)hw)->SHCSR.reg;
	tmp = (tmp & SystemControl_SHCSR_USGFAULTPENDED) >> 12;
	return (bool)tmp;
}

static inline void hri_systemcontrol_write_SHCSR_USGFAULTPENDED_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	SystemControl_CRITICAL_SECTION_ENTER();
	tmp = ((Systemcontrol *)hw)->SHCSR.reg;
	tmp &= ~SystemControl_SHCSR_USGFAULTPENDED;
	tmp |= value << 12;
	((Systemcontrol *)hw)->SHCSR.reg = tmp;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_clear_SHCSR_USGFAULTPENDED_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->SHCSR.reg &= ~SystemControl_SHCSR_USGFAULTPENDED;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_toggle_SHCSR_USGFAULTPENDED_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->SHCSR.reg ^= SystemControl_SHCSR_USGFAULTPENDED;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_set_SHCSR_MEMFAULTPENDED_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->SHCSR.reg |= SystemControl_SHCSR_MEMFAULTPENDED;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_systemcontrol_get_SHCSR_MEMFAULTPENDED_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Systemcontrol *)hw)->SHCSR.reg;
	tmp = (tmp & SystemControl_SHCSR_MEMFAULTPENDED) >> 13;
	return (bool)tmp;
}

static inline void hri_systemcontrol_write_SHCSR_MEMFAULTPENDED_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	SystemControl_CRITICAL_SECTION_ENTER();
	tmp = ((Systemcontrol *)hw)->SHCSR.reg;
	tmp &= ~SystemControl_SHCSR_MEMFAULTPENDED;
	tmp |= value << 13;
	((Systemcontrol *)hw)->SHCSR.reg = tmp;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_clear_SHCSR_MEMFAULTPENDED_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->SHCSR.reg &= ~SystemControl_SHCSR_MEMFAULTPENDED;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_toggle_SHCSR_MEMFAULTPENDED_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->SHCSR.reg ^= SystemControl_SHCSR_MEMFAULTPENDED;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_set_SHCSR_BUSFAULTPENDED_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->SHCSR.reg |= SystemControl_SHCSR_BUSFAULTPENDED;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_systemcontrol_get_SHCSR_BUSFAULTPENDED_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Systemcontrol *)hw)->SHCSR.reg;
	tmp = (tmp & SystemControl_SHCSR_BUSFAULTPENDED) >> 14;
	return (bool)tmp;
}

static inline void hri_systemcontrol_write_SHCSR_BUSFAULTPENDED_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	SystemControl_CRITICAL_SECTION_ENTER();
	tmp = ((Systemcontrol *)hw)->SHCSR.reg;
	tmp &= ~SystemControl_SHCSR_BUSFAULTPENDED;
	tmp |= value << 14;
	((Systemcontrol *)hw)->SHCSR.reg = tmp;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_clear_SHCSR_BUSFAULTPENDED_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->SHCSR.reg &= ~SystemControl_SHCSR_BUSFAULTPENDED;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_toggle_SHCSR_BUSFAULTPENDED_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->SHCSR.reg ^= SystemControl_SHCSR_BUSFAULTPENDED;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_set_SHCSR_SVCALLPENDED_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->SHCSR.reg |= SystemControl_SHCSR_SVCALLPENDED;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_systemcontrol_get_SHCSR_SVCALLPENDED_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Systemcontrol *)hw)->SHCSR.reg;
	tmp = (tmp & SystemControl_SHCSR_SVCALLPENDED) >> 15;
	return (bool)tmp;
}

static inline void hri_systemcontrol_write_SHCSR_SVCALLPENDED_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	SystemControl_CRITICAL_SECTION_ENTER();
	tmp = ((Systemcontrol *)hw)->SHCSR.reg;
	tmp &= ~SystemControl_SHCSR_SVCALLPENDED;
	tmp |= value << 15;
	((Systemcontrol *)hw)->SHCSR.reg = tmp;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_clear_SHCSR_SVCALLPENDED_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->SHCSR.reg &= ~SystemControl_SHCSR_SVCALLPENDED;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_toggle_SHCSR_SVCALLPENDED_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->SHCSR.reg ^= SystemControl_SHCSR_SVCALLPENDED;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_set_SHCSR_MEMFAULTENA_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->SHCSR.reg |= SystemControl_SHCSR_MEMFAULTENA;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_systemcontrol_get_SHCSR_MEMFAULTENA_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Systemcontrol *)hw)->SHCSR.reg;
	tmp = (tmp & SystemControl_SHCSR_MEMFAULTENA) >> 16;
	return (bool)tmp;
}

static inline void hri_systemcontrol_write_SHCSR_MEMFAULTENA_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	SystemControl_CRITICAL_SECTION_ENTER();
	tmp = ((Systemcontrol *)hw)->SHCSR.reg;
	tmp &= ~SystemControl_SHCSR_MEMFAULTENA;
	tmp |= value << 16;
	((Systemcontrol *)hw)->SHCSR.reg = tmp;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_clear_SHCSR_MEMFAULTENA_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->SHCSR.reg &= ~SystemControl_SHCSR_MEMFAULTENA;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_toggle_SHCSR_MEMFAULTENA_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->SHCSR.reg ^= SystemControl_SHCSR_MEMFAULTENA;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_set_SHCSR_BUSFAULTENA_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->SHCSR.reg |= SystemControl_SHCSR_BUSFAULTENA;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_systemcontrol_get_SHCSR_BUSFAULTENA_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Systemcontrol *)hw)->SHCSR.reg;
	tmp = (tmp & SystemControl_SHCSR_BUSFAULTENA) >> 17;
	return (bool)tmp;
}

static inline void hri_systemcontrol_write_SHCSR_BUSFAULTENA_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	SystemControl_CRITICAL_SECTION_ENTER();
	tmp = ((Systemcontrol *)hw)->SHCSR.reg;
	tmp &= ~SystemControl_SHCSR_BUSFAULTENA;
	tmp |= value << 17;
	((Systemcontrol *)hw)->SHCSR.reg = tmp;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_clear_SHCSR_BUSFAULTENA_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->SHCSR.reg &= ~SystemControl_SHCSR_BUSFAULTENA;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_toggle_SHCSR_BUSFAULTENA_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->SHCSR.reg ^= SystemControl_SHCSR_BUSFAULTENA;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_set_SHCSR_USGFAULTENA_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->SHCSR.reg |= SystemControl_SHCSR_USGFAULTENA;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_systemcontrol_get_SHCSR_USGFAULTENA_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Systemcontrol *)hw)->SHCSR.reg;
	tmp = (tmp & SystemControl_SHCSR_USGFAULTENA) >> 18;
	return (bool)tmp;
}

static inline void hri_systemcontrol_write_SHCSR_USGFAULTENA_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	SystemControl_CRITICAL_SECTION_ENTER();
	tmp = ((Systemcontrol *)hw)->SHCSR.reg;
	tmp &= ~SystemControl_SHCSR_USGFAULTENA;
	tmp |= value << 18;
	((Systemcontrol *)hw)->SHCSR.reg = tmp;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_clear_SHCSR_USGFAULTENA_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->SHCSR.reg &= ~SystemControl_SHCSR_USGFAULTENA;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_toggle_SHCSR_USGFAULTENA_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->SHCSR.reg ^= SystemControl_SHCSR_USGFAULTENA;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_set_SHCSR_reg(const void *const hw, hri_systemcontrol_shcsr_reg_t mask)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->SHCSR.reg |= mask;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline hri_systemcontrol_shcsr_reg_t hri_systemcontrol_get_SHCSR_reg(const void *const             hw,
                                                                            hri_systemcontrol_shcsr_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Systemcontrol *)hw)->SHCSR.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_systemcontrol_write_SHCSR_reg(const void *const hw, hri_systemcontrol_shcsr_reg_t data)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->SHCSR.reg = data;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_clear_SHCSR_reg(const void *const hw, hri_systemcontrol_shcsr_reg_t mask)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->SHCSR.reg &= ~mask;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_toggle_SHCSR_reg(const void *const hw, hri_systemcontrol_shcsr_reg_t mask)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->SHCSR.reg ^= mask;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline hri_systemcontrol_shcsr_reg_t hri_systemcontrol_read_SHCSR_reg(const void *const hw)
{
	return ((Systemcontrol *)hw)->SHCSR.reg;
}

static inline void hri_systemcontrol_set_CFSR_IACCVIOL_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->CFSR.reg |= SystemControl_CFSR_IACCVIOL;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_systemcontrol_get_CFSR_IACCVIOL_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Systemcontrol *)hw)->CFSR.reg;
	tmp = (tmp & SystemControl_CFSR_IACCVIOL) >> 0;
	return (bool)tmp;
}

static inline void hri_systemcontrol_write_CFSR_IACCVIOL_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	SystemControl_CRITICAL_SECTION_ENTER();
	tmp = ((Systemcontrol *)hw)->CFSR.reg;
	tmp &= ~SystemControl_CFSR_IACCVIOL;
	tmp |= value << 0;
	((Systemcontrol *)hw)->CFSR.reg = tmp;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_clear_CFSR_IACCVIOL_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->CFSR.reg &= ~SystemControl_CFSR_IACCVIOL;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_toggle_CFSR_IACCVIOL_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->CFSR.reg ^= SystemControl_CFSR_IACCVIOL;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_set_CFSR_DACCVIOL_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->CFSR.reg |= SystemControl_CFSR_DACCVIOL;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_systemcontrol_get_CFSR_DACCVIOL_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Systemcontrol *)hw)->CFSR.reg;
	tmp = (tmp & SystemControl_CFSR_DACCVIOL) >> 1;
	return (bool)tmp;
}

static inline void hri_systemcontrol_write_CFSR_DACCVIOL_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	SystemControl_CRITICAL_SECTION_ENTER();
	tmp = ((Systemcontrol *)hw)->CFSR.reg;
	tmp &= ~SystemControl_CFSR_DACCVIOL;
	tmp |= value << 1;
	((Systemcontrol *)hw)->CFSR.reg = tmp;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_clear_CFSR_DACCVIOL_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->CFSR.reg &= ~SystemControl_CFSR_DACCVIOL;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_toggle_CFSR_DACCVIOL_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->CFSR.reg ^= SystemControl_CFSR_DACCVIOL;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_set_CFSR_MUNSTKERR_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->CFSR.reg |= SystemControl_CFSR_MUNSTKERR;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_systemcontrol_get_CFSR_MUNSTKERR_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Systemcontrol *)hw)->CFSR.reg;
	tmp = (tmp & SystemControl_CFSR_MUNSTKERR) >> 3;
	return (bool)tmp;
}

static inline void hri_systemcontrol_write_CFSR_MUNSTKERR_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	SystemControl_CRITICAL_SECTION_ENTER();
	tmp = ((Systemcontrol *)hw)->CFSR.reg;
	tmp &= ~SystemControl_CFSR_MUNSTKERR;
	tmp |= value << 3;
	((Systemcontrol *)hw)->CFSR.reg = tmp;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_clear_CFSR_MUNSTKERR_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->CFSR.reg &= ~SystemControl_CFSR_MUNSTKERR;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_toggle_CFSR_MUNSTKERR_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->CFSR.reg ^= SystemControl_CFSR_MUNSTKERR;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_set_CFSR_MSTKERR_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->CFSR.reg |= SystemControl_CFSR_MSTKERR;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_systemcontrol_get_CFSR_MSTKERR_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Systemcontrol *)hw)->CFSR.reg;
	tmp = (tmp & SystemControl_CFSR_MSTKERR) >> 4;
	return (bool)tmp;
}

static inline void hri_systemcontrol_write_CFSR_MSTKERR_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	SystemControl_CRITICAL_SECTION_ENTER();
	tmp = ((Systemcontrol *)hw)->CFSR.reg;
	tmp &= ~SystemControl_CFSR_MSTKERR;
	tmp |= value << 4;
	((Systemcontrol *)hw)->CFSR.reg = tmp;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_clear_CFSR_MSTKERR_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->CFSR.reg &= ~SystemControl_CFSR_MSTKERR;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_toggle_CFSR_MSTKERR_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->CFSR.reg ^= SystemControl_CFSR_MSTKERR;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_set_CFSR_MLSPERR_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->CFSR.reg |= SystemControl_CFSR_MLSPERR;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_systemcontrol_get_CFSR_MLSPERR_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Systemcontrol *)hw)->CFSR.reg;
	tmp = (tmp & SystemControl_CFSR_MLSPERR) >> 5;
	return (bool)tmp;
}

static inline void hri_systemcontrol_write_CFSR_MLSPERR_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	SystemControl_CRITICAL_SECTION_ENTER();
	tmp = ((Systemcontrol *)hw)->CFSR.reg;
	tmp &= ~SystemControl_CFSR_MLSPERR;
	tmp |= value << 5;
	((Systemcontrol *)hw)->CFSR.reg = tmp;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_clear_CFSR_MLSPERR_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->CFSR.reg &= ~SystemControl_CFSR_MLSPERR;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_toggle_CFSR_MLSPERR_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->CFSR.reg ^= SystemControl_CFSR_MLSPERR;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_set_CFSR_MMARVALID_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->CFSR.reg |= SystemControl_CFSR_MMARVALID;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_systemcontrol_get_CFSR_MMARVALID_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Systemcontrol *)hw)->CFSR.reg;
	tmp = (tmp & SystemControl_CFSR_MMARVALID) >> 7;
	return (bool)tmp;
}

static inline void hri_systemcontrol_write_CFSR_MMARVALID_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	SystemControl_CRITICAL_SECTION_ENTER();
	tmp = ((Systemcontrol *)hw)->CFSR.reg;
	tmp &= ~SystemControl_CFSR_MMARVALID;
	tmp |= value << 7;
	((Systemcontrol *)hw)->CFSR.reg = tmp;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_clear_CFSR_MMARVALID_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->CFSR.reg &= ~SystemControl_CFSR_MMARVALID;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_toggle_CFSR_MMARVALID_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->CFSR.reg ^= SystemControl_CFSR_MMARVALID;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_set_CFSR_IBUSERR_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->CFSR.reg |= SystemControl_CFSR_IBUSERR;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_systemcontrol_get_CFSR_IBUSERR_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Systemcontrol *)hw)->CFSR.reg;
	tmp = (tmp & SystemControl_CFSR_IBUSERR) >> 8;
	return (bool)tmp;
}

static inline void hri_systemcontrol_write_CFSR_IBUSERR_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	SystemControl_CRITICAL_SECTION_ENTER();
	tmp = ((Systemcontrol *)hw)->CFSR.reg;
	tmp &= ~SystemControl_CFSR_IBUSERR;
	tmp |= value << 8;
	((Systemcontrol *)hw)->CFSR.reg = tmp;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_clear_CFSR_IBUSERR_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->CFSR.reg &= ~SystemControl_CFSR_IBUSERR;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_toggle_CFSR_IBUSERR_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->CFSR.reg ^= SystemControl_CFSR_IBUSERR;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_set_CFSR_PRECISERR_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->CFSR.reg |= SystemControl_CFSR_PRECISERR;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_systemcontrol_get_CFSR_PRECISERR_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Systemcontrol *)hw)->CFSR.reg;
	tmp = (tmp & SystemControl_CFSR_PRECISERR) >> 9;
	return (bool)tmp;
}

static inline void hri_systemcontrol_write_CFSR_PRECISERR_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	SystemControl_CRITICAL_SECTION_ENTER();
	tmp = ((Systemcontrol *)hw)->CFSR.reg;
	tmp &= ~SystemControl_CFSR_PRECISERR;
	tmp |= value << 9;
	((Systemcontrol *)hw)->CFSR.reg = tmp;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_clear_CFSR_PRECISERR_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->CFSR.reg &= ~SystemControl_CFSR_PRECISERR;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_toggle_CFSR_PRECISERR_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->CFSR.reg ^= SystemControl_CFSR_PRECISERR;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_set_CFSR_IMPRECISERR_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->CFSR.reg |= SystemControl_CFSR_IMPRECISERR;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_systemcontrol_get_CFSR_IMPRECISERR_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Systemcontrol *)hw)->CFSR.reg;
	tmp = (tmp & SystemControl_CFSR_IMPRECISERR) >> 10;
	return (bool)tmp;
}

static inline void hri_systemcontrol_write_CFSR_IMPRECISERR_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	SystemControl_CRITICAL_SECTION_ENTER();
	tmp = ((Systemcontrol *)hw)->CFSR.reg;
	tmp &= ~SystemControl_CFSR_IMPRECISERR;
	tmp |= value << 10;
	((Systemcontrol *)hw)->CFSR.reg = tmp;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_clear_CFSR_IMPRECISERR_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->CFSR.reg &= ~SystemControl_CFSR_IMPRECISERR;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_toggle_CFSR_IMPRECISERR_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->CFSR.reg ^= SystemControl_CFSR_IMPRECISERR;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_set_CFSR_UNSTKERR_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->CFSR.reg |= SystemControl_CFSR_UNSTKERR;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_systemcontrol_get_CFSR_UNSTKERR_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Systemcontrol *)hw)->CFSR.reg;
	tmp = (tmp & SystemControl_CFSR_UNSTKERR) >> 11;
	return (bool)tmp;
}

static inline void hri_systemcontrol_write_CFSR_UNSTKERR_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	SystemControl_CRITICAL_SECTION_ENTER();
	tmp = ((Systemcontrol *)hw)->CFSR.reg;
	tmp &= ~SystemControl_CFSR_UNSTKERR;
	tmp |= value << 11;
	((Systemcontrol *)hw)->CFSR.reg = tmp;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_clear_CFSR_UNSTKERR_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->CFSR.reg &= ~SystemControl_CFSR_UNSTKERR;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_toggle_CFSR_UNSTKERR_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->CFSR.reg ^= SystemControl_CFSR_UNSTKERR;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_set_CFSR_STKERR_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->CFSR.reg |= SystemControl_CFSR_STKERR;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_systemcontrol_get_CFSR_STKERR_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Systemcontrol *)hw)->CFSR.reg;
	tmp = (tmp & SystemControl_CFSR_STKERR) >> 12;
	return (bool)tmp;
}

static inline void hri_systemcontrol_write_CFSR_STKERR_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	SystemControl_CRITICAL_SECTION_ENTER();
	tmp = ((Systemcontrol *)hw)->CFSR.reg;
	tmp &= ~SystemControl_CFSR_STKERR;
	tmp |= value << 12;
	((Systemcontrol *)hw)->CFSR.reg = tmp;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_clear_CFSR_STKERR_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->CFSR.reg &= ~SystemControl_CFSR_STKERR;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_toggle_CFSR_STKERR_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->CFSR.reg ^= SystemControl_CFSR_STKERR;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_set_CFSR_LSPERR_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->CFSR.reg |= SystemControl_CFSR_LSPERR;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_systemcontrol_get_CFSR_LSPERR_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Systemcontrol *)hw)->CFSR.reg;
	tmp = (tmp & SystemControl_CFSR_LSPERR) >> 13;
	return (bool)tmp;
}

static inline void hri_systemcontrol_write_CFSR_LSPERR_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	SystemControl_CRITICAL_SECTION_ENTER();
	tmp = ((Systemcontrol *)hw)->CFSR.reg;
	tmp &= ~SystemControl_CFSR_LSPERR;
	tmp |= value << 13;
	((Systemcontrol *)hw)->CFSR.reg = tmp;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_clear_CFSR_LSPERR_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->CFSR.reg &= ~SystemControl_CFSR_LSPERR;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_toggle_CFSR_LSPERR_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->CFSR.reg ^= SystemControl_CFSR_LSPERR;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_set_CFSR_BFARVALID_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->CFSR.reg |= SystemControl_CFSR_BFARVALID;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_systemcontrol_get_CFSR_BFARVALID_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Systemcontrol *)hw)->CFSR.reg;
	tmp = (tmp & SystemControl_CFSR_BFARVALID) >> 15;
	return (bool)tmp;
}

static inline void hri_systemcontrol_write_CFSR_BFARVALID_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	SystemControl_CRITICAL_SECTION_ENTER();
	tmp = ((Systemcontrol *)hw)->CFSR.reg;
	tmp &= ~SystemControl_CFSR_BFARVALID;
	tmp |= value << 15;
	((Systemcontrol *)hw)->CFSR.reg = tmp;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_clear_CFSR_BFARVALID_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->CFSR.reg &= ~SystemControl_CFSR_BFARVALID;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_toggle_CFSR_BFARVALID_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->CFSR.reg ^= SystemControl_CFSR_BFARVALID;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_set_CFSR_UNDEFINSTR_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->CFSR.reg |= SystemControl_CFSR_UNDEFINSTR;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_systemcontrol_get_CFSR_UNDEFINSTR_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Systemcontrol *)hw)->CFSR.reg;
	tmp = (tmp & SystemControl_CFSR_UNDEFINSTR) >> 16;
	return (bool)tmp;
}

static inline void hri_systemcontrol_write_CFSR_UNDEFINSTR_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	SystemControl_CRITICAL_SECTION_ENTER();
	tmp = ((Systemcontrol *)hw)->CFSR.reg;
	tmp &= ~SystemControl_CFSR_UNDEFINSTR;
	tmp |= value << 16;
	((Systemcontrol *)hw)->CFSR.reg = tmp;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_clear_CFSR_UNDEFINSTR_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->CFSR.reg &= ~SystemControl_CFSR_UNDEFINSTR;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_toggle_CFSR_UNDEFINSTR_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->CFSR.reg ^= SystemControl_CFSR_UNDEFINSTR;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_set_CFSR_INVSTATE_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->CFSR.reg |= SystemControl_CFSR_INVSTATE;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_systemcontrol_get_CFSR_INVSTATE_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Systemcontrol *)hw)->CFSR.reg;
	tmp = (tmp & SystemControl_CFSR_INVSTATE) >> 17;
	return (bool)tmp;
}

static inline void hri_systemcontrol_write_CFSR_INVSTATE_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	SystemControl_CRITICAL_SECTION_ENTER();
	tmp = ((Systemcontrol *)hw)->CFSR.reg;
	tmp &= ~SystemControl_CFSR_INVSTATE;
	tmp |= value << 17;
	((Systemcontrol *)hw)->CFSR.reg = tmp;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_clear_CFSR_INVSTATE_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->CFSR.reg &= ~SystemControl_CFSR_INVSTATE;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_toggle_CFSR_INVSTATE_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->CFSR.reg ^= SystemControl_CFSR_INVSTATE;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_set_CFSR_INVPC_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->CFSR.reg |= SystemControl_CFSR_INVPC;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_systemcontrol_get_CFSR_INVPC_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Systemcontrol *)hw)->CFSR.reg;
	tmp = (tmp & SystemControl_CFSR_INVPC) >> 18;
	return (bool)tmp;
}

static inline void hri_systemcontrol_write_CFSR_INVPC_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	SystemControl_CRITICAL_SECTION_ENTER();
	tmp = ((Systemcontrol *)hw)->CFSR.reg;
	tmp &= ~SystemControl_CFSR_INVPC;
	tmp |= value << 18;
	((Systemcontrol *)hw)->CFSR.reg = tmp;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_clear_CFSR_INVPC_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->CFSR.reg &= ~SystemControl_CFSR_INVPC;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_toggle_CFSR_INVPC_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->CFSR.reg ^= SystemControl_CFSR_INVPC;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_set_CFSR_NOCP_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->CFSR.reg |= SystemControl_CFSR_NOCP;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_systemcontrol_get_CFSR_NOCP_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Systemcontrol *)hw)->CFSR.reg;
	tmp = (tmp & SystemControl_CFSR_NOCP) >> 19;
	return (bool)tmp;
}

static inline void hri_systemcontrol_write_CFSR_NOCP_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	SystemControl_CRITICAL_SECTION_ENTER();
	tmp = ((Systemcontrol *)hw)->CFSR.reg;
	tmp &= ~SystemControl_CFSR_NOCP;
	tmp |= value << 19;
	((Systemcontrol *)hw)->CFSR.reg = tmp;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_clear_CFSR_NOCP_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->CFSR.reg &= ~SystemControl_CFSR_NOCP;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_toggle_CFSR_NOCP_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->CFSR.reg ^= SystemControl_CFSR_NOCP;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_set_CFSR_UNALIGNED_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->CFSR.reg |= SystemControl_CFSR_UNALIGNED;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_systemcontrol_get_CFSR_UNALIGNED_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Systemcontrol *)hw)->CFSR.reg;
	tmp = (tmp & SystemControl_CFSR_UNALIGNED) >> 24;
	return (bool)tmp;
}

static inline void hri_systemcontrol_write_CFSR_UNALIGNED_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	SystemControl_CRITICAL_SECTION_ENTER();
	tmp = ((Systemcontrol *)hw)->CFSR.reg;
	tmp &= ~SystemControl_CFSR_UNALIGNED;
	tmp |= value << 24;
	((Systemcontrol *)hw)->CFSR.reg = tmp;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_clear_CFSR_UNALIGNED_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->CFSR.reg &= ~SystemControl_CFSR_UNALIGNED;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_toggle_CFSR_UNALIGNED_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->CFSR.reg ^= SystemControl_CFSR_UNALIGNED;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_set_CFSR_DIVBYZERO_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->CFSR.reg |= SystemControl_CFSR_DIVBYZERO;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_systemcontrol_get_CFSR_DIVBYZERO_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Systemcontrol *)hw)->CFSR.reg;
	tmp = (tmp & SystemControl_CFSR_DIVBYZERO) >> 25;
	return (bool)tmp;
}

static inline void hri_systemcontrol_write_CFSR_DIVBYZERO_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	SystemControl_CRITICAL_SECTION_ENTER();
	tmp = ((Systemcontrol *)hw)->CFSR.reg;
	tmp &= ~SystemControl_CFSR_DIVBYZERO;
	tmp |= value << 25;
	((Systemcontrol *)hw)->CFSR.reg = tmp;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_clear_CFSR_DIVBYZERO_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->CFSR.reg &= ~SystemControl_CFSR_DIVBYZERO;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_toggle_CFSR_DIVBYZERO_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->CFSR.reg ^= SystemControl_CFSR_DIVBYZERO;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_set_CFSR_reg(const void *const hw, hri_systemcontrol_cfsr_reg_t mask)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->CFSR.reg |= mask;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline hri_systemcontrol_cfsr_reg_t hri_systemcontrol_get_CFSR_reg(const void *const            hw,
                                                                          hri_systemcontrol_cfsr_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Systemcontrol *)hw)->CFSR.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_systemcontrol_write_CFSR_reg(const void *const hw, hri_systemcontrol_cfsr_reg_t data)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->CFSR.reg = data;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_clear_CFSR_reg(const void *const hw, hri_systemcontrol_cfsr_reg_t mask)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->CFSR.reg &= ~mask;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_toggle_CFSR_reg(const void *const hw, hri_systemcontrol_cfsr_reg_t mask)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->CFSR.reg ^= mask;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline hri_systemcontrol_cfsr_reg_t hri_systemcontrol_read_CFSR_reg(const void *const hw)
{
	return ((Systemcontrol *)hw)->CFSR.reg;
}

static inline void hri_systemcontrol_set_HFSR_VECTTBL_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->HFSR.reg |= SystemControl_HFSR_VECTTBL;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_systemcontrol_get_HFSR_VECTTBL_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Systemcontrol *)hw)->HFSR.reg;
	tmp = (tmp & SystemControl_HFSR_VECTTBL) >> 1;
	return (bool)tmp;
}

static inline void hri_systemcontrol_write_HFSR_VECTTBL_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	SystemControl_CRITICAL_SECTION_ENTER();
	tmp = ((Systemcontrol *)hw)->HFSR.reg;
	tmp &= ~SystemControl_HFSR_VECTTBL;
	tmp |= value << 1;
	((Systemcontrol *)hw)->HFSR.reg = tmp;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_clear_HFSR_VECTTBL_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->HFSR.reg &= ~SystemControl_HFSR_VECTTBL;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_toggle_HFSR_VECTTBL_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->HFSR.reg ^= SystemControl_HFSR_VECTTBL;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_set_HFSR_FORCED_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->HFSR.reg |= SystemControl_HFSR_FORCED;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_systemcontrol_get_HFSR_FORCED_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Systemcontrol *)hw)->HFSR.reg;
	tmp = (tmp & SystemControl_HFSR_FORCED) >> 30;
	return (bool)tmp;
}

static inline void hri_systemcontrol_write_HFSR_FORCED_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	SystemControl_CRITICAL_SECTION_ENTER();
	tmp = ((Systemcontrol *)hw)->HFSR.reg;
	tmp &= ~SystemControl_HFSR_FORCED;
	tmp |= value << 30;
	((Systemcontrol *)hw)->HFSR.reg = tmp;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_clear_HFSR_FORCED_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->HFSR.reg &= ~SystemControl_HFSR_FORCED;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_toggle_HFSR_FORCED_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->HFSR.reg ^= SystemControl_HFSR_FORCED;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_set_HFSR_DEBUGEVT_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->HFSR.reg |= SystemControl_HFSR_DEBUGEVT;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_systemcontrol_get_HFSR_DEBUGEVT_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Systemcontrol *)hw)->HFSR.reg;
	tmp = (tmp & SystemControl_HFSR_DEBUGEVT) >> 31;
	return (bool)tmp;
}

static inline void hri_systemcontrol_write_HFSR_DEBUGEVT_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	SystemControl_CRITICAL_SECTION_ENTER();
	tmp = ((Systemcontrol *)hw)->HFSR.reg;
	tmp &= ~SystemControl_HFSR_DEBUGEVT;
	tmp |= value << 31;
	((Systemcontrol *)hw)->HFSR.reg = tmp;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_clear_HFSR_DEBUGEVT_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->HFSR.reg &= ~SystemControl_HFSR_DEBUGEVT;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_toggle_HFSR_DEBUGEVT_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->HFSR.reg ^= SystemControl_HFSR_DEBUGEVT;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_set_HFSR_reg(const void *const hw, hri_systemcontrol_hfsr_reg_t mask)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->HFSR.reg |= mask;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline hri_systemcontrol_hfsr_reg_t hri_systemcontrol_get_HFSR_reg(const void *const            hw,
                                                                          hri_systemcontrol_hfsr_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Systemcontrol *)hw)->HFSR.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_systemcontrol_write_HFSR_reg(const void *const hw, hri_systemcontrol_hfsr_reg_t data)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->HFSR.reg = data;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_clear_HFSR_reg(const void *const hw, hri_systemcontrol_hfsr_reg_t mask)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->HFSR.reg &= ~mask;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_toggle_HFSR_reg(const void *const hw, hri_systemcontrol_hfsr_reg_t mask)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->HFSR.reg ^= mask;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline hri_systemcontrol_hfsr_reg_t hri_systemcontrol_read_HFSR_reg(const void *const hw)
{
	return ((Systemcontrol *)hw)->HFSR.reg;
}

static inline void hri_systemcontrol_set_DFSR_HALTED_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->DFSR.reg |= SystemControl_DFSR_HALTED;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_systemcontrol_get_DFSR_HALTED_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Systemcontrol *)hw)->DFSR.reg;
	tmp = (tmp & SystemControl_DFSR_HALTED) >> 0;
	return (bool)tmp;
}

static inline void hri_systemcontrol_write_DFSR_HALTED_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	SystemControl_CRITICAL_SECTION_ENTER();
	tmp = ((Systemcontrol *)hw)->DFSR.reg;
	tmp &= ~SystemControl_DFSR_HALTED;
	tmp |= value << 0;
	((Systemcontrol *)hw)->DFSR.reg = tmp;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_clear_DFSR_HALTED_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->DFSR.reg &= ~SystemControl_DFSR_HALTED;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_toggle_DFSR_HALTED_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->DFSR.reg ^= SystemControl_DFSR_HALTED;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_set_DFSR_BKPT_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->DFSR.reg |= SystemControl_DFSR_BKPT;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_systemcontrol_get_DFSR_BKPT_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Systemcontrol *)hw)->DFSR.reg;
	tmp = (tmp & SystemControl_DFSR_BKPT) >> 1;
	return (bool)tmp;
}

static inline void hri_systemcontrol_write_DFSR_BKPT_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	SystemControl_CRITICAL_SECTION_ENTER();
	tmp = ((Systemcontrol *)hw)->DFSR.reg;
	tmp &= ~SystemControl_DFSR_BKPT;
	tmp |= value << 1;
	((Systemcontrol *)hw)->DFSR.reg = tmp;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_clear_DFSR_BKPT_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->DFSR.reg &= ~SystemControl_DFSR_BKPT;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_toggle_DFSR_BKPT_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->DFSR.reg ^= SystemControl_DFSR_BKPT;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_set_DFSR_DWTTRAP_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->DFSR.reg |= SystemControl_DFSR_DWTTRAP;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_systemcontrol_get_DFSR_DWTTRAP_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Systemcontrol *)hw)->DFSR.reg;
	tmp = (tmp & SystemControl_DFSR_DWTTRAP) >> 2;
	return (bool)tmp;
}

static inline void hri_systemcontrol_write_DFSR_DWTTRAP_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	SystemControl_CRITICAL_SECTION_ENTER();
	tmp = ((Systemcontrol *)hw)->DFSR.reg;
	tmp &= ~SystemControl_DFSR_DWTTRAP;
	tmp |= value << 2;
	((Systemcontrol *)hw)->DFSR.reg = tmp;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_clear_DFSR_DWTTRAP_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->DFSR.reg &= ~SystemControl_DFSR_DWTTRAP;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_toggle_DFSR_DWTTRAP_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->DFSR.reg ^= SystemControl_DFSR_DWTTRAP;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_set_DFSR_VCATCH_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->DFSR.reg |= SystemControl_DFSR_VCATCH;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_systemcontrol_get_DFSR_VCATCH_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Systemcontrol *)hw)->DFSR.reg;
	tmp = (tmp & SystemControl_DFSR_VCATCH) >> 3;
	return (bool)tmp;
}

static inline void hri_systemcontrol_write_DFSR_VCATCH_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	SystemControl_CRITICAL_SECTION_ENTER();
	tmp = ((Systemcontrol *)hw)->DFSR.reg;
	tmp &= ~SystemControl_DFSR_VCATCH;
	tmp |= value << 3;
	((Systemcontrol *)hw)->DFSR.reg = tmp;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_clear_DFSR_VCATCH_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->DFSR.reg &= ~SystemControl_DFSR_VCATCH;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_toggle_DFSR_VCATCH_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->DFSR.reg ^= SystemControl_DFSR_VCATCH;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_set_DFSR_EXTERNAL_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->DFSR.reg |= SystemControl_DFSR_EXTERNAL;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_systemcontrol_get_DFSR_EXTERNAL_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Systemcontrol *)hw)->DFSR.reg;
	tmp = (tmp & SystemControl_DFSR_EXTERNAL) >> 4;
	return (bool)tmp;
}

static inline void hri_systemcontrol_write_DFSR_EXTERNAL_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	SystemControl_CRITICAL_SECTION_ENTER();
	tmp = ((Systemcontrol *)hw)->DFSR.reg;
	tmp &= ~SystemControl_DFSR_EXTERNAL;
	tmp |= value << 4;
	((Systemcontrol *)hw)->DFSR.reg = tmp;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_clear_DFSR_EXTERNAL_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->DFSR.reg &= ~SystemControl_DFSR_EXTERNAL;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_toggle_DFSR_EXTERNAL_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->DFSR.reg ^= SystemControl_DFSR_EXTERNAL;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_set_DFSR_reg(const void *const hw, hri_systemcontrol_dfsr_reg_t mask)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->DFSR.reg |= mask;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline hri_systemcontrol_dfsr_reg_t hri_systemcontrol_get_DFSR_reg(const void *const            hw,
                                                                          hri_systemcontrol_dfsr_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Systemcontrol *)hw)->DFSR.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_systemcontrol_write_DFSR_reg(const void *const hw, hri_systemcontrol_dfsr_reg_t data)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->DFSR.reg = data;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_clear_DFSR_reg(const void *const hw, hri_systemcontrol_dfsr_reg_t mask)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->DFSR.reg &= ~mask;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_toggle_DFSR_reg(const void *const hw, hri_systemcontrol_dfsr_reg_t mask)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->DFSR.reg ^= mask;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline hri_systemcontrol_dfsr_reg_t hri_systemcontrol_read_DFSR_reg(const void *const hw)
{
	return ((Systemcontrol *)hw)->DFSR.reg;
}

static inline void hri_systemcontrol_set_MMFAR_ADDRESS_bf(const void *const hw, hri_systemcontrol_mmfar_reg_t mask)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->MMFAR.reg |= SystemControl_MMFAR_ADDRESS(mask);
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline hri_systemcontrol_mmfar_reg_t hri_systemcontrol_get_MMFAR_ADDRESS_bf(const void *const             hw,
                                                                                   hri_systemcontrol_mmfar_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Systemcontrol *)hw)->MMFAR.reg;
	tmp = (tmp & SystemControl_MMFAR_ADDRESS(mask)) >> 0;
	return tmp;
}

static inline void hri_systemcontrol_write_MMFAR_ADDRESS_bf(const void *const hw, hri_systemcontrol_mmfar_reg_t data)
{
	uint32_t tmp;
	SystemControl_CRITICAL_SECTION_ENTER();
	tmp = ((Systemcontrol *)hw)->MMFAR.reg;
	tmp &= ~SystemControl_MMFAR_ADDRESS_Msk;
	tmp |= SystemControl_MMFAR_ADDRESS(data);
	((Systemcontrol *)hw)->MMFAR.reg = tmp;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_clear_MMFAR_ADDRESS_bf(const void *const hw, hri_systemcontrol_mmfar_reg_t mask)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->MMFAR.reg &= ~SystemControl_MMFAR_ADDRESS(mask);
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_toggle_MMFAR_ADDRESS_bf(const void *const hw, hri_systemcontrol_mmfar_reg_t mask)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->MMFAR.reg ^= SystemControl_MMFAR_ADDRESS(mask);
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline hri_systemcontrol_mmfar_reg_t hri_systemcontrol_read_MMFAR_ADDRESS_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Systemcontrol *)hw)->MMFAR.reg;
	tmp = (tmp & SystemControl_MMFAR_ADDRESS_Msk) >> 0;
	return tmp;
}

static inline void hri_systemcontrol_set_MMFAR_reg(const void *const hw, hri_systemcontrol_mmfar_reg_t mask)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->MMFAR.reg |= mask;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline hri_systemcontrol_mmfar_reg_t hri_systemcontrol_get_MMFAR_reg(const void *const             hw,
                                                                            hri_systemcontrol_mmfar_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Systemcontrol *)hw)->MMFAR.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_systemcontrol_write_MMFAR_reg(const void *const hw, hri_systemcontrol_mmfar_reg_t data)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->MMFAR.reg = data;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_clear_MMFAR_reg(const void *const hw, hri_systemcontrol_mmfar_reg_t mask)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->MMFAR.reg &= ~mask;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_toggle_MMFAR_reg(const void *const hw, hri_systemcontrol_mmfar_reg_t mask)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->MMFAR.reg ^= mask;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline hri_systemcontrol_mmfar_reg_t hri_systemcontrol_read_MMFAR_reg(const void *const hw)
{
	return ((Systemcontrol *)hw)->MMFAR.reg;
}

static inline void hri_systemcontrol_set_BFAR_ADDRESS_bf(const void *const hw, hri_systemcontrol_bfar_reg_t mask)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->BFAR.reg |= SystemControl_BFAR_ADDRESS(mask);
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline hri_systemcontrol_bfar_reg_t hri_systemcontrol_get_BFAR_ADDRESS_bf(const void *const            hw,
                                                                                 hri_systemcontrol_bfar_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Systemcontrol *)hw)->BFAR.reg;
	tmp = (tmp & SystemControl_BFAR_ADDRESS(mask)) >> 0;
	return tmp;
}

static inline void hri_systemcontrol_write_BFAR_ADDRESS_bf(const void *const hw, hri_systemcontrol_bfar_reg_t data)
{
	uint32_t tmp;
	SystemControl_CRITICAL_SECTION_ENTER();
	tmp = ((Systemcontrol *)hw)->BFAR.reg;
	tmp &= ~SystemControl_BFAR_ADDRESS_Msk;
	tmp |= SystemControl_BFAR_ADDRESS(data);
	((Systemcontrol *)hw)->BFAR.reg = tmp;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_clear_BFAR_ADDRESS_bf(const void *const hw, hri_systemcontrol_bfar_reg_t mask)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->BFAR.reg &= ~SystemControl_BFAR_ADDRESS(mask);
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_toggle_BFAR_ADDRESS_bf(const void *const hw, hri_systemcontrol_bfar_reg_t mask)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->BFAR.reg ^= SystemControl_BFAR_ADDRESS(mask);
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline hri_systemcontrol_bfar_reg_t hri_systemcontrol_read_BFAR_ADDRESS_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Systemcontrol *)hw)->BFAR.reg;
	tmp = (tmp & SystemControl_BFAR_ADDRESS_Msk) >> 0;
	return tmp;
}

static inline void hri_systemcontrol_set_BFAR_reg(const void *const hw, hri_systemcontrol_bfar_reg_t mask)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->BFAR.reg |= mask;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline hri_systemcontrol_bfar_reg_t hri_systemcontrol_get_BFAR_reg(const void *const            hw,
                                                                          hri_systemcontrol_bfar_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Systemcontrol *)hw)->BFAR.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_systemcontrol_write_BFAR_reg(const void *const hw, hri_systemcontrol_bfar_reg_t data)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->BFAR.reg = data;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_clear_BFAR_reg(const void *const hw, hri_systemcontrol_bfar_reg_t mask)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->BFAR.reg &= ~mask;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_toggle_BFAR_reg(const void *const hw, hri_systemcontrol_bfar_reg_t mask)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->BFAR.reg ^= mask;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline hri_systemcontrol_bfar_reg_t hri_systemcontrol_read_BFAR_reg(const void *const hw)
{
	return ((Systemcontrol *)hw)->BFAR.reg;
}

static inline void hri_systemcontrol_set_AFSR_AUXFAULT_bf(const void *const hw, hri_systemcontrol_afsr_reg_t mask)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->AFSR.reg |= SystemControl_AFSR_AUXFAULT(mask);
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline hri_systemcontrol_afsr_reg_t hri_systemcontrol_get_AFSR_AUXFAULT_bf(const void *const            hw,
                                                                                  hri_systemcontrol_afsr_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Systemcontrol *)hw)->AFSR.reg;
	tmp = (tmp & SystemControl_AFSR_AUXFAULT(mask)) >> 0;
	return tmp;
}

static inline void hri_systemcontrol_write_AFSR_AUXFAULT_bf(const void *const hw, hri_systemcontrol_afsr_reg_t data)
{
	uint32_t tmp;
	SystemControl_CRITICAL_SECTION_ENTER();
	tmp = ((Systemcontrol *)hw)->AFSR.reg;
	tmp &= ~SystemControl_AFSR_AUXFAULT_Msk;
	tmp |= SystemControl_AFSR_AUXFAULT(data);
	((Systemcontrol *)hw)->AFSR.reg = tmp;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_clear_AFSR_AUXFAULT_bf(const void *const hw, hri_systemcontrol_afsr_reg_t mask)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->AFSR.reg &= ~SystemControl_AFSR_AUXFAULT(mask);
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_toggle_AFSR_AUXFAULT_bf(const void *const hw, hri_systemcontrol_afsr_reg_t mask)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->AFSR.reg ^= SystemControl_AFSR_AUXFAULT(mask);
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline hri_systemcontrol_afsr_reg_t hri_systemcontrol_read_AFSR_AUXFAULT_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Systemcontrol *)hw)->AFSR.reg;
	tmp = (tmp & SystemControl_AFSR_AUXFAULT_Msk) >> 0;
	return tmp;
}

static inline void hri_systemcontrol_set_AFSR_reg(const void *const hw, hri_systemcontrol_afsr_reg_t mask)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->AFSR.reg |= mask;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline hri_systemcontrol_afsr_reg_t hri_systemcontrol_get_AFSR_reg(const void *const            hw,
                                                                          hri_systemcontrol_afsr_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Systemcontrol *)hw)->AFSR.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_systemcontrol_write_AFSR_reg(const void *const hw, hri_systemcontrol_afsr_reg_t data)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->AFSR.reg = data;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_clear_AFSR_reg(const void *const hw, hri_systemcontrol_afsr_reg_t mask)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->AFSR.reg &= ~mask;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_toggle_AFSR_reg(const void *const hw, hri_systemcontrol_afsr_reg_t mask)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->AFSR.reg ^= mask;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline hri_systemcontrol_afsr_reg_t hri_systemcontrol_read_AFSR_reg(const void *const hw)
{
	return ((Systemcontrol *)hw)->AFSR.reg;
}

#ifdef __cplusplus
}
#endif

#endif /* _HRI_SystemControl_E54_H_INCLUDED */
#endif /* _SAME54_SystemControl_COMPONENT_ */
