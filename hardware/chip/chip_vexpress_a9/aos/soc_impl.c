/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include <assert.h>
#include <stdio.h>
#include <sys/time.h>


#if (RHINO_CONFIG_HW_COUNT > 0)
void soc_hw_timer_init(void)
{
    uint32_t cntp_ctl    = 3; /* do't to gengerate interrupt */
    uint8_t  core_number = cpu_cur_get();
    if (core_number != 0) {
        asm volatile ("mcr p15, 0, %0, c14, c2, 1" ::"r"(cntp_ctl));
        asm volatile ("isb");
    }
}

hr_timer_t soc_hr_hw_cnt_get(void)
{
    uint64_t hr_timer;
    return 0;
    /*
    asm volatile ("mrrc p15, 0, %Q0, %R0, c14" :"=r"(hr_timer));
    asm volatile ("isb");
    return (hr_timer_t)hr_timer;
    */
}

lr_timer_t soc_lr_hw_cnt_get(void)
{
    uint64_t lr_timer;
    return 0;
    /*
    asm volatile ("mrrc p15, 0, %Q0, %R0, c14" :"=r"(lr_timer));
    asm volatile ("isb");
    return (lr_timer_t)lr_timer;
    */
}
#endif /* RHINO_CONFIG_HW_COUNT */

#if (RHINO_CONFIG_INTRPT_STACK_OVF_CHECK > 0)
void soc_intrpt_stack_ovf_check(void)
{
}
#endif

#if (RHINO_CONFIG_MM_TLF > 0)
#if !defined (__CC_ARM) /* Keil / armcc */
extern void         *_heap_start;
extern void         *_heap_end;
extern void         *_heap_len;


extern void         *_heap1_start;
extern void         *_heap1_len;

extern void         *_dma_start;
extern void         *_dma_len;

extern void         *_dsp_reserved_start;
extern void         *_dsp_reserved_len;
extern void         *_consys_reserved_start;
extern void         *_consys_reserved_len;

extern void         *_mmu_text_start;
extern void         *_mmu_text_len;
extern void         *_mmu_data_start;
extern void         *_mmu_data_len;
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

k_mm_region_t g_mm_region[] = {{(uint8_t *)&_heap_start,(size_t)&_heap_len},
                               {(uint8_t *)&_heap1_start,(size_t)&_heap1_len}};
k_mmu_regions_t g_memmaps[]  ={
    {0x60000000, 0x60000000,0x004f8000,MMU_ATTR_SO},//ram
    {(uintptr_t)&_mmu_text_start, (uintptr_t)&_mmu_text_start , (size_t)&_mmu_text_len, MMU_ATTR_EXEC},
    {(uintptr_t)&_mmu_data_start, (uintptr_t)&_mmu_data_start , (size_t)&_mmu_data_len, MMU_ATTR_DATA},
    {(uintptr_t)&_heap1_start, (uintptr_t)&_heap1_start , (size_t)&_heap1_len, MMU_ATTR_DATA},
    {(uintptr_t)&_dsp_reserved_start, (uintptr_t)&_dsp_reserved_start , (size_t)&_dsp_reserved_len, MMU_ATTR_NON_CACHE}, // for dsp memory
    {(uintptr_t)&_consys_reserved_start, (uintptr_t)&_consys_reserved_start , (size_t)&_consys_reserved_start, MMU_ATTR_NON_CACHE}, // for consys memory
    {(uintptr_t)&_dma_start, (uintptr_t)&_dma_start , (size_t)&_dma_len, MMU_ATTR_NON_CACHE}, // for DMA memory
    //{0x00000000, 0x00000000,0x4000000,MMU_ATTR_SO},
    {0x40000000, 0x40000000,0x4000000,MMU_ATTR_SO},
    {0x44000000, 0x44000000,0x4000000,MMU_ATTR_SO},
    {0x1E000000, 0x1E000000,0x400000,MMU_ATTR_SO},
#ifdef FEATURE_NETWORK_USPACE_VEXPRESS_TMP
    {0x4F000000, 0x4F000000,0x400000,MMU_ATTR_SO},
#else
    {0x4E000000, 0x4E000000,0x400000,MMU_ATTR_SO},
#endif
    /*****for register mapping******/
    {0x10000000, 0x10000000, 0x400000,MMU_ATTR_SO},
    {0x10400000, 0x10400000, 0x400000,MMU_ATTR_SO},
    {0x10800000, 0x10800000, 0x400000,MMU_ATTR_SO},
    {0x10c00000, 0x10c00000, 0x400000,MMU_ATTR_SO},
    {0x11000000, 0x11000000, 0x400000,MMU_ATTR_SO},
    {0x11400000, 0x11400000, 0x400000,MMU_ATTR_SO},
    {0x11800000, 0x11800000, 0x400000,MMU_ATTR_SO},
    {0x11c00000, 0x11c00000, 0x400000,MMU_ATTR_SO},
    {0x12000000, 0x12000000, 0x400000,MMU_ATTR_SO},
    {0x12400000, 0x12400000, 0x400000,MMU_ATTR_SO},
    {0x12800000, 0x12800000, 0x400000,MMU_ATTR_SO},
    {0x12c00000, 0x12c00000, 0x400000,MMU_ATTR_SO},
    {0x13000000, 0x13000000, 0x400000,MMU_ATTR_SO},
    {0x13400000, 0x13400000, 0x400000,MMU_ATTR_SO},
    {0x13800000, 0x13800000, 0x400000,MMU_ATTR_SO},
    {0x13c00000, 0x13c00000, 0x400000,MMU_ATTR_SO},
    {0x14000000, 0x14000000, 0x400000,MMU_ATTR_SO},
    {0x14400000, 0x14400000, 0x400000,MMU_ATTR_SO},
    {0x14800000, 0x14800000, 0x400000,MMU_ATTR_SO},
    {0x14c00000, 0x14c00000, 0x400000,MMU_ATTR_SO},
    {0x15000000, 0x15000000, 0x400000,MMU_ATTR_SO},
    {0x15400000, 0x15400000, 0x400000,MMU_ATTR_SO},
    {0x15800000, 0x15800000, 0x400000,MMU_ATTR_SO},
    {0x15c00000, 0x15c00000, 0x400000,MMU_ATTR_SO},
    {0x16000000, 0x16000000, 0x400000,MMU_ATTR_SO},
    {0x16400000, 0x16400000, 0x400000,MMU_ATTR_SO},
    {0x16800000, 0x16800000, 0x400000,MMU_ATTR_SO},
    {0x16c00000, 0x16c00000, 0x400000,MMU_ATTR_SO},
    {0x17000000, 0x17000000, 0x400000,MMU_ATTR_SO},
    {0x17400000, 0x17400000, 0x400000,MMU_ATTR_SO},
    {0x17800000, 0x17800000, 0x400000,MMU_ATTR_SO},
    {0x17c00000, 0x17c00000, 0x400000,MMU_ATTR_SO},
    {0x18000000, 0x18000000, 0x400000,MMU_ATTR_SO},
    {0x18400000, 0x18400000, 0x400000,MMU_ATTR_SO},
    {0x18800000, 0x18800000, 0x400000,MMU_ATTR_SO},
    {0x18c00000, 0x18c00000, 0x400000,MMU_ATTR_SO}
};

#endif
int g_region_num = sizeof(g_mm_region)/sizeof(k_mm_region_t);
int g_memmaps_size = sizeof(g_memmaps)/sizeof(g_memmaps[0]);
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
