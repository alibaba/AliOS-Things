#include "gd32f3x0.h"
#include <aos/aos.h>
#include <aos/kernel.h>
#include <k_api.h>
#include "gd32f350r_eval.h"
#include "hal/soc/soc.h"
#include <stdio.h>

#define CERTIFICATE_TASK    1

uart_dev_t  uart_0;

#ifdef __GNUC__
void set_errno(int err)
{
    errno = err;
}
#endif
void dev_uart_init(uart_dev_t *uart);
void test_certificate(void);
    
void hal_reboot(void)
{
    NVIC_SystemReset();
}

#if 0 == CERTIFICATE_TASK
#define LED1_TASK_STACKSIZE    128 //128*cpu_stack_t = 512byte
#define LED1_TASK_PRIORITY     20

#define LED2_TASK_STACKSIZE    128 //256*cpu_stack_t = 512byte
#define LED2_TASK_PRIORITY     21

static ktask_t led1_task_obj;
cpu_stack_t led1_task_buf[LED1_TASK_STACKSIZE];

static ktask_t led2_task_obj;
cpu_stack_t led2_task_buf[LED2_TASK_STACKSIZE];

void led1_task(void *p)
{
    while(1)
    {
        gd_eval_led_toggle(LED2);
        krhino_task_sleep(RHINO_CONFIG_TICKS_PER_SECOND);
    }
}
    
void led2_task(void *p)
{
    while(1)
    {
        gd_eval_led_toggle(LED3);
        krhino_task_sleep(RHINO_CONFIG_TICKS_PER_SECOND);
    }

}

#endif /* 0 == CERTIFICATE_TASK */

void soc_init(void)
{
    nvic_priority_group_set(NVIC_PRIGROUP_PRE4_SUB0);
    SysTick_Config(SystemCoreClock / RHINO_CONFIG_TICKS_PER_SECOND);

    uart_0.port = 0;
    uart_0.config.baud_rate = 115200;
    uart_0.config.data_width = DATA_WIDTH_8BIT;
    uart_0.config.flow_control = FLOW_CONTROL_DISABLED;
    uart_0.config.parity = NO_PARITY;
    uart_0.config.stop_bits = STOP_BITS_1;
    dev_uart_init(&uart_0);
#if CERTIFICATE_TASK == 1
    test_certificate();
#else 
    krhino_init();    
    krhino_task_create(&led1_task_obj, "led1_task", 0, LED1_TASK_PRIORITY, 
        50, led1_task_buf, LED1_TASK_STACKSIZE, led1_task, 1);
    krhino_task_create(&led2_task_obj, "led2_task", 0, LED2_TASK_PRIORITY, 
        50, led2_task_buf, LED2_TASK_STACKSIZE, led2_task, 1);
    krhino_start();
#endif
    while(1);
}
