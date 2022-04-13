/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef VFS_TYPES_H
#define VFS_TYPES_H

#include <stdint.h>
#include <time.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netdb.h>
#include "vfs_conf.h"


typedef struct {
    time_t actime;  /* time of last access */
    time_t modtime; /* time of last modification */
} vfs_utimbuf_t;

typedef struct {
    uint16_t st_mode;    /* mode of file */
    uint32_t st_size;    /* bytes of file */
    time_t   st_actime;  /* time of last access */
    time_t   st_modtime; /* time of last modification */
} vfs_stat_t;

typedef struct {
    int32_t d_ino;    /* file number */
    uint8_t d_type;   /* type of file */
    char    d_name[]; /* file name */
} vfs_dirent_t;

typedef struct {
    int32_t dd_vfs_fd;
    int32_t dd_rsv;
} vfs_dir_t;

typedef struct {
    int32_t f_type;    /* fs type */
    int32_t f_bsize;   /* optimized transport block size */
    int32_t f_blocks;  /* total blocks */
    int32_t f_bfree;   /* available blocks */
    int32_t f_bavail;  /* number of blocks that non-super users can acquire */
    int32_t f_files;   /* total number of file nodes */
    int32_t f_ffree;   /* available file nodes */
    int32_t f_fsid;    /* fs id */
    int32_t f_namelen; /* max file name length */
} vfs_statfs_t;

typedef void (*vfs_poll_notify_t)(void *fds, void *arg);

typedef struct vfs_file_ops       vfs_file_ops_t;
typedef struct vfs_filesystem_ops vfs_filesystem_ops_t;
typedef struct vfs_network_ops vfs_network_ops_t;

union vfs_inode_ops_t {
    const vfs_file_ops_t       *i_ops;
    const vfs_filesystem_ops_t *i_fops;
    const vfs_network_ops_t    *i_nops;
};


typedef struct {
    union vfs_inode_ops_t ops; /* inode operations */

    void    *i_arg;   /* per inode private data */
    char    *i_name;  /* name of inode */
    int32_t  i_flags; /* flags for inode */
    uint8_t  type;    /* type for inode */
    uint8_t  refs;    /* refs for inode */
} vfs_inode_t;

typedef struct {
    vfs_inode_t *node;   /* node for file or device */
    void        *f_arg;  /* arguments for file or device */
    uint32_t     offset; /* offset of the file */
    int32_t     param;   /* parameter for inode */
#ifdef CONFIG_VFS_LSOPEN
    char         filename[VFS_PATH_MAX];
#endif
} vfs_file_t;

typedef enum {
    VFS_LIST_TYPE_FS,
    VFS_LIST_TYPE_DEVICE
} vfs_list_type_t;

struct vfs_file_ops {
    int32_t (*open)(vfs_inode_t *node, vfs_file_t *fp);
    int32_t (*close)(vfs_file_t *fp);
    int32_t (*read)(vfs_file_t *fp, void *buf, uint32_t nbytes);
    int32_t (*write)(vfs_file_t *fp, const void *buf, uint32_t nbytes);
    uint32_t (*lseek)(vfs_file_t *fp, int64_t off, int32_t whence);
    int32_t (*ioctl)(vfs_file_t *fp, int32_t cmd, uint32_t arg);
    int32_t (*poll)(vfs_file_t *fp, int32_t flag, vfs_poll_notify_t notify, void *fds, void *arg);
    int32_t (*stat)(vfs_file_t *fp, const char *path, vfs_stat_t *st);
    void   *(*mmap)(vfs_file_t *fp, size_t len);
    int32_t (*access)(vfs_file_t *fp, const char *path, int32_t amode);
};

