#include <stdio.h>
#include <stdint.h>

#include <k_api.h>

#include <hal/soc/uart.h>
#include <aos/aos.h>

#include "c_types.h"
#include "ets_sys.h"

#include "espos_scheduler.h"

extern int ets_printf(const char *fmt, ...);

static ktask_t *g_aos_init;

uart_dev_t uart_0 = {
    .port = 0,
};

static void application_start(void *p)
{
    size_t ticks = 0;

    while (1) {
        printf("tick %d\n", ticks++);
        krhino_task_sleep(RHINO_CONFIG_TICKS_PER_SECOND);
    }
}

static void idle_start(void *p)
{
    extern void system_soft_wdt_feed(void);
    extern void vApplicationIdleHook(void);
    extern void wDev_ProcessFiq(void);

    while (1) {
        system_soft_wdt_feed();
        vApplicationIdleHook();
        wDev_ProcessFiq();
        krhino_task_sleep(1);
    }
}

void call_user_start(void *p)
{
    extern int _text_start;

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
    extern void user_conn_test_init(void);

    user_conn_test_init();

    kstat_t ret = krhino_task_dyn_create(&g_aos_init, "aos app", 0, AOS_DEFAULT_APP_PRI, 1, 512, (task_entry_t)application_start, 1);
    if (ret != RHINO_SUCCESS)
        printf("aos app %d\n", ret);

    espos_task_t task;
    ret = espos_task_create(&task, "idle", NULL, 1, 1, 1024, idle_start, ESPOS_TASK_CREATE_NORMAL);
    if (ret)
        printf("idle app %d\n", ret);
}

#if 0

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
}
