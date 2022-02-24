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

#ifndef __KWS_EXT_HEAP_H__
#define __KWS_EXT_HEAP_H__

#ifdef __cplusplus
extern "C" {
#endif
#include <stdint.h>

int32_t ext_heap_init(void);
int32_t ext_heap_deinit(void);
uint32_t ext_heap_get_used_buff_size();
uint32_t ext_heap_get_free_buff_size();
void *ext_get_buff(uint32_t size);
void *ext_alloc(int size);
void *ext_malloc(int size);
void *ext_calloc(int nitems, int size);
void ext_free(void *mem_ptr);

#ifdef __cplusplus
}
#endif

#endif