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

/**
 * @brief aos_utimbuf structure describes the filesystem inode's
 *        last access time and last modification time.
 */
struct aos_utimbuf {
    time_t actime;  /**< time of last access */
    time_t modtime; /**< time of last modification */
};

struct aos_statfs {
    long f_type;    /**< fs type */
    long f_bsize;   /**< optimized transport block size */
    long f_blocks;  /**< total blocks */
    long f_bfree;   /**< available blocks */
    long f_bavail;  /**< number of blocks that non-super users can acquire */
    long f_files;   /**< total number of file nodes */
    long f_ffree;   /**< available file nodes */
    long f_fsid;    /**< fs id */
    long f_namelen; /**< max file name length */
};

struct aos_stat {
    uint16_t st_mode;    /**< mode of file */
    uint32_t st_size;    /**< bytes of file */
    time_t   st_actime;  /**< time of last access */
    time_t   st_modtime; /**< time of last modification */
};

typedef struct {
    int32_t d_ino;    /**< file number */
    uint8_t d_type;   /**< type of file */
    char    d_name[]; /**< file name */
} aos_dirent_t;

typedef struct {
    int32_t dd_vfs_fd;  /**< file index in vfs */
    int32_t dd_rsv;     /**< Reserved */
} aos_dir_t;

typedef const struct file_ops file_ops_t;
typedef const struct fs_ops   fs_ops_t;

union inode_ops_t {
    const file_ops_t *i_ops;  /**< char driver operations */
    const fs_ops_t   *i_fops; /**< FS operations */
};

typedef struct {
    union inode_ops_t  ops;     /**< inode operations */
    void              *i_arg;   /**< per inode private data */
    char              *i_name;  /**< name of inode */
    int                i_flags; /**< flags for inode */
    uint8_t            type;    /**< type for inode */
    uint8_t            refs;    /**< refs for inode */
} inode_t;

typedef struct {
    inode_t *node;   /**< node for file */
    void    *f_arg;  /**< f_arg for file */
    size_t   offset; /**< offset for file */
} file_t;

typedef void (*poll_notify_t)(void *pollfd, void *arg);

/**
 * @brief file_ops structure defines the file operation handles
 */
struct file_ops {
    int     (*open)(inode_t *node, file_t *fp);
    int     (*close)(file_t *fp);
    ssize_t (*read)(file_t *fp, void *buf, size_t nbytes);
    ssize_t (*write)(file_t *fp, const void *buf, size_t nbytes);
    int     (*ioctl)(file_t *fp, int cmd, unsigned long arg);
    int     (*poll)(file_t *fp, int flag, poll_notify_t notify, void *fd, void *arg);
};

/**
 * @brief fs_ops structures defines the filesystem operation handles
 */
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
 * @brief aos_vfs_init() initializes vfs system.
 *
 * @param[in] NULL
 *
 * @return  On success, return new file descriptor.
 *          On error, negative error code is returned to indicate the cause
 *          of the error.
 */
int aos_vfs_init(void);

/**
 * @brief aos_open() opens the file or device by its @path.
 *
 * @param[in] path   the path of the file or device to open.
 * @param[in] flags  the mode of open operation.
 *
 * @return  On success, return new file descriptor.
 *          On error, negative error code is returned to indicate the cause
 *          of the error.
 */
int aos_open(const char *path, int flags);

/**
 * @brief aos_close() closes the file or device associated with file
 *        descriptor @fd.
 *
 * @param[in] fd  the file descriptor of the file or device.
 *
 * @return  On success, return 0.
 *          On error, negative error code is returned to indicate the cause
 *          of the error.
 */
int aos_close(int fd);

/**
 * @brief aos_read() attempts to read up to @nbytes bytes from file
 *        descriptor @fd into the buffer starting at @buf.
 *
 * @param[in]  fd      the file descriptor of the file or device.
 * @param[out] buf     the buffer to read bytes into.
 * @param[in]  nbytes  the number of bytes to read.
 *
 * @return  On success, the number of bytes is returned (0 indicates end
 *          of file) and the file position is advanced by this number.
 *          On error, negative error code is returned to indicate the cause
 *          of the error.
 */
ssize_t aos_read(int fd, void *buf, size_t nbytes);

/**
 * @brief aos_write() writes up to @nbytes bytes from the buffer starting
 *        at @buf to the file referred to by the file descriptor @fd.
 *
 * @param[in] fd      the file descriptor of the file or device.
 * @param[in] buf     the buffer to write bytes from.
 * @param[in] nbytes  the number of bytes to write.
 *
 * @return  On success, the number of bytes written is returned, adn the file
 *          position is advanced by this number..
 *          On error, negative error code is returned to indicate the cause
 *          of the error.
 */
