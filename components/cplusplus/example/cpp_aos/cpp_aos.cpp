/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */

#include "aos/kernel.h"
#include <aos_cpp.h>
#include <iostream>
#include <stdio.h>

#if AOS_COMP_CLI
#include "aos/cli.h"
#endif

using namespace std;

void test_inheritance(void);
void test_thread(void);
void test_timer(void);
void test_queue(void);
void test_semaphore(void);
void test_mutex(void);
void test_workqueue(void);

static void cpp_aos_example(int argc, char **argv)
{
    printf("hello world C!\n");
    cout << "hello world C++!" << endl;

    test_inheritance();
    test_thread();
    test_timer();
    test_queue();
    test_semaphore();
    test_mutex();
    test_workqueue();

    return;
}

#if AOS_COMP_CLI
/* reg args: fun, cmd, description*/
ALIOS_CLI_CMD_REGISTER(cpp_aos_example, cpp_aos, basic aos cpp api example)
#endif
