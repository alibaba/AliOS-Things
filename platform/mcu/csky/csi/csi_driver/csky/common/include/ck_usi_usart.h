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
 * @file     ck_usi_usart.h
 * @brief    header file for usart driver
 * @version  V1.0
 * @date     02. June 2017
 ******************************************************************************/
#ifndef _CK_USI_USART_H_
#define _CK_USI_USART_H_

#include "ck_usi.h"

#define CTRL_DBIT_BITS  0x3c
#define CTRL_DBIT_5     0x0
#define CTRL_DBIT_6     0x1
#define CTRL_DBIT_7     0x2
#define CTRL_DBIT_8     0x3

#define CTRL_STOP_BITS 0x33
#define CTRL_STOP_1     (0x0 << 2)
#define CTRL_STOP_1_5   (0x1 << 2)
#define CTRL_STOP_2     (0x2 << 2)

#define CTRL_PARITY_BITS    0x0f
#define CTRL_PARITY_ENABLE  (0x1 << 4)
#define CTRL_PARITY_ODD     (0x0 << 5)
#define CTRL_PARITY_EVEN    (0x1 << 5)

#define CK_UART_INT_ENABLE_DEFAUL (USI_INT_UART_PERR | USI_INT_UART_STOP | USI_INT_RX_WERR | \
                                   USI_INT_RX_RERR | USI_INT_RX_FULL \
                                   | USI_INT_TX_WERR | USI_INT_TX_RERR)
#endif /* _CK_USI_USART_H_ */
