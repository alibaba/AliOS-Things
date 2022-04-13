/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef _DECHUNK_H_
#define _DECHUNK_H_

#include <stdio.h>


#define DCE_OK              0
#define DCE_ISRUNNING       1
#define DCE_FORMAT          2
#define DCE_ARGUMENT        3
#define DCE_MEM             4
#define DCE_LOCK            5


int dechunk_init();
int dechunk(void *input, size_t inlen);
int dechunk_getbuff(void **buff, size_t *buf_size);
int dechunk_free();


void *memstr(void *src, size_t src_len, char *sub);

#endif // _DECHUNK_H_

