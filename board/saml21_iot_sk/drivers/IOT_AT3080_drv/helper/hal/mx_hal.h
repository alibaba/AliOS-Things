/**
 ******************************************************************************
 * @file    mx_hal.c
 * @author  William Xu
 * @version V1.0.0
 * @date    18-Apr-2018
 * @brief   MCU peripheral driver PI header file
 ******************************************************************************
 *
 * Copyright (c) 2009-2018 MXCHIP Co.,Ltd.
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
 *
 ******************************************************************************
 */

#ifndef _MX_HAL_H_
#define _MX_HAL_H_

#include "mx_common.h"
#include "mx_debug.h"

#ifdef __cplusplus
extern "C"
{
#endif

/** \addtogroup emhost */
/** @{*/

/**
 *  Tick used for delay and timeout mechanism
 */
void mx_hal_ms_ticker_init(void);
uint32_t mx_hal_ms_ticker_read(void);
void mx_hal_delay_ms(volatile uint32_t delay);

/**
 *  Serial port driver used for AT parser 
 */
void mx_hal_serial_init(int timeout);
void mx_hal_serial_set_timeout(int timeout);
int mx_hal_serial_putc(char c);
int mx_hal_serial_getc(void);
bool mx_hal_serial_readable(void);
void mx_hal_serial_flush(void);

/**
 *  For debug info 
 */
void mx_hal_stdio_init(void);

/**
 *  For I2C sensor drivers
 */
void* mx_hal_i2c_init(void *config);
int32_t mx_hal_i2c_cmd_write(void *instance, uint16_t slave_addr, uint8_t reg, uint8_t *buffer, uint8_t length);
int32_t mx_hal_i2c_cmd_read(void *instance, uint16_t slave_addr, uint8_t reg, uint8_t *buffer, uint8_t length);


/** @}*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif