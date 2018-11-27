
/**
 * \file
 *
 * \brief EIC related functionality implementation.
 *
 * Copyright (c) 2015-2018 Microchip Technology Inc. and its subsidiaries.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Subject to your compliance with these terms, you may use Microchip
 * software and any derivatives exclusively with Microchip products.
 * It is your responsibility to comply with third party license terms applicable
 * to your use of third party software (including open source software) that
 * may accompany Microchip software.
 *
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE,
 * INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY,
 * AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT WILL MICROCHIP BE
 * LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, INCIDENTAL OR CONSEQUENTIAL
 * LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND WHATSOEVER RELATED TO THE
 * SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS BEEN ADVISED OF THE
 * POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE FULLEST EXTENT
 * ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN ANY WAY
 * RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
 * THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
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

#define EXT_IRQ_AMOUNT 2

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
	if (!hri_eic_is_syncing(EIC, EIC_SYNCBUSY_SWRST)) {
		if (hri_eic_get_CTRLA_reg(EIC, EIC_CTRLA_ENABLE)) {
			hri_eic_clear_CTRLA_ENABLE_bit(EIC);
			hri_eic_wait_for_sync(EIC, EIC_SYNCBUSY_ENABLE);
		}
		hri_eic_write_CTRLA_reg(EIC, EIC_CTRLA_SWRST);
	}
	hri_eic_wait_for_sync(EIC, EIC_SYNCBUSY_SWRST);

	hri_eic_write_CTRLA_CKSEL_bit(EIC, CONF_EIC_CKSEL);

	hri_eic_write_NMICTRL_reg(EIC,
	                          (CONF_EIC_NMIFILTEN << EIC_NMICTRL_NMIFILTEN_Pos)
	                              | EIC_NMICTRL_NMISENSE(CONF_EIC_NMISENSE) | EIC_ASYNCH_ASYNCH(CONF_EIC_NMIASYNCH)
	                              | 0);

	hri_eic_write_EVCTRL_reg(EIC,
	                         (CONF_EIC_EXTINTEO0 << 0) | (CONF_EIC_EXTINTEO1 << 1) | (CONF_EIC_EXTINTEO2 << 2)
	                             | (CONF_EIC_EXTINTEO3 << 3) | (CONF_EIC_EXTINTEO4 << 4) | (CONF_EIC_EXTINTEO5 << 5)
	                             | (CONF_EIC_EXTINTEO6 << 6) | (CONF_EIC_EXTINTEO7 << 7) | (CONF_EIC_EXTINTEO8 << 8)
	                             | (CONF_EIC_EXTINTEO9 << 9) | (CONF_EIC_EXTINTEO10 << 10) | (CONF_EIC_EXTINTEO11 << 11)
	                             | (CONF_EIC_EXTINTEO12 << 12) | (CONF_EIC_EXTINTEO13 << 13)
	                             | (CONF_EIC_EXTINTEO14 << 14) | (CONF_EIC_EXTINTEO15 << 15) | 0);
	hri_eic_write_ASYNCH_reg(EIC,
	                         (CONF_EIC_ASYNCH0 << 0) | (CONF_EIC_ASYNCH1 << 1) | (CONF_EIC_ASYNCH2 << 2)
	                             | (CONF_EIC_ASYNCH3 << 3) | (CONF_EIC_ASYNCH4 << 4) | (CONF_EIC_ASYNCH5 << 5)
	                             | (CONF_EIC_ASYNCH6 << 6) | (CONF_EIC_ASYNCH7 << 7) | (CONF_EIC_ASYNCH8 << 8)
	                             | (CONF_EIC_ASYNCH9 << 9) | (CONF_EIC_ASYNCH10 << 10) | (CONF_EIC_ASYNCH11 << 11)
	                             | (CONF_EIC_ASYNCH12 << 12) | (CONF_EIC_ASYNCH13 << 13) | (CONF_EIC_ASYNCH14 << 14)
	                             | (CONF_EIC_ASYNCH15 << 15) | 0);
	hri_eic_write_DEBOUNCEN_reg(
	    EIC,
	    (CONF_EIC_DEBOUNCE_ENABLE0 << 0) | (CONF_EIC_DEBOUNCE_ENABLE1 << 1) | (CONF_EIC_DEBOUNCE_ENABLE2 << 2)
	        | (CONF_EIC_DEBOUNCE_ENABLE3 << 3) | (CONF_EIC_DEBOUNCE_ENABLE4 << 4) | (CONF_EIC_DEBOUNCE_ENABLE5 << 5)
	        | (CONF_EIC_DEBOUNCE_ENABLE6 << 6) | (CONF_EIC_DEBOUNCE_ENABLE7 << 7) | (CONF_EIC_DEBOUNCE_ENABLE8 << 8)
	        | (CONF_EIC_DEBOUNCE_ENABLE9 << 9) | (CONF_EIC_DEBOUNCE_ENABLE10 << 10) | (CONF_EIC_DEBOUNCE_ENABLE11 << 11)
	        | (CONF_EIC_DEBOUNCE_ENABLE12 << 12) | (CONF_EIC_DEBOUNCE_ENABLE13 << 13)
	        | (CONF_EIC_DEBOUNCE_ENABLE14 << 14) | (CONF_EIC_DEBOUNCE_ENABLE15 << 15) | 0);

	hri_eic_write_DPRESCALER_reg(
	    EIC,
	    (EIC_DPRESCALER_PRESCALER0(CONF_EIC_DPRESCALER0)) | (CONF_EIC_STATES0 << EIC_DPRESCALER_STATES0_Pos)
	        | (EIC_DPRESCALER_PRESCALER1(CONF_EIC_DPRESCALER1)) | (CONF_EIC_STATES1 << EIC_DPRESCALER_STATES1_Pos)
	        | CONF_EIC_TICKON << EIC_DPRESCALER_TICKON_Pos | 0);

	hri_eic_write_CONFIG_reg(EIC,
	                         0,
	                         (CONF_EIC_FILTEN0 << EIC_CONFIG_FILTEN0_Pos) | EIC_CONFIG_SENSE0(CONF_EIC_SENSE0)
	                             | (CONF_EIC_FILTEN1 << EIC_CONFIG_FILTEN1_Pos) | EIC_CONFIG_SENSE1(CONF_EIC_SENSE1)
	                             | (CONF_EIC_FILTEN2 << EIC_CONFIG_FILTEN2_Pos) | EIC_CONFIG_SENSE2(CONF_EIC_SENSE2)
	                             | (CONF_EIC_FILTEN3 << EIC_CONFIG_FILTEN3_Pos) | EIC_CONFIG_SENSE3(CONF_EIC_SENSE3)
	                             | (CONF_EIC_FILTEN4 << EIC_CONFIG_FILTEN4_Pos) | EIC_CONFIG_SENSE4(CONF_EIC_SENSE4)
	                             | (CONF_EIC_FILTEN5 << EIC_CONFIG_FILTEN5_Pos) | EIC_CONFIG_SENSE5(CONF_EIC_SENSE5)
	                             | (CONF_EIC_FILTEN6 << EIC_CONFIG_FILTEN6_Pos) | EIC_CONFIG_SENSE6(CONF_EIC_SENSE6)
	                             | (CONF_EIC_FILTEN7 << EIC_CONFIG_FILTEN7_Pos) | EIC_CONFIG_SENSE7(CONF_EIC_SENSE7)
	                             | 0);

	hri_eic_write_CONFIG_reg(EIC,
	                         1,
	                         (CONF_EIC_FILTEN8 << EIC_CONFIG_FILTEN0_Pos) | EIC_CONFIG_SENSE0(CONF_EIC_SENSE8)
	                             | (CONF_EIC_FILTEN9 << EIC_CONFIG_FILTEN1_Pos) | EIC_CONFIG_SENSE1(CONF_EIC_SENSE9)
	                             | (CONF_EIC_FILTEN10 << EIC_CONFIG_FILTEN2_Pos) | EIC_CONFIG_SENSE2(CONF_EIC_SENSE10)
	                             | (CONF_EIC_FILTEN11 << EIC_CONFIG_FILTEN3_Pos) | EIC_CONFIG_SENSE3(CONF_EIC_SENSE11)
	                             | (CONF_EIC_FILTEN12 << EIC_CONFIG_FILTEN4_Pos) | EIC_CONFIG_SENSE4(CONF_EIC_SENSE12)
	                             | (CONF_EIC_FILTEN13 << EIC_CONFIG_FILTEN5_Pos) | EIC_CONFIG_SENSE5(CONF_EIC_SENSE13)
	                             | (CONF_EIC_FILTEN14 << EIC_CONFIG_FILTEN6_Pos) | EIC_CONFIG_SENSE6(CONF_EIC_SENSE14)
	                             | (CONF_EIC_FILTEN15 << EIC_CONFIG_FILTEN7_Pos) | EIC_CONFIG_SENSE7(CONF_EIC_SENSE15)
	                             | 0);

	hri_eic_set_CTRLA_ENABLE_bit(EIC);
	NVIC_DisableIRQ(EIC_7_IRQn);
	NVIC_ClearPendingIRQ(EIC_7_IRQn);
	NVIC_EnableIRQ(EIC_7_IRQn);
	NVIC_DisableIRQ(EIC_15_IRQn);
	NVIC_ClearPendingIRQ(EIC_15_IRQn);
	NVIC_EnableIRQ(EIC_15_IRQn);

	callback = cb;

	return ERR_NONE;
}

/**
 * \brief De-initialize external interrupt module
 */
int32_t _ext_irq_deinit(void)
{
	NVIC_DisableIRQ(EIC_7_IRQn);
	NVIC_DisableIRQ(EIC_15_IRQn);
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
	int8_t			  skip_wilc_flag = 0;

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
				if (pin == PIN_PC23)	// skip wilc irq pin detection in next checking 
					skip_wilc_flag = 1;
			}
			flags &= ~(1ul << pos);
			pos = ffs(flags) - 1;
		}
		flags = hri_eic_read_INTFLAG_reg(EIC);
		if (skip_wilc_flag)
			flags &=(~0x80); 
		hri_eic_clear_INTFLAG_reg(EIC, flags);
	}
}

/**
 * \brief EIC interrupt handler
 */
void EIC_7_Handler(void)
{
	_ext_irq_handler();
}

void EIC_15_Handler(void)
{
	_ext_irq_handler();
}
