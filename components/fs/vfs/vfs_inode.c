/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "fs/vfs_types.h"
#include "fs/vfs_api.h"
#include "fs/vfs_conf.h"

#include "vfs_inode.h"
#include "vfs_adapt.h"

static vfs_inode_t g_vfs_nodes[VFS_DEVICE_NODES];

static int32_t vfs_inode_set_name(const char *path, vfs_inode_t **p_node)
{
    int32_t  len;
    void    *mem;

    len = strlen(path);
    mem = (void *)vfs_malloc(len + 1);
    if (mem == NULL) {
        return VFS_ERR_NOMEM;
    }

    memcpy(mem, (const void *)path, len);

    (*p_node)->i_name      = (char *)mem;
    (*p_node)->i_name[len] = '\0';

    return VFS_OK;
}

int32_t vfs_inode_init(void)
{
    memset(g_vfs_nodes, 0, sizeof(vfs_inode_t) * VFS_DEVICE_NODES);

    return VFS_OK;
}

int32_t vfs_inode_alloc(void)
{
    int32_t idx;

    for (idx = 0; idx < VFS_DEVICE_NODES; idx++) {
        if (g_vfs_nodes[idx].type == VFS_TYPE_NOT_INIT) {
            return idx;
        }
    }

    return VFS_ERR_NOMEM;
}

int32_t vfs_inode_del(vfs_inode_t *node)
{
    if (node->refs > 0) {
        return VFS_ERR_BUSY;
    }

    if (node->refs == 0) {
        if (node->i_name != NULL) {
            vfs_free(node->i_name);
        }

        node->i_name  = NULL;
        node->i_arg   = NULL;
        node->i_flags = 0;
        node->type    = VFS_TYPE_NOT_INIT;
    }

    return VFS_OK;
}

vfs_inode_t *vfs_inode_open(const char *path)
{
    int32_t      idx;
    vfs_inode_t *node;

    for (idx = 0; idx < VFS_DEVICE_NODES; idx++) {
        node = &g_vfs_nodes[idx];

        if (node->i_name == NULL) {
            continue;
        }

        if (INODE_IS_TYPE(node, VFS_TYPE_FS_DEV)) {
            if ((strncmp(node->i_name, path, strlen(node->i_name)) == 0) && 
                (*(path + strlen(node->i_name)) == '/')) {
                return node;
            }
        }

        if (strcmp(node->i_name, path) == 0) {
            return node;
        }
    }

    return NULL;
}

int32_t vfs_inode_ptr_get(int32_t fd, vfs_inode_t **p_node)
{
    if (fd < 0 || fd >= VFS_DEVICE_NODES) {
        return VFS_ERR_INVAL;
    }

    *p_node = &g_vfs_nodes[fd];

    return VFS_OK;
}

int32_t vfs_inode_avail_count(void)
{
    int32_t idx, count = 0;

    for (idx = 0; idx < VFS_DEVICE_NODES; idx++){
        if (g_vfs_nodes[count].type == VFS_TYPE_NOT_INIT) {
            count++;
        }
    }

    return count;
}

void vfs_inode_ref(vfs_inode_t *node)
{
    node->refs++;
}

void vfs_inode_unref(vfs_inode_t *node)
{
    if (node->refs > 0) {
        node->refs--;
    }
}

int32_t vfs_inode_busy(vfs_inode_t *node)
{
    return (node->refs > 0);
}

int32_t vfs_inode_reserve(const char *path, vfs_inode_t **p_node)
{
    int32_t ret;

    vfs_inode_t *node = NULL;

    if ((path == NULL) || (p_node == NULL)) {
        return VFS_ERR_INVAL;
    }

    *p_node = NULL;

    /* Handle paths that are interpreted as the root directory */
#ifdef _WIN32
    if ((path[0] == '\0') || (path[1] != ':')) {
#else
    if ((path[0] == '\0') || (path[0] != '/')) {
#endif
        return VFS_ERR_INVAL;
    }

    ret = vfs_inode_alloc();
    if (ret == VFS_ERR_NOMEM) {
        return ret;
    }

    vfs_inode_ptr_get(ret, &node);
    if (node == NULL) {
        return VFS_ERR_NOMEM;
    }

    ret = vfs_inode_set_name(path, &node);
    if (ret != VFS_OK) {
        return ret;
    }

    *p_node = node;

    return VFS_OK;
}

int32_t vfs_inode_release(const char *path)
{
    int32_t ret;
    vfs_inode_t *node;

    if (path == NULL) {
        return VFS_ERR_INVAL;
    }

    node = vfs_inode_open(path);
    if (node == NULL) {
        return VFS_ERR_NODEV;
    }

    ret = vfs_inode_del(node);

    return ret;
}



