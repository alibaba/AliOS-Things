#include <stdio.h>
#include <stdint.h>

#include <hal/soc/soc.h>
#include <hal/wifi.h>
#include <hal/ota.h>
#include <aos/aos.h>
#include <sdkconfig.h>

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
    aos_kernel_init(&kinit);
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
void app_main(void)
{
    initialise_wifi();
    hal_uart_init(&uart_0);
#ifdef HCI_H4_NRF51822
    extern void nrf51822_h4_set_uart_config();
    nrf51822_h4_set_uart_config();
#endif
#if defined(CONFIG_AOS_MESH) && defined(CONFIG_BT_MESH)
    extern void esp_bt_mesh_register(void);
    esp_bt_mesh_register();
#endif
    hal_ota_register_module(&esp32_yos_ota_module);
    aos_task_new("main", app_entry, 0, 8192);
}
