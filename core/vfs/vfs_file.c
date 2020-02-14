/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stddef.h>
#include <stdint.h>

#include "vfs_types.h"
#include "vfs_conf.h"

#include "vfs_file.h"

static vfs_file_t g_files[VFS_MAX_FILE_NUM];
static uint32_t   g_opened_fd_bitmap[(VFS_MAX_FILE_NUM + 31) / 32];

extern void vfs_inode_ref(vfs_inode_t *node);
extern void vfs_inode_unref(vfs_inode_t *node);

int32_t vfs_fd_get(vfs_file_t *file)
{
    return (file - g_files) + VFS_FD_OFFSET;
}

vfs_file_t *vfs_file_get(int32_t fd)
{
    vfs_file_t *f;

    fd -= VFS_FD_OFFSET;

    if (fd < 0) {
        return NULL;
    }

    if (fd >= VFS_MAX_FILE_NUM) {
        return NULL;
    }

    f = &g_files[fd];
    return f->node ? f : NULL;
}

vfs_file_t *vfs_file_new(vfs_inode_t *node)
{
    int32_t     idx;
    vfs_file_t *f;

    for (idx = 0; idx < VFS_MAX_FILE_NUM; idx++) {
        f = &g_files[idx];

        if (f->node == NULL) {
            goto got_file;
        }
    }

    return NULL;

got_file:
    f->node   = node;
    f->f_arg  = NULL;
    f->offset = 0;
    vfs_inode_ref(node);

    return f;
}

void vfs_file_del(vfs_file_t *file)
{
    vfs_inode_unref(file->node);

    file->node = NULL;
}

int32_t vfs_fd_mark_open(int32_t fd)
{
    int word, bit;

    fd -= VFS_FD_OFFSET;

    /* invalid fd */
    if (fd < 0) {
        return -1;
    }

    word = fd / 32;
    bit = fd % 32;

    if (g_opened_fd_bitmap[word] & (1 << bit)) {
        /* fd has been opened */
        return 1;
    } else {
        g_opened_fd_bitmap[word] |= (1 << bit);
    }

    return 0;
}

int32_t vfs_fd_mark_close(int32_t fd)
{
    int word, bit;

    fd -= VFS_FD_OFFSET;

    /* invalid fd */
    if (fd < 0) {
        return -1;
    }

    word = fd / 32;
    bit = fd % 32;

    if (g_opened_fd_bitmap[word] & (1 << bit)) {
        g_opened_fd_bitmap[word] &= ~(1 << bit);
    } else {
        /* fd has been close */
        return 1;
    }

    return 0;
}

int32_t vfs_fd_is_open(int32_t fd)
{
    int word, bit;

    fd -= VFS_FD_OFFSET;

    /* invalid fd */
    if (fd < 0) {
        return -1;
    }

    word = fd / 32;
    bit = fd % 32;

    return g_opened_fd_bitmap[word] & (1 << bit);
}

