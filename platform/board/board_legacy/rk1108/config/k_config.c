/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include <assert.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <execinfo.h>

#define _linux_backtrace_depth 10


#if (RHINO_CONFIG_USER_HOOK > 0)
#if (RHINO_CONFIG_CPU_NUM > 1)
extern volatile uint64_t g_cpu_flag;
#endif
void krhino_idle_pre_hook(void)
{
    #if (RHINO_CONFIG_CPU_NUM > 1)
    CPSR_ALLOC();
    uint8_t cpu;

    RHINO_CPU_INTRPT_DISABLE();
    cpu = cpu_cur_get();
    g_cpu_flag |= (1 << cpu);
    RHINO_CPU_INTRPT_ENABLE();
    #endif
}

void krhino_idle_hook(void)
{
    cpu_idle_hook();
}

void krhino_init_hook(void)
{
#if (RHINO_CONFIG_HW_COUNT > 0)
    soc_hw_timer_init();
#endif
    cpu_init_hook();
}
#endif

void krhino_start_hook(void)
{
#if (RHINO_CONFIG_SYS_STATS > 0)
    krhino_task_sched_stats_reset();
#endif
    cpu_start_hook();
}

void krhino_task_create_hook(ktask_t *task)
{
    cpu_task_create_hook(task);
}

void krhino_task_del_hook(ktask_t *task, res_free_t *arg)
{
    cpu_task_del_hook(task, arg);
}

void krhino_task_switch_hook(ktask_t *orgin, ktask_t *dest)
{
    (void)orgin;
    (void)dest;
}

void krhino_tick_hook(void)
{
}

void krhino_task_abort_hook(ktask_t *task)
{
    (void)task;
}

void krhino_mm_alloc_hook(void *mem, size_t size)
{
    (void)mem;
    (void)size;
}


#if (RHINO_CONFIG_HW_COUNT > 0)
struct timespec time_start;
void soc_hw_timer_init(void)
{
    clock_gettime (CLOCK_REALTIME, &time_start);
}

hr_timer_t soc_hr_hw_cnt_get(void)
{
    return soc_lr_hw_cnt_get();
}

lr_timer_t soc_lr_hw_cnt_get(void)
{
    lr_timer_t ret;
    int64_t t1;
    //int64_t t2;

    struct timespec time1 = {0, 0};
    clock_gettime(CLOCK_REALTIME, &time1);

    t1 = time1.tv_sec - time_start.tv_sec;

    ret = t1  * 1000000000 -  time_start.tv_nsec + time1.tv_nsec;
    return ret;
}
#endif

#ifdef GCOV_ENABLE
#define       SYS_DYN_POOL_SIZE (1024 *1024)
#else
#define       SYS_DYN_POOL_SIZE (512 *1024)
#endif /* GCOV_ENABLE */

#if (RHINO_CONFIG_CPU_NUM > 1)
#undef        SYS_DYN_POOL_SIZE
#define       SYS_DYN_POOL_SIZE (1024 *1024)
#endif

size_t        sys_pool_start[SYS_DYN_POOL_SIZE / sizeof(size_t)];
k_mm_region_t g_mm_region[] = {{(uint8_t*)&sys_pool_start,SYS_DYN_POOL_SIZE}};
int           g_region_num  = sizeof(g_mm_region)/sizeof(k_mm_region_t);

void _linux_backtrace()
{
   void   *array[_linux_backtrace_depth];
   size_t size;
   char **strings;
   size_t e;

   size = backtrace (array,_linux_backtrace_depth);
   strings = (char **)backtrace_symbols (array, size);

   fprintf(stderr, "Stack trace:\n");
   for (e = 0; e < size; e++) {
       fprintf(stderr, "%d %s \n",e,strings[e]);
   }

   free (strings);
}

void soc_err_proc(kstat_t err)
{
    printf("kernel panic,err %d!\n",err);
    _linux_backtrace();
    if(err == RHINO_TASK_STACK_OVF){
        assert(0);
    }
}

krhino_err_proc_t g_err_proc = soc_err_proc;

