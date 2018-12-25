/* Microcontroller Library
 * Copyright (c) 2015-2016 Nuvoton
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "objects.h"
#include "nu_modutil.h"
#include "nu_bitutil.h"
#include "hal_status.h"
#include <k_api.h>

#include "aos/hal/pwm.h"
#include "board.h"


struct nu_pwm_var {
    uint32_t    en_msk;
};

static struct nu_pwm_var pwm0_var = {
    .en_msk = 0
};

static struct nu_pwm_var pwm1_var = {
    .en_msk = 0
};

static uint32_t pwm_modinit_mask = 0;

static const struct nu_modinit_s pwm_modinit_tab[] = {
    {PWM_0_0, EPWM0_MODULE, CLK_CLKSEL2_EPWM0SEL_PCLK0, 0, EPWM0_RST, EPWM0P0_IRQn, &pwm0_var},
    {PWM_0_1, EPWM0_MODULE, CLK_CLKSEL2_EPWM0SEL_PCLK0, 0, EPWM0_RST, EPWM0P0_IRQn, &pwm0_var},
    {PWM_0_2, EPWM0_MODULE, CLK_CLKSEL2_EPWM0SEL_PCLK0, 0, EPWM0_RST, EPWM0P1_IRQn, &pwm0_var},
    {PWM_0_3, EPWM0_MODULE, CLK_CLKSEL2_EPWM0SEL_PCLK0, 0, EPWM0_RST, EPWM0P1_IRQn, &pwm0_var},
    {PWM_0_4, EPWM0_MODULE, CLK_CLKSEL2_EPWM0SEL_PCLK0, 0, EPWM0_RST, EPWM0P2_IRQn, &pwm0_var},
    {PWM_0_5, EPWM0_MODULE, CLK_CLKSEL2_EPWM0SEL_PCLK0, 0, EPWM0_RST, EPWM0P2_IRQn, &pwm0_var},

    {PWM_1_0, EPWM1_MODULE, CLK_CLKSEL2_EPWM1SEL_PCLK1, 0, EPWM1_RST, EPWM1P0_IRQn, &pwm1_var},
    {PWM_1_1, EPWM1_MODULE, CLK_CLKSEL2_EPWM1SEL_PCLK1, 0, EPWM1_RST, EPWM1P0_IRQn, &pwm1_var},
    {PWM_1_2, EPWM1_MODULE, CLK_CLKSEL2_EPWM1SEL_PCLK1, 0, EPWM1_RST, EPWM1P1_IRQn, &pwm1_var},
    {PWM_1_3, EPWM1_MODULE, CLK_CLKSEL2_EPWM1SEL_PCLK1, 0, EPWM1_RST, EPWM1P1_IRQn, &pwm1_var},
    {PWM_1_4, EPWM1_MODULE, CLK_CLKSEL2_EPWM1SEL_PCLK1, 0, EPWM1_RST, EPWM1P2_IRQn, &pwm1_var},
    {PWM_1_5, EPWM1_MODULE, CLK_CLKSEL2_EPWM1SEL_PCLK1, 0, EPWM1_RST, EPWM1P2_IRQn, &pwm1_var},

    {NC, 0, 0, 0, 0, (IRQn_Type) 0, NULL}
};

static void platform_pwmout_config(struct pwmout_s* obj, int start)
{
	EPWM_T *pwm_base = (EPWM_T *) NU_MODBASE(obj->pwm);
	uint32_t chn = NU_MODSUBINDEX(obj->pwm);

    	EPWM_ForceStop(pwm_base, 1 << chn);

	EPWM_ConfigOutputChannel(pwm_base, chn, 1000000/obj->period_us, (100 - obj->pulsewidth_us * 100 / obj->period_us) );
	pwm_base->POLCTL |= 1 << (EPWM_POLCTL_PINV0_Pos + chn);

	if (start) {
        	// Enable output of the specified PWM channel
        	EPWM_EnableOutput(pwm_base, 1 << chn);
        	EPWM_Start(pwm_base, 1 << chn);
    	}
}

static int32_t platform_pwmout_init(struct pwmout_s* obj, PinName pin)
{
	struct nu_modinit_s *modinit = NULL;
	if ( obj->pwm != (PWMName) pinmap_peripheral(pin, PinMap_PWM) )
		goto exit_platform_pwmout_init;

		modinit = get_modinit(obj->pwm, pwm_modinit_tab);
    if ( !modinit || modinit->modname != (int) obj->pwm )
			goto exit_platform_pwmout_init;

    // NOTE: All channels (identified by PWMName) share a PWM module. This reset will also affect other channels of the same PWM module.
    if (! ((struct nu_pwm_var *) modinit->var)->en_msk) {
        // Reset this module if no channel enabled
        SYS_ResetModule(modinit->rsetidx);
    }

    uint32_t chn =  NU_MODSUBINDEX(obj->pwm);

    // NOTE: Channels 0/1/2/3/4/5 share a clock source.
    if ((((struct nu_pwm_var *) modinit->var)->en_msk & 0x3F) == 0) {
        // Select clock source of paired channels
        CLK_SetModuleClock(modinit->clkidx, modinit->clksrc, modinit->clkdiv);
        // Enable clock of paired channels
        CLK_EnableModuleClock(modinit->clkidx);
    }

    // Wire pinout
    pinmap_pinout(pin, PinMap_PWM);

    // Default: period = 10 ms, pulse width = 0 ms
    obj->period_us = 1000 * 10;
    obj->pulsewidth_us = 0;
    platform_pwmout_config(obj, 0);

    ((struct nu_pwm_var *) modinit->var)->en_msk |= 1 << chn;

    // Mark this module to be inited.
    int i = modinit - pwm_modinit_tab;
    pwm_modinit_mask |= 1 << i;

		return HAL_OK;
		
exit_platform_pwmout_init:		

		return HAL_ERROR;
}

static int platform_pwmout_free(struct pwmout_s* obj)
{
		struct nu_modinit_s *modinit = NULL;
    EPWM_T *pwm_base = (EPWM_T *) NU_MODBASE(obj->pwm);
    uint32_t chn =  NU_MODSUBINDEX(obj->pwm);
    EPWM_ForceStop(pwm_base, 1 << chn);

    modinit = get_modinit(obj->pwm, pwm_modinit_tab);
		if ( !modinit || (modinit->modname != (int) obj->pwm) )
			goto exit_platform_pwmout_free;
		
    ((struct nu_pwm_var *) modinit->var)->en_msk &= ~(1 << chn);

    if ((((struct nu_pwm_var *) modinit->var)->en_msk & 0x3F) == 0) {
        CLK_DisableModuleClock(modinit->clkidx);
    }

    // Mark this module to be deinited.
    int i = modinit - pwm_modinit_tab;
    pwm_modinit_mask &= ~(1 << i);

		return HAL_OK;
		
exit_platform_pwmout_free:
		return HAL_ERROR;
}


static struct pwmout_s* hal_get_pwmout_s ( pwm_dev_t *pwm )
{
	if ( !(pwm) || (pwm->port >= i32BoardMaxPWMNum) )
		goto exit_hal_get_pwmout_s;

	return (struct pwmout_s*)&board_pwm[pwm->port];

exit_hal_get_pwmout_s:
	
	return NULL;
}

/**
 * Initialises a PWM pin
 *
 *
 * @param[in]  pwm  the PWM device
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_pwm_init(pwm_dev_t *pwm)
{
	struct pwmout_s *obj = hal_get_pwmout_s(pwm);
	if ( !obj )
		goto exit_hal_pwm_init;
	
	return platform_pwmout_init(obj, obj->pin);

exit_hal_pwm_init:
	
	return HAL_ERROR;
}

/**
 * Starts Pulse-Width Modulation signal output on a PWM pin
 *
 * @param[in]  pwm  the PWM device
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_pwm_start(pwm_dev_t *pwm)
{
	struct pwmout_s *obj = hal_get_pwmout_s(pwm);
	if ( !obj )
		goto exit_hal_pwm_start;
	
	if ( pwm->config.freq )
	{
		obj->period_us 			= 1000000 / pwm->config.freq; //us
		obj->pulsewidth_us 	= obj->period_us * ((pwm->config.duty_cycle>1)?1:pwm->config.duty_cycle) ; //us
		platform_pwmout_config(obj, 1);
	} else
		platform_pwmout_config(obj, 0);
	
	return HAL_OK;

exit_hal_pwm_start:

	return HAL_ERROR;	
}

/**
 * Stops output on a PWM pin
 *
 * @param[in]  pwm  the PWM device
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_pwm_stop(pwm_dev_t *pwm)
{
	struct pwmout_s *obj = hal_get_pwmout_s(pwm);
	if ( !obj )
		goto exit_hal_pwm_stop;

	platform_pwmout_config(obj, 0);

	return HAL_OK;

exit_hal_pwm_stop:
	
	return HAL_ERROR;
}

/**
 * De-initialises an PWM interface, Turns off an PWM hardware interface
 *
 * @param[in]  pwm  the interface which should be de-initialised
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_pwm_finalize(pwm_dev_t *pwm)
{
	struct pwmout_s *obj = hal_get_pwmout_s(pwm);
	if ( !obj )
		goto exit_hal_pwm_finalize;

	platform_pwmout_free(obj);

	return HAL_OK;

exit_hal_pwm_finalize:

	return HAL_ERROR;
}
