/**
 * File: zx2975100_aes.h
 * Brief: Implementation of Sanechips aes
 *
 * Copyright (C) 2017 Sanechips Technology Co., Ltd.
 * Author:
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */


#ifndef ZX297100_AES_H
#define ZX297100_AES_H

/*******************************************************************************
*                               Include header files                            *
*******************************************************************************/
#include "drv_aes.h"
#include "soc.h"

/*******************************************************************************
*                                Macro definitions                              *
*******************************************************************************/
#define AES_IT_DATAINT  0x4
#define AES_IT_KEYINT   0x2
#define AES_IT_BUSY     0x1
#define AES_IT_ALL      0x7

/*******************************************************************************
*                               Type definitions                                *
*******************************************************************************/
struct aes_reg_t {
    uint32_t version;               /*0x00*/
    uint32_t mode_ctrl;             /*0x04   Ä£Ê½¼Ä´æÆ÷£¬[0]:0,¼ÓÃÜ 1,ÒëÂë  [2:1]aes mode 00£ºecb mode  01£ºctr mode  10£ºcbc mode  1£ºaes-xcbc-mac-96
                                        [3]:0,Ð¡¶Ë 1,´ó¶Ë  [4]byte_mode_rx 0,rx byte from high to low  1,rx byte from low to high
                                        [5]byte_mode_tx 0,tx byte from high to low  1,tx byte from low to high
                                        [7:6]key_num 00: 128 bits  01: 192 bits  10: 256bits  11: reserved*/
    uint32_t key[8];                /*0x08*/
    uint32_t counter[4];            /*0x28*/
    uint32_t counter_step;          /*0x38*/
    uint32_t fifo_dma_req_en;       /*0x3c*/
    uint32_t fifo_int_en;           /*0x40*/
    uint32_t fifo_int_req;          /*0x44*/
    uint32_t fifo_data_num;         /*0x48*/
    uint32_t fifo_data_input;       /*0x4c*/
    uint32_t fifo_data_output;      /*0x50*/
    uint32_t crypt_en;              /*0x54*/
};

/****** Enum of AES Edian ******/

enum aes_endian_t {
    AES_ENDIAN_LITTLE1,
    AES_ENDIAN_BIG1
};

/***** Enum of AES Opcode ******/
enum aes_opcode_t {
    AES_OPCODE_ENCRYPT,
    AES_OPCODE_DECRYPT
};

/***** Enum of AES Key Length *****/
enum aes_keylength_t {
    AES_KEYLEN_128,
    AES_KEYLEN_192,
    AES_KEYLEN_256
};

/***** Enum of AES Mode *******/
enum aes_mode_t {
    AES_MODE_ECB1,
    AES_MODE_CTR1,
    AES_MODE_CBC1,
    AES_MODE_XCBC_MAC_96
};

/***** Enum of AES Fifo *******/
enum aes_fifo_t {
    AES_RX_FIFO,
    AES_TX_FIFO,
};

#endif //ZX297100_AES_H

