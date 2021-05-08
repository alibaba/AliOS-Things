/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */
#include "aos/kernel.h"
#include <aos_cpp.h>
#include <stdio.h>
#if AOS_COMP_CLI
#include "aos/cli.h"
#endif

void thread_test(void);
void conditon_varialbe_test(void);
void static_singleton_test(void);
void shared_ptr_test(void);
void lamda_test(void);
void tuple_test(void);
void rvalue_test(void);
void atomic_test(void);
void timed_mutex_test(void);
void rtti_test(void);

void cpp_standard_example(int argc, char *argv[])
{
    printf("c++ feature test!\r\n");

    timed_mutex_test();

    thread_test();
    conditon_varialbe_test();
    static_singleton_test();
    shared_ptr_test();
    lamda_test();
    tuple_test();
    rvalue_test();
    atomic_test();
}

#if AOS_COMP_CLI
/* reg args: fun, cmd, description*/
ALIOS_CLI_CMD_REGISTER(cpp_standard_example,
                       cpp_stand,
                       cpp standard feature example)
#endif
