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
#ifndef __SPEECH_MEMORY_H__
#define __SPEECH_MEMORY_H__

#ifdef __cplusplus
extern "C" {
#endif

#ifndef VQE_SIMULATE
#include "med_memory.h"

#define speech_heap_init(a, b)          med_heap_init(a, b)
#define speech_heap_add_block(a, b)     med_heap_add_block(a, b)
#define speech_malloc(a)                med_malloc(a)
#define speech_realloc(a, b)            med_realloc(a, b)
#define speech_calloc(a, b)             med_calloc(a, b)
#define speech_free(a)                  med_free(a)
#define speech_memory_info(a, b, c)     med_memory_info(a, b, c)

#define speech_heap_set_cp(a)           med_heap_set_cp(a)
#else
#include <stddef.h>
#include <stdlib.h>

void speech_heap_init(void *begin_addr, size_t size);
void *speech_malloc(size_t size);
void speech_free(void *p);
void *speech_calloc(size_t nmemb, size_t size);
void *speech_realloc(void *ptr, size_t size);
void speech_memory_info(size_t *total,
                    size_t *used,
                    size_t *max_used);

#define speech_heap_set_cp(a)
#endif

#ifdef __cplusplus
}
#endif

#endif