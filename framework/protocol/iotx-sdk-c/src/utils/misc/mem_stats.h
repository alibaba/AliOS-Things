/*
 * Copyright (c) 2014-2016 Alibaba Group. All rights reserved.
 * License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */


#ifndef __MEM_STATS_H__
#define __MEM_STATS_H__

#include "lite-utils_internal.h"

#if defined(_PLATFORM_IS_LINUX_) && WITH_MEM_STATS
    #include <execinfo.h>
#endif

typedef struct {
    void               *buf;
    int                 buflen;
    char               *func;
    int                 line;
#if defined(_PLATFORM_IS_LINUX_)
    char              **bt_symbols;
    int                 bt_level;
#endif
    list_head_t         list;

#if WITH_MEM_STATS_PER_MODULE
    void               *mem_table;
#endif
} OS_malloc_record;

#endif  /* __MEM_STATS_H__ */

