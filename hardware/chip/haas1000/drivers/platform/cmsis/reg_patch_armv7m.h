/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#ifndef __REG_PATCH_ARMV7M_H__
#define __REG_PATCH_ARMV7M_H__

#ifdef __ARM_ARCH_7EM__

#include "plat_types.h"
#include "cmsis.h"

struct PATCH_ARMV7M_T {
    __IO uint32_t FP_CTRL;
    __IO uint32_t FP_REMAP;
    __IO uint32_t FP_COMP[512];
};

#define NUM_CODE_6_4_SHIFT                      12
#define NUM_CODE_6_4_MASK                       (0x7 << NUM_CODE_6_4_SHIFT)
#define NUM_CODE_6_4(n)                         BITFIELD_VAL(NUM_CODE_6_4, n)
#define NUM_LIT_SHIFT                           8
#define NUM_LIT_MASK                            (0xF << NUM_LIT_SHIFT)
#define NUM_LIT(n)                              BITFIELD_VAL(NUM_LIT, n)
#define NUM_CODE_3_0_SHIFT                      4
#define NUM_CODE_3_0_MASK                       (0xF << NUM_CODE_3_0_SHIFT)
#define NUM_CODE_3_0(n)                         BITFIELD_VAL(NUM_CODE_3_0, n)
#define FP_CTRL_KEY                             (1 << 1)
#define FP_CTRL_ENABLE                          (1 << 0)

#define REMAP_RMPSPT                            (1 << 29)
#define REMAP_ADDR_28_5_SHIFT                   5
#define REMAP_ADDR_28_5_MASK                    (0xFFFFFF << REMAP_ADDR_28_5_SHIFT)
#define REMAP_ADDR_28_5(n)                      BITFIELD_VAL(REMAP_ADDR_28_5, n)
#define REMAP_ADDR_SHIFT                        0
#define REMAP_ADDR_MASK                         (0x1FFFFFFF << REMAP_ADDR_SHIFT)
#define REMAP_ADDR(n)                           BITFIELD_VAL(REMAP_ADDR, n)

#define COMP_REPLACE_SHIFT                      30
#define COMP_REPLACE_MASK                       (0x3 << COMP_REPLACE_SHIFT)
#define COMP_REPLACE(n)                         BITFIELD_VAL(COMP_REPLACE, n)
#define COMP_ADDR_28_2_SHIFT                    2
#define COMP_ADDR_28_2_MASK                     (0x7FFFFFF << COMP_ADDR_28_2_SHIFT)
#define COMP_ADDR_28_2(n)                       BITFIELD_VAL(COMP_ADDR_28_2, n)
#define COMP_ADDR_SHIFT                         0
#define COMP_ADDR_MASK                          (0x1FFFFFFF << COMP_ADDR_SHIFT)
#define COMP_ADDR(n)                            BITFIELD_VAL(COMP_ADDR, n)
#define COMP_ENABLE                             (1 << 0)

#endif

#endif
