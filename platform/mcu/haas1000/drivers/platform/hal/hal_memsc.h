/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#ifndef __HAL_MEMSC_H__
#define __HAL_MEMSC_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "stdint.h"
#include "stdbool.h"

enum HAL_MEMSC_ID_T {
    HAL_MEMSC_ID_0,
    HAL_MEMSC_ID_1,
    HAL_MEMSC_ID_2,
    HAL_MEMSC_ID_3,

    HAL_MEMSC_ID_QTY
};

int hal_memsc_lock(enum HAL_MEMSC_ID_T id);

void hal_memsc_unlock(enum HAL_MEMSC_ID_T id);

bool hal_memsc_avail(enum HAL_MEMSC_ID_T id);

#ifdef __cplusplus
}
#endif

#endif

