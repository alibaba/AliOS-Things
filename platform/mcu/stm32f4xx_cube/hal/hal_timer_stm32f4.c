/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include <stdio.h>
#include <stdlib.h>

#include "aos/hal/timer.h"
#include "stm32f4xx_hal.h"
#ifdef HAL_TIM_MODULE_ENABLED
#include "hal_timer_stm32f4.h"
#include "stm32f4xx_hal_adc.h"

#define us2tick(us) \
    ((us * RHINO_CONFIG_TICKS_PER_SECOND + 999999) / 1000000)

/* Init and deInit function for adc1 */
static int32_t timer_normal_init(timer_dev_t *tim);
static int32_t timer0_init(timer_dev_t *tim);

/* function used to transform hal para to stm32l4 para */
/* int32_t timer_reload_mode_transform(uint8_t reload_mode_hal, uint8_t *reload_mode_stm32l4); */

typedef struct {
    uint8_t                inited;
    TIM_HandleTypeDef      hal_timer_handle;
    /* handle for time */
    hal_timer_cb_t pFun_timer;
    void * arg_timer;
}stm32_timer_t;

/* will be initialized in hal_can_init */
static stm32_timer_t stm32_timer[PORT_TIMER_SIZE] = {0};

/* Get TIMER Instanse & attribute from Logical Port */
TIMER_MAPPING* get_timer_list_logical(const PORT_TIMER_TYPE port)
{
    int8_t i = 0;
    TIMER_MAPPING* rc = NULL;
    for(i=0; i<PORT_TIMER_SIZE; i++)
    {
        if(TIMER_MAPPING_TABLE[i].logical_func == port)
        {
            rc = &TIMER_MAPPING_TABLE[i];
            break;
        }
    }
    return rc;
}

static PORT_TIMER_TYPE get_timer_logical_port(void* physical_port)
{
    int8_t i = 0;
    PORT_TIMER_TYPE type = PORT_TIMER_INVALID;
    if(NULL == physical_port){
        return PORT_TIMER_INVALID;
    }
    for(i=0; i<PORT_TIMER_SIZE; i++)
    {
        if(TIMER_MAPPING_TABLE[i].physical_port == physical_port)
        {
            type = TIMER_MAPPING_TABLE[i].logical_func;
            break;
        }
    }
    return type;
}

#ifdef TIM3
void TIM3_IRQHandler(void)
{
    PORT_TIMER_TYPE type = PORT_TIMER_INVALID;
    krhino_intrpt_enter();
    type = get_timer_logical_port(TIM3);
    if((type != PORT_TIMER_INVALID) && (stm32_timer[type].inited)){
        HAL_TIM_IRQHandler(&stm32_timer[type].hal_timer_handle);
    }
    krhino_intrpt_exit();
}
#endif

#ifdef TIM5
void TIM5_IRQHandler(void)
{
    PORT_TIMER_TYPE type = PORT_TIMER_INVALID;
    krhino_intrpt_enter();
    type = get_timer_logical_port(TIM5);
    if((type != PORT_TIMER_INVALID) && (stm32_timer[type].inited)){
        HAL_TIM_IRQHandler(&stm32_timer[type].hal_timer_handle);
    }
    krhino_intrpt_exit();
}
#endif

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

    TIMER_MAPPING* timerIns = get_timer_list_logical(tim->port);
    if (timerIns != NULL)
    {
        memset(&stm32_timer[tim->port],0,sizeof(stm32_timer_t));
        TIM_HandleTypeDef * const psttimhandle = &stm32_timer[tim->port].hal_timer_handle;
        psttimhandle->Instance = timerIns->physical_port;
        tim->priv = psttimhandle;
        if (timerIns->physical_port == PORT_TIMER0) {
            ret = timer0_init(tim);
        }else if (timerIns->physical_port == TIM3 || timerIns->physical_port == TIM5){
            ret = timer_normal_init(tim);
        }
    }

    return ret;

}

static int32_t timer0_init(timer_dev_t *tim)
{
    if (tim->config.reload_mode == TIMER_RELOAD_AUTO) {
        krhino_timer_dyn_create((ktimer_t **)&tim->priv, "hwtmr", _timer_cb,
                                us2tick(tim->config.period), us2tick(tim->config.period), tim, 0);
    }
    else {
        krhino_timer_dyn_create((ktimer_t **)&tim->priv, "hwtmr", _timer_cb,
                                us2tick(tim->config.period), 0, tim, 0);
    }

    stm32_timer[tim->port].inited = 1;
}

