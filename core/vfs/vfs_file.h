/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef VFS_FILE_H
#define VFS_FILE_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Get the file descriptor by file structure
 *
 * @param[in] file pointer the file structure
 *
 * @return the file descriptor
 *
 */
int32_t vfs_fd_get(vfs_file_t *file);

/**
 * @brief Get the file structure by file descriptor
 *
 * @param[in] fd the file descriptor
 *
 * @return the pointer of the file structure
 *
 */
vfs_file_t *vfs_file_get(int32_t fd);

/**
 * @brief Create a new file structure
 *
 * @param[in] node pointer to the inode
 *
 * @return the pointer of the file structure
 *
 */
vfs_file_t *vfs_file_new(vfs_inode_t *node);

/**
 * @brief Delete the file structure
 *
 * @param[in] file pointer to the file structure
 *
 * @return none
 */
void vfs_file_del(vfs_file_t *file);

/**
 * @brief Mark fd as opened
 *
 * @param[in] fd the file descriptor
 *
 * @return On success return 0
 *         if the fd is invalid return -1
 *         if the fd has already been marked return 1
 */
int32_t vfs_fd_mark_open(int32_t fd);

/**
 * @brief mark fd as closed
 *
 * @param[in] fd the file descriptor
 *
 * @return On success return 0
 *         if the fd is invalid return -1
 *         if the fd has already been marked return 1
 */
int32_t vfs_fd_mark_close(int32_t fd);

/**
 * @brief query wether the fd is opened
 *
 * @param[in] fd the file descriptor
 *
 * @return if the fd is opened, return 1
 *         else return 0
 */
int32_t vfs_fd_is_open(int32_t fd);

#ifdef __cplusplus
}
#endif

#endif /* VFS_FILE_H */
