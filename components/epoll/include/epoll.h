/**
 * @file epoll.h
 * epoll.h API header file.
 *
 * @version   V1.0
 * @date      2020-04-26
 * @copyright Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#ifndef ___EPOLL_H__
#define ___EPOLL_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define EPOLL_CTL_ADD   1
#define EPOLL_CTL_DEL   2
#define EPOLL_CTL_MOD   3

#ifndef O_CLOEXEC
#define O_CLOEXEC 02000000
#endif

#define EPOLL_CLOEXEC   O_CLOEXEC

enum EPOLL_EVENTS
  {
    EPOLLIN       = 0x001,
    EPOLLPRI      = 0x002,
    EPOLLOUT      = 0x004,
    EPOLLRDNORM   = 0x040,
    EPOLLRDBAND   = 0x080,
    EPOLLWRNORM   = 0x100,
    EPOLLWRBAND   = 0x200,
    EPOLLMSG      = 0x400,
    EPOLLERR      = 0x008,
    EPOLLHUP      = 0x010,
    EPOLLRDHUP    = 0x2000,
    EPOLLEXCLUSIVE = 1u << 28,
    EPOLLWAKEUP   = 1u << 29,
    EPOLLONESHOT  = 1u << 30,
    EPOLLET       = 1u << 31
  };

typedef union epoll_data {
    void *ptr;
    int fd;
    uint32_t u32;
    uint64_t u64;
} epoll_data_t;

typedef struct epoll_event {
    uint32_t events;
    epoll_data_t data;
} epoll_event_t;

int aos_epoll_create(int size);
int aos_epoll_create1(int flag);
int aos_epoll_ctl(int epid, int op, int sockid, struct epoll_event *event);
int aos_epoll_wait(int epid, struct epoll_event *events, int maxevents, int timeout);

#if (!CONFIG_WITH_MUSL)
static inline int epoll_create(int size)
{
    return aos_epoll_create(size);
}

static inline int epoll_create1(int flag)
{
    return aos_epoll_create1(flag);
}

static inline int epoll_ctl(int epid, int op, int sockid, struct epoll_event *event)
{
    return aos_epoll_ctl(epid, op, sockid, event);
}

static inline int epoll_wait(int epid, struct epoll_event *events, int maxevents, int timeout)
{
    return aos_epoll_wait(epid, events, maxevents, timeout);
}
#endif

#ifdef __cplusplus
}
#endif

#endif
