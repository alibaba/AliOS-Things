/**
 * @file vfs.h
 * @copyright Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef AOS_VFS_H
#define AOS_VFS_H

#include <stddef.h>
#include <stdint.h>
#include <unistd.h>
#include <time.h>

#include <sys/stat.h>
#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup aos_vfs VFS
 *  VFS AOS API.
 *
 *  @{
 */

struct aos_utimbuf {
    time_t actime;  /* time of last access */
    time_t modtime; /* time of last modification */
};

struct aos_statfs {
    long f_type;    /* fs type */
    long f_bsize;   /* optimized transport block size */
    long f_blocks;  /* total blocks */
    long f_bfree;   /* available blocks */
    long f_bavail;  /* number of blocks that non-super users can acquire */
    long f_files;   /* total number of file nodes */
    long f_ffree;   /* available file nodes */
    long f_fsid;    /* fs id */
    long f_namelen; /* max file name length */
};

struct aos_stat {
    uint16_t st_mode;    /* mode of file */
    uint32_t st_size;    /* bytes of file */
    time_t   st_actime;  /* time of last access */
    time_t   st_modtime; /* time of last modification */
};

typedef struct {
    int32_t d_ino;    /* file number */
    uint8_t d_type;   /* type of file */
    char    d_name[]; /* file name */
} aos_dirent_t;

typedef struct {
    int32_t dd_vfs_fd;  /* file index in vfs */
    int32_t dd_rsv;     /* Reserved */
} aos_dir_t;

typedef const struct file_ops file_ops_t;
typedef const struct fs_ops   fs_ops_t;

union inode_ops_t {
    const file_ops_t *i_ops;  /* char driver operations */
    const fs_ops_t   *i_fops; /* FS operations */
};

typedef struct {
    union inode_ops_t  ops;     /* inode operations */
    void              *i_arg;   /* per inode private data */
    char              *i_name;  /* name of inode */
    int                i_flags; /* flags for inode */
    uint8_t            type;    /* type for inode */
    uint8_t            refs;    /* refs for inode */
} inode_t;

typedef struct {
    inode_t *node;   /* node for file */
    void    *f_arg;  /* f_arg for file */
    size_t   offset; /* offset for file */
} file_t;

typedef void (*poll_notify_t)(void *pollfd, void *arg);

/* This struct defined the file operate handles */
struct file_ops {
    int     (*open)(inode_t *node, file_t *fp);
    int     (*close)(file_t *fp);
    ssize_t (*read)(file_t *fp, void *buf, size_t nbytes);
    ssize_t (*write)(file_t *fp, const void *buf, size_t nbytes);
    int     (*ioctl)(file_t *fp, int cmd, unsigned long arg);
    int     (*poll)(file_t *fp, int flag, poll_notify_t notify, void *fd, void *arg);
};

/* This struct defined the file system operate handles */
struct fs_ops {
    int           (*open)(file_t *fp, const char *path, int flags);
    int           (*close)(file_t *fp);
    ssize_t       (*read)(file_t *fp, char *buf, size_t len);
    ssize_t       (*write)(file_t *fp, const char *buf, size_t len);
    off_t         (*lseek)(file_t *fp, off_t off, int whence);
    int           (*sync)(file_t *fp);
    int           (*stat)(file_t *fp, const char *path, struct aos_stat *st);
    int           (*fstat)(file_t *fp, struct aos_stat *st);
    int           (*link)(file_t *fp, const char *path1, const char *path2);
    int           (*unlink)(file_t *fp, const char *path);
    int           (*remove)(file_t *fp, const char *path);
    int           (*rename)(file_t *fp, const char *oldpath, const char *newpath);
    aos_dir_t    *(*opendir)(file_t *fp, const char *path);
    aos_dirent_t *(*readdir)(file_t *fp, aos_dir_t *dir);
    int           (*closedir)(file_t *fp, aos_dir_t *dir);
    int           (*mkdir)(file_t *fp, const char *path);
    int           (*rmdir)(file_t *fp, const char *path);
    void          (*rewinddir)(file_t *fp, aos_dir_t *dir);
    long          (*telldir)(file_t *fp, aos_dir_t *dir);
    void          (*seekdir)(file_t *fp, aos_dir_t *dir, long loc);
    int           (*ioctl)(file_t *fp, int cmd, unsigned long arg);
    int           (*statfs)(file_t *fp, const char *path, struct aos_statfs *suf);
    int           (*access)(file_t *fp, const char *path, int amode);
    long          (*pathconf)(file_t *fp, const char *path, int name);
    long          (*fpathconf)(file_t *fp, int name);
    int           (*utime)(file_t *fp, const char *path, const struct aos_utimbuf *times);
};

