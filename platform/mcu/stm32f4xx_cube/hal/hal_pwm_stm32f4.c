/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include <stdio.h>
#include <stdlib.h>
#include "aos/hal/gpio.h"
#include "aos/hal/pwm.h"

#include "stm32f4xx_hal.h"
#ifdef HAL_TIM_MODULE_ENABLED
#include "hal_pwm_stm32f4.h"
#include "hal_gpio_stm32f4.h"
#include "stm32f4xx_hal_adc.h"

#define HAL_APB1_DIV_FREQ       64000UL

typedef struct {
    uint8_t                inited;
    TIM_HandleTypeDef      hal_timer_handle;
}stm32_pwm_t;

static stm32_pwm_t stm32_pwm[PORT_PWM_SIZE];

/* Get TIMER Instanse & attribute from Logical Port */
static PWM_MAPPING* get_pwm_list_logical(const PORT_PWM_TYPE port)
{
    int8_t i = 0;
    PWM_MAPPING* rc = NULL;
    for(i=0; i<PORT_PWM_SIZE; i++)
    {
        if(PWM_MAPPING_TABLE[i].logical_func == port)
        {
            rc = &PWM_MAPPING_TABLE[i];
            break;
        }
    }
    return rc;
}

static PORT_PWM_TYPE get_pwm_logical_port(void* physical_port)
{
    int8_t i = 0;
    PORT_PWM_TYPE type = PORT_PWM_INVALID;
    if(NULL == physical_port){
        return PORT_PWM_INVALID;
    }
    for(i=0; i<PORT_PWM_SIZE; i++)
    {
        if(PWM_MAPPING_TABLE[i].physical_port == physical_port)
        {
            type = PWM_MAPPING_TABLE[i].logical_func;
            break;
        }
    }
    return type;
}


#ifdef TIM4
void TIM4_IRQHandler(void)
{
    PORT_PWM_TYPE pwm_port = PORT_PWM_INVALID;
    krhino_intrpt_enter();
    pwm_port = get_pwm_logical_port(TIM4);
    if(PORT_PWM_INVALID != pwm_port && (stm32_pwm[pwm_port].inited) ){
        HAL_TIM_IRQHandler(&(stm32_pwm[pwm_port].hal_timer_handle));
    }
    krhino_intrpt_exit();
}
#endif

static int32_t timer4_init(pwm_dev_t *tim)
{
    int32_t ret = -1;
    RCC_ClkInitTypeDef    clkconfig;
    uint32_t              uwTimclock, uwAPB1Prescaler = 0U;
    uint32_t              pFLatency;
    uint32_t uwPrescalerValue = 0;

    TIM_HandleTypeDef * const psttimhandle = &stm32_pwm[tim->port].hal_timer_handle;
    __HAL_RCC_TIM4_CLK_ENABLE();
    /* Get clock configuration */
    HAL_RCC_GetClockConfig(&clkconfig, &pFLatency);
    /* Get APB1 prescaler */
    uwAPB1Prescaler = clkconfig.APB1CLKDivider;
    /* Compute TIM4 clock */
    if (uwAPB1Prescaler == RCC_HCLK_DIV1)
    {
        uwTimclock = HAL_RCC_GetPCLK1Freq();
    }
    else
    {
        uwTimclock = 2*HAL_RCC_GetPCLK1Freq();
    }

    /* Compute the prescaler value to have TIMx counter clock equal to 20000 Hz */
    uwPrescalerValue = (uint32_t)(uwTimclock / HAL_APB1_DIV_FREQ) - 1;
    psttimhandle->Init.Period            = (HAL_APB1_DIV_FREQ / tim->config.freq) - 1;
    psttimhandle->Init.Prescaler         = uwPrescalerValue;
    psttimhandle->Init.ClockDivision     = 0;
    psttimhandle->Init.CounterMode       = TIM_COUNTERMODE_UP;
    psttimhandle->Init.RepetitionCounter = 0;

    ret = HAL_TIM_PWM_Init(psttimhandle);

    stm32_pwm[tim->port].inited = 1;

    return ret;
}

int32_t hal_pwm_init(pwm_dev_t *tim)
{
    int32_t ret = -1;

    if (tim == NULL) {
       return -1;
    }

    if (stm32_pwm[tim->port].inited == 1){
        return 0;
    }

    PWM_MAPPING* pwmIns = get_pwm_list_logical(tim->port);

    if (pwmIns != NULL)
    {
        memset(&stm32_pwm[tim->port],0,sizeof(stm32_pwm_t));
        TIM_HandleTypeDef * const psttimhandle = &(stm32_pwm[tim->port].hal_timer_handle);
        psttimhandle->Instance = pwmIns->physical_port;
        tim->priv = psttimhandle;
        if (pwmIns->physical_port == TIM4){
            gpio_dev_t gpio_pwm;
            gpio_pwm.port = ON_BOARD_TIM4_CH4;
            gpio_pwm.config = OUTPUT_PUSH_PULL_AF;
            hal_gpio_init(&gpio_pwm);

            ret = timer4_init(tim);
        }
    }

    return ret;
}

