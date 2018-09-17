/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef __UNISTD_H__
#define __UNISTD_H__

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _WIN32
#include <unistd.h>
#else
#include <winsock2.h>
#include <ws2tcpip.h>
#include <Windows.h>
#include <stdint.h>

typedef long int ssize_t;
#endif

#ifdef __cplusplus
}
#endif

#endif /* __UNISTD_H__ */

