/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include "vfs_types.h"
#include "vfs_api.h"
#include "vfs_conf.h"

#include "vfs_inode.h"
#include "vfs_adapt.h"

static vfs_inode_t g_vfs_nodes[VFS_DEVICE_NODES];
#ifdef VFS_CONFIG_ROOTFS
static vfs_inode_t *g_rootfs_node;
#endif

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

static void close_all_files_on_node(vfs_inode_t *node)
{
    int idx;
    vfs_file_t *f;

    for (idx = 0; idx < VFS_MAX_FILE_NUM; idx++) {
        f = vfs_file_get(idx);
        if (f && f->node == node) {
            if (INODE_IS_FS(node)) {
                if (node->ops.i_fops->close) {
                    node->ops.i_fops->close(f);
                }
            } else {
                if (node->ops.i_ops->close) {
                    node->ops.i_ops->close(f);
                }
            }
        }
    }
}

int32_t vfs_inode_del(vfs_inode_t *node)
{
#if 0
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
#else
    if (vfs_lock(node->lock) != VFS_OK)
    {
        VFS_ERROR("%s failed to lock inode %p!\n\r", __func__, node);
        return VFS_ERR_LOCK;
    }

    if (node->refs > 0)
    {
        node->status = VFS_INODE_INVALID;
        // ensure to close all files at this point since
        // ops will become unavailable soon!!
        close_all_files_on_node(node);

        if (vfs_unlock(node->lock) != VFS_OK)
        {
            VFS_ERROR("%s failed to unlock inode %p!\n\r", __func__, node);
            return VFS_ERR_LOCK;
        }
    }
    else
    {
        if (node->i_name != NULL)
        {
            vfs_free(node->i_name);
        }

        node->i_name  = NULL;
        node->i_arg   = NULL;
        node->i_flags = 0;
        node->type    = VFS_TYPE_NOT_INIT;

        if (vfs_lock_free(node->lock) != VFS_OK)
        {
            VFS_ERROR("%s failed to free lock for node %p\n\r", __func__, node);
            return VFS_ERR_LOCK;
        }
    }

    return VFS_OK;
#endif
}

vfs_inode_t *vfs_inode_open(const char *path)
{
    int32_t      idx;
    vfs_inode_t *node;
#ifdef VFS_CONFIG_ROOTFS
    bool         fs_match = false;
#endif

    for (idx = 0; idx < VFS_DEVICE_NODES; idx++) {
        node = &g_vfs_nodes[idx];

        if (node->i_name == NULL) {
            continue;
        }

        if (INODE_IS_TYPE(node, VFS_TYPE_FS_DEV)) {
            if (strncmp(node->i_name, path, strlen(node->i_name)) == 0) {
#ifdef VFS_CONFIG_ROOTFS
                fs_match = true;
#endif
                if (*(path + strlen(node->i_name)) == '/') {
                    return node;
                }
            }
        }

        if (strcmp(node->i_name, path) == 0) {
            return node;
        }
    }

#ifdef VFS_CONFIG_ROOTFS
    if (fs_match) {
        return g_rootfs_node;
    }
#endif

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
    if (!node)
	{
		return;
	}

    if (vfs_lock(node->lock) != VFS_OK) {
        VFS_ERROR("%s failed to lock inode %p!\n\r", __func__, node);
        return;
    }

    node->refs++;

    if (vfs_unlock(node->lock) != VFS_OK) {
        VFS_ERROR("%s failed to unlock inode %p!\n\r", __func__, node);
        return;
    }
}

void vfs_inode_unref(vfs_inode_t *node)
{
    bool delete = false, detach = false;;

    if (!node) return;

    if (vfs_lock(node->lock) != VFS_OK) {
        VFS_ERROR("%s failed to lock inode %p!\n\r", __func__, node);
        return;
    }

    if (node->refs > 0) {
        node->refs--;
    }

    if (node->refs == 0) {
        if (node->status == VFS_INODE_INVALID) {
            delete = true;
        } else if (node->status == VFS_INODE_DETACHED) {
            detach = true;
        }
    }

    if (vfs_unlock(node->lock) != VFS_OK) {
        VFS_ERROR("%s failed to unlock inode %p!\n\r", __func__, node);
        return;
    }

    if (delete) {
        vfs_inode_del(node);
    } else if (detach) {
       // umount(node->i_name);
    }
}

int32_t vfs_inode_busy(vfs_inode_t *node)
{
    int32_t ret;

    if (!node) return VFS_ERR_INVAL;

    if (vfs_lock(node->lock) != VFS_OK) {
        VFS_ERROR("%s failed to lock inode %p!\n\r", __func__, node);
        return VFS_ERR_LOCK;
    }

    ret = node->refs > 0 ? 1 : 0;

    if (vfs_unlock(node->lock) != VFS_OK) {
        VFS_ERROR("%s failed to unlock inode %p!\n\r", __func__, node);
        return VFS_ERR_LOCK;
    }

    return ret;
}

