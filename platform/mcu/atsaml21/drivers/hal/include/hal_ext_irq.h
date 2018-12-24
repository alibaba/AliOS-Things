/**
 * \file
 *
 * \brief External interrupt functionality declaration.
 *
 * Copyright (C) 2015 Atmel Corporation. All rights reserved.
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

#ifndef _HAL_EXT_IRQ_H_INCLUDED
#define _HAL_EXT_IRQ_H_INCLUDED

#include <hpl_ext_irq.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup doc_driver_hal_ext_irq
 *
 * @{
 */

/**
 * \brief External IRQ callback type
 */
typedef void (*ext_irq_cb_t)(void);

/**
 * \brief Initialize external IRQ component, if any
 *
 * \return Initialization status.
 * \retval -1 External IRQ module is already initialized
 * \retval 0 The initialization is completed successfully
 */
int32_t ext_irq_init(void);

/**
 * \brief Deinitialize external IRQ, if any
 *
 * \return De-initialization status.
 * \retval -1 External IRQ module is already deinitialized
 * \retval 0 The de-initialization is completed successfully
 */
int32_t ext_irq_deinit(void);

/**
 * \brief Register callback for the given external interrupt
 *
 * \param[in] pin Pin to enable external IRQ on
 * \param[in] cb Callback function
 *
 * \return Registration status.
 * \retval -1 Passed parameters were invalid
 * \retval 0 The callback registration is completed successfully
 */
int32_t ext_irq_register(const uint32_t pin, ext_irq_cb_t cb);

/**
 * \brief Enable external IRQ
 *
 * \param[in] pin Pin to enable external IRQ on
 *
 * \return Enabling status.
 * \retval -1 Passed parameters were invalid
 * \retval 0 The enabling is completed successfully
 */
int32_t ext_irq_enable(const uint32_t pin);

/**
 * \brief Disable external IRQ
 *
 * \param[in] pin Pin to enable external IRQ on
 *
 * \return Disabling status.
 * \retval -1 Passed parameters were invalid
 * \retval 0 The disabling is completed successfully
 */
int32_t ext_irq_disable(const uint32_t pin);

/**
 * \brief Retrieve the current driver version
 *
 * \return Current driver version.
 */
uint32_t ext_irq_get_version(void);
/**@}*/

#ifdef __cplusplus
}
#endif

#endif /* _HAL_EXT_IRQ_H_INCLUDED */
