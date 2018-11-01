/**
 * \file
 *
 * \brief External interrupt functionality imkplementation.
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

#include "hal_ext_irq.h"

#define EXT_IRQ_AMOUNT 1

/**
 * \brief Driver version
 */
#define DRIVER_VERSION 0x00000001u

/**
 * \brief External IRQ struct
 */
struct ext_irq {
	ext_irq_cb_t cb;
	uint32_t     pin;
};

/* Remove KEIL compiling error in case no IRQ line selected */
#if EXT_IRQ_AMOUNT == 0
#undef EXT_IRQ_AMOUNT
#define EXT_IRQ_AMOUNT 1
#endif

/**
 * \brief Array of external IRQs callbacks
 */
static struct ext_irq ext_irqs[EXT_IRQ_AMOUNT];

static void process_ext_irq(const uint32_t pin);

/**
 * \brief Initialize external irq component if any
 */
int32_t ext_irq_init(void)
{
	uint16_t i;

	for (i = 0; i < EXT_IRQ_AMOUNT; i++) {
		ext_irqs[i].pin = 0xFFFFFFFF;
		ext_irqs[i].cb  = NULL;
	}

	return _ext_irq_init(process_ext_irq);
}

/**
 * \brief Deinitialize external irq if any
 */
int32_t ext_irq_deinit(void)
{
	return _ext_irq_deinit();
}

/**
 * \brief Register callback for the given external interrupt
 */
int32_t ext_irq_register(const uint32_t pin, ext_irq_cb_t cb)
{
	uint8_t i = 0, j = 0;
	bool    found = false;

	for (; i < EXT_IRQ_AMOUNT; i++) {
		if (ext_irqs[i].pin == pin) {
			ext_irqs[i].cb = cb;
			found          = true;
			break;
		}
	}

	if (NULL == cb) {
		if (!found) {
			return ERR_INVALID_ARG;
		}
		return _ext_irq_enable(pin, false);
	}

	if (!found) {
		for (i = 0; i < EXT_IRQ_AMOUNT; i++) {
			if (NULL == ext_irqs[i].cb) {
				ext_irqs[i].cb  = cb;
				ext_irqs[i].pin = pin;
				found           = true;
				break;
			}
		}
		for (; (j < EXT_IRQ_AMOUNT) && (i < EXT_IRQ_AMOUNT); j++) {
			if ((ext_irqs[i].pin < ext_irqs[j].pin) && (ext_irqs[j].pin != 0xFFFFFFFF)) {
				struct ext_irq tmp = ext_irqs[j];

				ext_irqs[j] = ext_irqs[i];
				ext_irqs[i] = tmp;
			}
		}
	}

	if (!found) {
		return ERR_INVALID_ARG;
	}

	return _ext_irq_enable(pin, true);
}

/**
 * \brief Enable external irq
 */
int32_t ext_irq_enable(const uint32_t pin)
{
	return _ext_irq_enable(pin, true);
}

/**
 * \brief Disable external irq
 */
int32_t ext_irq_disable(const uint32_t pin)
{
	return _ext_irq_enable(pin, false);
}

/**
 * \brief Retrieve the current driver version
 */
uint32_t ext_irq_get_version(void)
{
	return DRIVER_VERSION;
}

/**
 * \brief Interrupt processing routine
 *
 * \param[in] pin The pin which triggered the interrupt
 */
static void process_ext_irq(const uint32_t pin)
{
	uint8_t lower = 0, middle, upper = EXT_IRQ_AMOUNT;

	while (upper >= lower) {
		middle = (upper + lower) >> 1;
		if (middle >= EXT_IRQ_AMOUNT) {
			return;
		}

		if (ext_irqs[middle].pin == pin) {
			if (ext_irqs[middle].cb) {
				ext_irqs[middle].cb();
			}
			return;
		}

		if (ext_irqs[middle].pin < pin) {
			lower = middle + 1;
		} else {
			upper = middle - 1;
		}
	}
}
