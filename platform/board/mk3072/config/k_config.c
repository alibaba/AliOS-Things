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

#if (RHINO_CONFIG_INTRPT_STACK_OVF_CHECK > 0)
void soc_intrpt_stack_ovf_check(void)
{
}
#endif

#if (RHINO_CONFIG_MM_TLF > 0)

#if defined (__CC_ARM) /* Keil / armcc */
#define HEAP_BUFFER_SIZE 1024*120
uint8_t g_heap_buf[HEAP_BUFFER_SIZE];
k_mm_region_t g_mm_region[] = {{g_heap_buf, HEAP_BUFFER_SIZE}};
#elif defined (__ICCARM__)/* IAR */
#define HEAP_BUFFER_SIZE 1024*120
uint8_t g_heap_buf[HEAP_BUFFER_SIZE];
k_mm_region_t g_mm_region[] = {{g_heap_buf, HEAP_BUFFER_SIZE}};
#else /* GCC */
extern void         *heap_start;
extern void         *heap_end;
extern void         *heap_len;
/* heap_start and heap_len is set by linkscript(*.ld) */
k_mm_region_t g_mm_region[] = {{(uint8_t*)&heap_start,(size_t)&heap_len}};
#endif

int           g_region_num  = sizeof(g_mm_region)/sizeof(k_mm_region_t);

#endif

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
    void    *cur, *end, *start;
    int stack_size;
    int      i=0;
    int     *p;
    ktask_t * cur_task;

    cur_task = krhino_cur_task_get();
    start = cur_task->task_stack_base;
    stack_size = cur_task->stack_size;
    end   = start + stack_size*4;
    cur = (void *)soc_get_cur_sp();
    p = (int*)cur;
    while(p < (int*)end) {
        if(i%4==0) {
            printf("\r\n%08lx:",(uint32_t)p);
        }
        printf("%08x ", *p);
        i++;
        p++;
    }
    printf("=============,task_name:%s,start:%x,stack:%x,cur:%x\r\n", cur_task->task_name, (unsigned int)start, stack_size, (unsigned int)cur);
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

#if (RHINO_CONFIG_USER_HOOK > 0)

/**
 * This function will provide init hook
 */
void krhino_init_hook(void)
{
    return;
}

/**
 * This function will provide system start hook
 */
void krhino_start_hook(void)
{
    return;
}


/**
 * This function will provide task create hook
 * @param[in]  task  pointer to the task
 */
void krhino_task_create_hook(ktask_t *task)
{
    return;
}


/**
 * This function will provide task delete hook
 * @param[in]  task  pointer to the task
 */
void krhino_task_del_hook(ktask_t *task, res_free_t *arg)
{
    return;
}


/**
 * This function will provide task abort hook
 * @param[in]  task  pointer to the task
 */
void krhino_task_abort_hook(ktask_t *task)
{
    return;
}


/**
 * This function will provide task switch hook
 */
void krhino_task_switch_hook(ktask_t *orgin, ktask_t *dest)
{
    return;
}


/**
 * This function will provide system tick hook
 */
void krhino_tick_hook(void)
{
    return;
}

/**
 * This function will provide idle pre hook
 */
void krhino_idle_pre_hook(void)
{
    return;
}

/**
 * This function will provide idle hook
 */
void krhino_idle_hook(void)
{
    extern void pmu_idle_hook(void);
    pmu_idle_hook();
}

/**
 * This function will provide  krhino_mm_alloc hook
 */
void krhino_mm_alloc_hook(void *mem, size_t size)
{
    return;
}

#endif


krhino_err_proc_t g_err_proc = soc_err_proc;

