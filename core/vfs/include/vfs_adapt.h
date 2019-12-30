/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef VFS_ADAPT_H
#define VFS_ADAPT_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Create OS lock
 *
 * @return the pointer of the lock
 *
 */
void *vfs_lock_create(void);

/**
 * @brief Free OS lock
 *
 * @param[in] lock pointer to the os lock
 *
 * @return 0 on success, negative error on failure
 *
 */
int32_t vfs_lock_free(void *lock);

/**
 * @brief Lock the os lock
 *
 * @param[in] lock pointer to the os lock
 *
 * @return 0 on success, negative error on failure
 *
 */
int32_t vfs_lock(void *lock);

/**
 * @brief Unlock the os lock
 *
 * @param[in] lock pointer to the os lock
 *
 * @return 0 on success, negative error on failure
 *
 */
int32_t vfs_unlock(void *lock);

/**
 * @brief wrapper of MM allocation
 *
 * @param[in]  size size of the mem to alloc
 *
 * @return NULL is error, other is memory address
 *
 */
void *vfs_malloc(uint32_t size);

/**
 * @brief wrapper of MM free
 *
 * @param[in] ptr address point of the mem
 *
 */
void vfs_free(void *ptr);

#ifdef __cplusplus
}
#endif

#endif /* VFS_ADAPT_H */
