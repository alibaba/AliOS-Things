/**
 * \file
 *
 * \brief SAM TCC
 *
 * Copyright (C) 2014-2017 Atmel Corporation. All rights reserved.
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
#include <hpl_pwm.h>
#include <hpl_tcc.h>
#include <hpl_tcc_config.h>
#include <hpl_timer.h>
#include <utils.h>
#include <utils_assert.h>

/**
 * \brief Macro is used to fill TCC configuration structure based on its number
 *
 * \param[in] n The number of structures
 */
#define TCC_CONFIGURATION(n)                                                                                           \
	{                                                                                                                  \
		n, TCC_CTRLA_PRESCALER(CONF_TCC##n##_PRESCALER) | (CONF_TCC##n##_RUNSTDBY << TCC_CTRLA_RUNSTDBY_Pos)           \
		       | TCC_CTRLA_PRESCSYNC(CONF_TCC##n##_PRESCSYNC) | (CONF_TCC##n##_ALOCK << TCC_CTRLA_ALOCK_Pos)           \
		       | (CONF_TCC##n##_CPTEN0 << TCC_CTRLA_CPTEN0_Pos) | (CONF_TCC##n##_CPTEN1 << TCC_CTRLA_CPTEN1_Pos)       \
		       | (CONF_TCC##n##_CPTEN2 << TCC_CTRLA_CPTEN2_Pos) | (CONF_TCC##n##_CPTEN3 << TCC_CTRLA_CPTEN3_Pos),      \
		    (CONF_TCC##n##_LUPD << TCC_CTRLBSET_LUPD_Pos), (CONF_TCC##n##_DBGRUN << TCC_DBGCTRL_DBGRUN_Pos),           \
		    TCC_EVCTRL_CNTSEL(CONF_TCC##n##_CNTSEL) | (CONF_TCC##n##_OVFEO << TCC_EVCTRL_OVFEO_Pos)                    \
		        | (CONF_TCC##n##_TRGEO << TCC_EVCTRL_TRGEO_Pos) | (CONF_TCC##n##_CNTEO << TCC_EVCTRL_CNTEO_Pos)        \
		        | (CONF_TCC##n##_MCEO0 << TCC_EVCTRL_MCEO0_Pos) | (CONF_TCC##n##_MCEO1 << TCC_EVCTRL_MCEO1_Pos)        \
		        | (CONF_TCC##n##_MCEO2 << TCC_EVCTRL_MCEO2_Pos) | (CONF_TCC##n##_MCEO3 << TCC_EVCTRL_MCEO3_Pos)        \
		        | (CONF_TCC##n##_MCEI0 << TCC_EVCTRL_MCEI0_Pos) | (CONF_TCC##n##_MCEI1 << TCC_EVCTRL_MCEI1_Pos)        \
		        | (CONF_TCC##n##_MCEI2 << TCC_EVCTRL_MCEI2_Pos) | (CONF_TCC##n##_MCEI3 << TCC_EVCTRL_MCEI3_Pos)        \
		        | (CONF_TCC##n##_TCEI0 << TCC_EVCTRL_TCEI0_Pos) | (CONF_TCC##n##_TCEI1 << TCC_EVCTRL_TCEI1_Pos)        \
		        | (CONF_TCC##n##_TCINV0 << TCC_EVCTRL_TCINV0_Pos) | (CONF_TCC##n##_TCINV1 << TCC_EVCTRL_TCINV1_Pos)    \
		        | TCC_EVCTRL_EVACT1(CONF_TCC##n##_EVACT1) | TCC_EVCTRL_EVACT0(CONF_TCC##n##_EVACT0),                   \
		    CONF_TCC##n##_PER, CONF_TCC##n##_CC0, CONF_TCC##n##_CC1, CONF_TCC##n##_CC2, CONF_TCC##n##_CC3              \
	}
/**
 * \brief Macro is used to fill pwm waveform configuration structure based on its
 * number
 *
 * \param[in] n The number of structures
 */

#define TCC_PWM_CONFIGURATION(n)                                                                                       \
	{                                                                                                                  \
		n, CONF_TCC##n##_SEL_CH, CONF_TCC##n##_PER_REG, CONF_TCC##n##_CCX_REG,                                         \
		    (CONF_TCC##n##_WAVEGEN << TCC_WAVE_WAVEGEN_Pos),                                                           \
	}
/* Renamed access REG name PERB -> PERBUF */
#define hri_tcc_write_PERB_reg hri_tcc_write_PERBUF_reg
#define hri_tcc_read_PERB_reg hri_tcc_read_PERBUF_reg

/** Renamed access REG name CCB -> CCBUF */
#define hri_tcc_write_CCB_reg hri_tcc_write_CCBUF_reg
#define hri_tcc_read_CCB_reg hri_tcc_read_CCBUF_reg

#ifndef CONF_TCC0_ENABLE
#define CONF_TCC0_ENABLE 0
#endif
#ifndef CONF_TCC1_ENABLE
#define CONF_TCC1_ENABLE 0
#endif
#ifndef CONF_TCC2_ENABLE
#define CONF_TCC2_ENABLE 0
#endif
#ifndef CONF_TCC3_ENABLE
#define CONF_TCC3_ENABLE 0
#endif
#ifndef CONF_TCC4_ENABLE
#define CONF_TCC4_ENABLE 0
#endif

/** Amount of TCC used. */
#define TCC_AMOUNT (CONF_TCC0_ENABLE + CONF_TCC1_ENABLE + CONF_TCC2_ENABLE + CONF_TCC3_ENABLE + CONF_TCC4_ENABLE)
/**
 * \brief TCC configuration type
 */
struct tcc_configuration {
	uint8_t                number;
	hri_tcc_ctrla_reg_t    ctrl_a;
	hri_tcc_ctrlbset_reg_t ctrl_b;
	hri_tcc_dbgctrl_reg_t  dbg_ctrl;
	hri_tcc_evctrl_reg_t   event_ctrl;
	hri_tcc_per_reg_t      per;
	hri_tcc_cc_reg_t       cc0;
	hri_tcc_cc_reg_t       cc1;
	hri_tcc_cc_reg_t       cc2;
	hri_tcc_cc_reg_t       cc3;
};

#if TCC_AMOUNT < 1
/** Dummy array to pass compiling. */
static struct tcc_configuration _tccs[1] = 0;
#else
#ifndef TCC_CONFIGURATION
/* remove compiling error. */
#define TCC_CONFIGURATION(n)                                                                                           \
	{                                                                                                                  \
		0                                                                                                              \
	}
#endif

/**
 * \brief Array of TCC configurations
 */
static struct tcc_configuration _tccs[] = {
#if CONF_TCC0_ENABLE == 1
    TCC_CONFIGURATION(0),
#endif
#if CONF_TCC1_ENABLE == 1
    TCC_CONFIGURATION(1),
#endif
#if CONF_TCC2_ENABLE == 1
    TCC_CONFIGURATION(2),
#endif
#if CONF_TCC3_ENABLE == 1
    TCC_CONFIGURATION(3),
#endif
#if CONF_TCC4_ENABLE == 1
    TCC_CONFIGURATION(4),
#endif
};
#endif
/**
 * \brief pwm configuration type
 */
struct tcc_pwm_configuration {
	uint8_t  number;
	uint8_t  sel_ch;
	uint32_t period;
	uint32_t duty_cycle;
	uint32_t wave;
};

#if TCC_AMOUNT < 1
/** Dummy array to pass compiling. */
static struct tcc_pwm_configuration _tcc_pwms[1] = 0;
#else
#ifndef TCC_PWM_CONFIGURATION
/* remove compiling error. */
#define TCC_PWM_CONFIGURATION(n)                                                                                       \
	{                                                                                                                  \
		0                                                                                                              \
	}
#endif

/**
 * \brief Array of PWM configurations
 */
static struct tcc_pwm_configuration _tcc_pwms[] = {
#if CONF_TCC0_PWM_ENABLE == 1
    TCC_PWM_CONFIGURATION(0),
#endif
#if CONF_TCC1_PWM_ENABLE == 1
    TCC_PWM_CONFIGURATION(1),
#endif
#if CONF_TCC2_PWM_ENABLE == 1
    TCC_PWM_CONFIGURATION(2),
#endif
#if CONF_TCC3_PWM_ENABLE == 1
    TCC_PWM_CONFIGURATION(3),
#endif
#if CONF_TCC4_PWM_ENABLE == 1
    TCC_PWM_CONFIGURATION(4),
#endif
};
#endif

static struct _pwm_device *_tcc0_dev = NULL;

static struct _pwm_device *_tcc2_dev = NULL;

/**
 * \brief Set of pointer to hal_pwm helper functions
 */
static struct _pwm_hpl_interface _tcc_pwm_functions = {
    _tcc_pwm_init,
    _tcc_pwm_deinit,
    _tcc_start_pwm,
    _tcc_stop_pwm,
    _tcc_set_pwm_param,
    _tcc_is_pwm_enabled,
    _tcc_pwm_get_period,
    _tcc_pwm_get_duty,
    _tcc_pwm_set_irq_state,
};
static int8_t get_tcc_index(const void *const hw);
static int8_t get_tcc_pwm_index(const void *const hw);

static inline uint8_t _get_hardware_offset(const void *const hw);

/**
 * \brief Init irq param with the given tcc hardware instance
 */
static void _talisds_init_irq_param(const void *const hw, void *dev)
{
	if (hw == TCC0) {
		_tcc0_dev = (struct _pwm_device *)dev;
	}
	if (hw == TCC2) {
		_tcc2_dev = (struct _pwm_device *)dev;
	}
}
/**
 * \brief Initialize TCC for PWM mode
 */
int32_t _tcc_pwm_init(struct _pwm_device *const device, void *const hw)
{
	int8_t i = get_tcc_index(hw);
	int8_t j = get_tcc_pwm_index(hw);

	device->hw = hw;
	ASSERT(ARRAY_SIZE(_tcc_pwms));

	hri_tcc_wait_for_sync(hw, TCC_SYNCBUSY_SWRST);
	if (hri_tcc_get_CTRLA_ENABLE_bit(hw)) {
		return ERR_DENIED;
	}
	hri_tcc_set_CTRLA_SWRST_bit(hw);
	hri_tcc_wait_for_sync(hw, TCC_SYNCBUSY_SWRST);

	hri_tcc_write_CTRLA_reg(hw, _tccs[i].ctrl_a);
	hri_tcc_set_CTRLB_reg(hw, _tccs[i].ctrl_b);
	hri_tcc_write_DBGCTRL_reg(hw, _tccs[i].dbg_ctrl);
	hri_tcc_write_EVCTRL_reg(hw, _tccs[i].event_ctrl);

	hri_tcc_write_WAVE_reg(hw, _tcc_pwms[j].wave);
	hri_tcc_write_PER_reg(hw, _tcc_pwms[j].period);
	if (0 == _tcc_pwms[j].sel_ch) {
		_tccs[i].cc0 = _tcc_pwms[j].duty_cycle;
		hri_tcc_write_CC_reg(hw, 0, _tccs[i].cc0);
	} else if (1 == _tcc_pwms[j].sel_ch) {
		_tccs[i].cc1 = _tcc_pwms[j].duty_cycle;
		hri_tcc_write_CC_reg(hw, 1, _tccs[i].cc1);
	} else if (2 == _tcc_pwms[j].sel_ch) {
		_tccs[i].cc2 = _tcc_pwms[j].duty_cycle;
		hri_tcc_write_CC_reg(hw, 2, _tccs[i].cc2);
	} else if (3 == _tcc_pwms[j].sel_ch) {
		_tccs[i].cc3 = _tcc_pwms[j].duty_cycle;
		hri_tcc_write_CC_reg(hw, 3, _tccs[i].cc3);
	} else {
		return ERR_NO_RESOURCE;
	}

	hri_tcc_clear_CTRLB_LUPD_bit(hw);

	_talisds_init_irq_param(hw, (void *)device);
	NVIC_DisableIRQ((IRQn_Type)((uint8_t)TCC0_IRQn + _get_hardware_offset(hw)));
	NVIC_ClearPendingIRQ((IRQn_Type)((uint8_t)TCC0_IRQn + _get_hardware_offset(hw)));
	NVIC_EnableIRQ((IRQn_Type)((uint8_t)TCC0_IRQn + _get_hardware_offset(hw)));

	return ERR_NONE;
}
/**
 * \brief De-initialize TCC for PWM mode
 */
void _tcc_pwm_deinit(struct _pwm_device *const device)
{
	void *const hw = device->hw;
	NVIC_DisableIRQ((IRQn_Type)((uint8_t)TCC0_IRQn + _get_hardware_offset(hw)));

	hri_tcc_clear_CTRLA_ENABLE_bit(hw);
	hri_tcc_set_CTRLA_SWRST_bit(hw);
}
/**
 * \brief Retrieve offset of the given tcc hardware instance
 */
uint8_t _tcc_pwm_get_hardware_offset(const struct _pwm_device *const device)
{
	return _get_hardware_offset(device->hw);
}
/**
 * \brief Start PWM
 */
void _tcc_start_pwm(struct _pwm_device *const device)
{
	hri_tcc_set_CTRLA_ENABLE_bit(device->hw);
}
/**
 * \brief Stop PWM
 */
void _tcc_stop_pwm(struct _pwm_device *const device)
{
	hri_tcc_clear_CTRLA_ENABLE_bit(device->hw);
}
/**
 * \brief Set PWM parameter
 */
void _tcc_set_pwm_param(struct _pwm_device *const device, const pwm_period_t period, const pwm_period_t duty_cycle)
{
	void *const hw = device->hw;
	int8_t      i  = get_tcc_pwm_index(hw);

	hri_tcc_write_PERB_reg(hw, period);
	hri_tcc_write_CCB_reg(hw, _tcc_pwms[i].sel_ch, duty_cycle);
}
/**
 * \brief Get pwm waveform period value
 */
pwm_period_t _tcc_pwm_get_period(const struct _pwm_device *const device)
{
	return (pwm_period_t)(hri_tcc_read_PERB_reg(device->hw));
}
/**
 * \brief Get pwm waveform duty cycle
 */
uint32_t _tcc_pwm_get_duty(const struct _pwm_device *const device)
{
	void *const hw         = device->hw;
	int8_t      i          = get_tcc_pwm_index(hw);
	uint32_t    per        = hri_tcc_read_PERB_reg(hw);
	uint32_t    duty_cycle = hri_tcc_read_CCB_reg(hw, _tcc_pwms[i].sel_ch);

	return ((duty_cycle * 1000) / per);
}
/**
 * \brief Check if PWM is running
 */
bool _tcc_is_pwm_enabled(const struct _pwm_device *const device)
{
	return hri_tcc_get_CTRLA_ENABLE_bit(device->hw);
}
/**
 * \brief Enable/disable PWM interrupt
 */
void _tcc_pwm_set_irq_state(struct _pwm_device *const device, const enum _pwm_callback_type type, const bool disable)
{
	ASSERT(device);

	if (PWM_DEVICE_PERIOD_CB == type) {
		hri_tcc_write_INTEN_OVF_bit(device->hw, disable);
	} else if (PWM_DEVICE_ERROR_CB == type) {
		hri_tcc_write_INTEN_ERR_bit(device->hw, disable);
	}
}

/**
 * \brief Retrieve timer helper functions
 */
struct _timer_hpl_interface *_tcc_get_timer(void)
{
	return NULL;
}

/**
 * \brief Retrieve pwm helper functions
 */
struct _pwm_hpl_interface *_tcc_get_pwm(void)
{
	return &_tcc_pwm_functions;
}
/**
 * \internal TC interrupt handler for PWM
 *
 * \param[in] instance TC instance number
 */
static void tcc_pwm_interrupt_handler(struct _pwm_device *device)
{
	void *const hw = device->hw;

	if (hri_tcc_get_interrupt_OVF_bit(hw)) {
		hri_tcc_clear_interrupt_OVF_bit(hw);
		if (NULL != device->callback.pwm_period_cb) {
			device->callback.pwm_period_cb(device);
		}
	}
	if (hri_tcc_get_INTEN_ERR_bit(hw)) {
		hri_tcc_clear_interrupt_ERR_bit(hw);
		if (NULL != device->callback.pwm_error_cb) {
			device->callback.pwm_error_cb(device);
		}
	}
}

/**
* \brief TCC interrupt handler
*/
void TCC0_Handler(void)
{
	tcc_pwm_interrupt_handler(_tcc0_dev);
}

/**
* \brief TCC interrupt handler
*/
void TCC2_Handler(void)
{
	tcc_pwm_interrupt_handler(_tcc2_dev);
}

/**
 * \internal Retrieve TCC index
 *
 * \param[in] hw The pointer to hardware instance
 *
 * \return The index of selected TCC
 */
static int8_t get_tcc_index(const void *const hw)
{
	uint8_t tcc_offset = _get_hardware_offset(hw);
	uint8_t i;

	for (i = 0; i < ARRAY_SIZE(_tccs); i++) {
		if (_tccs[i].number == tcc_offset) {
			return i;
		}
	}

	ASSERT(false);
	return -1;
}
/**
 * \internal Retrieve TCC PWM Configuation index
 *
 * \param[in] hw The pointer to hardware instance
 *
 * \return The index of TCC PWM configuration for this TCC
 */
static int8_t get_tcc_pwm_index(const void *const hw)
{
	uint8_t tcc_offset = _get_hardware_offset(hw);
	uint8_t i;

	for (i = 0; i < ARRAY_SIZE(_tcc_pwms); i++) {
		if (_tcc_pwms[i].number == tcc_offset) {
			return i;
		}
	}

	ASSERT(false);
	return -1;
}

static inline uint8_t _get_hardware_offset(const void *const hw)
{
#ifndef _UNIT_TEST_
	return ((uint32_t)hw - (uint32_t)TCC0) >> 10;
#else
	return ((uint32_t)hw - (uint32_t)TCC0) / sizeof(Tcc);
#endif
}
