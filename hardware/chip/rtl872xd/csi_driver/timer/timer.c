/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include "objects.h"
#include <drv/timer.h>

#define GTIMER_MAX  4

static void csi_gtimer_timeout_handler(uint32_t data)
{
	csi_timer_t *timer = (csi_timer_t *)data;
	uint32_t tid = timer->dev.idx;

	RTIM_INTClear(TIMx[tid]);

	if (timer->callback != NULL) {
        timer->callback(timer,timer->arg);
	}
}

csi_error_t csi_timer_init(csi_timer_t *timer, uint32_t idx)
{
    if(!timer)
        return CSI_ERROR;

    if(idx > GTIMER_MAX)
        return CSI_ERROR;
    
    timer->priv = (RTIM_TimeBaseInitTypeDef *)malloc(sizeof(RTIM_TimeBaseInitTypeDef));
    RTIM_TimeBaseInitTypeDef *TIM_InitStruct = (RTIM_TimeBaseInitTypeDef *)timer->priv;

    timer->dev.idx = idx;

    RTIM_TimeBaseStructInit(TIM_InitStruct);
	TIM_InitStruct->TIM_Idx = (uint8_t)idx;

	TIM_InitStruct->TIM_UpdateEvent = ENABLE; /* UEV enable */
	TIM_InitStruct->TIM_UpdateSource = TIM_UpdateSource_Overflow;
	TIM_InitStruct->TIM_ARRProtection = ENABLE;
	
	RTIM_TimeBaseInit(TIMx[idx], TIM_InitStruct, TIMx_irq[idx], (IRQ_FUN)csi_gtimer_timeout_handler, (u32)timer);
    return CSI_OK;
}

void csi_timer_uninit(csi_timer_t *timer)
{
    uint32_t tid = timer->dev.idx;
    RTIM_DeInit(TIMx[tid]);

    if(timer && timer->priv) {
        free(timer->priv);
        timer->priv = NULL;
    }
}

csi_error_t csi_timer_start(csi_timer_t *timer, uint32_t timeout_us)
{
    uint32_t tid = timer->dev.idx;
    uint32_t temp = (uint32_t)((float)timeout_us  / 1000000 * 32768);
    
    RTIM_ChangePeriodImmediate(TIMx[tid], temp);
    RTIM_INTConfig(TIMx[tid], TIM_IT_Update, ENABLE);
	RTIM_Cmd(TIMx[tid], ENABLE);
    return CSI_OK;
}

void csi_timer_stop(csi_timer_t *timer)
{
    uint32_t tid = timer->dev.idx;

    RTIM_Cmd(TIMx[tid], DISABLE); 
}

uint32_t csi_timer_get_remaining_value(csi_timer_t *timer)
{
    uint32_t tid = timer->dev.idx;
    uint32_t tick;
    uint32_t load;
    uint32_t time_us;
    
    RTIM_TypeDef* TIM = TIMx[tid];
    tick = RTIM_GetCount(TIM);
    load = (uint32_t)((float)TIM->ARR * 1000000 / 32768);
    time_us = load - (uint32_t)((float)tick * 1000000 / 32768);
    return time_us;
}

uint32_t csi_timer_get_load_value(csi_timer_t *timer)
{
    uint32_t tid = timer->dev.idx;
    uint32_t time_us;
    
    RTIM_TypeDef* TIM = TIMx[tid];
    time_us = (uint32_t)((float)TIM->ARR * 1000000 / 32768);
    return time_us;
}

bool csi_timer_is_running(csi_timer_t *timer)
{
    uint32_t tid = timer->dev.idx;
    uint32_t time_us;
    
    RTIM_TypeDef* TIM = TIMx[tid];
    if(TIM->EN & TIM_CR_CNT_RUN)
        return TRUE;
    
    return FALSE;
}

csi_error_t csi_timer_attach_callback(csi_timer_t *timer, void *callback, void *arg)
{ 
    timer->callback = callback;
    timer->arg = arg;
    return CSI_OK;
}

void csi_timer_detach_callback(csi_timer_t *timer)
{
    timer->callback = NULL;
}