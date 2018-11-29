/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef RHINO_MM_DEBUG_H
#define RHINO_MM_DEBUG_H

#if (RHINO_CONFIG_MM_DEBUG > 0)

#ifdef __cplusplus
extern "C" {
#endif

#define AOS_MM_SCAN_REGION_MAX 10
typedef struct {
    void *start;
    void *end;
} mm_scan_region_t;

#if (RHINO_CONFIG_GCC_RETADDR > 0u)
#include <k_mm.h>
#define AOS_UNSIGNED_INT_MSB (1u << (sizeof(unsigned int) * 8 - 1))
void krhino_owner_attach(k_mm_head *mmhead, void *addr, size_t allocator);
#endif

uint32_t krhino_mm_leak_region_init(void *start, void *end);

uint32_t dumpsys_mm_info_func(uint32_t len);

uint32_t dump_mmleak(void);

#ifdef __cplusplus
}
#endif

#endif /* RHINO_CONFIG_MM_DEBUG */

#endif /* YSH_H */


