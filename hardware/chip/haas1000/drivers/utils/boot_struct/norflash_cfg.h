/*
 * Copyright (c) 2021 Bestechnic (Shanghai) Co., Ltd. All rights reserved.
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
 */
#ifndef __NORFLASH_CFG_H__
#define __NORFLASH_CFG_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "plat_types.h"

#define STATUS_S10_LB_BIT_SHIFT             2
#define STATUS_S10_LB_BIT_MASK              (1 << STATUS_S10_LB_BIT_SHIFT)

#define STATUS_S11_LB1_BIT_SHIFT            3
#define STATUS_S11_LB1_BIT_MASK             (1 << STATUS_S11_LB1_BIT_SHIFT)

enum SEC_REG_BASE_T {
    SEC_REG_BASE_0X1000,
    SEC_REG_BASE_0X0000,
};

enum SEC_REG_SIZE_T {
    SEC_REG_SIZE_1024,
    SEC_REG_SIZE_512,
    SEC_REG_SIZE_256,
};

enum SEC_REG_OFFSET_T {
    SEC_REG_OFFSET_0X1000,
    SEC_REG_OFFSET_0X0100,
};

enum SEC_REG_CNT_T {
    SEC_REG_CNT_3,
    SEC_REG_CNT_4,
};

enum SEC_REG_PP_T {
    SEC_REG_PP_256,
    SEC_REG_PP_1024,
};

enum SEC_REG_LB_T {
    SEC_REG_LB_S11_S13,
    SEC_REG_LB_S10,
};

struct norflash_cfg_struct_t {
    uint8_t neg_phase:1;
    uint8_t pos_neg:1;
    uint8_t cmdquad:1;
    uint8_t samdly:3;

    uint8_t div; /* least 2 */

    uint8_t dualmode:1;
    uint8_t holdpin:1;
    uint8_t wprpin:1;
    uint8_t quadmode:1;
    uint8_t mod_clk:4;

    uint8_t spiruen:3;
    uint8_t spirden:3;

    uint8_t dualiocmd;
    uint8_t rdcmd;
    uint8_t frdcmd;
    uint8_t qrdcmd; /* quad io cmd */
};

#ifdef __cplusplus
}
#endif

#endif