struct vfs_filesystem_ops {
    int32_t (*open)(vfs_file_t *fp, const char *path, int32_t flags);
    int32_t (*close)(vfs_file_t *fp);
    int32_t (*read)(vfs_file_t *fp, char *buf, uint32_t len);
    int32_t (*write)(vfs_file_t *fp, const char *buf, uint32_t len);
    uint32_t (*lseek)(vfs_file_t *fp, int64_t off, int32_t whence);
    int32_t (*sync)(vfs_file_t *fp);
    int32_t (*stat)(vfs_file_t *fp, const char *path, vfs_stat_t *st);
    int32_t (*fstat)(vfs_file_t *fp, vfs_stat_t *st);
    int32_t (*link)(vfs_file_t *fp, const char *path1, const char *path2);
    int32_t (*unlink)(vfs_file_t *fp, const char *path);
    int32_t (*remove)(vfs_file_t *fp, const char *path);
    int32_t (*rename)(vfs_file_t *fp, const char *oldpath, const char *newpath);
    vfs_dir_t    *(*opendir)(vfs_file_t *fp, const char *path);
    vfs_dirent_t *(*readdir)(vfs_file_t *fp, vfs_dir_t *dir);
    int32_t (*closedir)(vfs_file_t *fp, vfs_dir_t *dir);
    int32_t (*mkdir)(vfs_file_t *fp, const char *path);
    int32_t (*rmdir)(vfs_file_t *fp, const char *path);
    void (*rewinddir)(vfs_file_t *fp, vfs_dir_t *dir);
    int32_t (*telldir)(vfs_file_t *fp, vfs_dir_t *dir);
    void (*seekdir)(vfs_file_t *fp, vfs_dir_t *dir, int32_t loc);
    int32_t (*ioctl)(vfs_file_t *fp, int32_t cmd, uint32_t arg);
    int32_t (*statfs)(vfs_file_t *fp, const char *path, vfs_statfs_t *suf);
    int32_t (*access)(vfs_file_t *fp, const char *path, int32_t amode);
    int32_t (*pathconf)(vfs_file_t *fp, const char *path, int32_t name);
    int32_t (*fpathconf)(vfs_file_t *fp, int32_t name);
    int32_t (*utime)(vfs_file_t *fp, const char *path, const vfs_utimbuf_t *times);
};

struct vfs_network_ops {
    int32_t (*socket)(int32_t domain, int32_t type, int32_t protocol);
    int32_t (*accept)(int32_t fd, struct sockaddr *addr, socklen_t *addrlen);
    int32_t (*bind)(int32_t fd, const struct sockaddr *name, socklen_t namelen);
    int32_t (*shutdown)(int32_t fd, int32_t how);
    int32_t (*getpeername)(int32_t fd, struct sockaddr *name, socklen_t *namelen);
    int32_t (*getsockname)(int32_t fd, struct sockaddr *name, socklen_t *namelen);
    int32_t (*getsockopt)(int32_t fd, int32_t level, int32_t optname, void *optval, socklen_t *optlen);
    int32_t (*setsockopt)(int32_t fd, int32_t level, int32_t optname, const void *optval, socklen_t optlen);
    int32_t (*close)(int32_t fd);
    int32_t (*connect)(int32_t fd, const struct sockaddr *name, socklen_t namelen);
    int32_t (*listen)(int32_t fd, int32_t backlog);
    int32_t (*recv)(int32_t fd, void *mem, size_t len, int32_t flags);
    int32_t (*recvfrom)(int32_t fd, void *mem, size_t len, int32_t flags,
        struct sockaddr *from, socklen_t *fromlen);
    int32_t (*send)(int32_t fd, const void *dataptr, size_t size, int32_t flags);
    int32_t (*sendmsg)(int32_t fd, const struct msghdr *message, int32_t flags);
    int32_t (*sendto)(int32_t fd, const void *dataptr, size_t size, int32_t flags,
        const struct sockaddr *to, socklen_t tolen);
    int32_t (*read)(int32_t fd, char *buf, uint32_t len);
    int32_t (*write)(int32_t fd, const char *buf, uint32_t len);
    int32_t (*ioctl)(int32_t fd, int32_t cmd, uint32_t arg);
    int32_t (*fcntl)(int32_t fd, int32_t cmd, int32_t val);
    int32_t (*writev)(int32_t fd, const struct iovec *iov, int32_t iovcnt);
    int32_t (*eventfd)(uint32_t initval, int32_t flags);
    int32_t (*try_wakeup)(int32_t fd, int32_t rcvevent, int32_t sendevent, int32_t errevent);
    struct hostent* (*gethostbyname)(const char *name);
    int (*gethostbyname_r)(const char *name, struct hostent *ret, char *buf,
                size_t buflen, struct hostent **result, int *h_errnop);
    void (*freeaddrinfo)(struct addrinfo *ai);
    int (*getaddrinfo)(const char *nodename,
       const char *servname,
       const struct addrinfo *hints,
       struct addrinfo **res);
    int (*select)(int maxfdp1, fd_set *readset, fd_set *writeset, fd_set *exceptset, struct timeval *timeout);
};
#endif /* VFS_TYPES_H */

