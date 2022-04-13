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
#if !defined(CONFIG_NO_TCPIP)
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/eventfd.h>
#endif /* WITH_LWIP */

#ifdef __cplusplus
}
#endif

#endif /* NETWORK_NETWORK_H */

