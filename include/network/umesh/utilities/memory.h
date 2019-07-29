/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef UR_MEMORY_H
#define UR_MEMORY_H

#include <stdint.h>

#include "umesh_types.h"

enum {
    MEM_BUF_SIZE = 16 * 1024,  // 16kB
};

void umesh_mem_init(void);
void umesh_mem_deinit(void);
void *ur_mem_alloc(uint16_t size);
void ur_mem_free(void *mem, uint16_t size);
const ur_mem_stats_t *ur_mem_get_stats(void);

#endif  /* UR_MEMORY_H */
