
/**
 * \file
 *
 * \brief EIC related functionality implementation.
 *
 * Copyright (C) 2015-2017 Atmel Corporation. All rights reserved.
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
#include <hpl_eic_config.h>
#include <hpl_ext_irq.h>
#include <string.h>
#include <utils.h>
#include <utils_assert.h>

#ifdef __MINGW32__
#define ffs __builtin_ffs
#endif
#if defined(__CC_ARM) || defined(__ICCARM__)
/* Find the first bit set */
static int ffs(int v)
{
	int i, bit = 1;
	for (i = 0; i < sizeof(int) * 8; i++) {
		if (v & bit) {
			return i + 1;
		}
		bit <<= 1;
	}
	return 0;
}
#endif

/**
 * \brief Invalid external interrupt and pin numbers
 */
#define INVALID_EXTINT_NUMBER 0xFF
#define INVALID_PIN_NUMBER 0xFFFFFFFF

#ifndef CONFIG_EIC_EXTINT_MAP
/** Dummy mapping to pass compiling. */
#define CONFIG_EIC_EXTINT_MAP                                                                                          \
	{                                                                                                                  \
		INVALID_EXTINT_NUMBER, INVALID_PIN_NUMBER                                                                      \
	}
#endif

#define EXT_IRQ_AMOUNT 1

/**
 * \brief EXTINTx and pin number map
 */
struct _eic_map {
	uint8_t  extint;
	uint32_t pin;
};

/**
 * \brief PIN and EXTINT map for enabled external interrupts
 */
static const struct _eic_map _map[] = {CONFIG_EIC_EXTINT_MAP};

/**
 * \brief The callback to upper layer's interrupt processing routine
 */
static void (*callback)(const uint32_t pin);

static void _ext_irq_handler(void);

/**
 * \brief Initialize external interrupt module
 */
int32_t _ext_irq_init(void (*cb)(const uint32_t pin))
{
	hri_eic_wait_for_sync(EIC, EIC_SYNCBUSY_SWRST);
	if (hri_eic_get_CTRLA_ENABLE_bit(EIC)) {
		return ERR_DENIED;
	}
	hri_eic_set_CTRLA_SWRST_bit(EIC);
	hri_eic_wait_for_sync(EIC, EIC_SYNCBUSY_SWRST);

	hri_eic_write_CTRLA_CKSEL_bit(EIC, CONF_EIC_CKSEL);

	hri_eic_write_NMICTRL_reg(
	    EIC, (CONF_EIC_NMIFILTEN << EIC_NMICTRL_NMIFILTEN_Pos) | EIC_NMICTRL_NMISENSE(CONF_EIC_NMISENSE) | 0);

	hri_eic_write_EVCTRL_reg(EIC,
	                         (CONF_EIC_EXTINTEO0 << 0) | (CONF_EIC_EXTINTEO1 << 1) | (CONF_EIC_EXTINTEO2 << 2)
	                             | (CONF_EIC_EXTINTEO3 << 3)
	                             | (CONF_EIC_EXTINTEO4 << 4)
	                             | (CONF_EIC_EXTINTEO5 << 5)
	                             | (CONF_EIC_EXTINTEO6 << 6)
	                             | (CONF_EIC_EXTINTEO7 << 7)
	                             | (CONF_EIC_EXTINTEO8 << 8)
	                             | (CONF_EIC_EXTINTEO9 << 9)
	                             | (CONF_EIC_EXTINTEO10 << 10)
	                             | (CONF_EIC_EXTINTEO11 << 11)
	                             | (CONF_EIC_EXTINTEO12 << 12)
	                             | (CONF_EIC_EXTINTEO13 << 13)
	                             | (CONF_EIC_EXTINTEO14 << 14)
	                             | (CONF_EIC_EXTINTEO15 << 15)
	                             | 0);

	hri_eic_write_CONFIG_reg(EIC,
	                         0,
	                         (CONF_EIC_FILTEN0 << EIC_CONFIG_FILTEN0_Pos) | EIC_CONFIG_SENSE0(CONF_EIC_SENSE0)
	                             | (CONF_EIC_FILTEN1 << EIC_CONFIG_FILTEN1_Pos)
	                             | EIC_CONFIG_SENSE1(CONF_EIC_SENSE1)
	                             | (CONF_EIC_FILTEN2 << EIC_CONFIG_FILTEN2_Pos)
	                             | EIC_CONFIG_SENSE2(CONF_EIC_SENSE2)
	                             | (CONF_EIC_FILTEN3 << EIC_CONFIG_FILTEN3_Pos)
	                             | EIC_CONFIG_SENSE3(CONF_EIC_SENSE3)
	                             | (CONF_EIC_FILTEN4 << EIC_CONFIG_FILTEN4_Pos)
	                             | EIC_CONFIG_SENSE4(CONF_EIC_SENSE4)
	                             | (CONF_EIC_FILTEN5 << EIC_CONFIG_FILTEN5_Pos)
	                             | EIC_CONFIG_SENSE5(CONF_EIC_SENSE5)
	                             | (CONF_EIC_FILTEN6 << EIC_CONFIG_FILTEN6_Pos)
	                             | EIC_CONFIG_SENSE6(CONF_EIC_SENSE6)
	                             | (CONF_EIC_FILTEN7 << EIC_CONFIG_FILTEN7_Pos)
	                             | EIC_CONFIG_SENSE7(CONF_EIC_SENSE7)
	                             | 0);

	hri_eic_write_CONFIG_reg(EIC,
	                         1,
	                         (CONF_EIC_FILTEN8 << EIC_CONFIG_FILTEN0_Pos) | EIC_CONFIG_SENSE0(CONF_EIC_SENSE8)
	                             | (CONF_EIC_FILTEN9 << EIC_CONFIG_FILTEN1_Pos)
	                             | EIC_CONFIG_SENSE1(CONF_EIC_SENSE9)
	                             | (CONF_EIC_FILTEN10 << EIC_CONFIG_FILTEN2_Pos)
	                             | EIC_CONFIG_SENSE2(CONF_EIC_SENSE10)
	                             | (CONF_EIC_FILTEN11 << EIC_CONFIG_FILTEN3_Pos)
	                             | EIC_CONFIG_SENSE3(CONF_EIC_SENSE11)
	                             | (CONF_EIC_FILTEN12 << EIC_CONFIG_FILTEN4_Pos)
	                             | EIC_CONFIG_SENSE4(CONF_EIC_SENSE12)
	                             | (CONF_EIC_FILTEN13 << EIC_CONFIG_FILTEN5_Pos)
	                             | EIC_CONFIG_SENSE5(CONF_EIC_SENSE13)
	                             | (CONF_EIC_FILTEN14 << EIC_CONFIG_FILTEN6_Pos)
	                             | EIC_CONFIG_SENSE6(CONF_EIC_SENSE14)
	                             | (CONF_EIC_FILTEN15 << EIC_CONFIG_FILTEN7_Pos)
	                             | EIC_CONFIG_SENSE7(CONF_EIC_SENSE15)
	                             | 0);

	hri_eic_set_CTRLA_ENABLE_bit(EIC);
	NVIC_DisableIRQ(EIC_IRQn);
	NVIC_ClearPendingIRQ(EIC_IRQn);
	NVIC_EnableIRQ(EIC_IRQn);

	callback = cb;

	return ERR_NONE;
}

