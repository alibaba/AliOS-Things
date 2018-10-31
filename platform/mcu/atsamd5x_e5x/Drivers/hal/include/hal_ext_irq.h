/**
 * \file
 *
 * \brief External interrupt functionality declaration.
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
