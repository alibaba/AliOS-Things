/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef VFS_API_H
#define VFS_API_H

#define VFS_OK               0
#define VFS_ERR_NOMEM       -10000
#define VFS_ERR_INVAL       -10001
#define VFS_ERR_NOENT       -10002
#define VFS_ERR_NAMETOOLONG -10003
#define VFS_ERR_NOSYS       -10004
#define VFS_ERR_ENFILE      -10005
#define VFS_ERR_NODEV       -10006
#define VFS_ERR_LOCK        -10007
#define VFS_ERR_BUSY        -10008

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize the vfs module
 *
 * @return 0 on success, negative error on failure
 *
 */
int32_t vfs_init(void);

/**
 * @brief Open the file or device by path
 *
 * @param[in] path  the path of the file or device to open
 * @param[in] flags the mode of the open operation
 *
 * @return the new file descriptor, negative error on failure
 *
 */
int32_t vfs_open(const char *path, int32_t flags);

/**
 * @brief Close the file or device by file descriptor
 *
 * @param[in] fd the file descriptor of the file ot device
 *
 * @return 0 on success, negative error on failure
 *
 */
int32_t vfs_close(int32_t fd);

/**
 * @brief Read the contents of a file or device into a buffer
 *
 * @param[in]  fd     the file descriptor of the file or device
 * @param[out] buf    the buffer to read into
 * @param[in]  nbytes the number of bytes to read
 *
 * @return the number of bytes read, 0 at end of file, negative error on failure
 *
 */
int32_t vfs_read(int32_t fd, void *buf, uint32_t nbytes);

/**
 * @brief Write the contents of a buffer to file or device
 *
 * @param[in] fd     the file descriptor of the file or device
 * @param[in] buf    the buffer to write from
 * @param[in] nbytes the number of bytes to write
 *
 * @return the number of bytes written, negative error on failure
 *
 */
int32_t vfs_write(int32_t fd, const void *buf, uint32_t nbytes);

/**
 * @brief This is a wildcard API for sending specific commands
 *
 * @param[in] fd  the file descriptor of the file or device
 * @param[in] cmd a specific command
 * @param[in] arg argument to the command, interpreted according to the cmd
 *
 * @return any return from the command
 *
 */
int32_t vfs_ioctl(int32_t fd, int32_t cmd, uint32_t arg);

/**
 * @brief This is a wildcard API for executing the particular poll by fd
 *
 * @param[in] fd     the file descriptor of the file or device
 * @param[in] flag   the flag of the polling
 * @param[in] notify the polling notify callback
 * @param[in] fds    a pointer to the array of pollfd
 * @param[in] arg    the arguments of the polling
 *
 * @return 0 on success, negative error on failure
 *
 */
int32_t vfs_do_pollfd(int32_t fd, int32_t flag, vfs_poll_notify_t notify,
                      void *fds, void *arg);

/**
 * @brief Move the file position to a given offset from a given location
 *
 * @param[in] fd     the file descriptor of the file
 * @param[in] offset the offset from whence to move to
 * @param[in] whence the start of where to seek
 *                   SEEK_SET to start from beginning of the file
 *                   SEEK_CUR to start from current position in file
 *                   SEEK_END to start from end of file
 *
 * @return the new offset of the file
 *
 */
uint32_t vfs_lseek(int32_t fd, uint32_t offset, int32_t whence);

/**
 * @brief Flush any buffers associated with the file
 *
 * @param[in] fd the file descriptor of the file
 *
 * @return 0 on success, negative error on failure
 *
 */
int32_t vfs_sync(int32_t fd);

/**
 * Store information about the file in a vfs_stat structure
 *
 * @param[in]  path the path of the file to find information about
 * @param[out] st   the vfs_stat buffer to write to
 *
 * @return 0 on success, negative error on failure
 *
 */
int32_t vfs_stat(const char *path, vfs_stat_t *st);

/**
 * Store information about the file in a vfs_stat structure
 *
 * @param[in]  fh the fh of the file to find information about
 * @param[out] st the vfs_stat buffer to write to
 *
 * @return 0 on success, negative error on failure
 *
 */
int32_t vfs_fstat(int fh, vfs_stat_t *st);

/**
 * @brief link path2 to path1
 *
 * @param[in] path1 the path to be linked
 * @param[in] path2 the path to link
 *
 * @return 0 on success, negative error on failure
 *
 */
int32_t vfs_link(const char *path1, const char *path2);

/**
 * @brief Remove a file from the filesystem
 *
 * @param[in] path the path of the file to remove
 *
 * @return 0 on success, negative error on failure
 *
 */
int32_t vfs_unlink(const char *path);

/**
 * @brief Remove a file from the filesystem
 *
 * @param[in] path the path of the file to remove
 *
 * @return 0 on success, negative error on failure
 *
 */
int32_t vfs_remove(const char *path);

/**
 * @brief Rename a file in the filesystem
 *
 * @param[in] oldpath the path of the file to rename
 * @param[in] newpath the path to rename it to
 *
 * @return 0 on success, negative error on failure
 *
 */