/*timer2 ~ timer7 use APB1*/
static int32_t timer_normal_init(timer_dev_t *tim)
{
    int32_t ret = -1;
    RCC_ClkInitTypeDef    clkconfig;
    uint32_t              uwTimclock, uwAPB1Prescaler = 0U;
    uint32_t              pFLatency;
    uint32_t uwPrescalerValue = 0;
    uint64_t udwcycle;

    TIM_HandleTypeDef * const psttimhandle = &stm32_timer[tim->port].hal_timer_handle;
    stm32_timer[tim->port].pFun_timer = tim->config.cb;
    stm32_timer[tim->port].arg_timer = tim->config.arg;

    /* Get clock configuration */
    HAL_RCC_GetClockConfig(&clkconfig, &pFLatency);
    /* Get APB1 prescaler */
    uwAPB1Prescaler = clkconfig.APB1CLKDivider;
    /* Compute TIM clock */
    if (uwAPB1Prescaler == RCC_HCLK_DIV1)
    {
        uwTimclock = HAL_RCC_GetPCLK1Freq();
    }
    else
    {
        uwTimclock = 2*HAL_RCC_GetPCLK1Freq();
    }

    /* Compute the prescaler value to have TIMx counter clock equal to 10000 Hz */
    uwPrescalerValue = (uint32_t)(uwTimclock / 64000) - 1;
    udwcycle = (uint64_t)tim->config.period;
    psttimhandle->Init.Period            = ((udwcycle * 64000UL ) / 1000000) - 1;
    psttimhandle->Init.Prescaler         = uwPrescalerValue;
    psttimhandle->Init.ClockDivision     = 0;
    psttimhandle->Init.CounterMode       = TIM_COUNTERMODE_UP;
    psttimhandle->Init.RepetitionCounter = 0;

    ret = HAL_TIM_Base_Init(psttimhandle);

    if (ret == 0 && (tim->config.reload_mode == TIMER_RELOAD_MANU)) {
        ret = HAL_TIM_OnePulse_Init(psttimhandle, TIM_OPMODE_SINGLE);
    }
    stm32_timer[tim->port].inited = 1;

    return ret;
}

static int32_t timer0_start(timer_dev_t *tim)
{
    return krhino_timer_start(tim->priv);
}

static int32_t timer_normal_start(timer_dev_t *tim)
{
    return HAL_TIM_Base_Start_IT((TIM_HandleTypeDef *)tim->priv);
}

int32_t hal_timer_start(timer_dev_t *tim)
{
    int32_t ret = -1;

    if(tim!=NULL && tim->port<PORT_TIMER_SIZE){
        if(!stm32_timer[tim->port].inited){
            ret = -1;
        }else{
            TIMER_MAPPING* timerIns = get_timer_list_logical(tim->port);
            if (timerIns != NULL)
            {
                if (timerIns->physical_port == PORT_TIMER0) {
                    ret = timer0_start(tim);
                }else if (timerIns->physical_port == TIM3 || timerIns->physical_port == TIM5){
                    ret = timer_normal_start(tim);
                }
            }
        }
    }
    return ret;
}

static void timer0_stop(timer_dev_t *tim)
{
    krhino_timer_stop(tim->priv);
    krhino_timer_dyn_del(tim->priv);
    tim->priv = NULL;
}

static void timer_normal_stop(timer_dev_t *tim)
{
    HAL_TIM_Base_Stop_IT((TIM_HandleTypeDef *)tim->priv);
}

void hal_timer_stop(timer_dev_t *tim)
{
    TIMER_MAPPING* timerIns = get_timer_list_logical(tim->port);
    if (timerIns != NULL)
    {
        if (timerIns->physical_port == PORT_TIMER0) {
            timer0_stop(tim);
        }else if (timerIns->physical_port == TIM3 || timerIns->physical_port == TIM5){
            timer_normal_stop(tim);
        }
    }
}

int32_t hal_timer_finalize(timer_dev_t *tim)
{
    int32_t ret = -1;

    if (tim != NULL)
    {
        ret = HAL_TIM_Base_DeInit((TIM_HandleTypeDef *)tim->priv);
    }

    return ret;
}

/*
int32_t timer_reload_mode_transform(uint8_t reload_mode_hal,
        uint8_t *reload_mode_stm32l4)
{
    uint32_t reload_mode = 0;
    int32_t	ret = 0;

    if(reload_mode_hal == TIMER_RELOAD_AUTO)
    {
        reload_mode = TIM_AUTORELOAD_PRELOAD_ENABLE;
    }
    else if(reload_mode_hal == TIMER_RELOAD_MANU)
    {
        reload_mode = TIM_AUTORELOAD_PRELOAD_DISABLE;
    }
    else
    {
        ret = -1;
    }

    if(ret == 0)
    {
        *reload_mode_stm32l4 = reload_mode;
    }

    return ret;
}
*/
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    PORT_TIMER_TYPE type = PORT_TIMER_INVALID;
    type = get_timer_logical_port(htim->Instance);
    if((type != PORT_TIMER_INVALID) && (stm32_timer[type].inited)){
        hal_timer_cb_t pFun_timer = stm32_timer[type].pFun_timer;
        if(pFun_timer){
            pFun_timer(stm32_timer[type].arg_timer);
        }
    }
}
#endif
