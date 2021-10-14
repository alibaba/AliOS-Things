/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */


#include <stdio.h>
#include <string.h>

#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>
#include <fcntl.h>
#include <lwip/netdb.h>

#include <aos/debug.h>
#include "linkkit/wrappers/wrappers_tcp.h"

#define PLATFORM_LOG(format, ...)                                         \
    do {                                                                  \
        HAL_Printf("SOCK %u %s() | " format "\n", __LINE__, __FUNCTION__, \
                   ##__VA_ARGS__);                                        \
    } while (0);

/*
 * mqtt does not call these functions,
 * but has been used in IoT libraries,
 * so implement empty functions and optimize footprint
 */
uintptr_t HAL_TCP_Establish(const char *host, uint16_t port)
{
    aos_assert(0);
    return 0;
}
int HAL_TCP_Destroy(uintptr_t fd)
{
    aos_assert(0);
    return 0;
}
int32_t HAL_TCP_Write(uintptr_t fd, const char *buf, uint32_t len,
                      uint32_t timeout_ms)
{
    aos_assert(0);
    return 0;
}
int32_t HAL_TCP_Read(uintptr_t fd, char *buf, uint32_t len, uint32_t timeout_ms)
{
    aos_assert(0);
    return 0;
}