static int32_t timer4_start(pwm_dev_t *tim, uint32_t channel)
{
    TIM_OC_InitTypeDef sConfig;
    sConfig.OCMode       = TIM_OCMODE_PWM1;
    sConfig.OCPolarity   = TIM_OCPOLARITY_HIGH;
    sConfig.Pulse        = (tim->config.duty_cycle) * (HAL_APB1_DIV_FREQ / tim->config.freq);
    sConfig.OCNPolarity  = TIM_OCNPOLARITY_HIGH;
    sConfig.OCFastMode   = TIM_OCFAST_DISABLE;
    sConfig.OCIdleState  = TIM_OCIDLESTATE_RESET;
    sConfig.OCNIdleState = TIM_OCNIDLESTATE_RESET;
    if (HAL_TIM_PWM_ConfigChannel(tim->priv, &sConfig, channel) != HAL_OK)
    {
      /* Configuration Error */
      Error_Handler();
    }

    /*##-3- Start PWM signal generation in DMA mode ############################*/
    if (HAL_TIM_PWM_Start_IT(tim->priv, channel) != HAL_OK)
    {
      /* Starting Error */
      Error_Handler();
    }
}

int32_t hal_pwm_start(pwm_dev_t *tim)
{
    int32_t ret = -1;

    if(tim != NULL && tim->port < PORT_PWM_INVALID){
        if(!stm32_pwm[tim->port].inited){
            ret = -1;
        }else{
            PWM_MAPPING* timerIns = get_pwm_list_logical(tim->port);
            if (timerIns != NULL)
            {
                if (timerIns->physical_port == TIM4){
                    ret = timer4_start(tim,timerIns->channel);
                }
            }
        }
    }
    return ret;
}

static int32_t timer4_change(pwm_dev_t *tim, uint32_t channel,pwm_config_t para)
{
    TIM_HandleTypeDef * const psttimhandle = &stm32_pwm[tim->port].hal_timer_handle;
    TIM_OC_InitTypeDef sConfig;

    tim->config.freq = para.freq;
    tim->config.duty_cycle = para.duty_cycle;

    psttimhandle->Init.Period = (HAL_APB1_DIV_FREQ / tim->config.freq) - 1;
    TIM_Base_SetConfig(psttimhandle->Instance, &(psttimhandle->Init));

    sConfig.OCMode       = TIM_OCMODE_PWM1;
    sConfig.OCPolarity   = TIM_OCPOLARITY_HIGH;
    sConfig.Pulse        = (tim->config.duty_cycle) * (HAL_APB1_DIV_FREQ / tim->config.freq);
    sConfig.OCNPolarity  = TIM_OCNPOLARITY_HIGH;
    sConfig.OCFastMode   = TIM_OCFAST_DISABLE;
    sConfig.OCIdleState  = TIM_OCIDLESTATE_RESET;
    sConfig.OCNIdleState = TIM_OCNIDLESTATE_RESET;
    if (HAL_TIM_PWM_ConfigChannel(tim->priv, &sConfig, channel) != HAL_OK)
    {
      /* Configuration Error */
      Error_Handler();
    }

}


int32_t hal_pwm_para_chg(pwm_dev_t *pwm, pwm_config_t para)
{
    int32_t ret = -1;

    if(pwm != NULL && pwm->port < PORT_PWM_INVALID){
        if(!stm32_pwm[pwm->port].inited){
            ret = -1;
        }else{
            PWM_MAPPING* timerIns = get_pwm_list_logical(pwm->port);
            if (timerIns != NULL)
            {
                if (timerIns->physical_port == TIM4){
                    ret = timer4_change(pwm,timerIns->channel,para);
                }
            }
        }
    }
    return ret;
}

static int32_t timer4_stop(pwm_dev_t *tim, uint32_t channel)
{
    return HAL_TIM_PWM_Stop_IT((TIM_HandleTypeDef *)tim->priv, channel);
}

int32_t hal_pwm_stop(pwm_dev_t *tim)
{
    int32_t ret = -1;
    PWM_MAPPING* timerIns = get_pwm_list_logical(tim->port);
    if (timerIns != NULL)
    {
        if (timerIns->physical_port == TIM4){
            ret = timer4_stop(tim,timerIns->channel);
        }
    }
    return ret;
}

int32_t hal_pwm_finalize(pwm_dev_t *tim)
{
    int32_t ret = -1;

    if (tim != NULL)
    {
        ret = HAL_TIM_PWM_DeInit((TIM_HandleTypeDef *)tim->priv);
    }

    return ret;
}

/*
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM3) {
        pFun_timer3(arg_timer3);
    }
}*/

void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim)
{

}
#endif
