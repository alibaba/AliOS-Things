/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <aos/aos.h>

extern test_case_task_entry(void *arg); 

int application_start(int argc, char *argv[])
{
    LOG("kernel test started.");

    test_case_task_entry(argv);

    return 0;
}

