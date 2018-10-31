// TODO copyright

#include <stdio.h>
#include <assert.h>
#include <sys/time.h>
#include <k_api.h>

#if defined (__CC_ARM)
#error "memory region not implemented for __CC_ARM"
#elif defined (__ICCARM__)
#error "memory region not implemented for __ICCARM__"
#else

extern uint8_t __StackTop[];
extern uint8_t __HeapLimit[];

k_mm_region_t g_mm_region[1];
int           g_region_num = 1;

void aos_heap_set(void)
{
    g_mm_region[0].start = (uint8_t*)__HeapLimit;
    g_mm_region[0].len = (uint8_t*)__StackTop
                         - (uint8_t*)__HeapLimit
                         - RHINO_CONFIG_SYSTEM_STACK_SIZE;
}
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
    printf("panic %d!\r\n", err);
    soc_print_stack();
    assert(0);
}


krhino_err_proc_t g_err_proc = soc_err_proc;
