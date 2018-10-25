/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "app_entry.h"


int main(int argc, char **argv)
{
    app_main_paras_t paras;
    paras.argc = argc;
    paras.argv = argv; 
    
    linkkit_main((void *)&paras);
    return 0;
}
