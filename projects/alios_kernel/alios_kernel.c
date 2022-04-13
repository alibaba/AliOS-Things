/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include "irq/irqdesc.h"
#include "board.h"
#ifdef AOS_COMP_NETMGR
#include <netmgr.h>
#endif
#include <aos/kernel.h>
#include "aos/cli.h"
#ifdef AOS_COMP_CUNIT
#include "cunit/CUnit.h"
#endif

#if (RHINO_CONFIG_USER_SPACE>0)
extern int uspace_init(void);
extern int interpret_ash_script(void);
#endif

extern void aos_load_pm(void);

#ifdef ENBALE_SQLITE_TEST
static void handle_sqlite_test_cmd(char *pwbuf, int blen, int argc, char **argv)
{
    extern void sqlite_demo(void *arg);
    aos_task_new("sqlitetest", sqlite_demo, NULL, 8192);
}
#endif

static struct cli_command ucmds[] = {
#ifdef ENBALE_SQLITE_TEST
    {"ksqlitetest", "Kernel space sqlite testes", handle_sqlite_test_cmd},
#endif
};

//< init array call, call ctors section data
extern int __ctors_start__;
extern int __ctors_end__;
extern int __dtors_start__;
extern int __dtors_end__;

static void append_start_init(void)
{
    unsigned int a = (unsigned int)&__ctors_start__;
    for (; a<(unsigned int)&__ctors_end__; a+=sizeof(void(*)()))
        (*(void (**)(void))a)();
}

static void append_end_destroy(void)
{
    unsigned int a = (unsigned int)&__dtors_start__;
    for (; a<(unsigned int)&__dtors_end__; a+=sizeof(void(*)()))
        (*(void (**)(void))a)();
}

#ifdef AOS_COMP_CUNIT
extern void cunit_run(void);
#ifdef TEST_CONFIG_GCOV_REPORT
extern int gcov_env_initialize(void);
//< gcov data flush command, call dtors section function
static void handle_gcov_cmd(char *pwbuf, int blen, int argc, char **argv)
{
    append_end_destroy();
}

static struct cli_command gcov_flush_cmd = {"gcov_flush", "flush gcov data to file", handle_gcov_cmd};


void unit_test_task(void *arg)
{
    int ret;
    ret = gcov_env_initialize();
    if (ret < 0)
    {
        printf("gcov_env_initialize failed!!\r\n");
        return;
    }

    cunit_run();
    printf("gcov case finish !!\r\n");

    return;
}
#else
static void handle_cunit_cmd(char *pwbuf, int blen, int argc, char **argv)
{
    cunit_run();
}
static struct cli_command test_cmd = {"testcase_run", "running unit kspace testcase", handle_cunit_cmd};
#endif

#endif //< AOS_COMP_CUNIT

void testirq()
{
    printf("kernel testirq\r\n");
}

int application_start(int argc, char *argv[])
{
    append_start_init();
    printf("nano entry here!\r\n");
    aos_cli_register_commands(ucmds, sizeof(ucmds)/sizeof(ucmds[0]));


#ifdef AOS_COMP_NETMGR
#if AOS_COMP_NETMGR_KSPACE_RUNABLE
    event_service_init(NULL);   //netmgr event replay on event_service
    netmgr_service_init();
#endif  //AOS_COMP_NETMGR_KSPACE_RUNABLE
#endif // AOS_COMP_NETMGR

#if (RHINO_CONFIG_USER_SPACE>0)
    int ret;
    ret = uspace_init();
    if (ret) {
        printf("uspace_init fail\r\n, ret %d\r\n", ret);
    }

    ret = interpret_ash_script();
    if (ret) {
        printf("interpret_ash_script return %d\r\n", ret);
    }
#endif

#ifdef AOS_COMP_CUNIT
#ifdef TEST_CONFIG_GCOV_REPORT
    aos_cli_register_command((const struct cli_command *)&gcov_flush_cmd);
    aos_task_new("unit_test", unit_test_task, NULL, 1024*1024);
#else
    aos_cli_register_command((const struct cli_command *)&test_cmd);
#endif //TEST_CONFIG_GCOV_REPORT
#endif //AOS_COMP_CUNIT

    aos_load_pm();

    printf("enter sleep\r\n");
    while(1) {
        aos_msleep(5000);
    };

    append_end_destroy();
}
