/**
 * \file
 *
 * \brief SAM SystemControl
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

#ifdef _SAML21_SystemControl_COMPONENT_
#ifndef _HRI_SystemControl_L21_H_INCLUDED_
#define _HRI_SystemControl_L21_H_INCLUDED_

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

typedef uint32_t hri_systemcontrol_aircr_reg_t;
typedef uint32_t hri_systemcontrol_ccr_reg_t;
typedef uint32_t hri_systemcontrol_cpuid_reg_t;
typedef uint32_t hri_systemcontrol_dfsr_reg_t;
typedef uint32_t hri_systemcontrol_icsr_reg_t;
typedef uint32_t hri_systemcontrol_scr_reg_t;
typedef uint32_t hri_systemcontrol_shcsr_reg_t;
typedef uint32_t hri_systemcontrol_shpr2_reg_t;
typedef uint32_t hri_systemcontrol_shpr3_reg_t;
typedef uint32_t hri_systemcontrol_vtor_reg_t;

static inline hri_systemcontrol_cpuid_reg_t hri_systemcontrol_get_CPUID_REVISION_bf(const void *const             hw,
                                                                                    hri_systemcontrol_cpuid_reg_t mask)
{
	return (((Systemcontrol *)hw)->CPUID.reg & SystemControl_CPUID_REVISION(mask)) >> 0;
}

static inline hri_systemcontrol_cpuid_reg_t hri_systemcontrol_read_CPUID_REVISION_bf(const void *const hw)
{
	return (((Systemcontrol *)hw)->CPUID.reg & SystemControl_CPUID_REVISION_Msk) >> 0;
}

static inline hri_systemcontrol_cpuid_reg_t hri_systemcontrol_get_CPUID_PARTNO_bf(const void *const             hw,
                                                                                  hri_systemcontrol_cpuid_reg_t mask)
{
	return (((Systemcontrol *)hw)->CPUID.reg & SystemControl_CPUID_PARTNO(mask)) >> 4;
}

static inline hri_systemcontrol_cpuid_reg_t hri_systemcontrol_read_CPUID_PARTNO_bf(const void *const hw)
{
	return (((Systemcontrol *)hw)->CPUID.reg & SystemControl_CPUID_PARTNO_Msk) >> 4;
}

static inline hri_systemcontrol_cpuid_reg_t
hri_systemcontrol_get_CPUID_ARCHITECTURE_bf(const void *const hw, hri_systemcontrol_cpuid_reg_t mask)
{
	return (((Systemcontrol *)hw)->CPUID.reg & SystemControl_CPUID_ARCHITECTURE(mask)) >> 16;
}

static inline hri_systemcontrol_cpuid_reg_t hri_systemcontrol_read_CPUID_ARCHITECTURE_bf(const void *const hw)
{
	return (((Systemcontrol *)hw)->CPUID.reg & SystemControl_CPUID_ARCHITECTURE_Msk) >> 16;
}

static inline hri_systemcontrol_cpuid_reg_t hri_systemcontrol_get_CPUID_VARIANT_bf(const void *const             hw,
                                                                                   hri_systemcontrol_cpuid_reg_t mask)
{
	return (((Systemcontrol *)hw)->CPUID.reg & SystemControl_CPUID_VARIANT(mask)) >> 20;
}

static inline hri_systemcontrol_cpuid_reg_t hri_systemcontrol_read_CPUID_VARIANT_bf(const void *const hw)
{
	return (((Systemcontrol *)hw)->CPUID.reg & SystemControl_CPUID_VARIANT_Msk) >> 20;
}

static inline hri_systemcontrol_cpuid_reg_t
hri_systemcontrol_get_CPUID_IMPLEMENTER_bf(const void *const hw, hri_systemcontrol_cpuid_reg_t mask)
{
	return (((Systemcontrol *)hw)->CPUID.reg & SystemControl_CPUID_IMPLEMENTER(mask)) >> 24;
}

static inline hri_systemcontrol_cpuid_reg_t hri_systemcontrol_read_CPUID_IMPLEMENTER_bf(const void *const hw)
{
	return (((Systemcontrol *)hw)->CPUID.reg & SystemControl_CPUID_IMPLEMENTER_Msk) >> 24;
}

static inline hri_systemcontrol_cpuid_reg_t hri_systemcontrol_get_CPUID_reg(const void *const             hw,
                                                                            hri_systemcontrol_cpuid_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Systemcontrol *)hw)->CPUID.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_systemcontrol_cpuid_reg_t hri_systemcontrol_read_CPUID_reg(const void *const hw)
{
	return ((Systemcontrol *)hw)->CPUID.reg;
}

static inline bool hri_systemcontrol_get_CCR_UNALIGN_TRP_bit(const void *const hw)
{
	return (((Systemcontrol *)hw)->CCR.reg & SystemControl_CCR_UNALIGN_TRP) >> 3;
}

static inline bool hri_systemcontrol_get_CCR_STKALIGN_bit(const void *const hw)
{
	return (((Systemcontrol *)hw)->CCR.reg & SystemControl_CCR_STKALIGN) >> 9;
}

static inline hri_systemcontrol_ccr_reg_t hri_systemcontrol_get_CCR_reg(const void *const           hw,
                                                                        hri_systemcontrol_ccr_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Systemcontrol *)hw)->CCR.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_systemcontrol_ccr_reg_t hri_systemcontrol_read_CCR_reg(const void *const hw)
{
	return ((Systemcontrol *)hw)->CCR.reg;
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

#ifdef __cplusplus
}
#endif

#endif /* _HRI_SystemControl_L21_H_INCLUDED */
#endif /* _SAML21_SystemControl_COMPONENT_ */
