/******************************************************************************
 *
 *  Copyright (C) 2014 Google, Inc.
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at:
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 ******************************************************************************/

#ifndef _ALLOCATOR_H_
#define _ALLOCATOR_H_

#include <stddef.h>
#include <stdlib.h>
#include <bt_target.h>

char *osi_strdup(const char *str);

void *osi_malloc_func(size_t size);
void *osi_calloc_func(size_t size);
void osi_free_func(void *ptr);

#if HEAP_MEMORY_DEBUG

void osi_mem_dbg_init(void);
void osi_mem_dbg_record(void *p, int size, const char *func, int line);
void osi_mem_dbg_clean(void *p, const char *func, int line);
void osi_mem_dbg_show(void);
uint32_t osi_mem_dbg_get_max_size(void);
uint32_t osi_mem_dbg_get_current_size(void);
void osi_men_dbg_set_section_start(uint8_t index);
void osi_men_dbg_set_section_end(uint8_t index);
uint32_t osi_mem_dbg_get_max_size_section(uint8_t index);

#define osi_free(ptr)                                   \
do {                                                    \
    void *tmp_point = (void *)(ptr);                    \
    osi_mem_dbg_clean(tmp_point, __func__, __LINE__);   \
    free(tmp_point);                                    \
} while (0)

#else

#define osi_malloc(size)                  malloc((size))
#define osi_calloc(size)                  calloc(1, (size))
#define osi_free(p)                       free((p))

#endif /* HEAP_MEMORY_DEBUG */

#define FREE_AND_RESET(a)   \
do {                        \
    if (a) {                \
        osi_free(a);        \
        a = NULL;           \
    }                       \
}while (0)


#endif /* _ALLOCATOR_H_ */
