#include <stdio.h>
#include <stdint.h>

#include <k_api.h>

#include "aos/hal/uart.h"
#include <hal/wifi.h>

#include "aos/cli.h"
#include "aos/init.h"
#include "aos/kernel.h"

#include "c_types.h"
#include "ets_sys.h"

#include "espos_scheduler.h"

extern int ets_printf(const char *fmt, ...);
extern void PendSV( char req );

extern char _bss_start;
extern char _bss_end;

static ktask_t *g_aos_init;

uart_dev_t uart_0 = {
    .port = 0,
};

static kinit_t kinit = {
    .argc = 0,
    .argv = NULL,
    .cli_enable = 1
};

static void app_entry(void *arg)
{
    aos_components_init(&kinit);
#ifndef AOS_BINS
    application_start(kinit.argc, kinit.argv);  /* jump to app/example entry */
#endif
}

extern uart_dev_t uart_0;

extern hal_wifi_module_t aos_wifi_esp8266;
void user_init(void)
{
    int ret = 0;

    extern int32_t hal_uart_init(uart_dev_t *uart);
    extern void key_gpio_init(void);

    key_gpio_init();
    hal_uart_init(&uart_0);

    hal_wifi_register_module(&aos_wifi_esp8266);
    ret = hal_wifi_init();
    if (ret){
        printf("waring: wifi init fail ret is %d \r\n", ret);
    }
#if defined(SUPPORT_SINGAPORE_DOMAIN)
    aos_task_new("main", app_entry, 0, 7.5*1024);
#elif defined(ESP8266_CHIPSET)
    aos_task_new("main", app_entry, 0, 2*1024);
#else
    aos_task_new("main", app_entry, 0, 6*1024);
#endif
}
