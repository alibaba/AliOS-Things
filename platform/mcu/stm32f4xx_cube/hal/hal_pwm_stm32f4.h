/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef __HAL_PWM_STM32F4_H
#define __HAL_PWM_STM32F4_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f4xx_hal.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "aos/kernel.h"
#include "board.h"

#define PORT_PWM0 0
#define PORT_PWM1 1

struct stm32_pwm_out_s
{
    uint32_t pol;    /* Polarity. Default: positive */
    uint32_t alt;    /* alternative function */
    uint32_t pin;    /* Output pin configuration */
};

struct stm32_pwmchan_s
{
    uint32_t                 channel;   /* Timer output channel: {1,..4} */
    uint32_t                 mode;      /* PWM channel mode (see stm32_pwm_chanmode_e) */
    struct stm32_pwm_out_s   out1;        /* PWM output configuration */
    //struct stm32_pwm_break_s brk;         /* PWM break configuration */
};

typedef struct{
    PORT_PWM_TYPE           logical_func;
    void                   *physical_port;
    struct stm32_pwmchan_s *channels;
    uint8_t                 channel_cnt;
}PWM_MAPPING;

extern PWM_MAPPING PWM_MAPPING_TABLE[PORT_PWM_SIZE];

#ifdef __cplusplus
}
#endif

#endif /* __HAL_PWM_STM32F4_H */
