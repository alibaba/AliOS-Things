/*
 * ESPRESSIF MIT License
 *
 * Copyright (c) 2019 <ESPRESSIF SYSTEMS (SHANGHAI) CO., LTD>
 *
 * Permission is hereby granted for use on all ESPRESSIF SYSTEMS products, in which case,
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

#ifndef _GPIO_ISR_H_
#define _GPIO_ISR_H_

#include  "esp_err.h"
#include "driver/gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief  Initialize the gpio's interrupt service routines.
 *
 * @param  gpio_num                The number of gpio to be initialized
 * @param  type                    The type of interrupts
 * @param  gpio_isr_handle_func    Interrupt handler
 * @param  isr_param               The parameters of interrupt handler
 * 
 * @return
 */
esp_err_t gpio_isr_init(int gpio_num, gpio_int_type_t type, gpio_isr_t gpio_isr_handle_func, void *isr_param);

/**
 * @brief   Deinitialize the gpio isr
 *
 * @param   gpio_num The number of gpio to be deinitialized
 *
 * @return  
 *      - ESP_OK on success
 *      - ESP_FAIL on failed 
 */
esp_err_t gpio_isr_deinit(int gpio_num);

#ifdef __cplusplus
}
#endif

#endif
