/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */
#ifndef __APP_ENTRY_H__
#define __APP_ENTRY_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

typedef struct {
    int argc;
    char **argv;
}app_main_paras_t;

int linkkit_main(void *paras);
#endif