/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <string.h>
#include <stdio.h>
#include <sys/time.h>
#include "k_config.h"
#include "aos/kernel.h"

#include "aos/hal/uart.h"

volatile int errno = 0;
//extern uart_dev_t uart_0;

//#if defined (__CC_ARM) && defined(__MICROLIB)
#if defined (__CC_ARM)
void __aeabi_assert(const char *expr, const char *file, int line)
{
    while (1);
}
extern uint64_t krhino_sys_time_get(void);
int gettimeofday(struct timeval *tv, void *tzp)
{
    uint64_t t = krhino_sys_time_get();
    tv->tv_sec = t / 1000;
    tv->tv_usec = (t % 1000) * 1000;
    return 0;
}

volatile int *__aeabi_errno_addr()
{
    return &errno;
}

#if (RHINO_CONFIG_MM_TLF > 0)
#define AOS_UNSIGNED_INT_MSB (1u << (sizeof(unsigned int) * 8 - 1))
extern void *aos_malloc(unsigned int size);
extern void aos_alloc_trace(void *addr, size_t allocator);
extern void aos_free(void *mem);
extern void *aos_realloc(void *mem, unsigned int size);

void *malloc(size_t size)
{
    void *mem;

#if (RHINO_CONFIG_MM_DEBUG > 0u)
    mem = aos_malloc(size | AOS_UNSIGNED_INT_MSB);
#else
    mem = aos_malloc(size);
#endif

    return mem;
}

void free(void *mem)
{
    aos_free(mem);
}

void *realloc(void *old, size_t newlen)
{
    void *mem;

#if (RHINO_CONFIG_MM_DEBUG > 0u)
    mem = aos_realloc(old, newlen | AOS_UNSIGNED_INT_MSB);
#else
    mem = aos_realloc(old, newlen);
#endif

    return mem;
}

void *calloc(size_t len, size_t elsize)
{
    void *mem;

#if (RHINO_CONFIG_MM_DEBUG > 0u)
    mem = aos_malloc((elsize * len) | AOS_UNSIGNED_INT_MSB);
#else
    mem = aos_malloc(elsize * len);
#endif

    if (mem) {
        memset(mem, 0, elsize * len);
    }

    return mem;
}

char * strdup(const char *s)
{
    size_t  len = strlen(s) +1;
    void *dup_str = aos_malloc(len);
    if (dup_str == NULL)
        return NULL;
    return (char *)memcpy(dup_str, s, len);
}

#endif

#pragma weak fputc
int fputc(int ch, FILE *f)
{
    /* Send data. */
    if (ch == '\n') {
      hal_uart_send(NULL, (void *)"\r", 1, AOS_WAIT_FOREVER);
    }
    hal_uart_send(NULL, &ch, 1, AOS_WAIT_FOREVER);
    return ch;
}

//referred from ota_socket.o
void bzero()
{

}

//referred from ssl_cli.o
time_t time(time_t *t)
{
    return (time_t)krhino_sys_time_get();
}

//referred from aos_network.o
int accept(int sock, long *addr, long *addrlen)
{
    return 0;
}

int listen(int sock, int backlog)
{
    return 0;
}

//referred from timing.o
unsigned int alarm(unsigned int seconds)
{
    return 0;
}

#endif

