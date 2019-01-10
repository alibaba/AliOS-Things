/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include <stdio.h>
#include <stdlib.h>

#include "aos/init.h"
#include "aos/hal/uart.h"
#include "aos/cli.h"
#include "network/hal/wifi.h"

#include "wm_regs.h"
#include "wm_io.h"
#include "wm_gpio.h"
#include "wm_params.h"
#include "wm_netif2.0.3.h"

extern void board_init(void);

#ifndef AOS_BINS
extern int application_start(int argc, char **argv);
#endif

/* main task stask size(byte) */
#define OS_MAIN_TASK_STACK 0x600
#define OS_MAIN_TASK_PRI 32

static ktask_t g_main_task;
cpu_stack_t g_main_task_buf[OS_MAIN_TASK_STACK];
static kinit_t kinit = {0, NULL, 1};

extern int tls_ethernet_init();

extern void wifi_net_status_changed(u8 status);

void do_mminfo(void)
{
    k_mm_head *mmhead = g_kmm_head;

//      MM_CRITICAL_ENTER(g_kmm_head);    
    printf("\r\n");
    printf("------------------------- memory allocation statistic ------------------------------ \r\n");

    int i;

    if (!mmhead) {
        return;
    }

    printf("     free     |     used     |     maxused\r\n");
    printf("  %10d  |  %10d  |  %10d\r\n", 
          mmhead->free_size, mmhead->used_size, mmhead->maxused_size);
    printf("\r\n");   
 
//    MM_CRITICAL_EXIT(g_kmm_head); 
}

static void wifi_rx_info(char *buf, int len, int argc, char **argv)
{
}
static void wifi_tx_info(char *buf, int len, int argc, char **argv)
{
    u8* buff = NULL;
    printf("wifi_tx_info enter\n");
    buff = tls_wifi_buffer_acquire(100);
    printf("tls_wifi_buffer_acquire buf %p\n", buff);
    tls_wifi_buffer_release(false, buff);
    printf("tls_wifi_buffer_release buf %p\n", buff);
}
static cb_fun(void* ctx)
{
    printf("\n@@@@@@@@@@@@@@@@@@@@@@ lwip task is alive! @@@@@@@@@@@@@@@@@@@@@@@@@@\n");    
}
extern void * lwip_fun_ptr;
static void lwip_info(char *buf, int len, int argc, char **argv)
{
    tcpip_callback_with_block(cb_fun, NULL, 1);
}

struct cli_command w600_cli_cmd[] = {
    { "wifi_rx", "dump wifi rx task info", wifi_rx_info },
    { "wifi_tx", "dump wifi tx task info", wifi_tx_info },
    { "lwip", "dump lwip task info", lwip_info },//tcpip_callback_with_block
};


void tls_w600_cli_init(void)
{
    aos_cli_register_commands(&w600_cli_cmd[0],sizeof(w600_cli_cmd) / sizeof(struct cli_command));
}

#define MCU_CLOCK_HZ        (80000000)

extern uart_dev_t uart_1;
extern uart_dev_t uart_0;

static void sys_init(void)
{
    hal_uart_init(&uart_0);

    SysTick_Config(MCU_CLOCK_HZ / 100);

    /* user code start*/
    tls_fls_init();

#if TLS_CONFIG_HARD_CRYPTO
    tls_crypto_init();
#endif

    tls_param_load_factory_default();
    tls_param_init();

    w600_wifi_register();
    hal_wifi_init();

    tls_ethernet_init();
    tls_sys_init();

    //hal_ota_register_module(&w600_ota_module);

    tls_netif_add_status_event(wifi_net_status_changed);
    /*insert driver to enable irq for example: starting to run tick time.
     drivers to trigger irq is forbidden before aos_start, which will start core schedule.
    */
    /*user_trigger_irq();*/  //for example

    /*aos components init including middleware and protocol and so on !*/
    aos_components_init(&kinit);

    #ifndef AOS_BINS
    application_start(0, NULL);  /* jump to app/example entry */
    #endif
}


int main(void)
{
    /*irq initialized is approved here.But irq triggering is forbidden, which will enter CPU scheduling.
    Put them in sys_init which will be called after aos_start.
    Irq for task schedule should be enabled here, such as PendSV for cortex-M4.
    */
    board_init();   //including aos_heap_set();  flash_partition_init();

    /*kernel init, malloc can use after this!*/
    krhino_init();

    /*main task to run */
    krhino_task_create(&g_main_task, "main_task", 0, 
                       OS_MAIN_TASK_PRI, 0, 
                       g_main_task_buf, OS_MAIN_TASK_STACK, 
                       (task_entry_t)sys_init, 1);

    /*kernel start schedule!*/
    krhino_start();

    /*never run here*/
    return 0;
}

