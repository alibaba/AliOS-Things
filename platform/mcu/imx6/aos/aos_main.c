/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <aos_main.h>
#include <k_api.h>
#include <aos/kernel.h>
#include <aos/init.h>

extern void time_init_global_tick(void);
extern void platform_init(void);

#define AOS_START_STACK 2048
ktask_t *g_aos_init;

static kinit_t kinit = {
    .argc = 0,
    .argv = NULL,
    .cli_enable = 0
};

unsigned int qq_tick;
static void sys_init(void)
{
    int ret = 0;
    hal_uart_init(NULL);

    printf("sys_init in\r\n");

	time_init_global_tick();

    #if (RHINO_CONFIG_CPU_NUM > 1)
    extern void os_load_slavecpu(void);
    os_load_slavecpu();
    #endif
    /*
#ifdef AOS_COMP_CLI
    aos_cli_init();
#endif
#ifdef AOS_COMP_ULOG
    ulog_init();
#endif
	#ifdef OSAL_RHINO
    dumpsys_cli_init();
	#endif
	*/
    //test_certificate();

    while(1) {

        //printf("tick = %d\r\n", qq_tick++);

        aos_msleep(100);
        //krhino_task_sleep(100);

    }
}

/*
#define ISR_STK_SIZE     400u
cpu_stack_t isr_stk[RHINO_CONFIG_CPU_NUM][ISR_STK_SIZE];
unsigned int const  isr_stk_size = ISR_STK_SIZE;
cpu_stack_t     *except_stack_top[RHINO_CONFIG_CPU_NUM];

void  isr_stk_init(void)
{
    unsigned int   i;
    cpu_stack_t *p_stk;
    int coreid;
    cpu_stack_t * isr_base_ptr;// = &isr_stk[0];

    coreid = cpu_cur_get();
    isr_base_ptr = &(isr_stk[coreid][0]);

    p_stk = isr_base_ptr;                            // Clear the ISR stack
    for (i = 0u; i < isr_stk_size; i++) {
        *p_stk++ = (cpu_stack_t)0u;
    }
    except_stack_top[coreid] = (cpu_stack_t *)(isr_base_ptr + isr_stk_size - 2u);
}
*/

void sys_start(void)
{
    int ret;
	platform_init();


#if (RHINO_CONFIG_CPU_NUM > 1)
    extern void smp_cpu_init(void);
    smp_cpu_init();
#endif

    k_vectable_set();
	//isr_stk_init();
    aos_init();

    #if (RHINO_CONFIG_CPU_NUM > 1)
    /*bind 0 core*/
    ret = krhino_task_cpu_dyn_create(&g_aos_init, "aos-init", 0, 2, 0, AOS_START_STACK, sys_init, 0,1);

    //soc_driver_init();
    #else
    ret = krhino_task_dyn_create(&g_aos_init, "aos-init", 0, 2, 0, AOS_START_STACK, sys_init, 1);

    #endif

    aos_start();
}

