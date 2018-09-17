/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "app_entry.h"

void __attribute__((weak)) set_iotx_info()
{

}

int main(int argc, char **argv)
{
    app_main_paras_t paras;
    paras.argc = argc;
    paras.argv = argv;

#ifdef WIFI_AWSS_ENABLED
    set_iotx_info();
#endif  
    linkkit_main((void *)&paras);
    return 0;
}
