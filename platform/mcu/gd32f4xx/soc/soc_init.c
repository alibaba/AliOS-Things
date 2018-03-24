#include "gd32f4xx.h"
#include <aos/aos.h>
#include <aos/kernel.h>
#include <k_api.h>
#include "gd32f450z_eval.h"
#include "hal/soc/soc.h"
#include <stdio.h>
#include "gd32f450z_eval.h"
#include <hal/wifi.h>
#include <hal/ota.h>

aos_task_t g_init_task;
uart_dev_t  uart_0;
static kinit_t kinit;

int aos_kernel_init(kinit_t *kinit);
void soc_init(void);
void dev_wifi_error_reset(void);

static void var_init()
{
    kinit.argc = 0;
    kinit.argv = NULL;
    kinit.cli_enable = 1;
}

void hal_reboot(void)
{
    NVIC_SystemReset();
}

void led1_task(void *p)
{
    while(1)
    {
        gd_eval_led_toggle(LED2);
        aos_msleep(2000);
    }
}
    
void led2_task(void *p)
{
    while(1)
    {
        gd_eval_led_toggle(LED3);
        aos_msleep(2000);
    }

}
extern hal_wifi_module_t aos_wifi_module_mk3060;
extern k_mm_region_t   g_mm_region[];

void dev_wifi_reset(void)
{
    /* enable the led clock */
    rcu_periph_clock_enable(RCU_GPIOE);
    /* configure led GPIO port */ 
    gpio_mode_set(GPIOE, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, GPIO_PIN_0);
    gpio_output_options_set(GPIOE, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_0);
    gpio_bit_reset(GPIOE, GPIO_PIN_0);
    aos_msleep(200);
    gpio_bit_set(GPIOE, GPIO_PIN_0);
    aos_msleep(500);
}

void init_task(void *p)
{
    var_init();
    nvic_priority_group_set(NVIC_PRIGROUP_PRE4_SUB0);
    SysTick_Config(SystemCoreClock / RHINO_CONFIG_TICKS_PER_SECOND);
    dev_wifi_reset();
    uart_0.port = 0;
    uart_0.config.baud_rate = 921600;
    uart_0.config.data_width = DATA_WIDTH_8BIT;
    uart_0.config.flow_control = FLOW_CONTROL_DISABLED;
    uart_0.config.parity = NO_PARITY;
    uart_0.config.stop_bits = STOP_BITS_1;
    hal_uart_init(&uart_0);
    tcpip_init( NULL, NULL );
    hal_wifi_register_module(&aos_wifi_module_mk3060);
    aos_kernel_init(&kinit);
}
uint32_t g_wifireset_flag = 0;
void soc_init(void)
{
    uint32_t temp;
    
    temp = *(uint32_t *)(0x20020000);
    
    if(0xaabbccdd == temp){
        g_wifireset_flag = 1;
    }
    
    memset((uint8_t*)g_mm_region[0].start, 0, g_mm_region[0].len);
    krhino_init();
    aos_task_new_ext(&g_init_task, "init", init_task, 0, 8192, AOS_DEFAULT_APP_PRI);
    krhino_start();
    while(1);
}

int aos_framework_init(void)
{
    LOGI(0,"aos framework init. v1.5");
    
    version_init();
#ifdef MESH_GATEWAY_SERVICE
    gateway_service_init();
#endif

#ifdef AOS_FOTA
   // ota_service_init();
#endif

    return 0;
}
typedef void (*p_recovery_t)(void);
void dev_wifi_error_reset(void)
{
    uint32_t temp;
    p_recovery_t p_recovery;
    
    SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
    usart_disable(USART1);
    __set_PSP(0x20030000);
    __set_MSP(0x20030000);
    temp = __get_CONTROL();
    __set_CONTROL(temp & (~0x00000002));
    *(uint32_t *)(0x20020000) = 0xaabbccdd;
    p_recovery = (p_recovery_t)(*(uint32_t*)0x08000004);
    (*p_recovery)();
}

void aos_components_init(void)
{

}

#if defined (__CC_ARM)
_ARMABI time_t time(time_t * p)
{
    return 0;
}
#endif

//int application_start(int argc, char *argv[])
//{
//    aos_task_new("led1", led1_task, 0, 512);
//    aos_task_new("led2", led2_task, 0, 512);
//    aos_task_exit(0);
//}
