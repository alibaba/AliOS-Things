/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#include <stdint.h>
#include <string.h>
#include "k_api.h"

void os_unlock_sys_spin(void)
{
    krhino_spin_unlock(&g_sys_lock);
}

