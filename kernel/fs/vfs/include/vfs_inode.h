/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef VFS_INODE_H
#define VFS_INODE_H

#include <stdbool.h>
#include <stdint.h>
#include <sys/stat.h>
#include <vfs_conf.h>
#include <k_api.h>
#include <aos/errno.h>
#include <vfs.h>

#ifdef __cplusplus
extern "C" {
#endif

enum {
    VFS_TYPE_NOT_INIT,
    VFS_TYPE_CHAR_DEV,
    VFS_TYPE_BLOCK_DEV,
    VFS_TYPE_FS_DEV
};

#define INODE_IS_TYPE(i,t) \
    ((i)->type == (t))

#define INODE_IS_CHAR(i)   INODE_IS_TYPE(i, VFS_TYPE_CHAR_DEV)
#define INODE_IS_BLOCK(i)  INODE_IS_TYPE(i, VFS_TYPE_BLOCK_DEV)
#define INODE_IS_FS(i)     INODE_IS_TYPE(i, VFS_TYPE_FS_DEV)

#define INODE_GET_TYPE(i) ((i)->type)
#define INODE_SET_TYPE(i,t) \
    do \
      { \
        (i)->type = (t); \
      } \
    while(0)

#define INODE_SET_CHAR(i)  INODE_SET_TYPE(i, VFS_TYPE_CHAR_DEV)
#define INODE_SET_BLOCK(i) INODE_SET_TYPE(i, VFS_TYPE_BLOCK_DEV)
#define INODE_SET_FS(i)    INODE_SET_TYPE(i, VFS_TYPE_FS_DEV)

typedef const struct file_ops file_ops_t;
typedef const struct fs_ops   fs_ops_t;

union inode_ops_t {
    const file_ops_t *i_ops;  /* char driver operations */
    const fs_ops_t   *i_fops; /* FS operations */
};

/* this structure represents inode for driver and fs*/
typedef struct {
    union inode_ops_t ops;     /* inode operations */
    void             *i_arg;   /* per inode private data */
    char             *i_name;  /* name of inode */
    int               i_flags; /* flags for inode */
    uint8_t           type;    /* type for inode */
    uint8_t           refs;    /* refs for inode */
} inode_t;

typedef struct {
    inode_t    *node;   /* node for file */
    void       *f_arg;  /* f_arg for file */
    size_t      offset; /* offset for file */
} file_t;

struct pollfd;
typedef void (*poll_notify_t)(struct pollfd *fd, void *arg);
struct file_ops {
    int     (*open)  (inode_t *node, file_t *fp);
    int     (*close) (file_t *fp);
    ssize_t (*read)  (file_t *fp, void *buf, size_t nbytes);
    ssize_t (*write) (file_t *fp, const void *buf, size_t nbytes);
    int     (*ioctl) (file_t *fp, int cmd, unsigned long arg);
#ifdef AOS_CONFIG_VFS_POLL_SUPPORT
    int     (*poll)  (file_t *fp, bool flag, poll_notify_t notify, struct pollfd *fd, void *arg);
#endif
};

struct fs_ops {
    int             (*open)     (file_t *fp, const char *path, int flags);
    int             (*close)    (file_t *fp);
    ssize_t         (*read)     (file_t *fp, char *buf, size_t len);
    ssize_t         (*write)    (file_t *fp, const char *buf, size_t len);
    off_t           (*lseek)    (file_t *fp, off_t off, int whence);
    int             (*sync)     (file_t *fp);
    int             (*stat)     (file_t *fp, const char *path, struct stat *st);
    int             (*unlink)   (file_t *fp, const char *path);
    int             (*rename)   (file_t *fp, const char *oldpath, const char *newpath);
    aos_dir_t      *(*opendir)  (file_t *fp, const char *path);
    aos_dirent_t   *(*readdir)  (file_t *fp, aos_dir_t *dir);
    int             (*closedir) (file_t *fp, aos_dir_t *dir);
    int             (*mkdir)    (file_t *fp, const char *path);
    int             (*rmdir)    (file_t *fp, const char *path);
    void            (*rewinddir)(file_t *fp, aos_dir_t *dir);
    long            (*telldir)  (file_t *fp, aos_dir_t *dir);
    void            (*seekdir)  (file_t *fp, aos_dir_t *dir, long loc);
    int             (*ioctl)    (file_t *fp, int cmd, unsigned long arg);
    int             (*statfs)   (file_t *fp, const char *path, struct statfs *suf);
    int             (*access)   (file_t *fp, const char *path, int amode);
};

int     inode_init(void);
int     inode_alloc(void);
int     inode_del(inode_t *node);
inode_t *inode_open(const char *path);
int     inode_ptr_get(int fd, inode_t **node);
int     inode_avail_count(void);
void    inode_ref(inode_t *);
void    inode_unref(inode_t *);
int     inode_busy(inode_t *);
int     inode_reserve(const char *path, inode_t **inode);
int     inode_release(const char *path);

#ifdef __cplusplus
}
#endif

#endif /*VFS_INODE_H*/

