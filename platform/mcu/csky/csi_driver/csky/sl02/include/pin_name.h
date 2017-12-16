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
 * @file     pn_name.h
 * @brief    header file for the pin_name
 * @version  V1.0
 * @date     02. June 2017
 ******************************************************************************/
#ifndef _PINNAMES_H
#define _PINNAMES_H


#ifdef __cplusplus
extern "C" {
#endif


typedef enum {
    PA0_GPIO_PORTA=0,
    PA1_GPIO_PORTA,
    PA2_GPIO_PORTA,
    PA3_GPIO_PORTA,
    PA4_GPIO_PORTA,
    PA5_GPIO_PORTA,
    PA6_GPIO_PORTA,
    PA7_GPIO_PORTA,
    PAD_UART0_SIN,
    PAD_UART0_SOUT    
}
pin_name_t;

typedef enum {
    PORTA = 0,
    PORTB = 1,
    PORTC = 2,
    PORTD = 3,
    PORTE = 4,
    PORTF = 5,
    PORTG = 6,
    PORTH = 7
} port_name_t;

#ifdef __cplusplus
}
#endif

#endif