ssize_t aos_write(int fd, const void *buf, size_t nbytes);

/**
 * @brief aos_ioctl() manipulates the underlying device parameters of special
 *        files. In particular, many operating characteristics of character
 *        special filse may be controlled with aos_iotcl() requests. The argumnet
 *        @fd must be an open file descriptor.
 *
 * @param[in] fd   the file descriptior of the file or device.
 * @param[in] cmd  A device-dependent request code.
 * @param[in] arg  Argument to the request code, which is interpreted according
 *                 to the request code.
 *
 * @return  Usually, on success 0 is returned. Some requests use the return
 *          value as an output parameter and return a nonnegative value on success.
 *          On error, neagtive error code is returned to indicate the cause
 *          of the error.
 */
int aos_ioctl(int fd, int cmd, unsigned long arg);

/**
 * @brief aos_do_pollfd() is a wildcard API for executing the particular poll events
 *
 * @param[in] fd      The file descriptor of the file or device
 * @param[in] flag    The flag of the polling
 * @param[in] notify  The polling notify callback
 * @param[in] fds     A pointer to the array of pollfd
 * @param[in] arg     The arguments of the polling
 *
 * @return  On success, return 0.
 *          On error, negative error code is returned to indicate the cause
 *          of the error.
 */
int aos_do_pollfd(int fd, int flag, poll_notify_t notify, void *fds, void *arg);

/**
 * @brief aos_lseek() repositions the file offset of the open file
 *        description associated with the file descriptor @fd to the
 *        argument @offset according to the directive @whence as follows:
 *
 *        SEEK_SET: The file offset is set to @offset bytes.
 *        SEEK_CUR: The file offset is set to its current location
 *                  plus @offset bytes.
 *        SEEK_END: The file offset is set to the size of the file
 *                  plus @offset bytes.
 *
 * @param[in] fd      The file descriptor of the file.
 * @param[in] offset  The offset relative to @whence directive.
 * @param[in] whence  The start position where to seek.
 *
 * @return  On success, return the resulting offset location as measured
 *          in bytes from the beginning of the file.
 *          On error, neagtive error code is returned to indicate the cause
 *          of the error.
 */
off_t aos_lseek(int fd, off_t offset, int whence);

/**
 * @brief aos_sync causes all pending modifications to filesystem metadata
 *        and cached file data to be written to the underlying filesystems.
 *
 * @param[in] fd  the file descriptor of the file.
 *
 * @return  On success return 0.
 *          On error, negative error code is returned to indicate the cause
 *          of the error.
 */
int aos_sync(int fd);

/**
 * @brief aos_stat() return information about a file pointed to by @path
 *        in the buffer pointed to by @st.
 *
 * @param[in]  path  The path of the file to be quried.
 * @param[out] st    The buffer to receive information.
 *
 * @return  On success, return 0.
 *          On error, negative error code is returned to indicate the cause
 *          of the error.
 */
int aos_stat(const char *path, struct aos_stat *st);

/**
 * @brief aos_fstat() return information about a file specified by the file
 *        descriptor @fd in the buffer pointed to by @st.
 *
 * @note  aos_fstat() is identical to aos_stat(), except that the file about
 *        which information is to be retrieved is specified by the file
 *        descriptor @fd.
 *
 * @param[in]  fd  The file descriptor of the file to be quired.
 * @param[out] st  The buffer to receive information.
 *
 * @return  On success, return 0.
 *          On error, negative error code is returned to indicate the cause
 *          of the error.
 */
int aos_fstat(int fd, struct aos_stat *st);

/**
 * @brief aos_link() creates a new link @newpath to an existing file @oldpath.
 *
 * @note  If @newpath exists, it will not be ovrewritten.
 *
 * @param[in] oldpath  The old path
 * @param[in] newpath  The new path to be created
 *
 * @return  On success, return 0.
 *          On error, negative error code is returned to indicate the cause
 *          of the error.
 */
int aos_link(const char *oldpath, const char *newpath);

/**
 * @brief aos_unlink() deletes a name from the filesystem.
 *
 * @param[in] path  The path of the file to be deleted.
 *
 * @return  On success, return 0.
 *          On error, negative error code is returned to indicate the cause
 *          of the error.
 */
int aos_unlink(const char *path);

