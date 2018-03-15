/*
 * Copyright (C) 2017 C-SKY Microsystems Co., Ltd. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/******************************************************************************
 * @file     zx29_gpio.h
 * @brief    CSI Source File for usart Driver
 * @version  V1.0
 * @date     02. June 2017
 ******************************************************************************/
#ifndef _ZX29_GPIO_H__
#define _ZX29_GPIO_H__

/*******************************************************************************
 *                           Include header files                              *
 ******************************************************************************/
#include <pin_name.h>
#include <stdint.h>
#include <drv_gpio.h>

/*******************************************************************************
 *                             Macro definitions                               *
 ******************************************************************************/
//#define   GPIO_AO_NR              (SI_GPIO_8)
#define GPIO_DIR_REG(gpio)      ((gpio < GPIO_AO_NR)? (GPIO_AO_BASEADDR + ((gpio >> 4) * 16) * 4):(GPIO_PD_BASEADDR + ((gpio >> 4) * 16) * 4))
#define GPIO_IN_REG(gpio)       ((gpio < GPIO_AO_NR)? (GPIO_AO_BASEADDR + ((gpio >> 4) * 16 + 5) * 4):(GPIO_PD_BASEADDR + ((gpio >> 4) * 16 + 5) * 4))
#define GPIO_OUT_HIGH_REG(gpio) ((gpio < GPIO_AO_NR)? (GPIO_AO_BASEADDR + ((gpio >> 4) * 16 + 6) * 4):(GPIO_PD_BASEADDR + ((gpio >> 4) * 16 + 6) * 4))
#define GPIO_OUT_LOW_REG(gpio)  ((gpio < GPIO_AO_NR)? (GPIO_AO_BASEADDR + ((gpio >> 4) * 16 + 7) * 4):(GPIO_PD_BASEADDR + ((gpio >> 4) * 16 + 7) * 4))

#endif
