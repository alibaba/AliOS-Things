/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef RAMFS_ADAPT_H
#define RAMFS_ADAPT_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief wrapper of MM allocation
 *
 * @param[in] size size of the mem to alloc
 *
 * @return NUll is error, other is memory address
 */
void *ramfs_mm_alloc(uint32_t size);

/**
 * @brief wrapper of MM free
 *
 * @param[in] ptr address point of the mem
 */
void ramfs_mm_free(void *ptr);

/**
 * @brief wrapper of MM realloc
 *
 * @param[in] oldmem  pointer to the original memory
 * @param[in] newsize the new size to realloc
 *
 * @return NULL is error, other is new memory address
 */
void *ramfs_mm_realloc(void *oldmem, uint32_t newsize);

#ifdef __cplusplus
}
#endif

#endif /* RAMFS_ADAPT_H */

