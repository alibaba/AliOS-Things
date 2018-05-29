/*
 * ESPRSSIF MIT License
 *
 * Copyright (c) 2015 <ESPRESSIF SYSTEMS (SHANGHAI) PTE LTD>
 *
 * Permission is hereby granted for use on ESPRESSIF SYSTEMS ESP8266 only, in which case,
 * it is free of charge, to any person obtaining a copy of this software and associated
 * documentation files (the "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished
 * to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#ifndef __HW_TIMER_H__
#define __HW_TIMER_H__

#ifdef __cplusplus
extern "C" {
#endif

/** \defgroup HW_Timer_APIs Hardware timer APIs
  * @brief Hardware timer APIs
  *
  * @attention Hardware timer can not interrupt other ISRs.
  *
  */

/** @addtogroup HW_Timer_APIs
  * @{
  */


/**
  * @brief   Initialize the hardware ISR timer.
  *
  * @param   null
  *
  * @return  null
  */
void hw_timer_init(void);

/**
  * @brief   Set a trigger timer delay to enable this timer.
  *
  * @param   uint32 val : Timing 
  *    - In autoload mode, range : 50 ~ 0x7fffff
  *    - In non-autoload mode, range : 10 ~ 0x7fffff
  *
  * @param   uint8 req : 0, not autoload; 1, autoload mode.
  * 
  * @return  null
  */
void hw_timer_arm(uint32 val, bool req);

/**
  * @brief   disable this timer.
  *
  * @param   null
  * 
  * @return  null
  */
void hw_timer_disarm(void);

/**
  * @brief   Set timer callback function.
  *
  *         For enabled timer, timer callback has to be set.
  *
  * @param   uint32 val : Timing 
  *    - In autoload mode, range : 50 ~ 0x7fffff
  *    - In non-autoload mode, range : 10 ~ 0x7fffff
  *
  * @return  null
  */
void hw_timer_set_func(void (* user_hw_timer_cb_set)(void));

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif
