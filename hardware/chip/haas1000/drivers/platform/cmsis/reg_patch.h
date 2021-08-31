/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#ifndef __REG_PATCH_H__
#define __REG_PATCH_H__

#include "plat_types.h"
#include "plat_addr_map.h"

#ifdef PATCH_CTRL_BASE

struct PATCH_CTRL_T {
    __IO uint32_t CTRL[PATCH_ENTRY_NUM];
};

struct PATCH_DATA_T {
    __IO uint32_t DATA[PATCH_ENTRY_NUM];
};

#define PATCH_CTRL_ENTRY_EN                     (1 << 0)
#define PATCH_CTRL_ADDR_17_2_SHIFT              2
#define PATCH_CTRL_ADDR_17_2_MASK               (0xFFFF << PATCH_CTRL_ADDR_17_2_SHIFT)
#define PATCH_CTRL_17_2_ADDR(n)                 BITFIELD_VAL(PATCH_CTRL_30_2_ADDR, n)
// For entry 0 only
#define PATCH_CTRL_GLOBAL_EN                    (1 << 31)

#define PATCH_CTRL_ADDR_MASK                    PATCH_CTRL_ADDR_17_2_MASK
#define PATCH_CTRL_ADDR(n)                      ((n) & PATCH_CTRL_ADDR_17_2_MASK)

#endif

#endif
