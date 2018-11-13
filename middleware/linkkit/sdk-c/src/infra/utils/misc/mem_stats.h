/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */




#ifndef __MEM_STATS_H__
#define __MEM_STATS_H__

#include "iotx_utils_internal.h"

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

