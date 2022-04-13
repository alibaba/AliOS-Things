/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include "k_api.h"
#include "aos/cli.h"
#include "ulog/ulog.h"
#include "aos/kernel.h"

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

static void uprint_cmd(int argc, char **argv)
{
    printk("\r\nbegin test uspace printk\r\n");
    print_test(printk);
    printk("printk : hello world ! yes we can\r\n");
    printk("\r\nprintk test end\r\n");

    printf("\r\nbegin test uspace printf\r\n");
    print_test(printf);
    printf("printf : hello world ! yes we can\r\n");
    printf("\r\nprintf test end\r\n");
}
ALIOS_CLI_CMD_REGISTER(uprint_cmd, uprint_t, Uspace Print Test)


/* test for ulog encoder to fs*/
static void ulogtest_cmd(int argc, char **argv)
{
    static int cnt = 0;

    LOGE("AOS", "hello alibaba : %d\n", cnt++);
}
ALIOS_CLI_CMD_REGISTER(ulogtest_cmd, ulogtest, Uspace ulog test)

/* test for panic trigger*/
static void panic_trigger(int argc, char **argv)
{
    __asm__ __volatile__("udf":::"memory");
}
ALIOS_CLI_CMD_REGISTER(panic_trigger, upanic, Uspace trigger udf panic)