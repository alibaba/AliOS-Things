#include <stdio.h>
#include <stdint.h>

#include <k_api.h>

#include <hal/soc/uart.h>
#include <aos/aos.h>

#include "c_types.h"
#include "ets_sys.h"

#include "espos_scheduler.h"

extern int ets_printf(const char *fmt, ...);

extern char _bss_start;
extern char _bss_end;

static ktask_t *g_aos_init;

uart_dev_t uart_0 = {
    .port = 0,
};


static void init_bss_data(void)
{
    int* p;

    for (p = (int*)&_bss_start; p < (int*)&_bss_end; p++) {
        *p = 0;
    }
}

extern int _text_start;

void call_user_start(void *p)
{
    asm volatile("wsr    %0, vecbase\n" \
                  ::"r"(&_text_start));

    extern void user_start(void);

    ets_printf("ESP8266 mcu start\n");

    user_start();
}

void vPortETSIntrLock(void)
{
    ETS_INTR_LOCK();
}

void vPortETSIntrUnlock(void)
{
    ETS_INTR_UNLOCK();
}

static kinit_t kinit = {
    .argc = 0,
    .argv = NULL,
    .cli_enable = 1
};

void user_init(void)
{
    static char s_buf[64];
    extern int32_t hal_uart_init(uart_dev_t *uart);

    hal_uart_init(&uart_0);

    aos_kernel_init(&kinit);
}

void dhcps_start(void)
{

}

void dhcps_stop(void)
{

}

void LwipTimOutLim(void)
{

}

void netif_create_ip6_linklocal_address(void)
{

}

void netif_create_ip4_linklocal_address(void)
{

}

void ethernetif_init(void)
{

}

void ethernetif_input(void)
{

}

void user_fatal_exception_handler(void)
{
    ets_printf("user_fatal_exception_handler\n");
}

char *hostname;
char *default_hostname;
