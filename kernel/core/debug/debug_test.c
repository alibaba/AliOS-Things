/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <assert.h>
#include "k_api.h"
#include "aos/cli.h"
#include "ulog/ulog.h"
#include "aos/kernel.h"
#include "k_arch.h"
#include "k_proc.h"
#include "debug_api.h"

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
static void print_irq_disable_test(int argc, char **argv)
{
    CPSR_ALLOC();

    printk("\r\nbegin test printk in irq disable\r\n");
    print_test(printk);
    RHINO_CPU_INTRPT_DISABLE();
    printk("printk : hello world ! yes we can\r\n");
    RHINO_CPU_INTRPT_ENABLE();
    printk("\r\nprintk test end\r\n");

    printf("\r\nbegin test printf in irq disable\r\n");
    print_test(printf);
    RHINO_CPU_INTRPT_DISABLE();
    printf("printf : hello world ! yes we can\r\n");
    RHINO_CPU_INTRPT_ENABLE();
    printf("\r\nprintf test end\r\n");
}
ALIOS_CLI_CMD_REGISTER(print_irq_disable_test, print_t, Console Cmd Print Test)


static void sleep_irq_disable_test(int argc, char **argv)
{
    CPSR_ALLOC();

    printk("begin test1 sleep in irq disable\r\n");
    printf("begin test2 sleep in irq disable\r\n");

    RHINO_CPU_INTRPT_DISABLE();
    aos_msleep(1000);
    RHINO_CPU_INTRPT_ENABLE();

    printk("sleep test1 end\r\n");
    printf("sleep test2 end\r\n");
}
ALIOS_CLI_CMD_REGISTER(sleep_irq_disable_test, sleep_t, Console Cmd Sleep Test)

/* test for ulog encoder to fs*/
static void ulog_encode_fs_test(int argc, char **argv)
{
    static int cnt = 0;

    LOGE("AOS", "hello alibaba : %d\n", cnt++);
}
ALIOS_CLI_CMD_REGISTER(ulog_encode_fs_test, uet, Console ulog encode fs test)

/* test for panic trigger in task*/
static void panic_trigger(int argc, char **argv)
{
    __asm__ __volatile__("udf":::"memory");
}
ALIOS_CLI_CMD_REGISTER(panic_trigger, panic, Console trigger system panic)

/* test for fatal error trigger in task*/
static void fatal_error_trigger(int argc, char **argv)
{
    assert(0);
}
ALIOS_CLI_CMD_REGISTER(fatal_error_trigger, assert, Console trigger system fatal error)


static void hung_trigger(int argc, char **argv)
{
    CPSR_ALLOC();

    RHINO_CPU_INTRPT_DISABLE();
    (void)cpsr;
    aos_msleep(1000);
}
ALIOS_CLI_CMD_REGISTER(hung_trigger, hung, Console trigger system hung)

__attribute__((weak))  void alios_debug_pc_show(int argc, char **argv)
{
    return;
}
ALIOS_CLI_CMD_REGISTER(alios_debug_pc_show, pcshow, Console show pc addr)
