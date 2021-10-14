/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "aos/kernel.h"
#include "linkkit/wrappers/wrappers_defs.h"

#define PLATFORM_LOG_D(format, ...)                                 \
    do {                                                            \
        printf("D: %d %s() | " format "\n", __LINE__, __func__, \
               ##__VA_ARGS__);                                      \
    } while (0);

#define PLATFORM_LOG_E(format, ...)                                 \
    do {                                                            \
        printf("E: %d %s() | " format "\n", __LINE__, __func__, \
               ##__VA_ARGS__);                                      \
    } while (0);

uintptr_t HAL_TCP_Establish(_IN_ const char *host, _IN_ uint16_t port)
{
    return 0;
}
int32_t HAL_TCP_Destroy(uintptr_t fd)
{
    return 0;
}
int32_t HAL_TCP_Write(uintptr_t fd, const char *buf, uint32_t len,
                      uint32_t timeout_ms)
{
    return 0;
}
int32_t HAL_TCP_Read(uintptr_t fd, char *buf, uint32_t len, uint32_t timeout_ms)
{
    return 0;
}
