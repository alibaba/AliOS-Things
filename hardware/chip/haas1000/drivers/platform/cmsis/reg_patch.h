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
#ifndef __REG_PATCH_H__
#define __REG_PATCH_H__

#include "plat_types.h"
#include "plat_addr_map.h"

#ifdef PATCH_CTRL_BASE

struct PATCH_CTRL_T {
    __IO uint32_t CTRL[PATCH_ENTRY_NUM];
};

struct PATCH_DATA_T {
    __IO uint32_t DATA[PATCH_ENTRY_NUM];
};

#define PATCH_CTRL_ENTRY_EN                     (1 << 0)
#define PATCH_CTRL_ADDR_17_2_SHIFT              2
#define PATCH_CTRL_ADDR_17_2_MASK               (0xFFFF << PATCH_CTRL_ADDR_17_2_SHIFT)
#define PATCH_CTRL_17_2_ADDR(n)                 BITFIELD_VAL(PATCH_CTRL_30_2_ADDR, n)
// For entry 0 only
#define PATCH_CTRL_GLOBAL_EN                    (1 << 31)

#define PATCH_CTRL_ADDR_MASK                    PATCH_CTRL_ADDR_17_2_MASK
#define PATCH_CTRL_ADDR(n)                      ((n) & PATCH_CTRL_ADDR_17_2_MASK)

#endif

#endif
