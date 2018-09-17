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
    //BSP_OS_TmrTickInit(RHINO_CONFIG_TICKS_PER_SECOND);

    //aos_kernel_init(&kinit);
	
	//smp_primes();
	
    //BSP_OS_TmrTickInit(RHINO_CONFIG_TICKS_PER_SECOND);
	time_init_global_tick();

    while(1) {
        
        printf("tick = %d\r\n", qq_tick++);

        aos_msleep(100);
        //krhino_task_sleep(100);

    }
    //while(1)  {BSP_Ser_Printf("016\r\n");}
}

#define ISR_STK_SIZE     400u
cpu_stack_t isr_stk[ISR_STK_SIZE];
cpu_stack_t * const  isr_base_ptr = &isr_stk[0];
unsigned int const  isr_stk_size = ISR_STK_SIZE;
cpu_stack_t     *except_stack_top;

void  isr_stk_init(void)
{
    unsigned int   i;
    cpu_stack_t *p_stk;


    p_stk = isr_base_ptr;                            /* Clear the ISR stack                                    */
    for (i = 0u; i < isr_stk_size; i++) {
        *p_stk++ = (cpu_stack_t)0u;
    }
    except_stack_top = (cpu_stack_t *)(isr_base_ptr + isr_stk_size - 2u);
}

//void main(void)

//void sys_start(void)
void main(void)
{
	platform_init();
    aos_hw_vector_init();

	isr_stk_init();
  
    aos_init();
    //soc_driver_init();

    krhino_task_dyn_create(&g_aos_init, "aos-init", 0, 5, 0, AOS_START_STACK, sys_init, 1);

    aos_start();
}

