/*
* Copyright (C) 2015-2020 Alibaba Group Holding Limited
*/

#ifndef __PAGE_ENTRY_H
#define __PAGE_ENTRY_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include "infra_list.h"
#include "render.h"

extern void page_list_init(void);
extern void page_list_add(const char *route);
extern void page_list_dump(void);
extern void page_list_free(void);
extern void page_entry_register(void);
extern void page_entry(void *para);
extern void page_exit(void *para);
extern void page_update(void *para);

#endif /* __PAGE_ENTRY_H */
