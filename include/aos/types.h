/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef AOS_TYPES_H
#define AOS_TYPES_H

#include <fcntl.h>
#include <stddef.h>
#include <stdint.h>
#include <unistd.h>

#if defined(WITH_LWIP) || defined(CONFIG_NO_TCPIP) || defined(WITH_SAL)

#define POLLIN  0x1
#define POLLOUT 0x2
#define POLLERR 0x4

struct pollfd {
    int   fd;
    short events;
    short revents;
};

#else

#include <poll.h>
#include <sys/stat.h>
#include <sys/types.h>

#endif /* defined(WITH_LWIP) || defined(CONFIG_NO_TCPIP) || defined(WITH_SAL) */

#ifndef AOS_EXPORTX
#define AOS_EXPORT(ret, fun, ...)
#endif

#ifndef AOS_COMPONENT_INIT
#define AOS_COMPONENT_INIT(fun, ...)
#endif

#ifndef AOS_TESTCASE
#define AOS_TESTCASE(fun, ...)
#endif

#endif /* AOS_TYPES_H */

