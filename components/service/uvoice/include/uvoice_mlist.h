/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 *
 */
 
#ifndef __UVOICE_MLIST_H__
#define __UVOICE_MLIST_H__


int mlist_source_show(void);
int mlist_source_scan(void);
int mlist_source_get(int index, char *path, int len);
int mlist_source_del(int index);
int mlist_index_get(int *index);
int mlist_index_set(int index);


#endif /* __UVOICE_MLIST_H__ */

