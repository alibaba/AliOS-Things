/** mbed Microcontroller Library
  ******************************************************************************
  * @file    timer_api.h
  * @author 
  * @version V1.0.0
  * @date    2016-08-01
  * @brief   This file provides mbed timer API
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2006-2013 ARM Limited
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
  ****************************************************************************** 
  */
#ifndef MBED_EXT_TIMER_API_EXT_H
#define MBED_EXT_TIMER_API_EXT_H

#include "device.h"

typedef void (*gtimer_irq_handler)(uint32_t id);

typedef struct gtimer_s gtimer_t;
enum { 
    TIMER0 = 0,    /*!< GTimer 0, 32k timer, share with us_tick(wait_ms()) functions. This timer is reserved and users are not recommended to use it */
    TIMER1 = 1,    /*!< GTimer 1, 32k timer, share with APP_TIM_ID */
    TIMER2 = 2,    /*!< GTimer 2, 32k timer, users can use it */
    TIMER3 = 3,    /*!< GTimer 3, 32k timer, users can use it */

    GTIMER_MAX = 4    
};

/** @defgroup AmebaZ_Mbed_API 
  * @{
  */
  
/** @defgroup MBED_TIMER
  * @brief MBED_TIMER driver modules
  * @{
  */
 
/** @defgroup MBED_TIMER_Standard_Functions MBED_TIMER standard Functions
  * @{
  */
void gtimer_init (gtimer_t *obj, uint32_t tid);
void gtimer_deinit (gtimer_t *obj);
uint32_t gtimer_read_tick (gtimer_t *obj);
uint64_t gtimer_read_us (gtimer_t *obj);
void gtimer_reload (gtimer_t *obj, uint32_t duration_us);
void gtimer_start (gtimer_t *obj);
void gtimer_start_one_shout (gtimer_t *obj, uint32_t duration_us, void* handler, uint32_t hid);
void gtimer_start_periodical (gtimer_t *obj, uint32_t duration_us, void* handler, uint32_t hid);
void gtimer_stop (gtimer_t *obj);
/** @} */

/** @} */
/** @} */

#endif
