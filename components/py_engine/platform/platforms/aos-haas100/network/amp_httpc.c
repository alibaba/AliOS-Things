/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include "amp_network.h"
#include "amp_platform.h"

struct hostent *HAL_Httpc_Socket_GetHostByName(const char *name)
{
    return gethostbyname(name);
}

int32_t HAL_Httpc_Socket_Connect(uintptr_t fd, const struct sockaddr *name, socklen_t namelen)
{
    return connect(fd, name, namelen);
}