/**
 * Init vfs.
 *
 * @param[in] NULL
 *
 * @return 0 on success, negative error on failure.
 *
 */
int aos_vfs_init(void);

/**
 * Open the file or device by its path.
 *
 * @param[in]  path   the path of the file or device to open.
 * @param[in]  flags  the mode of open operation.
 *
 * @return  the new file descriptor, negative error on failure.
 *
 */
int aos_open(const char *path, int flags);

/**
 * Close the file or device by its file descriptor.
 *
 * @param[in]  fd  the file descriptor of the file or device.
 *
 * @return  0 on success, negative error on failure.
 *
 */
int aos_close(int fd);

/**
 * Read the contents of a file or device into a buffer.
 *
 * @param[in]   fd      the file descriptor of the file or device.
 * @param[in]   nbytes  the number of bytes to read.
 * @param[out]  buf     the buffer to read in to.
 *
 * @return  The number of bytes read, 0 at end of file, negative error on failure.
 *
 */
ssize_t aos_read(int fd, void *buf, size_t nbytes);

/**
 * Write the contents of a buffer to file or device.
 *
 * @param[in]  fd      the file descriptor of the file or device.
 * @param[in]  nbytes  the number of bytes to write.
 * @param[in]  buf     the buffer to write from.
 *
 * @return  The number of bytes written, negative error on failure.
 *
 */
ssize_t aos_write(int fd, const void *buf, size_t nbytes);

/**
 * This is a wildcard API for sending controller specific commands.
 *
 * @param[in]  fd   the file descriptior of the file or device.
 * @param[in]  cmd  A controller specific command.
 * @param[in]  arg  Argument to the command, interpreted according to the command.
 *
 * @return  any return from the command.
 *
 */
int aos_ioctl(int fd, int cmd, unsigned long arg);

/**
 * @brief This is a wildcard API for executing the particular poll by fd
 *
 * @param[in]  fd      the file descriptor of the file or device
 * @param[in]  flag    the flag of the polling
 * @param[in]  notify  the polling notify callback
 * @param[in]  fds     a pointer to the array of pollfd
 * @param[in]  arg     the arguments of the polling
 *
 * @return 0 on success, negative error on failure
 *
 */
int aos_do_pollfd(int fd, int flag, poll_notify_t notify, void *fds, void *arg);

/**
 * Move the file position to a given offset from a given location.
 *
 * @param[in]  fd      the file descriptor of the file.
 * @param[in]  offset  The offset from whence to move to.
 * @param[in]  whence  The start of where to seek.
 *                     SEEK_SET to start from beginning of file.
 *                     SEEK_CUR to start from current position in file.
 *                     SEEK_END to start from end of file.
 *
 * @return  The new offset of the file.
 *
 */
off_t aos_lseek(int fd, off_t offset, int whence);

/**
 * Flush any buffers associated with the file.
 *
 * @param[in]  fd  the file descriptor of the file.
 *
 * @return  0 on success, negative error code on failure.
 *
 */
int aos_sync(int fd);

/**
 * Store information about the file in a stat structure.
 *
 * @param[in]   path  The path of the file to find information about.
 * @param[out]  st    The stat buffer to write to.
 *
 * @return  0 on success, negative error code on failure.
 *
 */
int aos_stat(const char *path, struct aos_stat *st);

/**
 * Store information about the file in a vfs_stat structure
 *
 * @param[in]   fh  the fh of the file to find information about
 * @param[out]  st  the vfs_stat buffer to write to
 *
 * @return 0 on success, negative error on failure
 *
 */
int aos_fstat(int fh, struct aos_stat *st);

/**
 * @brief link path2 to path1
 *
 * @param[in]  path1  the path to be linked
 * @param[in]  path2  the path to link
 *
 * @return 0 on success, negative error on failure
 *
 */
int aos_link(const char *path1, const char *path2);

/**
 * Remove a file from the filesystem.
 *
 * @param[in]  path  The path of the file to remove.
 *
 * @return  0 on success, negative error code on failure.
 *
 */
int aos_unlink(const char *path);

/**
 * @brief Remove a file from the filesystem
 *
 * @param[in] path the path of the file to remove
 *
 * @return 0 on success, negative error on failure
 *
 */
int aos_remove(const char *path);

/**
 * Rename a file in the filesystem.
 *
 * @param[in]  oldpath  The path of the file to rename.
 * @param[in]  newpath  The path to rename it to.
 *
 * @return  0 on success, negative error code on failure.
 *
 */
int aos_rename(const char *oldpath, const char *newpath);

/**
 * Open a directory on the filesystem.
 *
 * @param[in]  path  the path of the directory to open.
 *
 * @return  a point of directory stream on success, NULL on failure.
 *
 */
