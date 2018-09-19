/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include <assert.h>
#include <stdio.h>
#include <sys/time.h>
#include "NuMicro.h"

#if (RHINO_CONFIG_HW_COUNT > 0)
void soc_hw_timer_init(void)
{
}

hr_timer_t soc_hr_hw_cnt_get(void)
{
    return 0;
}

lr_timer_t soc_lr_hw_cnt_get(void)
{
    return 0;
}
#endif /* RHINO_CONFIG_HW_COUNT */

#if (RHINO_CONFIG_INTRPT_GUARD > 0)
void soc_intrpt_guard(void)
{
}
#endif

#if (RHINO_CONFIG_INTRPT_STACK_REMAIN_GET > 0)
size_t soc_intrpt_stack_remain_get(void)
{
    return 0;
}
#endif

#if (RHINO_CONFIG_INTRPT_STACK_OVF_CHECK > 0)
void soc_intrpt_stack_ovf_check(void)
{
}
#endif

#if (RHINO_CONFIG_MM_TLF > 0)

#if defined (__CC_ARM) /* Keil / armcc */
	#define DEF_IRAM_END				(0x20000000+0x00004000)
	extern unsigned int Image$$RW_IRAM1$$ZI$$Limit;
	int           g_region_num = 1;
	k_mm_region_t g_mm_region[1];
	void aos_heap_set()
	{
		g_mm_region[0].start = (uint8_t*)&Image$$RW_IRAM1$$ZI$$Limit;
		g_mm_region[0].len   = (DEF_IRAM_END - (size_t)&Image$$RW_IRAM1$$ZI$$Limit);
	}
#elif defined (__ICCARM__)/* IAR */
	#define HEAP_BUFFER_SIZE 1024*6
	int           g_region_num = 1;
	uint8_t g_heap_buf[HEAP_BUFFER_SIZE];
	k_mm_region_t g_mm_region[] = {{g_heap_buf, HEAP_BUFFER_SIZE}};
	void aos_heap_set()
	{
		g_mm_region[0].start = g_heap_buf;
		g_mm_region[0].len   = HEAP_BUFFER_SIZE;
	}

#else /* GCC */
	extern uint32_t __aos_heap_start;
	extern uint32_t __aos_heap_size;

	k_mm_region_t g_mm_region[] = {
		{ (uint8_t *)&__aos_heap_start, (uint32_t)&__aos_heap_size},
	};
	int g_region_num = 1;	
	void aos_heap_set() { }
#endif //GCC

#endif

#if (RHINO_CONFIG_MM_LEAKCHECK > 0 )

extern int __bss_start__, __bss_end__, _sdata, _edata;

void aos_mm_leak_region_init(void)
{
#if (RHINO_CONFIG_MM_DEBUG > 0)
    krhino_mm_leak_region_init(&__bss_start__, &__bss_end__);
    krhino_mm_leak_region_init(&_sdata, &_edata);
#endif
}

#endif //#if (RHINO_CONFIG_MM_TLF > 0)

#if (RHINO_CONFIG_TASK_STACK_CUR_CHECK > 0)
size_t soc_get_cur_sp()
{
    size_t sp = 0;
#if defined (__GNUC__)&&!defined(__CC_ARM)
    asm volatile(
        "mov %0,sp\n"
        :"=r"(sp));
#endif
    return sp;
}
static void soc_print_stack()
{
    void    *cur, *end;
    int      i=0;
    int     *p;

    end   = krhino_cur_task_get()->task_stack_base + krhino_cur_task_get()->stack_size;
    cur = (void *)soc_get_cur_sp();
    p = (int*)cur;
    while(p < (int*)end) {
        if(i%4==0) {
            printf("\r\n%08x:",(uint32_t)p);
        }
        printf("%08x ", *p);
        i++;
        p++;
    }
    printf("\r\n");
    return;
}
#endif

void soc_err_proc(kstat_t err)
{
    (void)err;
    
    #if (RHINO_CONFIG_TASK_STACK_CUR_CHECK > 0)
    soc_print_stack();
    #endif
    assert(0);
}

