/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#ifndef __PATCH_H__
#define __PATCH_H__

#include "plat_types.h"

#ifdef __cplusplus
extern "C" {
#endif

enum PATCH_TYPE_T {
    PATCH_TYPE_CODE,
    PATCH_TYPE_DATA,
    PATCH_TYPE_FUNC,

    PATCH_TYPE_QTY
};

typedef int PATCH_ID;

int patch_open(uint32_t remap_addr);

PATCH_ID patch_enable(enum PATCH_TYPE_T type, uint32_t addr, uint32_t data);

int patch_disable(PATCH_ID patch_id);

void patch_close(void);

#ifdef __cplusplus
}
#endif

#endif
