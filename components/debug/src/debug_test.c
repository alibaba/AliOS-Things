/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */
#if AOS_COMP_CLI
#include <stdio.h>
#include <assert.h>
#include "debug_api.h"
#include "aos/debug.h"
#include "aos/cli.h"
#ifdef AOS_COMP_ULOG
#include "ulog/ulog.h"
#endif

#define MAX_32 ((long)(~0UL))
#define MAX_64 ((long long)(~0ULL))

typedef int (*print_func)(const char *fmt, ...);

static void print_test(int (*print_func)(const char *fmt, ...))
{
    int a = 1234;
    unsigned long long b = 12345678;
    float c = 3.14;
    double d = 5.6789;

    print_func("int : %d  max_32 : 0x%X\r\n", a, MAX_32);
    print_func("ll : %lld max_64 : 0x%llX\r\n", b, MAX_64);
    print_func("float : %f\r\n", c);
    print_func("double : %lf\r\n", d);
}

/*test for printf when irq disable, which mostly test uart_send_dirver of vendor*/
static void print_test_cmd(int argc, char **argv)
{
    CPSR_ALLOC();

    printk("\r\nbegin test printk\r\n");
    print_test(printk);
    RHINO_CPU_INTRPT_DISABLE();
    printk("printk : hello world ! yes we can\r\n");
    RHINO_CPU_INTRPT_ENABLE();
    printk("\r\nprintk test end\r\n");

    printf("\r\nbegin test printf\r\n");
    print_test(printf);
    RHINO_CPU_INTRPT_DISABLE();
    printf("printf : hello world ! yes we can\r\n");
    RHINO_CPU_INTRPT_ENABLE();
    printf("\r\nprintf test end\r\n");
}
ALIOS_CLI_CMD_REGISTER(print_test_cmd, print_t, Console Cmd Print Test)

/* test for ulog encoder to fs*/
#ifdef AOS_COMP_ULOG
static void ulog_encode_fs_test(int argc, char **argv)
{
    static int cnt = 0;

    LOGE("AOS", "hello alibaba : %d\n", cnt++);
}
ALIOS_CLI_CMD_REGISTER(ulog_encode_fs_test, uet, Console ulog encode fs test)
#endif

/* test for panic trigger in task*/
static void panic_trigger(int argc, char **argv)
{
#ifdef OS_UDF
    OS_UDF();
#endif
}
ALIOS_CLI_CMD_REGISTER(panic_trigger, panic, Console trigger system panic)

/* test for fatal error trigger in task*/
static void fatal_error_trigger(int argc, char **argv)
{
    assert(0);
}
ALIOS_CLI_CMD_REGISTER(fatal_error_trigger, assert, Trigger assert)


static void hung_trigger(int argc, char **argv)
{
    CPSR_ALLOC();

    RHINO_CPU_INTRPT_DISABLE();
    (void)cpsr;
    aos_msleep(1000);
}
ALIOS_CLI_CMD_REGISTER(hung_trigger, hung, Trigger system hung)

__attribute__((weak))  void alios_debug_pc_show(int argc, char **argv)
{
    return;
}
ALIOS_CLI_CMD_REGISTER(alios_debug_pc_show, pcshow, Show pc addr region)

#endif /* AOS_COMP_CLI */
