/**
 * \file
 *
 * \brief External IRQ related functionality declaration.
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

#ifndef _HPL_EXT_IRQ_H_INCLUDED
#define _HPL_EXT_IRQ_H_INCLUDED

/**
 * \addtogroup HPL EXT IRQ
 *
 * \section hpl_ext_irq_rev Revision History
 * - v1.0.0 Initial Release
 *
 *@{
 */

#include <compiler.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \name HPL functions
 */
//@{
/**
 * \brief Initialize external interrupt module
 *
 * This function does low level external interrupt configuration.
 *
 * \param[in] cb The pointer to callback function from external interrupt
 *
 * \return Initialization status.
 * \retval -1 External irq module is already initialized
 * \retval 0 The initialization is completed successfully
 */
int32_t _ext_irq_init(void (*cb)(const uint32_t pin));

/**
 * \brief Deinitialize external interrupt module
 *
 * \return Initialization status.
 * \retval -1 External irq module is already deinitialized
 * \retval 0 The de-initialization is completed successfully
 */
int32_t _ext_irq_deinit(void);

/**
 * \brief Enable / disable external irq
 *
 * \param[in] pin Pin to enable external irq on
 * \param[in] enable True to enable, false to disable
 *
 * \return Status of external irq enabling / disabling
 * \retval -1 External irq module can't be enabled / disabled
 * \retval 0 External irq module is enabled / disabled successfully
 */
int32_t _ext_irq_enable(const uint32_t pin, const bool enable);
//@}

#ifdef __cplusplus
}
#endif
/**@}*/
#endif /* _HPL_EXT_IRQ_H_INCLUDED */
