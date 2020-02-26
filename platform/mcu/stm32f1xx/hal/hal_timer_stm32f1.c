/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "stm32f1xx_hal.h"

#ifdef HAL_TIM_MODULE_ENABLED

#include <k_api.h>
#include <stdio.h>
#include <stdlib.h>

#include "board.h"
#include "aos/hal/timer.h"
#include "hal_timer_stm32f1.h"
#include "stm32f1xx_hal_adc.h"

/* Init and deInit function for adc1 */
static int32_t timer_normal_init(timer_dev_t *tim);

typedef struct {
    TIM_HandleTypeDef  htim;
    uint8_t            inited;
    hal_timer_cb_t     timer_handler;
    void              *timer_arg;
} stm32_timer_t;

/* will be initialized in hal_can_init */
static stm32_timer_t stm32_timer[PORT_TIMER_SIZE] = {0};

static TIMER_MAPPING *get_mapping_entry_by_port(uint8_t port)
{
    int i;

    for (i = 0; i < PORT_TIMER_SIZE; i++) {
        if (port == TIMER_MAPPING_TABLE[i].port) {
            return &TIMER_MAPPING_TABLE[i];
        }
    }

    return NULL;
}

static TIMER_MAPPING *get_mapping_entry_by_logic_port(uint32_t hal_port)
{
    int i;

    for (i = 0; i < PORT_TIMER_SIZE; i++) {
        if (hal_port == TIMER_MAPPING_TABLE[i].hal_timer) {
            return &TIMER_MAPPING_TABLE[i];
        }
    }

    return NULL;
}

static void _timer_cb(void *timer, void *arg)
{
    timer_dev_t *tim = arg;
    tim->config.cb(tim->config.arg);
}

int32_t hal_timer_init(timer_dev_t *tim)
{
    int32_t ret = -1;

    if (tim == NULL) {
       return -1;
    }


    if (stm32_timer[tim->port].inited == 1){
        return 0;
    }

    TIMER_MAPPING* timerIns = get_mapping_entry_by_port(tim->port);
    if (timerIns != NULL)
    {
        memset(&stm32_timer[tim->port],0,sizeof(stm32_timer_t));
        TIM_HandleTypeDef * const psttimhandle = &stm32_timer[tim->port].htim;
        psttimhandle->Instance = (TIM_TypeDef*)timerIns->hal_timer;
        tim->priv = psttimhandle;
        ret = timer_normal_init(tim);
    }

    return ret;
}

/**
 * @note: APB1 CLK: TIM2, TIM3, TIM4, TIM5, TIM6, TIM7, TIM12, TIM13, TIM14
 *        APB2 CLK: TIM1, TIM8, TIM9, TIM10, TIM11
 */
static int32_t timer_normal_init(timer_dev_t *tim)
{

    RCC_ClkInitTypeDef  clkconfig;
    uint32_t            uwTimclock;
    uint32_t            Prescaler;
    uint32_t            pFLatency;
    uint32_t            hal_timer;
    TIMER_MAPPING      *timerIns;
    uint32_t            uwPrescalerValue = 0;
    uint64_t            udwcycle;

    int32_t ret = -1;

    TIM_HandleTypeDef * const psttimhandle = &stm32_timer[tim->port].htim;
    stm32_timer[tim->port].timer_handler = tim->config.cb;
    stm32_timer[tim->port].timer_arg = tim->config.arg;

    /* Get clock configuration */
    HAL_RCC_GetClockConfig(&clkconfig, &pFLatency);

    timerIns = get_mapping_entry_by_port(tim->port);

    printf("hal_timer_init: port %d, hal_timer 0x%x\r\n", tim->port, timerIns->hal_timer);

    switch (timerIns->hal_timer) {
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

    /* Compute the prescaler value to have TIMx counter clock equal to 10000 Hz */
    uwPrescalerValue = (uint32_t)(uwTimclock / 64000) - 1;
    udwcycle = (uint64_t)tim->config.period;
    psttimhandle->Init.Period            = ((udwcycle * 64000UL ) / 1000000) - 1;
    psttimhandle->Init.Prescaler         = uwPrescalerValue;
    psttimhandle->Init.ClockDivision     = 0;
    psttimhandle->Init.CounterMode       = TIM_COUNTERMODE_UP;
    psttimhandle->Init.RepetitionCounter = 0;

    if (tim->config.reload_mode == TIMER_RELOAD_MANU) {
        ret = HAL_TIM_OnePulse_Init(psttimhandle, TIM_OPMODE_SINGLE);
    } else {
        ret = HAL_TIM_Base_Init(psttimhandle);
    }

    stm32_timer[tim->port].inited = 1;

    return ret;
}

int32_t hal_timer_start(timer_dev_t *tim)
{
    TIMER_MAPPING* timerIns;

    int32_t ret = -1;

    if(tim != NULL && tim->port < PORT_TIMER_SIZE) {
        if(!stm32_timer[tim->port].inited){
            ret = -1;
        } else {
            timerIns = get_mapping_entry_by_port(tim->port);
            if (timerIns != NULL)
            {
                ret = HAL_TIM_Base_Start_IT(&stm32_timer[tim->port].htim);
            }
        }
    }

    return ret;
}

void hal_timer_stop(timer_dev_t *tim)
{
    TIMER_MAPPING* timerIns;

    timerIns = get_mapping_entry_by_port(tim->port);
    if (timerIns != NULL)
    {
        HAL_TIM_Base_Stop_IT(&stm32_timer[tim->port].htim);
    }
}

int32_t hal_timer_finalize(timer_dev_t *tim)
{
    int32_t ret = -1;

    if (tim != NULL)
    {
        ret = HAL_TIM_Base_DeInit(&stm32_timer[tim->port].htim);
    }

    return ret;
}

static void IRQ_Common(uint32_t hal_timer)
{
    TIMER_MAPPING *entry;
    uint8_t        port;

    krhino_intrpt_enter();

    entry = get_mapping_entry_by_logic_port(hal_timer);
    if (entry) {
        port = entry->port;
        if((PORT_TIMER_INVALID != port) && (stm32_timer[port].inited)){
            HAL_TIM_IRQHandler(&stm32_timer[port].htim);
        }
    }

    krhino_intrpt_exit();
}

void TIM1_UP_IRQHandler(void)
{
    IRQ_Common(HAL_TIMER_1);
}

void TIM2_IRQHandler(void)
{
    IRQ_Common(HAL_TIMER_2);
}

void TIM3_IRQHandler(void)
{
    IRQ_Common(HAL_TIMER_3);
}

void TIM4_IRQHandler(void)
{
    IRQ_Common(HAL_TIMER_4);
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    stm32_timer_t *timer = (stm32_timer_t *)htim;

   if (timer && timer->timer_handler) {
        timer->timer_handler(timer->timer_arg);
    }
}

#endif
