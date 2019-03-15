/**
 * \file
 *
 * \brief SAM Timer/Counter for Control Applications
 *
 * Copyright (C) 2014-2016 Atmel Corporation. All rights reserved.
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
 *    and/or other materials provided with the distributionn.
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

#ifndef _HPL_TCC_V101_BASE_H_INCLUDED
#define _HPL_TCC_V101_BASE_H_INCLUDED

#include <hpl_timer.h>
#include <hpl_pwm.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup tcc_group TCC Low Level Driver Helpers
 *
 * \section tcc_helpers_rev Revision History
 * - v0.0.0.1 Initial Commit
 *
 *@{
 */

/**
 * \name HPL functions
 */
//@{

/**
 * \brief Retrieve timer helper functions
 *
 * \return A pointer to set of timer helper functions
 */
struct _timer_hpl_interface *_tcc_get_timer(void);
/**
 * \brief Initialize TCC for PWM
 *
 * This function does low level TCC configuration.
 *
 * \param[in] device The pointer to PWM device instance
 * \param[in] hw The pointer to hardware instance
 *
 * \return Initialization status.
 */
int32_t _tcc_pwm_init(struct _pwm_device *const device, void *const hw);

/**
 * \brief De-initialize TCC for PWM
 *
 * \param[in] device The pointer to TCC device instance
 */
void _tcc_pwm_deinit(struct _pwm_device *const device);

/**
 * \brief Retrieve offset of the given tcc hardware instance
 *
 * \param[in] device The pointer to TCC device instance
 *
 * \return The offset of the given tcc hardware instance
 */
uint8_t _tcc_pwm_get_hardware_offset(const struct _pwm_device *const device);

/**
 * \brief Start PWM
 *
 * \param[in] device The pointer to TCC device instance
 */
void _tcc_start_pwm(struct _pwm_device *const device);

/**
 * \brief Stop PWM
 *
 * \param[in] device The pointer to TCC device instance
 */
void _tcc_stop_pwm(struct _pwm_device *const device);

/**
 * \brief Check if PWM is running
 *
 * \param[in] device The pointer to TCC device instance
 *
 * \return Check status.
 * \retval true The given pwm is running
 * \retval false The given pwm is not running
 */
bool _tcc_is_pwm_enabled(const struct _pwm_device *const device);

/**
 * \brief Set PWM parameter
 * \param[in] device The pointer to TCC device instance
 * \param[in] period Total period of one PWM cycle.
 * \param[in] duty_cycle Period of PWM first half during one cycle.
 */
void _tcc_set_pwm_param(struct _pwm_device *const device, const pwm_period_t period, const pwm_period_t duty_cycle);

/**
 * \brief Get pwm waveform period value
 * \param[in] device The pointer to TCC device instance
 * \return Period value.
 */
pwm_period_t _tcc_pwm_get_period(const struct _pwm_device *const device);

/**
 * \brief Get pwm waveform duty cycle value
 * \param[in] device The pointer to TCC device instance
 * \return Duty cycle value
 */
uint32_t _tcc_pwm_get_duty(const struct _pwm_device *const device);

/**
 * \brief Enable/disable PWM interrupt
 *
 * param[in] device The pointer to PWM device instance
 * param[in] type The type of interrupt to disable/enable if applicable
 * param[in] disable Enable or disable
 */
void _tcc_pwm_set_irq_state(struct _pwm_device *const device, const enum _pwm_callback_type type, const bool disable);

/**
 * \brief Retrieve pwm helper functions
 *
 * \return A pointer to set of pwm helper functions
 */
struct _pwm_hpl_interface *_tcc_get_pwm(void);

//@}
/**@}*/

#ifdef __cplusplus
}
#endif
#endif /* _HPL_TCC_V101_BASE_H_INCLUDED */
