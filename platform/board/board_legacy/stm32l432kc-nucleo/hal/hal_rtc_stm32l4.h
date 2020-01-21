/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef __HAL_ADC_STM32L4_H
#define __HAL_ADC_STM32L4_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "stm32l4xx_hal.h"  
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "aos/kernel.h"

#define RTC_ASYNCH_PREDIV    0x7F
#define RTC_SYNCH_PREDIV     0xF9 

#define RTC_INIT_HOUR_FORMAT    RTC_HOURFORMAT_24
#define RTC_INIT_ASYNC_PREDIV   RTC_ASYNCH_PREDIV
#define RTC_INIT_SYNC_PREDIV    RTC_SYNCH_PREDIV
#define RTC_INIT_OUTPUT         RTC_OUTPUT_DISABLE
#define RTC_INIT_OUTPUT_PRI     RTC_OUTPUT_POLARITY_HIGH
#define RTC_INIT_OUTPUT_TYPE    RTC_OUTPUT_TYPE_OPENDRAIN

#ifdef __cplusplus
}
#endif

#endif /* __HAL_UART_STM32L4_H */
