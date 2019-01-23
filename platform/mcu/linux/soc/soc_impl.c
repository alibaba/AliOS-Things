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

