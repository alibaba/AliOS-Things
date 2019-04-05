#include <stdio.h>
#include <stdint.h>

#include "aos/hal/uart.h"
#include <hal/wifi.h>

#include "aos/init.h"
#include "aos/kernel.h"
#include "ulog/ulog.h"

#include <sdkconfig.h>

#include <xtensa_api.h>

#include "k_api.h"
uart_dev_t uart_0 = {
    .port = CONFIG_CONSOLE_UART_NUM,
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

#ifdef ENABLE_WIFI
#include "lwip/sockets.h"
int __attribute__((weak)) eventfd(unsigned int initval, int flags)
{
    return socket(AF_INET, SOCK_DGRAM, 0);
}

extern void tcpip_adapter_init(void);
extern hal_wifi_module_t sim_aos_wifi_eps32;
extern void esp32_wifi_mesh_register(void);
static void initialise_wifi(void)
{
    tcpip_adapter_init();
    hal_wifi_register_module(&sim_aos_wifi_eps32);
#ifdef CONFIG_AOS_MESH
    esp32_wifi_mesh_register();
#endif
    hal_wifi_init();
}
#else
static void initialise_wifi(void)
{

}
#endif

extern uart_dev_t uart_0;
extern struct hal_ota_module_s esp32_yos_ota_module;

#if (RHINO_CONFIG_CPU_NUM > 1)
extern void dport_access_init_core_aos();
extern void os_crosscore_int_init(void);
/*wait another core start*/
extern void os_load_slavecpu(void);
extern void  rtc_isr(void *arg);
extern void  timer_alarm_isr(void *arg);
extern void  task_wdt_isr(void *arg);
extern void os_modify_int_handle(int n,void *f);
extern void  os_crosscore_isr(void *arg);
extern void os_set_int_muticore(int n);
#endif


void app_main(void)
{
    hal_uart_init(&uart_0);
    #if (RHINO_CONFIG_CPU_NUM > 1)
    /*modify all int vector in cpu0 for 2 cpu*/
    os_modify_int_handle(24,os_crosscore_isr);

    /*wait another core start*/
    os_load_slavecpu();
    #endif

    initialise_wifi();

#ifdef HCI_H4_NRF51822
    extern void nrf51822_h4_set_uart_config();
    nrf51822_h4_set_uart_config();
#endif
#ifdef HCI_H4_NRF52840
    extern void nrf52840_h4_set_uart_config();
    nrf52840_h4_set_uart_config();
#endif
#if defined(CONFIG_AOS_MESH) && defined(CONFIG_BT_MESH)
    extern void esp_bt_mesh_register(void);
    esp_bt_mesh_register();
#endif

#ifdef TINY_ENGINE
    aos_task_new("main", app_entry, 0, 8192+4096+2048);
#else
    aos_task_new("main", app_entry, 0, 8192);
#endif
}

