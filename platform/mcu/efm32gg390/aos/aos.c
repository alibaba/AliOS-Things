#include <stdio.h>
#include <k_api.h>
#include <aos/aos.h>
#include <aos/kernel.h>
#include "hal/hal.h"

#include <em_device.h>



static kinit_t kinit;
#define EFM32_HFXO_FREQ       UINT32_C(48000000)/**< value of the high speed external oscillator */

#define INIT_TASK_STACK_SIZE 2014
static ktask_t init_task;
cpu_stack_t init_task_stack[INIT_TASK_STACK_SIZE];

#define TEST_TASK_STACK_SIZE 1024
static ktask_t test_task;
cpu_stack_t test_task_stack[TEST_TASK_STACK_SIZE];

static var_init()
{
    kinit.argc = 0;
    kinit.argv = NULL;
    kinit.cli_enable = 1;
}

extern void hw_start_hal(void);

static void sys_init(void)
{
    //efm32_soc_init();

    SysTick_Config(EFM32_HFXO_FREQ/RHINO_CONFIG_TICKS_PER_SECOND);
    //xdk_board_init();
    //start_hw();
    efm32_soc_peripheral_init();
    hw_start_hal();

    var_init();
    aos_kernel_init(&kinit);


}

static void test_run(void)
{
    uint32_t cnt = 0;

    while (1) {
        printf("%s heart beat %u\r\n", __func__, cnt);
        cnt++;

        aos_msleep(5000);
    }
}

extern k_mm_region_t   g_mm_region[];

int main(void)
{
    aos_heap_set();

    efm32_soc_init();

    aos_init();

    krhino_task_create(&init_task, "aos_init", NULL,
                      AOS_DEFAULT_APP_PRI, 0,
                      init_task_stack,
                      INIT_TASK_STACK_SIZE,
                      (task_entry_t)sys_init, 1);

    krhino_task_create(&test_task, "test_init", NULL,
                      AOS_DEFAULT_APP_PRI, 0,
                      test_task_stack,
                      TEST_TASK_STACK_SIZE,
                      (task_entry_t)test_run, 1);


    aos_start();

    return 0;
}


uint32_t aos_get_version_info(void)
{
    return 0;
}

