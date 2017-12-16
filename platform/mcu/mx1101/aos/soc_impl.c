/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include <assert.h>
#include <stdio.h>

extern void         *heap_start;
extern void         *heap_len;

k_mm_region_t g_mm_region[] = {{(uint8_t*)&heap_start,(size_t)&heap_len}};
int           g_region_num  = sizeof(g_mm_region)/sizeof(k_mm_region_t);

extern int __bss_start__, __bss_end__, _sdata, _edata;

void aos_mm_leak_region_init(void)
{
    krhino_mm_leak_region_init(&__bss_start__, &__bss_end__);
    krhino_mm_leak_region_init(&_sdata, &_edata);
}

size_t soc_get_cur_sp()
{
    size_t sp = 0;
    asm volatile(
        "mov %0,sp\n"
        :"=r"(sp));
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

void soc_err_proc(kstat_t err)
{
    (void)err;
    soc_print_stack();
    assert(0);
}

krhino_err_proc_t g_err_proc = soc_err_proc;
