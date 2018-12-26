/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef RAMFS_API_H
#define RAMFS_API_H

#include "ramfs_err.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    uint32_t st_mode;
    uint32_t st_size;
    uint8_t  is_dir;
} ramfs_stat_t;

typedef enum {
    RAMFS_MODE_WR = 0x01,
    RAMFS_MODE_RD = 0x02,
} ramfs_mode_e;

/**
 * @brief Initialize ramfs module
 *
 * @return None
 */
void ramfs_init(void);

/**
 * @brief Give the state of the ramfs
 *
 * @return 0 if the ramfs is initialized and can be used else false
 */
int32_t ramfs_ready(void);

/**
 * @brief Open a file in ramfs
 *
 * @param[out] fp   pointer to a ramfs file object
 * @param[in]  fn   name of the file. There are no directories. e.g. "a.txt"
 * @param[in]  mode file open mode (e.g. RAMFS_MODE_WR | RAMFS_MODE_RD)
 *
 * @return 0 on success, otherwise will be failed
 */
int32_t ramfs_open(void *fp, const char* fn, uint32_t mode);

/**
 * @brief Create a file with a constant data
 *
 * @param[in] fn   name of the file (directories are not supported)
 * @param[in] data pointer to a constant data
 * @param[in] len  length of the constant data
 *
 * @return 0 on success, otherwise will be failed
 */
int32_t ramfs_create_const(const char *fn, const void *data, uint32_t len);

/**
 * @brief Close an opened file
 *
 * @param[in] fp pointer to a ramfs file object
 *
 * @return 0 on success, otherwise will be failed
 */
int32_t ramfs_close(void *fp);

/**
 * @brief Remove a file, the file should not be opened
 *
 * @param[in] fn pointer to a ramfs file object
 *
 * @return 0 on success, otherwise will be failed
 */
int32_t ramfs_remove(const char *fn);

/**
 * @brief Remove a file, the file should not be opened
 *
 * @param[in] old pointer to old name
 * @param[in] new pointer to new name
 *
 * @return 0 on success, otherwise will be failed
 */
int32_t ramfs_rename(const char *old, const char *new);

/**
 * @brief Read data from an opened file
 *
 * @param[in]  fp  pointer to a ramfs file object
 * @param[out] buf poiner to a memory block where to store the read data
 * @param[in]  btr number of bytes to read
 * @param[out] br  the real number of read bytes(Byte Read)
 *
 * @return 0 on success, otherwise will be failed
 */
int32_t ramfs_read(void *fp, void *buf, uint32_t btr, uint32_t *br);

/**
 * @brief Write data to an opened file
 *
 * @param[in]  fp  pointer to a ramfs file object
 * @param[in]  buf pointer to a memory block which content will be written
 * @param[in]  btw number of bytes to write
 * @param[out] bw  the real number to write bytes(Byte Written)
 *
 * @return 0 on success, otherwise will be failed
 */
int32_t ramfs_write(void *fp, const void *buf, uint32_t btw, uint32_t *bw);

/**
 * @brief Set the read/write pointer. Also expand the file size if necessary.
 *
 * @param[in] fp  pointer to a ramfs file object (opened with ramfs_open)
 * @param[in] pos the new position of read/write pointer
 *
 * @return 0 on success, otherwise will be failed
 */
int32_t ramfs_seek(void *fp, uint32_t pos);

/**
 * @brief Give the position of the read/write pointer
 *
 * @param[in]  fp  pointer to a ramfs file object (opened with ramfs_open)
 * @param[out] pos pointer to store the result
 *
 * @return 0 on success, otherwise will be failed
 */
int32_t ramfs_tell(void *fp, uint32_t *pos);

/**
 * @brief Truncate the file size to the current position of the read/write pointer
 *
 * @param[in] fp pointer to a ramfs file object (opened with ramfs_open)
 *
 * @return 0 on success, otherwise will be failed
 */
int32_t ramfs_trunc(void *fp);

/**
 * @brief Give the size of the file in bytes
 *
 * @param[in]  fp   pointer to a ramfs file object
 * @param[out] size pointer to store the size
 *
 * @return 0 on success, otherwise will be failed
 */
int32_t ramfs_size(void *fp, uint32_t *size);

/**
 * @brief Get access information
 *
 * @param[in] path the path of the file
 * @param[in] mode the information to get
 *
 * @return 0 on success, otherwise will be failed
 */
int32_t ramfs_access(const char *path, int32_t mode);

/**
 * @brief Create a directory
 *
 * @param[in] path the path of the directory
 *
 * @return 0 on success, otherwise will be failed
 */
int32_t ramfs_mkdir(const char *path);

/**
 * @brief Open a directory in ramfs
 *
 * @param[out] dp   pointer to a ramfs directory object
 * @param[in]  path the path of the directory
 *
 * @return 0 on success, otherwise will be failed
 */
int32_t ramfs_opendir(void *dp, const char *path);

/**
 * @brief Read the next file name under the directory
 *
 * @param[in]  dp pointer to a ramfs directory object
 * @param[out] fn pointer to buffer to store the file name
 *
 * @return 0 on success, otherwise will be failed
 */
int32_t ramfs_readdir(void *dp, char *fn);

/**
 * @brief Close the directory
 *
 * @param[in] dp pointer to a ramfs directory object
 *
 * @return 0 on success, otherwise will be failed
 */
int32_t ramfs_closedir(void *dp);

/**
 * @brief Remove a directory
 *
 * @param[in]  path the path of the directory
 *
 * @return 0 on success, otherwise will be failed
 */
int32_t ramfs_rmdir(const char *path);

/**
 * @brief Get file info
 *
 * @param[in]  path the path of the file to find info about
 * @param[out] st   the stat buffer to write to
 *
 * @return 0 on success, otherwise will be failed
 */
int32_t ramfs_stat(const char *path, ramfs_stat_t *st);

/**
 * @brief Get path info
 *
 * @param[in] name the kind of path conf to get
 *
 * @return value of path info
 */
int32_t ramfs_pathconf(int32_t name);

/**
 * @brief link path2 to path1
 *
 * @param[in] path1 the path to be linked
 * @param[in] path2 the path to link
 *
 * @return 0 on success, negative error on failure
 *
 */
int ramfs_link(const char *path1, const char *path2);

/**
 * @brief Remove a file from the filesystem
 *
 * @param[in] path the path of the file to remove
 *
 * @return 0 on success, negative error on failure
 *
 */
int ramfs_unlink(const char *path);

#ifdef __cplusplus
}
#endif

#endif /* RAMFS_API_H */

