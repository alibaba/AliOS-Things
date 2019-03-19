/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <string.h>
#include <stdio.h>
#include <sys/time.h>
#include "k_config.h"

#ifdef AOS_USPACE

#include <u_mm.h>
#include <hal/hal.h>

#if defined(__CC_ARM)

unsigned long __stack_chk_guard __attribute__((weak)) = 0xDEADDEAD;

void __attribute__((weak)) __stack_chk_fail(void)
{
    return;
}

void __aeabi_assert(const char *expr, const char *file, int line)
{
    while (1);
}

#endif

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

char *strdup(const char *s)
{
    size_t  len = strlen(s) +1;
    void *dup_str = umm_malloc(len);

    if (dup_str == NULL)
        return NULL;

    return (char *)memcpy(dup_str, s, len);
}

int fputc(int ch, FILE *f)
{
	static uint8_t print_buf[64];
	static int print_count = 0;

	if (print_count > 64) {
		print_count = 0;
	}

	print_buf[print_count++] = ch;
	if (print_count >= 64) {
		hal_uart_send(NULL, print_buf, print_count, 1000);
		print_count = 0;
	}

	if (ch == '\n') {
		print_buf[print_count++] = '\r';
		hal_uart_send(NULL, print_buf, print_count, 1000);
		print_count = 0;
	}

	return 0;
}

#endif

extern uint64_t krhino_sys_time_get(void);

int gettimeofday(struct timeval *tv, void *tzp)
{
    uint64_t t = krhino_sys_time_get();

    tv->tv_sec = t / 1000;
    tv->tv_usec = (t % 1000) * 1000;

    return 0;
}

//referred from ota_socket.o
void bzero(void)
{}

//referred from ssl_cli.o
time_t time(time_t *t)
{
    return 0;
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

