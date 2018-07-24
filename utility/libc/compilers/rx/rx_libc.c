/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <string.h>
#include <stdio.h>
#include <sys/time.h>
#include "k_config.h"
#include "rx_platform.h"           // Located in the FIT BSP module
#include "r_sci_rx_if.h"        // The SCI module API interface file.
#include "r_byteq_if.h"         // The BYTEQ module API interface file.
#include "r_sci_rx_config.h"    // User configurable options for the SCI module
#include "uart/uart.h"
int errno;

extern uart_dev_t console;
extern _uart_drv uart_drv[];
#if defined _RX//(__CC_ARM) && defined(__MICROLIB)
void __aeabi_assert(const char *expr, const char *file, int line)
{
    while (1);
}
extern long long krhino_sys_time_get(void);
int gettimeofday(struct timeval *tv, void *tzp)
{
    uint64_t t = krhino_sys_time_get();
    tv->tv_sec = t / 1000;
    tv->tv_usec = (t % 1000) * 1000;
    return 0;
}

#if (RHINO_CONFIG_MM_TLF > 0)
#define AOS_UNSIGNED_INT_MSB (1u << (sizeof(unsigned int) * 8 - 1))
extern void *aos_malloc(unsigned int size);
extern void aos_alloc_trace(void *addr, size_t allocator);
extern void aos_free(void *mem);
extern void *aos_realloc(void *mem, unsigned int size);
extern int32_t aos_uart_send(void *data, uint32_t size, uint32_t timeout);

void *malloc(size_t size)
{
    void *mem;

#if (RHINO_CONFIG_MM_DEBUG > 0u && RHINO_CONFIG_GCC_RETADDR > 0u)
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

#if (RHINO_CONFIG_MM_DEBUG > 0u && RHINO_CONFIG_GCC_RETADDR > 0u)
    mem = aos_realloc(old, newlen | AOS_UNSIGNED_INT_MSB);
#else
    mem = aos_realloc(old, newlen);
#endif

    return mem;
}

void *calloc(size_t len, size_t elsize)
{
    void *mem;

#if (RHINO_CONFIG_MM_DEBUG > 0u && RHINO_CONFIG_GCC_RETADDR > 0u)
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

#pragma weak fputc
int fputc(int ch, FILE *f)
{
    /* Send data. */
//	return  hal_uart_send(&console,&c,1,0xFFFF);
    return aos_uart_send((uint8_t *)(&ch), 1, 1000);
}

void SCI_PUT_CHAR(sci_hdl_t const ,long );
void my_sw_charput_function(long ch)
{
	//hal_uart_send(&console,(uint8_t *)(&ch),1,0xffff);
	uint8_t		chan;
	chan = console.port;
	R_SCI_Send(uart_drv[chan].hdl,(uint8_t *)(&ch),1);
	//SCI_PUT_CHAR(uart_drv[chan].hdl,ch);

	}

#endif

//referred from ota_socket.o
void bzero()
{

}

//referred from ssl_cli.o
time_t time(time_t *t)
{
}

//referred from aos_network.o
int accept(int sock, long *addr, long *addrlen)
{
}

int listen(int sock, int backlog)
{

}

#endif
