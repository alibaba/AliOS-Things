/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef RHINO_MM_DEBUG_H
#define RHINO_MM_DEBUG_H

#include "k_mm.h"

#if (RHINO_CONFIG_MM_DEBUG > 0)

#ifdef __cplusplus
extern "C" {
#endif

#define AOS_UNSIGNED_INT_MSB (1u << (sizeof(unsigned int) * 8 - 1))

void krhino_owner_attach(void *addr, size_t allocator);

/* set owner to return address */
#define krhino_owner_return_addr(addr) \
    krhino_owner_attach(addr, (size_t)RHINO_GET_RA())

uint32_t dumpsys_mm_info_func(uint32_t len);
#ifdef __cplusplus
}
#endif

#endif /* RHINO_CONFIG_MM_DEBUG */

#endif /* RHINO_MM_DEBUG_H */