/**
 * @brief aos_remove() deletes a name from the filesystem.
 *
 * @param[in] path  The path of the file to be deleted.
 *
 * @return  On success, return 0.
 *          On error, negative error code is returned to indicate the cause
 *          of the error.
 */
int aos_remove(const char *path);

/**
 * @brief aos_rename() renames a file, moving it between directories
 *        if required.
 *
 * @param[in] oldpath  The old path of the file to rename.
 * @param[in] newpath  The new path to rename the file to.
 *
 * @return  On success, return 0.
 *          On error, negative error code is returned to indicate the cause
 *          of the error.
 */
int aos_rename(const char *oldpath, const char *newpath);

/**
 * @brief aos_opendir() opens a directory stream corresponding to the
 *        directory @path, and returns a pointer to the directory stream.
 *        The stream is positioned at the first entry in the directory.
 *
 * @param[in] path  the path of the directory to open.
 *
 * @return  On success, return a point of directory stream.
 *          On error, NULL is returned.
 */
aos_dir_t *aos_opendir(const char *path);

/**
 * @brief aos_closedir() closes the directory stream associated with
 *        @dir. A successful call to aos_closedir() also closes the
 *        underlying file descriptor associated with @dir. The directory
 *        stream descriptor @dir is not available after this call.
 *
 * @param[in] dir  The directory stream descriptor to be closed.
 *
 * @return  On success, return 0.
 *          On error, negative error code is returned to indicate the cause
 *          of the error.
 */
int aos_closedir(aos_dir_t *dir);

/**
 * @brief aos_readdir() returns a pointer to an @aos_dirent_t representing
 *        the next directory entry in the directory stream pointed to by
 *        @dir. It returns Null on reaching the end of the directory stream
 *        or if an error occurred.
 *
 * @param[in] dir  The directory stream descriptor to read.
 *
 * @return  On success, aos_readdir() returns a pointer to an @aos_dirent_t
 *          structure. If the end of the directory stream is reached, NULL is
 *          returned.
 *          On error, NULL is returned.
 */
aos_dirent_t *aos_readdir(aos_dir_t *dir);

/**
 * @brief aos_mkdir() attempts to create a directory named @path
 *
 * @param[in] path  The name of directory to be created.
 *
 * @return  On success, return 0.
 *          On error, negative error code is returned to indicate the cause
 *          of the error.
 */
int aos_mkdir(const char *path);

/**
 * @brief aos_rmdir() deletes a directory, which must be emtpy.
 *
 * @param[in] path  The directory to be deleted.
 *
 * @return  On success, return 0.
 *          On error, negative error code is returned to indicate the cause
 *          of the error.
 */
int aos_rmdir(const char *path);

/**
 * @brief aos_rewinddir() resets the position of the directory stream @dir
 *        to the beginning of the directory.
 *
 * @param[in] dir  The directory stream descriptor pointer.
 *
 * @return  none.
 */
void aos_rewinddir(aos_dir_t *dir);

/**
 * @brief aos_telldir() returns the current location associated with the
 *        directory stream @dir.
 *
 * @param[in] dir  The directory stream descriptor pointer.
 *
 * @return  On success, aos_telldir() returns the current location in the
 *          directory stream.
 *          On error, negative error code is returned to indicate the cause
 *          of the error.
 */
long aos_telldir(aos_dir_t *dir);

/**
 * @brief aos_seekdir() sets the location in the directory stram from
 *        which the next aos_readdir() call will start. The @loc argument
 *        should be a value returnned by a previous call to aos_telldir().
 *
 * @param[in] dir  The directory stream descriptor pointer.
 * @param[in] loc  The location in the directory stream from which the next
 *                 aos_readdir() call will start.
 *
 * @return  none.
 */
void aos_seekdir(aos_dir_t *dir, long loc);

/**
 * @brief aos_statfs() gets information about a mounted filesystem.
 *
 * @param[in]  path  The path name of any file within the mounted filessytem.
 * @param[out] buf   Buffer points to an aos_statfs structure to receive
 *                   filesystem information.
 *
 * @return  On success, return 0.
 *          On error, negative error code is returned to indicate the cause
 *          of the error.
 */
int aos_statfs(const char *path, struct aos_statfs *buf);

