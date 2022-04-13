/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "ql_data_call.h"

#include "quec_include.h"
#include "wrappers_defs.h"

#include "amp_errno.h"

struct hostent *HAL_Httpc_Socket_GetHostByName(const char *name)
{
    return gethostbyname_with_pcid(name, 1);
}

int32_t HAL_Httpc_Socket_Connect(uintptr_t fd, const struct sockaddr *name, socklen_t namelen)
{
    int32_t ret = -1;
    static struct in_addr ip4_addr = {0};
    struct ql_data_call_info info = {0};
    struct sockaddr_in	ip4_local_addr = {0};
    
    ql_get_data_call_info(1, 0, &info);
    ip4_addr = info.v4.addr.ip;

    ip4_local_addr.sin_family = AF_INET;
	ip4_local_addr.sin_port = htons(quec_soc_generate_port());
	ip4_local_addr.sin_addr = ip4_addr;

    ret = bind(fd, (struct sockaddr *)&ip4_local_addr, sizeof(ip4_local_addr));
    if(ret < 0)
    {
        printf("%s\r\n", "[TCP] *** bind fail ***\r\n");
    }

    return connect(fd, name, namelen);
}