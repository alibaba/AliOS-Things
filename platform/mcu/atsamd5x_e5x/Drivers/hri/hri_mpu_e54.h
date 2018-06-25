/**
 * \file
 *
 * \brief SAM MPU
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

#ifdef _SAME54_MPU_COMPONENT_
#ifndef _HRI_MPU_E54_H_INCLUDED_
#define _HRI_MPU_E54_H_INCLUDED_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <hal_atomic.h>

#if defined(ENABLE_MPU_CRITICAL_SECTIONS)
#define MPU_CRITICAL_SECTION_ENTER() CRITICAL_SECTION_ENTER()
#define MPU_CRITICAL_SECTION_LEAVE() CRITICAL_SECTION_LEAVE()
#else
#define MPU_CRITICAL_SECTION_ENTER()
#define MPU_CRITICAL_SECTION_LEAVE()
#endif

typedef uint32_t hri_mpu_ctrl_reg_t;
typedef uint32_t hri_mpu_rasr_reg_t;
typedef uint32_t hri_mpu_rbar_reg_t;
typedef uint32_t hri_mpu_rnr_reg_t;
typedef uint32_t hri_mpu_type_reg_t;

static inline void hri_mpu_set_CTRL_ENABLE_bit(const void *const hw)
{
	MPU_CRITICAL_SECTION_ENTER();
	((Mpu *)hw)->CTRL.reg |= MPU_CTRL_ENABLE;
	MPU_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_mpu_get_CTRL_ENABLE_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Mpu *)hw)->CTRL.reg;
	tmp = (tmp & MPU_CTRL_ENABLE) >> 0;
	return (bool)tmp;
}

static inline void hri_mpu_write_CTRL_ENABLE_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	MPU_CRITICAL_SECTION_ENTER();
	tmp = ((Mpu *)hw)->CTRL.reg;
	tmp &= ~MPU_CTRL_ENABLE;
	tmp |= value << 0;
	((Mpu *)hw)->CTRL.reg = tmp;
	MPU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mpu_clear_CTRL_ENABLE_bit(const void *const hw)
{
	MPU_CRITICAL_SECTION_ENTER();
	((Mpu *)hw)->CTRL.reg &= ~MPU_CTRL_ENABLE;
	MPU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mpu_toggle_CTRL_ENABLE_bit(const void *const hw)
{
	MPU_CRITICAL_SECTION_ENTER();
	((Mpu *)hw)->CTRL.reg ^= MPU_CTRL_ENABLE;
	MPU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mpu_set_CTRL_HFNMIENA_bit(const void *const hw)
{
	MPU_CRITICAL_SECTION_ENTER();
	((Mpu *)hw)->CTRL.reg |= MPU_CTRL_HFNMIENA;
	MPU_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_mpu_get_CTRL_HFNMIENA_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Mpu *)hw)->CTRL.reg;
	tmp = (tmp & MPU_CTRL_HFNMIENA) >> 1;
	return (bool)tmp;
}

static inline void hri_mpu_write_CTRL_HFNMIENA_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	MPU_CRITICAL_SECTION_ENTER();
	tmp = ((Mpu *)hw)->CTRL.reg;
	tmp &= ~MPU_CTRL_HFNMIENA;
	tmp |= value << 1;
	((Mpu *)hw)->CTRL.reg = tmp;
	MPU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mpu_clear_CTRL_HFNMIENA_bit(const void *const hw)
{
	MPU_CRITICAL_SECTION_ENTER();
	((Mpu *)hw)->CTRL.reg &= ~MPU_CTRL_HFNMIENA;
	MPU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mpu_toggle_CTRL_HFNMIENA_bit(const void *const hw)
{
	MPU_CRITICAL_SECTION_ENTER();
	((Mpu *)hw)->CTRL.reg ^= MPU_CTRL_HFNMIENA;
	MPU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mpu_set_CTRL_PRIVDEFENA_bit(const void *const hw)
{
	MPU_CRITICAL_SECTION_ENTER();
	((Mpu *)hw)->CTRL.reg |= MPU_CTRL_PRIVDEFENA;
	MPU_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_mpu_get_CTRL_PRIVDEFENA_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Mpu *)hw)->CTRL.reg;
	tmp = (tmp & MPU_CTRL_PRIVDEFENA) >> 2;
	return (bool)tmp;
}

static inline void hri_mpu_write_CTRL_PRIVDEFENA_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	MPU_CRITICAL_SECTION_ENTER();
	tmp = ((Mpu *)hw)->CTRL.reg;
	tmp &= ~MPU_CTRL_PRIVDEFENA;
	tmp |= value << 2;
	((Mpu *)hw)->CTRL.reg = tmp;
	MPU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mpu_clear_CTRL_PRIVDEFENA_bit(const void *const hw)
{
	MPU_CRITICAL_SECTION_ENTER();
	((Mpu *)hw)->CTRL.reg &= ~MPU_CTRL_PRIVDEFENA;
	MPU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mpu_toggle_CTRL_PRIVDEFENA_bit(const void *const hw)
{
	MPU_CRITICAL_SECTION_ENTER();
	((Mpu *)hw)->CTRL.reg ^= MPU_CTRL_PRIVDEFENA;
	MPU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mpu_set_CTRL_reg(const void *const hw, hri_mpu_ctrl_reg_t mask)
{
	MPU_CRITICAL_SECTION_ENTER();
	((Mpu *)hw)->CTRL.reg |= mask;
	MPU_CRITICAL_SECTION_LEAVE();
}

static inline hri_mpu_ctrl_reg_t hri_mpu_get_CTRL_reg(const void *const hw, hri_mpu_ctrl_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Mpu *)hw)->CTRL.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_mpu_write_CTRL_reg(const void *const hw, hri_mpu_ctrl_reg_t data)
{
	MPU_CRITICAL_SECTION_ENTER();
	((Mpu *)hw)->CTRL.reg = data;
	MPU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mpu_clear_CTRL_reg(const void *const hw, hri_mpu_ctrl_reg_t mask)
{
	MPU_CRITICAL_SECTION_ENTER();
	((Mpu *)hw)->CTRL.reg &= ~mask;
	MPU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mpu_toggle_CTRL_reg(const void *const hw, hri_mpu_ctrl_reg_t mask)
{
	MPU_CRITICAL_SECTION_ENTER();
	((Mpu *)hw)->CTRL.reg ^= mask;
	MPU_CRITICAL_SECTION_LEAVE();
}

static inline hri_mpu_ctrl_reg_t hri_mpu_read_CTRL_reg(const void *const hw)
{
	return ((Mpu *)hw)->CTRL.reg;
}

static inline void hri_mpu_set_RNR_REGION_bf(const void *const hw, hri_mpu_rnr_reg_t mask)
{
	MPU_CRITICAL_SECTION_ENTER();
	((Mpu *)hw)->RNR.reg |= MPU_RNR_REGION(mask);
	MPU_CRITICAL_SECTION_LEAVE();
}

static inline hri_mpu_rnr_reg_t hri_mpu_get_RNR_REGION_bf(const void *const hw, hri_mpu_rnr_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Mpu *)hw)->RNR.reg;
	tmp = (tmp & MPU_RNR_REGION(mask)) >> 0;
	return tmp;
}

static inline void hri_mpu_write_RNR_REGION_bf(const void *const hw, hri_mpu_rnr_reg_t data)
{
	uint32_t tmp;
	MPU_CRITICAL_SECTION_ENTER();
	tmp = ((Mpu *)hw)->RNR.reg;
	tmp &= ~MPU_RNR_REGION_Msk;
	tmp |= MPU_RNR_REGION(data);
	((Mpu *)hw)->RNR.reg = tmp;
	MPU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mpu_clear_RNR_REGION_bf(const void *const hw, hri_mpu_rnr_reg_t mask)
{
	MPU_CRITICAL_SECTION_ENTER();
	((Mpu *)hw)->RNR.reg &= ~MPU_RNR_REGION(mask);
	MPU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mpu_toggle_RNR_REGION_bf(const void *const hw, hri_mpu_rnr_reg_t mask)
{
	MPU_CRITICAL_SECTION_ENTER();
	((Mpu *)hw)->RNR.reg ^= MPU_RNR_REGION(mask);
	MPU_CRITICAL_SECTION_LEAVE();
}

static inline hri_mpu_rnr_reg_t hri_mpu_read_RNR_REGION_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Mpu *)hw)->RNR.reg;
	tmp = (tmp & MPU_RNR_REGION_Msk) >> 0;
	return tmp;
}

static inline void hri_mpu_set_RNR_reg(const void *const hw, hri_mpu_rnr_reg_t mask)
{
	MPU_CRITICAL_SECTION_ENTER();
	((Mpu *)hw)->RNR.reg |= mask;
	MPU_CRITICAL_SECTION_LEAVE();
}

static inline hri_mpu_rnr_reg_t hri_mpu_get_RNR_reg(const void *const hw, hri_mpu_rnr_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Mpu *)hw)->RNR.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_mpu_write_RNR_reg(const void *const hw, hri_mpu_rnr_reg_t data)
{
	MPU_CRITICAL_SECTION_ENTER();
	((Mpu *)hw)->RNR.reg = data;
	MPU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mpu_clear_RNR_reg(const void *const hw, hri_mpu_rnr_reg_t mask)
{
	MPU_CRITICAL_SECTION_ENTER();
	((Mpu *)hw)->RNR.reg &= ~mask;
	MPU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mpu_toggle_RNR_reg(const void *const hw, hri_mpu_rnr_reg_t mask)
{
	MPU_CRITICAL_SECTION_ENTER();
	((Mpu *)hw)->RNR.reg ^= mask;
	MPU_CRITICAL_SECTION_LEAVE();
}

static inline hri_mpu_rnr_reg_t hri_mpu_read_RNR_reg(const void *const hw)
{
	return ((Mpu *)hw)->RNR.reg;
}

static inline void hri_mpu_set_RBAR_VALID_bit(const void *const hw)
{
	MPU_CRITICAL_SECTION_ENTER();
	((Mpu *)hw)->RBAR.reg |= MPU_RBAR_VALID;
	MPU_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_mpu_get_RBAR_VALID_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Mpu *)hw)->RBAR.reg;
	tmp = (tmp & MPU_RBAR_VALID) >> 4;
	return (bool)tmp;
}

static inline void hri_mpu_write_RBAR_VALID_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	MPU_CRITICAL_SECTION_ENTER();
	tmp = ((Mpu *)hw)->RBAR.reg;
	tmp &= ~MPU_RBAR_VALID;
	tmp |= value << 4;
	((Mpu *)hw)->RBAR.reg = tmp;
	MPU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mpu_clear_RBAR_VALID_bit(const void *const hw)
{
	MPU_CRITICAL_SECTION_ENTER();
	((Mpu *)hw)->RBAR.reg &= ~MPU_RBAR_VALID;
	MPU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mpu_toggle_RBAR_VALID_bit(const void *const hw)
{
	MPU_CRITICAL_SECTION_ENTER();
	((Mpu *)hw)->RBAR.reg ^= MPU_RBAR_VALID;
	MPU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mpu_set_RBAR_REGION_bf(const void *const hw, hri_mpu_rbar_reg_t mask)
{
	MPU_CRITICAL_SECTION_ENTER();
	((Mpu *)hw)->RBAR.reg |= MPU_RBAR_REGION(mask);
	MPU_CRITICAL_SECTION_LEAVE();
}

static inline hri_mpu_rbar_reg_t hri_mpu_get_RBAR_REGION_bf(const void *const hw, hri_mpu_rbar_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Mpu *)hw)->RBAR.reg;
	tmp = (tmp & MPU_RBAR_REGION(mask)) >> 0;
	return tmp;
}

static inline void hri_mpu_write_RBAR_REGION_bf(const void *const hw, hri_mpu_rbar_reg_t data)
{
	uint32_t tmp;
	MPU_CRITICAL_SECTION_ENTER();
	tmp = ((Mpu *)hw)->RBAR.reg;
	tmp &= ~MPU_RBAR_REGION_Msk;
	tmp |= MPU_RBAR_REGION(data);
	((Mpu *)hw)->RBAR.reg = tmp;
	MPU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mpu_clear_RBAR_REGION_bf(const void *const hw, hri_mpu_rbar_reg_t mask)
{
	MPU_CRITICAL_SECTION_ENTER();
	((Mpu *)hw)->RBAR.reg &= ~MPU_RBAR_REGION(mask);
	MPU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mpu_toggle_RBAR_REGION_bf(const void *const hw, hri_mpu_rbar_reg_t mask)
{
	MPU_CRITICAL_SECTION_ENTER();
	((Mpu *)hw)->RBAR.reg ^= MPU_RBAR_REGION(mask);
	MPU_CRITICAL_SECTION_LEAVE();
}

static inline hri_mpu_rbar_reg_t hri_mpu_read_RBAR_REGION_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Mpu *)hw)->RBAR.reg;
	tmp = (tmp & MPU_RBAR_REGION_Msk) >> 0;
	return tmp;
}

static inline void hri_mpu_set_RBAR_ADDR_bf(const void *const hw, hri_mpu_rbar_reg_t mask)
{
	MPU_CRITICAL_SECTION_ENTER();
	((Mpu *)hw)->RBAR.reg |= MPU_RBAR_ADDR(mask);
	MPU_CRITICAL_SECTION_LEAVE();
}

static inline hri_mpu_rbar_reg_t hri_mpu_get_RBAR_ADDR_bf(const void *const hw, hri_mpu_rbar_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Mpu *)hw)->RBAR.reg;
	tmp = (tmp & MPU_RBAR_ADDR(mask)) >> 5;
	return tmp;
}

static inline void hri_mpu_write_RBAR_ADDR_bf(const void *const hw, hri_mpu_rbar_reg_t data)
{
	uint32_t tmp;
	MPU_CRITICAL_SECTION_ENTER();
	tmp = ((Mpu *)hw)->RBAR.reg;
	tmp &= ~MPU_RBAR_ADDR_Msk;
	tmp |= MPU_RBAR_ADDR(data);
	((Mpu *)hw)->RBAR.reg = tmp;
	MPU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mpu_clear_RBAR_ADDR_bf(const void *const hw, hri_mpu_rbar_reg_t mask)
{
	MPU_CRITICAL_SECTION_ENTER();
	((Mpu *)hw)->RBAR.reg &= ~MPU_RBAR_ADDR(mask);
	MPU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mpu_toggle_RBAR_ADDR_bf(const void *const hw, hri_mpu_rbar_reg_t mask)
{
	MPU_CRITICAL_SECTION_ENTER();
	((Mpu *)hw)->RBAR.reg ^= MPU_RBAR_ADDR(mask);
	MPU_CRITICAL_SECTION_LEAVE();
}

static inline hri_mpu_rbar_reg_t hri_mpu_read_RBAR_ADDR_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Mpu *)hw)->RBAR.reg;
	tmp = (tmp & MPU_RBAR_ADDR_Msk) >> 5;
	return tmp;
}

static inline void hri_mpu_set_RBAR_reg(const void *const hw, hri_mpu_rbar_reg_t mask)
{
	MPU_CRITICAL_SECTION_ENTER();
	((Mpu *)hw)->RBAR.reg |= mask;
	MPU_CRITICAL_SECTION_LEAVE();
}

static inline hri_mpu_rbar_reg_t hri_mpu_get_RBAR_reg(const void *const hw, hri_mpu_rbar_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Mpu *)hw)->RBAR.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_mpu_write_RBAR_reg(const void *const hw, hri_mpu_rbar_reg_t data)
{
	MPU_CRITICAL_SECTION_ENTER();
	((Mpu *)hw)->RBAR.reg = data;
	MPU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mpu_clear_RBAR_reg(const void *const hw, hri_mpu_rbar_reg_t mask)
{
	MPU_CRITICAL_SECTION_ENTER();
	((Mpu *)hw)->RBAR.reg &= ~mask;
	MPU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mpu_toggle_RBAR_reg(const void *const hw, hri_mpu_rbar_reg_t mask)
{
	MPU_CRITICAL_SECTION_ENTER();
	((Mpu *)hw)->RBAR.reg ^= mask;
	MPU_CRITICAL_SECTION_LEAVE();
}

static inline hri_mpu_rbar_reg_t hri_mpu_read_RBAR_reg(const void *const hw)
{
	return ((Mpu *)hw)->RBAR.reg;
}

static inline void hri_mpu_set_RASR_ENABLE_bit(const void *const hw)
{
	MPU_CRITICAL_SECTION_ENTER();
	((Mpu *)hw)->RASR.reg |= MPU_RASR_ENABLE;
	MPU_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_mpu_get_RASR_ENABLE_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Mpu *)hw)->RASR.reg;
	tmp = (tmp & MPU_RASR_ENABLE) >> 0;
	return (bool)tmp;
}

static inline void hri_mpu_write_RASR_ENABLE_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	MPU_CRITICAL_SECTION_ENTER();
	tmp = ((Mpu *)hw)->RASR.reg;
	tmp &= ~MPU_RASR_ENABLE;
	tmp |= value << 0;
	((Mpu *)hw)->RASR.reg = tmp;
	MPU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mpu_clear_RASR_ENABLE_bit(const void *const hw)
{
	MPU_CRITICAL_SECTION_ENTER();
	((Mpu *)hw)->RASR.reg &= ~MPU_RASR_ENABLE;
	MPU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mpu_toggle_RASR_ENABLE_bit(const void *const hw)
{
	MPU_CRITICAL_SECTION_ENTER();
	((Mpu *)hw)->RASR.reg ^= MPU_RASR_ENABLE;
	MPU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mpu_set_RASR_B_bit(const void *const hw)
{
	MPU_CRITICAL_SECTION_ENTER();
	((Mpu *)hw)->RASR.reg |= MPU_RASR_B;
	MPU_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_mpu_get_RASR_B_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Mpu *)hw)->RASR.reg;
	tmp = (tmp & MPU_RASR_B) >> 16;
	return (bool)tmp;
}

static inline void hri_mpu_write_RASR_B_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	MPU_CRITICAL_SECTION_ENTER();
	tmp = ((Mpu *)hw)->RASR.reg;
	tmp &= ~MPU_RASR_B;
	tmp |= value << 16;
	((Mpu *)hw)->RASR.reg = tmp;
	MPU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mpu_clear_RASR_B_bit(const void *const hw)
{
	MPU_CRITICAL_SECTION_ENTER();
	((Mpu *)hw)->RASR.reg &= ~MPU_RASR_B;
	MPU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mpu_toggle_RASR_B_bit(const void *const hw)
{
	MPU_CRITICAL_SECTION_ENTER();
	((Mpu *)hw)->RASR.reg ^= MPU_RASR_B;
	MPU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mpu_set_RASR_C_bit(const void *const hw)
{
	MPU_CRITICAL_SECTION_ENTER();
	((Mpu *)hw)->RASR.reg |= MPU_RASR_C;
	MPU_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_mpu_get_RASR_C_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Mpu *)hw)->RASR.reg;
	tmp = (tmp & MPU_RASR_C) >> 17;
	return (bool)tmp;
}

static inline void hri_mpu_write_RASR_C_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	MPU_CRITICAL_SECTION_ENTER();
	tmp = ((Mpu *)hw)->RASR.reg;
	tmp &= ~MPU_RASR_C;
	tmp |= value << 17;
	((Mpu *)hw)->RASR.reg = tmp;
	MPU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mpu_clear_RASR_C_bit(const void *const hw)
{
	MPU_CRITICAL_SECTION_ENTER();
	((Mpu *)hw)->RASR.reg &= ~MPU_RASR_C;
	MPU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mpu_toggle_RASR_C_bit(const void *const hw)
{
	MPU_CRITICAL_SECTION_ENTER();
	((Mpu *)hw)->RASR.reg ^= MPU_RASR_C;
	MPU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mpu_set_RASR_S_bit(const void *const hw)
{
	MPU_CRITICAL_SECTION_ENTER();
	((Mpu *)hw)->RASR.reg |= MPU_RASR_S;
	MPU_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_mpu_get_RASR_S_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Mpu *)hw)->RASR.reg;
	tmp = (tmp & MPU_RASR_S) >> 18;
	return (bool)tmp;
}

static inline void hri_mpu_write_RASR_S_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	MPU_CRITICAL_SECTION_ENTER();
	tmp = ((Mpu *)hw)->RASR.reg;
	tmp &= ~MPU_RASR_S;
	tmp |= value << 18;
	((Mpu *)hw)->RASR.reg = tmp;
	MPU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mpu_clear_RASR_S_bit(const void *const hw)
{
	MPU_CRITICAL_SECTION_ENTER();
	((Mpu *)hw)->RASR.reg &= ~MPU_RASR_S;
	MPU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mpu_toggle_RASR_S_bit(const void *const hw)
{
	MPU_CRITICAL_SECTION_ENTER();
	((Mpu *)hw)->RASR.reg ^= MPU_RASR_S;
	MPU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mpu_set_RASR_XN_bit(const void *const hw)
{
	MPU_CRITICAL_SECTION_ENTER();
	((Mpu *)hw)->RASR.reg |= MPU_RASR_XN;
	MPU_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_mpu_get_RASR_XN_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Mpu *)hw)->RASR.reg;
	tmp = (tmp & MPU_RASR_XN) >> 28;
	return (bool)tmp;
}

static inline void hri_mpu_write_RASR_XN_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	MPU_CRITICAL_SECTION_ENTER();
	tmp = ((Mpu *)hw)->RASR.reg;
	tmp &= ~MPU_RASR_XN;
	tmp |= value << 28;
	((Mpu *)hw)->RASR.reg = tmp;
	MPU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mpu_clear_RASR_XN_bit(const void *const hw)
{
	MPU_CRITICAL_SECTION_ENTER();
	((Mpu *)hw)->RASR.reg &= ~MPU_RASR_XN;
	MPU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mpu_toggle_RASR_XN_bit(const void *const hw)
{
	MPU_CRITICAL_SECTION_ENTER();
	((Mpu *)hw)->RASR.reg ^= MPU_RASR_XN;
	MPU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mpu_set_RASR_SIZE_bf(const void *const hw, hri_mpu_rasr_reg_t mask)
{
	MPU_CRITICAL_SECTION_ENTER();
	((Mpu *)hw)->RASR.reg |= MPU_RASR_SIZE(mask);
	MPU_CRITICAL_SECTION_LEAVE();
}

static inline hri_mpu_rasr_reg_t hri_mpu_get_RASR_SIZE_bf(const void *const hw, hri_mpu_rasr_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Mpu *)hw)->RASR.reg;
	tmp = (tmp & MPU_RASR_SIZE(mask)) >> 1;
	return tmp;
}

static inline void hri_mpu_write_RASR_SIZE_bf(const void *const hw, hri_mpu_rasr_reg_t data)
{
	uint32_t tmp;
	MPU_CRITICAL_SECTION_ENTER();
	tmp = ((Mpu *)hw)->RASR.reg;
	tmp &= ~MPU_RASR_SIZE_Msk;
	tmp |= MPU_RASR_SIZE(data);
	((Mpu *)hw)->RASR.reg = tmp;
	MPU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mpu_clear_RASR_SIZE_bf(const void *const hw, hri_mpu_rasr_reg_t mask)
{
	MPU_CRITICAL_SECTION_ENTER();
	((Mpu *)hw)->RASR.reg &= ~MPU_RASR_SIZE(mask);
	MPU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mpu_toggle_RASR_SIZE_bf(const void *const hw, hri_mpu_rasr_reg_t mask)
{
	MPU_CRITICAL_SECTION_ENTER();
	((Mpu *)hw)->RASR.reg ^= MPU_RASR_SIZE(mask);
	MPU_CRITICAL_SECTION_LEAVE();
}

static inline hri_mpu_rasr_reg_t hri_mpu_read_RASR_SIZE_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Mpu *)hw)->RASR.reg;
	tmp = (tmp & MPU_RASR_SIZE_Msk) >> 1;
	return tmp;
}

static inline void hri_mpu_set_RASR_SRD_bf(const void *const hw, hri_mpu_rasr_reg_t mask)
{
	MPU_CRITICAL_SECTION_ENTER();
	((Mpu *)hw)->RASR.reg |= MPU_RASR_SRD(mask);
	MPU_CRITICAL_SECTION_LEAVE();
}

static inline hri_mpu_rasr_reg_t hri_mpu_get_RASR_SRD_bf(const void *const hw, hri_mpu_rasr_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Mpu *)hw)->RASR.reg;
	tmp = (tmp & MPU_RASR_SRD(mask)) >> 8;
	return tmp;
}

static inline void hri_mpu_write_RASR_SRD_bf(const void *const hw, hri_mpu_rasr_reg_t data)
{
	uint32_t tmp;
	MPU_CRITICAL_SECTION_ENTER();
	tmp = ((Mpu *)hw)->RASR.reg;
	tmp &= ~MPU_RASR_SRD_Msk;
	tmp |= MPU_RASR_SRD(data);
	((Mpu *)hw)->RASR.reg = tmp;
	MPU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mpu_clear_RASR_SRD_bf(const void *const hw, hri_mpu_rasr_reg_t mask)
{
	MPU_CRITICAL_SECTION_ENTER();
	((Mpu *)hw)->RASR.reg &= ~MPU_RASR_SRD(mask);
	MPU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mpu_toggle_RASR_SRD_bf(const void *const hw, hri_mpu_rasr_reg_t mask)
{
	MPU_CRITICAL_SECTION_ENTER();
	((Mpu *)hw)->RASR.reg ^= MPU_RASR_SRD(mask);
	MPU_CRITICAL_SECTION_LEAVE();
}

static inline hri_mpu_rasr_reg_t hri_mpu_read_RASR_SRD_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Mpu *)hw)->RASR.reg;
	tmp = (tmp & MPU_RASR_SRD_Msk) >> 8;
	return tmp;
}

static inline void hri_mpu_set_RASR_TEX_bf(const void *const hw, hri_mpu_rasr_reg_t mask)
{
	MPU_CRITICAL_SECTION_ENTER();
	((Mpu *)hw)->RASR.reg |= MPU_RASR_TEX(mask);
	MPU_CRITICAL_SECTION_LEAVE();
}

static inline hri_mpu_rasr_reg_t hri_mpu_get_RASR_TEX_bf(const void *const hw, hri_mpu_rasr_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Mpu *)hw)->RASR.reg;
	tmp = (tmp & MPU_RASR_TEX(mask)) >> 19;
	return tmp;
}

static inline void hri_mpu_write_RASR_TEX_bf(const void *const hw, hri_mpu_rasr_reg_t data)
{
	uint32_t tmp;
	MPU_CRITICAL_SECTION_ENTER();
	tmp = ((Mpu *)hw)->RASR.reg;
	tmp &= ~MPU_RASR_TEX_Msk;
	tmp |= MPU_RASR_TEX(data);
	((Mpu *)hw)->RASR.reg = tmp;
	MPU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mpu_clear_RASR_TEX_bf(const void *const hw, hri_mpu_rasr_reg_t mask)
{
	MPU_CRITICAL_SECTION_ENTER();
	((Mpu *)hw)->RASR.reg &= ~MPU_RASR_TEX(mask);
	MPU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mpu_toggle_RASR_TEX_bf(const void *const hw, hri_mpu_rasr_reg_t mask)
{
	MPU_CRITICAL_SECTION_ENTER();
	((Mpu *)hw)->RASR.reg ^= MPU_RASR_TEX(mask);
	MPU_CRITICAL_SECTION_LEAVE();
}

static inline hri_mpu_rasr_reg_t hri_mpu_read_RASR_TEX_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Mpu *)hw)->RASR.reg;
	tmp = (tmp & MPU_RASR_TEX_Msk) >> 19;
	return tmp;
}

static inline void hri_mpu_set_RASR_AP_bf(const void *const hw, hri_mpu_rasr_reg_t mask)
{
	MPU_CRITICAL_SECTION_ENTER();
	((Mpu *)hw)->RASR.reg |= MPU_RASR_AP(mask);
	MPU_CRITICAL_SECTION_LEAVE();
}

static inline hri_mpu_rasr_reg_t hri_mpu_get_RASR_AP_bf(const void *const hw, hri_mpu_rasr_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Mpu *)hw)->RASR.reg;
	tmp = (tmp & MPU_RASR_AP(mask)) >> 24;
	return tmp;
}

static inline void hri_mpu_write_RASR_AP_bf(const void *const hw, hri_mpu_rasr_reg_t data)
{
	uint32_t tmp;
	MPU_CRITICAL_SECTION_ENTER();
	tmp = ((Mpu *)hw)->RASR.reg;
	tmp &= ~MPU_RASR_AP_Msk;
	tmp |= MPU_RASR_AP(data);
	((Mpu *)hw)->RASR.reg = tmp;
	MPU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mpu_clear_RASR_AP_bf(const void *const hw, hri_mpu_rasr_reg_t mask)
{
	MPU_CRITICAL_SECTION_ENTER();
	((Mpu *)hw)->RASR.reg &= ~MPU_RASR_AP(mask);
	MPU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mpu_toggle_RASR_AP_bf(const void *const hw, hri_mpu_rasr_reg_t mask)
{
	MPU_CRITICAL_SECTION_ENTER();
	((Mpu *)hw)->RASR.reg ^= MPU_RASR_AP(mask);
	MPU_CRITICAL_SECTION_LEAVE();
}

static inline hri_mpu_rasr_reg_t hri_mpu_read_RASR_AP_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Mpu *)hw)->RASR.reg;
	tmp = (tmp & MPU_RASR_AP_Msk) >> 24;
	return tmp;
}

static inline void hri_mpu_set_RASR_reg(const void *const hw, hri_mpu_rasr_reg_t mask)
{
	MPU_CRITICAL_SECTION_ENTER();
	((Mpu *)hw)->RASR.reg |= mask;
	MPU_CRITICAL_SECTION_LEAVE();
}

static inline hri_mpu_rasr_reg_t hri_mpu_get_RASR_reg(const void *const hw, hri_mpu_rasr_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Mpu *)hw)->RASR.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_mpu_write_RASR_reg(const void *const hw, hri_mpu_rasr_reg_t data)
{
	MPU_CRITICAL_SECTION_ENTER();
	((Mpu *)hw)->RASR.reg = data;
	MPU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mpu_clear_RASR_reg(const void *const hw, hri_mpu_rasr_reg_t mask)
{
	MPU_CRITICAL_SECTION_ENTER();
	((Mpu *)hw)->RASR.reg &= ~mask;
	MPU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_mpu_toggle_RASR_reg(const void *const hw, hri_mpu_rasr_reg_t mask)
{
	MPU_CRITICAL_SECTION_ENTER();
	((Mpu *)hw)->RASR.reg ^= mask;
	MPU_CRITICAL_SECTION_LEAVE();
}

static inline hri_mpu_rasr_reg_t hri_mpu_read_RASR_reg(const void *const hw)
{
	return ((Mpu *)hw)->RASR.reg;
}

static inline bool hri_mpu_get_TYPE_SEPARATE_bit(const void *const hw)
{
	return (((Mpu *)hw)->TYPE.reg & MPU_TYPE_SEPARATE) >> 0;
}

static inline hri_mpu_type_reg_t hri_mpu_get_TYPE_DREGION_bf(const void *const hw, hri_mpu_type_reg_t mask)
{
	return (((Mpu *)hw)->TYPE.reg & MPU_TYPE_DREGION(mask)) >> 8;
}

static inline hri_mpu_type_reg_t hri_mpu_read_TYPE_DREGION_bf(const void *const hw)
{
	return (((Mpu *)hw)->TYPE.reg & MPU_TYPE_DREGION_Msk) >> 8;
}

static inline hri_mpu_type_reg_t hri_mpu_get_TYPE_IREGION_bf(const void *const hw, hri_mpu_type_reg_t mask)
{
	return (((Mpu *)hw)->TYPE.reg & MPU_TYPE_IREGION(mask)) >> 16;
}

static inline hri_mpu_type_reg_t hri_mpu_read_TYPE_IREGION_bf(const void *const hw)
{
	return (((Mpu *)hw)->TYPE.reg & MPU_TYPE_IREGION_Msk) >> 16;
}

static inline hri_mpu_type_reg_t hri_mpu_get_TYPE_reg(const void *const hw, hri_mpu_type_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Mpu *)hw)->TYPE.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_mpu_type_reg_t hri_mpu_read_TYPE_reg(const void *const hw)
{
	return ((Mpu *)hw)->TYPE.reg;
}

#ifdef __cplusplus
}
#endif

#endif /* _HRI_MPU_E54_H_INCLUDED */
#endif /* _SAME54_MPU_COMPONENT_ */
