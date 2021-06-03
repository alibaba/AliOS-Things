/*
* Copyright (C) 2015-2020 Alibaba Group Holding Limited
*/

#ifndef __APP_ENTRY_H
#define __APP_ENTRY_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    /* options object */
    int object;
    /* ref of globalData */
    int global_data;
    /* ref of onLaunch() */
    int on_launch;
    /* ref of onError() */
    int on_error;
    /* ref of onExit() */
    int on_exit;
}app_options_t;

extern void app_entry_register(void);
extern void app_entry(void* data);

#endif /* __APP_ENTRY_H */