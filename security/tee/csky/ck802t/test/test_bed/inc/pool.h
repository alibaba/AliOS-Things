/**
 * Copyright (C) 2015 The YunOS Project. All rights reserved.
 */

/*
 * Filename     : tee_pool.h
 * Author       : Jun Lin
 * Date Created : 02/12/2015
 * Description  : memory phys pool functions
 *
 */

#ifndef _POOL_H_
#define _POOL_H_

#include "tee_types.h"

extern void *pool_create(void *start, uint32_t size, uint32_t align);
extern void pool_delete(void *handle);
extern void *pool_alloc(void *handle, uint32_t size);
extern void pool_free(void *handle, void *p);

extern void kmem_init(void);
extern void *kmalloc(size_t size, int flags);
extern void kfree(const void *ptr);

#endif /*_POOL_H_ */
