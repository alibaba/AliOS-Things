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
#ifndef __PATCH_H__
#define __PATCH_H__

#include "plat_types.h"

#ifdef __cplusplus
extern "C" {
#endif

enum PATCH_TYPE_T {
    PATCH_TYPE_CODE,
    PATCH_TYPE_DATA,
    PATCH_TYPE_FUNC,

    PATCH_TYPE_QTY
};

typedef int PATCH_ID;

int patch_open(uint32_t remap_addr);

PATCH_ID patch_enable(enum PATCH_TYPE_T type, uint32_t addr, uint32_t data);

int patch_disable(PATCH_ID patch_id);

void patch_close(void);

#ifdef __cplusplus
}
#endif

#endif
