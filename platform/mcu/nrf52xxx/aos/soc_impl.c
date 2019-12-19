/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include <assert.h>
#include <stdio.h>
#include <sys/time.h>

#include "aos/hal/uart.h"

extern void hal_reboot(void);

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

#if defined (__CC_ARM) /* Keil / armcc */
#define HEAP_BUFFER_SIZE 1024*20
uint8_t g_heap_buf[HEAP_BUFFER_SIZE];
k_mm_region_t g_mm_region[] = {{g_heap_buf, HEAP_BUFFER_SIZE}};
#elif defined (__ICCARM__)/* IAR */
#define HEAP_BUFFER_SIZE 1024*20
uint8_t g_heap_buf[HEAP_BUFFER_SIZE];
k_mm_region_t g_mm_region[] = {{g_heap_buf, HEAP_BUFFER_SIZE}};
#else /* GCC */
#if 1
extern void         *heap_start;
extern void         *heap_end;
extern void         *heap_len;
/* heap_start and heap_len is set by linkscript(*.ld) */
k_mm_region_t g_mm_region[] = {{(uint8_t *) &heap_start, (size_t) &heap_len}};
#endif

/*
#define HEAP_BUFFER_SIZE 1024*50
uint8_t g_heap_buf[HEAP_BUFFER_SIZE];
k_mm_region_t g_mm_region[] = {{g_heap_buf, HEAP_BUFFER_SIZE}};
*/
#endif

int           g_region_num  = sizeof(g_mm_region) / sizeof(k_mm_region_t);

#endif

void soc_err_proc(kstat_t err)
{
    (void)err;
    hal_reboot();
}

krhino_err_proc_t g_err_proc = soc_err_proc;

