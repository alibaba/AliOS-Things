/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */



#include "sdk-testsuites_internal.h"
#include "cut.h"

int main(int argc, char *argv[])
{
    ADD_SUITE(HAL_OS);

    cut_main(argc, argv);

    return 0;
}
