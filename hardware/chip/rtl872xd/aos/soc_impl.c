/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include <assert.h>
#include <stdio.h>
#include <sys/time.h>
#include "diag.h"

#include "ameba_soc.h"
#include "build_info.h"
//#include "strproc.h"
#include "rtl8721d_system.h"
#include <aos/tty_csi.h>
#include <aos/gpioc_csi.h>

#if AOS_COMP_CLI
#include "aos/cli.h"
#endif

#if (RHINO_CONFIG_HW_COUNT > 0)
void soc_hw_timer_init(void)
{
    /* Stop and reset the SysTick. */
    SysTick->CTRL = 0UL;
    SysTick->VAL = 0UL;
	/* Configure SysTick to interrupt at the requested rate. */
	SysTick->LOAD = ( configCPU_CLOCK_HZ / configTICK_RATE_HZ ) - 1UL;
	SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk |
                    SysTick_CTRL_TICKINT_Msk   |
                    SysTick_CTRL_ENABLE_Msk;
}

hr_timer_t soc_hr_hw_cnt_get(void)
{
    return SysTick->VAL;
}

lr_timer_t soc_lr_hw_cnt_get(void)
{
    return 0;
}
#endif /* RHINO_CONFIG_HW_COUNT */

#if (RHINO_CONFIG_INTRPT_STACK_OVF_CHECK > 0)
void soc_intrpt_stack_ovf_check(void)
{
}
#endif

#if (RHINO_CONFIG_MM_TLF > 0)
#if !defined (__CC_ARM) /* Keil / armcc */
extern void         *heap_start;
extern void         *heap_end;
extern void         *heap_len;

//extern void         *heap2_start;
//extern void         *heap2_len;
#endif

//#include "section_config.h"
//SRAM_BF_DATA_SECTION
//#endif
//static unsigned char ucHeap[ configTOTAL_HEAP_SIZE ];


#if defined (__CC_ARM) /* Keil / armcc */
#define HEAP_BUFFER_SIZE 1024*60
uint8_t g_heap_buf[HEAP_BUFFER_SIZE];
k_mm_region_t g_mm_region[] = {{g_heap_buf, HEAP_BUFFER_SIZE}, {(uint8_t *)0x10000000, 0x8000}};
#else

k_mm_region_t g_mm_region[] = {{(uint8_t*)&heap_start,(size_t)&heap_len}}; //,
                                                         // {(uint8_t*)MM_ALIGN_UP(0x100014f9), MM_ALIGN_DOWN(0xb07)},
                                                         // {(uint8_t*)MM_ALIGN_UP(0x10002475), MM_ALIGN_DOWN(0x2b8b)}};

#endif
int           g_region_num  = sizeof(g_mm_region)/sizeof(k_mm_region_t);

#endif

extern void hal_reboot(void);
void soc_err_proc(kstat_t err)
{
    DBG_8195A("soc_err_proc : %d\n\r", err);
    if ( RHINO_NO_MEM == err )
    {
        /* while mem not enought, reboot */
        hal_reboot();
    }
}

krhino_err_proc_t g_err_proc = soc_err_proc;

static int tty_csi_init(void)
{
    static aos_tty_csi_t tty_csi[2];
    int ret;

    tty_csi[0].tty.dev.id = 0;
    ret = (int)aos_tty_csi_register(&tty_csi[0]);
    if (ret)
        return ret;

    tty_csi[1].tty.dev.id = 3;
    ret = (int)aos_tty_csi_register(&tty_csi[1]);
    if (ret) {
        (void)aos_tty_csi_unregister(0);
        return ret;
    }

    return 0;
}

LEVEL1_DRIVER_ENTRY(tty_csi_init)

static int gpioc_csi_init(void)
{
    static aos_gpioc_csi_t gpioc_csi[2];
    int ret;

    gpioc_csi[0].gpioc.dev.id = 0;
    gpioc_csi[0].default_input_cfg = AOS_GPIO_INPUT_CFG_HI;
    gpioc_csi[0].default_output_cfg = AOS_GPIO_OUTPUT_CFG_PP;
    ret = (int)aos_gpioc_csi_register(&gpioc_csi[0]);
    if (ret)
        return ret;

    gpioc_csi[1].gpioc.dev.id = 1;
    gpioc_csi[1].default_input_cfg = AOS_GPIO_INPUT_CFG_HI;
    gpioc_csi[1].default_output_cfg = AOS_GPIO_OUTPUT_CFG_PP;
    ret = (int)aos_gpioc_csi_register(&gpioc_csi[1]);
    if (ret) {
        (void)aos_gpioc_csi_unregister(0);
        return ret;
    }

    return 0;
}

LEVEL1_DRIVER_ENTRY(gpioc_csi_init)


/* use for printk */
int alios_debug_print(const char *buf, int size)
{
    int i;

    for (i = 0; i < size; i++) {
        DiagPutChar(*buf++);
    }

    return 0;
}

char uart_input_read(void)
{
    return DiagGetChar(1);
}

/*  check pc available  0:available  other:not available */
extern uint8_t __flash_text_start__[];
extern uint8_t __flash_text_end__[];
extern uint8_t __ram_image2_text_start__[];
extern uint8_t __ram_image2_text_end__[];

int alios_debug_pc_check(char *pc)
{
    if ((((uint32_t)pc > (uint32_t)__flash_text_start__) &&
         ((uint32_t)pc < (uint32_t)__flash_text_end__)) ||
        (((uint32_t)pc > (uint32_t)__ram_image2_text_start__) &&
         ((uint32_t)pc < (uint32_t)__ram_image2_text_end__))) {
        return 0;
    } else {
        return -1;
    }
}

#if AOS_COMP_CLI
void alios_debug_pc_show(int argc, char **argv)
{
    aos_cli_printf("----- PC Addr ------\r\n");
    aos_cli_printf("addr 1 : 0x%08X ~ 0x%08X\r\n", (uint32_t)__flash_text_start__, (uint32_t)__flash_text_end__);
    aos_cli_printf("addr 2 : 0x%08X ~ 0x%08X\r\n", (uint32_t)__ram_image2_text_start__, (uint32_t)__ram_image2_text_end__);
}
#endif
