#include <stdio.h>
#include <stdint.h>

#include <k_api.h>

#include <hal/soc/uart.h>
#include <aos/aos.h>

extern int ets_printf(const char *fmt, ...);

extern char _bss_start;
extern char _bss_end;

static ktask_t *g_aos_init;

uart_dev_t uart_0 = {
    .port = 0,
};

void application_start(void *p)
{
    while (1) {
        ets_printf("tick %lld\n", krhino_sys_tick_get());
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

void call_user_start(void)
{
    asm volatile("wsr    %0, vecbase\n" \
                 ::"r"(&_text_start));

    init_bss_data();

    ets_printf("esp8266 mcu start\n");

    aos_init();

    kstat_t ret = krhino_task_dyn_create(&g_aos_init, "aos app", 0, AOS_DEFAULT_APP_PRI, 0, 512, (task_entry_t)application_start, 1);
    if (ret != RHINO_SUCCESS)
        ets_printf("%d\n", ret);

    aos_start();

    /* Should never get here, unless there is an error in vTaskStartScheduler */
    for(;;) ;
}