/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <string.h>
#include <vfs_conf.h>
#include <vfs_err.h>
#include <vfs_register.h>

extern kmutex_t g_vfs_mutex;

int aos_register_driver(const char *path, file_ops_t *ops, void *arg)
{
    inode_t *node = NULL;
    int err, ret;

    err = krhino_mutex_lock(&g_vfs_mutex, RHINO_WAIT_FOREVER);
    if (err != 0) {
        return err;
    }

    ret = inode_reserve(path, &node);
    if (ret == VFS_SUCCESS) {
        /* now populate it with char specific information */
        INODE_SET_CHAR(node);

        node->ops.i_ops = ops;
        node->i_arg     = arg;
    }

    /* step out critical area for type is allocated */
    err = krhino_mutex_unlock(&g_vfs_mutex);
    if (err != 0) {
        if (node->i_name != NULL) {
            krhino_mm_free(node->i_name);
        }

        memset(node, 0, sizeof(inode_t));
        return err;
    }

    return ret;
}

int aos_unregister_driver(const char *path)
{
    int err, ret;

    err = krhino_mutex_lock(&g_vfs_mutex, RHINO_WAIT_FOREVER);
    if (err != 0) {
        return err;
    }

    ret = inode_release(path);

    err = krhino_mutex_unlock(&g_vfs_mutex);
    if (err != 0) {
        return err;
    }

    return ret;
}

int aos_register_fs(const char *path, fs_ops_t *ops, void *arg)
{
    inode_t *node = NULL;
    int err, ret;

    err = krhino_mutex_lock(&g_vfs_mutex, RHINO_WAIT_FOREVER);
    if (err != 0) {
        return err;
    }

    ret = inode_reserve(path, &node);
    if (ret == VFS_SUCCESS) {
        INODE_SET_FS(node);

        node->ops.i_fops = ops;
        node->i_arg      = arg;
    }

    err = krhino_mutex_unlock(&g_vfs_mutex);
    if (err != 0) {
        if (node->i_name != NULL) {
            krhino_mm_free(node->i_name);
        }

        memset(node, 0, sizeof(inode_t));
        return err;
    }

    return ret;
}

int aos_unregister_fs(const char *path)
{
    int err, ret;
 
    err = krhino_mutex_lock(&g_vfs_mutex, RHINO_WAIT_FOREVER);
    if (err != 0) {
        return err;
    }

    ret = inode_release(path);

    err = krhino_mutex_unlock(&g_vfs_mutex);
    if (err != 0) {
        return err;
    }

    return ret;
}

