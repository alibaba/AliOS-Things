/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef NETWORK_NETWORK_H
#define NETWORK_NETWORK_H

#ifdef __cplusplus
extern "C" {
#endif

#if defined(__ICCARM__) || defined(__CC_ARM)
#include <sys/errno.h>
#else
#include <errno.h>
#endif

#include <fcntl.h>

/* network */
#if defined(CONFIG_AOS_LWIP) || defined(CONFIG_VENDOR_LWIP)
#include <lwip/def.h>
#include <lwip/netdb.h>
#include <lwip/sockets.h>
#elif defined(CONFIG_NO_TCPIP) && defined(WITH_SAL)
#include <sal/sal_arch.h>
#include <sal/sal_def.h>
#include <sal/sal_ipaddr.h>
#include <sal/sal_sockets.h>
#elif !defined(CONFIG_NO_TCPIP)
#if defined(_WIN32)
#include "unistd.h"
#define read(fd, buf, len)         recv(fd, (char*)buf, (int)len, 0)
#define write(fd, buf, len)        send(fd, (char*)buf, (int)len, 0)
#define close(fd)                  closesocket(fd)
#define inet_aton(strptr, addrptr) inet_pton(AF_INET, strptr, addrptr)
#define bzero(b, len)              memset((b), '\0', (len))
#else
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/eventfd.h>
#endif
#endif /* CONFIG_AOS_TCPIP */

#ifdef __cplusplus
}
#endif

#endif /* NETWORK_NETWORK_H */

