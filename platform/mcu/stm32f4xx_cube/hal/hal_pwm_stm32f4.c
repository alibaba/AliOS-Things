/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "stm32f4xx_hal.h"

#ifdef HAL_TIM_MODULE_ENABLED

#include <k_api.h>
#include <stdio.h>
#include <stdlib.h>
#include "aos/hal/gpio.h"
#include "aos/hal/pwm.h"

#include "hal_pwm_stm32f4.h"
#include "hal_gpio_stm32f4.h"

#define HAL_APB1_DIV_FREQ       64000UL

 typedef struct {
     TIM_HandleTypeDef hal_timer_handle;
     uint8_t           inited;
} stm32_pwm_t;

static stm32_pwm_t stm32_pwm[PORT_PWM_SIZE];

static PWM_MAPPING *get_mapping_entry_by_port(uint8_t port)
{
    int i;

    for (i = 0; i < PORT_TIMER_SIZE; i++) {
        if (port == PWM_MAPPING_TABLE[i].port) {
            return &PWM_MAPPING_TABLE[i];
        }
    }

    return NULL;
}

int32_t hal_pwm_init(pwm_dev_t *tim)
{
    RCC_ClkInitTypeDef clkconfig;
    uint32_t           uwTimclock, Prescaler = 0U;
    uint32_t           pFLatency;
    GPIO_InitTypeDef   GPIO_InitStruct;
    GPIO_TypeDef       *GPIOx;
    int      i;
    uint32_t uwPrescalerValue = 0;
    int32_t  ret = -1;

    if (tim == NULL) {
       return -1;
    }

    if (stm32_pwm[tim->port].inited == 1){
        return 0;
    }

    PWM_MAPPING* pwmIns = get_mapping_entry_by_port(tim->port);

    if (pwmIns != NULL)
    {
        memset(&stm32_pwm[tim->port],0,sizeof(stm32_pwm_t));
        TIM_HandleTypeDef * const psttimhandle = &(stm32_pwm[tim->port].hal_timer_handle);
		memset(psttimhandle, 0, sizeof(TIM_HandleTypeDef));
        psttimhandle->Instance = (TIM_TypeDef*)pwmIns->hal_timer;
        tim->priv = psttimhandle;

        for (i = 0; i < pwmIns->channel_cnt; i++) {
            GPIOx = hal_gpio_typedef(pwmIns->channels[i].out1.pin);
            if (NULL == GPIOx) {
                return -1;
            }
            uint16_t pin = hal_gpio_pin(pwmIns->channels[i].out1.pin);
            hal_gpio_enable_clk(GPIOx);

            GPIO_InitStruct.Pin = pin;
            GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
            GPIO_InitStruct.Pull = GPIO_PULLUP;
            GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
            GPIO_InitStruct.Alternate = pwmIns->channels[i].out1.alt;
            HAL_GPIO_Init(GPIOx, &GPIO_InitStruct);
        }

        /* Get clock configuration */
        HAL_RCC_GetClockConfig(&clkconfig, &pFLatency);

        switch (pwmIns->hal_timer) {
            case HAL_TIMER_1:
#ifdef HAL_TIMER_8
            case HAL_TIMER_8:
#endif
#ifdef HAL_TIMER_9
            case HAL_TIMER_9:
#endif
#ifdef HAL_TIMER_10
            case HAL_TIMER_10:
#endif
#ifdef HAL_TIMER_11
            case HAL_TIMER_11:
#endif
                Prescaler = clkconfig.APB2CLKDivider;
                uwTimclock = HAL_RCC_GetPCLK2Freq();
                break;
            case HAL_TIMER_2:
            case HAL_TIMER_3:
            case HAL_TIMER_4:
#ifdef HAL_TIMER_5
            case HAL_TIMER_5:
#endif
#ifdef HAL_TIMER_6
            case HAL_TIMER_6:
#endif
#ifdef HAL_TIMER_7
            case HAL_TIMER_7:
#endif
#ifdef HAL_TIMER_12
            case HAL_TIMER_12:
#endif
#ifdef HAL_TIMER_13
            case HAL_TIMER_13:
#endif
#ifdef HAL_TIMER_14
            case HAL_TIMER_14:
#endif
                Prescaler = clkconfig.APB1CLKDivider;
                uwTimclock = HAL_RCC_GetPCLK1Freq();
                break;
            default:
                return -1;
        }

        /* TIM clock frequence: if APBx prescaler is 1, then TIMx clock frequence
         * is PCKL1 or PCLK2, else is PCKL1 * 2 or PCLK2 * 2
         */
        if (Prescaler != RCC_HCLK_DIV1) {
            uwTimclock *= 2;
        }

        /* Compute the prescaler value to have TIMx counter clock equal to 20000 Hz */
        uwPrescalerValue = (uint32_t)(uwTimclock / HAL_APB1_DIV_FREQ) - 1;
        psttimhandle->Init.Period            = (HAL_APB1_DIV_FREQ / tim->config.freq) - 1;
        psttimhandle->Init.Prescaler         = uwPrescalerValue;
        psttimhandle->Init.ClockDivision     = TIM_CLOCKDIVISION_DIV1;
        psttimhandle->Init.CounterMode       = TIM_COUNTERMODE_UP;
        ret = HAL_TIM_PWM_Init(psttimhandle);
        if (HAL_OK == ret) {
            stm32_pwm[tim->port].inited = 1;
		}
    }

    return ret;
}

