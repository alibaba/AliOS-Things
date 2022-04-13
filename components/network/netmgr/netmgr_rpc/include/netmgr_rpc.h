/*
 * Copyright (C) 2020 Alibaba Group Holding Limited
 */

#ifndef _NETMGR_RPC_H_
#define _NETMGR_RPC_H_

#if defined(__cplusplus)
extern "C"
{
#endif

#include <stdint.h>
#include <aos/kernel.h>

int netmgr_rpc_service_start();
int netmgr_rpc_service_stop();

#ifdef __cplusplus
}
#endif

#endif