int32_t vfs_inode_busy_by_name(const char *name)
{
    for (int i = 0; i < VFS_DEVICE_NODES; i++) {
        if (strcmp(g_vfs_nodes[i].i_name, name) == 0) {
            return vfs_inode_busy(&(g_vfs_nodes[i]));
        }
    }

    return 0;
}

int32_t vfs_inode_detach(vfs_inode_t *node)
{
    if (vfs_lock(node->lock) != VFS_OK) {
        VFS_ERROR("%s failed to lock inode %p!\n\r", __func__, node);
        return VFS_ERR_LOCK;
    }

    node->status = VFS_INODE_DETACHED;

    if (vfs_unlock(node->lock) != VFS_OK) {
        VFS_ERROR("%s failed to unlock inode %p!\n\r", __func__, node);
        return VFS_ERR_LOCK;
    }

    return VFS_OK;
}

/* only used by FS inode to umount itself after it ceases to be busy */
int32_t vfs_inode_detach_by_name(const char *name)
{
    int32_t ret = -1;

    for (int i = 0; i < VFS_DEVICE_NODES; i++) {
        if ((g_vfs_nodes[i].type == VFS_TYPE_FS_DEV) && (strcmp(g_vfs_nodes[i].i_name, name) == 0)) {
            ret = vfs_inode_detach(&(g_vfs_nodes[i]));
        }
    }

    return ret;
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

    node->lock = vfs_lock_create();
    if (node->lock == NULL) {
        VFS_ERROR("%s faile to create lock for inode %p\r\n", __func__, node);
        return VFS_ERR_LOCK;
    }

    ret = vfs_inode_set_name(path, &node);
    if (ret != VFS_OK) {
        return ret;
    }

    node->status = VFS_INODE_VALID;
    *p_node = node;

#ifdef VFS_CONFIG_ROOTFS
    /* for rootfs use */
    if (strcmp(path, "/") == 0) g_rootfs_node = node;
#endif

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

int32_t vfs_inode_list(vfs_list_type_t type)
{
    int32_t idx;

    for (idx = 0; idx < VFS_DEVICE_NODES; idx++) {
        if (VFS_LIST_TYPE_FS == type) {
            if (g_vfs_nodes[idx].type == VFS_TYPE_FS_DEV) {
                printf("%s\r\n", g_vfs_nodes[idx].i_name);
            }
        } else if (VFS_LIST_TYPE_DEVICE == type) {
            if (g_vfs_nodes[idx].type == VFS_TYPE_CHAR_DEV ||
                g_vfs_nodes[idx].type == VFS_TYPE_BLOCK_DEV) {
                printf("%s\r\n", g_vfs_nodes[idx].i_name);
            }
        }
    }

    return VFS_OK;
}

uint32_t vfs_get_match_dev_node(const char *name, char *match_name)
{
    int32_t  idx;
    uint32_t match_count = 0;
    int32_t  match_idx = 0;
    for (idx = 0; idx < VFS_DEVICE_NODES; idx++) {
        if (g_vfs_nodes[idx].type == VFS_TYPE_CHAR_DEV ||
            g_vfs_nodes[idx].type == VFS_TYPE_BLOCK_DEV) {
            if (name == NULL) {
                printf("%s  ", g_vfs_nodes[idx].i_name + strlen("/dev/"));
            } else if (!strncmp(name, g_vfs_nodes[idx].i_name + strlen("/dev/"), strlen(name))) {
                match_count++;

                if (1 == match_count) {
                    match_idx = idx;
                } else if (match_count == 2) {
                    printf("%s  %s",
                            g_vfs_nodes[match_idx].i_name + strlen("/dev/"),
                            g_vfs_nodes[idx].i_name + strlen("/dev/"));
                } else {
                    printf("  %s", g_vfs_nodes[idx].i_name + strlen("/dev/"));
                }
            }
        }
    }

    if (1 == match_count) {
        strncpy(match_name,
                g_vfs_nodes[match_idx].i_name + strlen("/dev/"),
                strlen(g_vfs_nodes[match_idx].i_name + strlen("/dev/")));
    }

    return match_count;
}

int vfs_inode_get_names(const char *path, char names[][64], uint32_t* size)
{
    uint32_t idx;
    uint32_t index = 0;
    uint32_t len = 0;

    if (path == NULL)
        return VFS_ERR_INVAL;

    for (idx = 0; idx < VFS_DEVICE_NODES; idx++) {
        if (g_vfs_nodes[idx].type == VFS_TYPE_FS_DEV &&
            strncmp(path, g_vfs_nodes[idx].i_name, strlen(path)) == 0) {
            memset(names[index], 0, 64);
            len = strlen(g_vfs_nodes[idx].i_name) + 1;
            if (len > 64) {
                strncpy(names[index], g_vfs_nodes[idx].i_name, 63);
                names[index][63] = '\0';
                index++;
            } else {
                strncpy(names[index++], g_vfs_nodes[idx].i_name, len);
            }
        }
    }
    *size = index;

    return VFS_OK;
}
