/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef AOS_TYPES_H
#define AOS_TYPES_H

#include <stdint.h>
#include <stddef.h>
#include <unistd.h>
#include <fcntl.h>

#if defined(WITH_LWIP) || defined(CONFIG_NO_TCPIP)
#define POLLIN  0x1
#define POLLOUT 0x2
#define POLLERR 0x4
struct pollfd {
    int fd;
    short events;
    short revents;
};
#else
#include <poll.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#endif

/**
 * define the EXPORT_SYMBOL_K macro, use in kernel
 *
 */
#define EXPORT_SYMBOL_K(condition, fun, fun_str)

/**
 * define the EXPORT_SYMBOL_F macro, use in framework
 *
 */
#define EXPORT_SYMBOL_F(condition, fun, fun_str)


#endif /* AOS_TYPES_H */

