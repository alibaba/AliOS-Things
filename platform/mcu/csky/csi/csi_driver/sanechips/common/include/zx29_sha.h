/**
 * File: zx2975100_sha.h
 * Brief: Implementation of Sanechips sha
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

#ifndef ZX297100_SHA_H
#define ZX297100_SHA_H

/*******************************************************************************
*                               Include header files                            *
*******************************************************************************/
#include "drv_sha.h"
#include "soc.h"

/*******************************************************************************
*                                Macro definitions                              *
*******************************************************************************/
#define SHA_DRV_INITED      0x0
#define SHA_DRV_STARTED     0x1
#define SHA_DRV_CONTINUE    0x2

/*******************************************************************************
*                               Type definitions                                *
*******************************************************************************/
struct sha_reg_t {
    uint32_t sha_version;
    uint32_t sha_ctrl;
    uint32_t sha_start;
    uint32_t reserved0;
    uint32_t sha_data;
    uint32_t sha_dma_ctrl;
    uint32_t sha_intr_en;
    uint32_t sha_intr_st_clr;
    uint32_t reserved1[2];
    uint32_t sha_len[2];
    uint32_t reserved2[11];
    uint32_t sha_result[5];
};

struct sha_drv_ctx_t {
    uint32_t state;
    uint8_t data[128];
    uint32_t data_size;
    uint32_t total_size;
    volatile struct sha_reg_t sha_regs;
};

/****** Enum of SHA Edian ******/
enum sha_endian_t {
    SHA_ENDIAN_LITTLE,
    SHA_ENDIAN_BIG
};

enum sha_mode_t {
    SHA_MODE_INVALID = 0,
    SHA_MODE_SHA1,
    SHA_MODE_SHA256,
    SHA_MODE_SHA224,
    SHA_MODE_SHA512,
    SHA_MODE_SHA384,
    SHA_MODE_MD5_SW,
    SHA_MODE_MAX,
};

struct sha_context_t {
    void *context;
    enum sha_mode_t mode;
};

#endif //#ifndef ZX297100_SHA_H