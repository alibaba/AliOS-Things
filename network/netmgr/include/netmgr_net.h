/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef NETMGR_NET_H
#define NETMGR_NET_H

#if defined(__cplusplus)
extern "C"
{
#endif

int netmgr_net_init(void);
void netmgr_net_deinit(void);
int netmgr_net_start(bool autoconfig);

#if defined(__cplusplus)
}
#endif

#endif /* NETMGR_NET_H */