/**
 * @brief aos_access() checks whether the calling process can access the
 *        file @path.
 *
 * @param[in] path  The path of the file.
 * @param[in] mode  Specifies the accessibility check(s) to be performed, and
 *                  is either the value of F_OK, or a mask consisting of the
 *                  bitwise OR of one or more of R_OK, W_OK, and X_OK. F_OK
 *                  tests for the existence of the file. R_OK, W_OK and X_OK
 *                  tests whether the file exists and grants read, write, and
 *                  execute permissions, repectively.
 *
 * @return On success (all requested permissions granted, or mode is F_OK and
 *         the file exists), 0 is returned.
 *         On error (at least one bit in mode asked for a permission that is
 *         denied, or mode is F_OK and the file does not exist, or some other
 *         error occurred), negative error code is returned to indicate the
 *         cause of the error.
 */
int aos_access(const char *path, int amode);

/**
 * @brief aos_chdir() changes the current working directory of the calling
 *        process to the directory specified in @path.
 *
 * @param[in] path  The path to change to.
 *
 * @return  On success return 0.
 *          On error, negative error code is returned to indicate the cause
 *          of the error.
 */
int aos_chdir(const char *path);

/**
 * @brief aos_getcwd() return a null-terminated string containing an absolute
 *        pathname that is the current working directory of the calling process.
 *        The pathname is returned as the function result and via the argument
 *        @buf, if present.
 *        aos_getcwd() function copies an absolute pathname of the current
 *        working directory to the array pointed by @buf, which is of length @size.
 *
 *        If the length of the absolute pathname of the current working directory,
 *        including the terminating null byte, exceeds @size bytes, NULL is returned.
 *
 * @param[out] buf   The buffer to receive the current working directory pathname.
 * @param[in]  size  The size of buffer.
 *
 * @return On success, aos_getcwd() returns a pointer to a string containing
 *         the pathname of the current working directory.
 *         On error, NULL is returned.
 */
char *aos_getcwd(char *buf, size_t size);

/**
 * @brief aos_pathconf() gets a value for configuration option @name for the
 *        filename @path.
 *
 * @param[in] path  The path name to quire
 * @param[in] name  The configuration option
 *
 * @return On error, negative error code is returned to indicate the cause
 *         of the error.
 *         On success, if @name corresponds to an option, a positive value is
 *         returned if the option is supported.
 */
long aos_pathconf(const char *path, int name);

/**
 * @brief aos_fpathconf() gets a value for the cofiguration option @name for
 *        the open file descriptor @fd.
 *
 * @param[in] fd    The open file descriptor to quire
 * @param[in] name  The configuration option
 *
 *  @return On success, if @name corresponds to an option, a positive value is
 *          returned if the option is supported.
 *          On error, negative error code is returned to indicate the cause
 *          of the error.
 */
long aos_fpathconf(int fd, int name);

/**
 * @brief aos_utime() changes teh access and modification times of the inode
 *        specified by @path to the actime and modtime fields of the @times
 *        respectively.
 *        If @times is NULL, then the access and modification times of the file
 *        are set to the current time.
 *
 * @param[in] path   Path name of the inode to operate.
 * @param[in] times  Buffer pointer to structure aos_utimbuf whose actime
 *                    and modtime fields will be set to the inode @path.
 *
 * @return 0 on success, negative error code on failure
 */
int aos_utime(const char *path, const struct aos_utimbuf *times);

/**
 * @brief aos_vfs_fd_offset_get() gets VFS fd offset
 *
 * @return VFS fd offset
 */
int aos_vfs_fd_offset_get(void);

/**
 * @brief Bind driver to the file or device
 *
 * @param[in] path  The path of the file or device
 * @param[in] ops   The driver operations to bind
 * @param[in] arg   The arguments of the driver operations
 *
 * @return On success return 0.
 *         On error, negative error code is returned to indicate the cause
 *         of the error.
 */
int aos_register_driver(const char *path, file_ops_t *ops, void *arg);

/**
 * @brief Unbind driver from the file or device
 *
 * @param[in] path  The path of the file or device
 *
 * @return On success return 0.
 *         On error, negative error code is returned to indicate the cause
 *         of the error.
 */
int aos_unregister_driver(const char *path);

/**
 * @brief aos_register_fs() mounts filesystem to the path
 *
 * @param[in] path  The mount point path
 * @param[in] ops   The filesystem operations
 * @param[in] arg   The arguments of the filesystem operations
 *
 * @return On success return 0.
 *         On error, negative error code is returned to indicate the cause
 *         of the error.
 */
int aos_register_fs(const char *path, fs_ops_t* ops, void *arg);

/**
 * @brief aos_unregister_fs() unmounts the filesystem
 *
 * @param[in] path  The mount point path
 *
 * @return On success return 0.
 *         On error, negative error code is returned to indicate the cause
 *         of the error.
 */
int aos_unregister_fs(const char *path);

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* AOS_VFS_H */