static int32_t timer_start(pwm_dev_t *tim)
{
    int i;
    TIM_OC_InitTypeDef sConfig;
    PWM_MAPPING* pwmIns = get_mapping_entry_by_port(tim->port);

    for (i = 0; i < pwmIns->channel_cnt; i++) {
        memset(&sConfig, 0, sizeof(sConfig));
        sConfig.OCMode       = pwmIns->channels[i].mode;
        sConfig.OCPolarity   = pwmIns->channels[i].out1.pol;
        sConfig.Pulse        = tim->config.duty_cycle*(HAL_APB1_DIV_FREQ / tim->config.freq);
        HAL_TIM_PWM_ConfigChannel(tim->priv, &sConfig, pwmIns->channels[i].channel);

        HAL_TIM_PWM_Start(tim->priv, pwmIns->channels[i].channel);
    }

    return 0;
}

int32_t hal_pwm_start(pwm_dev_t *tim)
{
    int32_t ret = -1;

    if(tim != NULL && tim->port < PORT_PWM_INVALID){
        if (!stm32_pwm[tim->port].inited) {
            ret = -1;
        } else {
            ret = timer_start(tim);
        }
    }
    return ret;
}

static int32_t timer_change(pwm_dev_t *tim, pwm_config_t *para)
{
    int i;
    TIM_HandleTypeDef * const psttimhandle = &stm32_pwm[tim->port].hal_timer_handle;
    TIM_OC_InitTypeDef sConfig;

    tim->config.freq = para->freq;
    tim->config.duty_cycle = para->duty_cycle;

    psttimhandle->Init.Period = (HAL_APB1_DIV_FREQ / tim->config.freq) - 1;
    TIM_Base_SetConfig(psttimhandle->Instance, &(psttimhandle->Init));

    PWM_MAPPING* pwmIns = get_mapping_entry_by_port(tim->port);
    if (NULL == pwmIns) {
        return -1;
    }

    for (i = 0; i < pwmIns->channel_cnt; i++) {
        sConfig.OCMode       = pwmIns->channels[i].mode;
        sConfig.OCPolarity   = pwmIns->channels[i].out1.pol;
        sConfig.Pulse        = (tim->config.duty_cycle) * (HAL_APB1_DIV_FREQ / tim->config.freq);

        HAL_TIM_PWM_ConfigChannel(tim->priv, &sConfig, pwmIns->channels[i].channel);

		HAL_TIM_PWM_Start(tim->priv, pwmIns->channels[i].channel);
    }

    return 0;
}


int32_t hal_pwm_para_chg(pwm_dev_t *pwm, pwm_config_t para)
{
    int32_t ret = -1;

    if(pwm != NULL && pwm->port < PORT_PWM_INVALID){
        if(!stm32_pwm[pwm->port].inited){
            ret = -1;
        }else{
            ret = timer_change(pwm, &para);
        }
    }
    return ret;
}

int32_t hal_pwm_stop(pwm_dev_t *tim)
{
    int i;
    PWM_MAPPING* timerIns = get_mapping_entry_by_port(tim->port);

    for (i = 0; i < timerIns->channel_cnt; i++) {
        HAL_TIM_PWM_Stop((TIM_HandleTypeDef *)tim->priv, timerIns->channels[i].channel);
    }

	return 0;
}

int32_t hal_pwm_finalize(pwm_dev_t *tim)
{
    int32_t ret = -1;

	if (NULL == tim) {
		return -1;
	}

	if (stm32_pwm[tim->port].inited != 1) {
		return -1;
	}

    HAL_TIM_PWM_DeInit((TIM_HandleTypeDef *)tim->priv);

	memset(&stm32_pwm[tim->port], 0, sizeof(stm32_pwm_t));
	stm32_pwm[tim->port].inited = 0;

    return 0;
}

#endif /* HAL_TIM_MODULE_ENABLED */
