#include <stdio.h>
#include <stdint.h>

#include <k_api.h>

#include "aos/hal/uart.h"
#include <hal/wifi.h>

#include "cli.h"
#include "aos/init.h"
#include "aos/kernel.h"

#include "c_types.h"
#include "ets_sys.h"

#include "espos_scheduler.h"

extern int ets_printf(const char *fmt, ...);
extern void PendSV( char req );

extern char _bss_start;
extern char _bss_end;


static void init_bss_data(void)
{
    int* p;

    for (p = (int*)&_bss_start; p < (int*)&_bss_end; p++) {
        *p = 0;
    }
}

extern int _text_start;

#ifndef CONFIG_ESP_LWIP
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

char *hostname;
char *default_hostname;

#endif

void user_fatal_exception_handler(void)
{
    ets_printf("user_fatal_exception_handler\n");
    while(1)
    {}
}

void wifi_nmi_post_soft_isr(void)
{
    PendSV(2);
}
