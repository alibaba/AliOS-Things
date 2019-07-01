/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef _WRAPPERS_TCP_H_
#define _WRAPPERS_TCP_H_

#include "infra_types.h"
#include "infra_defs.h"

uintptr_t HAL_TCP_Establish(const char *host, uint16_t port);

int HAL_TCP_Destroy(uintptr_t fd);

int32_t HAL_TCP_Write(uintptr_t fd, const char *buf, uint32_t len, uint32_t timeout_ms);

int32_t HAL_TCP_Read(uintptr_t fd, char *buf, uint32_t len, uint32_t timeout_ms);

#endif