/**
 * \brief De-initialize external interrupt module
 */
int32_t _ext_irq_deinit(void)
{
	NVIC_DisableIRQ(EIC_IRQn);
	callback = NULL;

	hri_eic_clear_CTRLA_ENABLE_bit(EIC);
	hri_eic_set_CTRLA_SWRST_bit(EIC);

	return ERR_NONE;
}

/**
 * \brief Enable / disable external irq
 */
int32_t _ext_irq_enable(const uint32_t pin, const bool enable)
{
	uint8_t extint = INVALID_EXTINT_NUMBER;
	uint8_t i      = 0;

	for (; i < ARRAY_SIZE(_map); i++) {
		if (_map[i].pin == pin) {
			extint = _map[i].extint;
			break;
		}
	}
	if (INVALID_EXTINT_NUMBER == extint) {
		return -1;
	}

	if (enable) {
		hri_eic_set_INTEN_reg(EIC, 1ul << extint);
	} else {
		hri_eic_clear_INTEN_reg(EIC, 1ul << extint);
		hri_eic_clear_INTFLAG_reg(EIC, 1ul << extint);
	}

	return ERR_NONE;
}

/**
 * \brief Inter EIC interrupt handler
 */
static void _ext_irq_handler(void)
{
	volatile uint32_t flags = hri_eic_read_INTFLAG_reg(EIC);
	int8_t            pos;
	uint32_t          pin = INVALID_PIN_NUMBER;

	hri_eic_clear_INTFLAG_reg(EIC, flags);

	ASSERT(callback);

	while (flags) {
		pos = ffs(flags) - 1;
		while (-1 != pos) {
			uint8_t lower = 0, middle, upper = EXT_IRQ_AMOUNT;

			while (upper >= lower) {
				middle = (upper + lower) >> 1;
				if (_map[middle].extint == pos) {
					pin = _map[middle].pin;
					break;
				}
				if (_map[middle].extint < pos) {
					lower = middle + 1;
				} else {
					upper = middle - 1;
				}
			}

			if (INVALID_PIN_NUMBER != pin) {
				callback(pin);
			}
			flags &= ~(1ul << pos);
			pos = ffs(flags) - 1;
		}
		flags = hri_eic_read_INTFLAG_reg(EIC);
		hri_eic_clear_INTFLAG_reg(EIC, flags);
	}
}

/**
* \brief EIC interrupt handler
*/
void EIC_Handler(void)
{
	_ext_irq_handler();
}