int32_t vfs_rename(const char *oldpath, const char *newpath);

/**
 * @brief Open a directory on the filesystem
 *
 * @param[in] path the path of the directory to open
 *
 * @return a pointer of directory stream on success, NULL on failure
 *
 */
vfs_dir_t *vfs_opendir(const char *path);

/**
 * @brief Close a directory
 *
 * @param[in] dir the pointer of the directory to close
 *
 * @return 0 on success, negative error on failure
 *
 */
int32_t vfs_closedir(vfs_dir_t *dir);

/**
 * @brief Read the next directory entry
 *
 * @param[in] dir the pointer of the directory to read
 *
 * @return a pointer to a vfs_dirent structure
 *
 */
vfs_dirent_t *vfs_readdir(vfs_dir_t *dir);

/**
 * @brief Create the directory, if ther do not already exist
 *
 * @param[in] path the path of the directory to create
 *
 * @return 0 on success, negative error on failure
 *
 */
int32_t vfs_mkdir(const char *path);

/**
 * @brief Remove a directory
 *
 * @param[in] path the path of the directory to remove
 *
 * @return 0 on success, negative error on failure
 *
 */
int32_t vfs_rmdir(const char *path);

/**
 * @brief Reset the position of a directory stream to the beginning of a directory
 *
 * @param[in] dir the pointer of the directory to rewind
 *
 * @return none
 *
 */
void vfs_rewinddir(vfs_dir_t *dir);

/**
 * @brief Obtain the current location associated with the directory
 *
 * @param[in] dir the pointer of the directory to tell
 *
 * @return the current location of the directory, negative error on failure
 *
 */
int32_t vfs_telldir(vfs_dir_t *dir);

/**
 * @brief Move the directory position to a given location
 *
 * @param[in] dir the pointer of the directory to seek
 * @param[in] loc the location of the directory
 *
 * @return none
 */
void vfs_seekdir(vfs_dir_t *dir, int32_t loc);

/**
 * @brief Store information about the filesystem in a vfs_statfs structure
 *
 * @param[in]  path the path of the filesystem to find information about
 * @param[out] buf  the vfs_statfs buffer to write to
 *
 * @return 0 on success, negative error on failure
 *
 */
int32_t vfs_statfs(const char *path, vfs_statfs_t *buf);

/**
 * @brief Get access information
 *
 * @param[in] path  the path of the file to access
 * @param[in] amode the access information to get
 *
 * @return 0 on success, negative error on failure
 *
 */
int32_t vfs_access(const char *path, int32_t amode);

/**
 * set the pathname of the current working directory
 *
 * @param path The path to set.
 *
 * @return 0 on success, negative error code on failure.
 *
 */
int vfs_chdir(const char *path);

/**
 * get the pathname of the current working directory.
 *
 * @param buf  The buffer to save the current working directory.
 * @param size The size of buf.
 *
 * @return NULL if error occured, buf if succeed.
 *
 */
char *vfs_getcwd(char *buf, size_t size);

/**
 * @brief Get path conf
 *
 * @param[in] path the path conf to get from
 * @param[in] name the kind of path conf to get
 *
 * @return value of path info
 */
int32_t vfs_pathconf(const char *path, int name);

/**
 * @brief Get path info
 *
 * @param[in]  name the path info to get
 *
 * @return value of path info
 */
int32_t vfs_fpathconf(int fd, int name);

/**
 * @brief Set the access and modification times
 *
 * @param[in] path the path conf to get from
 * @param[in] times the buffer to store time info
 *
 * @return 0 on success, negative error code on failure
 */
int vfs_utime(const char *path, const vfs_utimbuf_t *times);

/**
 * @brief Get file descriptor offset
 *
 * @return the vfs file descriptor offset
 *
 */
int32_t vfs_fd_offset_get(void);

/**
 * @brief Bind driver to the file or device
 *
 * @param[in] path the path of the file or device
 * @param[in] ops the driver operations to bind
 * @param[in] arg  the arguments of the driver operations
 *
 * @return 0 on success, negative error on failure
 *
 */
int32_t vfs_register_driver(const char *path, vfs_file_ops_t *ops, void *arg);

/**
 * @brief Unbind driver from the file or device
 *
 * @param[in] path the path of the file or device
 *
 * @return 0 on success, negative error on failure
 *
 */
int32_t vfs_unregister_driver(const char *path);

/**
 * @brief Mount filesystem to the path
 *
 * @param[in] path the mount point path
 * @param[in] ops the filesystem operations
 * @param[in] arg  the arguments of the filesystem operations
 *
 * @return 0 on success, negative error on failure
 *
 */
int32_t vfs_register_fs(const char *path, vfs_filesystem_ops_t* ops, void *arg);

/**
 * @brief Unmount the filesystem
 *
 * @param[in] path the mount point path
 *
 * @return 0 on success, negative error on failure
 *
 */
int32_t vfs_unregister_fs(const char *path);

#ifdef __cplusplus
}
#endif

#endif /* VFS_API_H */
