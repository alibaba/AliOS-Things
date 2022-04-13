/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <string.h>
#include <stdio.h>
#include <sys/time.h>
#include "k_config.h"
#include <aos/hal/uart.h>
#include <umm.h>

#if defined (__CC_ARM)
#pragma weak  __aeabi_assert
void __aeabi_assert(const char *expr, const char *file, int line)
{
    while (1);
}

#pragma weak __stack_chk_fail
void __stack_chk_fail(void)
{
}

unsigned long __stack_chk_guard __attribute__((weak)) = 0xDEADDEAD;

extern uint64_t krhino_sys_time_get(void);

#pragma weak  gettimeofday
int gettimeofday(struct timeval *tv, void *tzp)
{
    uint64_t t;

    t = krhino_sys_time_get();

    tv->tv_sec  = t / 1000;
    tv->tv_usec = (t % 1000) * 1000;

    return 0;
}

#pragma weak malloc
void *malloc(size_t size)
{
    void *mem;
    mem = umm_alloc(size);

    return mem;
}

#pragma weak free
void free(void *mem)
{
    umm_free(mem);
}

#pragma weak realloc
void *realloc(void *old, size_t newlen)
{
    void *mem;

    mem = umm_realloc(old, newlen);

    return mem;
}

#pragma weak calloc
void *calloc(size_t len, size_t elsize)
{
    void *mem;
    mem = umm_alloc(elsize * len);

    if (mem) {
        memset(mem, 0, elsize * len);
    }

    return mem;
}
#pragma weak strdup
char * strdup(const char *s)
{
    size_t  len = strlen(s) +1;
    void *dup_str = umm_alloc(len);
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
#pragma weak  bzero
/* referred from ota_socket.o */
void bzero()
{

}

/* referred from ssl_cli.o */
#pragma weak time
time_t time(time_t *t)
{
    return 0;
}

/* referred from aos_network.o */
#pragma weak accept
int accept(int sock, long *addr, long *addrlen)
{
    return 0;
}

#pragma weak listen
int listen(int sock, int backlog)
{
    return 0;
}

/* referred from timing.o */
#pragma weak alarm
unsigned int alarm(unsigned int seconds)
{
    return 0;
}

#endif

