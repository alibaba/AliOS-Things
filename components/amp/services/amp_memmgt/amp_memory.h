/*!
 * @file amp_memory.h
 *
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */

#ifndef _AMP_MEMORY_H_
#define _AMP_MEMORY_H_

void amp_memory_init();
void *amp_malloc(unsigned int size);
void amp_free(void *ptr);
void *amp_realloc(void *ptr, unsigned int size);
unsigned int amp_malloc_usable_size(void *ptr);
void amp_memmgt_mem_show_rec();

#endif

