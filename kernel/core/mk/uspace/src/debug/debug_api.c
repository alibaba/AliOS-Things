#include <stdio.h>
#include "k_api.h"
#include "k_compiler.h"
#include "debug_api.h"

void debug_cpu_stop(void);

void debug_backtrace_now()
{
    char *PC = RHINO_GET_PC();
    int  *SP = RHINO_GET_SP();

    backtrace_now(PC, SP, BACKTRACE_USPACE, printk);
}

void k_err_proc_debug(kstat_t err, char *file, int line)
{
    /* stop the othre core */
    debug_cpu_stop();

    printk("system error : file = %s, line = %d\r\n", file, line);
    __asm__ __volatile__("udf":::"memory");
}
