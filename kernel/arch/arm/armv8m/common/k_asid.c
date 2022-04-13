/*
 * Copyright (C) 2020 Alibaba Group Holding Limited
 */

#include "k_api.h"

static int g_asid = 0;
void k_asid_set(uint32_t asid)
{
    /* Address Space Identifier (ASID) */
    if (asid < MK_CONFIG_PROCESS_MAX){
        g_asid = asid;
    }
    return;
}

uint32_t k_asid_get()
{
    return g_asid;
}
