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
 * @file     ck_spu_usart.h
 * @brief    header file for spu usart driver
 * @version  V1.0
 * @date     02. June 2017
 ******************************************************************************/
#ifndef __DW_USART_H
#define __DW_USART_H

//======================================
//filter
//======================================
#define USART_FILTER_COE 2

//======================================
//PROG_RAM offset
//======================================
#define USART_OFFSET_CTRL 0x0
#define USART_OFFSET_TRANS_LEN 0x4
#define USART_OFFSET_INT 0x8
#define USART_OFFSET_FIFO_HEAD_W0 0xC
#define USART_OFFSET_FIFO_HEAD_W1 0x10
#define USART_OFFSET_FIFO_MEM 0x14

//======================================
//"INTERRUPT" value
//======================================
#define USART_INT_RX_DONE 0x1
#define USART_INT_RX_FULL 0x2
#define USART_INT_TX_DONE 0x4
#define USART_INT_TX_EMPTY 0x8
#define USART_INT_PARITY_ERR 0x10
#define USART_INT_STOP_ERR 0x20

//======================================
//configuration of TX/RX fifo
//======================================
#define USART_FIFO_DEPTH 7
#define USART_FIFO_EMPTY_TL 1
#define USART_FIFO_FULL_TL 6

#define USART_PROG_BYTES (USART_OFFSET_FIFO_MEM+(USART_FIFO_DEPTH+1)*4)

#endif /* __DW_USART_H */
