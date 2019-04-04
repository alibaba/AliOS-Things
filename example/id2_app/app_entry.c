/*
 * Copyright (C) 2018 Alibaba Group Holding Limited
 */
#include <aos/aos.h>

extern void id2_client_test();

int application_start(int argc, char *argv[])
{
    LOG("id2 app begin\n");
    id2_client_test();
    LOG("id2 app end\n");
    return 0;
}

