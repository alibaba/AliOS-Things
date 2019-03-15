/**
 * File: zx2975100_rsa.h
 * Brief: Implementation of Sanechips rsa
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

#ifndef ZX297100_RSA_H
#define ZX297100_RSA_H

/*******************************************************************************
*                               Include header files                            *
*******************************************************************************/

#include "soc.h"

/*******************************************************************************
*                                Macro definitions                              *
*******************************************************************************/
#define RSA_M_RAM       (ZX29_RSA_BASE+0x100)
#define RSA_E_RAM       (ZX29_RSA_BASE+0x200)
#define RSA_N_RAM       (ZX29_RSA_BASE+0x300)
#define RSA_CALC_RAM    (ZX29_RSA_BASE+0x400)
#define RSA_RESULT_RAM  (ZX29_RSA_BASE+0x500)

#define M_DATA_WORDS    (64)
#define E_DATA_WORDS    (64)
#define N_DATA_WORDS    (64)
#define C_DATA_WORDS    (64)
#define R_DATA_WORDS    (64)

#define  KEY_BITS     1024
//#define KEY_BITS        2048
#define KEY_BYTES       (KEY_BITS >> 3)

#define RSA_PUBLIC      0
#define RSA_PRIVATE     1

/*******************************************************************************
*                               Type definitions                                *
*******************************************************************************/
enum rsa_calmode {
    RSA_INIT_CAL                = 2,
    RSA_MOD_MULTI_WITH_INIT_CAL = 10,
    RSA_MOD_MULTI_NO_INIT_CAL   = 8,
    RSA_MOD_EXPON_WITH_INIT_CAL = 6,
    RSA_MOD_EXPON_NO_INIT_CAL   = 4,
    RSA_BIG_NUM_CAL             = 9,
};

struct rsa_reg_t {
    uint32_t version;//0x00
    uint32_t module_enable;
    uint32_t modular_len;
    uint32_t exponentiation_len;
    uint32_t interrupt_status;//0x10
    uint32_t interrupt_enable;
    uint32_t interrupt_mask;
    uint32_t nzero;
    uint32_t nzero_inv;//0x20
    uint32_t calculate_mode;//0x24
    uint32_t Rev1[54];
    uint32_t m_data_ram[64];//0x100
    uint32_t e_data_ram[64];//0x200
    uint32_t n_data_ram[64];//0x300
    uint32_t c_data_ram[64];//0x400
    uint32_t result_data_ram[64];//0x500
};

#endif//#ifndef ZX297100_RSA_H
