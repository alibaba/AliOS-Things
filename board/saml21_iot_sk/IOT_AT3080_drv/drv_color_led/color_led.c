/**
 ******************************************************************************
 * @file    color_led.c
 * @author  William Xu
 * @version V1.0.0
 * @date    9-Apr-2018
 * @brief   PWM driver for RGB led
 ******************************************************************************
 *
 * Copyright (c) 2009-2018 MXCHIP Co.,Ltd.
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
 *
 ******************************************************************************
 */

#include <stdlib.h>
#include <compiler.h>
#include <hpl_tcc_config.h>
#include <hpl_tc_config.h>
#include <hal_pwm.h>

#include "color_led.h"
 

extern struct pwm_descriptor PWM_B;
extern struct pwm_descriptor PWM_R;
extern struct pwm_descriptor PWM_G;

/******************************************************************************
 *                              Function Definitions
 ******************************************************************************/

void color_led_init( void )
{
	/* PWM pins configured in driver_init.c */
}

void color_led_open_rgb(uint8_t red, uint8_t green, uint8_t blue)
{
	/* 1M Hz PWM */
	pwm_set_parameters(&PWM_R, CONF_TCC0_PER_REG, ((uint32_t)(((double)(double)CONF_TCC0_PER_REG * red) / 255)));
	pwm_set_parameters(&PWM_G, CONF_TCC2_PER_REG, ((uint32_t)(((double)(double)CONF_TCC2_PER_REG * green) / 255)));
	pwm_set_parameters(&PWM_B, CONF_TC0_CC0, ((CONF_TC0_CC0 * blue) / 255));
	pwm_enable(&PWM_R);
	pwm_enable(&PWM_G);
	pwm_enable(&PWM_B);
}

void color_led_close(void)
{
	color_led_init();
	color_led_open_rgb(0, 0, 0);
}
