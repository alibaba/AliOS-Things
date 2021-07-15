/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
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

static vfs_file_t *vfs_file_get_helper(int32_t fd, int explore)
{
    int32_t rfd, real_fd, real_rfd;
    vfs_file_t *f;

    real_fd = fd - VFS_FD_OFFSET;

    if ((real_fd < 0) || (real_fd >= VFS_MAX_FILE_NUM) || (!vfs_fd_is_open(fd)))
    {
        return NULL;
    }

    f = &g_files[real_fd];

    if (!explore)
    {
        return f;
    }

    /* fd redirect logic */
    rfd = f->redirect_fd;
    real_rfd = rfd - VFS_FD_OFFSET;
    while (real_rfd >= 0) {
        if (real_rfd >= VFS_MAX_FILE_NUM || !vfs_fd_is_open(rfd))
        {
            return NULL;
        }
        else
        {
            f = &g_files[real_rfd];
            rfd = f->redirect_fd;
            real_rfd = rfd - VFS_FD_OFFSET;
        }
    }

    return f->node ? f : NULL;
}

vfs_file_t *vfs_file_get(int32_t fd)
{
    return vfs_file_get_helper(fd, 1);
}

vfs_file_t *vfs_file_get2(int32_t fd)
{
    return vfs_file_get_helper(fd, 0);
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

    printf("[vfs_warn]: Failed to open file, too many files open now in system!\r\n");
    return NULL;

got_file:
    f->redirect_fd = -1;
    f->node     = node;
    f->f_arg    = NULL;
    f->offset   = 0;
    /* do NOT really use node if it is for redirect fd (i.e. -1 as node) */
    if (node && node != (vfs_inode_t *)(-1))
	{
		vfs_inode_ref(node);
	}

    return f;
}

void vfs_file_del(vfs_file_t *file)
{
    /* do NOT really use node if it is for redirect fd (i.e. -1 as node) */
    if (file && file->node && file->node != (vfs_inode_t *)(-1)) {
        vfs_inode_unref(file->node);
    }

    file->node  = NULL;
    file->redirect_fd = -1;
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

    if (g_opened_fd_bitmap[word] & (1 << bit))
    {
        /* fd has been opened */
        return 1;
    }
    else
    {
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

    if (g_opened_fd_bitmap[word] & (1 << bit))
    {
        g_opened_fd_bitmap[word] &= ~(1 << bit);
    }
    else
    {
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

