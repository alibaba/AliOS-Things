/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <string.h>
#include <vfs_conf.h>
#include <vfs_err.h>
#include <vfs_inode.h>
#include <aos/aos.h>

#define VFS_NULL_PARA_CHK(para)     do { if (!(para)) return -EINVAL; } while(0)

static inode_t g_vfs_dev_nodes[AOS_CONFIG_VFS_DEV_NODES];

int inode_init()
{
    memset(g_vfs_dev_nodes, 0, sizeof(inode_t) * AOS_CONFIG_VFS_DEV_NODES);
    return 0;
}

int inode_alloc()
{
    int e = 0;

    for (; e < AOS_CONFIG_VFS_DEV_NODES; e++) {
        if (g_vfs_dev_nodes[e].type == VFS_TYPE_NOT_INIT) {
            return e;
        }
    }

    return -ENOMEM;
}

int inode_del(inode_t *node)
{
    if (node->refs > 0) {
        return -EBUSY;
    }

    if (node->refs == 0) {
        if (node->i_name != NULL) {
            aos_free(node->i_name);
        }

        node->i_name = NULL;
        node->i_arg = NULL;
        node->i_flags = 0;
        node->type = VFS_TYPE_NOT_INIT;
    }

    return VFS_SUCCESS;
}

inode_t *inode_open(const char *path)
{
    int e = 0;
    inode_t *node;

    for (; e < AOS_CONFIG_VFS_DEV_NODES; e++) {
        node = &g_vfs_dev_nodes[e];
        if (node == NULL) {
            continue;
        }
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

int inode_ptr_get(int fd, inode_t **node)
{
    if (fd < 0 || fd >= AOS_CONFIG_VFS_DEV_NODES) {
        return -EINVAL;
    }

    *node = &g_vfs_dev_nodes[fd];

    return VFS_SUCCESS;
}

void inode_ref(inode_t *node)
{
    node->refs++;
}

void inode_unref(inode_t *node)
{
    if (node->refs > 0) {
        node->refs--;
    }
}

int inode_busy(inode_t *node)
{
    return node->refs > 0;
}

int inode_avail_count(void)
{
    int count = 0;
    int e = 0;

    for (; e < AOS_CONFIG_VFS_DEV_NODES; e++) {
        if (g_vfs_dev_nodes[count].type == VFS_TYPE_NOT_INIT) {
            count++;
        }
    }

    return count;
}

static int inode_set_name(const char *path, inode_t **inode)
{
    size_t len;
    void  *mem;

    len = strlen(path);
    mem = (void *)aos_malloc(len + 1);
    if (!mem) {
        return -ENOMEM;
    }

    memcpy(mem, (const void *)path, len);
    (*inode)->i_name = (char *)mem;
    (*inode)->i_name[len] = '\0';

    return VFS_SUCCESS;
}

int inode_reserve(const char *path, inode_t **inode)
{
    int ret;
    inode_t *node = NULL;

    VFS_NULL_PARA_CHK(path != NULL && inode != NULL);
    *inode = NULL;

    /* Handle paths that are interpreted as the root directory */
    if (path[0] == '\0' || path[0] != '/') {
        return -EINVAL;
    }

    ret = inode_alloc();
    if (ret < 0) {
        return ret;
    }

    inode_ptr_get(ret, &node);

    ret = inode_set_name(path, &node);
    if (ret < 0) {
        return ret;
    }

    *inode = node;
    return VFS_SUCCESS;
}

int inode_release(const char *path)
{
    int ret;
    inode_t *node;

    VFS_NULL_PARA_CHK(path != NULL);

    node = inode_open(path);
    if (node == NULL) {
        return -ENODEV;
    }

    ret = inode_del(node);
    if (ret < 0) {
        return ret;
    }

    return VFS_SUCCESS;
}