krhino_err_proc_t g_err_proc = soc_err_proc;

void soc_init(void)
{
    // NOTE: Support singleton semantics to be called from other init functions
    static int inited = 0;
    if (inited) {
        return;
    }
    inited = 1;
    
    /*---------------------------------------------------------------------------------------------------------*/
    /* Init System Clock                                                                                       */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Enable HIRC clock (internal OSC 12MHz) */
    CLK_EnableXtalRC(CLK_PWRCTL_HIRC_EN_Msk);
    /* Enable HXT clock (external XTAL 12MHz) */
    CLK_EnableXtalRC(CLK_PWRCTL_HXT_EN_Msk);
    /* Enable LIRC clock (OSC 10KHz) for lp_ticker */
    CLK_EnableXtalRC(CLK_PWRCTL_LIRC_EN_Msk);
    /* Enable LXT clock (XTAL 32KHz) for RTC */
    CLK_EnableXtalRC(CLK_PWRCTL_LXT_EN_Msk);

    /* Wait for HIRC clock ready */
    CLK_WaitClockReady(CLK_CLKSTATUS_HIRC_STB_Msk);
    /* Wait for HXT clock ready */
    CLK_WaitClockReady(CLK_CLKSTATUS_HXT_STB_Msk);
    /* Wait for LIRC clock ready */
    CLK_WaitClockReady(CLK_CLKSTATUS_LIRC_STB_Msk);
    /* Wait for LXT clock ready */
    CLK_WaitClockReady(CLK_CLKSTATUS_LXT_STB_Msk);

    /* Set HCLK source form HXT and HCLK source divide 1  */
    CLK_SetHCLK(CLK_CLKSEL0_HCLK_S_HXT, CLK_HCLK_CLK_DIVIDER(1));

    /* Select HXT/HIRC to clock PLL
     *
     * Comparison between HXT/HIRC-clocked PLL:
     * 1. Spare HXT on board if only HIRC is used.
     * 2. HIRC has shorter stable time.
     * 3. HXT has better accuracy. USBD requires HXT-clocked PLL.
     * 4. HIRC has shorter wake-up time from power-down mode. 
     *    Per test, wake-up time from power-down mode would take:
     *    T1. 1~13 ms (proportional to deep sleep time) with HXT-clocked PLL as HCLK clock source
     *    T2. <1 ms with HIRC-clocked PLL as HCLK clock source
     *    T1 will fail Greentea test which requires max 10 ms wake-up time.
     *
     *    If we just call CLK_SetCoreClock(FREQ_42MHZ) to configure HCLK to 42 MHz,
     *    it will go T1 with HXT already enabled in front. So we manually configure
     *    it to choose HXT/HIRC-clocked PLL.
     */
#define NU_HXT_PLL      1
#define NU_HIRC_PLL     2

#ifndef NU_CLOCK_PLL
#define NU_CLOCK_PLL    NU_HIRC_PLL
#endif

#if (NU_CLOCK_PLL == NU_HXT_PLL)
    CLK_EnablePLL(CLK_PLLCTL_PLL_SRC_HXT, FREQ_42MHZ*2);
    CLK_SetHCLK(CLK_CLKSEL0_HCLK_S_PLL, CLK_HCLK_CLK_DIVIDER(2));
#elif (NU_CLOCK_PLL == NU_HIRC_PLL)
    CLK_EnablePLL(CLK_PLLCTL_PLL_SRC_HIRC, FREQ_42MHZ*2);
    CLK_SetHCLK(CLK_CLKSEL0_HCLK_S_PLL, CLK_HCLK_CLK_DIVIDER(2));
#endif

    /* Update System Core Clock */
    /* User can use SystemCoreClockUpdate() to calculate SystemCoreClock. */
    SystemCoreClockUpdate();

    /* Lock protected registers */
    SYS_LockReg();
}

void SysTick_Handler(void)
{
    krhino_intrpt_enter();
    krhino_tick_proc();
    krhino_intrpt_exit();
}
