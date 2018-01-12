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

static void application_start(void *p)
{
    size_t ticks = 0;
    extern void system_soft_wdt_feed(void);

    extern void uart_init_new(void);

    uart_init_new();

    while (1) {
        printf("tick %d\n", ticks++);
        system_soft_wdt_feed();
        krhino_task_sleep(RHINO_CONFIG_TICKS_PER_SECOND);
    }
}

static void init_bss_data(void)
{
    int* p;

    for (p = (int*)&_bss_start; p < (int*)&_bss_end; p++) {
        *p = 0;
    }
}

extern int _text_start;

// void call_user_start(void)
// {
//     asm volatile("wsr    %0, vecbase\n" \
//                  ::"r"(&_text_start));

//     init_bss_data();

//     ets_printf("ESP8266 mcu start\n");

//     aos_init();

//     kstat_t ret = krhino_task_dyn_create(&g_aos_init, "aos app", 0, AOS_DEFAULT_APP_PRI, 0, 512, (task_entry_t)application_start, 1);
//     if (ret != RHINO_SUCCESS)
//         ets_printf("%d\n", ret);

//     aos_start();

//     /* Should never get here, unless there is an error in vTaskStartScheduler */
//     for(;;) ;
// }

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

void user_init(void)
{
    static char s_buf[64];

    kstat_t ret = krhino_task_dyn_create(&g_aos_init, "aos app", 0, AOS_DEFAULT_APP_PRI, 1, 512, (task_entry_t)application_start, 1);
    if (ret != RHINO_SUCCESS)
        ets_printf("aos app %d\n", ret);
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
