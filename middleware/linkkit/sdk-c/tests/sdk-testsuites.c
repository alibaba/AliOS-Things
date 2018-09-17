/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */



#include "sdk-testsuites_internal.h"
#include "cut.h"

int main(int argc, char *argv[])
{
    ADD_SUITE(HAL_OS);
#ifdef SDK_ENHANCE
    ADD_SUITE(DM_TEST);
#endif
#ifdef HTTP2_COMM_ENABLED
    ADD_SUITE(HTTP2);
#endif	
    cut_main(argc, argv);

    return 0;
}