aos_dir_t *aos_opendir(const char *path);

/**
 * Close a directory.
 *
 * @param[in]  dir  the handle of the directory to close.
 *
 * @return  0 on success, negative error code on failure.
 *
 */
int aos_closedir(aos_dir_t *dir);

/**
 * Read the next directory entry.
 *
 * @param[in]  dir  the handle of the directory to read.
 *
 * @return  a pointer to a dirent structure.
 *
 */
aos_dirent_t *aos_readdir(aos_dir_t *dir);

/**
 * Create the directory, if they do not already exist.
 *
 * @param[in]  path  the path of the directory.
 *
 * @return  0 on success, negative error code on failure.
 *
 */
int aos_mkdir(const char *path);

/**
 * Remove a directory.
 *
 * @param[in]  path  the path of the directory.
 *
 * @return  0 on success, negative error code on failure.
 *
 */
int aos_rmdir(const char *path);

/**
 * Reset the position of a directory stream to the beginning of a directory.
 *
 * @param[in]  dir  the handle of the directory.
 *
 * @return  none.
 *
 */
void aos_rewinddir(aos_dir_t *dir);

/**
 * Obtain the current location associated with the directory stream specified by dirp.
 *
 * @param[in]  dir  the handle of the directory.
 *
 * @return  0 on success, negative error code on failure.
 *
 */
long aos_telldir(aos_dir_t *dir);

/**
 * Reset the position of a directory stream to the beginning of a directory.
 *
 * @param[in]  dir  the handle of the directory.
 * @param[in]  loc  the position of the directory.
 *
 * @return  none.
 *
 */
void aos_seekdir(aos_dir_t *dir, long loc);

/**
 * Store information about the file system in a statfs structure.
 *
 * @param[in]   path  The path of the file system to find information about.
 * @param[out]  buf   The statfs buffer to write to.
 *
 * @return  0 on success, negative error code on failure.
 *
 */
int aos_statfs(const char *path, struct aos_statfs *buf);

/**
 * get access info.
 *
 * @param  path  The path of the file.
 * @param  mode  the info to get.
 *
 * @return  0 on success, negative error code on failure.
 *
 */
int aos_access(const char *path, int amode);

/**
 * set the pathname of the current working directory
 *
 * @param  path  The path to set.
 *
 * @return  0 on success, negative error code on failure.
 *
 */
int aos_chdir(const char *path);

/**
 * get the pathname of the current working directory.
 *
 * @param  buf   The buffer to save the current working directory.
 * @param  size  The size of buffer.
 *
 * @return  NULL if error occured, buf if succeed.
 *
 */
char *aos_getcwd(char *buf, size_t size);

/**
 * @brief Get path conf
 *
 * @param[in]  path  the path conf to get from
 * @param[in]  name  the kind of path conf to get
 *
 * @return value of path info
 */
long aos_pathconf(const char *path, int name);

/**
 * @brief Get path info
 *
 * @param[in]  name  the path info to get
 *
 * @return value of path info
 */
long aos_fpathconf(int fd, int name);

/**
 * @brief Set the access and modification times
 *
 * @param[in]  path   the path conf to get from
 * @param[in]  times  the buffer to store time info
 *
 * @return 0 on success, negative error code on failure
 */
int aos_utime(const char *path, const struct aos_utimbuf *times);

/**
 * get VFS fd offset
 *
 * @return VFS fd offset
 *
 */
int aos_vfs_fd_offset_get(void);

/**
 * @brief Bind driver to the file or device
 *
 * @param[in]  path  the path of the file or device
 * @param[in]  ops   the driver operations to bind
 * @param[in]  arg   the arguments of the driver operations
 *
 * @return 0 on success, negative error on failure
 *
 */
int aos_register_driver(const char *path, file_ops_t *ops, void *arg);

/**
 * @brief Unbind driver from the file or device
 *
 * @param[in]  path  the path of the file or device
 *
 * @return 0 on success, negative error on failure
 *
 */
int aos_unregister_driver(const char *path);

/**
 * @brief Mount filesystem to the path
 *
 * @param[in]  path  the mount point path
 * @param[in]  ops   the filesystem operations
 * @param[in]  arg   the arguments of the filesystem operations
 *
 * @return 0 on success, negative error on failure
 *
 */
int aos_register_fs(const char *path, fs_ops_t* ops, void *arg);

/**
 * @brief Unmount the filesystem
 *
 * @param[in]  path  the mount point path
 *
 * @return 0 on success, negative error on failure
 *
 */
int aos_unregister_fs(const char *path);

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* AOS_VFS_H */

