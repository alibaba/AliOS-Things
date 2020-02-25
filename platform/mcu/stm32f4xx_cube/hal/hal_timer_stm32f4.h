/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef __HAL_TIMER_STM32F4_H
#define __HAL_TIMER_STM32F4_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "stm32f4xx_hal.h"

#ifdef HAL_TIM_MODULE_ENABLED

#include <stdint.h>

#define HAL_TIMER_1   ((uint32_t)TIM1) /**< HAL_TIMER_1  <--> TIM1  */
#define HAL_TIMER_2   ((uint32_t)TIM2) /**< HAL_TIMER_2  <--> TIM2  */
#define HAL_TIMER_3   ((uint32_t)TIM3) /**< HAL_TIMER_3  <--> TIM3  */
#define HAL_TIMER_4   ((uint32_t)TIM4) /**< HAL_TIMER_4  <--> TIM4  */
#ifdef TIM5
#define HAL_TIMER_5   ((uint32_t)TIM5) /**< HAL_TIMER_5  <--> TIM5  */
#endif
#ifdef TIM6
#define HAL_TIMER_6   ((uint32_t)TIM6) /**< HAL_TIMER_6  <--> TIM6  */
#endif
#ifdef TIM7
#define HAL_TIMER_7   ((uint32_t)TIM7) /**< HAL_TIMER_7  <--> TIM7  */
#endif
#ifdef TIM8
#define HAL_TIMER_8   ((uint32_t)TIM8) /**< HAL_TIMER_8  <--> TIM8  */
#endif
#ifdef TIM9
#define HAL_TIMER_9   ((uint32_t)TIM9) /**< HAL_TIMER_9  <--> TIM9  */
#endif
#ifdef TIM10
#define HAL_TIMER_10   ((uint32_t)TIM10) /**< HAL_TIMER_10  <--> TIM10  */
#endif
#ifdef TIM11
#define HAL_TIMER_11   ((uint32_t)TIM11) /**< HAL_TIMER_11  <--> TIM11  */
#endif
#ifdef TIM12
#define HAL_TIMER_12   ((uint32_t)TIM12) /**< HAL_TIMER_12  <--> TIM12  */
#endif
#ifdef TIM13
#define HAL_TIMER_13   ((uint32_t)TIM13) /**< HAL_TIMER_13  <--> TIM13  */
#endif
#ifdef TIM14
#define HAL_TIMER_14   ((uint32_t)TIM14) /**< HAL_TIMER_14  <--> TIM14  */
#endif

typedef struct{
    uint8_t   port;
    uint32_t  hal_timer;
} TIMER_MAPPING;

extern TIMER_MAPPING TIMER_MAPPING_TABLE[];

#endif /* HAL_TIM_MODULE_ENABLED */

#ifdef __cplusplus
}
#endif

#endif /* __HAL_TIMER_STM32F4_H */
