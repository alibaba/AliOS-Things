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

#if AOS_COMP_CLI
#include "aos/cli.h"
#endif

#if (RHINO_CONFIG_HW_COUNT > 0)
void soc_hw_timer_init(void)
{
    uint32_t temp = (uint32_t)(0xFFFFFFFF / 1000000 * 32768);

    RTIM_ChangePeriodImmediate(TIMx[0], temp);
    RTIM_INTConfig(TIMx[0], TIM_IT_Update, ENABLE);
    RTIM_Cmd(TIMx[0], ENABLE);
}

hr_timer_t soc_hr_hw_cnt_get(void)
{
    uint32_t tick;

    RTIM_TypeDef *TIM = TIMx[0];
    tick = RTIM_GetCount(TIM);

    return tick;
}

lr_timer_t soc_lr_hw_cnt_get(void)
{
    uint32_t tick;

    RTIM_TypeDef *TIM = TIMx[0];
    tick = RTIM_GetCount(TIM);

    return tick;
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
#define configTOTAL_PSRAM_HEAP_SIZE (0x400000 - 0x20000)
PSRAM_HEAP_SECTION static unsigned char psRAMHeap[configTOTAL_PSRAM_HEAP_SIZE];

k_mm_region_t g_mm_region[2] = {{(uint8_t*)&heap_start,(size_t)&heap_len},
                                {(uint8_t*)psRAMHeap,(size_t)configTOTAL_PSRAM_HEAP_SIZE},
                               }; //,
                                                         // {(uint8_t*)MM_ALIGN_UP(0x100014f9), MM_ALIGN_DOWN(0xb07)},
                                                         // {(uint8_t*)MM_ALIGN_UP(0x10002475), MM_ALIGN_DOWN(0x2b8b)}};

#endif
int g_region_num  = sizeof(g_mm_region)/sizeof(k_mm_region_t);

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
