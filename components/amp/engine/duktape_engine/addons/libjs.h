/*
* Copyright (C) 2015-2020 Alibaba Group Holding Limited
*/
#ifndef __LIBJS_H
#define __LIBJS_H

#include <stdio.h>
#include <string.h>

#ifdef JSE_HIGHLEVEL_JSAPI

typedef struct {
    char *name;
    char *content;
}libjs_entry_t;

extern libjs_entry_t LIBJS_ENTRIES[];
extern int libjs_num;

#endif /* JSE_HIGHLEVEL_JSAPI */
#endif /* __LIBJS_H */