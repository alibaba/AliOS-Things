/*
* Copyright (C) 2015-2020 Alibaba Group Holding Limited
*/

#ifndef __PAGE_ENTRY_H
#define __PAGE_ENTRY_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "infra_list.h"

typedef struct {
    /* options object */
    int object;
    /* ref of globalData */
    int data;
    /* ref of onShow() */
    int on_show;
    /* ref of onUpdate() */
    int on_update;
    /* ref of onExit() */
    int on_exit;
}page_options_t;

typedef struct page_entry
{
    char *page;
    page_options_t *options;
    dlist_t node;
}page_entry_t;

extern void page_list_init(void);
extern void page_list_add(const char *route);
extern void page_list_dump(void);
extern void page_list_free(void);
extern void page_entry_register(void);
extern void page_entry(page_options_t *options);

#endif /* __PAGE_ENTRY_H */