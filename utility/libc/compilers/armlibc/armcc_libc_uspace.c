/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <string.h>
#include <stdio.h>
#include <sys/time.h>
#include "k_config.h"
#include <aos/hal/hal.h>
#include <u_mm.h>

#if defined (__CC_ARM) && defined(__MICROLIB)
void __aeabi_assert(const char *expr, const char *file, int line)
{
    while (1);
}

extern uint64_t krhino_sys_time_get(void);

int gettimeofday(struct timeval *tv, void *tzp)
{
    uint64_t t;

    t = krhino_sys_time_get();

    tv->tv_sec  = t / 1000;
    tv->tv_usec = (t % 1000) * 1000;

    return 0;
}

void *malloc(size_t size)
{
    void *mem;
    mem = umm_malloc(size);

    return mem;
}

void free(void *mem)
{
    umm_free(mem);
}

void *realloc(void *old, size_t newlen)
{
    void *mem;

    mem = umm_realloc(old, newlen);

    return mem;
}

void *calloc(size_t len, size_t elsize)
{
    void *mem;
    mem = umm_malloc(elsize * len);

    if (mem) {
        memset(mem, 0, elsize * len);
    }

    return mem;
}

char * strdup(const char *s)
{
    size_t  len = strlen(s) +1;
    void *dup_str = umm_malloc(len);
    if (dup_str == NULL)
        return NULL;

    return (char *)memcpy(dup_str, s, len);
}

#pragma weak fputc
int fputc(int ch, FILE *f)
{
    uart_dev_t uart_stdio;

    memset(&uart_stdio, 0, sizeof(uart_stdio));
    uart_stdio.port = 0;
    /* Send data. */
    return hal_uart_send(&uart_stdio, (uint8_t *)(&ch), 1, 1000);
}

/* referred from ota_socket.o */
void bzero()
{

}

/* referred from ssl_cli.o */
time_t time(time_t *t)
{
    return 0;
}

/* referred from aos_network.o */
int accept(int sock, long *addr, long *addrlen)
{
    return 0;
}

int listen(int sock, int backlog)
{
    return 0;
}

/* referred from timing.o */
unsigned int alarm(unsigned int seconds)
{
    return 0;
}

#endif

