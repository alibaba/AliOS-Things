/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "k_api.h"
#include "espos_err.h"

int espos_err_map (kstat_t err_code)
{
    if (err_code == RHINO_SUCCESS) {
        return 0;
    }

    return -1;
}

size_t espos_get_free_heap_size(void)
{
    extern k_mm_head *g_kmm_head;

    return g_kmm_head->free_size;
}
