/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include "aos/vfs.h"
#include "device/aos_device.h"
#include "ulog/ulog.h"

#define TAG "VFS_ADAPTER"

static int _device_open(inode_t *node, file_t *fp)
{
    aos_device_t dev = NULL;

    DEV_ASSERT(fp != NULL);
    DEV_ASSERT(fp->node != NULL);
    DEV_ASSERT(fp->node->i_name != NULL);

    /* find device */
#ifdef AOS_DEVICE_BRIDGE_USE_VFS
    dev = aos_device_find(fp->node->i_name);
#else
    dev = fp->node->i_arg;
#endif
    if (NULL == dev) {
        return -1;
    }

    return aos_device_open(dev, fp->node->i_flags);
}

static int _device_close(file_t *fp)
{
    aos_device_t dev = NULL;

    DEV_ASSERT(fp != NULL);
    DEV_ASSERT(fp->node != NULL);
    DEV_ASSERT(fp->node->i_name != NULL);

    /* find device */
#ifdef AOS_DEVICE_BRIDGE_USE_VFS
    dev = aos_device_find(fp->node->i_name);
#else
    dev = fp->node->i_arg;
#endif
    if (NULL == dev) {
        return -1;
    }

    return aos_device_close(dev);
}

static int _device_read(file_t *fp, void *buf, size_t nbytes)
{
    int ret;
    aos_device_t dev = NULL;

    DEV_ASSERT(fp != NULL);
    DEV_ASSERT(buf != NULL);
    DEV_ASSERT(fp->node != NULL);
    DEV_ASSERT(fp->node->i_name != NULL);

    /* find device */
#ifdef AOS_DEVICE_BRIDGE_USE_VFS
    dev = aos_device_find(fp->node->i_name);
#else
    dev = fp->node->i_arg;
#endif
    if (NULL == dev) {
        return -1;
    }

    ret = aos_device_read(dev, fp->offset, buf, nbytes);
    if (ret >= 0) {
        fp->offset += ret;
    }

    return ret;
}

static ssize_t _device_write(file_t *fp, const void *buf, size_t nbytes)
{
    int ret;
    aos_device_t dev = NULL;

    DEV_ASSERT(fp != NULL);
    DEV_ASSERT(buf != NULL);
    DEV_ASSERT(fp->node != NULL);
    DEV_ASSERT(fp->node->i_name != NULL);

    /* find device */
#ifdef AOS_DEVICE_BRIDGE_USE_VFS
    dev = aos_device_find(fp->node->i_name);
#else
    dev = fp->node->i_arg;
#endif
    if (NULL == dev) {
        return -1;
    }

    ret = aos_device_write(dev,  fp->offset, buf, nbytes);
    if (ret >= 0) {
        fp->offset += ret;
    }

    return ret;
}

static int _device_ioctl(file_t *fp, int cmd, unsigned long arg)
{
    aos_device_t dev = NULL;

    DEV_ASSERT(fp != NULL);
    DEV_ASSERT(fp->node != NULL);
    DEV_ASSERT(fp->node->i_name != NULL);

    /* find device */
#ifdef AOS_DEVICE_BRIDGE_USE_VFS
    dev = aos_device_find(fp->node->i_name);
#else
    dev = fp->node->i_arg;
#endif
    if (NULL == dev) {
        return -1;
    }

    return aos_device_control(dev, cmd, (void *)arg);
}

static uint32_t _device_lseek(file_t *fp, int64_t off, int32_t whence)
{
    aos_device_t dev = NULL;

    DEV_ASSERT(fp != NULL);
    DEV_ASSERT(fp->node != NULL);
    DEV_ASSERT(fp->node->i_name != NULL);

    /* find device */
#ifdef AOS_DEVICE_BRIDGE_USE_VFS
    dev = aos_device_find(fp->node->i_name);
#else
    dev = fp->node->i_arg;
#endif
    if (NULL == dev) {
        return -1;
    }

    fp->offset = off;

    return 0;
}

struct file_ops m_dev_fops = {
    .open   = _device_open,
    .close  = _device_close,
    .read   = _device_read,
    .write  = _device_write,
    .ioctl  = _device_ioctl,
    .lseek =  _device_lseek,
 };
