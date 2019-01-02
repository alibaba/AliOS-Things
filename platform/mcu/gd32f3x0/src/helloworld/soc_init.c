#include "gd32f3x0.h"
#include <aos/aos.h>
#include <aos/kernel.h>
#include <k_api.h>
#include "gd32f350r_eval.h"
#include "hal/soc/soc.h"
#include <stdio.h>

#define CERTIFICATE_TEST    0

aos_task_t g_init_task;
uart_dev_t  uart_0;
kinit_t g_kinit;

int aos_kernel_init(kinit_t *kinit);
void soc_init(void);
void test_certificate(void);

void hal_reboot(void)
{
    NVIC_SystemReset();
}

#if (1 == CERTIFICATE_TEST)
void led1_task(void *p)
{
    while(1)
    {
        gd_eval_led_toggle(LED2);
        aos_msleep(2000);
    }
}

#endif /* 0 == CERTIFICATE_TEST */

extern k_mm_region_t   g_mm_region[];

void init_task(void *p)
{
    nvic_priority_group_set(NVIC_PRIGROUP_PRE4_SUB0);
    SysTick_Config(SystemCoreClock / RHINO_CONFIG_TICKS_PER_SECOND);

    uart_0.port = 0;
    uart_0.config.baud_rate = 115200;
    uart_0.config.data_width = DATA_WIDTH_8BIT;
    uart_0.config.flow_control = FLOW_CONTROL_DISABLED;
    uart_0.config.parity = NO_PARITY;
    uart_0.config.stop_bits = STOP_BITS_1;
    hal_uart_init(&uart_0);
    
    g_kinit.argc = 0;
    g_kinit.argv = NULL;
    g_kinit.cli_enable = 1;
    aos_kernel_init(&g_kinit);
}

void soc_init(void)
{
    memset((uint8_t*)g_mm_region[0].start, 0, g_mm_region[0].len);
    krhino_init();
    aos_task_new_ext(&g_init_task, "init", init_task, 0, 512, AOS_DEFAULT_APP_PRI);
    krhino_start();
    while(1);
}

#if (1 == CERTIFICATE_TEST)
int application_start(int argc, char *argv[])
{
    aos_task_new("led1", led1_task, 0, 512);
    test_certificate();
    aos_task_exit(0);

    return 0;
}
#endif /* 1 == CERTIFICATE_TEST */
