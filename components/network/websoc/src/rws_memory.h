/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef __RWS_MEMORY_H__
#define __RWS_MEMORY_H__ 1

#include <stdio.h>
#include <stdlib.h>

// size > 0 => malloc
void * rws_malloc(const size_t size);

// size > 0 => malloc
void * rws_malloc_zero(const size_t size);

void rws_free(void * mem);

void rws_free_clean(void ** mem);

#endif
