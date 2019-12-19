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
#include "system_8195a.h"


#if (RHINO_CONFIG_HW_COUNT > 0)
void soc_hw_timer_init(void)
{
}

hr_timer_t soc_hr_hw_cnt_get(void)
{
    return 0;
    //return *(volatile uint64_t *)0xc0000120;
}

lr_timer_t soc_lr_hw_cnt_get(void)
{
    return 0;
}
#endif /* RHINO_CONFIG_HW_COUNT */

#if (RHINO_CONFIG_MM_TLF > 0)
#if !defined (__CC_ARM) /* Keil / armcc */
//extern void         *heap_start;
//extern void         *heap_end;
//extern void         *heap_len;

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

k_mm_region_t g_mm_region[] = {{(uint8_t*)&heap_start,(size_t)&heap_len},
                                                          {(uint8_t*)MM_ALIGN_UP(0x100014f9), MM_ALIGN_DOWN(0xb07)},
                                                          {(uint8_t*)MM_ALIGN_UP(0x10002475), MM_ALIGN_DOWN(0x2b8b)}};

#endif
int           g_region_num  = sizeof(g_mm_region)/sizeof(k_mm_region_t);

#endif

extern void hal_reboot(void);
void soc_err_proc(kstat_t err)
{
    printf("soc_err_proc : %d\n\r", err);
    if ( RHINO_NO_MEM == err )
    {
        /* while mem not enought, reboot */
        hal_reboot();
    }
}

krhino_err_proc_t g_err_proc = soc_err_proc;

