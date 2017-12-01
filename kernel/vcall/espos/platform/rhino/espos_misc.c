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


