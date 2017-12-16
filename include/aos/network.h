/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef AOS_NETWORK_H
#define AOS_NETWORK_H

#ifdef __cplusplus
extern "C" {
#endif

#include <errno.h>
#include <fcntl.h>

/* network */
#ifdef WITH_LWIP
#include <lwip/def.h>
#include <lwip/netdb.h>
#include <lwip/sockets.h>
#elif defined(WITH_SAL)
#include <sal_arch.h>
#include <sal_def.h>
#include <sal_ipaddr.h>
#include <sal_sockets.h>
#elif !defined(CONFIG_NO_TCPIP)
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/eventfd.h>
#endif

#ifdef __cplusplus
}
#endif

#endif /* AOS_NETWORK_H */

